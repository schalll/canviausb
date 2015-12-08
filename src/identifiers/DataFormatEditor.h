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

#ifndef DATAFORMATEDITOR_H
#define DATAFORMATEDITOR_H

#include <QComboBox>
#include "IdentifiersModel.h"

class DataFormatEditor : public QComboBox
{
    Q_OBJECT
    Q_PROPERTY(unsigned int format READ format WRITE setFormat USER true)

public:
    DataFormatEditor(QWidget *widget = 0);
    unsigned int format() const;
    void setFormat(unsigned int f);

private:
    void populateList();
    QString formatAsString(unsigned int format); // TO DO - this function i sduplicated in IdentifierModel - this is not good
};

#endif
