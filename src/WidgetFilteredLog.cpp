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
#include <QSizePolicy>

// non QT includes
#include "WidgetFilteredLog.h"
#include "mainWindow.h"
#include "CommunicationProxyModel.h"

WidgetFilteredLog::WidgetFilteredLog(QStackedWidget* stackedWidget, QWidget *parent) :
        WidgetBase(stackedWidget,parent)
{
	settings = AppSettings::instance();

    // Title
    title = new TitleFrame(this,"Filtered communication log");
    // Tool
	tool = new ToolFrame(this);
	tool->makeSearchBarVisible(true);
	// Body
    body = new CommunicationView(this, true);
	// Selection
	selection = new	FilterActivationView((CommunicationProxyModel*)body->model(), this);

	connect(tool, SIGNAL(searchStringChanged(QString)),body, SLOT(setSearchString(QString)));
	connect(tool, SIGNAL(searchByFiltering()),body, SLOT(setSearchByFiltering()));
	connect(tool, SIGNAL(searchByHighlighting()),body, SLOT(setSearchByHighlighting()));
    
	// Layout
    splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(body);
    splitter->addWidget(selection);
	splitter->setStretchFactor(0,4);
	splitter->setStretchFactor(1,1);
	splitter->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(title);
    mainLayout->addWidget(tool);
	//mainLayout->addWidget(body);
	mainLayout->addWidget(splitter);


    mainLayout->setContentsMargins (0, 0, 0, 0);
    setLayout(mainLayout);

    printAct = new QAction(tr("&Print filtered communication log"), this);
    printAct->setIcon(QIcon(":/printer.png"));
    printAct->setStatusTip(tr("Print the filtered communication log"));
    printAct->setEnabled(true);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));
	tool->addAction(printAct);

    setFilterAct = new QAction(tr("Set filter"), this);
    setFilterAct->setIcon(QIcon(":/filter_edit.png"));
    setFilterAct->setStatusTip(tr("Set filter list"));
    setFilterAct->setEnabled(true);
    connect(setFilterAct, SIGNAL(triggered()), this, SLOT(setFilter()));
	tool->addAction(setFilterAct);

	showDiffAct = new QAction(tr("&Show differences between consecutives datas of same CAN Identifier"), this);
    showDiffAct->setIcon(QIcon(":/find_doc.png"));
    showDiffAct->setStatusTip(tr("Show differences between consecutives datas of same CAN Identifier"));
    showDiffAct->setEnabled(true);
    showDiffAct->setCheckable(true);
	connect(showDiffAct, SIGNAL(triggered()), this, SLOT(showDifferences()));

	tool->addSeparator();
	tool->addAction(showDiffAct);
}

WidgetFilteredLog::~WidgetFilteredLog()
{
}

void 
WidgetFilteredLog::applyPreferences() {
    body->applyPreferences();
}

bool WidgetFilteredLog::canQuickSend() {
    return true;
}

bool 
WidgetFilteredLog::canPrint() {
    return true;
}

void 
WidgetFilteredLog::print() {
    body->print();
}

void 
        WidgetFilteredLog::showDifferences() {
    settings->enabledDifferences(showDiffAct->isChecked());
	body->redraw();
}

void 
WidgetFilteredLog::setFilter() {
	MainWindow *mainWindow = MainWindow::instance();
	mainWindow->setFilter();
}

bool 
        WidgetFilteredLog::isFilter() {
    return true;
}

QList<identifierStruct> 
        WidgetFilteredLog::getFilter() {
    return body->getFilter();
}

void 
        WidgetFilteredLog::setFilter(QList<identifierStruct> filter) {
    body->setFilter(filter);
}


void WidgetFilteredLog::setTitleInfo(QString info) {
    title->setInfo(info);
}

bool 
WidgetFilteredLog::event(QEvent *event) {
	// Catch the show event in order to empty the search bar at this moment
	if (event->type() == QEvent::Show) {
		tool->emptySearchBar();
		showDiffAct->setChecked(settings->displayDifferences());
	}
	return WidgetBase::event(event);
}
