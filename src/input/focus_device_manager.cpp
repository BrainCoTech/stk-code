#include "input/focus_device_manager.hpp"
#include "states_screens/options/options_screen_input.hpp"
#include "input/input_manager.hpp"
#include "input/device_manager.hpp"
#include "input/focus_device.hpp"
#include "utils/log.hpp"
#include "io/file_manager.hpp"
#include "states_screens/dialogs/message_dialog.hpp"
#include <vector>
#include "fusi_sdk.h"
#include <sstream>
#include <chrono>
#include "guiengine/screen_keyboard.hpp"
using namespace std;
FocusDeviceManager* focus_device_manager;
std::vector<FusiDeviceInfo>         m_fusi_device_info_list;
static const char DEVICE_FILE_NAME[] = "focus.xml";

MessageDialog* focus_device_search_dialog;

FocusDeviceManager::FocusDeviceManager()
{
    m_deviceLog = new std::ofstream();
    std::string filepath = file_manager->getUserConfigFile(DEVICE_FILE_NAME);
    m_deviceLog -> open(filepath.c_str(), std::ofstream::out | std::ofstream::app);
}

static string current_timelabel = "";
static string getTimeLabel(){
        chrono::time_point<chrono::system_clock> sys_clock = chrono::system_clock::now();
        time_t now = chrono::system_clock::to_time_t(sys_clock);
        tm* localTime = localtime(&now);

        ostringstream timeLabelStream;
        timeLabelStream << localTime->tm_year + 1900;

        if(localTime->tm_mon < 9) timeLabelStream << '0';
        timeLabelStream << localTime->tm_mon + 1;
        if(localTime->tm_mday < 10) timeLabelStream << '0';
        timeLabelStream << localTime->tm_mday << '-';
        if(localTime -> tm_hour < 10) timeLabelStream << '0';
        timeLabelStream << localTime->tm_hour << '-';
        if(localTime -> tm_min < 10) timeLabelStream << '0';
        timeLabelStream<< localTime->tm_min << '-';
        if(localTime -> tm_sec < 10) timeLabelStream << '0';
        timeLabelStream << localTime -> tm_sec;
        return timeLabelStream.str();
    }

// -----------------------------------------------------------------------------
FocusDeviceManager::~FocusDeviceManager()
{
    if(m_deviceLog->is_open()){
        m_deviceLog->close();
    }

    delete m_deviceLog;
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
    bool created = false;
    if(length < 0){
        printf("errorcode : %d\n", length);
    }
    else if(length == 0){
        printf("no device found\n");
    }
    else{
        printf("found [%d] devices\n", length);
        DeviceManager* device_manager = input_manager->getDeviceManager();
        for(int id = 0; id < length; id++){
            printf("device ip[%s] mac[%s] name[%s]\n", device[id].ip, device[id].mac, device[id].name);
            m_fusi_device_info_list.push_back(device[id]);
            FocusConfig* device_config = NULL;
            if (device_manager->getConfigForFocusDevice(id, device[id].name, &device_config) == true)
            {
                Log::info("Focus device manager","creating new configuration.");
                created = true;
            }

            FocusDevice* focusDevice = NULL;
            FusiDeviceInfo* focusDeviceInfo = device_info_deep_copy(&device[id]);
            device_config->setPlugged();
            focusDevice = new FocusDevice(id,
                                          focusDeviceInfo,
                                          device_config
                                        );
            device_manager->addFocusDevice(focusDevice);
            if(device_config->getAutoConnect()){
                if(device_manager->m_current_focus_device != NULL){
                    device_manager->m_current_focus_device->disconnectDevice();
                    device_manager->m_current_focus_device = NULL;
                }
                device_manager->m_current_focus_device = focusDevice;
                device_manager->m_current_focus_device->connectDevice();
            }
        }
        if (created) device_manager->save();
    }
    if (focus_device_search_dialog != NULL){
        focus_device_search_dialog->dismiss();
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
        irr::SEvent event = events[i];
        if(Input::IT_NONE != event.UserEvent.type)
            input_manager->input(event);

        logEvent(event);        
    }
    //Log::warn("focus device manager", "before update left event size is: %ld", input_manager->getDeviceManager()->m_current_focus_device->m_irr_event.getData().size());
    events.clear();
    //Log::warn("focus device manager", "after update left event size is: %ld", input_manager->getDeviceManager()->m_current_focus_device->m_irr_event.getData().size());
    input_manager->getDeviceManager()->m_current_focus_device->m_irr_event.unlock();
}

void FocusDeviceManager::logEvent(irr::SEvent event)
{
    current_timelabel = getTimeLabel();
    if(Input::IT_NONE == event.UserEvent.type){
        EEGData* dataForLog = (EEGData*)event.UserEvent.data;
        *m_deviceLog << current_timelabel << " " << input_manager->getDeviceManager()->getFocusDevice(event.UserEvent.UserData1)->getFocusDeviceMac() << " " << event.UserEvent.type << " ";
        *m_deviceLog << dataForLog->size << " " << dataForLog->sample_rate << " " << dataForLog->pga << " ";
        *m_deviceLog << "[" << dataForLog->data[0];
        for(int i = 1; i < dataForLog->size; i++){
            *m_deviceLog << " " << dataForLog->data[i] ;
        }
        *m_deviceLog << "]" << endl;

        delete [] dataForLog->data;
        delete dataForLog;
    }
    else{
        bool isInGame = (StateManager::get()->getGameState() == GUIEngine::GAME &&
             !GUIEngine::ModalDialog::isADialogActive()            &&
             !GUIEngine::ScreenKeyboard::isActive()                &&
             !race_manager->isWatchingReplay() );

        bool isInGameMode = (StateManager::get()->getGameState() == GUIEngine::GAME);
        if(isInGame)
            *m_deviceLog << current_timelabel << " " << input_manager->getDeviceManager()->getFocusDevice(event.UserEvent.UserData1)->getFocusDeviceMac() << " " << event.UserEvent.type << " " << event.UserEvent.UserData2 << " 0 INGAME" <<  std::endl;
        else if(isInGameMode)
            *m_deviceLog << current_timelabel << " " << input_manager->getDeviceManager()->getFocusDevice(event.UserEvent.UserData1)->getFocusDeviceMac() << " " << event.UserEvent.type << " " << event.UserEvent.UserData2 << " 1 INPAUSE" <<  std::endl;
        else
            *m_deviceLog << current_timelabel << " " << input_manager->getDeviceManager()->getFocusDevice(event.UserEvent.UserData1)->getFocusDeviceMac() << " " << event.UserEvent.type << " " << event.UserEvent.UserData2 << " 2 OTHERS" << std::endl;
    }
}

void FocusDeviceManager::logTagEvent(string tag)
{
    current_timelabel = getTimeLabel();
    *m_deviceLog << current_timelabel << " " << input_manager->getDeviceManager()->m_current_focus_device->getFocusDeviceMac() << " " << tag << std::endl;

}