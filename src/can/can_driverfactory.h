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

#ifndef CAN_DRIVERFACTORY_H
#define CAN_DRIVERFACTORY_H

// QT includes
#include <QtCore>

// Non QT includes
#include "can_driver.h"
#include "can_demo.h"

#include "can_zcan4usbfx.h"
#include "can_lawicel.h"
#include "can_mhs.h"

class Can_DriverFactory : public QObject
{
    Q_OBJECT
public:
    static Can_DriverFactory* instance(QObject * parent = 0);
    ~Can_DriverFactory();

    static QStringList driverList();
    const	QString currentDriver();
    QString defaultDriver();
    void selectDriver(const QString &driverName);

    Can_Driver* driver(const QString &driverName);
    Can_Driver* actualDriver();

    QWidget* driverUserInterface(const QString &driverName);
    QWidget* actualDriverUserInterface();

    void deleteAllDriver();

signals:
    void driverBaudrate(int baudrate);
    void driverHasChanged(QString Name);
    void driverIsReady(bool ready);
    void driverIsCommunicating(bool communicating);

public slots:
    void send(const Can_Frame &Frame);

private:
    Can_DriverFactory(QObject * parent = 0);
    static void addCanDriver(Can_Driver *driver);

    struct Can_DriverFactoryPrivate *d;
    static struct Can_DriverFactoryStatic *s;
    static void createStaticData();

    // Driver plugins
    //void loadDriverPlugins();
    //void initializeDriverPlugin(QObject *plugin, QString filename);

    QString m_currentDriver;
};

#endif // CAN_DRIVERFACTORY_H


