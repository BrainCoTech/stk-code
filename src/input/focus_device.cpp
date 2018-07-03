#include "input/focus_device.hpp"
#include "input/focus_device_manager.hpp"
#include "input/focus_config.hpp"
#include "fusi_sdk.h"
// ----------------------------------------------------------------------------
FocusDevice::FocusDevice(const int focus_device_id, FusiDeviceInfo* focus_device_info,
                         FocusConfig *configuration)
{
    m_focus_device_id = focus_device_id;
    m_focus_device_info = focus_device_info;
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
    input_manager->input(event);
}

void FocusDevice::connectDevice(){
    Log::info("Focus device manager","start connecting %s %s", m_focus_device_info->mac, m_focus_device_info->ip);
    FusiDevice* fusiDevice = fusi_device_create(*m_focus_device_info);
    set_attention_callback(fusiDevice, on_device_attention_callback);
    fusi_connect(fusiDevice, on_device_connect_callback, 1);
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
    assert(type==Input::IT_FOCUS);
    // bindings can only be accessed in game
    if (mode == InputManager::INGAME)
    {
        //return m_configuration->getGameAction(Input::IT_FOCUS, id, value, action);
        *action = PlayerAction(PA_FOCUS);
        *value = int(Input::MAX_VALUE * (*value) / 100);
        return true;
    }
    return false;
}   // processAndMapInput