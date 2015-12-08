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

#ifndef PREFERENCES_H
#define PREFERENCES_H

// QT includes

// non QT includes

class AppSettings
{

public:
    static AppSettings* instance();
    ~AppSettings();

    bool displayPCTimestamp() const { return m_displayPCTimestamp; }
    bool displayAdapterTimestamp() const { return m_displayAdapterTimestamp; }
    int  displayIDFormat() const { return m_displayIDFormat; }
    int  displayMessageFormat() const { return m_displayMessageFormat; }
    bool displayMessageAscii() const { return m_displayMessageAscii; }
    bool displayMessageName() const { return m_displayName; }
    int  displayRefreshRate() const { return m_displayRefreshRate; }
    bool displayDifferences() const { return m_displayDifferences; }


    void enablePCTimestamp(bool enable);
    void enableAdapterTimestamp(bool enable);
    void setIDFormat(int format);
    void setMessageFormat(int format);
    void enableMessageDisplayAscii(bool enable);
    void enableMessageDisplayName(bool enable);
    void setdisplayRefreshRate(int rate);
	void enabledDifferences(bool enable);

private:
    AppSettings();

    bool m_displayInfos;
    bool m_displayPCTimestamp;
    bool m_displayAdapterTimestamp;
    int  m_displayIDFormat;
    int  m_displayMessageFormat;
    bool m_displayMessageAscii;
    bool m_displayName;
    int m_displayRefreshRate;
	bool m_displayDifferences;
};

#endif
