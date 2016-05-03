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
#include <QPushButton>

// non QT includes
#include "can_selectdriverdialog.h"

// QDialog using QStringList example @ http://doc.trolltech.com/qq/qq11-stringlistedit.html

Can_SelectCanDriverDialog::Can_SelectCanDriverDialog(QWidget *parent)
        : QDialog(parent)
{
    setWindowTitle(tr("Select CAN device"));

    QHBoxLayout *hbox = new QHBoxLayout(this);
    listBox = new QListWidget(this);
    hbox->addWidget(listBox, 1);

    // Select Button
    selectButton = new QPushButton(tr("&Select"), this);

    // Cancel Button
    cancelButton = new QPushButton(tr("Cancel"), this);
    cancelButton->setDefault(true);

    buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(selectButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(cancelButton, QDialogButtonBox::RejectRole);

    connect(selectButton, SIGNAL(clicked()), this, SLOT(selectClicked()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

    hbox->addWidget(buttonBox);

    setList(canFactory()->driverList());

}

Can_SelectCanDriverDialog::~Can_SelectCanDriverDialog() {

}

Can_DriverFactory *
        Can_SelectCanDriverDialog::canFactory() {
    return Can_DriverFactory::instance(this);
}

void
        Can_SelectCanDriverDialog::setList(const QStringList &list)
{
    listBox->clear();
    for (int i = 0; i < (int)list.count(); i++) {
        listBox->addItem(list[i]);
    }
}

void
        Can_SelectCanDriverDialog::selectClicked() {
    // Get the name of the current selected driver
    QString driverName = listBox->currentItem()->text();
    // Emit a signal if this driver is different from the previous selected one
    if (driverName != canFactory()->currentDriver())
        canFactory()->selectDriver(driverName);
    // Close window
    close();
}

void
        Can_SelectCanDriverDialog::setVisible(bool visible) {
    // First select the current driver
    for(int i=0; i<listBox->count(); i++){
        QListWidgetItem *item = listBox->item(i);
        if(item->text() == canFactory()->currentDriver())
            listBox->setCurrentItem(item);
    }
    QDialog::setVisible(visible);
}
