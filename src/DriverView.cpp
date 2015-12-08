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
#include "DriverView.h"

DriverView::DriverView( QWidget* parent ) :
        QWidget( parent )
{
    body = canFactory()->actualDriverUserInterface();

    // Layout
    mainLayout = new QVBoxLayout;
    //mainLayout->addWidget(title);
    mainLayout->addWidget(body);
    mainLayout->setContentsMargins (0, 0, 0, 0);
    setLayout(mainLayout);

    connect(canFactory(),SIGNAL(driverHasChanged(QString)), this, SLOT(refreshDriverView()));

}

DriverView::~DriverView()
{

}

Can_DriverFactory *
        DriverView::canFactory() {
    return Can_DriverFactory::instance(this);
}

void 
        DriverView::refreshDriverView() {
    body->hide();
    mainLayout->removeWidget(body);
    //body = canFactory()->driverUserInterface(canFactory()->currentDriver());
    body = canFactory()->actualDriverUserInterface();
    mainLayout->addWidget(body);
    body->show();
}

// ##############################################

DriverViewSettings::DriverViewSettings(QWidget* parent ) :
        QFrame( parent ) {

    //QCursor cursor(Qt::PointingHandCursor);

    QLabel *selectingDriverLabel = new QLabel(tr("Select the CAN device:"),this);
    listBox = new QListWidget(this);
    connect(listBox, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this , SLOT(selectClicked()));

    // Select Button
    selectButton = new QPushButton(tr("&Select"), this);
    buttonBox = new QDialogButtonBox(Qt::Vertical);
    buttonBox->addButton(selectButton, QDialogButtonBox::ActionRole);
    connect(selectButton, SIGNAL(clicked()), this, SLOT(selectClicked()));
    setList(canFactory()->driverList());

    extension = new QWidget;

    QHBoxLayout *extensionLayout = new QHBoxLayout;
    extensionLayout->setMargin(0);
    extensionLayout->addWidget(listBox, 1);
    extensionLayout->addWidget(buttonBox);
    extensionLayout->addStretch();

    QVBoxLayout *extensionVLayout = new QVBoxLayout;
    extensionVLayout->addWidget(selectingDriverLabel);
    extensionVLayout->addLayout(extensionLayout);

    extension->setLayout(extensionVLayout);
    extension->setVisible(false);

    QLabel *settingsLabel = new QLabel(tr("Select CAN device:"),this);
    openSettingsButton = new QPushButton("", this);
    openSettingsButton->setFlat(true);
    openSettingsButton->setCheckable(true);
    openSettingsButton->setIcon(QIcon( ":/nav_up_blue.png" ));
    openSettingsButton->setCursor(QCursor(Qt::PointingHandCursor));

    QHBoxLayout *openSettingsLayout = new QHBoxLayout;
    openSettingsLayout->addStretch();
    openSettingsLayout->addWidget(settingsLabel);
    openSettingsLayout->addWidget(openSettingsButton);

    verticalLayout = new QVBoxLayout;
    verticalLayout->addLayout(openSettingsLayout);
    verticalLayout->addWidget(extension);

    setLayout(verticalLayout);

    connect(openSettingsButton, SIGNAL(toggled(bool)), extension, SLOT(setVisible(bool)));
    connect(openSettingsButton, SIGNAL(toggled(bool)), this, SLOT(setButtonIcon(bool)));
    connect(canFactory(), SIGNAL(driverIsCommunicating(bool)),this, SLOT(enableChangeInSettings(bool)));
    setFixedHeight(50);

}


DriverViewSettings::~DriverViewSettings() {

}

DriverViewSettings*
        DriverViewSettings::instance(QWidget* parent) {

    static DriverViewSettings* singleton = 0;
    if (singleton == 0) {
        singleton = new DriverViewSettings(parent);
    }
    return singleton;


}

void
        DriverViewSettings::setButtonIcon(bool checked) {
    if (checked) {
        openSettingsButton->setIcon(QIcon( ":/nav_down_blue.png" ));
        setFixedHeight(200);
        selectCurrentDriver();
    } else {
        openSettingsButton->setIcon(QIcon( ":/nav_up_blue.png" ));
        setFixedHeight(50);
    }
}

Can_DriverFactory *
        DriverViewSettings::canFactory() {
    return Can_DriverFactory::instance(this);
}

void 
        DriverViewSettings::setList(const QStringList &list)
{
    listBox->clear();
    for (int i = 0; i < (int)list.count(); i++) {
        listBox->addItem(list[i]);
    }
}

void 
        DriverViewSettings::selectClicked() {
    // Get the name of the current selected driver
    QString driverName = listBox->currentItem()->text();
    // Emit a signal if this driver is different from the previous selected one
    if (driverName != canFactory()->currentDriver())
        canFactory()->selectDriver(driverName);
}

void 
        DriverViewSettings::selectCurrentDriver() {
    // First select the current driver
    for(int i=0; i<listBox->count(); i++){
        QListWidgetItem *item = listBox->item(i);
        if(item->text() == canFactory()->currentDriver())
            listBox->setCurrentItem(item);
    }
}

void
        DriverViewSettings::enableChangeInSettings(bool isCommunicating) {
    buttonBox->setDisabled(isCommunicating);
    listBox->setDisabled(isCommunicating);
}
