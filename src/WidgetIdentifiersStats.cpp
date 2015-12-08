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
#include <QTimer>

// non QT includes
#include "WidgetIdentifiersStats.h"
#include "can/can_driverfactory.h"

WidgetIdentifiersStats::WidgetIdentifiersStats(QStackedWidget* stackedWidget, QWidget *parent) :
        WidgetBase(stackedWidget,parent)
{
    // Title
    title = new TitleFrame(this,"Identifiers occurences");
    // Tool
    tool = new ToolFrame(this);
    // Body
    body = new IdentifiersSimpleStatsView(this);

    // Layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(title);
    mainLayout->addWidget(tool);
    mainLayout->addWidget(body);

    mainLayout->setContentsMargins (0, 0, 0, 0);
    setLayout(mainLayout);

    printAct = new QAction(tr("&Print identifier occurences list"), this);
    printAct->setIcon(QIcon(":/printer.png"));
    printAct->setStatusTip(tr("Print the identifier occurences list"));
    printAct->setEnabled(true);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));
	tool->addAction(printAct);
}

WidgetIdentifiersStats::~WidgetIdentifiersStats()
{

}


bool 
        WidgetIdentifiersStats::canPrint() {
    return true;
}

void 
        WidgetIdentifiersStats::print() {
    body->print();
}
