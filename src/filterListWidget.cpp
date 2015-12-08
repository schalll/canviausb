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
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>

// non QT includes
#include "filterListWidget.h"

FilterListWidget::FilterListWidget(QWidget *parent)
        : QListWidget(parent)
{
    setAcceptDrops(true);
}

void FilterListWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        startPos = event->pos();
    QListWidget::mousePressEvent(event);
}

void FilterListWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - startPos).manhattanLength();
        if (distance >= QApplication::startDragDistance())
            performDrag();
    }
    QListWidget::mouseMoveEvent(event);
}

void FilterListWidget::dragEnterEvent(QDragEnterEvent *event)
{
    FilterListWidget *source =
            qobject_cast<FilterListWidget *>(event->source());
    if (source && source != this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void FilterListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    FilterListWidget *source =
            qobject_cast<FilterListWidget *>(event->source());
    if (source && source != this) {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void FilterListWidget::dropEvent(QDropEvent *event)
{
    FilterListWidget *source =
            qobject_cast<FilterListWidget *>(event->source());
    if (source && source != this) {
        // Create an item using the text that has to be displayed
        QListWidgetItem *item = new QListWidgetItem(event->mimeData()->text(), this);
        // Define the indentifier that was transmitted using the "UserRole" mimeType
        item->setData(Qt::UserRole, event->mimeData()->data("UserRole").toUInt());
		uint tmp = event->mimeData()->data("UserRole+1").toUInt();
		if (tmp == 0) {
			item->setData(Qt::UserRole+1, QVariant(false));
		} else {
			item->setData(Qt::UserRole+1, QVariant(true));
		}
		addItem(item);
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
}

void FilterListWidget::performDrag()
{
    QListWidgetItem *item = currentItem();
    if (item) {
        QMimeData *mimeData = new QMimeData;
        // set the text displayed
        mimeData->setText(item->text());
        // also 'pack' the identifier stored in the UserRole data
        mimeData->setData("UserRole",item->data(Qt::UserRole).toByteArray());
        QVariant tmp = item->data(Qt::UserRole+1);
		if (tmp.toBool()) {
			mimeData->setData("UserRole+1","1");
		} else {
			mimeData->setData("UserRole+1","0");
		}

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        //drag->setPixmap(QPixmap(":/form_blue_add.png"));
        if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
            delete item;
    }
}
