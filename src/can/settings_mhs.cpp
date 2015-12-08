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
#include <QSettings>

// non QT includes
#include "settings_mhs.h"

//***************************************************************************
Settings_MHS::Settings_MHS(QWidget *parent)
        : Settings_Driver(parent)
{
    setupUi(this);
    readSettings();
}

Settings_MHS::~Settings_MHS() 
{
    writeSettings();
}

void 
Settings_MHS::disable_ui() {
}

void 
Settings_MHS::enable_ui() {
}

void 
Settings_MHS::disable_uiWhileNotReady() {
}

void
Settings_MHS::enable_uiWhileReady() {
}

/*
void 
        Settings_MHS::setDriverInfo(const QString &driverInfo)
{
	if (driverInfo.size() > 60) {
		labelDriverInfo->setText(driverInfo.left(60) +  QString("..."));
		//labelDriverInfo->setToolTip(driverInfo);
	} else {
		labelDriverInfo->setText(driverInfo);
	}
}
*/

void 
Settings_MHS::setDeviceName(const QString &deviceName) {
	labelDeviceNameInfo->setText(deviceName);
}

void 
Settings_MHS::setManufacturer(const QString &deviceManufacturer) {
	labelDeviceManufacturerInfo->setText(deviceManufacturer);
}

void 
Settings_MHS::setHardwareDriverInfo(const QString &hardwareDriverInfo) 
{
	labelHardwareDriverInfo->setText(hardwareDriverInfo);
}


void 
Settings_MHS::setDeviceStatus(const QString &deviceStatus)
{
    labelDeviceStatusInfo->setText(deviceStatus); 
	//labelDeviceStatusInfo->setToolTip(deviceStatus);
}

void 
Settings_MHS::on_comboBoxBaudRate_currentIndexChanged(int value) {
    // TODO - Eventually handle a special setting for the baudrate
    switch(value) {
		case 0:
			m_baudrate = 10;
			break;
        case 1:
            m_baudrate = 20;
            break;
        case 2:
            m_baudrate = 50;
            break;
        case 3:
            m_baudrate = 100;
            break;
        case 4:
            m_baudrate = 125;
            break;
        case 5:
            m_baudrate = 250;
            break;
        case 6:
            m_baudrate = 500;
            break;
        case 7:
            m_baudrate = 800;
            break;
        case 8:
            m_baudrate = 1000;
            break;
        }
}


int 
Settings_MHS::baudrate(void) {
    return m_baudrate;
}

void
Settings_MHS::writeSettings()
{
    QSettings settings;
    settings.setValue("MHS/baudrate",m_baudrate);

}

void 
Settings_MHS::readSettings()
{
    QSettings settings;
    // Baudrate
    m_baudrate = settings.value("MHS/baudrate",10).toInt();
    switch(m_baudrate) {
                case 10:
                    comboBoxBaudRate->setCurrentIndex(0);
                    break;
                case 20:
                    comboBoxBaudRate->setCurrentIndex(1);
                    break;
                case 50:
                    comboBoxBaudRate->setCurrentIndex(2);
                    break;
                case 100:
                    comboBoxBaudRate->setCurrentIndex(3);
                    break;
                case 125:
                    comboBoxBaudRate->setCurrentIndex(4);
                    break;
                case 250:
                    comboBoxBaudRate->setCurrentIndex(5);
                    break;
                case 500:
                    comboBoxBaudRate->setCurrentIndex(6);
                    break;
                case 800:
                    comboBoxBaudRate->setCurrentIndex(7);
                    break;
                case 1000:
                    comboBoxBaudRate->setCurrentIndex(8);
                    break;
                default:
                    // TO DO - Eventually handle a special setting for the baudrate
                    m_baudrate = 10;
                    comboBoxBaudRate->setCurrentIndex(0);
                    break;
                } 
}

