#ifndef HEADER_FOCUS_SETTINGS_DIALOG_HPP
#define HEADER_FOCUS_SETTINGS_DIALOG_HPP

#include "guiengine/modaldialog.hpp"

/**
 * \brief Dialog that allows the player to adjust multitouch steering settings
 * \ingroup states_screens
 */
class FocusSettingsDialog : public GUIEngine::ModalDialog
{
private:
    void updateValues();
    
public:
    /**
     * Creates a modal dialog with given percentage of screen width and height
     */
    FocusSettingsDialog(const float percentWidth, const float percentHeight);
    ~FocusSettingsDialog();

    virtual void beforeAddingWidgets();

    GUIEngine::EventPropagation processEvent(const std::string& eventSource);
    
};

#endif
