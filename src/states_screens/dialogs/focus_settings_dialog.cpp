//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2014-2015 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "states_screens/dialogs/focus_settings_dialog.hpp"

#include "config/user_config.hpp"
#include "graphics/irr_driver.hpp"
#include "guiengine/widgets/check_box_widget.hpp"
#include "guiengine/widgets/spinner_widget.hpp"
#include "input/device_manager.hpp"
#include "input/input_manager.hpp"
#include "input/focus_device.hpp"
#include "modes/world.hpp"
#include "utils/translation.hpp"


#include <IGUIEnvironment.h>


using namespace GUIEngine;

// -----------------------------------------------------------------------------

FocusSettingsDialog::FocusSettingsDialog(const float w, const float h, FocusConfig* config)
        : ModalDialog(w, h)
{
    loadFromFile("focus_settings_dialog.stkgui");
    m_focus_device = input_manager->getDeviceManager()->getFocusDeviceByName(config->getName());
    m_focus_config = config;
    m_auto_connect_checkbox = getWidget<CheckBoxWidget>("auto_connect");
    assert(m_auto_connect_checkbox != NULL);
    m_auto_connect_checkbox->setState(m_focus_config->getAutoConnect());
    m_low_threshold_slider = getWidget<SpinnerWidget>("low_threshold");
    assert(m_low_threshold_slider != NULL);
    m_low_threshold_slider->setValue(m_focus_config->getLowThreshold());
    m_high_threshold_slider = getWidget<SpinnerWidget>("high_threshold");
    assert(m_high_threshold_slider != NULL);
    m_high_threshold_slider->setValue(m_focus_config->getHighThreshold());
}

// -----------------------------------------------------------------------------

FocusSettingsDialog::~FocusSettingsDialog()
{
}

// -----------------------------------------------------------------------------

GUIEngine::EventPropagation FocusSettingsDialog::processEvent(
                                                const std::string& eventSource)
{
    if (eventSource == "close")
    {

        m_focus_config->setAutoConnect(m_auto_connect_checkbox->getState());
        m_focus_config->setLowThreshold(m_low_threshold_slider->getValue());
        m_focus_config->setHighThreshold(m_high_threshold_slider->getValue());
        DeviceManager* device_manager = input_manager->getDeviceManager();
        device_manager->save();
        if(m_focus_config->getAutoConnect()){
            connectDevice();
        }
        ModalDialog::dismiss();
        return GUIEngine::EVENT_BLOCK;
    }
    if (eventSource == "connect")
    {
        connectDevice();
        return GUIEngine::EVENT_BLOCK;
    }
    else if (eventSource == "restore")
    {
        m_auto_connect_checkbox->setState(m_focus_config->getAutoConnect());
        m_low_threshold_slider->setValue(m_focus_config->getLowThreshold());
        m_high_threshold_slider->setValue(m_focus_config->getHighThreshold());
        return GUIEngine::EVENT_BLOCK;
    }

    return GUIEngine::EVENT_LET;
}   // processEvent

// -----------------------------------------------------------------------------

void FocusSettingsDialog::connectDevice()
{
    DeviceManager* device_manager = input_manager->getDeviceManager();
    if(device_manager->m_current_focus_device != m_focus_device){
        if(device_manager->m_current_focus_device != NULL){
            device_manager->m_current_focus_device->disconnectDevice();
            device_manager->m_current_focus_device = NULL;
        }
        device_manager->m_current_focus_device = m_focus_device;
        device_manager->m_current_focus_device->connectDevice();
    }
    
}

// -----------------------------------------------------------------------------
