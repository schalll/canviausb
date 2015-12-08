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

#ifndef DRIVERVIEW_H
#define DRIVERVIEW_H

// QT includes
#include <QWidget>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QListWidget>

// Non QT includes
#include "TitleFrame.h"
#include "can/can_driverfactory.h"

class DriverView : public QWidget
{
    Q_OBJECT

public:
    DriverView( QWidget* parent = 0 ); 
    ~DriverView();

public slots:
    void refreshDriverView();

private:
    Can_DriverFactory *canFactory();

    QVBoxLayout *mainLayout;
    TitleFrame *title;
    QWidget *body;
};

/* DriverViewSettings
*  The class is a singleton in order to be accessible from different parts of the application
*  F.e. to use the signal-slot mechanisnm on it
*
*  The class is used in WidgetDrivers.cpp
*
*/

class DriverViewSettings : public QFrame
{
    Q_OBJECT

public:
    static DriverViewSettings* instance(QWidget* parent = 0 );
    ~DriverViewSettings();

private slots:
    void setButtonIcon(bool checked);
    void selectClicked();
    void enableChangeInSettings(bool enable);

private:

    DriverViewSettings(QWidget* parent = 0 );

    Can_DriverFactory *canFactory();
    void setList(const QStringList &list);
    void selectCurrentDriver();

    QVBoxLayout *verticalLayout;
    QPushButton *openSettingsButton;
    QWidget *extension;

    QPushButton *selectButton;
    QDialogButtonBox *buttonBox;
    QListWidget *listBox;
};

#endif
