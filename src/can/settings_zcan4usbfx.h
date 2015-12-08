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

#ifndef SETTINGS_ZCAN4USBFX_H
#define SETTINGS_ZCAN4USBFX_H

// QT includes
#include <QCheckBox>

// Non QT includes
#include "settings_driver.h"
#include "ui_settings_zcan4usbfx.h"

class Settings_ZCAN4USBFX: public Settings_Driver, private Ui::SettingsZcan4usbfx
{
    Q_OBJECT

public:
    Settings_ZCAN4USBFX(QWidget *parent = 0);
    ~Settings_ZCAN4USBFX();
    //	void destroy();			// Called before the driver got destroyed (end of the application)

    void setDeviceName(const QString &deviceName);
    void setManufacturer(const QString &manufacturerName);
    void setSoftwareVersion(const QString &softwareVersion);
    void setDeviceStatus(const QString &deviceStatus);
    void setEditable(bool edit);
    void setRegisterValues(unsigned char column, unsigned char *values);
    
    bool useFilter();
    unsigned int getFilter(unsigned char num);

    int baudrate(void);
    bool useTimestamp(void);
    bool readRegister(void);

protected:
    void enable_ui();		// Called when the stop button is pressed
    void disable_ui();	// Called when the start button is pressed
    void disable_uiWhileNotReady(); // Called when the driver is not ready
    void enable_uiWhileReady();  // Called when the driver is ready

private slots:
    void on_comboBoxBaudRate_currentIndexChanged(int value);
    void on_checkBoxTimeStamp_stateChanged(int state);
    void on_pushButtonReadRegister_clicked();
    void verifyIDFilter();

private:
    void writeSettings();
    void readSettings();
    void verifyIDFilterFor(QCheckBox *cb, QLineEdit *le);

    int m_baudrate;
    bool m_timestamp;
    bool m_readRegister;

};

#endif // SETTINGS_ZCAN4USBFX_H


