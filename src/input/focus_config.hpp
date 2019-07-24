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

private:
    bool m_auto_connect;
    int m_low_threshold;
    int m_high_threshold;
public:

                FocusConfig();
                FocusConfig(const std::string &name);
    virtual    ~FocusConfig() {}

    virtual void setDefaultBinds();
    bool getAutoConnect() const { return m_auto_connect; }
    void setAutoConnect(bool auto_connect) { m_auto_connect = auto_connect; }
    int getLowThreshold() const { return m_low_threshold; }
    void setLowThreshold(int low_threshold) { m_low_threshold = low_threshold; }
    int getHighThreshold() const { return m_high_threshold; }
    void setHighThreshold(int high_threshold) { m_high_threshold = high_threshold; }

    virtual void save(std::ofstream& stream);
    virtual bool load(const XMLNode *config);
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
