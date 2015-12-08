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

#ifndef LOGVIEW_H
#define LOGVIEW_H

// QT includes
#include <QPushButton>

// Non QT includes
#include "can/can_frame.h"
#include "can/can_driverfactory.h"


class QuickSendWidget;
class QuickSendItem;

class QuickSend : public QFrame
{
    Q_OBJECT
    Q_ENUMS(canDirection)

        public:
            QuickSend( QWidget* parent = 0);
    ~QuickSend();


signals:
    void contentChanged();

public slots:
	void setContentChanged();


private:
    QuickSendWidget *m_quickSend;
};

class QuickSendWidget : public QTabWidget
{
    Q_OBJECT

signals:
    void contentChanged();

public slots:
	void setContentChanged();

public:
    QuickSendWidget(QWidget* parent = 0 );
    ~QuickSendWidget();

private:
	QuickSendItem* m_sendItems[16];

};

/* QuickSendItem
*
*
*/

class QuickSendItem : public QWidget
{
    Q_OBJECT

public:
    QuickSendItem(unsigned int index, QWidget* parent = 0);
    ~QuickSendItem();

signals:
    void contentChanged();

private slots:
    void dlcChanged(int dlc);
    void verifyID();
    void verifyDatas();
    void verifyRepeatTime(int time);
    void send();
	void rtrChanged();
	void setAutorepeat(bool repeat);

protected:
    void timerEvent(QTimerEvent *event);

private:
    unsigned int m_index;

    // extended or simple
    QLabel *extLabel;
    QCheckBox *extCheckbox;

    //RTR
    QLabel *rtrLabel;
    QCheckBox *rtrCheckbox;

    // Can identifier
    QLabel *idLabel;
    QLineEdit *idLineEdit;

    // Message DLC
    QLabel *dlcLabel;
    QSpinBox *dlcSpinBox;

    // Message datas
    QLabel *dLabel[8];
    QLineEdit *dLineEdit[8];

    // Repeat
    QLabel *repeatLabel;
    QSpinBox *repeatSpinBox;
    QLabel *msLabel;

    QPushButton *sendButton;
    QGridLayout *attributesLayout;
    QGridLayout *dataLayout;
    QGridLayout *sendLayout;
    QHBoxLayout *layout;

    Can_DriverFactory *canFactory();

	int m_repeatTimerId;
	int m_inhibitButtonTimerId;
};

#endif
