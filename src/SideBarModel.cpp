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
#include "SideBarItem.h"
#include "SideBarModel.h"
#include "WidgetBase.h"
#include "WidgetDrivers.h"
#include "WidgetIdentifiers.h"
#include "WidgetCommunication.h"
#include "WidgetFilteredLog.h"
#include "WidgetMonitor.h"
#include "WidgetIdentifiersStats.h"
#include "WidgetPlugin.h"
//#include "WidgetBusStatistics.h"

using namespace SideBar;

SideBarModel::SideBarModel(QStackedWidget* stackedWidget, QObject *parent)
    : QAbstractItemModel(parent)
{
    this->stackedWidget = stackedWidget;

    m_filteredListsCount = 0;
    m_pluginsCount = 0;

    rootItem = new SideBarItem();
    // Append all fixed list of children to the rootItem
    rootItem->insertChild(0, Spacer1);
    WidgetDrivers *widgetDrivers = new WidgetDrivers(stackedWidget);
    rootItem->insertChild(1, device, widgetDrivers);
    rootItem->insertChild(2, identifiers, new WidgetIdentifiers(stackedWidget));
    rootItem->insertChild(3, Spacer2);
    rootItem->insertChild(4, comLog, new WidgetCommunication(stackedWidget));
    rootItem->insertChild(5, occurences, new WidgetIdentifiersStats(stackedWidget));
    rootItem->insertChild(6, Spacer3);
    rootItem->insertChild(7, end);

    widgetDrivers->setTitleInfo(canFactory()->currentDriver());
    connect(canFactory(), SIGNAL(driverHasChanged(QString)),widgetDrivers, SLOT(setTitleInfo(QString)));

    com_model = CommunicationModel::instance();

    // So that signal/slot works on can_msg
    qRegisterMetaType<can_msg>("can_msg");

}

SideBarModel::~SideBarModel()
{
    delete rootItem;
}


QVariant SideBarModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    SideBarItem *item = getItem(index);
    QString itemName = item->name();
    QString itemToolTip = item->toolTip();
    int itemType = item->type();

    QString tmp;
    unsigned int tmpBaudrate;

    // Display role - returns a text
    if (role == Qt::DisplayRole) {
        switch (itemType) {
        case device:
            //tmpBaudrate = Can_DriverFactory::instance()->actualDriver()->baudrate();
            tmp = Can_DriverFactory::instance()->actualDriver()->fullName(); // instead of name
            //if (tmpBaudrate) tmp = tmp + QString(" (%1kbps)").arg(tmpBaudrate);
            return tmp;
        case identifiers:	return tr("Identifiers");
        case comLog:		return tr("Communication log");
        case occurences:	return tr("Identifiers occurences");
        case traffic:		return tr("Traffic monitor");
        case dispersion:	return tr("Traffic dispersion");
        case filteredList:  return itemName;
        case plugin:		return itemName;
        }
    }

    if (role == Qt::DecorationRole) {
        switch (itemType) {
        case filteredList:  return QIcon(":/filter.png");
        }
    }

    if (role == Qt::EditRole) {
        switch (itemType) {
        case filteredList:  return itemName;
        }
    }

    if (role == Qt::ToolTipRole) {
        return itemToolTip;
    }

#ifdef Q_WS_MAC
    if (role == Qt::FontRole)
    {
        QFont font;
        font.setPixelSize( 11 );
        return font;
    }
#endif

    return QVariant();

}

Qt::ItemFlags SideBarModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    SideBarItem *item = getItem(index);
    QString itemName = item->name();
    int itemType = item->type();

    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    switch (itemType) {
    case device:
    case identifiers:
    case comLog:
    case occurences:
    case plugin:
    case traffic:
    case dispersion:
        flags |= Qt::ItemIsSelectable;
        break;
    case filteredList:
        flags |= Qt::ItemIsSelectable | Qt::ItemIsEditable;
    default: break;
    }
    return flags;
}

SideBarItem *SideBarModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        SideBarItem *item = static_cast<SideBarItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem;
}

QModelIndex SideBarModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    SideBarItem *parentItem = getItem(parent);

    SideBarItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

bool SideBarModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    //SideBarItem *parentItem = getItem(parent);

    beginInsertRows(parent, position, position + rows - 1);
    endInsertRows();

    return true;
}

QModelIndex SideBarModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    SideBarItem *childItem = getItem(index);
    SideBarItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool SideBarModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    SideBarItem *parentItem = getItem(parent);
    bool success = true;

    beginRemoveRows(parent, position, position + rows - 1);
    success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

int SideBarModel::rowCount(const QModelIndex &parent) const
{
    SideBarItem *parentItem = getItem(parent);

    return parentItem->childCount();
}

bool SideBarModel::setData(const QModelIndex &index, const QVariant &value,
                           int role)
{
    if (role != Qt::EditRole)
        return false;

    SideBarItem *item = getItem(index);
    bool result = item->setName(value.toString());
    qDebug() << QString("Set item name to %1").arg(value.toString());
    emit contentChanged(); // So that the save button gets active in the main window
    if (result)
        emit dataChanged(index, index);

    return result;
}

bool SideBarModel::setHeaderData(int section, Qt::Orientation orientation,
                                 const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    bool result = rootItem->setName(value.toString());

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}

int 
SideBarModel::filteredListsCount() {
    return m_filteredListsCount;
}

int 
SideBarModel::pluginsCount() {
    return m_pluginsCount;
}


unsigned int 
SideBarModel::addFilteredList(const QString &name) {
    // Find the STATISTICS Item -  This is the first item after the last Filtered list
    qDebug() << QString("There is a total of %1 childs").arg(rootItem->childCount());
    QModelIndex tmp_index;
    QString tmp_name;
    for (int row=0; row<rootItem->childCount(); row++) {
        tmp_index = index(row,0);
        SideBarItem *item = getItem(tmp_index);

        if (item->type() == Spacer3) {

            //qDebug() << QString("Found plugins at place %1").arg(row);
            SideBarItem *newItem = rootItem->insertChild(row, filteredList, new WidgetFilteredLog(stackedWidget));

            if (name.isEmpty())
                tmp_name = QString(tr("Filtered communication log"));
            else
                tmp_name = name;
            newItem->setName(tmp_name);
            insertRows(row, 1);
            m_filteredListsCount++;
            return row;
        }
    }
}

unsigned int 
SideBarModel::addFilteredList(const QString &name, QList<identifierStruct> filter) {
    // Find the STATISTICS Item -  This is the first item after the last Filtered list
    qDebug() << QString("There is a total of %1 childs").arg(rootItem->childCount());
    QModelIndex tmp_index;
    QString tmp_name;
    for (int row=0; row<rootItem->childCount(); row++) {
        tmp_index = index(row,0);
        SideBarItem *item = getItem(tmp_index);

        if (item->type() == Spacer3) {;

            //qDebug() << QString("Found plugins at place %1").arg(row);
            WidgetFilteredLog *m_ui = new WidgetFilteredLog(stackedWidget);
            SideBarItem *newItem = rootItem->insertChild(row, filteredList, m_ui);

            if (name.isEmpty())
                tmp_name = QString(tr("Filtered Log"));
            else
                tmp_name = name;
            newItem->setName(tmp_name);
            m_ui->setFilter(filter);
            insertRows(row, 1);
            m_filteredListsCount++;
            return row;
        }
    }
}

void 
SideBarModel::removeFilteredLists()
{
    QModelIndex tmp_index;
    for (int row=0; row<rootItem->childCount(); row++) {
        tmp_index = index(row,0);
        SideBarItem *item = getItem(tmp_index);
        if (item->type() == filteredList) {
            WidgetFilteredLog* w_filtered = (WidgetFilteredLog*)item->ui();
            w_filtered->removeWidget();
            delete w_filtered;
            removeFilteredList(tmp_index);
            m_filteredListsCount--;
            //order may have change - so begin from row 0 again
            row = 0;
        }
    }
}

void
SideBarModel::removeFilteredList(const QModelIndex &index) {
    removeRows(index.row(),1);
    emit contentChanged();
}

Can_DriverFactory * 
SideBarModel::canFactory() {
    return Can_DriverFactory::instance();
} 

void  
SideBarModel::writeXMLStream( QXmlStreamWriter* stream )
{
    QModelIndex tmp_index;
    for (int row=0; row<rootItem->childCount(); row++) {
        tmp_index = index(row,0);
        SideBarItem *item = getItem(tmp_index);
        if (item->type() == filteredList) {
            qDebug() << QString("Found Filtered List at place %1").arg(row);
            stream->writeEmptyElement( "filteredList" );
            stream->writeAttribute("name", item->name() );
            WidgetFilteredLog* w_filtered = (WidgetFilteredLog*)item->ui();
            QList<identifierStruct> filterList = w_filtered->getFilter();
            QListIterator<identifierStruct> i(filterList);
            QStringList filter;
            identifierStruct id;
            while (i.hasNext()) {
                id = i.next();
                filter << QString("%1,%2").arg(id.identifier).arg(id.extended);
            }
            stream->writeAttribute("filter", filter.join(",") );
        }
    }
}

void 
SideBarModel::readXMLStream(QXmlStreamReader* stream)
{
    QString tmp_name;
    QString tmp_filter;
    QList<identifierStruct> filter;
    foreach( QXmlStreamAttribute attribute, stream->attributes() )
    {
        if ( attribute.name() == "name" ) tmp_name = attribute.value().toString();
        if ( attribute.name() == "filter" ) tmp_filter = attribute.value().toString();
    }

    QStringList tmp_filterList = tmp_filter.split(",");
    unsigned int tmp_size = tmp_filterList.size(); 
    if ((tmp_size % 2) == 0) {
        identifierStruct tmp_identifier;
        for (int i = 0; i < tmp_size; i=i+2) {
            tmp_identifier.identifier = QVariant(tmp_filterList.at(i)).toUInt();
            tmp_identifier.extended = QVariant(tmp_filterList.at(i+1)).toBool();
            // Do not append to the filter if it is an empty id (i.e. 0)
            if (tmp_identifier.identifier) {
                filter.append(tmp_identifier);
            }
        }
    }
    addFilteredList(tmp_name, filter);
}

void 
SideBarModel::addPlugin(PluginInterface* itf) {
    // Find the STATISTICS Item -  This is the first item after the last Filtered list
    qDebug() << QString("There is a total of %1 childs").arg(rootItem->childCount());
    QModelIndex tmp_index;
    QString tmp_name;
    QString name;
    for (int row=0; row<rootItem->childCount(); row++) {
        tmp_index = index(row,0);
        SideBarItem *item = getItem(tmp_index);

        if (item->type() == end) {

            //itf->setAppInterface(this);
            WidgetPlugin *newPlugin = new WidgetPlugin(stackedWidget);
            //newPlugin->setAccessibleName("test");
            name = itf->name();
            newPlugin->setTitle(name);
            QWidget *tmp = itf->ui(newPlugin);
            newPlugin->setBody(tmp); // Pass the instance of WidgetPlugin as the parent of this widget
            //			connect(itf, SIGNAL(sendOnCan(can_msg)), this, SLOT(sendOnCanFromPlugin(can_msg)));
            connect(com_model, SIGNAL(frameReceived(can_msg)), itf, SLOT(receivedFromCan(can_msg)));
            connect(this, SIGNAL(informPluginAboutCommunicationStatus(bool)), itf, SLOT(setCommunicating(bool)));
            SideBarItem *newItem = rootItem->insertChild(row, plugin, newPlugin);

            if (name.isEmpty())
                tmp_name = QString(tr("Plugin"));
            else
                tmp_name = name;
            newItem->setName(itf->name());
            newItem->setToolTip(itf->description());
            insertRows(row, 1);
            m_pluginsCount++;
            return;
        }
    }
}

void 
SideBarModel::sendOnCanFromPlugin(can_msg msg) {
    canFactory()->send(
                Can_Frame(
                    1, //canDirection::send,
                    QDateTime::currentDateTime(),
                    msg.extended,
                    msg.rtr,
                    msg.id,
                    msg.dlc,
                    msg.data
                    )
                );
}
