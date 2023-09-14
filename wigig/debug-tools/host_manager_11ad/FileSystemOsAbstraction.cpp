/*
* Copyright (c) 2018-2020 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*
* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*       contributors may be used to endorse or promote products derived
*       from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
* IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "DebugLogger.h"
#include "FileSystemOsAbstraction.h"

#ifdef __linux
    #include <stdlib.h>
    #include <cstring>
    #include <cerrno>
    #include <sys/stat.h> //added for config- template path
    #include <linux/limits.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <array>
    #include <iostream>
#elif _WINDOWS
    #include <windows.h>
    #include <ShlObj.h>

    #pragma comment(lib, "comsuppw")
#endif

#include <fstream>
#include <sstream>

using namespace std;


bool FileSystemOsAbstraction::ReadFile(const std::string& fileName, string& data)
{
    ifstream fd(fileName.c_str());
    if (!fd.good())
    {
        // Probably file doesn't exist
        LOG_WARNING << "Failed to open file: " << fileName  << std::endl;
        data = "";
        return false;
    }

    fd.open(fileName.c_str());
    stringstream content;
    content << fd.rdbuf();
    fd.close();
    data = content.str();
    return true;
}

bool FileSystemOsAbstraction::WriteFile(const string& fileName, const string& content)
{
    std::ofstream fd(fileName.c_str());
    if (!fd.is_open())
    {
        LOG_WARNING << "Failed to open file: " << fileName << std::endl;
        return false;
    }
    fd << content;
    if (fd.bad())
    {
        LOG_WARNING << "Failed to write to file: " << fileName << std::endl;
        fd.close();
        return false;
    }
    fd.close();
    return true;
}

bool FileSystemOsAbstraction::AppendToFile(const string& fileName, const string& content) // TODO - add a flag to WriteFile and remove this function (differ only in constructor)
{
    std::ofstream fd(fileName.c_str(), std::ios_base::app);
    if (!fd.is_open())
    {
        LOG_WARNING << "Failed to open file: " << fileName << std::endl;
        return false;
    }
    fd << content;
    if (fd.bad())
    {
        LOG_WARNING << "Failed to append to file: " << fileName << std::endl;
        fd.close();
        return false;
    }
    fd.close();
    return true;
}

string FileSystemOsAbstraction::GetConfigurationFilesLocation()
{
    stringstream path;

    //should check __ANDROID__ first since __LINUX flag also set in Android
#ifdef __ANDROID__
    std::string t_path = "/data/vendor/wifi/host_manager_11ad/";
    if (!FileSystemOsAbstraction::DoesFileExist(t_path))
    {
        path << "/data/host_manager_11ad/";
    }
    else
    {
        path << t_path;
    }
#elif __linux
    return "/etc/host_manager_11ad/";
#elif _WINDOWS //windows
    LPWSTR lpwstrPath = NULL;
    // Get the ProgramData folder path of windows
    HRESULT result = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, NULL, &lpwstrPath);
    if (SUCCEEDED(result))
    {
        // Convert the path to string
        std::wstring wpath(lpwstrPath);
        std::string strPath = std::string(wpath.cbegin(), wpath.cend());
        CoTaskMemFree(lpwstrPath);
        path << strPath << "\\Wilocity\\host_manager_11ad\\";
    }
#else //OS3
    return "/etc/host_manager_11ad/";
#endif // __linux
    return path.str();
}

std::string FileSystemOsAbstraction::GetTemplateFilesLocation()
{
    stringstream path;

    //should check __ANDROID__ first since __LINUX flag also set in Android


#ifdef  __ANDROID__
    std::string t_path = "/vendor/etc/wifi/host_manager_11ad/";
    if (!FileSystemOsAbstraction::DoesFileExist(t_path))
    {
        path << "/data/host_manager_11ad/";
    }
    else
    {
        path << t_path;
    }

#elif __linux
    return "/etc/host_manager_11ad/";
#elif _WINDOWS
    path << ".\\OnTargetUI\\";
#else //OS3
    return "/etc/host_manager_11ad/";
#endif // __linux
    return path.str();
}

string FileSystemOsAbstraction::GetOutputFilesLocation()
{
#ifdef __ANDROID__
    return "/data/host_manager_11ad_data/";
#elif __linux
    return "/var/host_manager_11ad_data/";
#elif _WINDOWS //windows
    return "C:\\Qualcomm\\11adTools\\host_manager\\output_files\\";
#else //OS3
    return "/etc/host_manager_11ad_data/";
#endif // __linux
}

string FileSystemOsAbstraction::GetDefaultLogConversionFilesLocation()
{
#ifdef __ANDROID__
    return "/data/host_manager_11ad_data/";
#elif __linux
    return "/lib/firmware/";
#elif _WINDOWS //windows
    return "C:\\Qualcomm\\11adTools\\host_manager\\conversion_files\\";
#else //OS3
    return "/etc/host_manager_11ad_data/";
#endif // __linux
}

string FileSystemOsAbstraction::GetDefaultPmcFilesLocation()
{
#ifdef __ANDROID__
    return "/data/pmc/";
#else
    return "/var/pmc/";
#endif
}

const char* FileSystemOsAbstraction::GetDirectoryDelimiter()
{
#ifndef _WINDOWS
    return "/";
#else
    return "\\";
#endif
}

bool FileSystemOsAbstraction::DoesFolderExist(const std::string& path)
{
#ifndef _WINDOWS
    DIR* pDir = opendir(path.c_str());
    if (pDir != NULL)
    {
        (void)closedir(pDir);
        return true;
    }
    return false;
#else
    const uint32_t fileAttributes = GetFileAttributesA(path.c_str());
    if (INVALID_FILE_ATTRIBUTES == fileAttributes) // no such path
    {
        return false;
    }
    if (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;   // given path is a directory
    }
    return false;    // given path isn't a directory
#endif
}

bool FileSystemOsAbstraction::DoesFileExist(const std::string& name)
{
#ifdef _WINDOWS
    struct _stat buf = {0};
    int Result = _stat(name.c_str(), &buf);
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
    struct stat buf = {};
#pragma GCC diagnostic pop
    int Result = stat(name.c_str(), &buf);
#endif
    return Result == 0;
}

OperationStatus FileSystemOsAbstraction::RemoveFile(const std::string& path)
{
#ifdef _WINDOWS
    const BOOL status = DeleteFileA(path.c_str());
    if (!status)
    {
        return OperationStatus(false, "Cannot delete file. Path: " + path + " Error: " + to_string(GetLastError()) + "\n");
    }
#else
    int status = unlink(path.c_str());
    if (0 != status)
    {
        return OperationStatus(false, "Cannot delete file. Path: " + path + " Error: " + strerror(errno) + "\n");
    }
#endif

    return OperationStatus(true);
}

bool FileSystemOsAbstraction::ReadLink(const std::string& path, std::string& result)
{
#ifdef _WINDOWS
    (void)path;
    (void)result;
    return false;
#else
    static std::array<char, PATH_MAX> readLinkBuffer;

    int count = readlink(path.c_str(), readLinkBuffer.data(), readLinkBuffer.size());

    if (count < 0)
    {
        LOG_ERROR << __FUNCTION__ << ": Failed to read link '" << path << "'. Error: " << strerror(errno) << endl;
        return false;
    }

    if (static_cast<size_t>(count) < readLinkBuffer.size())
    {
        result.assign(readLinkBuffer.data(), count);
        return true;
    }

    // otherwise, buffer was truncated, not enough space
    LOG_ERROR << __FUNCTION__ << ": Failed to read link '" << path << "', exceeded maximum allowed buffer size of " << PATH_MAX << " Bytes" << endl;
    return false;
#endif
}

OperationStatus FileSystemOsAbstraction::CreateFolder(const std::string& path)
{
    if (DoesFolderExist(path))
    {
        return OperationStatus(true, "Directory already exist: " + path + "\n");
    }
#ifndef _WINDOWS
    int status = 0;
    for (std::string::size_type i = 1; i < path.size(); i++) // starting from pos #1 - we do not want to start from the home path "/"
    {
        if (path[i] == '/')
        {
            const std::string partialPath = path.substr(0, i);
            status = mkdir(partialPath.c_str(), S_IRWXU);
            if (0 != status && -1 != status) // -1 File exists is not an error
            {
                return OperationStatus(false, "Failed to create directory at: " + partialPath + ", error: " + strerror(errno) + "\n");
            }
        }
    }
    status = mkdir(path.c_str(), S_IRWXU);
    if (0 != status && -1 != status) // -1 File exists is not an error
    {
        return OperationStatus(false, "Failed to create directory at: " + path + ", error: " + strerror(errno) + "\n");
    }
    return OperationStatus(true, "Created directory at: " + path + "\n");
#else
    if (path.empty())
    {
        return OperationStatus(false);
    }
    std::wstring wpath = std::wstring(path.cbegin(), path.cend());

    bool ok = true;
    size_t pos = 0;
    do
    {
        pos = wpath.find_first_of(L"\\/", pos + 1);
        ok &= CreateDirectory(wpath.substr(0, pos).c_str(), NULL) == TRUE || GetLastError() == ERROR_ALREADY_EXISTS;
    } while (ok && pos != std::string::npos);
    return OperationStatus(ok);
#endif
}

bool FileSystemOsAbstraction::MoveFileToNewLocation(const std::string& oldFileLocation, const std::string& newFileLocation)
{
#ifndef _WINDOWS
    system(("mv " + oldFileLocation + " " + newFileLocation).c_str());
    return true;
#else
    std::wstring wOldPath = std::wstring(oldFileLocation.cbegin(), oldFileLocation.cend());
    std::wstring wNewPath = std::wstring(newFileLocation.cbegin(), newFileLocation.cend());
    return MoveFile(wOldPath.c_str(), wNewPath.c_str()) == TRUE;
#endif
}

bool FileSystemOsAbstraction::PatternMatch(const std::string& value, const std::string& prefix, const std::string& suffix)
{
    // note: string.rfind with pos=0 argument will search for match at first position only
    return value.rfind(prefix, 0) == 0 // starts with
        && value.size() >= suffix.size()
        && value.compare(value.size() - suffix.size(), suffix.size(), suffix) == 0; // ends with
}

OperationStatus FileSystemOsAbstraction::GetFilesByPattern(const std::string& directory,
    const std::string& prefix, const std::string& suffix, std::vector<std::string>& filesVector)
{
#ifdef _WINDOWS
    WIN32_FIND_DATAA  findFileData;
    const string filesFolder = directory + "*.*";
    const HANDLE fResults = FindFirstFileA(filesFolder.c_str(), &findFileData);
    if (fResults == INVALID_HANDLE_VALUE)
    {
        return OperationStatus(false, "Failed to find first file: " + to_string(GetLastError()));
    }
    do
    {
        string filename(findFileData.cFileName);
        if (PatternMatch(filename, prefix, suffix))
        {
            filesVector.push_back(directory + filename);
        }
    } while (FindNextFileA(fResults, &findFileData) != 0);
    FindClose(fResults);
#else
    DIR * dirp = opendir(directory.c_str());
    if (!dirp)
    {
        return OperationStatus(false, "Failed to open dir: " + string(strerror(errno)));
    }
    dirent * dp;
    while ((dp = readdir(dirp)) != nullptr)
    {
        string filename(dp->d_name);
        if (PatternMatch(filename, prefix, suffix))
        {
            filesVector.push_back(directory + filename);
        }
    }
    closedir(dirp);
#endif

    return OperationStatus(true);
}

std::string FileSystemOsAbstraction::ExtractFileName(const std::string& fullPath, bool removeExtension /*= false*/)
{
    if (fullPath.empty() || fullPath.back() == *GetDirectoryDelimiter())
    {
        return "";
    }

    std::string filename;
    const auto lastDelimiterPos = fullPath.find_last_of(GetDirectoryDelimiter());
    if (lastDelimiterPos != std::string::npos)
    {
        filename = fullPath.substr(lastDelimiterPos + 1);
    }

    if (removeExtension)
    {
        const auto periodPos = filename.rfind('.');
        if (periodPos != std::string::npos)
        {
            filename.erase(periodPos);
        }
    }

    return filename;
}
