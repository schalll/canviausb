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
#include "IdentifiersModel.h"
#include "IdentifiersDelegate.h"
#include "ColorListEditor.h"
#include "DataFormatEditor.h"

IdentifiersDelegate::IdentifiersDelegate(QObject *parent) :
        QItemDelegate(parent) {
    //QStyledItemDelegate( parent ) {
}

QWidget *
        IdentifiersDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    if (index.column() == 1) {
        // COLOR
		ColorListEditor* colorEditor = new ColorListEditor(parent);
        colorEditor->setFrame(false);
        return colorEditor;
    }  if (index.column() == 2) {
        // DATA FORMAT
		DataFormatEditor* formatEditor = new DataFormatEditor(parent);
        formatEditor->setFrame(false);
        return formatEditor;
    } else
        return QItemDelegate::createEditor( parent, option, index );
}

void 
        IdentifiersDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{ 

    if (index.column() == 1) {
        // The data function from the model is returning a QVariant that cannot be a QColor
        // So we have to use the QVariant::value() function
        // See http://doc.trolltech.com/4.5/qvariant.html - A Note on GUI Types
        QColor color = index.model()->data(index, Qt::EditRole).value<QColor>();
        ColorListEditor *colorEditor = qobject_cast<ColorListEditor *>( editor );
        colorEditor->setColor(color);
        return;
//    } if (index.column() == 2) {
//        // The data function from the model is returning a QVariant that cannot be a QColor
//        // So we have to use the QVariant::value() function
//        // See http://doc.trolltech.com/4.5/qvariant.html - A Note on GUI Types
//        QColor color = index.model()->data(index, Qt::EditRole).value<QColor>();
//        ColorListEditor *colorEditor = qobject_cast<ColorListEditor *>( editor );
//        colorEditor->setColor(color);
//        return;
    } else
        QItemDelegate::setEditorData( editor, index );
} 

void 
        IdentifiersDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {

    if (index.column() == 1) {
        ColorListEditor *colorEditor = qobject_cast<ColorListEditor *>( editor );
        QColor color = colorEditor->color();
        model->setData(index, color, Qt::EditRole);
        return;
//    } if (index.column() == 2) {
//        ColorListEditor *colorEditor = qobject_cast<ColorListEditor *>( editor );
//        QColor color = colorEditor->color();
//        model->setData(index, color, Qt::EditRole);
//        return;
    } else
        QItemDelegate::setModelData( editor, model, index );
}


QSize IdentifiersDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize tmpSz = QItemDelegate::sizeHint(option,index);
    int height = 50;

    return QSize(tmpSz.width(), height);
}

void
        IdentifiersDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& i ) const
{
    QRect displayRect;
    QRect displayRect2;

    switch (i.column()) {
        /*
                case 0:
                        displayRect = QRect(option.rect.x() + 36, option.rect.y() + 2, option.rect.width() - 38, (option.rect.height() - 4) / 2);
                        displayRect2 = QRect(option.rect.x() + 36, option.rect.y() + option.rect.height() / 2, option.rect.width() - 38, (option.rect.height() - 4) / 2);

                        // We do this in oder to see the correct background when this column is selected
                        if (option.showDecorationSelected && (option.state & QStyle::State_Selected)){
                                if (option.state & QStyle::State_Active) {
                                        painter->fillRect(option.rect, option.palette.highlight().color());
                                        painter->setPen(Qt::white);
                                } else {
                                        painter->setPen(Qt::black);
                                        QPalette p=option.palette;
                                        painter->fillRect(option.rect, p.color(QPalette::Inactive, QPalette::Background));
                                }
                        }

                        //QColor textColor = Qt::black;
                        //QFont font;
                        //font.setBold(true);
                        id = i.model()->data(i, Qt::DisplayRole).toUInt();
                        painter->drawText(displayRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, QString("0x%1 ").arg(id, 8, 16, QLatin1Char('0')));
                        painter->drawText(displayRect2, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, QString("%1 ").arg(id, 10));
                        painter->setPen(Qt::black);
                        return;
*/
                case 1:

        // We do this in oder to see the correct background when this column is selected
        if (option.showDecorationSelected && (option.state & QStyle::State_Selected)){
            if (option.state & QStyle::State_Active)
                painter->fillRect(option.rect, option.palette.highlight().color());
            else {
                QPalette p=option.palette;
                painter->fillRect(option.rect, p.color(QPalette::Inactive, QPalette::Background));
            }
        }

        QRect colorRect(option.rect.x()+15, option.rect.y()+3, option.rect.width()-30, option.rect.height()-6);
        QColor color = QColor(i.data().toString());
        //color.setAlpha(150); // TODO alpha value should be a property (also in IdentifiersDelegate.cpp, LogView.cpp and ColorListEditot.cpp)
        painter->fillRect(colorRect, QBrush(color));
        painter->drawRect(colorRect);
        return;
    }

    QItemDelegate::paint( painter, option, i );


}
