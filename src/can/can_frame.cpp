/* Copyright 2008 Roine Stenberg
 * Copyright 2008 Johan Östensson
 * Copyright 2008 ENEA AB
 *
 * This file is part of Canister.
 *
 * Canister is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Canister is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Canister.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "can_frame.h"
#include "../identifiers/IdentifiersView.h"
#include "../identifiers/IdentifiersModel.h"

// ************************************
// Can_Frame
// ************************************

Can_Frame::Can_Frame()  {
}

Can_Frame::Can_Frame(char _direction, QDateTime _time_pc, bool _ext, bool _rtr, unsigned int _id, unsigned char _dlc, unsigned char *_data)
        : direction(_direction), time_pc(_time_pc), ext(_ext), rtr(_rtr), id(_id), dlc(_dlc) {

    (void)memcpy(&data[0], _data, _dlc);
}

Can_Frame::Can_Frame(char _direction, QDateTime _time_pc, bool _ext, bool _rtr, unsigned int _id, unsigned char _dlc, unsigned char d0, unsigned char d1,unsigned char d2,unsigned char d3, unsigned char d4, unsigned char d5,unsigned char d6,unsigned char d7)
        : direction(_direction), time_pc(_time_pc), ext(_ext), rtr(_rtr), id(_id), dlc(_dlc) {

    data[0] = d0; data[1] = d1; data[2] = d2; data[3] = d3;
    data[4] = d4; data[5] = d5; data[6] = d6; data[7] = d7;
}

QString 
        Can_Frame::toString(bool PCTimestamp, bool AdapterTimestamp, bool IDHex, bool MessageHex, bool MessageAscii, bool IDName) const
{
    Q_UNUSED(AdapterTimestamp);

    QString result;

    // PC timestamp
    if (PCTimestamp) result = QString("%1 ").arg(time_pc.toString("hh:mm:ss.zzz"));
    // Adapter timestamp
    //	 if (AdapterTimestamp) result = result + QString("%1 ").arg(time_device);
    // ID in HExadecimal
    if (IDHex) {
        result = result + QString("0x%1 ").arg(id, 8, 16, QLatin1Char('0'));
    } else {
        result = result + QString("%1 ").arg(id, 10);
    }

    if (!rtr) {

        // dlc
        result = result + QString("(%1) ").arg(dlc);

        result = result + QString("%1 %2 %3 %4 %5 %6 %7 %8");
        for (unsigned int i = 0; i < 8; ++i) {
            if (i < dlc) {
                // Message in HExadecimal
                if (MessageHex)
                    result =  result.arg(data[i], 2, 16, QLatin1Char('0'));
                else
                    result =  result.arg(data[i], 3, 10, QLatin1Char('0'));
            }
            else {
                if (MessageHex)
                    result = result.arg("  ");
                else
                    result = result.arg("   ");
            }
        }

        // Message in Asii
        if (MessageAscii) {
            result = result + QString(" %1%2%3%4%5%6%7%8");
            for (unsigned int i=0; i<8; ++i) {
                if (i < dlc) {
                    if ((data[i]>31) && (data[i]<127))
                        result = result.arg(QLatin1Char(data[i]));
                    else
                        result = result.arg(".");
                }
                else {
                    result = result.arg(" ");
                }
            }
        }

    } else {
        // RTR
        result = result + QString("RTR (%1) ").arg(dlc);
    }

    if (IDName) {
        // Add the name associated to this identifier
        // Get the model
        IdentifiersModel* m_model;				// Identifiers model
        m_model = IdentifiersModel::instance();
        result = result + QString(" ") + QString(m_model->getIdName(id,ext));
    }

    return result;
}
