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

#ifndef SIDEBARMODEL_H
#define SIDEBARMODEL_H

// #define SHOW_ALL_WINDOWS // Uncomment to display the statistics part 

// QT includes 
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QStackedWidget>

// non QT includes 
#include "can/can_driverfactory.h" 
#include "CommunicationModel.h"

namespace SideBar
{
enum Type
{
    // Parent level
    Spacer1=0,			// The first spacer named DEVICE
    device,			// The currently used CAN device
    identifiers,		// The list of properties from each CAN identifier (Name, Color,..)
    Spacer2,			// The second spacer named CAN TRAFFIC
    comLog,			// The entire capture list
    occurences,			// The identifiers occurences list
    filteredList,		// Filtered list (0..n)
    Spacer3,			// The third spacer named PLUGINS
    plugin,			// Plugins
    traffic,			// The trafic monitor
    dispersion,			// The dispersion (chart) view
    end,			// LatestItem
    TypeUnknown     // Don't touch me - This will be used as a default type
};
}

class SideBarItem;

class SideBarModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    //static SideBarModel* instance(QWidget* parent = 0 );
    SideBarModel(QStackedWidget* stackedWidget, QObject *parent = 0);
    ~SideBarModel();

    QVariant data(const QModelIndex &index, int role) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /** no header, one column always */
    QVariant headerData( int, Qt::Orientation, int ) const { return QVariant(); }
    int columnCount( const QModelIndex& = QModelIndex()) const { return 1; }

    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);

    bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex());

    int filteredListsCount();
    int pluginsCount();

    SideBarItem *getItem(const QModelIndex &index) const;
    void removeFilteredLists();

    // TO Save/Load from project file
    void writeXMLStream(QXmlStreamWriter* stream);
    void readXMLStream(QXmlStreamReader* stream);

protected:
    bool loadPlugin();

signals:
    void contentChanged();
    void informPluginAboutCommunicationStatus(bool on);

public slots:
    unsigned int addFilteredList(const QString &name = "");
    unsigned int addFilteredList(const QString &name, QList<identifierStruct> filter);
    void removeFilteredList(const QModelIndex &index);

    void addPlugin(PluginInterface *itf);
    void sendOnCanFromPlugin(can_msg msg);

private:
    // Pointer to the QStackedWidget where all views exists
    QStackedWidget* stackedWidget;

    int m_filteredListsCount;
    int m_pluginsCount;

    SideBarItem *rootItem;
    Can_DriverFactory *canFactory();
    CommunicationModel* com_model;
};

#endif
