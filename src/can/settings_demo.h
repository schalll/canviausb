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

#ifndef SETTINGS_DEMO_H
#define SETTINGS_DEMO_H

// QT includes

// Non QT includes
#include "settings_driver.h"
#include "ui_settings_demo.h"

class Settings_Demo: public Settings_Driver, private Ui::SettingsDemo
{
    Q_OBJECT

public:
    Settings_Demo(QWidget *parent = 0);
    ~Settings_Demo();
    //void destroy();			// Called before the driver got destroyed (end of the application)

    void setNumberIdentifiers(const int &numberIdentifiers);
    int getNumberIdentifiers();

    void setIdentifierType(const int &type);
    int getIdentifierType();

    void setFixedDLC(const bool &fixedDlc);
    bool getFixedDLC();

protected:
    void enable_ui();		// Called when the stop button is pressed
    void disable_ui();	// Called when the start button is pressed
    void disable_uiWhileNotReady(); // Called when the driver is not ready
    void enable_uiWhileReady();  // Called when the driver is ready




private:
    void writeSettings();
    void readSettings();
};

#endif // SETTINGS_DEMO_H


