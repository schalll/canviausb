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

#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

// QT includes
#include <QDialog>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>

// non QT includes
#include "appSettings.h"

class QDialogButtonBox;
class QFileInfo;
class QTabWidget;

class DisplayTab : public QWidget
{
    Q_OBJECT

public:
    DisplayTab(QWidget *parent = 0);

    void saveSettings();
    void loadSettings();

private:
    AppSettings *settings;
    QCheckBox *checkboxPCTime;
    QCheckBox *checkboxAdapterTime;
    QRadioButton *decimalID;
    QRadioButton *hexadecimalID;
    QRadioButton *decimalAndHexadecimalID;
    QCheckBox *checkboxIDName;
    QRadioButton *decimalMessage;
    QRadioButton *hexadecimalMessage;
    QCheckBox *checkboxMessageAscii;
    QSpinBox *refreshRateSpinBox;
};

/*
class FilterTab : public QWidget
{
   Q_OBJECT

public:
   FilterTab(QWidget *parent = 0);

private:
};

*/
class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    PreferencesDialog(QWidget *parent = 0);

signals:
    void settingsChanged();

public slots:
    void accept();
    void reject();

private:
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;
    DisplayTab *displayTab;
    //FilterTab *filterTab;
};

#endif
