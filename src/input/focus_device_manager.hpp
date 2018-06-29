#ifndef FOCUS_DEVICE_MANAGER_HPP
#define FOCUS_DEVICE_MANAGER_HPP

#include <vector>
#include "fusi_sdk.h"
#include "input/focus_device.hpp"
#include "states_screens/dialogs/message_dialog.hpp"
class FocusDeviceManager;

extern FocusDeviceManager* focus_device_manager;

class FocusDeviceManager{
    private:
    FusiDeviceInfo* device_info_deep_copy(FusiDeviceInfo* info);
    void device_info_release(FusiDeviceInfo* info);
    public:
    FocusDeviceManager();
    ~FocusDeviceManager();
    bool searchFocusDevices();

    void askUserToConnectFocusDevices();
    void addFocusDevice(FusiDevice& fusiDevice);
    int getDeviceIdFromMac(const char* device_mac);
};

#endif