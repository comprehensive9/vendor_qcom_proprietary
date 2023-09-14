/*==============================================================================
*  DutSocketServer.java
*
*  DESCRIPTION:
*       Provides a socket interface to receive and parse commands from native
*       sigma.
*
*  Copyright (c) 2020 Qualcomm Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Qualcomm Technologies, Inc.
*===============================================================================
*/

package com.qualcomm.wfd.client;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.nio.ByteBuffer;
import java.lang.Runtime;
import java.util.StringTokenizer;
import java.util.NoSuchElementException;

import java.net.ServerSocket;
import java.net.Socket;
import java.text.Normalizer;
import android.util.Log;

import android.os.RemoteException;

import com.qualcomm.wfd.WfdDevice;
import com.qualcomm.wfd.WfdEnums;
import com.qualcomm.wfd.WfdStatus;
import com.qualcomm.wfd.WfdEnums.WFDDeviceType;
import com.qualcomm.wfd.WfdEnums.ErrorType;

public class DutSocketServer implements Runnable
{
    private static final String TAG = "SIGMA.DutSocketServer";
    private int DUT_SOCKET_PORT = 9000;
    private ServerSocket _server;
    private Socket connection;
    private InputStream  inStream;
    private OutputStream outStream;
    private BufferedReader inReader;
    private volatile boolean _serverStopping;
    private WFDSigmaActivity sigmaActivity;

    public WfdDevice localWfdDev, peerWfdDev;

    public DutSocketServer(WFDSigmaActivity activity)
    {
        Log.d(TAG, "DutSocketServer Constructor");

        this.sigmaActivity = activity;
        initSocketServer();
    }

    public void initSocketServer() {
        Log.d(TAG, "Start socket server");
        try
        {
            _server = new ServerSocket(DUT_SOCKET_PORT);
        }
        catch (IOException e)
        {
            Log.d(TAG, "Exception on instantiate socket server");
            e.printStackTrace();
            _server = null;
        }

        if(_server == null)
        {
            Log.e(TAG, "Failed to create socket server");
            return;
        }

        _serverStopping = false;
        Log.d(TAG, "new socket server create success");
    }

    public void run()
    {
        Log.d(TAG, "Enter Socket Server Thread Run");
        if (_server == null)
        {
            Log.e(TAG, "The socket server was not instantiated");
            return;
        }
        while ( !_serverStopping)
        {
            try
            {
                if (connection == null)
                {
                    Log.d(TAG, "Accept inbound connection on socket server");
                    connection = _server.accept();
                    if(connection == null)
                    {
                        Log.e(TAG, "accept return null connection");
                        break;
                    }
                }
                inStream = connection.getInputStream();
                outStream = connection.getOutputStream();
                inReader = new BufferedReader(new InputStreamReader(inStream));
                if((inStream == null) || (outStream == null) || (inReader == null))
                {
                    Log.e(TAG, "Failed to created I/O streams");
                    break;
                }
                Log.d(TAG, "The socket server is connected");
                String line = inReader.readLine();
                Log.d(TAG,"SocketServer Received :  "+ line);
                if (line == null)
                {
                    //EOF - close the connection
                    Log.d(TAG, "EOF : closing socket connection");
                    connectionClose();
                }
                else {
                    //Get rid of unwanted/junk characters
                    line = line.replaceAll("\\p{Cc}", "");
                    line = line.replaceAll("[^\\p{ASCII}]", "");
                    line = line.replaceAll("[\u0000-\u002F]","");
                    Log.d(TAG,"Decoded String : "+ line);
                    Log.d(TAG,"Decoded String length : "+ line.length());

                    String resp = interpret(line);
                    if (!resp.equals("")) {
                        Log.d(TAG, "Response would be :: \"" + resp + "\"");
                        SendResponse( connection, resp);
                    }
                    else {
                        Log.d(TAG, "No response being sent");
                    }
                }
            }
            catch (IOException e)
            {
                Log.e(TAG, "IOException in connection instance runtime");
                e.printStackTrace();
                _serverStopping = true;
            }
        }
        Log.d(TAG, "Stopping server and exiting the thread run");
        StopServer();
    }

    public void connectionClose() throws IOException {
        Log.d(TAG, "Closing I/O stream, reader & connection");
        if (inStream != null ) inStream.close();
        if (outStream != null) outStream.close();
        if (inReader != null) inReader.close();
        inStream = null; outStream = null; inReader = null;
        if (connection != null) connection.close();
        connection = null;
    }

    public void StopServer(){
        Log.d(TAG, "Stopping server");
        _serverStopping = true;

        if(connection != null)
        {
            //close the connection
            try {
                connectionClose();
            } catch (IOException e) {
                Log.d(TAG, "Exception on closing connection" + e.getMessage());
                e.printStackTrace();
            }
        }

        if (_server != null)
        {
            try
            {
                Log.d(TAG, "Closing and Exiting the server");
                _server.close();
                _server = null;
            } catch (IOException e) {
                Log.d(TAG, "Exception closing the server");
                e.printStackTrace();
            }
        }
        Thread.currentThread().interrupt();
    }

    public void SendResponse(Socket connection, String data)
    {
         Log.d(TAG, "Enter SendResponse");
         if(connection != null &&  outStream !=null && data != null && data.isEmpty() == false)
         {
            Log.d(TAG, "Connection not null and data is ok");
            try
            {
                 Log.d(TAG, "Writing to output stream in SendResponse");
                 //format of outgoing responses should be
                 //"SC_CMD_NAME:return status code=[value]" -passed in as data

                 //write the newline character first, so that the response is printed in newline
                 char newline[] = {'\n'};
                 outStream.write(new String(newline).getBytes());
                 //write the actual data here
                 outStream.write(data.getBytes());
                 outStream.write(new String(newline).getBytes());
                 outStream.flush();
            }
            catch (IOException e)
            {
                Log.d(TAG, "exception while writing to stream in SendResponse");
                e.printStackTrace();
            }
        }
        Log.d(TAG, "Exit SendResponse successfully");
    }

    private enum CmdName
    {
        START_WFD_SESSION,
        TEARDOWN_RTSP, PAUSE_RTSP,  PLAY_RTSP, STANDBY_RTSP,
        SHOW_SURFACE, HIDE_SURFACE,
        START_UIBC, STOP_UIBC,
        SELECT_TCP, SELECT_UDP, SET_DECODER_LATENCY, FLUSH,
        ENABLE_DS, DISABLE_DS,
        NOVALUE;

        public static CmdName getValue(String str)
        {
            try
            {
                if (str != null)
                {
                    return valueOf(str);
                }
                else return NOVALUE;
            }
            catch (IllegalArgumentException e)
            {
                return NOVALUE;
            }
        }
    }

    //parse out cmdName and identify the command
    String interpret(String buffer)
    {
        Log.d(TAG, "interpreting buffer string");
        String cmdName = "";
        int i = 0;

        if((buffer == null) || (buffer.length() < 3) || (buffer.startsWith("SC_") != true)
            || (buffer.contains(":") != true) || (buffer.endsWith(";") != true))
        {
             Log.d(TAG, "Invalid Command/malformed expression");
             return "";
        }

        cmdName = buffer.substring((buffer.indexOf("SC_") + 3), buffer.indexOf(':'));

        Log.d(TAG, "Parsed Command name is: \"" + cmdName + "\"");

        String response = "SC_";
        int retValue = -10;
        switch(CmdName.getValue(cmdName))
        {
            case START_WFD_SESSION:
                retValue = sigma_startWfdSession(buffer);
                response = response.concat(cmdName);
                break;
            case TEARDOWN_RTSP:
                sigmaActivity.teardownRTSP();
                break;
            case PAUSE_RTSP:
                sigmaActivity.pauseRTSP();
                break;
            case PLAY_RTSP:
                sigmaActivity.playRTSP();
                break;
            case STANDBY_RTSP:
                retValue = sigmaActivity.standbyRTSP();
                response = response.concat(cmdName);
                break;
            case SHOW_SURFACE:
                sigmaActivity.showSinkSurface();
                break;
            case HIDE_SURFACE:
                sigmaActivity.hideSinkSurface();
                break;
            case START_UIBC:
                sigmaActivity.startUIBC();
                break;
            case STOP_UIBC:
                sigmaActivity.stopUIBC();
                break;
            case SELECT_TCP:
                sigma_selectTCP(buffer);
                break;
            case SELECT_UDP:
                sigmaActivity.selectUDP();
                break;
            case SET_DECODER_LATENCY:
                sigma_setDecoderLatency(buffer);
                break;
            case FLUSH:
                sigmaActivity.flush();
                break;
            case ENABLE_DS:
                sigmaActivity.toggleDirectStreaming(true);
                break;
            case DISABLE_DS:
                sigmaActivity.toggleDirectStreaming(false);
                break;
        }
        if (-10 != retValue)
        {
            response = response.concat(":return status code="+retValue+";");
            Log.d(TAG, "Found response to send from dutsocketserver, value is "+retValue);
            return response;
        }
        return "";
    }

    int sigma_startWfdSession(String buffer){
        Log.d(TAG, "Attempting to parse params for startWfdSession");
        String macVal = "", devVal = "", goIntentValue = "", initWfdVal = "", extSupport = "";

        StringTokenizer startWfdTokenizer = new StringTokenizer(buffer,"=;");

        while (startWfdTokenizer.hasMoreTokens()) {
            String token = startWfdTokenizer.nextToken();
            //Log.d(TAG, "Printing Token :" + token);
            try
            {
                if(token.contains("peermac"))
                {
                    Log.d(TAG, "Peer mac address found");
                    macVal = startWfdTokenizer.nextToken();
                }
                if(token.contains("deviceType"))
                {
                    Log.d(TAG, "Device type found");
                    devVal = startWfdTokenizer.nextToken();
                }
                if(token.contains("goIntentVal"))
                {
                    Log.d(TAG, "GO Intent value found");
                    goIntentValue = startWfdTokenizer.nextToken();
                }
                if(token.contains("initWfd"))
                {
                    Log.d(TAG, "Init Wfd value found");
                    initWfdVal = startWfdTokenizer.nextToken();
                }
                if(token.contains("miracast_version"))
                {
                    Log.d(TAG, "Miracast version found");
                    extSupport = startWfdTokenizer.nextToken();
                }
            }
            catch (NoSuchElementException e) {
                Log.e(TAG, "NoSuchElementException: Failed to get next token, missing one/more params");
                return ErrorType.INVALID_ARG.getCode();
            }
        }
        if (macVal.isEmpty() || devVal.isEmpty() || goIntentValue.isEmpty() ||
            initWfdVal.isEmpty() || extSupport.isEmpty())
        {
            Log.e(TAG, "Malformed command: failed to parse one/more params");
            return ErrorType.INVALID_ARG.getCode();
        }
        return sigmaActivity.startWfdSession(macVal, devVal, goIntentValue, initWfdVal, extSupport);
    }

   void sigma_selectTCP(String buffer){
        Log.d(TAG, "Attempting to parse params for selectTCP");
        String tcpBufferingDelay = "";
        StringTokenizer selectTcpTokenizer = new StringTokenizer(buffer,"=;");

        while (selectTcpTokenizer.hasMoreTokens()) {
            String token = selectTcpTokenizer.nextToken();
            //Log.d(TAG, "Printing Token :" + token);
            try
            {
                if(token.contains("tcpBufferingDelay"))
                {
                    Log.d(TAG, "TCP buffering delay found");
                    tcpBufferingDelay = selectTcpTokenizer.nextToken();
                }
            }
            catch (NoSuchElementException e) {
                Log.e(TAG, "NoSuchElementException: Failed to get next token, missing one/more params");
                return;
            }
        }
        if (tcpBufferingDelay.isEmpty())
        {
            Log.e(TAG, "Malformed command: failed to parse tcpBufferingDelay");
            return;
        }
        sigmaActivity.selectTCP(tcpBufferingDelay);
    }

    void sigma_setDecoderLatency(String buffer){
        Log.d(TAG, "Attempting to parse params for setDecoderLatency");
        String decoderLatency = "";
        StringTokenizer setDecoderLatencyTokenizer = new StringTokenizer(buffer,"=;");

        while (setDecoderLatencyTokenizer.hasMoreTokens()) {
            String token = setDecoderLatencyTokenizer.nextToken();
            //Log.d(TAG, "Printing Token :" + token);
            try
            {
                if(token.contains("decoderLatency"))
                {
                    Log.d(TAG, "Decoder Latency found");
                    decoderLatency = setDecoderLatencyTokenizer.nextToken();
                }
            }
            catch (NoSuchElementException e) {
                Log.e(TAG, "NoSuchElementException: Failed to get next token, missing one/more params");
                return;
            }
        }
        if (decoderLatency.isEmpty())
        {
            Log.e(TAG, "Malformed command: failed to parse decoderLatency");
            return;
        }
        sigmaActivity.setDecoderLatency(decoderLatency);
    }
}

