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
#include <iomanip>  // setfill, setw
#include "guiengine/screen_keyboard.hpp"
using namespace std;
FocusDeviceManager* focus_device_manager;
std::vector<FusiDeviceInfo> m_fusi_device_info_list;
static const char DEVICE_FILE_NAME[] = "focus.log";

MessageDialog* focus_device_search_dialog;

FocusDeviceManager::FocusDeviceManager()
{
    m_deviceLog = new std::ofstream();

    // TODO: add timestamp to file name
    std::string filepath = file_manager->getUserConfigFile(DEVICE_FILE_NAME);
    m_deviceLog -> open(filepath.c_str(), std::ofstream::out | std::ofstream::app);
}

static string getTimeLabel()
{
    chrono::time_point<chrono::system_clock> sys_clock = chrono::system_clock::now();
    time_t now = chrono::system_clock::to_time_t(sys_clock);
    tm* localTime = localtime(&now);

    ostringstream timeLabelStream;

    timeLabelStream << localTime->tm_year + 1900            << '-'  // year
        << setfill('0') << setw(2) << localTime->tm_mon + 1 << '-'  // month
        << setfill('0') << setw(2) << localTime->tm_mday    << ' '  // day
        << setfill('0') << setw(2) << localTime->tm_hour    << ':'  // hour
        << setfill('0') << setw(2) << localTime->tm_min     << ':'  // min
        << setfill('0') << setw(2) << localTime->tm_sec;            // sec

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

//TODO: This function is getting super ugly, rewrite
static void on_focus_search_done(FusiDeviceInfo* device, int length, FusiError* error)
{
    bool created = false;
    DeviceManager* device_manager = input_manager->getDeviceManager();
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
            FocusConfig* device_config = NULL;
            if (device_manager->getConfigForFocusDevice(id, device[id].name, &device_config) == true)
            {
                Log::info("Focus device manager","creating new configuration.");
                created = true;
            }

            FocusDevice* focusDevice = device_manager->getFocusDeviceByName(device[id].name);
            if(focusDevice == NULL){
                FusiDeviceInfo* focusDeviceInfo = device_info_deep_copy(&device[id]);
                if(!device_config->isPlugged()){
                    device_config->setPlugged();
                }
                focusDevice = new FocusDevice(id,
                                          focusDeviceInfo,
                                          device_config
                                        );
                device_manager->addFocusDevice(focusDevice);
            }

            if(device_config->getAutoConnect() && device_manager->m_current_focus_device == NULL){
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
    if(device_manager->m_current_focus_device == NULL){
        focus_device_manager->searchFocusDevices();
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
    events.clear();
    input_manager->getDeviceManager()->m_current_focus_device->m_irr_event.unlock();
}

void FocusDeviceManager::logEvent(irr::SEvent event)
{
    // JSON Log Format
    // https://github.com/BrainCoTech/stk-code/issues/9

    // begin
    *m_deviceLog << "{";

    // 1. date_time
    string currentTimeLabel = getTimeLabel();
    *m_deviceLog << "\"date_time\": \"" << currentTimeLabel << "\"";

    // in game mode
    bool isInGameMode = StateManager::get()->getGameState() == GUIEngine::GAME;
    // in game
    bool isInGame = isInGameMode
            && !GUIEngine::ModalDialog::isADialogActive()
            && !GUIEngine::ScreenKeyboard::isActive()
            && !race_manager->isWatchingReplay();

    // 2. game status
    string gameStatus;
    if (isInGame) {
        gameStatus = "INGAME";
    } else if (isInGameMode) {
        gameStatus = "INPAUSE";
    } else {
        gameStatus = "OTHERS";
    }
    *m_deviceLog << ", \"game_status\": \"" << gameStatus << "\"";

    // 3. FocusDeviceMac
    const char * focusDeviceMac = input_manager->getDeviceManager()
                                    ->getFocusDevice(event.UserEvent.UserData1)
                                    ->getFocusDeviceMac();
    *m_deviceLog << ", \"device_mac\": \"" << focusDeviceMac << "\"";

    // 4. TODO: low, high threshold

    // 5. event type and data
    *m_deviceLog << ", \"event_type\": ";
    switch (event.UserEvent.type) {
        // 5-1. IT_NONE
        case Input::IT_NONE: {
            EEGData* eeg = (EEGData*) event.UserEvent.data;
            *m_deviceLog << "\"IT_NONE\""
                         << ", \"egg\": {\"sample_rate\": " << eeg->sample_rate
                         << ", \"pga\": " << eeg->pga
                         << ", \"data\": [";

            for (int i = 0; i < eeg->size; i++) {
                if (i != 0) {
                    *m_deviceLog << ", ";
                }
                *m_deviceLog << eeg->data[i];
            }
            *m_deviceLog << "]}";

            delete [] eeg->data;
            delete eeg;
            break;
        }

        // 5-2. IT_FOCUS
        case Input::IT_FOCUS: {
            *m_deviceLog << "\"IT_FOCUS\", \"attention_or_acc\": " << event.UserEvent.UserData2;
            break;
        }

        // 5-3. IT_FOCUS_CONTACT
        case Input::IT_FOCUS_CONTACT:{
            // state: -1 ~ 5
            *m_deviceLog << "\"IT_FOCUS_CONTACT\", \"contact_state\": " << event.UserEvent.UserData2;
            break;
        }

        // 5-4. UNKNOWN
        default:{
            *m_deviceLog << "\"UNKNOWN\"";
            break;
        }
    }

    // end
    *m_deviceLog << "}" << endl;
}

void FocusDeviceManager::logTagEvent(string tag)
{
    string currentTimeLabel = getTimeLabel();
    const char * focusDeviceMac = input_manager->getDeviceManager()
                                    ->m_current_focus_device
                                    ->getFocusDeviceMac();

    *m_deviceLog << currentTimeLabel
            << " " << focusDeviceMac
            << " " << tag << std::endl;
}
