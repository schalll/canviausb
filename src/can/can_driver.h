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

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#define DEBUG_CAN_GENERIC_ON 0 // 0 to ignore, 1 to display debug messages
#define DEBUG_CAN_GENERIC(x) { if (DEBUG_CAN_GENERIC_ON) qDebug(x); }

// QT includes
#include <QString>
#include <QThread>
#include <QWidget>

// non QT includes
#include "can_frame.h"
#include "CANviaUSB_DriverInterface.h"

// Sample Reference: http://www.google.de/codesearch?hl=de&q=show:b07s2HRe0_g:JQpSMDjCasQ:6BKI0Daz0P0&sa=N&ct=rd&cs_p=http://ftp.debian.org/debian/pool/main/k/kwave/kwave_0.7.2.orig.tar.gz&cs_f=kwave-0.7.2/libkwave/KwavePlugin.h&start=1

/** The abstract CAN driver interface.
 * All CAN drivers should inherit this.
 */

//class CanDriver: public QObject
class Can_Driver: public QThread, public CANviaUSB_DriverInterface
{
    Q_OBJECT

public:
    Can_Driver(QObject * parent = 0);
    ~Can_Driver();

    enum detectionStatus {
        deviceNotConnected, // Default status - we assume that the driver is installed
        deviceConnected,	// Device has been detected as connected
        driverNotInstalled	// Driver has not been detected
    };

    /**
    * Returns the name of the driver
    * Must be reimplemented by it derivates
    */
    const QString name();
    const QString fullName();
    /** Returns the author of the driver */
    const QString author();
    /** Returns the version string of the driver */
    const QString version();
    /** Returns the status of the driver */
    const QString status();
    /** Returns the user interface for this driver */
    QWidget *userInterface();
    /** Returns the CAN baudrate used by this driver */
    unsigned int baudrate();

    /** Threading related functions */
    void run();
    void stop();

    /** Overall state changing related funtions **/
    virtual void init(); // called when the driver is choosen from the list of driver
    virtual void go();   // called when the start button is pressed
    virtual void hold(); // called when the stop button is pressed
    virtual void leave();// called when the driver is unchoosen from the list of driver
    virtual void driver_state_machine();

    /** Board detection related functions */
    void setReady(bool ready);
    bool isReady();

    /** Communication related functions */
    void startCommunication();
    void stopCommunication();

    void setCommunicating(bool communicating);
    bool isCommunicating();

    virtual void send(const Can_Frame &Frame);

signals:
    void readyStatus(bool ready);
    void communicationStatus(bool communicating);
    void baudrateStatus(int baudrate);
    void infoMessage(QString msg);
    void sendFrame(const Can_Frame &Frame);

private slots:
    //void addIdentifierToModel(const Can_Frame &Frame);

protected:

    QString m_name;				// Device name
    QString m_fullName;			// Full name (Manufactured + Device name)
    QString m_version;          // Driver version
    QString m_author;           // Manufacturer information
    QString m_status;           // Driver status
    QWidget *m_userInterface;	// Driver settings GUI
    unsigned int m_baudrate;		// Driver baudrate

    /** Returns the status of the driver */
    detectionStatus m_deviceStatus;
    const detectionStatus deviceStatus();
    bool m_errorDetected;				// To start again

    volatile bool m_stopped;
    volatile bool m_communicate;
    volatile bool m_ready;


};

#endif // CAN_DRIVER_H


