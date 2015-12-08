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
#include <QDebug>

// non QT includes
#include "CommunicationProxyModel.h"

CommunicationProxyModel::CommunicationProxyModel(QObject *parent, bool useFilter)
        : QSortFilterProxyModel(parent)
{
	m_useFitler = useFilter;
}

bool CommunicationProxyModel::filterAcceptsRow(int sourceRow,
                                                     const QModelIndex &sourceParent) const
{
    QModelIndex index2 = sourceModel()->index(sourceRow, 1, sourceParent);
    QModelIndex index_data = sourceModel()->index(sourceRow, 3, sourceParent);

    identifierStruct id;
	id.identifier = sourceModel()->data(index2,Qt::EditRole).toUInt();
    id.extended = sourceModel()->data(index2,Qt::UserRole+1).toBool();

	if (m_useFitler) {
		if (m_filterStr.isEmpty()) {
			return (m_filter.contains(id) && !m_filter_exclusion.contains(id));
		} else {
			return ((m_filter.contains(id) && !m_filter_exclusion.contains(id)) && sourceModel()->data(index_data,Qt::DisplayRole).toString().contains(m_filterStr));
		}
	} else {
		if (m_filterStr.isEmpty()) {
			return true;
		} else {
			return (sourceModel()->data(index_data,Qt::DisplayRole).toString().contains(m_filterStr));
		}
	}
}


QList<identifierStruct> 
CommunicationProxyModel::getFilter() {
    return m_filter;
}

QList<identifierStruct> 
CommunicationProxyModel::getFilterExclusion() {
	return m_filter_exclusion;
}

// This function is called when a new filter has been set
void 
CommunicationProxyModel::setFilter(QList<identifierStruct> filter) {
    m_filter = filter;
    //invalidateFilter();
    invalidate();
}

// This function is called when a new search string has been set
void
CommunicationProxyModel::setFilterString(const QString &filterStr) {
	m_filterStr = filterStr;
	invalidate();
}

void 
CommunicationProxyModel::toggleExclusion(identifierStruct id) {
	if (m_filter_exclusion.contains(id)) {
		m_filter_exclusion.removeAll(id);
		qDebug() << QString("Remove %1 %2 from exclusion list").arg(id.identifier).arg(id.extended);
	} else {
		m_filter_exclusion.append(id);
		qDebug() << QString("Add %1 %2 to exclusion list").arg(id.identifier).arg(id.extended);
	}
    invalidate();
}
