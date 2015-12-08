/***************************************************************************
*   Copyright (C) 2008 by
*   Laurent Schall
***************************************************************************/

// QT includes

// non QT includes
#include "settings_cpcusb_arm7.h"

//***************************************************************************
Settings_CPCUSB_ARM7::Settings_CPCUSB_ARM7(QWidget *parent)
        : Settings_Driver(parent)
{
    setupUi(this);
}

// Use this function instead of the destructor
//void
//Settings_CPCUSB_ARM7::destroy()
//{
//        //writeSettings();
//}

void 
        Settings_CPCUSB_ARM7::disable_ui() {
}

void 
        Settings_CPCUSB_ARM7::enable_ui() {
}

void 
        Settings_CPCUSB_ARM7::disable_uiWhileNotReady() {
}

void
        Settings_CPCUSB_ARM7::enable_uiWhileReady() {
}
