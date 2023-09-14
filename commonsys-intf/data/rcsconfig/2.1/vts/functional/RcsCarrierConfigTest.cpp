/*****************************************************************************
Copyright (c) 2020 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
*****************************************************************************/
#include "RcsCarrierConfigTest.h"


std::string RcsCarrierConfigTest::compressFileData(bool compressData)
{
  std::ifstream file(g_configValues[xmlFileName_key].c_str(),
                    (std::ifstream::binary | std::ifstream::ate));
  // get pointer to associated buffer object
  std::filebuf* pbuf = file.rdbuf();

  // get file size using buffer's members
  std::size_t size = pbuf->pubseekoff (0,file.end,file.in);
  ALOGI("original file size %d", size);
  // rewind buffer pointer to the beginning to read config file
  pbuf->pubseekoff(0, file.beg, file.in);

  char* buffer = new char[size];
  int numCopied = pbuf->sgetn (buffer,size);
  ALOGI("Copied %d characters", numCopied);
  //std::cout.write (buffer,size);
  file.close();
  if(compressData == false)
  {
    ALOGI("not compressing");
    return buffer;
  }

  //logic for string compression

  z_stream c_stream = {0};
  inflateInit2(&c_stream, GZIP_WINDOWSIZE + 16);

  if(deflateInit2(&c_stream, Z_BEST_COMPRESSION, Z_DEFLATED,
                  GZIP_WINDOWSIZE + 16,
                  GZIP_CFACTOR,
                  Z_DEFAULT_STRATEGY) != Z_OK)
  {
    std::cout << "failed while compressing,deflateInit2 failed.";
    return buffer;
  }

  c_stream.next_in = (Bytef*)buffer;
  c_stream.avail_in = size;

  char outputBuffer[GZIP_OUTPUTBUFFSIZE];
  std::string resOutputString;
  int err;

  do
  {
    c_stream.next_out = reinterpret_cast<Bytef*>(outputBuffer);
    c_stream.avail_out = sizeof(GZIP_OUTPUTBUFFSIZE);
    err = deflate(&c_stream, Z_FINISH);
    if (resOutputString.size() < c_stream.total_out)
    {
      resOutputString.append(outputBuffer,c_stream.total_out - resOutputString.size());
    }
  } while (err == Z_OK);

  deflateEnd(&c_stream);

  if (err != Z_STREAM_END)
  {
    std::cout << "error in compression: (" << err << ") " << c_stream.msg;
  }
  //std::cout << "\n\n gzip: -[" << resOutputString << "]\n";
  return resOutputString;
}

TEST_F(RcsCarrierConfigTest, SetConfigTest) {
  Container data;
  string buffer;
  bool compressData = false;
  if(g_configValues[isCompressed_key] == "1")
  {
    cout << "Compressing the xml data";
    compressData = true;
  }
  buffer = compressFileData(compressData);
  data.isCompressed = compressData;
  data.config.resize(buffer.size());
  ALOGI("send_buffer size %d", buffer.size());
  data.config.setToExternal(((uint8_t*)buffer.data()), buffer.size());
  auto result = pService->setConfig(data, mListener);
  // need to check what result is of type.
  //EXPECT_TRUE(result, )
  auto res = mListener->WaitForCallback(kCallbacksetConfigResponse);
  EXPECT_TRUE(res.no_timeout);
  EXPECT_EQ(RequestStatus::OK, res.args->status);
}