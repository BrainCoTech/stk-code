#include "input/focus_device_manager.hpp"
#include "states_screens/options_screen_input.hpp"
#include "input/input_manager.hpp"
#include "input/device_manager.hpp"
#include "input/focus_device.hpp"
#include "utils/log.hpp"
#include "states_screens/dialogs/message_dialog.hpp"
#include <vector>
#include "fusi_sdk.h"

FocusDeviceManager* focus_device_manager;
std::vector<FusiDeviceInfo>         m_fusi_device_info_list;

MessageDialog* focus_device_search_dialog;

FocusDeviceManager::FocusDeviceManager()
{
}

// -----------------------------------------------------------------------------
FocusDeviceManager::~FocusDeviceManager()
{
}

static int str_deep_copy(const char** dst, const char* src){
    if(src == NULL)
        *dst = NULL;
    else{
        char* dst_temp = (char*) calloc(strlen(src)+1, sizeof(char));
        if(dst_temp == NULL)
            return -1;
        strcpy(dst_temp, src);
        *dst = dst_temp;
    }
    return 0;
}

FusiDeviceInfo* device_info_deep_copy(FusiDeviceInfo* info){
    if(info == NULL)
        return NULL;

    FusiDeviceInfo* copy = (FusiDeviceInfo*)malloc(sizeof(FusiDeviceInfo));
    if(copy == NULL)
        return NULL;
    
    if(str_deep_copy(&copy->ip, info->ip) != 0 ||
        str_deep_copy(&copy->mac, info->mac) != 0 ||
        str_deep_copy(&copy->name, info->name) != 0){
            return NULL;
        }
    
    return copy;
}

void device_info_release(FusiDeviceInfo* info){
    if(info == NULL)
        return;
    if(info->ip != NULL) free((void*)info->ip);
    if(info->mac != NULL) free((void*)info->mac);
    if(info->name != NULL) free((void*)info->name);
    free(info);
}

static void on_focus_search_done(FusiDeviceInfo* device, int length, FusiError* error)
{
    if(length < 0){
        printf("errorcode : %d\n", length);
    }
    else if(length == 0){
        printf("no device found\n");
    }
    else{
        printf("found [%d] devices\n", length);
        for(int id = 0; id < length; id++){
            printf("device ip[%s] mac[%s] name[%s]\n", device[id].ip, device[id].mac, device[id].name);
            m_fusi_device_info_list.push_back(device[id]);
            DeviceManager* device_manager = input_manager->getDeviceManager();
            FocusConfig* device_config = NULL;
            if (device_manager->getConfigForFocusDevice(id, device[id].name, &device_config) == true)
            {
                Log::info("Focus device manager","creating new configuration.");
            }
            else
            {
                Log::info("Focus device manager","using existing configuration.");
            }

            FocusDevice* focusDevice = NULL;
            FusiDeviceInfo* focusDeviceInfo = device_info_deep_copy(&device[id]);
            focusDevice = new FocusDevice(id,
                                          focusDeviceInfo,
                                          device_config
                                        );
            if(std::strncmp(device[id].name, "F1EDU_040A", 10) == 0)
            {
                //Log::info("Focus device manager","hahahahahhahahahahahahahah");
                //focusDevice->connectDevice();
            }
            device_manager->addFocusDevice(focusDevice);
        }
    }
    if (focus_device_search_dialog != NULL){
        focus_device_search_dialog->dismiss();
        Log::info("Focus device manager","2");
        ((OptionsScreenInput*)GUIEngine::getCurrentScreen())->rebuildDeviceList();
    }
}

bool FocusDeviceManager::searchFocusDevices(){
    Log::info("Focus device manager","start searching");
    fusi_devices_search(on_focus_search_done, 5000);
    m_fusi_device_info_list.clear();
    
    return false;
}

void FocusDeviceManager::askUserToConnectFocusDevices()
{
    focus_device_search_dialog = new MessageDialog("Searching for active Focus devices",
    MessageDialog::MESSAGE_DIALOG_WAIT,
        NULL, true);
    Log::info("Focus device manager","1");
    searchFocusDevices();
}

int FocusDeviceManager::getDeviceIdFromMac(const char* device_mac){
    DeviceManager* device_manager = input_manager->getDeviceManager();
    for (unsigned id=0; id < device_manager->getFocusDeviceAmount(); id++) {
        if(strcmp(device_manager->getFocusDevice(id)->getFocusDeviceMac(), device_mac) == 0)
        {
            Log::info("Focus device manager","device id found for mac %s", device_mac);
            return device_manager->getFocusDevice(id)->getFocusDeviceId();
        }
    }
    return -1;
}

void FocusDeviceManager::update()
{
    if(input_manager->getDeviceManager()->m_current_focus_device == NULL)
        return;
    input_manager->getDeviceManager()->m_current_focus_device->m_irr_event.lock();
    std::vector<irr::SEvent>&  events = input_manager->getDeviceManager()->m_current_focus_device->m_irr_event.getData();
    for(unsigned int i = 0; i < events.size(); i++){
        irr::SEvent event = events[0];
        input_manager->input(event);
    }
    //Log::warn("focus device manager", "before update left event size is: %ld", input_manager->getDeviceManager()->m_current_focus_device->m_irr_event.getData().size());
    events.clear();
    //Log::warn("focus device manager", "after update left event size is: %ld", input_manager->getDeviceManager()->m_current_focus_device->m_irr_event.getData().size());
    input_manager->getDeviceManager()->m_current_focus_device->m_irr_event.unlock();
}