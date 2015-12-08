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
#include "can_demo.h"
#include "can_frame.h"
#include "../mainWindow.h"

//***************************************************************************
Can_Demo::Can_Demo(QObject * parent) : Can_Driver(parent)
{
    //qDebug(" Can_Demo::Can_Demo ");

    // Just for debugging now
    m_name = QString(demoDriverName);
    m_fullName = QString(demoFullName);
    m_version = QString("Demo version");
    m_ui = new Settings_Demo();
    m_userInterface = m_ui;
    
    m_start_communication = false;
    m_stop_communication = false;
    m_packet_to_be_send = false;


    // Make sure user interface is disabled while communicating
    connect(this, SIGNAL(communicationStatus(bool)),m_userInterface, SLOT(communicationStatusChanged(bool)));
    //m_identifierCreated = false;

    id_model = IdentifiersModel::instance();
    idStat_model = IdentifiersStatsModel::instance();
    com_model = CommunicationModel::instance();

}

//***************************************************************************
Can_Demo::~Can_Demo()
{	
    delete m_ui; //-> not necessary, this will be done by QT
    //m_ui->destroy();
    m_userInterface = NULL;
}

//***************************************************************************

void Can_Demo::driver_state_machine() {

    switch(m_state)
    {
    case state_init:
        setReady(true);
        // NEXT STATE - Wait for Start Command
        m_state = state_waitForStartCommand;
        break;

    case state_waitForStartCommand:
        if (m_start_communication) {
            m_start_communication = false;
            createIDs();
            com_model->addInfoMessage(QString("Communication has been started"));
            setCommunicating(true);
            // NEXT STATE - Wait for Stop Command
            m_state = state_waitForStopCommand;
            break;
        }
        break;

    case state_waitForStopCommand:
        if (m_stop_communication) {
            m_stop_communication = false;
            com_model->addInfoMessage(QString("Communication has been stopped"));
            setCommunicating(false);
            // NEXT STATE - Wait for Start Command
            m_state = state_waitForStartCommand;
            break;
        }
        // Simulate incoming packet
        simulateSomeIncommingPackets();
        // Send outgoing packet
        if (m_packet_to_be_send) {
            com_model->addFrame(SendFrame);
            m_packet_to_be_send = false;
        }
        break;

    default:
        m_state = state_init;
    }
    msleep(100);
}

void Can_Demo::createIDs() {

    // Verify how many ID's are already in the list
    int count = id_model->rowCount(QModelIndex());
    if (count >= m_ui->getNumberIdentifiers()) {
        //qDebug("There is alredy enough identifiers in the list nothing need to be done");
    } else {
        int missing = m_ui->getNumberIdentifiers() - count;
        com_model->addInfoMessage(QString("Create %1 identifier(s)").arg(missing));
        //qDebug("We need to create %d identifiers", missing);

        for (int i=0 ; i<missing; i++) {
            int tmpIdentifier;
            tmpIdentifier = rand();
            id_model->insertRows(0, 1, QModelIndex());
            QModelIndex index = id_model->index(0, 0, QModelIndex());
            id_model->setData(index, tmpIdentifier, Qt::EditRole);
            //Find out if we create standard, extended or both types of identifiers.
            if (m_ui->getIdentifierType() == 0) id_model->setData(index, false, Qt::UserRole);		// Always Std
            else if (m_ui->getIdentifierType() == 1) id_model->setData(index, true, Qt::UserRole);	// Always Ext
            else {
                if (rand() % 100 > 50) id_model->setData(index, false, Qt::UserRole);				// Std
                else id_model->setData(index, true, Qt::UserRole);									// Ext
            }
            index = id_model->index(0, 1, QModelIndex());
            id_model->setData(index, QColor("#ffffff"), Qt::EditRole);
            index = id_model->index(0, 2, QModelIndex());
            id_model->setData(index, "", Qt::EditRole);
        }
    }
}

void Can_Demo::simulateSomeIncommingPackets() {

    // Create 0..1 message
    int amount = qrand() % 2;
    int rang;
    int dlc;
    Can_Frame frame;
    for (int i=0; i<amount; i++) {

        // A - Verify how many identifiers are in the list
        int count = id_model->rowCount(QModelIndex());

        // B -  get an idenifier from the list
        if (count > 0) rang = qrand() % count;
        QModelIndex index = id_model->index(rang, 0, QModelIndex());
        unsigned int identifier = id_model->data(index, Qt::EditRole).toUInt();		// ID
        bool extended = id_model->data(index, Qt::UserRole).toBool();				// Std vs. Ext
        if (identifier) {
            unsigned char data[8];
            for (int i = 0; i < 8; ++i) {
                data[i] = qrand() % 256;
            }
            if (m_ui->getFixedDLC()) {
                // Find this ID in the stat list in order to use the same DLC
                if (idStat_model->findDLC(identifier, extended, dlc)) {
                    // Use DLC from latest stat frame
                    frame = Can_Frame(0, QDateTime::currentDateTime(), extended, false, identifier, dlc, data);
                } else {
                    // DLC did not exist do create one
                    frame = Can_Frame(0, QDateTime::currentDateTime(), extended, false, identifier, qrand() % 9, data);
                }
            } else {
                // Variable DLC
                frame = Can_Frame(0, QDateTime::currentDateTime(), extended, false, identifier, qrand() % 9, data);
            }
            // Add this frame to the communication model
            com_model->addFrame(frame);

        }
    }
}


void 
Can_Demo::init() {
    com_model->addInfoMessage(QString("Initializing Can demo driver"));
    m_state = state_init;
    m_baudrate = 0;
    setCommunicating(false);
}
void 
Can_Demo::go() {
    m_start_communication = true;
}
void 
Can_Demo::hold() {
    m_stop_communication = true;
}
void 
Can_Demo::leave() {
    com_model->addInfoMessage(QString("Leaving Can demo driver"));
}

void 
Can_Demo::send(const Can_Frame &Frame) {
    if (isCommunicating()) {

        if (!m_packet_to_be_send) {
            SendFrame = Frame;
            m_packet_to_be_send = true;
        }

        // Add this frame to the communication model
        //        com_model->addFrame(Frame);

    }
}

