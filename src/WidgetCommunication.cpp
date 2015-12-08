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
#include <QAction>
#include <QEvent>
#include <QDebug>

// non QT includes
#include "WidgetCommunication.h"

WidgetCommunication::WidgetCommunication(QStackedWidget* stackedWidget, QWidget *parent) :
        WidgetBase(stackedWidget,parent)
{
	settings = AppSettings::instance();

    // Title
    title = new TitleFrame(this,"Communication log");
    // Tool
    tool = new ToolFrame(this);
	tool->makeSearchBarVisible(true);
	
	// Body
    body = new CommunicationView(this, false);
	connect(tool, SIGNAL(searchStringChanged(QString)),body, SLOT(setSearchString(QString)));
	connect(tool, SIGNAL(searchByFiltering()),body, SLOT(setSearchByFiltering()));
	connect(tool, SIGNAL(searchByHighlighting()),body, SLOT(setSearchByHighlighting()));

	// Layout
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(title);
    mainLayout->addWidget(tool);
	mainLayout->addWidget(body);

    mainLayout->setContentsMargins (0, 0, 0, 0);
    setLayout(mainLayout);

    printAct = new QAction(tr("&Print communication log"), this);
    printAct->setIcon(QIcon(":/printer.png"));
    printAct->setStatusTip(tr("Print the communication log"));
    printAct->setEnabled(true);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));
	

	showDiffAct = new QAction(tr("&Show differences between consecutives datas of same CAN Identifier"), this);
    showDiffAct->setIcon(QIcon(":/find_doc.png"));
    showDiffAct->setStatusTip(tr("Show differences between consecutives datas of same CAN Identifier"));
    showDiffAct->setEnabled(true);
    showDiffAct->setCheckable(true);
	connect(showDiffAct, SIGNAL(triggered()), this, SLOT(showDifferences()));

	tool->addAction(printAct);
	tool->addSeparator();
	tool->addAction(showDiffAct);
}

WidgetCommunication::~WidgetCommunication()
{

}

bool WidgetCommunication::canQuickSend() {
    return true;
}

bool 
        WidgetCommunication::canPrint() {
    return true;
}

void 
        WidgetCommunication::print() {

    qDebug() << QString("WidgetCommunication::print test");

    body->print();
}

void 
        WidgetCommunication::showDifferences() {
    settings->enabledDifferences(showDiffAct->isChecked());
	body->redraw();
}

void 
        WidgetCommunication::applyPreferences() {
    body->applyPreferences();
}

bool 
WidgetCommunication::event(QEvent *event) {
	// Catch the show event in order to empty the search bar at this moment
	if (event->type() == QEvent::Show) {
		tool->emptySearchBar();
		showDiffAct->setChecked(settings->displayDifferences());
	}
	return WidgetBase::event(event);
}
