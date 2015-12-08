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
#include "infoWidget.h"

InfoWidget::InfoWidget(QWidget* parent ) :
        QFrame(parent)
{		
    setupUi(this);
}


void
        InfoWidget::setBaudrate(int number) {
    lblBaudrate->setText(QString("%1 kbps").arg(number));
}

void
        InfoWidget::setIdentifierNumber(int number) {
    lblIdentifiers->setText(QString("%1").arg(number));
}

void 
        InfoWidget::setFrameNumber(int number) {
    lblFrames->setText(QString("%1").arg(number));
}

void 
        InfoWidget::setFramePerSecondNumber(int number) {
    lblFramesPerSecond->setText(QString("%1").arg(number));
}
