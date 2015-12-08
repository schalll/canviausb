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

#ifndef WIDGETIDENTIFIERS_H
#define WIDGETIDENTIFIERS_H

// QT includes
#include <QVBoxLayout>

// Non QT includes
#include "WidgetBase.h"
#include "TitleFrame.h"
#include "ToolFrame.h"
#include "identifiers/IdentifiersView.h"

class WidgetIdentifiers : public WidgetBase
{
    Q_OBJECT

public:
    WidgetIdentifiers(QStackedWidget* stackedWidget, QWidget *parent = 0);
    ~WidgetIdentifiers();

    bool canPrint();

public slots:
    void print();

private:
    QVBoxLayout *mainLayout;
    TitleFrame *title;
    ToolFrame *tool; 
    IdentifiersView* body;

	QAction *printAct;
};

#endif
