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
#include <QSettings>

// non QT includes
#include "settings_zcan4usbfx.h"

//***************************************************************************
Settings_ZCAN4USBFX::Settings_ZCAN4USBFX(QWidget *parent)
        : Settings_Driver(parent)
{
    setupUi(this);
    readSettings();

//		m_readRegister = false;
//
//		// Restrict the size of the QLineEdit widgets for the acceptance filters
//    QFontMetrics fm(lineEditFilter0->font());
//    int idPixelsWide = fm.width("88888888888") + 8;
//    lineEditFilter0->setFixedWidth(idPixelsWide);
//    lineEditFilter1->setFixedWidth(idPixelsWide);
//    lineEditFilter2->setFixedWidth(idPixelsWide);
//    lineEditFilter3->setFixedWidth(idPixelsWide);
//    lineEditFilter4->setFixedWidth(idPixelsWide);
//    lineEditFilter5->setFixedWidth(idPixelsWide);

//		connect(cbFilterExt0, SIGNAL(stateChanged(int)), this, SLOT(verifyIDFilter()));
//    connect(lineEditFilter0, SIGNAL(editingFinished()), this, SLOT(verifyIDFilter()));
//    connect(lineEditFilter1, SIGNAL(editingFinished()), this, SLOT(verifyIDFilter()));
//    connect(lineEditFilter2, SIGNAL(editingFinished()), this, SLOT(verifyIDFilter()));
//    connect(lineEditFilter3, SIGNAL(editingFinished()), this, SLOT(verifyIDFilter()));
//    connect(lineEditFilter4, SIGNAL(editingFinished()), this, SLOT(verifyIDFilter()));
//    connect(lineEditFilter5, SIGNAL(editingFinished()), this, SLOT(verifyIDFilter()));

}

Settings_ZCAN4USBFX::~Settings_ZCAN4USBFX() 
{
    writeSettings();
}


//void
//Settings_ZCAN4USBFX::destroy()
//{
//  writeSettings();
//}

void 
        Settings_ZCAN4USBFX::setDeviceName(const QString &deviceName)
{
    labelDeviceNameInfo->setText(deviceName);
}

void 
        Settings_ZCAN4USBFX::setManufacturer(const QString &manufacturerName)
{
    labelDeviceManufacturerInfo->setText(manufacturerName);
}

void 
        Settings_ZCAN4USBFX::setSoftwareVersion(const QString &softwareVersion)
{
    labelDeviceVersionInfo->setText(softwareVersion);
}

void 
        Settings_ZCAN4USBFX::setDeviceStatus(const QString &deviceStatus)
{
    labelDeviceStatusInfo->setText(deviceStatus);
}

void 
        Settings_ZCAN4USBFX::setEditable(bool edit) {
    Q_UNUSED(edit);
    //deviceSettings->setEnabled(edit);
}

bool 
Settings_ZCAN4USBFX::useFilter() {
	return false;
//	return cbUseFilter->isChecked();
}


unsigned int 
Settings_ZCAN4USBFX::getFilter(unsigned char num) {
	unsigned int filter;
	filter = 0;
/*
	switch(num) {
		case 0:
			filter = QVariant(lineEditFilter0->text()).toUInt();
			if (cbFilterExt0->isChecked()) filter = filter | 0x80000000;
			break;
		case 1:
			filter = QVariant(lineEditFilter1->text()).toUInt();
			if (cbFilterExt1->isChecked()) filter = filter | 0x80000000;
			break;
		case 2:
			filter = QVariant(lineEditFilter2->text()).toUInt();
			if (cbFilterExt2->isChecked()) filter = filter | 0x80000000;
			break;
		case 3:
			filter = QVariant(lineEditFilter3->text()).toUInt();
			if (cbFilterExt3->isChecked()) filter = filter | 0x80000000;
			break;
		case 4:
			filter = QVariant(lineEditFilter4->text()).toUInt();
			if (cbFilterExt4->isChecked()) filter = filter | 0x80000000;
			break;
		case 5:
			filter = QVariant(lineEditFilter5->text()).toUInt();
			if (cbFilterExt5->isChecked()) filter = filter | 0x80000000;
			break;
	}
	*/
	return filter;
}

void 
        Settings_ZCAN4USBFX::on_comboBoxBaudRate_currentIndexChanged(int value) {
    // TODO - Eventually handle a special setting for the baudrate
    switch(value) {
                case 0:
                    m_baudrate = 10;
                    break;
                case 1:
                    m_baudrate = 20;
                    break;
                case 2:
                    m_baudrate = 50;
                    break;
                case 3:
                    m_baudrate = 100;
                    break;
                case 4:
                    m_baudrate = 125;
                    break;
                case 5:
                    m_baudrate = 250;
                    break;
                case 6:
                    m_baudrate = 500;
                    break;
                case 7:
                    m_baudrate = 800;
                    break;
                case 8:
                    m_baudrate = 1000;
                    break;
                }
}

void 
        Settings_ZCAN4USBFX::on_checkBoxTimeStamp_stateChanged (int state) {
/*
			switch(state) {
                case 2:
                    m_timestamp = true;
                    break;
                default:
                    m_timestamp = false;
                    break;
                }
*/
}

void 
Settings_ZCAN4USBFX::on_pushButtonReadRegister_clicked() {
//	m_readRegister = true;
}

int 
        Settings_ZCAN4USBFX::baudrate(void) {
    return m_baudrate;
}

bool 
        Settings_ZCAN4USBFX::useTimestamp(void) {
	return false;
			//    return m_timestamp;
}

bool 
        Settings_ZCAN4USBFX::readRegister(void) {
return false;
			/*    bool tmp;
		tmp = m_readRegister;
		if (tmp) m_readRegister = false;
		return tmp;
*/
}

void 
        Settings_ZCAN4USBFX::disable_ui() {
    deviceSettings->setEnabled(false);
//		registersDisplay->setEnabled(false);
}

void 
        Settings_ZCAN4USBFX::enable_ui() {
    deviceSettings->setEnabled(true);
//		registersDisplay->setEnabled(true);
}



void 
Settings_ZCAN4USBFX::disable_uiWhileNotReady() {
//	pushButtonReadRegister->setEnabled(false);

}

void 
Settings_ZCAN4USBFX::enable_uiWhileReady() {
//	pushButtonReadRegister->setEnabled(true);
}


void
        Settings_ZCAN4USBFX::writeSettings()
{
    QSettings settings;
    settings.setValue("ZCAN4USBFX/baudrate",m_baudrate);
/*
	settings.setValue("ZCAN4USBFX/timestamp",m_timestamp);

		// Filter settings
    settings.setValue(QString("ZCAN4USBFX/UseFilter"),cbUseFilter->isChecked());
		settings.setValue(QString("ZCAN4USBFX/Filter0/ext"),cbFilterExt0->isChecked());
    settings.setValue(QString("ZCAN4USBFX/Filter0/value"),lineEditFilter0->text());
    settings.setValue(QString("ZCAN4USBFX/Filter1/ext"),cbFilterExt1->isChecked());
    settings.setValue(QString("ZCAN4USBFX/Filter1/value"),lineEditFilter1->text());
    settings.setValue(QString("ZCAN4USBFX/Filter2/ext"),cbFilterExt2->isChecked());
    settings.setValue(QString("ZCAN4USBFX/Filter2/value"),lineEditFilter2->text());
    settings.setValue(QString("ZCAN4USBFX/Filter3/ext"),cbFilterExt3->isChecked());
    settings.setValue(QString("ZCAN4USBFX/Filter3/value"),lineEditFilter3->text());
    settings.setValue(QString("ZCAN4USBFX/Filter4/ext"),cbFilterExt4->isChecked());
    settings.setValue(QString("ZCAN4USBFX/Filter4/value"),lineEditFilter4->text());
    settings.setValue(QString("ZCAN4USBFX/Filter5/ext"),cbFilterExt5->isChecked());
    settings.setValue(QString("ZCAN4USBFX/Filter5/value"),lineEditFilter5->text());
*/
}

void 
        Settings_ZCAN4USBFX::readSettings()
{
    QSettings settings;
    // Baudrate
    m_baudrate = settings.value("ZCAN4USBFX/baudrate",10).toInt();
    switch(m_baudrate) {
                case 10:
                    comboBoxBaudRate->setCurrentIndex(0);
                    break;
                case 20:
                    comboBoxBaudRate->setCurrentIndex(1);
                    break;
                case 50:
                    comboBoxBaudRate->setCurrentIndex(2);
                    break;
                case 100:
                    comboBoxBaudRate->setCurrentIndex(3);
                    break;
                case 125:
                    comboBoxBaudRate->setCurrentIndex(4);
                    break;
                case 250:
                    comboBoxBaudRate->setCurrentIndex(5);
                    break;
                case 500:
                    comboBoxBaudRate->setCurrentIndex(6);
                    break;
                case 800:
                    comboBoxBaudRate->setCurrentIndex(7);
                    break;
                case 1000:
                    comboBoxBaudRate->setCurrentIndex(8);
                    break;
                default:
                    // TO DO - Eventually handle a special setting for the baudrate
                    m_baudrate = 10;
                    comboBoxBaudRate->setCurrentIndex(0);
                    break;
                }
/*
	// Timestamp
    m_timestamp = settings.value("ZCAN4USBFX/timestamp",false).toBool();
    if (m_timestamp) checkBoxTimeStamp->setCheckState(Qt::Checked);
    else checkBoxTimeStamp->setCheckState(Qt::Unchecked);

		// Filter settings
		cbUseFilter->setChecked(settings.value(QString("ZCAN4USBFX/UseFilter"),false).toBool());
		cbFilterExt0->setChecked(settings.value(QString("ZCAN4USBFX/Filter0/ext"),false).toBool());
    lineEditFilter0->setText(settings.value(QString("ZCAN4USBFX/Filter0/value"),"0").toString());
		cbFilterExt1->setChecked(settings.value(QString("ZCAN4USBFX/Filter1/ext"),false).toBool());
    lineEditFilter1->setText(settings.value(QString("ZCAN4USBFX/Filter1/value"),"0").toString());
		cbFilterExt2->setChecked(settings.value(QString("ZCAN4USBFX/Filter2/ext"),false).toBool());
    lineEditFilter2->setText(settings.value(QString("ZCAN4USBFX/Filter2/value"),"0").toString());
		cbFilterExt3->setChecked(settings.value(QString("ZCAN4USBFX/Filter3/ext"),false).toBool());
    lineEditFilter3->setText(settings.value(QString("ZCAN4USBFX/Filter3/value"),"0").toString());
		cbFilterExt4->setChecked(settings.value(QString("ZCAN4USBFX/Filter4/ext"),false).toBool());
    lineEditFilter4->setText(settings.value(QString("ZCAN4USBFX/Filter4/value"),"0").toString());
		cbFilterExt5->setChecked(settings.value(QString("ZCAN4USBFX/Filter5/ext"),false).toBool());
    lineEditFilter5->setText(settings.value(QString("ZCAN4USBFX/Filter5/value"),"0").toString());

		verifyIDFilter();
*/
}

void 
Settings_ZCAN4USBFX::verifyIDFilter() {
/*
	verifyIDFilterFor(cbFilterExt0,lineEditFilter0);
	verifyIDFilterFor(cbFilterExt1,lineEditFilter1);
	verifyIDFilterFor(cbFilterExt2,lineEditFilter2);
	verifyIDFilterFor(cbFilterExt3,lineEditFilter3);
	verifyIDFilterFor(cbFilterExt4,lineEditFilter4);
	verifyIDFilterFor(cbFilterExt5,lineEditFilter5);
*/
}

void 
Settings_ZCAN4USBFX::verifyIDFilterFor(QCheckBox *cb, QLineEdit *le) {
/*
    // First make sure the input mask are set correctly !
    if (!cb->isChecked()) {
        // 11 Bits identifier
        QRegExp id11RegExp("0x[0-9A-Fa-f]{,3}|\\d{,4}");
        le->setValidator(new QRegExpValidator(id11RegExp, this));
    } else {
        // 29 Bits identifier
        QRegExp id29RegExp("0x[0-9A-Fa-f]{,8}|\\d{,9}");
        le->setValidator(new QRegExpValidator(id29RegExp, this));
    }

    bool ok;
    if (!le->text().startsWith("0x")) {
        // Decimal entry
        if (!cb->isChecked()) {
            // 11 Bits identifier
            if (QVariant(le->text()).toUInt() > 0x7FF) {
                le->setText("2047");
            }
        } else {
            // 29 Bits identifier
            if (QVariant(lineEditFilter0->text()).toUInt() > 0x1FFFFFFF) {
                le->setText("536870911");
            }
        }
    } else {
        // Hexadecimal entry
        unsigned int tmp = QString(le->text()).toUInt(&ok,16);
        if (!cb->isChecked()) {
            // 11 Bits identifier
            if (tmp > 2047) {
                le->setText("0x1FF");
            }
        } else {
            // 29 Bits identifier
            if (tmp > 536870911) {
                le->setText("0x1FFFFFFF");
            }
        }
    }
*/
}

void 
Settings_ZCAN4USBFX::setRegisterValues(unsigned char column, unsigned char *values) {
/*
	for (unsigned int row=0;row<16;row++) {
		QTableWidgetItem *item = new QTableWidgetItem();
		//item->setText(QString("(0x%1)/%2").arg(values[row], 2, 16, QLatin1Char('0')).arg(values[row]));
		item->setText(QString("0x%1").arg(values[row], 2, 16, QLatin1Char('0')));
		item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		registersTable->setItem(row, column, item);
	}
*/
}
