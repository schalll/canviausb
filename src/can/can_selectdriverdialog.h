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

#ifndef CAN_SELECTDRIVERDIALOG_H
#define CAN_SELECTDRIVERDIALOG_H

// QT includes
#include <QDialog>
#include <QToolBar>
#include <QDialogButtonBox>
#include <QListWidget>

// Non QT includes
#include "can_driverfactory.h"

class Can_SelectCanDriverDialog : public QDialog
{
    Q_OBJECT

public:
    Can_SelectCanDriverDialog(QWidget *parent = 0);
    ~Can_SelectCanDriverDialog();

signals:
    void driverSelected(const QString &str);

private slots:
    void selectClicked();

public slots:
    virtual void setVisible(bool visible);

private:
    Can_DriverFactory *canFactory();
    void setList(const QStringList &list);
    QPushButton *selectButton;
    QPushButton *cancelButton;
    QDialogButtonBox *buttonBox;
    QListWidget *listBox;
};

#endif // CAN_SELECTDRIVERDIALOG_H


