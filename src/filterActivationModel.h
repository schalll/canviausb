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

#ifndef FILTERACTIVATIONMODEL_H
#define FILTERACTIVATIONMODEL_H

#include "CommunicationProxyModel.h"
#include "identifiers\IdentifiersModel.h"
#include "appSettings.h"

class FilterActivationModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	FilterActivationModel(CommunicationProxyModel* communicationModel, QObject *parent = 0);

	int rowCount(const QModelIndex &index) const;
	int columnCount(const QModelIndex &index) const;

	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	
	Qt::ItemFlags flags(const QModelIndex &index) const;

private:	
	CommunicationProxyModel* m_comModel;
	IdentifiersModel* id_model;
    AppSettings* settings;
};

#endif
