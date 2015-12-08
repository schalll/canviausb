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
#include <QApplication>

// non QT includes
#include "WidgetBase.h"


WidgetBase::WidgetBase(QStackedWidget* stackedWidget, QWidget *parent)
        : QWidget(parent)
{
    this->stackedWidget = stackedWidget;
    stackedWidget->addWidget(this);
    setContextMenuPolicy(Qt::NoContextMenu);
}

WidgetBase::~WidgetBase()
{
}

int WidgetBase::activateWidget() {
    // Try to find and activate view from QStackedWidget
    int ret = -1;
    for (int i=0;i<stackedWidget->count();i++)
    {
        QWidget* w = stackedWidget->widget(i);
        if (w==this)
        {
            stackedWidget->setCurrentWidget(w);
            ret = 0;
            break;
        }
    }
    return ret;
}

void WidgetBase::removeWidget() {
    for (int i=0;i<stackedWidget->count();i++)
    {
        QWidget* w = stackedWidget->widget(i);
        if (w==this)
        {
            stackedWidget->removeWidget(w);
            break;
        }
    }
}

void WidgetBase::setTitleInfo(QString info) {
}

bool WidgetBase::canQuickSend() {
    return false;
}

bool WidgetBase::canPrint() {
    return false;
}

bool WidgetBase::isFilter() {
    return false;
}
