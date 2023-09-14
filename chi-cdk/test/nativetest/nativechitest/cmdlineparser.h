//=================================================================================================
// Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//=================================================================================================

//=================================================================================================
// @file  cmdlineparser.h
// @brief Header file for NATIVECHI3 command line parser
//=================================================================================================

#ifndef __NATIVETEST_CMDPARSER__
#define __NATIVETEST_CMDPARSER__

#include <stdint.h>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include "nativetestlog.h"
#include "json.hpp"
#include "jsonparser.h"

#define MAX_PATH        260
const UINT32 DEFAULT_FRAME_COUNT    = 30;
const UINT32 DEFAULT_SNAPS_COUNT    = 1;
const UINT32 DEFAULT_EXPOSURE_COUNT = 3;   // Default number of exposures for HDR test

///Buffer timeout (in milliseconds) is obtained by multiplying this by TIMEOUT_RETRIES
///Timeout waiting for buffer on device
const UINT64 BUFFER_TIMEOUT_DEVICE_MILLI = 1000;    // in milliseconds (1 sec)
const UINT64 BUFFER_TIMEOUT_DEVICE_SEC   = 1;       // in seconds

//=================================================================================================
// Class Definition
//=================================================================================================
class CmdLineParser
{
    public:

         CmdLineParser() = default;
        ~CmdLineParser() = default;

        /// Do not allow the copy constructor or assignment operator
        CmdLineParser(const CmdLineParser&) = delete;
        CmdLineParser& operator= (const CmdLineParser&) = delete;

        static int  ParseCommandLine(int argc, char** argv);
        static void PrintCommandLineUsage();
        static void resetDefaultFlags();

        /// @brief SwitchPort enums
        typedef enum SwitchPort
        {
            InvalidSwitch = -1,
            IfeFulltoDisp = 0,
            IfeDisptoFull,
            IpeDisptoVideo,
            IpeVideotoDisp,
        }switchport_t;


        ////////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////
        ///  GetSwitchPortType()
        ///
        /// @brief  returns int value depending on switch flag selected
        ///
        /// @return switchport_t
        /// IfeFulltoDisp  - flag to switch IFEOutputPortFull to IFEOutputPortDisplayFull
        /// IfeDisptoFull  - flag to switch IFEOutputPortDisplayFull to IFEOutputPortFull
        /// IpeVideotoDisp - flag to switch IPEOutputPortVideo to IPEOutputPortDisplay
        /// IpeDisptoVideo - flag to switch IPEOutputPortDisplay to IPEOutputPortVideo
        ///
        ////////////////////////////////////////////////////////////////////////////////////////////
        ///////////////////////////////
        static switchport_t GetSwitchPortType()
        {
            return m_switchPortType;
        }

        static int GetLogLevel()
        {
            return m_nLog;
        }

        static UINT32 GetFrameCount()
        {
            return (m_nFrames > 0 && m_nFrames < UINT32_MAX) ? m_nFrames : DEFAULT_FRAME_COUNT;
        }

        static bool PrintHelp()
        {
            return m_bHelp;
        }

        static int GetCamera()
        {
            return m_nCamera;
        }

        static std::string GetTestSuiteName()
        {
            return m_suiteName;
        }
        static std::string GetTestCaseName()
        {
            return m_caseName;
        }
        static std::string GetTestFullName()
        {
            return m_fullName;
        }

        static bool NoImageDump()
        {
            return m_bNoImageDump;
        }

        static char const* GetInputImage()
        {
            return m_inputImage;
        }

        static UINT32 GetInputWidth()
        {
            return m_inputWidth;
        }

        static UINT32 GetInputHeight()
        {
            return m_inputHeight;
        }

        static int GetInputFormat()
        {
            return m_inputFormat;
        }

        // Defaults to -1 if user did not provide overriding output port
        static int GetInputPort()
        {
            return m_inputPort;
        }

        static UINT32 GetRdiOutputWidth()
        {
            return m_rdiOutputWidth;
        }

        static UINT32 GetRdiOutputHeight()
        {
            return m_rdiOutputHeight;
        }

        /* Applies to preview output stream */
        static int GetOutputFormat()
        {
            return m_outputFormat;
        }
        static UINT32 GetOutputWidth()
        {
            return m_outputWidth;
        }
        static UINT32 GetOutputHeight()
        {
            return m_outputHeight;
        }

        /* Applies to output Snapshot streams */
        static int GetOutFormatSnapshot()
        {
            return m_snapshotFormat;
        }
        static UINT32 GetSnapshotWidth()
        {
            return m_snapshotWidth;
        }
        static UINT32 GetSnapshotHeight()
        {
            return m_snapshotHeight;
        }

        /* Applies to output Video streams */
        static int GetOutFormatVideo()
        {
            return m_videoFormat;
        }
        static UINT32 GetVideoWidth()
        {
            return m_videoWidth;
        }
        static UINT32 GetVideoHeight()
        {
            return m_videoHeight;
        }

        // Defaults to -1 if user did not provide overriding output port
        static int GetOutputPort()
        {
            return m_outputPort;
        }

        // Returns...
        //   < 0        disable timeout
        //   otherwise  timeout override in milliseconds
        static int GetTimeoutOverride()
        {
            return m_timeout;
        }

        static bool isTestGenMode()
        {
            return m_isTestGenMode;
        }

        static int GetSensorMode()
        {
            return m_sensorMode;
        }

        // Defaults to -1 if user did not provide overriding HW FD Type
        static int GetHwFdType()
        {
            return m_hwFdType;
        }

        // Returns the name of the FD test image for IOU calculation
        static std::string GetFDTestImage()
        {
            return m_fdTestImage;
        }

        static int GetFlushDelay()
        {
            return m_flushDelay;
        }

        static int GetFlushPipelineId()
        {
            return m_flushPipelineId;
        }

        static UINT32 GetFlushFrame()
        {
            return m_flushFrame;
        }

        static UINT32 GetEarlyPCRCount()
        {
            return m_numEarlyPCRs;
        }

        static bool IsLatencyCheckEnabled()
        {
            return m_enableLatencyCheck;
        }

        static bool RemoveSensor()
        {
            return m_bRemoveSensor;
        }

        //todo: move this enum to a common place for entire project
        // Enum list tracking capabilities for sensor and testgen in chi.h/CHISENSORMODECAPS and chi.h/CHITESTGENCAPS
        enum SensorCaps
        {
            NORMAL = 0,
            HFR,
            PDAF,
            QCFA,
            DEPTH,
            FS,
            INSENSORZOOM,
            INTERNAL,
            IHDR,  //1-exposure
            IHDR2, //2-exposure
            IHDR3, //3-exposure
            QHDR,
            QHDR2,
            QHDR3,
            ZZHDR,
            SHDR,
            SHDR2,
            SHDR3,
            MaxSensorCaps
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetSensorCapEnum
        ///
        /// @brief  Return sensor caps enum based on user input
        /// @param  std::string
        /// @return SensorCaps
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static SensorCaps GetSensorCapEnum(std::string userInput)
        {
            // convert input to lowercase
            std::transform(userInput.begin(), userInput.end(), userInput.begin(),[](unsigned char c)
            {
                return tolower(c);
            });

            // return a matching enum and set appropriate exposure count
            if (userInput == "ihdr")
            {
                m_nExposures = 1;
                return IHDR;
            }
            if (userInput == "ihdr2")
            {
                m_nExposures = 2;
                return IHDR2;
            }
            if (userInput == "ihdr3")
            {
                m_nExposures = 3;
                return IHDR3;
            }
            if (userInput == "qhdr")
            {
                m_nExposures = 1;
                return QHDR;
            }
            if (userInput == "qhdr2")
            {
                m_nExposures = 2;
                return QHDR2;
            }
            if (userInput == "qhdr3")
            {
                m_nExposures = 3;
                return QHDR3;
            }
            if (userInput == "zzhdr")  return ZZHDR;
            if (userInput == "shdr")
            {
                m_nExposures = 1;
                return SHDR;
            }
            if (userInput == "shdr2")
            {
                m_nExposures = 2;
                return SHDR2;
            }
            if (userInput == "shdr3")
            {
                m_nExposures = 3;
                return SHDR3;
            }
            return MaxSensorCaps;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetSensorCapsList
        ///
        /// @brief  Return the list of sensor capabilities requested by user for test
        /// @param  None
        /// @return std::vector<SensorCaps>
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static std::vector<SensorCaps> GetSensorCapsList()
        {
            return m_sensorCapsList;
        }

        static UINT32 GetDisplayPortOrDualFoVWidth()
        {
            return m_dfw;
        }

        static UINT32 GetDisplayPortOrDualFoVHeight()
        {
            return m_dfh;
        }

        static int GetDualDisplayOrDualFoVFormat()
        {
            return m_dff;
        }

        // Returns the name of the JSON file
        static std::string GetJsonFileName()
        {
            return m_testcaseOrJson;
        }

        static float GetZoomFactor()
        {
            return m_zoom;
        }

        static UINT32 GetSnapCount()
        {
            // todo: use NT_LOG to print warning for less than requested snaps
            return (m_nSnaps > 0 && m_nSnaps < m_nFrames) ? m_nSnaps : DEFAULT_SNAPS_COUNT;
        }

        // Get the snapshot frame numbers based on number of snaps requested
        static std::set<int> GetSnapFrameNumbers()
        {
            std::set<int> linear_spaced_frames;

            float interval = float(GetFrameCount()) / float((GetSnapCount() +1));
            if(interval == 0.0)
            {
                interval = 1.0;
            }

            for(UINT32 snap = 1; snap <= GetSnapCount(); snap++)
            {
                linear_spaced_frames.insert(static_cast<int>(ceil(interval*snap)));
            }

            return linear_spaced_frames;
        }

        static bool IsZSLEnabled()
        {
            return m_bZslMode;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// GetNumExposures
        ///
        /// @brief  Return the number of exposures set by user for HDR test
        /// @param  None
        /// @return UINT32
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        static UINT32 GetNumExposures()
        {
            return m_nExposures;
        }

        static int            m_nLog;
        static bool           m_bHelp;
        static bool           m_bReport;
        static bool           m_bPrint;
        static UINT32         m_nFrames;
        static bool           m_bNoImageDump;
        static int            m_nCamera;
        static char           m_camList[];
        static int            m_timeout;
        static int            m_sensorMode;
        static std::vector<SensorCaps> m_sensorCapsList;
        static int            m_hwFdType;
        static bool           m_isTestGenMode;
        static std::string    m_fdTestImage;
        static UINT32         m_numEarlyPCRs;
        static bool           m_enableLatencyCheck;
        static std::string    m_testcaseOrJson;
        static float          m_zoom;
        static UINT32         m_nSnaps;
        static bool           m_bZslMode;
        static std::string    m_suiteName;
        static std::string    m_caseName;
        static std::string    m_fullName;
        static bool           m_bRemoveSensor;
        static UINT32         m_nExposures; // Number of exposures in an HDR test

        // input parameters
        static char const*    m_inputImage;
        static UINT32         m_inputWidth;
        static UINT32         m_inputHeight;
        static int            m_inputFormat;
        static int            m_inputPort;

        // output parameters
        // Preview stream
        static int             m_outputFormat;     // Preview stream format
        static int             m_outputPort;
        static UINT32          m_outputWidth;      // Preview stream width
        static UINT32          m_outputHeight;     // Preview stream height

        static switchport_t    m_switchPortType;   //Switch IPE or IFE output port

        // Snapshot stream
        static int           m_snapshotFormat;   // Snapshot stream format
        static UINT32        m_snapshotWidth;    // Snapshot stream width
        static UINT32        m_snapshotHeight;   // Snapshot stream height

        // Video stream
        static int           m_videoFormat;      // Video stream format
        static UINT32        m_videoWidth;       // Video stream width
        static UINT32        m_videoHeight;      // Video stream height

        static UINT32        m_rdiOutputWidth;
        static UINT32        m_rdiOutputHeight;

        // Dual FOV (or) Display full Parameters (port cannot be modified)
        static int           m_dff;              // Display full/dual fov stream format
        static UINT32        m_dfw;              // Display full/dual fov stream width
        static UINT32        m_dfh;              // Display full/dual fov stream height

        // flush related runtime parameters
        static int            m_flushDelay;       // delay before issuing flush
        static UINT32         m_flushFrame;       // frame to issue flush at
        static int            m_flushPipelineId;  // pipeline ID to flush in case of pipeline flush
};
#endif //__NATIVETEST_CMDPARSER__
