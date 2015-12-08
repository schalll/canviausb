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

#ifndef CAN_DRIVER_INTERFACE_H
#define CAN_DRIVER_INTERFACE_H

#include <QtPlugin>

QT_BEGIN_NAMESPACE
class QString;
class QWidget;
QT_END_NAMESPACE
class Can_Frame;

//! The CAN driver plugin interface class
/*!
  This is the declaration of the interfaces a driver plugin can use.
  The followinf functions have to be implemented by a plugin in the same exact way they are declared in this interface
*/
class CANviaUSB_DriverInterface
{

public:
    virtual ~CANviaUSB_DriverInterface() {}

    //! Driver identification related functions
    /*!
      The following functions are used to identify and report the current status of the driver
    */

    //! Returns the name of the driver
    virtual const QString name() = 0;

    //! Returns the full name of the driver
    virtual const QString fullName() = 0;

    //! Returns the name of the author of the driver
    virtual const QString author() = 0;

    //! Returns the version of the driver
    virtual const QString version() = 0;

    //! Returns the user interface for this driver
    virtual QWidget *userInterface() = 0;

    //! Returns the status of the driver
    virtual const QString status() = 0;

    //! Returns the CAN baudrate used by this driver
    virtual unsigned int baudrate() = 0;

    //! Driver interface functions
    /*!
      The following functions are used by CANviaUSB to interface with the driver
    */

    //! This function is called by CANviaUSB when the driver is choosen from the list of driver
    virtual void init() = 0;

    //! This function is called by CANviaUSB when the start button is pressed
    virtual void go() = 0;

    //! This function is called by CANviaUSB when the stop button is pressed
    virtual void hold() = 0;

    //! This function is called by CANviaUSB when the driver is unchoosen from the list of driver
    virtual void leave() = 0;

//    /** Board detection related functions */
//    virtual void setReady(bool ready);
//    virtual bool isReady();
//
//    /** Communication related functions */
//    virtual void startCommunication();
//    virtual void stopCommunication();
//
//    virtual void setCommunicating(bool communicating);
//    virtual bool isCommunicating();
//
    //! This function is called by CANviaUSB when the application wants to send a CAN telegram
    virtual void send(const Can_Frame &Frame) = 0;

//signals:
//    virtual void readyStatus(bool ready);
//    virtual void communicationStatus(bool communicating);
//    virtual void baudrateStatus(int baudrate);
//    virtual void infoMessage(QString msg);
//    virtual void sendFrame(const Can_Frame &Frame);
};

QT_BEGIN_NAMESPACE
#define CANviaUSB_DriverInterface_iid "com.canviausb.DriverInterface/1.0"
Q_DECLARE_INTERFACE(CANviaUSB_DriverInterface, CANviaUSB_DriverInterface_iid)
QT_END_NAMESPACE

#endif // CAN_DRIVER_INTERFACE_H
