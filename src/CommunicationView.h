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

#ifndef COMMUNICATIONVIEW_H
#define COMMUNICATIONVIEW_H

// QT includes
#include <QTableView>

// Non QT includes
#include "CommunicationModel.h"
#include "CommunicationDelegate.h"
#include "appSettings.h"
#include "PrintableTableView.h"
#include "CommunicationProxyModel.h"
#include "identifiers\IdentifiersModel.h"


class CommunicationView : public PrintableTableView
{
    Q_OBJECT
public:
    CommunicationView( QWidget* parent = 0, bool useFilter = false);
    ~CommunicationView();

    void print();
	QString htmlTable();
	QString htmlFilter();

    QList<identifierStruct> getFilter();
    void setFilter(QList<identifierStruct> filter);

    void applyPreferences();

public slots:
	void setSearchString(const QString &searchStr);
	void setSearchByFiltering();
	void setSearchByHighlighting();
	void redraw();

private slots:
    void highlightLogEntry();
    void deleteHighlightLogEntry();
    void deleteLogEntry();
	void createFilteredListFromSelection();
	void computeStatistics(QItemSelection selected, QItemSelection deselected);

protected:
	bool printOnlySelection;
	QModelIndexList selectionList;

    CommunicationModel* m_model;

    void createActions();
    void createContextMenu();

	void rowsInserted(const QModelIndex &parent, int start, int end);

private:
    CommunicationDelegate* m_delegate;
	CommunicationProxyModel *proxyModel;
	QItemSelectionModel *m_selection;

	bool m_useFilter;

    QAction *highlightLogEntryAct;
    QAction *deleteHighlightLogEntryAct;
    QAction *deleteLogEntryAct;
	QAction *createFilteredListFromSelectionAct;

    AppSettings* settings;
	IdentifiersModel* id_model;

    int m_timerId;

	// Search related definitions
	QString m_searchString;
	enum search_methods
	{
		search_by_highlighting,
		search_by_filtering
	};
	search_methods m_search_method;
};

#endif
