/***************************************************************************
*   Copyright (C) 2008 by
*   Laurent Schall
***************************************************************************/

#ifndef SETTINGS_CPCUSB_ARM7_H
#define SETTINGS_CPCUSB_ARM7_H

// QT includes

// Non QT includes
#include "settings_driver.h"
#include "ui_settings_cpcusb_arm7.h"

class Settings_CPCUSB_ARM7: public Settings_Driver, private Ui::SettingsCpcUsbArm7
{
    Q_OBJECT

public:
    Settings_CPCUSB_ARM7(QWidget *parent = 0);
    //	void destroy();			// Called before the driver got destroyed (end of the application)

protected:
    void enable_ui();		// Called when the stop button is pressed
    void disable_ui();	// Called when the start button is pressed
		void disable_uiWhileNotReady(); // Called when the driver is not ready
    void enable_uiWhileReady();  // Called when the driver is ready

};

#endif // SETTINGS_CPCUSB_ARM7_H


