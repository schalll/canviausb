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

#ifndef SETTINGS_DRIVER_H
#define SETTINGS_DRIVER_H

// QT includes
#include <QWidget>

// Non QT includes


class Settings_Driver: public QWidget
{
    Q_OBJECT

public:
    Settings_Driver(QWidget *parent = 0);
    //virtual void destroy();			// Called before the driver got destroyed (end of the application)

protected:
    virtual void enable_ui();		// Called when the stop button is pressed
    virtual void disable_ui();	// Called when the start button is pressed
    virtual void disable_uiWhileNotReady(); // Called when the driver is not ready
    virtual void enable_uiWhileReady();  // Called when the driver is ready

private slots:
    void communicationStatusChanged(bool isCommunicating);
    void readyStatusChanged(bool isReady);
};

#endif // SETTINGS_DRIVER_H
