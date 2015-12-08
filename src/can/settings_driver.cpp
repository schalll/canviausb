/*
 *  Copyright 2015 Laurent Schall (schall_l@yahoo.fr)
 *
 *  This file is part of CANviaUSB.
 *
 *  CANviaUSB is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  CANviaUSB is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CANviaUSB. If not, see <http://www.gnu.org/licenses/>.
*/

// QT includes

// non QT includes
#include "settings_driver.h"

//***************************************************************************
Settings_Driver::Settings_Driver(QWidget *parent)
        : QWidget(parent)
{
    //setupUi(this);
}

//void 
//Settings_Driver::destroy() {			// Called before the driver got destroyed (end of the application)
//}

void 
        Settings_Driver::communicationStatusChanged(bool isCommunicating) {
    // This slot is connected to the can_driver communicationStatus signal in the constructor from each driver:
    // - Can_Demo::Can_Demo()
    // - Can_CPCUSB_ARM7::Can_CPCUSB_ARM7()
    // - Can_ZCAN4USBFX::Can_ZCAN4USBFX()

    if (isCommunicating)
        disable_ui();
    else
        enable_ui();
}

void 
        Settings_Driver::readyStatusChanged(bool isReady) {
    // This slot is connected to the can_driver communicationStatus signal in the constructor from each driver:
    // - Can_Demo::Can_Demo()
    // - Can_CPCUSB_ARM7::Can_CPCUSB_ARM7()
    // - Can_ZCAN4USBFX::Can_ZCAN4USBFX()

    if (isReady)
        enable_uiWhileReady();
    else
        disable_uiWhileNotReady();
}

void 
        Settings_Driver::disable_ui() {
}

void 
        Settings_Driver::enable_ui() {
}


void 
        Settings_Driver::disable_uiWhileNotReady() {

}

void 
        Settings_Driver::enable_uiWhileReady() {

}
