#ifndef HEADER_FOCUS_CONFIG_HPP
#define HEADER_FOCUS_CONFIG_HPP

#include "input/binding.hpp"
#include "input/device_config.hpp"
#include "input/input.hpp"
#include "utils/no_copy.hpp"
#include "utils/cpp2011.hpp"
#include <iosfwd>

/**
  * \brief specialisation of DeviceConfig for focus type devices
  * \ingroup config
  */
class FocusConfig : public DeviceConfig
{

public:

                FocusConfig();
                FocusConfig(const std::string &name);
    virtual    ~FocusConfig() {}

    virtual void setDefaultBinds();
    virtual void save(std::ofstream& stream);

    // ------------------------------------------------------------------------
    virtual bool isGamePad()  const { return false; }
    // ------------------------------------------------------------------------
    virtual bool isGamePadAndroid()  const { return false; }
    // ------------------------------------------------------------------------
    virtual bool isKeyboard() const { return false; }
    // ------------------------------------------------------------------------
    virtual bool isFocusDevice() const { return true; }

};   // class FocusConfig

#endif
