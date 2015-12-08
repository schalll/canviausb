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

#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QObject>
#include <QDateTime>
#include <QSettings>

class QColor;
class QIcon;

// CAN message structure
struct can_msg {
	unsigned int id;			// Message identifier
	bool extended;				// False for 11bit identifier (CAN 2.0A) - True for 29bit identifier (CAN 2.0B)
	bool rtr;					// True i.e. Remote Transmission Request
	unsigned char dlc;			// Message Length valid values are 0-8 
	unsigned char data[8];		// Body of the message (8 bytes)
	QDateTime timestamp;		// Timestamp associated to the message}
};

enum CanIdFormat { IdDecimal=0, IdHexadecimal, IdBoth };
enum CanMessageFormat { MsgDecimal=0, MsgHexadecimal};

class AppInterface {
public:
    virtual bool displayTimestamp() = 0;		
    virtual CanIdFormat displayIDFormat() = 0;		 
    virtual CanMessageFormat displayMessageFormat() = 0;	
    virtual bool displayMessageAscii() = 0;	
    virtual bool displayMessageName() = 0;	
    virtual unsigned int displayRefreshRate() = 0;

    // Conveniency functions
	virtual QColor getIdColor(unsigned int identifier, bool extended) = 0;		// Returns the color associated to a specific identifier
    virtual QIcon getIdColorIcon(unsigned int identifier, bool extended) = 0;   // Returns an icon representing the color associated to a specific identifierThis can be used instead of getIdColor because of QT bug 8357 http://bugreports.qt.nokia.com/browse/QTBUG-8357 
	virtual QString getIdName(unsigned int identifier, bool extended) = 0;		// Returns the name associated to a specific identifier

//	virtual QAbstractTableModel * communicationModelReadOnly;					// Read only model to access the communication log
//	virtual QAbstractTableModel * identifierModelReadOnly;						// Read only model to access the identifier settings
//	virtual QAbstractTableModel * identifierStatsModelReadonly;					// Read only model to access the identifiers statistics
};

class PluginInterface : public QObject
{
	// Q_OBJECT - DO NOT REMOVE THIS FROM COMMENT - let the subclass handle this
public:
	
	void setAppInterface(AppInterface *app)
		{	m_appInterface=app; 
			initialize();
		}
	virtual ~PluginInterface() {}				
	
	virtual void initialize() = 0;					// Use instead of constructor if you plane on using m_appinterface
	virtual QString name() = 0;						// Returns a string that identify the plugin
	virtual QString author() = 0;					// Returns a string that identify the plugin author
	virtual QString authorEmail() = 0;				// Returns a string that contains the email of the author
	virtual QString version() = 0;					// Returns a string that identify the plugin version
	virtual QString description() = 0;				// Returns a string that describes what the plugin does

	virtual QWidget *ui(QWidget* parent = 0) = 0;	// Returns the widget defining the user interface of the plugin

public slots:
	virtual void receivedFromCan(can_msg msg) = 0;	// Slot is called when a CAN message has been received
	virtual void setCommunicating(bool on) = 0;		// Slot is called when the main application communication is started or stopped

signals:
	virtual void sendOnCan(can_msg const msg) = 0;	// Signal can be used to send a CAN message

protected:
    AppInterface * m_appInterface;
};

Q_DECLARE_INTERFACE(PluginInterface, "canviausb.PluginInterface/1.0");

#endif // PLUGININTERFACE_H
