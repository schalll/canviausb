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
#include <QToolBar>
#include <QHBoxLayout>
#include <QStyleOption>
#include <QPainter>

// non QT includes
#include "ToolFrame.h"
#include "utilities/searchlineedit.h"


ToolFrame::ToolFrame( QWidget* parent) :
        QFrame( parent )
{
	//setMinimumHeight(40);
	
	m_toolbar = new QToolBar(this);
	
	m_search = new SearchLineEdit(this);
	m_search->setStatusTip(tr("Search within CAN data"));
	m_search->setMinimumWidth(150);
	m_search->setMaximumWidth(150);
	m_search->setStyleSheet("background-color: white;");
	m_search->setVisible(false);	
	
	connect(m_search, SIGNAL(textChanged(const QString&)),this, SIGNAL(searchStringChanged(const QString &)));
	connect(m_search, SIGNAL(searchByFiltering()),this, SIGNAL(searchByFiltering()));
	connect(m_search, SIGNAL(searchByHighlighting()),this, SIGNAL(searchByHighlighting()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(m_toolbar);
    layout->addStretch();
	layout->addWidget(m_search);
	setLayout(layout);
}

ToolFrame::~ToolFrame()
{

}

void 
ToolFrame::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);	
}

void 
ToolFrame::makeSearchBarVisible(bool visible) {
	m_search->setVisible(visible);
}


void 
ToolFrame::emptySearchBar() {
	m_search->clearFocus();
	m_search->clear();
	
}

void 
ToolFrame::addAction(QAction *act) {
	m_toolbar->addAction(act);
}

void 
ToolFrame::addSeparator() {
	m_toolbar->addSeparator();
}
