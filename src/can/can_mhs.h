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

#ifndef CAN_MHS_H
#define CAN_MHS_H


// QT includes
#include <QLibrary>

// Non QT includes
#include "can_driver.h"
#include "settings_mhs.h" // user interface
#include "../CommunicationModel.h"

#define mhsDriverName "MHS Tiny-CAN"
#define mhsFullName "MHS Tiny-CAN"

// CAN Bus Mode
#define OP_CAN_NO_CHANGE         0  // Aktuellen Zustand nicht ändern
#define OP_CAN_START             1  // Startet den CAN-Bus
#define OP_CAN_STOP              2  // Stopt den CAN-Bus
#define OP_CAN_RESET             3  // Reset CAN Controller (BusOff löschen)
#define OP_CAN_START_LOM         4  // Startet den CAN-Bus im Silent Mode (Listen Only Mode)
#define OP_CAN_START_NO_RETRANS  5  // Startet den CAN-Bus im Automatic Retransmission disable Mode


#define CAN_CMD_NONE                0x0000
#define CAN_CMD_RXD_OVERRUN_CLEAR   0x0001
#define CAN_CMD_RXD_FIFOS_CLEAR     0x0002
#define CAN_CMD_TXD_OVERRUN_CLEAR   0x0004
#define CAN_CMD_TXD_FIFOS_CLEAR     0x0008
#define CAN_CMD_HW_FILTER_CLEAR     0x0010
#define CAN_CMD_SW_FILTER_CLEAR     0x0020
#define CAN_CMD_TXD_PUFFERS_CLEAR   0x0040

#define CAN_CMD_ALL_CLEAR           0x0FFF

// DrvStatus
#define DRV_NOT_LOAD              0  // Die Treiber DLL wurde noch nicht geladen
#define DRV_STATUS_NOT_INIT       1  // Treiber noch nicht Initialisiert
#define DRV_STATUS_INIT           2  // Treiber erfolgrich Initialisiert
#define DRV_STATUS_PORT_NOT_OPEN  3  // Die Schnittstelle wurde geöffnet
#define DRV_STATUS_PORT_OPEN      4  // Die Schnittstelle wurde nicht geöffnet
#define DRV_STATUS_DEVICE_FOUND   5  // Verbindung zur Hardware wurde Hergestellt
#define DRV_STATUS_CAN_OPEN       6  // Device wurde geöffnet und erfolgreich Initialisiert
#define DRV_STATUS_CAN_RUN_TX     7  // CAN Bus RUN nur Transmitter (wird nicht verwendet !)
#define DRV_STATUS_CAN_RUN        8  // CAN Bus RUN

// CanStatus
#define CAN_STATUS_OK          0     // CAN-Controller: Ok
#define CAN_STATUS_ERROR       1     // CAN-Controller: CAN Error
#define CAN_STATUS_WARNING     2     // CAN-Controller: Error warning
#define CAN_STATUS_PASSIV      3     // CAN-Controller: Error passiv
#define CAN_STATUS_BUS_OFF     4     // CAN-Controller: Bus Off
#define CAN_STATUS_UNBEKANNT   5     // CAN-Controller: Status Unbekannt

// Fifo Status
#define FIFO_OK                 0 // Fifo-Status: Ok
#define FIFO_HW_OVERRUN         1 // Fifo-Status: Hardware Fifo Überlauf
#define FIFO_SW_OVERRUN         2 // Fifo-Status: Software Fifo Überlauf
#define FIFO_HW_SW_OVERRUN      3 // Fifo-Status: Hardware & Software Fifo Überlauf
#define FIFO_STATUS_UNBEKANNT   4 // Fifo-Status: Unbekannt

// Makros für SetEvent
#define EVENT_ENABLE_PNP_CHANGE          0x0001
#define EVENT_ENABLE_STATUS_CHANGE       0x0002
#define EVENT_ENABLE_RX_FILTER_MESSAGES  0x0004
#define EVENT_ENABLE_RX_MESSAGES         0x0008
#define EVENT_ENABLE_ALL                 0x00FF

#define EVENT_DISABLE_PNP_CHANGE         0x0100
#define EVENT_DISABLE_STATUS_CHANGE      0x0200
#define EVENT_DISABLE_RX_FILTER_MESSAGES 0x0400
#define EVENT_DISABLE_RX_MESSAGES        0x0800
#define EVENT_DISABLE_ALL                0xFF00

struct TDeviceStatus
  {
  __int32 DrvStatus;
  unsigned char CanStatus;
  unsigned char FifoStatus;
  };

/******************************************/
/*            CAN Message Type            */
/******************************************/
#define MsgFlags Flags.Long
#define MsgLen Flags.Flag.Len
#define MsgRTR Flags.Flag.RTR
#define MsgEFF Flags.Flag.EFF
#define MsgTxD Flags.Flag.TxD
#define MsgData Data.Bytes

struct TCanFlagsBits
  {
  unsigned Len:4;   // Dlc
  unsigned TxD:1;
  unsigned Res:1;   // Reserviert
  unsigned RTR:1;   // remote transmition request bit
  unsigned EFF:1;   // extended frame bit
  unsigned Res2:8;
  };

union TCanFlags
  {
  struct TCanFlagsBits Flag;
  unsigned long Long;
  };

union TCanData
  {
  char Chars[8];
  unsigned char Bytes[8];
  unsigned short Words[4];
  unsigned long Longs[2];
  };

struct TTime
  {
  unsigned long Sec;
  unsigned long USec;
  };

struct TCanMsg
  {
  unsigned long Id;
  union TCanFlags Flags;
  union TCanData Data;
  struct TTime Time;
  };


// These are the functions available in the MHS DLL we are using
// Note that the calling convention __stdcall need to specified (see http://www.unixwiz.net/techtips/win32-callconv.html for some details)
typedef char *(__stdcall *CanDrvInfo)(void);
typedef int   (__stdcall *CanInitDriver)(char *options);
typedef int   (__stdcall *CanDeviceOpen)(unsigned long index, char *parameter);
typedef char *(__stdcall *CanDrvHwInfo)(unsigned long index);
typedef int   (__stdcall *CanSetSpeed)(unsigned long index, unsigned short speed);
typedef int   (__stdcall *CanSetMode)(unsigned long index, unsigned char can_op_mode, unsigned short can_command);
typedef void  (__stdcall *CanReceiveClear)(unsigned long index);
typedef int   (__stdcall *CanGetDeviceStatus)(unsigned long index, struct TDeviceStatus *status);
typedef int   (__stdcall *CanReceive)(unsigned long index, struct TCanMsg *msg, int count);
typedef int   (__stdcall *CanTransmit)(unsigned long index, struct TCanMsg *msg, int count);

/** Concrete class for the CAN CPCUSB Module from EMS Thomas Wnsche */
class Can_MHS: public Can_Driver
{
    Q_OBJECT

public:
    Can_MHS(QObject * parent = 0);
    ~Can_MHS();

    void init();						// called when the driver is choosen from the list of driver
    void go();							// called when the start button is pressed
    void hold();						// called when the stop button is pressed
    void leave();						// called when the driver is unchoosen from the list of driver
    void driver_state_machine();		// driver cyclic function
    void send(const Can_Frame &Frame);	// driver send function

private:
    // Local definition of the user interface
    Settings_MHS* m_ui;

    // m_index is defined in chapter 4.1 from the Tiny-CAN API Document - we leave this value to 0 for the moment
    unsigned long m_index;

    QLibrary *m_dll;

	// Reference to the models used in the application
    CommunicationModel* com_model;

    mutable QMutex mutex;

	// Driver specific implementations
	bool drvInfo();
	int  initDriver();
	bool openDevice();
	bool hrwInfo();
	bool configureDeviceBeforeCommunicating();
	bool poll();
	bool configureDeviceAfterCommunicating();
	void sendPendingPacket();

	// State machine related definitions
	enum Can_MHS_states
	{
		state_init,
		state_getDriverInfo,
		state_initDriver,
		state_waitForDeviceConnected,
		state_waitForDeviceInfo,
		state_waitForStartCommand,
		state_waitForStopCommand,
		state_closeDriver,
		state_dead_end
	};
	Can_MHS_states m_state;

	// Member variables used to interact with the state machine
    bool m_start_communication;
    bool m_stop_communication;
    bool m_error_while_sending;
	bool m_packet_to_be_send;

	QString m_driverInfo;
	QString m_hardwareInfo;

	// Send Frame buffer
	Can_Frame SendFrame;

};

#endif // CAN_MHS_H
