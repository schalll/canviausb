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

#ifndef CAN_DEMO_H
#define CAN_DEMO_H

// QT includes
#include <QList>

// Non QT includes
#include "can_driver.h"
#include "settings_demo.h" // user interface
#include "../identifiers/IdentifiersView.h"
#include "../CommunicationModel.h"
#include "../IdentifiersStatsModel.h"
#include "../identifiers/IdentifiersModel.h"

#define demoDriverName "Demo device"
#define demoFullName "CANviaUSB Demo device"



/** Concrete class for a CAN Demo Module */
class Can_Demo: public Can_Driver
{
    Q_OBJECT

public:
    Can_Demo(QObject * parent = 0);
    ~Can_Demo();

    void init();						// called when the driver is choosen from the list of driver
    void go();							// called when the start button is pressed
    void hold();						// called when the stop button is pressed
    void leave();						// called when the driver is unchoosen from the list of driver
    void driver_state_machine();		// driver cyclic function
    void send(const Can_Frame &Frame);	// driver send function

protected:
    void createIDs();
    void simulateSomeIncommingPackets();

private:

    // Local definition of the user interface
    Settings_Demo* m_ui;

    // Reference to the models used in the application
    IdentifiersModel* id_model;
    CommunicationModel* com_model;
    IdentifiersStatsModel* idStat_model;

    // State machine related definitions
    enum Can_Demo_states
    {
        state_init,
        state_waitForStartCommand,
        state_waitForStopCommand
    };
    Can_Demo_states m_state;

    // Member variables used to interact with the state machine
    bool m_identifierCreated;
    bool m_start_communication;
    bool m_stop_communication;
    bool m_packet_to_be_send;
    Can_Frame SendFrame;
};

#endif // CAN_DEMO_H


