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

#ifndef COMMUNICATIONMODEL_H
#define COMMUNICATIONMODEL_H

#define DEBUG_COMMUNICATIONMODEL_ON 0 // 0 to ignore, 1 to display debug messages
#define DEBUG_COMMUNICATIONMODEL(x) { if (DEBUG_COMMUNICATIONMODEL_ON) qDebug(x); }

// QT includes
#include <QTableView>
#include <QTimer>
#include <QEventLoop>
#include <QMutex>

// non QT includes
#include "can/can_frame.h"
#include "identifiers/IdentifiersModel.h"
#include "appSettings.h"
#include "CommunicationData.h"
#include "IdentifiersStatsModel.h"
#include "pluginInterface.h"
//#include "traffic/TrafficModel.h"

class CommunicationModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static CommunicationModel* instance(QWidget* parent = 0);
    ~CommunicationModel();

    int rowCount( const QModelIndex &index) const;
    int columnCount( const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags( const QModelIndex& ) const ;

    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

    bool addFrame(const Can_Frame &frame);
    bool addInfoMessage(const QString &message);
    bool addErrorMessage(const QString &message);


    CommunicationData::CommunicationDataType type(int row);	// Returns the type of this CommunicationData
    Can_Frame frame(int row);																// Returns the Can_Frame from this CommunicationData
    Info_Message infoMessage(int row) const;								// Returns the message from this CommunicationData
    Error_Message errorMessage(int row) const;							// Returns the message from this CommunicationData

    void clear();

    //void sort (int column, Qt::SortOrder order = Qt::AscendingOrder );

    void clearHighlightList();
    void addHighlight(unsigned int identifier);
    bool highlight(unsigned int identifier);

    void setHighlightDataString(QString dataString);

signals:
    void frameNumberChanged(int number);
    void frameNumberPerSecondChanged(int number);
    void frameReceived(can_msg const msg);   // Used to inform plugin

protected:
    void timerEvent(QTimerEvent *event);

private:
    CommunicationModel(QObject *parent = 0);

    IdentifiersModel* id_model;
    IdentifiersStatsModel* id_stats_model;
    //TrafficModel* traffic_model;

    //QString formatFrame(Can_Frame frame);

    QVariant InfoMessageData(const QModelIndex &index, int role) const;
    QVariant ErrorMessageData(const QModelIndex &index, int role) const;
    QVariant CanFrameData(const QModelIndex &index, int role) const;

    QList<CommunicationData> m_communication_buffer; // Communication log buffer
    QList<CommunicationData> m_communication;        // Communication log
    
    //QList<unsigned int> m_traffic;		     // Traffic log
    unsigned int traffic_counter;
    int m_trafficTimerId;
    unsigned int m_bufferCopyTimerTime;
    int m_bufferCopyTimerId;
    QMap<unsigned int,int> m_highlighted_items;		// Items that have to be highlighted
    QString m_dataHighlightStr;						// Data that have to be highlighted
    AppSettings* settings;

    mutable QMutex mutex;
};

#endif
