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
#include "can_lawicel.h"
#include "can_frame.h"
#include "ftd2xx.h"
#include <stdio.h>

//***************************************************************************
Can_Lawicel::Can_Lawicel(QObject * parent): Can_Driver(parent)
{ 
    DEBUG_LAWICEL_CAN(" Can_Lawicel::Can_Lawicel ");

    // Driver name
    m_name = QString(lawicelDriverName);
    m_fullName = QString(lawicelFullName);
    m_author = QString(lawicelManufacturerName);
    // Communication model
    com_model = CommunicationModel::instance();

    // User interface
    m_ui = new Settings_Lawicel();
    m_userInterface = m_ui;
    // Make sure user interface is disabled while communicating
    connect(this, SIGNAL(communicationStatus(bool)),m_userInterface, SLOT(communicationStatusChanged(bool)));
    connect(this, SIGNAL(readyStatus(bool)),m_userInterface, SLOT(readyStatusChanged(bool)));

    connect(this, SIGNAL(driverVersionReceived(QString)), this, SLOT (updateDriverVersionInGUI(QString)));
    connect(this, SIGNAL(serialNumberReceived(QString)), this, SLOT (updateSerialNumberInGUI(QString)));
    connect(this, SIGNAL(versionReceived(QString)), this, SLOT (updateVersionInGUI(QString)));
    connect(this, SIGNAL(canFrameReceived(Can_Frame)), this, SLOT (publishCanFrameToGUI(Can_Frame)));
}

//***************************************************************************
Can_Lawicel::~Can_Lawicel()
{
    delete m_ui;
    //m_ui->destroy();
    m_userInterface = NULL;
    DEBUG_LAWICEL_CAN(" Can_Lawicel has been deleted !!!");
}

//***************************************************************************

void
Can_Lawicel::init(){
    com_model->addInfoMessage(QString("Initializing Lawicel CANUSB driver"));
    m_ui->setDeviceName(m_name);
    m_ui->setManufacturer(m_author);
    // Init all member variables used to interact with the state machine
    m_state = state_init;
    m_reception_state = state_waitForReplyId;
    m_displayDriverVersion = true;
    m_packet_to_be_send = false;
    m_start_communication = false;
    m_stop_communication = false;
    setCommunicating(false);
    m_deviceOpen = false;
    ft_handle = NULL;
}

void
Can_Lawicel::go() {
    m_start_communication = true;
}

void
Can_Lawicel::hold(){
    m_stop_communication = true;
}
void
Can_Lawicel::leave(){
    closeDevice();
    com_model->addInfoMessage(QString("Leaving Lawicel CANUSB driver"));
}

void
Can_Lawicel::driver_state_machine() {
/*
    qDebug() << QString("Can_Lawicel::driver_state_machine = %1").arg(m_state);
    switch(m_state)
    {
    case state_init:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_init");
        break;
    case state_waitForDriverInstalled:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_waitForDriverInstalled");
        break;
    case state_waitForDeviceConnected:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_waitForDeviceConnected");
        break;
    case state_openDevice:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_openDevice");
        break;
    case state_emptyPriorCommand:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_emptyPriorCommand");
        break;
    case state_requestDeviceVersion:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_requestDeviceVersion");
        break;
    case state_waitForDeviceVersion:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_waitForDeviceVersion");
        break;
    case state_requestSerialNumber:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_requestSerialNumber");
        break;
    case state_waitForSerialNumber:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_waitForSerialNumber");
        break;
    case state_waitForStartCommand:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_waitForStartCommand");
        break;
    case state_waitForStopCommand:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_waitForStopCommand");
        break;
    case state_closeDriver:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_closeDriver");
        break;
    case state_closeDevice:
        qDebug() << QString("Can_Lawicel::driver_state_machine = state_closeDevice");
        break;
    }

    */

    // Enter critical section here
    QMutexLocker locker(&dsm_mutex);

    switch(m_state)
    {
    case state_init:
        m_ui->setDeviceStatus(QString("Device not connected"));
        setReady(false);

        // NEXT STATE - Wait for Device connected
        m_state = state_waitForDriverInstalled;
        break;

    case state_waitForDriverInstalled:
        if (!isDriverInstalled()) {
            // NEXT STATE - Init
            m_state = state_init;
            break;
        }

        // NEXT STATE - Wait for Device connected
        m_state = state_waitForDeviceConnected;
        break;

    case state_waitForDeviceConnected:
        if (!isDeviceConnected()) {
            break;
        }

        m_ui->setDeviceStatus(QString("Device connected"));
        m_ui->setManufacturer(m_author);
        setReady(true);
        // NEXT STATE - Open Device
        m_state = state_openDevice;
        break;


    case state_openDevice:
        // Open the device
        if (!openDevice()) {
            // NEXT STATE - Init
            m_state = state_init;
            break;
        }

        // NEXT STATE - Empty prior command
        m_state = state_emptyPriorCommand;
        break;

    case state_emptyPriorCommand:
        // Empty prior command
        if (!emptyPriorCommand()) {
            // NEXT STATE - Close device
            m_state = state_closeDevice;
            break;
        }

        // Purge input and output buffer
        purgeBuffer();

        // NEXT STATE - Request Device Version
        m_state = state_requestDeviceVersion;
        break;

    case state_requestDeviceVersion:
        m_deviceVersionReceived = false;
        if (!askForDeviceVersion()) {
            // NEXT STATE - Close device
            m_state = state_closeDevice;
            break;
        }

        // Start delay timer to be able to treat a timeout
        delayTimer.restart();

        // NEXT STATE - Wait for Device Version
        m_state = state_waitForDeviceVersion;
        break;

    case state_waitForDeviceVersion:
        if (m_deviceVersionReceived) {
            // NEXT STATE - Request Serial Number
            m_state = state_requestSerialNumber;
            break;
        }

        // Monitor timeout - we allow 1 second
        if (delayTimer.elapsed() > 1000) {
            // NEXT STATE - Close device
            m_state = state_closeDevice;
            break;
        }

        break;

    case state_requestSerialNumber:
        m_deviceSerialNumberReceived = false;
        if (!askForSerialNumber()) {
            // NEXT STATE - Close device
            m_state = state_closeDevice;
            break;
        }

        // Start delay timer to be able to treat a timeout
        delayTimer.restart();

        // NEXT STATE - Wait for Serial Number
        m_state = state_waitForSerialNumber;
        break;

    case state_waitForSerialNumber:
        // Monitor timeout - we allow 1 second
        int tmp;
        tmp = delayTimer.elapsed();

        if (delayTimer.elapsed() > 1000) {
            // NEXT STATE - Close device
            m_state = state_closeDevice;
            break;
        }

        if (m_deviceSerialNumberReceived) {
            // Start delay timer to be able to treat a timeout
            delayTimer.restart();

            // NEXT STATE - Wait for Start Command
            m_state = state_waitForStartCommand;
            break;
        }
        break;


    case state_waitForStartCommand:
        // Check if still connected
        // Monitor timeout - Ask for serail number each second
        if (delayTimer.elapsed() > 1000) {

            m_deviceSerialNumberReceived = false;
            if (!askForSerialNumber()) {
                // NEXT STATE - Close device
                m_state = state_closeDevice;
                break;
            }

            // Start delay timer to be able to treat a timeout
            delayTimer.restart();

            // NEXT STATE - Wait for Serial Number
            m_state = state_waitForSerialNumber;
            break;
        }


        // Check if a start communication command is pending
        if (m_start_communication) {
            m_start_communication = false;

            m_baudrate = m_ui->baudrate();
            com_model->addInfoMessage(QString("Set device baudrate (%1kps)").arg(m_baudrate));
            if (setDeviceBaudrate(m_baudrate)) {
                com_model->addInfoMessage(QString("Open CAN channel"));
                if (openCANChannel()) {
                    com_model->addInfoMessage(QString("Communicating @ %1 kpbs").arg(m_baudrate));
                    emit baudrateStatus(m_baudrate);
                    setCommunicating(true);
                    // NEXT STATE - Wait for Stop Command
                    m_state = state_waitForStopCommand;
                } else {
                    qDebug() << QString("Holllllllllllllla minaud");
                    // NEXT STATE - Close device
                    m_state = state_closeDevice;
                    break;
                }
            } else {
                // NEXT STATE - Close device
                m_state = state_closeDevice;
                break;
            }
        }
        break;

    case state_waitForStopCommand:
        // Check if a stop communication command is pending
        if (m_stop_communication) {
            m_stop_communication = false;
            com_model->addInfoMessage(QString("Communication has been stopped"));
            setCommunicating(false);
            if (closeCANChannel()) {
                // NEXT STATE - Wait for Start Command
                m_state = state_waitForStartCommand;
                break;
            } else {
                // NEXT STATE - Close device
                m_state = state_closeDevice;
                break;
            }
        }

        // Send pending outgoing packet
        if (m_packet_to_be_send) {
            m_packet_to_be_send = false;
            sendPendingPacket();
        }

        break;

    case state_closeDevice:
        closeDevice();
        // NEXT STATE - Init
        m_state = state_init;
        break;

    default:
        // Something went wrong
        // NEXT STATE - Init
        m_state = state_init;
    }

    // poll for incoming datas from the device
    pollForDatas();
    msleep(1);
}


bool
Can_Lawicel::isDriverInstalled() {
    DWORD dwLibraryVersion = 0;
    ft_status = FT_GetLibraryVersion( &dwLibraryVersion );

    emit driverVersionReceived(QString("D2XX DLL Version 0x%1").arg(dwLibraryVersion, 8, 16, QLatin1Char('0')));
    if ( ft_status != FT_OK )
        return false;
    return true;
}

bool
Can_Lawicel::isDeviceConnected() {
    DWORD num_of_devices = 0;
    DWORD device_index = 0;
    char buf[10000];

    DEBUG_LAWICEL_CAN(" Can_Lawicel::isConnected");

    // Verify how many device are attached to this PC
    ft_status = FT_ListDevices( &num_of_devices, NULL, FT_LIST_NUMBER_ONLY );

    if (num_of_devices == 0)
        return false;	// No device connected so we do not need to investigate any further

    // Get some info from the device
    ft_status = FT_ListDevices( (PVOID)device_index, buf, FT_LIST_BY_INDEX | FT_OPEN_BY_DESCRIPTION );

    if ( ft_status != FT_OK )
        return false;	// Hardware cannot be listed
    ft_status = FT_ListDevices( (PVOID)device_index, device_to_open, FT_LIST_BY_INDEX | FT_OPEN_BY_SERIAL_NUMBER );

    if ( ft_status != FT_OK )
        return false;	// Hardware cannot be listed
    if ( strstr( "CANUSB", buf ) == NULL )
        return false;	// Hardware not connected
    DEBUG_LAWICEL_CAN("Let's print the device that need to be openeed: ")
            DEBUG_LAWICEL_CAN(device_to_open)

            return true;
}

bool 
Can_Lawicel::openDevice() {
    DEBUG_LAWICEL_CAN(" Can_Lawicel::openDevice");
    ft_status = FT_OpenEx( (void *)device_to_open, FT_OPEN_BY_SERIAL_NUMBER , &ft_handle);

    if ( ft_status != FT_OK )
        return false;	// Device could not be opened
    m_deviceOpen = true;
    return true;		// Device is now open
}

void
Can_Lawicel::closeDevice() {
    DEBUG_LAWICEL_CAN(" Can_Lawicel::closeDevice");
    FT_Close( ft_handle );

    ft_handle = NULL;
    m_deviceOpen = false;
}

bool 
Can_Lawicel::emptyPriorCommand() {
    DEBUG_LAWICEL_CAN(" Can_Lawicel::emptyPriorCommand");
    FT_STATUS status;
    char empty_cmd[] = { 0x0d, 0x0d, 0x0d,0x0d, 0x0d,0x0d, 0x0d,0x0d, 0x0d};

    m_lastCommandForDebug  = QString("0x0d, 0x0d, 0x0d - emptyPriorCommand");
    status = writeDatas( empty_cmd, sizeof( empty_cmd ) );
    if ( status == FT_OK ) 
        return true;
    return false;
}

bool 
Can_Lawicel::askForDeviceVersion() {
    DEBUG_LAWICEL_CAN(" Can_Lawicel::askForDeviceVersion");
    FT_STATUS status;
    char version_cmd[] = { 'V', 0x0d };

    m_lastCommandForDebug  = QString("'V', 0x0d - askForDeviceVersion");
    status = writeDatas( version_cmd, sizeof( version_cmd ) );
    if ( status == FT_OK ) 
        return true;
    return false;
}

bool 
Can_Lawicel::setDeviceBaudrate(int baudrate) {
    DEBUG_LAWICEL_CAN(" Can_Lawicel::setDeviceBaudrate");
    FT_STATUS status;
    char baudrate_cmd[] = { 'S', '0', 0x0d };;
    switch(baudrate)
    {
    case 10:
        baudrate_cmd[1] = '0';
        break;
    case 20:
        baudrate_cmd[1] = '1';
        break;
    case 50:
        baudrate_cmd[1] = '2';
        break;
    case 100:
        baudrate_cmd[1] = '3';
        break;
    case 125:
        baudrate_cmd[1] = '4';
        break;
    case 250:
        baudrate_cmd[1] = '5';
        break;
    case 500:
        baudrate_cmd[1] = '6';
        break;
    case 800:
        baudrate_cmd[1] = '7';
        break;
    case 1000:
        baudrate_cmd[1] = '8';
        break;
    default:
        com_model->addErrorMessage(QString("This baudrate (%1kps) is not supported yet").arg(baudrate));
        return false;
    }
    m_lastCommandForDebug  = QString("'S', '0', 0x0d - setDeviceBaudrate");
    status = writeDatas( baudrate_cmd, sizeof( baudrate_cmd ) );
    if ( status == FT_OK ) 
        return true;
    return false;
}

bool 
Can_Lawicel::openCANChannel() {
    DEBUG_LAWICEL_CAN(" Can_Lawicel::openCANChannel");
    FT_STATUS status;
    char open_cmd[] = { 'O', 0x0d };

    m_lastCommandForDebug  = QString("'O', 0x0d - openCANChannel");
    status = writeDatas( open_cmd, sizeof( open_cmd ) );
    if ( status == FT_OK ) 
        return true;
    return false;
}

bool 
Can_Lawicel::closeCANChannel() {
    DEBUG_LAWICEL_CAN(" Can_Lawicel::closeCANChannel");
    FT_STATUS status;
    char close_cmd[] = { 'C', 0x0d };

    m_lastCommandForDebug  = QString("'C', 0x0d - closeCANChannel");
    status = writeDatas( close_cmd, sizeof( close_cmd ) );
    if ( status == FT_OK ) 
        return true;
    return false;
}

bool 
Can_Lawicel::askForSerialNumber() {
    DEBUG_LAWICEL_CAN(" Can_Lawicel::askForSerialNumber");
    FT_STATUS status;
    char version_cmd[] = { 'N', 0x0d };

    m_lastCommandForDebug  = QString("'N', 0x0d - askForSerialNumber");
    status = writeDatas( version_cmd, sizeof( version_cmd ) );
    if ( status == FT_OK ) 
        return true;
    return false;
}

bool
Can_Lawicel::sendPendingPacket(){
    DEBUG_LAWICEL_CAN(" Can_Lawicel::sendPendingPacket");
    FT_STATUS status;

    if (isCommunicating()) {

        // Construct the command that is going to be send to the adapter
        // First lets'see how long the command will be:
        // 1 byte for the command
        unsigned int commandLength = 1;
        // 3 bytes for a standard (11 bit) id, 8 bytes for an extended (29 bit) id
        if (!SendFrame.ext)
            commandLength += 3;
        else
            commandLength += 8;
        // 1 byte for the dlc
        commandLength += 1;
        // 2 bytes * dlc for the datas
        commandLength += 2*SendFrame.dlc;
        // 1 byte for the [CR]
        commandLength += 1;

        char transmit_cmd[100];
        unsigned int dataStartingIndex;
        char command;

        // Put in the command, the identifier and the dlc
        if ( SendFrame.rtr )
        {
            if ( SendFrame.ext )
                command = 'R';
            else
                command = 'r';
        }
        else
        {
            if ( SendFrame.ext )
                command = 'T';
            else
                command = 't';
        }
        if ( !SendFrame.ext ) {
            sprintf( transmit_cmd, "%c%03x%d", command, SendFrame.id, SendFrame.dlc );
            dataStartingIndex = 5;
        } else {
            sprintf( transmit_cmd, "%c%08x%d", command, SendFrame.id, SendFrame.dlc );
            dataStartingIndex = 10;
        }
        // Put in the datas
        char *data = (char*)SendFrame.data;
        for ( unsigned int i = 0; i < SendFrame.dlc; i++ ) {
            sprintf( &(transmit_cmd[ dataStartingIndex + i*2 ]), "%02x", ((*data) & 0xff) );
            data++;
        }
        // Put in the [CR]
        transmit_cmd[dataStartingIndex + SendFrame.dlc*2] = 0xd;

        m_lastCommandForDebug  = QString("sendPendingPacket");
        status = writeDatas( transmit_cmd, commandLength );
        if ( status == FT_OK ) {
            // Add this frame to the communication model
            com_model->addFrame(SendFrame);
            return true;
        }
        return false;
    }
    return false;
}

void
Can_Lawicel::pollForDatas()
{
    //	DEBUG_LAWICEL_CAN(" Can_Lawicel::pollForDatas")
    //qDebug(">> Can_Lawicel::pollForDatas");
    static char rx_buf[100];
    int bytes_read;
    DWORD last_error = 0;
    DWORD rx_buf_count;
    unsigned int i;
    int data_byte;
    static Can_Frame frame;
    // First fill in the buffer with some possible data from the adapter
    if (readDatas() != FT_OK) {
        return;
    }

    unsigned int test;
    bool do_another_loop = true;
    // Is there some data in the buffer
    while (( m_inputBuffer.getDataAvailable() > 0) && (do_another_loop == true))
        //	if ( m_inputBuffer.getDataAvailable() > 0)
    {

        switch (m_reception_state) {
        case state_waitForReplyId:
            DEBUG_LAWICEL_CAN("m_reception_state :: state_waitForReplyId")
                    m_inputBuffer.removeData(rx_buf,1);
            qDebug() << QString("We just received %1").arg((int)rx_buf[0]);
            switch ( rx_buf[0] ) {
            case 7: // [Bell]
                emit errorReceived();
                // Wait for the next header
                m_reception_state = state_waitForReplyId;
                break;

            case 13: // [CR]
                emit okReceived();
                // Wait for the next header
                m_reception_state = state_waitForReplyId;
                break;

            case 'N': // Serial Number request
                m_reception_state = state_waitForReplyId_N;
                break;

            case 't': // Standard Data Frame
                frame.time_pc = QDateTime::currentDateTime();
                m_reception_state = state_waitForReplyId_t;
                break;

            case 'T': // Extended Data Frame
                frame.time_pc = QDateTime::currentDateTime();
                m_reception_state = state_waitForReplyId_T;
                break;

            case 'r': // Standard Remote Data Frame
                frame.time_pc = QDateTime::currentDateTime();
                m_reception_state = state_waitForReplyId_r;
                break;

            case 'R': // Extended Remote Data Frame
                frame.time_pc = QDateTime::currentDateTime();
                m_reception_state = state_waitForReplyId_R;
                break;

            case 'V': // Version
                m_reception_state = state_waitForReplyId_V;
                break;

            default: // Unknow response
                // Wait for the next header
                m_reception_state = state_waitForReplyId;
                break;
            }
            break;

            // SERIAL NUMBER
        case state_waitForReplyId_N:
            DEBUG_LAWICEL_CAN("state_waitForReplyId_N");
            // NXXXX[CR] - so we look for XXXX[CR]
            if ( m_inputBuffer.getDataAvailable() >= 5) {
                m_inputBuffer.removeData(rx_buf,5);
                m_deviceSerialNumberReceived = true;
                // Emit signal to inform that serial Number info has been received
                emit serialNumberReceived(QString("%1%2%3%4").arg(rx_buf[0]).arg(rx_buf[1]).arg(rx_buf[2]).arg(rx_buf[3]));
                // Wait for the next header
                m_reception_state = state_waitForReplyId;
            } else {
                do_another_loop = false;
            }
            break;

            // STANDARD DATA FRAME
        case state_waitForReplyId_t:
            DEBUG_LAWICEL_CAN("state_waitForReplyId_t");
            // tiiiIdd..[CR] - so we look for iiiI
            if ( m_inputBuffer.getDataAvailable() >= 4) {
                m_inputBuffer.removeData(rx_buf,3);		// total of 3 out of 4
                frame.ext = false;
                frame.rtr = false;
                sscanf( rx_buf, "%03x", &frame.id);
                m_inputBuffer.removeData(rx_buf,1);		// total of 4 out of 4
                sscanf( &rx_buf[0], "%1x", &frame.dlc);
                // Wait for data of this standard message
                m_reception_state = state_waitForReplyFrame_datas;
            } else {
                do_another_loop = false;
            }
            break;

            // EXTENDED DATA FRAME
        case state_waitForReplyId_T:
            DEBUG_LAWICEL_CAN("state_waitForReplyId_T");
            // TiiiiiiiiIdd..[CR] - so we look for iiiiiiiiI
            if ( m_inputBuffer.getDataAvailable() >= 9) {
                m_inputBuffer.removeData(rx_buf,8);	// total of 8 out of 9
                frame.ext = true;
                frame.rtr = false;
                sscanf( rx_buf, "%08x", &frame.id);
                m_inputBuffer.removeData(rx_buf,1); // total of 9 out of 9
                sscanf( &rx_buf[0], "%1x", &frame.dlc);
                // Wait for data of this extended message
                m_reception_state = state_waitForReplyFrame_datas;
            } else {
                do_another_loop = false;
            }
            break;

            // STANDARD REMOTE DATA FRAME
        case state_waitForReplyId_r:
            DEBUG_LAWICEL_CAN("state_waitForReplyId_r");
            // riiiI[CR] - so we look for iiiI[CR]
            if ( m_inputBuffer.getDataAvailable() >= 5) {
                m_inputBuffer.removeData(rx_buf,3);	// total of 3 out of 5
                //tmp = QString("Standard remote ID: [0][1][2] - %1 %2 %3").arg((int)rx_buf[0]).arg((int)rx_buf[1]).arg((int)rx_buf[2]);
                //qDebug(tmp.toAscii());
                frame.ext = false;
                frame.rtr = true;
                sscanf( rx_buf, "%03x", &frame.id);
                m_inputBuffer.removeData(rx_buf,1);	// total of 4 out of 5
                //tmp = QString("Standard remote DLC: [0] - %1 ").arg((int)rx_buf[0]);
                //qDebug(tmp.toAscii());
                sscanf( &rx_buf[0], "%1x", &frame.dlc);
                m_inputBuffer.removeData(rx_buf,1);	// total of 5 out of 5
                // Verify the datas where transmitted with a time stamp
                if ((int)(rx_buf[0]) != 13) {
                    // Wait for the timestamp
                    m_reception_state = state_waitForReplyFrame_timestamp; // for r,R,t or T command
                    break;
                } else {
                    // Emit signal to inform that a CAN frame has been received
                    //emit canFrameReceived(frame);
                    publishCanFrameToGUI(frame);
                    // Wait for the next header
                    m_reception_state = state_waitForReplyId;
                    break;
                }
            } else {
                do_another_loop = false;
            }
            break;

            // EXTENDED REMOTE DATA FRAME
        case state_waitForReplyId_R:
            DEBUG_LAWICEL_CAN("state_waitForReplyId_R");
            // RiiiiiiiiI[CR] - so we look for iiiiiiiiI[CR]
            if ( m_inputBuffer.getDataAvailable() >= 10) {
                m_inputBuffer.removeData(rx_buf,8);	// total of 8 out of 10
                //tmp = QString("Extended remote ID: [0][1][2][3] - %1 %2 %3 %4").arg((int)rx_buf[0]).arg((int)rx_buf[1]).arg((int)rx_buf[2]).arg((int)rx_buf[3]);
                //qDebug(tmp.toAscii());
                //tmp = QString("Extended remote ID: [4][5][6][7] - %4 %5 %6 %7").arg((int)rx_buf[4]).arg((int)rx_buf[5]).arg((int)rx_buf[6]).arg((int)rx_buf[7]);
                //qDebug(tmp.toAscii());
                frame.ext = true;
                frame.rtr = true;
                sscanf( rx_buf, "%08x", &frame.id);
                m_inputBuffer.removeData(rx_buf,1);	// total of 9 out of 10
                //tmp = QString("Extended remote DLC: [0] - %1 ").arg((int)rx_buf[0]);
                //qDebug(tmp.toAscii());
                sscanf( &rx_buf[0], "%1x", &frame.dlc);
                m_inputBuffer.removeData(rx_buf,1);	// total of 10 out of 10
                // Verify the datas where transmitted with a time stamp
                if ((int)(rx_buf[0]) != 13) {
                    // Wait for the timestamp
                    m_reception_state = state_waitForReplyFrame_timestamp; // for r,R,t or T command
                    break;
                } else {
                    // Emit signal to inform that a CAN frame has been received
                    //emit canFrameReceived(frame);
                    publishCanFrameToGUI(frame);
                    // Wait for the next header
                    m_reception_state = state_waitForReplyId;
                    break;
                }
            } else {
                do_another_loop = false;
            }
            break;

            // STANDARD/EXTENDED REMOTE FRAME DATAS
            // STANDARD/EXTENDED TRANSMIT FRAME DATAS
        case state_waitForReplyFrame_datas:
            DEBUG_LAWICEL_CAN("state_waitForReplyFrame_datas ");
            // tiiiIdd..[CR] or TiiiiiiiiIdd..[CR] - so we look for dd..[CR]
            if ( m_inputBuffer.getDataAvailable() >= frame.dlc*2+1) {
                m_inputBuffer.removeData(rx_buf,frame.dlc*2+1);
                //tmp = QString("FRAME DATAS: DLC = %1 so we need to get %2 bytes and the last one is %3").arg(frame.dlc).arg(frame.dlc*2+1).arg((int)rx_buf[frame.dlc*2]);
                //qDebug(tmp.toAscii());
                for ( i=0; i < frame.dlc; i++ ) {
                    sscanf( &rx_buf[i*2], "%2x", &data_byte );
                    frame.data[i] = (char)data_byte;
                }
                // Verify the datas where transmitted with a time stamp
                if ((int)(rx_buf[frame.dlc*2]) != 13) {
                    // Wait for the timestamp
                    m_reception_state = state_waitForReplyFrame_timestamp; // for r,R,t or T command
                    break;
                } else {
                    // Emit signal to inform that a CAN frame has been received
                    //emit canFrameReceived(frame);
                    publishCanFrameToGUI(frame);
                    // Wait for the next header
                    m_reception_state = state_waitForReplyId;
                    break;
                }
            } else {
                do_another_loop = false;
            }
            break;

            // STANDARD/EXTENDED REMOTE FRAME TIMESTAMP
            // STANDARD/EXTENDED TRANSMIT FRAME TIMESTAMP
        case state_waitForReplyFrame_timestamp:
            DEBUG_LAWICEL_CAN("state_waitForReplyFrame_timestamp ");
            // Timestampt is TTTT[CR], first T has already been received, so we look for TTT[CR]
            if ( m_inputBuffer.getDataAvailable() >= 4) {
                m_inputBuffer.removeData(rx_buf+frame.dlc*2+1,4);
                //tmp = QString("FRAME DATAS TIMESTAMP: %1 | %2 %3 %4 %5 concluded with %6").arg((int)rx_buf[frame.dlc*2-1]).arg((int)rx_buf[frame.dlc*2]).arg((int)rx_buf[frame.dlc*2+1]).arg((int)rx_buf[frame.dlc*2+2]).arg((int)rx_buf[frame.dlc*2+3]).arg((int)rx_buf[frame.dlc*2+4]);
                //qDebug(tmp.toAscii());
                sscanf( &rx_buf[frame.dlc*2], "%04x", &frame.adapterTimeStamp);
                // Emit signal to inform that a CAN frame has been received
                //emit canFrameReceived(frame);
                publishCanFrameToGUI(frame);
                // Wait for the next header
                m_reception_state = state_waitForReplyId;
                break;
            } else {
                do_another_loop = false;
            }
            break;

            // VERSION
        case state_waitForReplyId_V:
            DEBUG_LAWICEL_CAN("state_waitForReplyId_V");
            if ( m_inputBuffer.getDataAvailable() >= 4) {
                m_inputBuffer.removeData(rx_buf,4);
                m_deviceVersionReceived = true;
                // Emit signal to inform that Version info has been received
                emit versionReceived(QString("HW:%1.%2 - SW:%3.%4").arg(rx_buf[0]).arg(rx_buf[1]).arg(rx_buf[2]).arg(rx_buf[3]));
                // Wait for the next header
                m_reception_state = state_waitForReplyId;
            } else {
                do_another_loop = false;
            }
            break;
        }
    }
    //qDebug("<< Can_Lawicel::pollForDatas");
}

void
Can_Lawicel::send(const Can_Frame &Frame){
    if (isCommunicating()) {

        if (!m_packet_to_be_send) {
            SendFrame = Frame;
            m_packet_to_be_send = true;
        }
    }
}

FT_STATUS
Can_Lawicel::writeDatas(char *data, int data_size) {
    // Write data to the adapter
    DEBUG_LAWICEL_CAN(" Can_Lawicel::writeDatas");
    FT_STATUS ret;
    DWORD bytes_written = 0;

    // Enter critical section here
    //	QMutexLocker locker(&mutex);
    ret = FT_Write(ft_handle, data, data_size, &bytes_written);

    return ret;
}

FT_STATUS 
Can_Lawicel::readDatas(){
    // Get whatever data is in the adapter and put this in the input ring buffer

    //	DEBUG_LAWICEL_CAN(" Can_Lawicel::readDatas")
    FT_STATUS ret;
    DWORD bytes_read;
    DWORD rx_buf_count;

    // Enter critical section here
    //	QMutexLocker locker(&mutex);

    ret = FT_GetQueueStatus(ft_handle, &rx_buf_count);

    if ( ret == FT_OK ) {
        if ( (int)rx_buf_count > 100 ) qDebug() << QString("rx_buf_count is %1").arg(rx_buf_count);
        // Some datas yet ?
        if ( (int)rx_buf_count > 0 )
        {
            char data[1000]; // This one need to be big enough- Also add a check if rx_buf_count > 1000 then retrieve only 1000

            if ( (int)rx_buf_count > 999 ) // Limit the amount of bytes that will be read
                rx_buf_count = 999;


            //			// Enter critical section here
            //			QMutexLocker locker(&mutex);
            ret == FT_Read(ft_handle, data, (int)rx_buf_count, &bytes_read);
            if ( ret == FT_OK )
            {
                qDebug() << QString("Adding %1 bytes to the m_inputBuffer: ").arg((unsigned int)bytes_read);
                for (int i=0; i<(unsigned int)bytes_read;i++ )
                    qDebug() << QString("%1 ").arg(data[i]);
                qDebug() << QString("....");
                m_inputBuffer.addData(data, (unsigned int)bytes_read);
            } else {
                qDebug() << QString("RET is %1").arg(ret);
            }
        }
    }
    return ret;
}


FT_STATUS  
Can_Lawicel::purgeBuffer() {
    FT_STATUS ret;
    DEBUG_LAWICEL_CAN(" Can_Lawicel::purgeBuffer");
    ret == FT_Purge(ft_handle, FT_PURGE_RX | FT_PURGE_TX);
    return ret;
}

void 
Can_Lawicel::updateDriverVersionInGUI(QString Version) {
    if (m_displayDriverVersion) {
        m_displayDriverVersion = false;
        com_model->addInfoMessage(Version);
    }
}

void 
Can_Lawicel::updateSerialNumberInGUI(QString SerialNumber) {
    m_ui->setSerialNumber(SerialNumber);
}

void 
Can_Lawicel::updateVersionInGUI(QString Version) {
    m_ui->setSoftwareVersion(Version);
}

void 
Can_Lawicel::publishCanFrameToGUI(Can_Frame Frame) {
    // Enter critical section here
    //	QMutexLocker locker(&publishing_mutex);
    // Add this frame only while the PLAY button is pressed
    if (m_state == state_waitForStopCommand)
        com_model->addFrame(Frame);
}
