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
#include "settings_demo.h"

//***************************************************************************
Settings_Demo::Settings_Demo(QWidget *parent)
        : Settings_Driver(parent)
{
    setupUi(this);
    readSettings();
    labelDeviceNameInfo->setText(tr("Demo device"));
    labelDeviceManufacturerInfo->setText(tr("CANviaUSB"));
    labelDeviceVersionInfo->setText(tr("1.0"));
    labelDeviceStatusInfo->setText(tr("Ready"));
}
Settings_Demo::~Settings_Demo()
{
    writeSettings();
}

// Use this function instead of the destructor
//void
//Settings_Demo::destroy()
//{
//        writeSettings();
//}

void
Settings_Demo::setNumberIdentifiers(const int &numberIdentifiers)
{
    spinBoxNbIdentifiers->setValue(numberIdentifiers);
}

int
Settings_Demo::getNumberIdentifiers()
{
    return spinBoxNbIdentifiers->value();
}

int
Settings_Demo::getIdentifierType()
{
    int tmp=0; //Standard only
	if (rbExtOnly->isChecked()) tmp=1;
	if (rbStdExt->isChecked()) tmp=2;
	return tmp;
}

void
Settings_Demo::setIdentifierType(const int &type)
{
	if (type==1) rbExtOnly->setChecked(true);
	else if (type==2) rbStdExt->setChecked(true);
	else rbStdOnly->setChecked(true);
}

void
Settings_Demo::setFixedDLC(const bool &fixedDlc)
{
    checkBoxFixDLC->setChecked(fixedDlc);
}

bool
Settings_Demo::getFixedDLC()
{
    return checkBoxFixDLC->isChecked();
}

void
Settings_Demo::writeSettings()
{
    QSettings settings;
    settings.setValue("Demo/nbDemoIdentifiers",getNumberIdentifiers());
    settings.setValue("Demo/IdentifiersType",getIdentifierType());
    settings.setValue("Demo/FixedDLC",getFixedDLC());

}

void Settings_Demo::readSettings()
{
    QSettings settings;
    setNumberIdentifiers(settings.value("Demo/nbDemoIdentifiers",10).toInt());
    setIdentifierType(settings.value("Demo/IdentifiersType",1).toInt());
    setIdentifierType(settings.value("Demo/FixedDLC",true).toBool());
}

void 
Settings_Demo::disable_ui() {
    deviceSettings->setEnabled(false);
}

void 
Settings_Demo::enable_ui() {
    deviceSettings->setEnabled(true);
}

void 
Settings_Demo::disable_uiWhileNotReady() {

}

void 
Settings_Demo::enable_uiWhileReady() {

}
