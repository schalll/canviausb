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

#ifndef WIDGETMONITOR_H
#define WIDGETMONITOR_H

// QT includes
#include <QBoxLayout>

// Non QT includes
#include "WidgetBase.h"
#include "TitleFrame.h"

class WidgetMonitor : public WidgetBase
{
    Q_OBJECT

public:
    WidgetMonitor(QStackedWidget* stackedWidget, QWidget *parent = 0);
    ~WidgetMonitor();

private:
    QVBoxLayout *mainLayout;
    TitleFrame *title;
    QWidget *body;
};


#endif
