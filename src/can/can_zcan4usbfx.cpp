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
#include "can_zcan4usbfx.h"
#include "can_frame.h"
#include <stdio.h>

//***************************************************************************
Can_ZCAN4USBFX::Can_ZCAN4USBFX(QObject * parent): Can_Driver(parent)
{
	// TO DO - Replace this value by the one located in the
	m_interfaceNum = 0;

	DEBUG_CAN(" Can_ZCAN4USBFX::Can_ZCAN4USBFX ")
	m_dll = new QLibrary("ZCAN4USBFX");

	// Driver name
	m_name = QString(can4usbfxDriverName);
    m_fullName = QString(can4usbfxFullName);

	// Communication model
	com_model = CommunicationModel::instance();

	// User interface
	m_ui = new Settings_ZCAN4USBFX();
	m_ui->setDeviceName(m_name);
	m_ui->setDeviceStatus(QString("Driver not installed"));
	m_userInterface = m_ui;
	// Make sure user interface is disabled while communicating
	connect(this, SIGNAL(communicationStatus(bool)),m_userInterface, SLOT(communicationStatusChanged(bool)));
	connect(this, SIGNAL(readyStatus(bool)),m_userInterface, SLOT(readyStatusChanged(bool)));

}

//***************************************************************************
Can_ZCAN4USBFX::~Can_ZCAN4USBFX()
{
	delete m_ui;
	delete m_dll;
	//m_ui->destroy();
	m_userInterface = NULL;
	DEBUG_CAN(" Can_ZCAN4USBFX has been deleted !!!")
}

//***************************************************************************
void
Can_ZCAN4USBFX::init() {
	com_model->addInfoMessage(QString("Initializing CAN4USBFX driver"));
	
	// Init all member variables used to interact with the state machine
    m_state = state_init;
	m_start_communication = false;
    m_stop_communication = false;
	m_error_while_sending = false;
	m_packet_to_be_send = false;
	setCommunicating(false);
}

void
Can_ZCAN4USBFX::go() {
	m_start_communication = true;
}

void
Can_ZCAN4USBFX::hold(){
    m_stop_communication = true;
}
void
Can_ZCAN4USBFX::leave(){
	com_model->addInfoMessage(QString("Leaving CAN4USBFX driver"));
}

void
Can_ZCAN4USBFX::driver_state_machine() {
	switch(m_state)
	{
		case state_init:
			if (m_dll->load()) {
				m_ui->setDeviceStatus(QString("Device not connected"));
				setReady(false);
				// NEXT STATE - Wait for Device connected
				m_state = state_waitForDeviceConnected;
				break;
			}
			break;

		case state_waitForDeviceConnected:
			if (info()) {
				m_ui->setDeviceStatus(QString("Device connected"));
				m_ui->setManufacturer(m_author);
				m_ui->setSoftwareVersion(m_version);
				setReady(true);
				// NEXT STATE - Wait for Start command
				m_state = state_waitForStartCommand;
				break;
			}
			break;

		case state_waitForStartCommand:
			// Check if device is still connected
			if (!info()) {
				m_ui->setDeviceStatus(QString("Device not connected"));
				setReady(false);
				msleep(500);
				// NEXT STATE - Close Driver
				m_state = state_closeDriver;
				break;
			}
			// Check if we want to read the registers
			if (m_ui->readRegister()) {
				if (!readRegister()) {
					// Something went wrong
					m_ui->setDeviceStatus(QString("Device not connected"));
					setReady(false);
					// NEXT STATE - Close Driver
					m_state = state_closeDriver;
					break;
				}
			}
			// Check if a start communication command is pending
			if (m_start_communication) {
				m_start_communication = false;
				if (configureDeviceBeforeCommunicating()) {
					// Report that the communication has now started
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
			// Check if there was an error while sending
			if (m_error_while_sending) {
				m_error_while_sending = false;
				// Something went wrong
				com_model->addInfoMessage(QString("Communication has been stopped"));
				setCommunicating(false);
				m_ui->setDeviceStatus(QString("Device not connected"));
				setReady(false);
				// NEXT STATE - Close Driver
				m_state = state_closeDriver;
				break;
			}

			// Check if a stop communication command is pending
			if (m_stop_communication) {
				m_stop_communication = false;
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

		default:
			// NEXT STATE - Init
			m_state = state_init;
	}
	msleep(1);
}


void
Can_ZCAN4USBFX::send(const Can_Frame &Frame){
	if (isCommunicating()) {

		if (!m_packet_to_be_send) {
			SendFrame = Frame;
			m_packet_to_be_send = true;
		}
    }
}

void
Can_ZCAN4USBFX::sendPendingPacket(){

	if (isCommunicating()) {
		QMutexLocker locker(&mutex);
		unsigned char data[8];
		data[0] = SendFrame.data[0];
		data[1] = SendFrame.data[1];
		data[2] = SendFrame.data[2];
		data[3] = SendFrame.data[3];
		data[4] = SendFrame.data[4];
		data[5] = SendFrame.data[5];
		data[6] = SendFrame.data[6];
		data[7] = SendFrame.data[7];		
		//memcpy(data, Frame.data, 8 * sizeof(unsigned char));
		//Resolve the SendCANMess function
		SendCANMess sendCANMess = (SendCANMess) m_dll->resolve("SendCANMess");

		// Reconstruct the id that will be sent to the driver using the ext and rtr field
		unsigned int id;
		id = SendFrame.id;
		if (SendFrame.ext) id = id | 0x80000000;
		if (SendFrame.rtr) id = id | 0x40000000;

		if (sendCANMess) {
			int result = sendCANMess(m_interfaceNum, 0, id, 0, SendFrame.dlc, data); //(unsigned char*)Frame.data);
			// Add this frame to the communication model
			com_model->addFrame(SendFrame);
			// Generic error
			if (result < 0) {
				m_error_while_sending = true;
				reportError(result);
			}
			// Transmission erro
			if (result == 0) {
				com_model->addErrorMessage(QString("ERROR While sending - MCP2515 TXBnCTRL = %1 - MCP2515 TEC = %2)").arg(data[0]).arg(data[1]));
			}
		}
	}
}

bool
Can_ZCAN4USBFX::info(){
	QMutexLocker locker(&mutex);

	bool success = false;

	//Resolve the OpenDriver function
	GetInfo getInfo = (GetInfo) m_dll->resolve("GetInfo");

	if (getInfo) {

		unsigned char tmp_version[3];
		tmp_version[0] = 0;
		tmp_version[1] = 0;
		tmp_version[2] = 0;

		unsigned char tmp_feature[1];
		unsigned char tmp_manufacturer[21];
		unsigned char tmp_controller[10];

		int result = getInfo(m_interfaceNum, 0, tmp_version, tmp_feature, tmp_manufacturer, tmp_controller);

		if (result > 0) {
			m_author = QString((char *)tmp_manufacturer);
			m_version = QString("%1.%2").arg((int)tmp_version[0]).arg((int)tmp_version[1]);

#if DEBUG_CAN_ON
			emit infoMessage(QString("Information about the CAN hardware have been retrieved:"));
			emit infoMessage(QString(" - Version: %1").arg(m_version));
			if (tmp_feature[1] & 0x01) emit infoMessage(QString(" - Time stamping capable device"));
			if (tmp_feature[1] & 0x02) emit infoMessage(QString(" - Receive interrupt capable device"));
			if (tmp_feature[1] & 0x04) emit infoMessage(QString(" - Full duplex capable device"));
#endif

			success = true;
		} else {
			m_author = QString("---");
			m_version = QString("---");
		}
	}
	return success;
}

bool
Can_ZCAN4USBFX::configureDeviceBeforeCommunicating() {
	QMutexLocker locker(&mutex);
	int result;
	// A - Reset the interface
	ResetInterface resetInterface = (ResetInterface) m_dll->resolve("ResetInterface");
	if (resetInterface) {
		result = resetInterface(m_interfaceNum, 0);
		if (result) {
			emit infoMessage(QString("Interface has been resetted"));
		} else {
			com_model->addErrorMessage(QString("Error while resetting the interface"));
			return false;
		}
	} else {
		com_model->addErrorMessage(QString("ERROR while resolving the ResetInterface function from the DLL"));
		return false;
	}

	// B -Set the baudrate
	SetCANBaud setCANBaud = (SetCANBaud) m_dll->resolve("SetCANBaud");
	if (setCANBaud) {
		result = setCANBaud(m_interfaceNum, 0, m_ui->baudrate());
		if (result) {
			m_baudrate = m_ui->baudrate();
			emit baudrateStatus(m_baudrate);	
			emit infoMessage(QString("Baudrate has been set to %1 kbps").arg(m_ui->baudrate()));
		} else {
			com_model->addErrorMessage(QString("Error while setting the baudrate"));
			return false;
		}
	} else {
		com_model->addErrorMessage(QString("ERROR while resolving the SetCANBaud function from the DLL"));
			return false;
	}

	// C - Set the use of the timestamp
	if (m_ui->useTimestamp()) {
		// Switch timestamp on
		EnableTS enableTS = (EnableTS) m_dll->resolve("EnableTS");
		if (enableTS) {
			result = enableTS(m_interfaceNum, 0);
			//TODO - Verify why EnableTS is returning 0
			if (result>=0) {
				emit infoMessage(QString("Timestamp enabled"));
			} else {
				com_model->addErrorMessage(QString("Error while enabling the timestamp"));
				return false;
			}
		} else {
			com_model->addErrorMessage(QString("ERROR while resolving the EnableTS function from the DLL"));
			return false;
		}
	} else {
		// Switch timestamp off
		DisableTS disableTS = (DisableTS) m_dll->resolve("DisableTS");
		if (disableTS) {
			result = disableTS(m_interfaceNum, 0);
			//TODO - Verify why EnableTS is returning 0
			if (result>=0) {
				emit infoMessage(QString("Timestamp disabled"));
			} else {
				com_model->addErrorMessage(QString("Error while disabling the timestamp"));
				return false;
			}
		} else {
			com_model->addErrorMessage(QString("ERROR while resolving the DisableTS function from the DLL"));
			return false;
		}
	}
	// D - Set the acceptance filters
	if (m_ui->useFilter()) {
		unsigned int filter;
		SetFilters setFilters = (SetFilters) m_dll->resolve("SetFilters");
		if (setFilters) {
			
			for (unsigned int num=0; num<6; num++) {
				filter = m_ui->getFilter(num);
				result = setFilters(m_interfaceNum, 0, num, filter);
				if (result<0) {
					com_model->addErrorMessage(QString("Error while setting acceptance filter %1").arg(num));
					return false;
				}
			}
		} else {
				com_model->addErrorMessage(QString("ERROR while resolving the SetFilters function from the DLL"));
				return false;
		}
	}
	return true;
}

bool
Can_ZCAN4USBFX::readRegister() {

	bool success = true;

	QMutexLocker locker(&mutex);
	com_model->addInfoMessage(QString("Read registers content"));	

	//Resolve the Read Register function
	ReadReg readReg = (ReadReg) m_dll->resolve("ReadReg");

	if (readReg) {
		unsigned char addressBase;
		unsigned char data[20];
		int tmp;
		tmp = 0;

		for (addressBase = 0; addressBase < 8 ; addressBase ++) {
			int result = readReg(m_interfaceNum, 0, addressBase*16, 16, data);
			if (result == 16) {
				m_ui->setRegisterValues(addressBase, data);
			}	
		}
	} else {
		success = false;
	}
	return success;
}

bool
Can_ZCAN4USBFX::poll()
{
	unsigned char buffer[16];
	int result;

	bool rtr;
	bool extended;
	unsigned int id;
	unsigned char length;
	unsigned char data[8];


	unsigned char tmp0,tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;

	QMutexLocker locker(&mutex);
	DEBUG_CAN(" Can_ZCAN4USBFX::poll")

	// A - Verify if there is any messages that have been received
	// TODO - Verify how long it takes to resolve the function
	GetCANMess getCANMess = (GetCANMess) m_dll->resolve("GetCANMess");
	if (getCANMess) {
		do {
			result = getCANMess(m_interfaceNum, 0, buffer);
			if (result > 0) {
				// Extended or not ?
				// Pay attention this is bit 7 from CB2 and not bit 31 from CAN ID
				// So I follow the example from the Visual Basic application rather then what the 
				// documentation is describing
				if (buffer[2] & 0x80) {
						 extended = true;  				
				} else {
						 extended = false;
				}

				tmp0 = buffer[0];
				tmp1 = buffer[1];
				tmp2 = buffer[2];
				tmp3 = buffer[3];
				tmp4 = buffer[4];
				tmp5 = buffer[5];
				tmp6 = buffer[6];

				// RTR ?
				if (buffer[2] & 0x40)
					rtr = true;
				else
					rtr = false;

				// CAN Identifier
				if (extended) {
					// Extended CAN id
					id = ((buffer[3] & 0x1F) << 24) + (buffer[4] << 16) + (buffer[5] << 8) + buffer[6];
				} else {
					// Standard CAN id
					id = ((buffer[5] & 0x07) << 8) + buffer[6];
				}

				// DLC/Length
				length = (buffer[1] & 0x0F);

				// Datas
				for (unsigned char i=0; i<length; i++) {
					data[i] = buffer[7+i];
				}

				QTime now = QTime::currentTime();
				//unsigned int time_device = now.hour() * 100000000 + now.minute() * 1000000 + now.second() * 10000 + now.msec();
				Can_Frame frame = Can_Frame(0, QDateTime::currentDateTime(), extended, rtr, id, length, data);
				//emit receiveFrame(frame);
				com_model->addFrame(frame);
			} else {
				if (result < 0) {
					reportError(result);
					return false;
				}
			}
		} while (result);
	} else {
		// TODO Add something here !!!
//		mutex.unlock();
		return false;
	}

	return true;
}

void 
Can_ZCAN4USBFX::reportError(int error) {
	switch(error) {
	case -1:
		com_model->addErrorMessage(QString("ERROR (-1) - Generic failure (No board present)"));
		break;
	case -2:
		com_model->addErrorMessage(QString("ERROR (-2) - No response"));
		break;
	case -3:
		com_model->addErrorMessage(QString("ERROR (-3) - Improper parameter calling"));
		break;
	case -4:
		com_model->addErrorMessage(QString("ERROR (-4) - Improper response"));
		break;
	case -5:
		com_model->addErrorMessage(QString("ERROR (-5) - More than one unit responsed"));
		break;
	case -6:
		com_model->addErrorMessage(QString("ERROR (-6) - System busy"));
		break;
	case -7:
		com_model->addErrorMessage(QString("ERROR (-7) - Bulk write failure"));
		break;
	case -8:
		com_model->addErrorMessage(QString("ERROR (-8) - Bulk read failure"));
		break;
	case -9:
		com_model->addErrorMessage(QString("ERROR (-9) - Interface not responsding"));
		break;
	case -10:
		com_model->addErrorMessage(QString("ERROR (-10) - Interface config file not properly loaded"));
		break;
	case -11:
		com_model->addErrorMessage(QString("ERROR (-11) - Start driver routine failed"));
		break;
	case -12:
		com_model->addErrorMessage(QString("ERROR (-12) - Load driver routine failed"));
		break;
	case -13:
		com_model->addErrorMessage(QString("ERROR (-13) - Device could not be opened"));
		break;
	case -14:
		com_model->addErrorMessage(QString("ERROR (-14) - ISR Device could not be opened"));
		break;
	case -15:
		com_model->addErrorMessage(QString("ERROR (-15) - ISR Thread could not be opened"));
		break;
	case -16:
		com_model->addErrorMessage(QString("ERROR (-16) - Driveralready loaded"));
		break;
	}
}

