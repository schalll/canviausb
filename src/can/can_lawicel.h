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

#ifndef CAN_LAWICEL_H
#define CAN_LAWICEL_H

#define DEBUG_LAWICEL_CAN_ON 1 // 0 to ignore, 1 to display debug messages
#define DEBUG_LAWICEL_CAN(x) { if (DEBUG_LAWICEL_CAN_ON) qDebug(x); }

// QT includes
#include <QLibrary>
#include <QMutex>

// Non QT includes
#include "settings_lawicel.h" // user interface
#include "can_driver.h"
#include "../utilities/ringbuffer.h"
#include "../CommunicationModel.h"
#include <windows.h> 
#include "ftd2xx.h"
// Next line is let the linker now which libraries this program is using - found at http://www.learncpp.com/cpp-tutorial/a2-using-libraries-with-visual-studio-2005-express/
// (D) See also Visual Studio settings: Extras -> Optionen -> Projekt un Projetmappen -> Vc++ Verzeichnisse -> Win32 -> Bibliothekdateien -> C:\cygwin\home\TestBuild\canmonitor\trunk\apps\libs\ftd2xx
// (E) See also Visual Studio settings: Tools -> Options -> Projects and Solutions -> VC++ Directoriers -> Win32 -> Library files -> C:\cygwin\home\TestBuild\canmonitor\trunk\apps\libs\ftd2xx
#pragma comment(lib, "ftd2xx.lib")

#define lawicelDriverName "CANUSB"
#define lawicelManufacturerName "Lawicel"
#define lawicelFullName "Lawicel CANUSB"

/** Concrete class for the CAN4USB Module from Zanthic technology */
class Can_Lawicel: public Can_Driver
{
    Q_OBJECT

public:
    Can_Lawicel(QObject * parent = 0);
    ~Can_Lawicel();

    void init();						// called when the driver is choosen from the list of driver
    void go();							// called when the start button is pressed
    void hold();						// called when the stop button is pressed
    void leave();						// called when the driver is unchoosen from the list of driver
    void driver_state_machine();		// driver cyclic function
    void send(const Can_Frame &Frame);	// driver send function

signals:
    void driverVersionReceived(QString Version);
    void okReceived();
    void errorReceived();
    void serialNumberReceived(QString SerialNumber);
    void versionReceived(QString Version);
    void canFrameReceived(Can_Frame Frame);

private slots:
    void updateDriverVersionInGUI(QString Version);
    void updateSerialNumberInGUI(QString SerialNumber);
    void updateVersionInGUI(QString Version);
    void publishCanFrameToGUI(Can_Frame Frame);

private:

    // Local definition of the user interface
    Settings_Lawicel* m_ui;

    CommunicationModel* com_model;

    mutable QMutex mutex;
    mutable QMutex dsm_mutex;
    mutable QMutex publishing_mutex;

    RingBuffer m_inputBuffer;

    // Driver specific implementations
    bool isDriverInstalled();
    bool isDeviceConnected();

    bool openDevice();
    void closeDevice();
    bool emptyPriorCommand();				// CR,CR,CR command
    bool askForDeviceVersion();				// V command
    bool askForSerialNumber();				// N command
    bool setDeviceBaudrate(int baudrate);	// Sn,(Sxxyy) command
    bool openCANChannel();					// O command
    bool closeCANChannel();					// C command
    bool sendPendingPacket();				// t,T commands
    void pollForDatas();


    // State machine related definitions
    enum Can_Lawicel_states
    {
        state_init,
        state_waitForDriverInstalled,
        state_waitForDeviceConnected,
        state_openDevice,
        state_emptyPriorCommand,
        state_requestDeviceVersion,
        state_waitForDeviceVersion,
        state_requestSerialNumber,
        state_waitForSerialNumber,
        state_waitForStartCommand,
        state_waitForStopCommand,
        state_closeDriver,
        state_closeDevice
    };
    Can_Lawicel_states m_state;

    enum Can_Lawicel_reception_states
    {
        state_waitForReplyId,
        state_waitForReplyId_N,
        state_waitForReplyId_t,
        state_waitForReplyId_T,
        state_waitForReplyId_r,
        state_waitForReplyId_R,
        state_waitForReplyFrame_datas, // for r,R,t or T command
        state_waitForReplyFrame_timestamp, // for r,R,t or T command
        state_waitForReplyId_V
    };
    Can_Lawicel_reception_states m_reception_state;

    // Member variables used to interact with the state machine
    bool m_start_communication;
    bool m_stop_communication;
    bool m_packet_to_be_send;

    bool m_displayDriverVersion;
    bool m_deviceOpen;
    bool m_deviceSerialNumberReceived;
    bool m_deviceVersionReceived;

    // Send Frame buffer
    Can_Frame SendFrame;

    // Functions to access the Library from Future Technology
    FT_STATUS ft_status;
    FT_HANDLE ft_handle;
    char device_to_open[64]; // This is going to containt the serial number to identifiy the define that will need to be opened
    FT_STATUS writeDatas(char *data, int data_size);
    FT_STATUS readDatas();
    FT_STATUS purgeBuffer();
    bool readSingleData(char *data);

    QString m_lastCommandForDebug;

    QTime delayTimer;
};

#endif // CAN_LAWICEL_H


