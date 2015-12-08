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

#ifndef CAN_ZCAN4USBFX_H
#define CAN_ZCAN4USBFX_H

#define DEBUG_CAN_ON 0 // 0 to ignore, 1 to display debug messages
#define DEBUG_CAN(x) { if (DEBUG_CAN_ON) qDebug(x); }

// QT includes
#include <QLibrary>
#include <QMutex>

// Non QT includes
#include "settings_zcan4usbfx.h" // user interface
#include "can_driver.h"
#include "../CommunicationModel.h"

#define can4usbfxDriverName "CAN-4-USB/FX"
#define can4usbfxFullName "Zanthic CAN-4-USB/FX"

// These are the functions available in the Zanthac DLL
// Note that the calling convention __stdcall need to specified (see http://www.unixwiz.net/techtips/win32-callconv.html for some details)
typedef short int (__stdcall *SetCANBaud)(unsigned char,unsigned char,short int);
typedef short int (__stdcall *ResetInterface)(unsigned char,unsigned char);
typedef short int (__stdcall *SendCANMess)(unsigned char,unsigned char,unsigned int,unsigned char,unsigned char,unsigned char*);
//RecCANMess - To be declared
typedef short int (__stdcall *GetCANMess)(unsigned char,unsigned char,unsigned char *);
typedef short int (__stdcall *ReadReg)(unsigned char,unsigned char,unsigned char,unsigned char,unsigned char *);
//WriteReg - To be declared
//GetNumCANMess - To be declared
//ClearCANMess - To be declared
typedef short int (__stdcall *GetInfo)(unsigned char,unsigned char,unsigned char *,unsigned char *,unsigned char *,unsigned char *);
typedef short int (__stdcall *SetFilters)(unsigned char,unsigned char,unsigned char,unsigned int);
typedef short int (__stdcall *EnableTS)(unsigned char,unsigned char);
typedef short int (__stdcall *DisableTS)(unsigned char,unsigned char);

/** Concrete class for the CAN4USB Module from Zanthic technology */
class Can_ZCAN4USBFX: public Can_Driver
{
    Q_OBJECT

public:
    Can_ZCAN4USBFX(QObject * parent = 0);
    ~Can_ZCAN4USBFX();

    void init();						// called when the driver is choosen from the list of driver
    void go();							// called when the start button is pressed
    void hold();						// called when the stop button is pressed
    void leave();						// called when the driver is unchoosen from the list of driver
    void driver_state_machine();		// driver cyclic function
    void send(const Can_Frame &Frame);	// driver send function

private:
    QLibrary *m_dll;

    // Local definition of the user interface
    Settings_ZCAN4USBFX* m_ui;

    // m_interfaceNum is a value between 0 and 9 depending on
    // how many CAN4USBFX devices are connected to the same computer
    unsigned char m_interfaceNum;

    // Reference to the models used in the application
    CommunicationModel* com_model;

    mutable QMutex mutex;

    // Driver specific implementations
    bool info();
    bool configureDeviceBeforeCommunicating();
    bool readRegister();
    bool poll();
    void sendPendingPacket();
    void reportError(int error);

    // State machine related definitions
    enum Can_ZCAN4USBFX_states
    {
        state_init,
        state_waitForDeviceConnected,
        state_waitForStartCommand,
        state_waitForStopCommand,
        state_closeDriver
    };
    Can_ZCAN4USBFX_states m_state;

    // Member variables used to interact with the state machine
    bool m_start_communication;
    bool m_stop_communication;
    bool m_error_while_sending;
    bool m_packet_to_be_send;

    // Send Frame buffer
    Can_Frame SendFrame;

};

#endif // CAN_ZCAN4USBFX_H


