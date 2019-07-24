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
    m_auto_connect = false;
    m_low_threshold = 30;
    m_high_threshold = 100;
}   // FocusConfig


FocusConfig::FocusConfig() 
              : DeviceConfig()
{
    m_auto_connect = false;
    m_low_threshold = 30;
    m_high_threshold = 100;
}   // FocusConfig

//------------------------------------------------------------------------------
/** Loads this configuration from the given XML node.
 *  \param config The XML tree.
 *  \return False in case of an error.
 */
bool FocusConfig::load(const XMLNode *config)
{
    config->get("low_threshold",        &m_low_threshold);
    config->get("high_threshold",       &m_high_threshold);
    config->get("auto_connect",  &m_auto_connect);
    bool ok = DeviceConfig::load(config);

    if(getName()=="")
    {
        Log::error("FocusConfig", "Unnamed focus device in config file.");
        return false;
    }
    return ok;
}   // load

// ----------------------------------------------------------------------------
/** Saves the configuration to a file. It writes the name for a gamepad
 *  config, saves the device specific parameters, and calls
 *  DeviceConfig::save() to save the rest.
 *  \param stream The stream to save to.
 */
void FocusConfig::save(std::ofstream& stream)
{
    stream << "<focus name =\"" << getName()
           << "\" auto_connect=\""    << m_auto_connect
           << "\" low_threshold=\""    << m_low_threshold
           << "\" high_threshold=\""      << m_high_threshold <<"\"\n";
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

