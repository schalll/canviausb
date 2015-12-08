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
#include "WidgetPlugin.h"

WidgetPlugin::WidgetPlugin(QStackedWidget* stackedWidget, QWidget *parent) :
        WidgetBase(stackedWidget,parent)
{

	title = NULL;
	body = NULL;
	mainLayout = NULL;
}

WidgetPlugin::~WidgetPlugin()
{

}

void 
WidgetPlugin::setTitle(QString _title) {
	if (title) {
		delete title;
		title = NULL;
	}
	title = new TitleFrame(this,_title);
	createLayout();
}

void 
WidgetPlugin::setBody(QWidget *_body) {
	if (body) {
		delete body;
		body = NULL;
	}
	body = _body;
	//body->setParent(this);
	createLayout();
}

void 
WidgetPlugin::createLayout() {
	if ((title != NULL) && (body != NULL)) {
		if (mainLayout) {
			delete mainLayout;
			mainLayout = NULL;
		}
		// Layout
		mainLayout = new QVBoxLayout(this);
		mainLayout->addWidget(title);
		mainLayout->addWidget(body);

		mainLayout->setContentsMargins (0, 0, 0, 0);
		setLayout(mainLayout);
	}
}
