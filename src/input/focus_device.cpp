#include "input/focus_device.hpp"
#include "input/focus_device_manager.hpp"
#include "input/focus_config.hpp"
#include "fusi_sdk.h"

#define ATTENTION_BUFF_SIZE 10
bool attentionStartFlag = false;
int attentionBuff[] = {0,0,0,0,0,0,0,0,0,0};
int attentionBuffIndex = 0;
// ----------------------------------------------------------------------------
FocusDevice::FocusDevice(const int focus_device_id, FusiDeviceInfo* focus_device_info,
                         FocusConfig *configuration)
{
    m_focus_device_id = focus_device_id;
    m_focus_device_info = focus_device_info;
    m_focus_device  = NULL;
    m_configuration = configuration;
    m_type          = DT_FOCUS;
    m_name          = "Focus";
    m_player        = NULL;
}   // FocusDevice

static void on_device_connect_callback(const char* device_mac, DeviceConnectionState state)
{
    Log::info("Focus device manager","connected %s", device_mac);
}

static void on_device_attention_callback(const char* device_mac, double attention){
    Log::info("Focus device manager","connected %s %f", device_mac, attention);
    irr::SEvent event;
    event.EventType = irr::EET_USER_EVENT;
    event.UserEvent.UserData1 = focus_device_manager->getDeviceIdFromMac(device_mac);
    event.UserEvent.UserData2 = attention;
    event.UserEvent.type = 100;
    input_manager->input(event);
}

static void on_device_connection_change_callback(const char* device_mac, DeviceConnectionState state)
{
    irr::SEvent event;
    event.EventType = irr::EET_USER_EVENT;
    event.UserEvent.UserData1 = focus_device_manager->getDeviceIdFromMac(device_mac);
    if(state == 0)
        event.UserEvent.UserData2 = 0;
    else if(state == 1)
        event.UserEvent.UserData2 = 1;
    else
        event.UserEvent.UserData2 = -1;
    
    event.UserEvent.type = Input::IT_FOCUS_CONTACT;
    input_manager->input(event);
}

static void on_device_contact_state_change_callback(const char* device_mac, DeviceContactState state)
{
    irr::SEvent event;
    event.EventType = irr::EET_USER_EVENT;
    event.UserEvent.UserData1 = focus_device_manager->getDeviceIdFromMac(device_mac);
    if(state == 0)
        event.UserEvent.UserData2 = 3;
    else if(state == 1)
        event.UserEvent.UserData2 = 1;
    else
        event.UserEvent.UserData2 = 2;
    
    event.UserEvent.type = Input::IT_FOCUS_CONTACT;
    input_manager->input(event);
}

static void on_eeg_stats_callback(const char* device_mac, EEGStats* stats){
    irr::SEvent event;
    event.EventType = irr::EET_USER_EVENT;
    event.UserEvent.UserData1 = focus_device_manager->getDeviceIdFromMac(device_mac);
    Log::warn("Focus device manager","lowbeta[%f] theta[%f] result[%f]", stats->low_beta, stats->theta, stats->low_beta*200/stats->theta);
    double newAcc =  stats->low_beta*250/(stats->theta + stats ->alpha);
    if(newAcc > 100.0) newAcc = 100.0;
    attentionBuff[attentionBuffIndex++] = newAcc;
    if(attentionBuffIndex == ATTENTION_BUFF_SIZE) {
        attentionBuffIndex = 0;
        if(!attentionStartFlag) attentionStartFlag = true;
    }
    double acc = 0;
    if(!attentionStartFlag) {
        for(int i = 0;i<=attentionBuffIndex;i++) acc += attentionBuff[attentionBuffIndex]/(attentionBuffIndex + 1);
    } else{
        for(int i = 0;i<ATTENTION_BUFF_SIZE;i++) acc += attentionBuff[attentionBuffIndex]/ATTENTION_BUFF_SIZE;
    }
    event.UserEvent.UserData2 = (int)acc;
    event.UserEvent.type = Input::IT_FOCUS;
    input_manager->input(event);
}

void FocusDevice::connectDevice(){
    Log::info("Focus device manager","start connecting %s %s", m_focus_device_info->mac, m_focus_device_info->ip);
    m_focus_device = fusi_device_create(*m_focus_device_info);
    //set_attention_callback(m_focus_device, on_device_attention_callback);
    set_eeg_stats_callback(m_focus_device, on_eeg_stats_callback);
    set_device_connection_callback(m_focus_device, on_device_connection_change_callback);
    set_device_contact_state_callback(m_focus_device, on_device_contact_state_change_callback);
    fusi_connect(m_focus_device, on_device_connection_change_callback, 1);
}

void FocusDevice::disconnectDevice(){
    if(m_focus_device != NULL)
        fusi_disconnect(m_focus_device);
}

// ----------------------------------------------------------------------------
/** Invoked when this device it used. Verifies if the key/button that was
 *  pressed is associated with a binding. If yes, sets action and returns
 *  true; otherwise returns false. It can also modify the value used.
 *  \param type Type of input (e.g. IT_STICKMOTION, ...).
 *  \param id   ID of the key that was pressed or of the axis that was
 *              triggered (depending on the value of the 'type' parameter).
 *  \param mode Used to determine whether to map menu actions or
 *              game actions
 * \param[out] action  The action associated to this input (only check
 *                     this value if method returned true)
 * \param[in,out] value The value associated with this type (typically
 *                      how far a gamepad axis is moved).
 *
 * \return Whether the pressed key/button is bound with an action
 */
bool FocusDevice::processAndMapInput(Input::InputType type,  const int id,
                                        InputManager::InputDriverMode mode,
                                        PlayerAction *action, int* value)
{
    // bindings can only be accessed in game
    if (mode == InputManager::INGAME)
    {
        if(type == Input::IT_FOCUS)
        {
            //return m_configuration->getGameAction(Input::IT_FOCUS, id, value, action);
            *action = PlayerAction(PA_FOCUS);
            *value = int(Input::MAX_VALUE * (*value) / 100);
        }
        return true;
    }

    if(type == Input::IT_FOCUS_CONTACT)
    {
        *action = PlayerAction(PA_FOCUS_CONTACT);
        return true;
    }
    return false;
}   // processAndMapInput