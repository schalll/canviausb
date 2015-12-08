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
#include "appSettings.h"

AppSettings::AppSettings() {	
    QSettings settings;
    m_displayPCTimestamp = settings.value("settings/displayPCTimestamp",true).toBool();
    m_displayAdapterTimestamp = settings.value("settings/displayAdapterTimestamp",false).toBool();
    m_displayIDFormat = settings.value("settings/displayIDFormat",2).toInt();
    m_displayMessageFormat = settings.value("settings/displayMessageFormat",1).toInt();
    m_displayMessageAscii = settings.value("settings/displayMessageAscii",false).toBool();
    m_displayName = settings.value("settings/displayMessageName",true).toBool();
    m_displayRefreshRate = settings.value("settings/displayRefreshRate",500).toInt();
    m_displayDifferences = settings.value("settings/displayDifferences",true).toBool();
}

AppSettings::~AppSettings() {
}

AppSettings*
AppSettings::instance() {
    static AppSettings* singleton = 0;
    if (singleton == 0) {
        singleton = new AppSettings();
    }
    return singleton;
}

void 
AppSettings::enablePCTimestamp(bool enable) {
    m_displayPCTimestamp = enable;
    // Save this in the registry
    QSettings settings;
    settings.setValue("settings/displayPCTimestamp",m_displayPCTimestamp);
}

void 
AppSettings::enableAdapterTimestamp(bool enable) {
    m_displayAdapterTimestamp = enable;
    // Save this in the registry
    QSettings settings;
    settings.setValue("settings/displayAdapterTimestamp",m_displayAdapterTimestamp);
}

void 
AppSettings::setIDFormat(int format) {
    m_displayIDFormat = format;
    // Save this in the registry
    QSettings settings;
    settings.setValue("settings/displayIDFormat",m_displayIDFormat);
}

void 
AppSettings::setdisplayRefreshRate(int rate) {
    m_displayRefreshRate = rate;
    // Save this in the registry
    QSettings settings;
    settings.setValue("settings/displayRefreshRate",m_displayRefreshRate);
}

void 
AppSettings::enableMessageDisplayName(bool enable) {
    m_displayName = enable;
    // Save this in the registry
    QSettings settings;
    settings.setValue("settings/displayMessageName",m_displayName);
}

void 
AppSettings::setMessageFormat(int format) {
    m_displayMessageFormat = format;
    // Save this in the registry
    QSettings settings;
    settings.setValue("settings/displayMessageFormat",m_displayMessageFormat);
}

void 
AppSettings::enableMessageDisplayAscii(bool enable) {
    m_displayMessageAscii = enable;
    // Save this in the registry
    QSettings settings;
    settings.setValue("settings/displayMessageAscii",m_displayMessageAscii);
}

void 
AppSettings::enabledDifferences(bool enable) {
    m_displayDifferences = enable;
    // Save this in the registry
    QSettings settings;
    settings.setValue("settings/displayDifferences",m_displayDifferences);

}

