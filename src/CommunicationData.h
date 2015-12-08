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

#ifndef COMMUNICATIONDATA_H
#define COMMUNICATIONDATA_H

// QT includes
#include <QString>

// non QT includes
#include "can/can_frame.h"

struct Info_Message{
    Info_Message();
    Info_Message(QString _text, QDateTime _timestamp);

    QString				text;
    QDateTime			timestamp;
};

struct Error_Message{
    Error_Message();
    Error_Message(QString _text, QDateTime _timestamp);

    QString				text;
    QDateTime			timestamp;
};

// The class CommunicationData is used to defined a variant type of communication data
// The class was defined in order to be able to define every piece of communication 
// that can be treated by the communication model/view architecture

class CommunicationData {
public:
	enum CommunicationDataType {
		VAR_UNDEFINED = 0,
		VAR_CAN_FRAME,
		VAR_INFO_MESSAGE,
		VAR_ERROR_MESSAGE
	};


	CommunicationData();										// Constructor
	~CommunicationData();										// Destructor
	CommunicationData(Can_Frame value);			// Constructs a CommunicationData representing a Can_Frame
	CommunicationData(Info_Message value);	// Constructs a CommunicationData representing an info message
	CommunicationData(Error_Message value);	// Constructs a CommunicationData representing an error message


  CommunicationData::CommunicationDataType type() const;			// Returns the type of this CommunicationData
	Can_Frame frame() const;								// Returns the Can_Frame from this CommunicationData
	Info_Message infoMessage() const;				// Returns the message from this CommunicationData
	Error_Message errorMessage() const;			// Returns the message from this CommunicationData


private:
	CommunicationDataType m_type;
	union {
		Info_Message	*infoMessageVal;
		Error_Message *errorMessageVal;
		Can_Frame			*frameVal;
	};
};

#endif
