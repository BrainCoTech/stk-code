//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2010-2015 SuperTuxKart-Team
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


#include "input/focus_config.hpp"

#include "io/xml_node.hpp"
#include "utils/log.hpp"

#include <SKeyMap.h>

#include <assert.h>


FocusConfig::FocusConfig( const std::string &name )
             : DeviceConfig()
{
    setName(name);
    setDefaultBinds();
}   // GamepadConfig


FocusConfig::FocusConfig() 
              : DeviceConfig()
{
    setDefaultBinds();
    setPlugged();
}   // FocusConfig

// ----------------------------------------------------------------------------
/** Saves the configuration to a file. It writes the name for a gamepad
 *  config, saves the device specific parameters, and calls
 *  DeviceConfig::save() to save the rest.
 *  \param stream The stream to save to.
 */
void FocusConfig::save(std::ofstream& stream)
{
    stream << "<focus name =\"" << getName()
           << "\" deadzone=\""    << getName()
           << "\" desensitize=\"" << getName()
           << "\" analog=\""      << getName()<<"\"\n";
    stream << "         ";
    DeviceConfig::save(stream);
    stream << "</focus>\n\n";
}  // save

//------------------------------------------------------------------------------

void FocusConfig::setDefaultBinds()
{
    //Bindings are not set
	//setBinding(PA_FOCUS, Input::IT_FOCUS, 0);
	//setBinding(PA_FOCUS_CONTACT, Input::IT_FOCUS_CONTACT, 0);
}

//------------------------------------------------------------------------------

