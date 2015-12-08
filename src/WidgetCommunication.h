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

#ifndef WIDGETCOMMUNICATION_H
#define WIDGETCOMMUNICATION_H

// QT includes
#include <QVBoxLayout>

// Non QT includes
#include "WidgetBase.h"
#include "TitleFrame.h"
#include "ToolFrame.h"
#include "CommunicationView.h"
#include "appSettings.h"

class WidgetCommunication : public WidgetBase
{
    Q_OBJECT

public:
    WidgetCommunication(QStackedWidget* stackedWidget, QWidget *parent = 0);
    ~WidgetCommunication();

    bool canQuickSend();				// indicates if this widget can quick send CAN packets
    bool canPrint();						// indicates if this widget can print
    void applyPreferences();		// called when preferenceshave changed

	bool event(QEvent *event);

public slots:
    void print();		// print function
	void showDifferences();

private:
    AppSettings* settings;
	QVBoxLayout *mainLayout;
    TitleFrame *title;
    ToolFrame *tool;
	CommunicationView *body;

	QAction *printAct;
	QAction *showDiffAct;
};


#endif
