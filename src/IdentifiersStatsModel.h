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

#ifndef IDENTIFIERSTATSMODEL_H
#define IDENTIFIERSTATSMODEL_H

#define DEBUG_IDENTIFIERSTATSMODEL_ON 0 // 0 to ignore, 1 to display debug messages
#define DEBUG_IDENTIFIERSTATSMODEL(x) { if (DEBUG_IDENTIFIERSTATSMODEL_ON) qDebug(x); }

// QT includes
#include <QTableView>
#include <QMutex>

// non QT includes

// non QT includes
#include "can/can_frame.h"
#include "appSettings.h"
#include "identifiers/IdentifiersModel.h"
#include "IdentifiersStatsModel.h"

struct identifierCommunicationStats
{
    identifierStruct id;			// identifier structure (id + standard vs extended)
    unsigned int counts;			// amount of received/send occurences for this identifier
    Can_Frame latestFrame;		// latest received/send frame for this identifier
};

class IdentifiersStatsModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static IdentifiersStatsModel* instance(QWidget* parent = 0);

    int rowCount( const QModelIndex &index) const;
    int columnCount( const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags( const QModelIndex& ) const ;

    //bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

    void sort (int column, Qt::SortOrder order = Qt::AscendingOrder );

    bool addFrame(const Can_Frame &frame);
    bool findDLC(const int identifier, const bool extended, int &dlc);

    void clear();

    QString getIdentifierStatistics(unsigned int identifier, bool extended);

private:
    IdentifiersStatsModel(QObject *parent = 0);

    IdentifiersModel* id_model;		
    QList<identifierCommunicationStats> m_identifiersStats;

    AppSettings* settings;

    mutable QMutex mutex;
};

#endif
