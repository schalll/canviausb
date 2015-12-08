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

#include <QStringList>

#include "SideBarItem.h"
#include "WidgetFilteredLog.h"


SideBarItem::SideBarItem(const QString &name, SideBarItem *parent, int type, QWidget* gui)
{
    parentItem = parent;
    itemName = name;
    itemType = type;
    this->gui = gui;

}
SideBarItem::~SideBarItem()
{
    qDeleteAll(childItems);
}

SideBarItem *SideBarItem::child(int number)
{
    return childItems.value(number);
}

int SideBarItem::childCount() const
{
    return childItems.count();
}

int SideBarItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<SideBarItem*>(this));

    return 0;
}

QString SideBarItem::name() const
{
    return itemName;
}

QString SideBarItem::toolTip() const
{
	return itemToolTip;
}


int SideBarItem::type() const
{
    return itemType;
}

QWidget* SideBarItem::ui() 
{
    return gui;
}

SideBarItem *SideBarItem::insertChild(int position, int type, QWidget* gui)
{
    qDebug() << QString("Insert sidebar item type %1").arg(type);
    if (position < 0 || position > childItems.size())
        return false;

    SideBarItem *item = new SideBarItem("", this, type, gui);
    childItems.insert(position, item);

    return item;
}

SideBarItem *SideBarItem::parent()
{
    return parentItem;
}

bool SideBarItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}

bool SideBarItem::setName(const QString &name)
{
    itemName = name;
    if (itemType == SideBar::filteredList) {
        WidgetFilteredLog* widget = static_cast<WidgetFilteredLog*>(gui);
        widget->setTitleInfo(itemName);
    }
    return true;
}

bool SideBarItem::setToolTip(const QString &toolTip) 
{
	itemToolTip = toolTip;
    return true;
}


