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
using namespace irr;
using namespace irr::core;
using namespace irr::gui;

// -----------------------------------------------------------------------------

FocusSettingsDialog::FocusSettingsDialog(const float w, const float h)
        : ModalDialog(w, h)
{
    loadFromFile("focus_settings_dialog.stkgui");
}

// -----------------------------------------------------------------------------

FocusSettingsDialog::~FocusSettingsDialog()
{
}

// -----------------------------------------------------------------------------

void FocusSettingsDialog::beforeAddingWidgets()
{
    if (StateManager::get()->getGameState() == GUIEngine::INGAME_MENU)
    {
        CheckBoxWidget* buttons_en = getWidget<CheckBoxWidget>("buttons_enabled");
        assert(buttons_en != NULL);
        buttons_en->setActive(false);
    }

    updateValues();
}

// -----------------------------------------------------------------------------

GUIEngine::EventPropagation FocusSettingsDialog::processEvent(
                                                const std::string& eventSource)
{
    if (eventSource == "close")
    {

        Log::info("FocusSettingsDialog", "close event");
        user_config->saveConfig();

        ModalDialog::dismiss();
        return GUIEngine::EVENT_BLOCK;
    }
    else if (eventSource == "restore")
    {
        Log::info("FocusSettingsDialog", "restore event");
        updateValues();

        return GUIEngine::EVENT_BLOCK;
    }

    return GUIEngine::EVENT_LET;
}   // processEvent

// -----------------------------------------------------------------------------

void FocusSettingsDialog::updateValues()
{
     Log::info("FocusSettingsDialog", "update");
}

// -----------------------------------------------------------------------------
