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

#ifndef CAN_FRAME_H
#define CAN_FRAME_H

// QT includes
#include <QDateTime>

// Non QT includes
#include <string.h>

struct Can_Frame{

    Can_Frame();
    Can_Frame(char _direction, QDateTime _time_pc, bool _ext, bool _rtr, unsigned int _id, unsigned char _dlc, unsigned char *_data);
    Can_Frame(char _direction, QDateTime _time_pc, bool _ext, bool _rtr, unsigned int _id, unsigned char _dlc, unsigned char d0 = 0, unsigned char d1 = 0,unsigned char d2 = 0,unsigned char d3 = 0, unsigned char d4 = 0, unsigned char d5 = 0,unsigned char d6 = 0,unsigned char d7 = 0);
    QString toString(bool PCTimestamp, bool AdapterTimestamp, bool IDHex, bool MessageHex, bool MessageAscii, bool IDName) const;

    char direction;
    QDateTime	  time_pc;
    unsigned int  adapterTimeStamp;
    bool ext;
    bool rtr;
    unsigned int  id;
    unsigned char dlc;
    unsigned char data[8];
};


#endif // CAN_FRAME_H
