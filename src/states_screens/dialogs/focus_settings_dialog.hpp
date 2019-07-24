#ifndef HEADER_FOCUS_SETTINGS_DIALOG_HPP
#define HEADER_FOCUS_SETTINGS_DIALOG_HPP

#include "guiengine/modaldialog.hpp"
#include "input/focus_config.hpp"
#include "input/focus_device.hpp"

namespace GUIEngine
{
    class SpinnerWidget;
    class CheckBoxWidget;
}

class FocusSettingsDialog : public GUIEngine::ModalDialog
{
private:
    FocusDevice* m_focus_device;
    FocusConfig* m_focus_config;

    GUIEngine::CheckBoxWidget* m_auto_connect_checkbox;
    GUIEngine::SpinnerWidget* m_low_threshold_slider;
    GUIEngine::SpinnerWidget* m_high_threshold_slider;
    void connectDevice();
    
public:
    /**
     * Creates a modal dialog with given percentage of screen width and height
     */
    FocusSettingsDialog(const float percentWidth, const float percentHeight, FocusConfig* config);
    ~FocusSettingsDialog();

    virtual void beforeAddingWidgets();

    GUIEngine::EventPropagation processEvent(const std::string& eventSource);
    
};

#endif
