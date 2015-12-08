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

#ifndef WIDGETBASE_H
#define WIDGETBASE_H

// QT includes
#include <QWidget>
#include <QStackedWidget>

// Non QT includes

// ---------------------------------------------------------------------------
// QBaseWidget
//
class WidgetBase : public QWidget
{
    Q_OBJECT

public:
    WidgetBase(QStackedWidget* stackedWidget, QWidget *parent = 0);
    ~WidgetBase();  

    // Activates view
    int activateWidget();
    // Removes view
    void removeWidget();

    virtual bool canQuickSend();				// indicates if this widget can quick send CAN packets
    virtual bool canPrint();						// indicates if this widget can print
    virtual void applyPreferences() {};	// called when preferenceshave changed
    virtual bool isFilter();						// indicates if this widget is a filtered log

public slots:
    virtual void print() {};						// print function
	virtual void setTitleInfo(QString info = "");

private:
    
    // Pointer to QStackedWidget where all views exists
    QStackedWidget*     stackedWidget;
};

#endif // QBaseWidget_h
