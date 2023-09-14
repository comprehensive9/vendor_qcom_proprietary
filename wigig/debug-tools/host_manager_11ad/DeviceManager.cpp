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

#include <chrono>
#include <sstream>
#include <mutex>

#include "DeviceManager.h"
#include "Utils.h"
#include "EventsDefinitions.h"
#include "DriversFactory.h"
#include "DebugLogger.h"
#include "Host.h"
#include "TaskManager.h"
#include "Device.h"
#include "DriverAPI.h"
#include "LogCollector/LogCollectorService.h"

using namespace std;

namespace
{
#ifdef RTL_SIMULATION
    const int ENUMERATION_INTERVAL_MSEC = 60000;
#else
    const int ENUMERATION_INTERVAL_MSEC = 500;
#endif
}

DeviceManager::DeviceManager() :
    m_enumerationPollingIntervalMs(ENUMERATION_INTERVAL_MSEC),
    m_enumerationTaskName("enumeration"),
    m_driversFactory([this](std::vector<std::unique_ptr<DriverAPI>>& deviceDrivers){ OnDeviceAdded(deviceDrivers); },
                     [this](const std::set<std::string>& devicesNames){ OnDeviceRemoved(devicesNames); })
{}

void DeviceManager::Init()
{
    // Force first enumeration to have it completed before host starts responding to requests
    // create devices for unmonitored interfaces
    auto unMonitoredDrivers = m_driversFactory.GetNonMonitoredDrivers();
    OnDeviceAdded(unMonitoredDrivers);

    // create devices for monitored interfaces
    m_driversFactory.StartNetworkInterfaceMonitors();

    // Register for periodic update of non monitored devices, if any
    if (m_driversFactory.IsPeriodicEnumerationNeeded())
    {
        if (!Host::GetHost().GetTaskManager().RegisterTask(
            std::bind(&DeviceManager::UpdateNonMonitoredDevices, this), m_enumerationTaskName, std::chrono::milliseconds(m_enumerationPollingIntervalMs)))
        {
            LOG_ERROR << "Cannot track changes in connected devices" << endl;
        }
    }
}

DeviceManager::~DeviceManager()
{
    if (m_driversFactory.IsPeriodicEnumerationNeeded())
    {
        Host::GetHost().GetTaskManager().UnregisterTaskBlocking(m_enumerationTaskName);
    }
}

void DeviceManager::OnDeviceAdded(std::vector<std::unique_ptr<DriverAPI>>& deviceDriversAdded)
{
    for (auto& deviceDriver : deviceDriversAdded)
    {
        CreateDevice(std::move(deviceDriver));
    }
}
void DeviceManager::OnDeviceRemoved(const std::set<std::string>& devicesRemoved)
{
    for (const auto& device : devicesRemoved)
    {
        LOG_DEBUG << __FUNCTION__ << ": interface for device " << device << " was removed, deleting..." << std::endl;
        DeleteDevice(device);
    }
}

vector<std::string> DeviceManager::GetDeviceNames() const
{
    vector<std::string> deviceNames;
    lock_guard<mutex> lock(m_connectedDevicesMutex);
    deviceNames.reserve(m_devices.size());
    for (const auto& device : m_devices)
    {
        deviceNames.push_back(device.first);
    }

    return deviceNames;
}

std::vector<std::shared_ptr<Device>> DeviceManager::GetDevices() const
{
    std::vector<std::shared_ptr<Device>> devices;
    lock_guard<mutex> lock(m_connectedDevicesMutex);
    for (const auto& device : m_devices)
    {
        devices.emplace_back(device.second);
    }

    return devices;
}

// Fills spDevice with:
// * Device that matches deviceName if deviceName is not empty
// * Device if it is a single device and deviceName is empty.
OperationStatus DeviceManager::GetDeviceByName(const string& deviceName, shared_ptr<Device>& spDevice)
{
    lock_guard<mutex> lock(m_connectedDevicesMutex);

    if (m_devices.empty())
    {
        return OperationStatus(false, "No devices found on this host");
    }

    if (deviceName.empty())
    {
        // Default device is required - defined for a single device setup
        if (m_devices.size() > 1)
        {
            return OperationStatus(false, "No default device is defined for a multi-device setup");
        }

        spDevice = m_devices.begin()->second;
        return OperationStatus();
    }

    // Explicit device is required, find it

    const auto deviceIter = m_devices.find(deviceName);
    if (deviceIter != m_devices.cend())
    {
        spDevice = deviceIter->second;
        return OperationStatus();
    }

    ostringstream oss;
    oss << "Device " << deviceName << " not found";
    return OperationStatus(false, oss.str());
}

void DeviceManager::CreateDevice(unique_ptr<DriverAPI>&& deviceDriver)
{
    const string& interfaceName = deviceDriver->GetInterfaceName();
    LOG_DEBUG << "Creating Device: " << interfaceName << endl;

    shared_ptr<Device> spDevice = make_shared<Device>(std::move(deviceDriver));
    m_connectedDevicesMutex.lock();
    m_devices.insert(make_pair(interfaceName, spDevice));
    m_connectedDevicesMutex.unlock();

    // report interface list change as soon as possible
    Host::GetHost().PushEvent(InterfaceListChangedEvent(GetDeviceNames()));

    // can be outside lock because we still have the device shared ptr.
    Host::GetHost().GetLogCollectorService().OnDeviceDiscovered(interfaceName);
}

void DeviceManager::DeleteDevice(const string& deviceName)
{
    unique_lock<mutex> lock(m_connectedDevicesMutex);

    const auto deviceIter = m_devices.find(deviceName);
    if (deviceIter == m_devices.cend())
    {
        LOG_INFO << __FUNCTION__ << ": device " << deviceName << " was already removed" << endl;
        return;
    }

    // keep holding the device to schedule device destruction after its removal handled
    shared_ptr<Device> spDevice = deviceIter->second;
    m_devices.erase(deviceIter);
    lock.unlock();

    // report logs health and statistics (if publisher is active) before reporting device removal
    Host::GetHost().GetLogCollectorService().PublishLoggingHealth(deviceName);

    // report interface list change as soon as possible
    Host::GetHost().PushEvent(InterfaceListChangedEvent(GetDeviceNames()));

    // we need to release lock before calling UnregisterTaskBlocking
    // otherwise it will wait for polling task that may be blocked on this lock for device access
    Host::GetHost().GetLogCollectorService().OnDeviceRemoved(deviceName);

    spDevice.reset(); // explicit release of device ownership, not strictly needed
}

void DeviceManager::UpdateNonMonitoredDevices()
{
    if (!Host::GetHost().IsEnumerating())
    {
        return;
    }

    // collect current non-monitored interface names
    std::vector<std::pair<DeviceType, std::string>> nonMonitoredInterfaces;
    m_connectedDevicesMutex.lock();
    for (const auto& connectedDevice : m_devices)
    {
        if (connectedDevice.second->IsMonitored()) // ignore monitored devices
        {
            continue;
        }

        nonMonitoredInterfaces.emplace_back(
            connectedDevice.second->GetDeviceType(),
            connectedDevice.second->GetDeviceName());
    }
    m_connectedDevicesMutex.unlock();

    // update non-monitored devices
    // will synchronously call add/remove devices when applicable
    m_driversFactory.UpdateNonMonitoredDevices(nonMonitoredInterfaces);
}