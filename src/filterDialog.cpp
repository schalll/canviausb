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

// non QT includes
#include "filterDialog.h"

FilterDialog::FilterDialog(QWidget *parent)
        : QDialog(parent)
{
    setupUi(this);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void FilterDialog::clear() {
    listA->clear();
    listB->clear();
}

void FilterDialog::addNonFilteredIdentifer(QString description, identifierStruct id) {
    QListWidgetItem *item = new QListWidgetItem(description, listA);
		item->setData(Qt::UserRole, id.identifier);  // We are only interested in the identifier ...
		item->setData(Qt::UserRole+1, id.extended);  // ... and the standard/extended informations
    listA->addItem(item);
}
void FilterDialog::addFilteredIdentifer(QString description, identifierStruct id) {
    QListWidgetItem *item = new QListWidgetItem(description, listB);
		item->setData(Qt::UserRole, id.identifier);  // We are only interested in the identifier ...
		item->setData(Qt::UserRole+1, id.extended);  // ... and the standard/extended informations
    listB->addItem(item);
}

QList<identifierStruct> FilterDialog::getFilter() {
    QList<identifierStruct> tmp;
		identifierStruct tmpId;
		int hopla;
		hopla = listB->count();
    for (int i=0; i<listB->count(); i++) {
				tmpId.identifier = listB->item(i)->data(Qt::UserRole).toUInt(); // We are only interested in the identifier ...
				tmpId.extended = listB->item(i)->data(Qt::UserRole+1).toBool();   // ... and the standard/extended informations
        tmp.append(tmpId);
    }
    return tmp;
}

void FilterDialog::on_leftButton_clicked()
{
    moveCurrentItem(listB, listA);
}

void FilterDialog::on_rightButton_clicked()
{
    moveCurrentItem(listA, listB);
}

void FilterDialog::moveCurrentItem(FilterListWidget *source,
                                   FilterListWidget *target)
{
    if (source->currentItem()) {
        QListWidgetItem *newItem = source->currentItem()->clone();
        target->addItem(newItem);
        target->setCurrentItem(newItem);
        delete source->currentItem();
    }
}
