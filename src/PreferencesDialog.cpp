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
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>

// non QT includes
#include "PreferencesDialog.h"
#include "appSettings.h"

PreferencesDialog::PreferencesDialog(QWidget *parent)
        : QDialog(parent)
{
    setModal(true);

    tabWidget = new QTabWidget;
    displayTab = new DisplayTab(this);
    displayTab->loadSettings();
    tabWidget->addTab(displayTab, tr("Display"));

    //filterTab = new FilterTab(this);
    //tabWidget->addTab(filterTab, tr("Filter"));

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setMinimumWidth(300);
    setWindowTitle(tr("Preferences"));
}

void 
        PreferencesDialog::accept() {
    displayTab->saveSettings();
    emit settingsChanged();
    hide();
}

void 
        PreferencesDialog::reject() {
    displayTab->loadSettings();
    hide();
}



DisplayTab::DisplayTab(QWidget *parent)
        : QWidget(parent)
{

    settings = AppSettings::instance();
    QGroupBox *timestampGroupBox = new QGroupBox(tr("Timestamp"));;
    QVBoxLayout *timestampLayout = new QVBoxLayout;
    checkboxPCTime = new QCheckBox(tr("&PC Time"), this);
    checkboxAdapterTime = new QCheckBox("&Adapter Time", this);
    // Disable this for now !!!
    checkboxAdapterTime->setEnabled(false);

    timestampLayout->addWidget(checkboxPCTime);
    timestampLayout->addWidget(checkboxAdapterTime);
    timestampGroupBox->setLayout(timestampLayout);

    QGroupBox *identifierGroupBox = new QGroupBox(tr("CAN identifier"));;
    QHBoxLayout *identifierLayout = new QHBoxLayout;
    QVBoxLayout *identifierDisplayLayout = new QVBoxLayout;
    decimalID = new QRadioButton(tr("Decimal"), this);
    hexadecimalID = new QRadioButton(tr("Hexadecimal"), this);
    decimalAndHexadecimalID = new QRadioButton(tr("Hex/Dec"), this);
    identifierDisplayLayout->addWidget(decimalID);
    identifierDisplayLayout->addWidget(hexadecimalID);
    identifierDisplayLayout->addWidget(decimalAndHexadecimalID);
    checkboxIDName = new QCheckBox(tr("&Name"), this);

    identifierLayout->addLayout(identifierDisplayLayout);
    identifierLayout->addWidget(checkboxIDName);
    identifierGroupBox->setLayout(identifierLayout);

    QGroupBox *messageGroupBox = new QGroupBox(tr("CAN message"));
    QHBoxLayout *messageLayout = new QHBoxLayout;
    QVBoxLayout *messageDisplayLayout = new QVBoxLayout;
    decimalMessage = new QRadioButton(tr("Decimal"), this);
    hexadecimalMessage = new QRadioButton(tr("Hexadecimal"), this);
    messageDisplayLayout->addWidget(decimalMessage);
    messageDisplayLayout->addWidget(hexadecimalMessage);
    checkboxMessageAscii = new QCheckBox(tr("ASCII"), this);
    messageLayout->addLayout(messageDisplayLayout);
    messageLayout->addWidget(checkboxMessageAscii);
    messageGroupBox->setLayout(messageLayout);

    QGroupBox *refreshGroupBox = new QGroupBox(tr("Refreshing rate"));
    QHBoxLayout *refreshLayout = new QHBoxLayout;
    refreshRateSpinBox = new QSpinBox(this);
    refreshRateSpinBox->setMinimum(100);
    refreshRateSpinBox->setMaximum(10000);
    refreshRateSpinBox->setSingleStep(50);
    QLabel *refreshRateLabel = new QLabel("ms",this);
    refreshLayout->addWidget(refreshRateSpinBox);
    refreshLayout->addWidget(refreshRateLabel);
    refreshGroupBox->setLayout(refreshLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(timestampGroupBox);
    mainLayout->addWidget(identifierGroupBox);
    mainLayout->addWidget(messageGroupBox);
    mainLayout->addWidget(refreshGroupBox);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

void DisplayTab::loadSettings() {
    int tmp;

    checkboxPCTime->setChecked(settings->displayPCTimestamp());
    checkboxAdapterTime->setChecked(settings->displayAdapterTimestamp());
    tmp = settings->displayIDFormat();
    switch (tmp) {
                case 0:
        decimalID->setChecked(true);
        break;
                case 1:
        hexadecimalID->setChecked(true);
        break;
                case 2:
        decimalAndHexadecimalID->setChecked(true);
        break;
                default:
        hexadecimalID->setChecked(true);
    }
    checkboxIDName->setChecked(settings->displayMessageName());

    tmp = settings->displayMessageFormat();
    switch (tmp) {
                case 0:
        decimalMessage->setChecked(true);
        break;
                case 1:
        hexadecimalMessage->setChecked(true);
        break;
                default:
        hexadecimalMessage->setChecked(true);
    }
    checkboxMessageAscii->setChecked(settings->displayMessageAscii());

    refreshRateSpinBox->setValue(settings->displayRefreshRate());
}

void DisplayTab::saveSettings() {
    settings->enablePCTimestamp(checkboxPCTime->isChecked());
    settings->enableAdapterTimestamp(checkboxAdapterTime->isChecked());
    if (decimalID->isChecked()) settings->setIDFormat(0);
    if (hexadecimalID->isChecked()) settings->setIDFormat(1);
    if (decimalAndHexadecimalID->isChecked()) settings->setIDFormat(2);
    if (decimalMessage->isChecked()) settings->setMessageFormat(0);
    if (hexadecimalMessage->isChecked()) settings->setMessageFormat(1);
    settings->enableMessageDisplayName(checkboxIDName->isChecked());
    settings->enableMessageDisplayAscii(checkboxMessageAscii->isChecked());
    settings->setdisplayRefreshRate(refreshRateSpinBox->value());
}

/*
FilterTab::FilterTab(QWidget *parent)
   : QWidget(parent)
{
}
*/
