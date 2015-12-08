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
#include <QLabel>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>

// non QT includes
#include "TitleFrame.h"
#include "utilities/searchlineedit.h"


TitleFrame::TitleFrame( QWidget* parent, QString title) :
        QFrame( parent )
{
	//setMinimumHeight(60);
    m_title = title;
    m_titleLabel = new QLabel(this);
    m_titleLabel->setText(m_title);
    //m_titleLabel->setStyleSheet("color: #505050; font: bold;");
	m_titleLabel->setStyleSheet("color: white; font: bold;");

	m_alphaLabel = new QLabel(this);
	m_alphaLabel->setText("ALPHA VERSION");
	m_alphaLabel->setStyleSheet("color: red; font: bold;");


	QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_titleLabel);
    layout->addStretch();
	layout->addWidget(m_alphaLabel);
	setLayout(layout);
}

TitleFrame::~TitleFrame()
{

}

void 
        TitleFrame::setInfo(QString info) {
    if (info != "") {
        m_titleLabel->setText(QString("%1 - %2").arg(m_title).arg(info));
    } else {
        m_titleLabel->setText(m_title);
    }
}

void 
        TitleFrame::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);	
}
