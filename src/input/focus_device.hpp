#ifndef HEADER_FOCUS_DEVICE_HPP
#define HEADER_FOCUS_DEVICE_HPP

#include "input/input_device.hpp"
#include "fusi_sdk.h"
#include "utils/cpp2011.hpp"
#include "utils/synchronised.hpp"
#include <vector>

class FocusConfig;

/**
  * \brief specialisation of InputDevice for focus type devices
  * \ingroup input
  */
class FocusDevice : public InputDevice
{
private:
    /** Index of this element the arrays of focus devices */
    int             m_focus_device_id;
    FusiDeviceInfo* m_focus_device_info;
    FusiDevice*     m_focus_device;
public:
    FocusDevice(const int focus_device_id, FusiDeviceInfo* focus_device_info,
                FocusConfig* configuration);

    virtual ~FocusDevice() {}
    virtual bool processAndMapInput(Input::InputType type,  const int id,
                                    InputManager::InputDriverMode mode,
                                    PlayerAction *action, int* value = NULL
                                    ) OVERRIDE;
    void connectDevice();
    void disconnectDevice();
    
    int getFocusDeviceId() { return m_focus_device_id; }
    const char* getFocusDeviceMac() { return m_focus_device_info->mac; }
    Synchronised<std::vector<irr::SEvent> > m_irr_event;
};   // FocusDevice

#endif
