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
#include <QDebug>

// non QT includes
#include "can_driverfactory.h"
#include "can_frame.h"
#include "can_zcan4usbfx.h"


struct Can_DriverFactoryStatic *Can_DriverFactory::s = NULL;
struct Can_DriverFactoryPrivate {
};

struct Can_DriverFactoryStatic {
    QList<Can_Driver*> drivers; // List of all factories
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAN DRIVER FACTORY IMPLEMENTATION
/////////////////////////////////////////////////////////////////////////////////////////////////////////
Can_DriverFactory::Can_DriverFactory(QObject * parent) 
    : QObject(parent)
{
    // So that signal/slot works on Can_Frame
    qRegisterMetaType<Can_Frame>("Can_Frame");

    d = new Can_DriverFactoryPrivate;

    // Load the internal drivers
    addCanDriver(new Can_Demo(this));
    addCanDriver(new Can_Lawicel(this));
    addCanDriver(new Can_MHS(this));
    addCanDriver(new Can_ZCAN4USBFX(this));

    // Load the external drivers provided via plugins
    //loadDriverPlugins();
}

Can_DriverFactory::~Can_DriverFactory() {
    delete d;
    delete s;
}

/*

void
Can_DriverFactory::loadDriverPlugins() {
    QDir pluginsDir(qApp->applicationDirPath());
    qDebug() << QString("Try to load plugins from folder %1").arg(pluginsDir.path());

    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader driverPluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *driverPlugin = driverPluginLoader.instance();
        if (driverPlugin) {
            qDebug() << QString("Loading %1").arg(fileName);
            initializeDriverPlugin(driverPlugin, fileName);
            //pluginFileNames += fileName;

        } else {
            qDebug() << QString("%1").arg(driverPluginLoader.errorString());
            //qDebug() << QString("%1").arg(driverPlugin);
        }
    }
}

void
Can_DriverFactory::initializeDriverPlugin(QObject *plugin, QString filename) {
    CANviaUSB_DriverInterface *CANviaUSBdriver = qobject_cast<CANviaUSB_DriverInterface *>(plugin);
    if (CANviaUSBdriver) {
        //cout << "Plugin file " << filename.toStdString() << " is valid." << endl;
        addCanDriver(CANviaUSBdriver);
    }
}


*/
void
Can_DriverFactory::deleteAllDriver() {
    // Make sure to delete also each CAN driver !!!
    QListIterator<Can_Driver*> i(s->drivers);
    while (i.hasNext()) {
        Can_Driver *tmp = i.next();
        // Make sure to stop thread before deleting
        qDebug() << QString("Delete driver: %1").arg(tmp->fullName());
        tmp->stop();
        tmp->wait();
        //s->drivers.removeAll(tmp);
        //delete(tmp);
    }
    i.toFront();
    while (i.hasNext()) {
        Can_Driver *tmp = i.next();
        s->drivers.removeAll(tmp);
        delete(tmp);
    }
}

Can_DriverFactory* 
Can_DriverFactory::instance(QObject * parent) {
    static Can_DriverFactory* singleton = 0;
    if (singleton == 0) {
        singleton = new Can_DriverFactory(parent);
        singleton->setParent(qApp); // To garantee that this object is deleted when the application is done
    }
    return singleton;
}


void
Can_DriverFactory::createStaticData() {
    if (s == NULL) {
        s = new Can_DriverFactoryStatic;
    }
}


void
Can_DriverFactory::addCanDriver(Can_Driver *driver) {
    createStaticData();
    s->drivers.append(driver);
}

QStringList
Can_DriverFactory::driverList() {

    QStringList list;
    QListIterator<Can_Driver*> i(s->drivers);
    while (i.hasNext()) {
        list.append (i.next()->fullName());
    }
    return list;
}

const	QString 
Can_DriverFactory::currentDriver() {
    return m_currentDriver;
}

QString 
Can_DriverFactory::defaultDriver() {
    //return QString(demoDriverName);
    return QString(can4usbfxDriverName);
}

void 
Can_DriverFactory::selectDriver(const QString &driverName) {
    // New driver selected ?
    if (m_currentDriver != driverName) {

        // A/ Stop thread from currently selected driver
        if (m_currentDriver != "") {
            driver(m_currentDriver)->stop();
            driver(m_currentDriver)->wait();
            disconnect(driver(m_currentDriver), SIGNAL(readyStatus(bool)),this, SIGNAL(driverIsReady(bool)));
            disconnect(driver(m_currentDriver), SIGNAL(communicationStatus(bool)),this, SIGNAL(driverIsCommunicating(bool)));
            disconnect(driver(m_currentDriver), SIGNAL(baudrateStatus(int)),this, SIGNAL(driverBaudrate(int)));
        }

        // B/ Make sure this driver is existing !!!
        QListIterator<Can_Driver*> i(s->drivers);
        Can_Driver *newDriver;
        while (i.hasNext()) {
            newDriver = i.next();
            if (newDriver->fullName() == driverName) {
                m_currentDriver = driverName;
                connect(newDriver, SIGNAL(readyStatus(bool)),this, SIGNAL(driverIsReady(bool)));
                connect(newDriver, SIGNAL(communicationStatus(bool)),this, SIGNAL(driverIsCommunicating(bool)));
                connect(newDriver, SIGNAL(baudrateStatus(int)),this, SIGNAL(driverBaudrate(int)));
                newDriver->start();
                emit driverHasChanged(driverName);
                return;
            }
        }

        // C/ If driver has not been found we are going to select the demo driver
        m_currentDriver = QString(demoFullName);
        driver(m_currentDriver)->start();
        emit driverHasChanged(demoFullName);
    }
}

Can_Driver * 
Can_DriverFactory::driver(const QString &driverName) {
    Can_Driver* retval = 0;

    // Find the correct driver
    QListIterator<Can_Driver*> i(s->drivers);
    while (i.hasNext()) {
        Can_Driver *tmp = i.next();
        if (tmp->fullName() == driverName) {
            retval = tmp;
        }
    }

    return retval;
}

Can_Driver * 
Can_DriverFactory::actualDriver() {
    Can_Driver* retval = 0;

    // Find the correct driver
    QListIterator<Can_Driver*> i(s->drivers);
    while (i.hasNext()) {
        Can_Driver *tmp = i.next();
        if (tmp->fullName() == m_currentDriver) {
            retval = tmp;
        }
    }
    return retval;
}

QWidget* 
Can_DriverFactory::driverUserInterface(const QString &driverName) {
    QWidget* retval = 0;

    // Find the correct driver
    QListIterator<Can_Driver*> i(s->drivers);
    while (i.hasNext()) {
        Can_Driver *tmp = i.next();
        if (tmp->fullName() == driverName) {
            retval = tmp->userInterface();
        }
    }

    return retval;
}

QWidget* 
Can_DriverFactory::actualDriverUserInterface() {
    QWidget* retval = 0;

    // Find the correct driver
    QListIterator<Can_Driver*> i(s->drivers);
    while (i.hasNext()) {
        Can_Driver *tmp = i.next();
        if (tmp->fullName() == m_currentDriver) {
            retval = tmp->userInterface();
        }
    }

    return retval;
}

void 
Can_DriverFactory::send(const Can_Frame &Frame) {
    driver(m_currentDriver)->send(Frame);
}
