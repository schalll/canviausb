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

#ifndef WIDGETFILTEREDLOG_H
#define WIDGETFILTEREDLOG_H

// QT includes
#include <QVBoxLayout>
#include <QSplitter>

// Non QT includes
#include "WidgetBase.h"
#include "TitleFrame.h"
#include "ToolFrame.h"
#include "CommunicationView.h"
#include "identifiers\IdentifiersModel.h"
#include "appSettings.h"
#include "filterActivationView.h"

/* WidgetDrivers
*  The class is a singleton in order to be accessible from different parts of the application
*  F.e. to use the signal-slot mechanisnm on it
*
*/

class WidgetFilteredLog : public WidgetBase
{
    Q_OBJECT

public:
    WidgetFilteredLog(QStackedWidget* stackedWidget, QWidget *parent = 0);
    ~WidgetFilteredLog();

    bool canQuickSend();					// indicates if this widget can quick send CAN packets
    bool canPrint();						// indicates if this widget can print
    void applyPreferences();
    bool isFilter();						// indicates that this widget is a filtered log
    QList<identifierStruct> getFilter();
    void setFilter(QList<identifierStruct> filter);

	bool event(QEvent *event);

public slots:
    void print();								// print function
    void setFilter();
	void setTitleInfo(QString info = QString());
	void showDifferences();

private:
    AppSettings* settings;
    QVBoxLayout *mainLayout;
	QSplitter *splitter;
    TitleFrame *title;
    ToolFrame *tool;
    CommunicationView *body;
	FilterActivationView *selection;

	QAction *setFilterAct;
	QAction *printAct;
	QAction *showDiffAct;
};

#endif
