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

// non QT includes
#include "can_mhs.h"

//***************************************************************************
Can_MHS::Can_MHS(QObject * parent) : Can_Driver(parent)
{
    qDebug(" Can_MHS::Can_MHS ");

	// See chapter 4.1 from the Tiny-CAN API reference document
	m_index = 0;

	m_dll = new QLibrary("mhstcan");

    // Just for debugging now
	// Driver name
	m_name = QString(mhsDriverName);
    m_fullName = QString(mhsFullName);

	// Communication model
	com_model = CommunicationModel::instance();

	// User interface
    m_ui = new Settings_MHS();
    m_userInterface = m_ui;
	m_ui->setDeviceStatus(QString("---"));

	// Make sure user interface is disabled while communicating
    connect(this, SIGNAL(communicationStatus(bool)),m_userInterface, SLOT(communicationStatusChanged(bool)));
}

//***************************************************************************
Can_MHS::~Can_MHS()
{
    delete m_ui;
	delete m_dll;
    m_userInterface = NULL;
    qDebug(" Can_MHS has been deleted !!!");
}

//***************************************************************************
// called when the driver is choosen from the list of driver
void Can_MHS::init() {
	com_model->addInfoMessage(QString("Initializing MHS driver"));

	setReady(false);

	// Init all member variables used to interact with the state machine
    m_state = state_init;
	m_start_communication = false;
    m_stop_communication = false;
	m_error_while_sending = false;
	m_packet_to_be_send = false;
	setCommunicating(false);
}

// called when the start button is pressed
void Can_MHS::go() {
	m_start_communication = true;
}

// called when the stop button is pressed
void Can_MHS::hold() {
    m_stop_communication = true;
}

// called when the driver is unchoosen from the list of driver
void Can_MHS::leave() {
	com_model->addInfoMessage(QString("Leaving MHS driver"));
}

// driver cyclic function
void Can_MHS::driver_state_machine() {
	switch(m_state)
	{
		case state_init:
			if (m_dll->load()) {
				m_ui->setDeviceStatus(QString("Driver loaded"));
				setReady(false);
				// NEXT STATE - Get Driver Info
				m_state = state_getDriverInfo;
				break;
			} else {
				m_ui->setDeviceStatus(QString("Driver not installed (install driver and restart the application)"));
				// NEXT STATE - Dead end
				m_state = state_dead_end;
			}
			break;

		case state_getDriverInfo:
			if (drvInfo()) {
				m_ui->setDeviceStatus(QString("Driver started"));
				// NEXT STATE - Init Driver
				m_state = state_initDriver;
				break;
			} else {
				m_ui->setDeviceStatus(QString("Driver could not correctly be started (check driver installation and restart the application)"));
				// NEXT STATE - Dead end
				m_state = state_dead_end;
			}
			break;

		case state_initDriver:
			if (initDriver() == 0) {
				m_ui->setDeviceStatus(QString("Device not connected"));
				// NEXT STATE - Wait for device connected
				m_state = state_waitForDeviceConnected;				
			}
			break;

		case state_waitForDeviceConnected:
			if (openDevice()) {
				m_ui->setDeviceStatus(QString("Device connected"));
				// NEXT STATE - Wait for Device Info
				m_state = state_waitForDeviceInfo;
				break;
			}
			break;


		case state_waitForDeviceInfo:
			if (hrwInfo()) {
				QStringList hardwareInfoList = m_hardwareInfo.split(";");
				foreach (QString str, hardwareInfoList) {
					if (str.contains("Version String", Qt::CaseInsensitive)) {
						str.remove(0, 15);
						m_ui->setHardwareDriverInfo(str);
					}
					if (str.contains("Hardware ID String", Qt::CaseInsensitive)) {
						str.remove(0, 19);
						m_ui->setDeviceName(str);
					}
					if (str.contains("Autor", Qt::CaseInsensitive)) {
						str.remove(0, 6);
						m_ui->setManufacturer(str);
					}
				}
				setReady(true);
				// NEXT STATE - Wait for Start command
				m_state = state_waitForStartCommand;
				break;
			}
			break;

		case state_waitForStartCommand:
			// Check if device is still connected
//			if (!info()) {
//				m_ui->setDeviceStatus(QString("Device not connected"));
//				setReady(false);
//				msleep(500);
//				// NEXT STATE - Close Driver
//				m_state = state_closeDriver;
//				break;
//			}
//			// Check if we want to read the registers
//			if (m_ui->readRegister()) {
//				if (!readRegister()) {
//					// Something went wrong
//					m_ui->setDeviceStatus(QString("Device not connected"));
//					setReady(false);
//					// NEXT STATE - Close Driver
//					m_state = state_closeDriver;
//					break;
//				}
//			}
//			// Check if a start communication command is pending
			if (m_start_communication) {
				m_start_communication = false;
				if (configureDeviceBeforeCommunicating()) {
//					// Report that the communication has now started
					com_model->addInfoMessage(QString("Communicating @ %1 kpbs").arg(m_baudrate));
					setCommunicating(true);
					// NEXT STATE - Wait for Stop Command
					m_state = state_waitForStopCommand;
				} else {
					m_ui->setDeviceStatus(QString("Device not connected"));
					setReady(false);
					// NEXT STATE - Close Driver
					m_state = state_closeDriver;
				}
				break;
			}
			break;

		case state_waitForStopCommand:
//			// Check if there was an error while sending
//			if (m_error_while_sending) {
//				m_error_while_sending = false;
//				// Something went wrong
//				com_model->addInfoMessage(QString("Communication has been stopped"));
//				setCommunicating(false);
//				m_ui->setDeviceStatus(QString("Device not connected"));
//				setReady(false);
//				// NEXT STATE - Close Driver
//				m_state = state_closeDriver;
//				break;
//			}

			// Check if a stop communication command is pending
			if (m_stop_communication) {
				m_stop_communication = false;
				configureDeviceAfterCommunicating();
				com_model->addInfoMessage(QString("Communication has been stopped"));
				setCommunicating(false);
				// NEXT STATE - Wait for Start Command
				m_state = state_waitForStartCommand;
				break;
			}

			// Poll for incoming packets
			if (!poll()) {
				// Something went wrong
				com_model->addInfoMessage(QString("Communication has been stopped"));
				setCommunicating(false);
				m_ui->setDeviceStatus(QString("Device not connected"));
				configureDeviceAfterCommunicating();
				setReady(false);
				// NEXT STATE - Close Driver
				m_state = state_closeDriver;
				break;
			}

			// Send pending outgoing packet
			if (m_packet_to_be_send) {
		        sendPendingPacket();
				m_packet_to_be_send = false;
			}
			break;

		case state_closeDriver:
			
			m_dll->unload();
			msleep(500);
			// NEXT STATE - Init
			m_state = state_init;
			break;

		case state_dead_end:
			break;

		default:
			// NEXT STATE - Init
			m_state = state_init;
	}
	msleep(1);
}

// driver send function
void Can_MHS::send(const Can_Frame &Frame) {
	if (isCommunicating()) {

		if (!m_packet_to_be_send) {
			SendFrame = Frame;
			m_packet_to_be_send = true;
		}
    }
}

int  
Can_MHS::initDriver() {

	//Resolve the CanInitDriver function
	CanInitDriver canInitDriver = (CanInitDriver) m_dll->resolve("CanInitDriver");

	int result = -1;
	if (canInitDriver) {
		result = canInitDriver(NULL);
	}
	return result;

}


bool  
Can_MHS::openDevice() {

	//Resolve the CanDeviceOpen function
	CanDeviceOpen canDeviceOpen = (CanDeviceOpen) m_dll->resolve("CanDeviceOpen");

	int result = -1;
	if (canDeviceOpen) {
		result = canDeviceOpen(0, NULL);
	}
	if (result == 0) 
		return true;
	else 
		return false;

}

bool 
Can_MHS::drvInfo() {
	QMutexLocker locker(&mutex);

	bool success = false;

	//Resolve the OpenDriver function
	CanDrvInfo canDrvInfo = (CanDrvInfo) m_dll->resolve("CanDrvInfo");

	if (canDrvInfo) {
		m_driverInfo = QString(canDrvInfo());
		success = true;
	}
	return success;
}

bool
Can_MHS::hrwInfo(){
	QMutexLocker locker(&mutex);

	bool success = false;

	//Resolve the CanDrvHwInfo function
	CanDrvHwInfo canDrvHwInfo = (CanDrvHwInfo) m_dll->resolve("CanDrvHwInfo");

	if (canDrvHwInfo) {
		m_hardwareInfo = QString(canDrvHwInfo(m_index));
		if (m_hardwareInfo != QString(""))
			success = true;
	}
	return success;
}

bool
Can_MHS::configureDeviceBeforeCommunicating() {
	QMutexLocker locker(&mutex);
	int result;

	// A - Set the baudrate
	CanSetSpeed canSetSpeed = (CanSetSpeed) m_dll->resolve("CanSetSpeed");
	if (canSetSpeed) {
		result = canSetSpeed(m_index, m_ui->baudrate());
		if (result == 0) { // 0 means everything went fine
			m_baudrate = m_ui->baudrate();
			emit baudrateStatus(m_baudrate);	
			emit infoMessage(QString("Baudrate has been set to %1 kbps").arg(m_ui->baudrate()));
		} else {
			com_model->addErrorMessage(QString("Error while setting the baudrate"));
			return false;
		}
	} else {
			com_model->addErrorMessage(QString("ERROR while resolving the CanSetSpeed function from the DLL"));
			return false;
	}

	// B - CAN Bus Start
	CanSetMode canSetMode = (CanSetMode) m_dll->resolve("CanSetMode");
	if (canSetMode) {
		result = canSetMode(m_index, OP_CAN_START, CAN_CMD_ALL_CLEAR);
		if (result == 0) { // 0 means everything went fine
			// 
		} else {
			com_model->addErrorMessage(QString("Error while starting the CAN chip"));
			return false;
		}
	} else {
			com_model->addErrorMessage(QString("ERROR while resolving the CanSetMode function from the DLL"));
			return false;
	}

	// C - Empty the receive fifo
	CanReceiveClear canReceiveClear = (CanReceiveClear) m_dll->resolve("CanReceiveClear");
	if (canReceiveClear) {
		canReceiveClear(m_index);
	} else {
			com_model->addErrorMessage(QString("ERROR while resolving the CanReceiveClear function from the DLL"));
			return false;
	}

	return true;
}

bool
Can_MHS::configureDeviceAfterCommunicating() {
	QMutexLocker locker(&mutex);
	int result;

	// B - CAN Bus Start
	CanSetMode canSetMode = (CanSetMode) m_dll->resolve("CanSetMode");
	if (canSetMode) {
		result = canSetMode(m_index, OP_CAN_STOP, CAN_CMD_ALL_CLEAR);
		if (result == 0) { // 0 means everything went fine
			// 
		} else {
			com_model->addErrorMessage(QString("Error while starting the CAN chip"));
			return false;
		}
	} else {
			com_model->addErrorMessage(QString("ERROR while resolving the CanSetMode function from the DLL"));
			return false;
	}

	return true;
}


bool
Can_MHS::poll()
{

	bool rtr;
	bool extended;
	unsigned int id;
	unsigned char length;
	unsigned char data[8];
	
	// A - Verify the status of the device
	// TODO - Verify how long it takes to resolve the function
	CanGetDeviceStatus canGetDeviceStatus = (CanGetDeviceStatus) m_dll->resolve("CanGetDeviceStatus");
	if (canGetDeviceStatus) {
		struct TDeviceStatus status;
		static TDeviceStatus old_status;

		canGetDeviceStatus(m_index, &status);
		if (status.DrvStatus <= DRV_STATUS_CAN_OPEN)
			return false;
	
		if (old_status.CanStatus != status.CanStatus) {
			switch(status.CanStatus) {
				case CAN_STATUS_OK:
					break;
				case CAN_STATUS_ERROR:				// CAN-Controller: CAN Error
					com_model->addErrorMessage(QString("CAN-Controller: CAN Error"));
					break;
				case CAN_STATUS_WARNING:			// CAN-Controller: Error warning
					com_model->addErrorMessage(QString("CAN-Controller: Error warning"));
					break;
				case CAN_STATUS_PASSIV:				// CAN-Controller: Error passiv
					com_model->addErrorMessage(QString("CAN-Controller: Error passiv"));
					break;
				case CAN_STATUS_BUS_OFF:			// CAN-Controller: Bus Off
					com_model->addErrorMessage(QString("CAN-Controller: Bus Off"));
					break;
				case CAN_STATUS_UNBEKANNT:			// CAN-Controller: Status Unbekannt
					break;
			}
			old_status = status;
		}
	
	} else {
		com_model->addErrorMessage(QString("ERROR while resolving the CanReceive function from the DLL"));
		return false;
	}

    // B - Verify if there is any messages that have been received
	// TODO - Verify how long it takes to resolve the function
	CanReceive canReceive = (CanReceive) m_dll->resolve("CanReceive");
	if (canReceive) {
		struct TCanMsg msg;
		if (canReceive(m_index, &msg, 1) > 0) {
	
			rtr = msg.Flags.Flag.RTR;
			extended = msg.Flags.Flag.EFF;
			id = msg.Id;
			length = msg.Flags.Flag.Len;
			
			// Datas
			for (unsigned char i=0; i<length; i++) {
				data[i] = msg.Data.Bytes[i];
			}

			QTime now = QTime::currentTime();
			Can_Frame frame = Can_Frame(0, QDateTime::currentDateTime(), extended, rtr, id, length, data);
			com_model->addFrame(frame);
		}
	} else {
		com_model->addErrorMessage(QString("ERROR while resolving the CanReceive function from the DLL"));
		return false;
	}

	return true;
}


void
Can_MHS::sendPendingPacket(){
	int result;
	struct TCanMsg msg;
	
	if (isCommunicating()) {
		QMutexLocker locker(&mutex);

		// Datas
		for (unsigned char i=0; i<8; i++) {
			msg.Data.Bytes[i] = SendFrame.data[i];
		}
		// ID
		unsigned int id;
		msg.Id = SendFrame.id;
		// 		
		if (SendFrame.ext)
			msg.Flags.Flag.EFF = 1;
		else
			msg.Flags.Flag.EFF = 0;

		if (SendFrame.rtr) 
			msg.Flags.Flag.RTR = 1;
		else
			msg.Flags.Flag.RTR = 0;
		

		// DLC
		msg.Flags.Flag.Len = SendFrame.dlc;

		// A - CAN Bus Transmit
		CanTransmit canTransmit = (CanTransmit) m_dll->resolve("CanTransmit");
		if (canTransmit) {
			result = canTransmit(m_index, &msg, 1);
			if (result == 0) { // 0 means everything went fine
				// Add this frame to the communication model
				com_model->addFrame(SendFrame);
			} else {
				com_model->addErrorMessage(QString("Error while sending the message"));
			}
		} else {
				com_model->addErrorMessage(QString("ERROR while resolving the CanTransmit function from the DLL"));
		}
	}
}
