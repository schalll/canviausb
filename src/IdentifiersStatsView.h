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

#ifndef IDENTIFIERSSTATSVIEW_H
#define IDENTIFIERSSTATSVIEW_H

// QT includes
#include <QTableView>
//#include <QItemEditorFactory>

// non QT includes
#include "IdentifiersStatsModel.h"
#include "PrintableTableView.h"
#include "appSettings.h"

class IdentifiersStatsView : public PrintableTableView
{
    Q_OBJECT

public:
    IdentifiersStatsView(QWidget* parent = 0 );
    ~IdentifiersStatsView();
    void applyPreferences();

public slots:
    void print();
	void createFilteredListFromSelection();

private:
    void createActions();
    void createContextMenu();

	QAction *createFilteredListFromSelectionAct;

    IdentifiersStatsModel* m_model;

	AppSettings* settings;

};
#endif
