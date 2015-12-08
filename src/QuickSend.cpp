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
#include <QLineEdit>

// non QT includes
#include "QuickSend.h"
#include "can/can_frame.h"


QuickSend::QuickSend( QWidget* parent ) :
        QFrame( parent )
{

    m_quickSend = new QuickSendWidget(this);
	connect(m_quickSend, SIGNAL(contentChanged()),this, SLOT(setContentChanged()));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_quickSend);
    layout->addStretch();
    setLayout(layout);

    setMaximumHeight(120);

}

QuickSend::~QuickSend()
{

}

void 
QuickSend::setContentChanged() {
	emit contentChanged();
}

// ##############################################

QuickSendWidget::QuickSendWidget(QWidget* parent) :
	QTabWidget( parent ) {
		
		for (int i=1; i<16; i++) {
			m_sendItems[i-1] = new QuickSendItem(i,this);
			addTab(m_sendItems[i-1],QString("%1").arg(i));
			connect(m_sendItems[i-1], SIGNAL(contentChanged()),this, SLOT(setContentChanged()));
		}
    setMaximumHeight(100);
}


QuickSendWidget::~QuickSendWidget() {
}

void 
QuickSendWidget::setContentChanged() {
	emit contentChanged();
}

// ##############################################

QuickSendItem::QuickSendItem(unsigned int index, QWidget* parent) :
        QWidget( parent ) {

    m_index = index;
    int i;

    extLabel = new QLabel("Ext",this);
    extLabel->setAlignment(Qt::AlignHCenter);
    extCheckbox = new QCheckBox("", this);
    connect(extCheckbox, SIGNAL(stateChanged(int)), this, SLOT(verifyID()));
	
    rtrLabel = new QLabel("RTR",this);
    rtrLabel->setAlignment(Qt::AlignHCenter);
    rtrCheckbox = new QCheckBox("", this);
	connect(rtrCheckbox, SIGNAL(stateChanged(int)), this, SLOT(rtrChanged()));

    idLabel = new QLabel("ID",this);
    idLabel->setAlignment(Qt::AlignHCenter);

    idLineEdit = new QLineEdit(this);
    connect(idLineEdit, SIGNAL(editingFinished()), this, SLOT(verifyID()));

    dlcLabel = new QLabel("DLC",this);
    dlcLabel->setAlignment(Qt::AlignHCenter);
    dlcSpinBox = new QSpinBox(this);
    dlcSpinBox->setMaximum(8);
    connect(dlcSpinBox, SIGNAL(valueChanged (int)), this, SLOT(dlcChanged(int)));

    for (i=0; i<8; i++) {
        dLabel[i] = new QLabel(QString("D%1").arg(i),this);
        dLabel[i]->setAlignment(Qt::AlignHCenter);
        dLineEdit[i] = new QLineEdit(this);
        QRegExp dRegExp("0x[0-9A-Fa-f]{,2}|\\d{,3}");
        dLineEdit[i]->setValidator(new QRegExpValidator(dRegExp, this));
        connect(dLineEdit[i], SIGNAL(editingFinished()), this, SLOT(verifyDatas()));
    }

    repeatLabel = new QLabel("Cyclic",this);
    repeatLabel->setAlignment(Qt::AlignHCenter);
    repeatSpinBox = new QSpinBox(this);
    repeatSpinBox->setMaximum(600);
    repeatSpinBox->adjustSize();
    connect(repeatSpinBox, SIGNAL(valueChanged (int)), this, SLOT(verifyRepeatTime(int)));

    msLabel = new QLabel("s  ",this);
    msLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    sendButton = new QPushButton(tr("Send"),this);
	sendButton->setEnabled(false);
    connect(sendButton, SIGNAL(clicked()), this, SLOT(send()));

    attributesLayout = new QGridLayout;
    attributesLayout->addWidget(extLabel, 0, 0);
    attributesLayout->addWidget(extCheckbox, 1, 0);
    attributesLayout->addWidget(rtrLabel, 0, 1);
    attributesLayout->addWidget(rtrCheckbox, 1, 1);
    attributesLayout->addWidget(idLabel, 0, 2);
    attributesLayout->addWidget(idLineEdit, 1, 2);
    attributesLayout->addWidget(dlcLabel, 0, 3);
    attributesLayout->addWidget(dlcSpinBox, 1, 3);
    attributesLayout->setSpacing(2);

    dataLayout = new QGridLayout;
    for (i=0; i<8; i++) {
        dataLayout->addWidget(dLabel[i], 0, i);
        dataLayout->addWidget(dLineEdit[i], 1, i);
    }
    dataLayout->setSpacing(0);

    sendLayout = new QGridLayout;
    sendLayout->addWidget(sendButton, 1, 0);
    sendLayout->addWidget(repeatLabel, 0, 1);
    sendLayout->addWidget(repeatSpinBox, 1, 1);
    sendLayout->addWidget(msLabel, 1, 2);
    sendLayout->setSpacing(2);

    layout = new QHBoxLayout;
    layout->addLayout(attributesLayout);
    layout->addLayout(dataLayout);
    layout->addLayout(sendLayout);
    setLayout(layout);

    // Restrict the size of the QLineEdit widgets
    QFontMetrics fm(idLineEdit->font());
    int idPixelsWide = fm.width("88888888888") + 8;
    idLineEdit->setFixedWidth(idPixelsWide);

    dlcSpinBox->adjustSize();

    for (i=0; i<8; i++) {
        int dataPixelsWide = fm.width("8888") + 8;
        dLineEdit[i]->setFixedWidth(dataPixelsWide);
    }

    //repeatLineEdit->setFixedWidth(repeatPixelsWide);
    repeatSpinBox->adjustSize();

    QSettings settings;
    extCheckbox->setChecked(settings.value(QString("LogView/quickSend%1/ext").arg(m_index),true).toBool());
    rtrCheckbox->setChecked(settings.value(QString("LogView/quickSend%1/rtr").arg(m_index),false).toBool());
    idLineEdit->setText(settings.value(QString("LogView/quickSend%1/id").arg(m_index),"0").toString());
    int tmp_dlc = settings.value(QString("LogView/quickSend%1/dlc").arg(m_index),"0").toInt();
    dlcSpinBox->setValue(tmp_dlc);
    dlcChanged(tmp_dlc);

    for (i=0; i<8; i++) {
        dLineEdit[i]->setText(settings.value(QString("LogView/quickSend%1/data%2").arg(m_index).arg(i),"0").toString());
    }
    repeatSpinBox->setValue(settings.value(QString("LogView/quickSend%1/repeat").arg(m_index),"0").toInt());
    connect(canFactory(), SIGNAL(driverIsCommunicating(bool)),sendButton, SLOT(setEnabled(bool)));
	connect(sendButton, SIGNAL(toggled(bool)),repeatSpinBox, SLOT(setDisabled(bool)));
	connect(sendButton, SIGNAL(toggled(bool)),this, SLOT(setAutorepeat(bool)));
}

QuickSendItem::~QuickSendItem() {
    QSettings settings;
    settings.setValue(QString("LogView/quickSend%1/ext").arg(m_index),extCheckbox->isChecked());
    settings.setValue(QString("LogView/quickSend%1/rtr").arg(m_index),rtrCheckbox->isChecked());
    settings.setValue(QString("LogView/quickSend%1/id").arg(m_index),idLineEdit->text());
    settings.setValue(QString("LogView/quickSend%1/dlc").arg(m_index),dlcSpinBox->value());
    for (int i=0; i<8; i++) {
        settings.setValue(QString("LogView/quickSend%1/data%2").arg(m_index).arg(i),dLineEdit[i]->text());
    }
    settings.setValue(QString("LogView/quickSend%1/repeat").arg(m_index),repeatSpinBox->value());
}

void 
        QuickSendItem::dlcChanged(int dlc) {
    int i;
    for (i=0;i<dlc;i++) {
        dLineEdit[i]->setEnabled(true);
    }
    for (i=dlc;i<8;i++) {
        dLineEdit[i]->setEnabled(false);
    }
	emit contentChanged();
}

Can_DriverFactory *
        QuickSendItem::canFactory() {
    return Can_DriverFactory::instance(this);
}

void 
        QuickSendItem::verifyID() {
    // First make sure the input mask are set correctly !
    if (!extCheckbox->isChecked()) {
        // 11 Bits identifier
        QRegExp id11RegExp("0x[0-9A-Fa-f]{,3}|\\d{,4}");
        idLineEdit->setValidator(new QRegExpValidator(id11RegExp, this));
    } else {
        // 29 Bits identifier
        QRegExp id11RegExp("0x[0-9A-Fa-f]{,8}|\\d{,9}");
        idLineEdit->setValidator(new QRegExpValidator(id11RegExp, this));
    }

    bool ok;
    if (!idLineEdit->text().startsWith("0x")) {
        // Decimal entry
        if (!extCheckbox->isChecked()) {
            // 11 Bits identifier
            if (QVariant(idLineEdit->text()).toUInt() > 0x7FF) {
                idLineEdit->setText("2047");
            }
        } else {
            // 29 Bits identifier
            if (QVariant(idLineEdit->text()).toUInt() > 0x1FFFFFFF) {
                idLineEdit->setText("536870911");
            }
        }
    } else {
        // Hexadecimal entry
        unsigned int tmp = QString(idLineEdit->text()).toUInt(&ok,16);
        if (!extCheckbox->isChecked()) {
            // 11 Bits identifier
            if (tmp > 2047) {
                idLineEdit->setText("0x1FF");
            }
        } else {
            // 29 Bits identifier
            if (tmp > 536870911) {
                idLineEdit->setText("0x1FFFFFFF");
            }
        }
    }

	emit contentChanged();
}

void 
        QuickSendItem::verifyDatas() {
    for (int i=0;i<8;i++) {
        if (!dLineEdit[i]->text().startsWith("0x")) {
            if (QVariant(dLineEdit[i]->text()).toUInt() > 255) {
                dLineEdit[i]->setText("255");
            }
        }
    }
	emit contentChanged();
}

void 
        QuickSendItem::verifyRepeatTime(int time) {
		if (time>0) {
                sendButton->setCheckable(true);
                sendButton->setAutoRepeat(true);
                sendButton->setAutoRepeatInterval(time);
                sendButton->setAutoRepeatDelay(time);
        } else {
                sendButton->setCheckable(false);
                sendButton->setAutoRepeat(false);
        }
		emit contentChanged();

}

void
QuickSendItem::rtrChanged() {
	emit contentChanged();
}

void 
QuickSendItem::setAutorepeat(bool repeat) {
	m_inhibitButtonTimerId = startTimer(100);
	sendButton->setEnabled(false);
	if (repeat) {
		qDebug("Start Auto repeat timer");
		m_repeatTimerId = startTimer(repeatSpinBox->value()*1000);
	} else {
		qDebug("Stop Auto repeat timer");
		killTimer(m_repeatTimerId);
	}

}

void 
QuickSendItem::timerEvent(QTimerEvent *event) {
    if (event->timerId() == m_repeatTimerId) {
		send();
    } else if (event->timerId() == m_inhibitButtonTimerId) {
		sendButton->setEnabled(true);
	} else {
        QWidget::timerEvent(event);
    }	
}

void
        QuickSendItem::send() {
    
	// Special treatment - if the button is checkable and not checked anymore, then we are not going to send the message
	if (sendButton->isCheckable()) {
		if (!sendButton->isChecked())
			return;
	}
	
		
	bool ok;
    unsigned char data[8];
    for (int i=0;i<8;i++) {
        if (!dLineEdit[i]->text().startsWith("0x")) {
            data[i] = (unsigned char)QVariant(dLineEdit[i]->text()).toUInt();
        } else {
            data[i] = (unsigned char)QString(dLineEdit[i]->text()).toUInt(&ok,16);
        }
    }

    unsigned int id;
    if (!idLineEdit->text().startsWith("0x")) {
        id = QVariant(idLineEdit->text()).toUInt();
    } else {
        id = QString(idLineEdit->text()).toUInt(&ok,16);
    }

    canFactory()->send(
            Can_Frame(
                    1, //canDirection::send,
                    QDateTime::currentDateTime(),
                    extCheckbox->isChecked(),
                    rtrCheckbox->isChecked(),
                    id,
                    dlcSpinBox->value(),
                    data
                    )
            );

}

