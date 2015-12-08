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

#include "filterActivationModel.h"
#include "can/can_frame.h"

FilterActivationModel::FilterActivationModel(CommunicationProxyModel* communicationModel, QObject *parent)
	: QAbstractTableModel(parent)
{
	m_comModel = communicationModel;				// communication model (to retrieve the folter list)
	id_model = IdentifiersModel::instance();	// Identifier model (to retrieve CAN ID related properties)
	settings = AppSettings::instance();
}

int 
FilterActivationModel::rowCount(const QModelIndex &index) const
{
	return m_comModel->getFilter().count();
}

int 
FilterActivationModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return 1;
}

QVariant 
FilterActivationModel::data(const QModelIndex &index, int role) const 
{

	int col = index.column();
	int row = index.row();

	identifierStruct m_id = m_comModel->getFilter().at(row);

	if (col == 0) {
		if (role == Qt::CheckStateRole)	
			if (m_comModel->getFilterExclusion().contains(m_id)) {
				return QVariant(Qt::Unchecked);
			} else {
				return QVariant(Qt::Checked);
			}
		if (role == Qt::DecorationRole) return id_model->getColorIcon(m_id.identifier, m_id.extended);  // getColorIcon is used instead of getIdColor because of QT bug 8357 http://bugreports.qt.nokia.com/browse/QTBUG-8357 
		if ((role == Qt::DisplayRole) || (role == Qt::ToolTipRole)) {
				QString infoLabel;
				if (!m_id.extended) {
					infoLabel = QString("(STD)");
				} else {
					infoLabel = QString("(EXT)");
				}
				switch(settings->displayIDFormat()) {
					case 0:
						return QString("%1 %2 %3").arg(infoLabel).arg(m_id.identifier).arg(id_model->getIdName(m_id.identifier, m_id.extended));
						break;
					case 1:
						return QString("%1 0x%2 %3").arg(infoLabel).arg(m_id.identifier, 8, 16, QLatin1Char('0')).arg(id_model->getIdName(m_id.identifier, m_id.extended));
						break;
					case 2:
						default:
						return QString("%1 0x%2 / %3 %4").arg(infoLabel).arg(m_id.identifier, 8, 16, QLatin1Char('0')).arg(m_id.identifier).arg(id_model->getIdName(m_id.identifier, m_id.extended));
				}
		}
		if (role == Qt::UserRole)			  return m_id.identifier;
		if (role == Qt::UserRole+1)			  return m_id.extended;
	}
	return QVariant();
}

QVariant 
FilterActivationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0) return QString(tr("Filter"));
    }

    return QVariant();
}


Qt::ItemFlags 
FilterActivationModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    // Identifier cannot be edited
    if (index.column() == 0)
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;


	
	return Qt::ItemIsEnabled;
}
