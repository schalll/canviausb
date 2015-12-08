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
#include "can_driver.h"

// QList with class sample @ http://www.phys.uu.nl/DU/QT/qlist.html

/*
*  Generic can driver implementation
*
*  The thread is suppose to start as soon as the driver is selected
*	The poll function is only supposed to be called when the user pushes the start button
*
*/

//***************************************************************************
Can_Driver::Can_Driver(QObject * parent) : QThread(parent) {
	qDebug(" Can_Driver::Can_Driver ");
	m_status = Can_Driver::deviceNotConnected;
	m_userInterface = 0;
	m_stopped = false;
	m_communicate = false;
	m_name = "";
	m_baudrate = 0;

	//connect(this, SIGNAL(receiveFrame(const Can_Frame &)),this, SLOT(addIdentifierToModel(const Can_Frame &)));
	//connect(this, SIGNAL(sendFrame(const Can_Frame &)),this, SLOT(addIdentifierToModel(const Can_Frame &)));
	
	m_errorDetected = false;
}

//***************************************************************************
Can_Driver::~Can_Driver() {
	if (m_userInterface) delete m_userInterface;
}

//***************************************************************************
const QString 
Can_Driver::name() {
	return m_name;
}

//***************************************************************************
const QString 
Can_Driver::fullName() {
	return m_fullName;
}

//***************************************************************************
const QString 
Can_Driver::version() {
	return m_version;
}

//***************************************************************************
const QString 
Can_Driver::author() {
	return m_author;
}

//***************************************************************************
const QString 
Can_Driver::status() {
	return m_status;
}

//***************************************************************************
QWidget *
Can_Driver::userInterface() {
	return m_userInterface;
}

//***************************************************************************
unsigned int 
Can_Driver::baudrate() {
	return m_baudrate;
}

//***************************************************************************
const Can_Driver::detectionStatus 
Can_Driver::deviceStatus() {
	return m_deviceStatus;
}

//***************************************************************************
// Start/Stop thread
void
Can_Driver::run() {

	// This function is executed before the driver is going to it cyclic execution
	DEBUG_CAN_GENERIC(" Can_Driver::run is now calling init() from selected driver")
	init(); 
	m_stopped = false;
	do {
		// This function is defining the driver cyclic execution
		driver_state_machine();
	} while (!m_stopped);

	DEBUG_CAN_GENERIC(" Can_Driver::run is now calling leave() from selected driver")
	// This function is executed after the driver is going out from it cyclic execution
	leave(); 
}

void
Can_Driver::stop() {
	m_stopped = true;
}


void
Can_Driver::init() {
	// Put in here what ever the driver has to do before it thread is starting
	setReady(false);
}

void
Can_Driver::go() {
	// Put in here what ever the driver has to do when the start button is called
}

void
Can_Driver::hold() {
	// Put in here what ever the driver has to do when the stop button is called
}

void
Can_Driver::leave() {
	// Put in here what ever the driver has to do after it thread is finished

}

void
Can_Driver::driver_state_machine() {
}

//**************************************************************************
bool 
Can_Driver::isReady() {
	return m_ready;
}

void 
Can_Driver::setReady(bool ready) {
	m_ready = ready;
	if (!ready) {
		if (m_communicate) stopCommunication();
	}
	emit readyStatus(m_ready);
}

//***************************************************************************
// Start/Stop communication
void 
Can_Driver::startCommunication(){
	go();
}

void 
Can_Driver::stopCommunication(){
	hold();
}

void 
Can_Driver::setCommunicating(bool communicating) {
	m_communicate = communicating;
	emit communicationStatus(m_communicate);
}

bool 
Can_Driver::isCommunicating() {
	return m_communicate;
}

void 
Can_Driver::send(const Can_Frame &Frame) {
	emit infoMessage(QString("Can_Driver::Send a frame"));
	Q_UNUSED(Frame);
	//addIdentifierToModel(Frame);
}
