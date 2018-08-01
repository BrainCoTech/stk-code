#ifndef FOCUS_DEVICE_MANAGER_HPP
#define FOCUS_DEVICE_MANAGER_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include "fusi_sdk.h"
#include "input/focus_device.hpp"
#include "states_screens/dialogs/message_dialog.hpp"
class FocusDeviceManager;

extern FocusDeviceManager* focus_device_manager;

class FocusDeviceManager{
    private:
    FusiDeviceInfo* device_info_deep_copy(FusiDeviceInfo* info);
    std::ofstream* m_deviceLog;
    void device_info_release(FusiDeviceInfo* info);
    public:
    FocusDeviceManager();
    ~FocusDeviceManager();
    bool searchFocusDevices();
    void update();

    void askUserToConnectFocusDevices();
    void addFocusDevice(FusiDevice& fusiDevice);
    int getDeviceIdFromMac(const char* device_mac);

    void logEvent(irr::SEvent event);
};

#endif