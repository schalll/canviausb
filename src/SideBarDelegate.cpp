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
#include <QModelIndex>
#include <QPainter>
#include <QDebug>

// non QT includes
#include "SideBarItem.h"
#include "SideBarModel.h"
#include "SideBarDelegate.h"

SideBarDelegate::SideBarDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QSize
        SideBarDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& i ) const
{
    int const w = QStyledItemDelegate::sizeHint( option, i ).width();

    using namespace SideBar;
    
    SideBarItem *item = getItem(i);
    int itemType = item->type();

    switch (itemType)
    {
    case Spacer1:
    case Spacer2:
    case Spacer3:
        return QSize( w, 24 );

    case device:
                                case identifiers:
                                case comLog:
                                case filteredList:
								case plugin:
								case traffic:
                                    return QSize( w, 20 );

                                default:
                                    return QSize( w, 20 );
                                }
}


void
        SideBarDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& i ) const
{

#define LIGHT_BLUE_TEXT QColor( 130, 147, 174 )
#define GRAY_TEXT QColor( 120, 120, 120 )

#ifdef Q_WS_MAC
#define X 11
#else
#define X 13
#endif
    int y;

    using namespace SideBar;

    SideBarItem *item = getItem(i);
    int itemType = item->type();

    switch (itemType)
    {
        // Special treatment for Spacer 1
                case Spacer1:
        {
            y = option.rect.top() + 13;
            QFont f = option.font;
            f.setPixelSize( 11 );
            f.setBold( true );
            painter->save();
            painter->setFont( f );
            painter->setPen( GRAY_TEXT );
            painter->drawText( X, y + 5, tr("SETTINGS") );
            painter->restore();
            return;
        }

        // Special treatment for Spacer 2
                case Spacer2:
        {
            y = option.rect.top() + 13;
            QFont f = option.font;
            f.setPixelSize( 11 );
            f.setBold( true );
            painter->save();
            painter->setFont( f );
            painter->setPen( GRAY_TEXT );
            painter->drawText( X, y + 5, tr("CAN TRAFFIC") );
            painter->restore();
            return;
        }

        // Special treatment for Spacer 3
                case Spacer3:
        {
            y = option.rect.top() + 13;
            QFont f = option.font;
            f.setPixelSize( 11 );
            f.setBold( true );
            painter->save();
            painter->setFont( f );
            painter->setPen( GRAY_TEXT );
            painter->drawText( X, y + 5, tr("PLUGINS") );
            painter->restore();
            return;
        }
    }

    QStyledItemDelegate::paint( painter, option, i );


}

SideBarItem *SideBarDelegate::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        SideBarItem *item = static_cast<SideBarItem*>(index.internalPointer());
        if (item) return item;
    }
    return NULL;
}
