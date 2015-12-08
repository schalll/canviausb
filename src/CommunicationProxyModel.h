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

#ifndef COMMUNICATIONPROXYMODEL_H
#define COMMUNICATIONPROXYMODEL_H

// QT includes
#include <QSortFilterProxyModel>

// Non QT includes
#include "identifiers\IdentifiersModel.h"

class CommunicationProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    CommunicationProxyModel(QObject *parent = 0, bool useFilter = false);
    QList<identifierStruct> getFilter();
    QList<identifierStruct> getFilterExclusion();

	void setFilter(QList<identifierStruct> filter);
	void setFilterString(const QString &filterStr);
	void toggleExclusion(identifierStruct id);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
	bool m_useFitler;
    QList<identifierStruct> m_filter;				// Filter
	QList<identifierStruct> m_filter_exclusion;		// Filter exclusion list (item in this list are part of the filter, but are not displayed int he view)
	QString m_filterStr;							// Search string
};

#endif
