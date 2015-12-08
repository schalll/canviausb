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

#ifndef SIDEBARITEM_H
#define SIDEBARITEM_H

// QT includes

// non QT includes
#include "SideBarModel.h"

class SideBarItem
{
public:
    SideBarItem(const QString &name = QString(""), SideBarItem *parent = 0, int type = 0, QWidget* gui = 0);
    ~SideBarItem();

    SideBarItem *child(int number);
    int childCount() const;

    QString name() const;
	QString toolTip() const;

    SideBarItem *parent();

    SideBarItem *insertChild(int position, int type = 0, QWidget* gui = 0);
    bool removeChildren(int position, int count);

    int type() const;
    QWidget* ui();
    int childNumber() const;
    bool setName(const QString &name);
	bool setToolTip(const QString &toolTip);

private:
    QList<SideBarItem*> childItems;
    QString itemName;
	QString itemToolTip;
    SideBarItem *parentItem;
    int itemType;
    QWidget* gui;
};

#endif
