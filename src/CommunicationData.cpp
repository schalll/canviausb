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
#include "CommunicationData.h"

Info_Message::Info_Message()  {
}

Info_Message::Info_Message(QString _text, QDateTime _timestamp)
        : text(_text), timestamp(_timestamp) {
}

Error_Message::Error_Message()  {
}

Error_Message::Error_Message(QString _text, QDateTime _timestamp)
        : text(_text), timestamp(_timestamp) {
}

CommunicationData::CommunicationData() {
	m_type = VAR_UNDEFINED;
}

CommunicationData::~CommunicationData() {
//	if (m_type == VAR_INFO_MESSAGE) {
//		if (infoMessageVal) 
//			delete infoMessageVal;
//	}
	//if (errorMessageVal) delete errorMessageVal;
	//if (frameVal) delete frameVal;
}

CommunicationData::CommunicationData(Can_Frame value) {
	m_type = VAR_CAN_FRAME;
	frameVal = new Can_Frame(value);
}

CommunicationData::CommunicationData(Info_Message value) {
	m_type = VAR_INFO_MESSAGE;
	infoMessageVal = new Info_Message(value);
}

CommunicationData::CommunicationData(Error_Message value) {
	m_type = VAR_ERROR_MESSAGE;
	errorMessageVal = new Error_Message(value);
}

CommunicationData::CommunicationDataType 
CommunicationData::type() const {
	return m_type;
}

Can_Frame 
CommunicationData::frame() const {
	if (m_type == VAR_CAN_FRAME)
		return *frameVal;
	else
		return Can_Frame();
}


Info_Message 
CommunicationData::infoMessage() const {
	if (m_type == VAR_INFO_MESSAGE)
		return *infoMessageVal;
	else
		return Info_Message();
} 

Error_Message
CommunicationData::errorMessage() const {
	if (m_type == VAR_ERROR_MESSAGE)
		return *errorMessageVal;
	else
		return Error_Message();
}
