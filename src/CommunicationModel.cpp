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
#include <QtDebug>
#include <QTimerEvent>

// non QT includes
#include "CommunicationModel.h"
#include "can/can_driverfactory.h"

CommunicationModel::CommunicationModel(QObject *parent): QAbstractTableModel(parent) 
{	
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::CommunicationModel");

    id_model = IdentifiersModel::instance();
    id_stats_model = IdentifiersStatsModel::instance();
    //traffic_model = TrafficModel::instance();
    settings = AppSettings::instance();
    traffic_counter = 0;
    m_trafficTimerId = startTimer(1000);		// this is the timer to count the frame per seconds - leave it unchanged for now
    m_bufferCopyTimerTime = settings->displayRefreshRate();
    m_bufferCopyTimerId = startTimer(m_bufferCopyTimerTime); // this is the timer to copy the buffer
}

CommunicationModel*
CommunicationModel::instance(QWidget* parent) {

    DEBUG_COMMUNICATIONMODEL("CommunicationModel::instance");

    static CommunicationModel* singleton = 0;
    if (singleton == 0) {
        singleton = new CommunicationModel(parent);
    }
    return singleton;
}
CommunicationModel::~CommunicationModel() {
    clear();
}

int 
CommunicationModel::rowCount(const QModelIndex &index) const
{
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::rowCount");
    Q_UNUSED(index);
    return m_communication.count();
}

int 
CommunicationModel::columnCount(const QModelIndex &index) const
{
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::columnCount");
    Q_UNUSED(index);
    return 6;
}

QString formatFrame(Can_Frame frame, unsigned int message_format) {
    QString format;
    if (!frame.rtr) {
        format = QString("%1 %2 %3 %4 %5 %6 %7 %8");
        bool MessageHex;
        if (message_format == 1)
            MessageHex = true;
        else
            MessageHex = false;
        for (unsigned int i = 0; i < 8; ++i) {
            if (i < frame.dlc) {
                // Message in HExadecimal
                if (MessageHex)
                    format = format.arg(frame.data[i], 2, 16, QLatin1Char('0'));
                else
                    format = format.arg(frame.data[i], 3, 10, QLatin1Char('0'));
            }
            else {
                if (MessageHex)
                    format = format.arg("  ");
                else
                    format = format.arg("   ");
            }
        }
    } else {
        format = QString("(length = %1)").arg(frame.dlc);
    }
    return format;
}

QString formatAsciiFrame(Can_Frame frame) {
    QString ascii = QString("%1%2%3%4%5%6%7%8");
    for (unsigned int i=0; i<8; ++i) {
        if (i < frame.dlc) {
            if ((frame.data[i]>31) && (frame.data[i]<127))
                ascii = ascii.arg(QLatin1Char(frame.data[i]));
            else
                ascii = ascii.arg(".");
        }
        else {
            ascii = ascii.arg(" ");
        }
    }
    return ascii;
}

QVariant 
CommunicationModel::CanFrameData(const QModelIndex &index, int role) const {

    DEBUG_COMMUNICATIONMODEL("CommunicationModel::CanFrameData");

    QString tmp;

    int row = index.row();
    int col = index.column();
    unsigned long previous_row;

    Can_Frame m_frame = m_communication.at(row).frame();

    if (role == Qt::ForegroundRole ) {
        if (m_frame.direction == 0) //canDirection::receive)
            return QColor(Qt::black);
        else
            return QColor(Qt::blue);
    }

    if (col == 0) {
        if (role == Qt::DisplayRole)		return QString("  %1 ").arg(m_frame.time_pc.toString("hh:mm:ss.zzz"));
    }
    if (col == 1) {
        //if (role == Qt::ToolTipRole) return id_stats_model->getIdentifierStatistics(m_frame.id, m_frame.ext);
        if (role == Qt::DecorationRole) return id_model->getColorIcon(m_frame.id, m_frame.ext);  // getColorIcon is used instead of getIdColor because of QT bug 8357 http://bugreports.qt.nokia.com/browse/QTBUG-8357
        if (role == Qt::DisplayRole) {
            QString infoLabel;
            if (!m_frame.ext) {
                if (!m_frame.rtr) infoLabel = QString("(STD)");
                else infoLabel = QString("(Remote STD)");
            } else {
                if (!m_frame.rtr) infoLabel = QString("(EXT)");
                else infoLabel = QString("(Remote EXT)");
            }
            switch(settings->displayIDFormat()) {
            case 0:
                return QString("%1 %2 ").arg(infoLabel).arg(m_frame.id);
                break;
            case 1:
                return QString("%1 0x%2 ").arg(infoLabel).arg(m_frame.id, 8, 16, QLatin1Char('0'));
                break;
            case 2:
            default:
                return QString("%1 0x%2 / %3 ").arg(infoLabel).arg(m_frame.id, 8, 16, QLatin1Char('0')).arg(m_frame.id);
            }
        }
        if (role == Qt::EditRole)			  return m_frame.id;
        if (role == Qt::UserRole+1)			  return m_frame.ext;
        if (role == Qt::UserRole+2)			  return id_model->getIdColor(m_frame.id, m_frame.ext);
        if (role == Qt::BackgroundColorRole) {
            if (m_highlighted_items.contains(m_frame.id))
                return QColor("#FFF79F");
        }
    }
    if (col == 2) {
        if (role == Qt::DisplayRole) {
            if (!m_frame.rtr)
                return QString("(%1)").arg(m_frame.dlc);
            else
                return QString("");
        }
        if (role == Qt::TextAlignmentRole)
            return Qt::AlignHCenter;
    }
    if (col == 3) {
        tmp = formatFrame(m_frame, settings->displayMessageFormat());

        if (role == Qt::BackgroundColorRole) {
            if (!m_dataHighlightStr.isEmpty())
                if (tmp.contains(m_dataHighlightStr)) return QColor("#FFF79F");
        }
        if (role == Qt::DisplayRole) {
            return tmp;
        }
        if (role == Qt::UserRole+1) {

            // Find previous frame with same id and extended field
            bool m_found_previous_frame = false;
            Can_Frame m_previous_frame;

            for (previous_row = row; previous_row >0 ; previous_row--) {
                m_previous_frame = m_communication.at(previous_row).frame();
                if ((previous_row != row) && (m_previous_frame.id == m_frame.id) && (m_previous_frame.ext == m_frame.ext)) {
                    // found one
                    m_found_previous_frame = true;
                    // So go out from the for loop
                    break;
                }
            }
            tmp = QString("");
            if (m_found_previous_frame) {
                if (!m_previous_frame.rtr) {
                    tmp = formatFrame(m_previous_frame, settings->displayMessageFormat());
                }
            }
            return tmp;
        }
    }

//    if (col == 4) {
//        if (role == Qt::DisplayRole) {
//            tmp = QString("");
//            if (!m_frame.rtr) {
//                tmp = formatAsciiFrame(m_frame);
//            }
//            return tmp;
//        }

//        if (role == Qt::UserRole+1) {

//            // Find previous frame with same id and extended field
//            bool m_found_previous_frame = false;
//            qDebug() << "Look for a previous frame";
//            Can_Frame m_previous_frame;
//            for (previous_row = row; previous_row >=0 ; previous_row--) {

//                qDebug() << QString("Previous row type is %1").arg(m_communication.at(previous_row).type());
//                m_previous_frame = m_communication.at(previous_row).frame();
//                if ((m_previous_frame.id == m_frame.id) && (m_previous_frame.ext == m_frame.ext)) {
//                    // found one
//                    m_found_previous_frame = true;
//                    // So go out from the for loop
//                    break;
//                }
//            }
//            tmp = QString("");
//            if (m_found_previous_frame) {
//                if (!m_frame.rtr) {
//                    tmp = formatAsciiFrame(m_previous_frame);
//                }
//            }
//            return tmp;
//        }

//    }

    if (col == 5) {
        if (role == Qt::DisplayRole)		return id_model->getIdName(m_frame.id, m_frame.ext);
    }
    return QVariant();
}

QVariant 
CommunicationModel::InfoMessageData(const QModelIndex &index, int role) const {
    //return QVariant(); //Just for now
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::InfoMessageData");

    int row = index.row();
    int col = index.column();

    //qDebug() << QString("role = ") << role << endl;

    if (role == Qt::ForegroundRole ) {
        return QColor(Qt::darkGreen);
    }
    if (col == 0) {
        if (role == Qt::DisplayRole)		return QString("  %1 ").arg(m_communication.at(row).infoMessage().timestamp.toString("hh:mm:ss.zzz"));
    }
    if (col == 1) {
        if (role == Qt::DisplayRole)		return m_communication.at(row).infoMessage().text;
        if (role == Qt::FontRole)    {
            QFont font;
            font.setBold(true);
            return font;
        }

    }
    return QVariant();
}

QVariant 
CommunicationModel::ErrorMessageData(const QModelIndex &index, int role) const {
    //return QVariant(); //Just for now
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::ErrorMessageData");

    int row = index.row();
    int col = index.column();

    if (role == Qt::ForegroundRole ) {
        return QColor(Qt::red);
    }
    if (col == 0) {
        if (role == Qt::DisplayRole)		return QString("  %1 ").arg(m_communication.at(row).errorMessage().timestamp.toString("hh:mm:ss.zzz"));
    }
    if (col == 1) {
        if (role == Qt::DisplayRole)		return m_communication.at(row).errorMessage().text;
        if (role == Qt::FontRole)    {
            QFont font;
            font.setBold(true);
            return font;
        }
    }
    return QVariant();
}

QVariant 
CommunicationModel::data(const QModelIndex &index, int role) const
{
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::data");
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    //int col = index.column();

    //if (role == Qt::FontRole)
    //	return QFont("Arial", 10, QFont::Normal);

    if (role == Qt::UserRole)
        return m_communication.at(row).type();

    // Data for a Can_Frame
    switch (m_communication.at(row).type()) {
    case CommunicationData::VAR_CAN_FRAME:
        return CanFrameData(index,role);
        break;
    case CommunicationData::VAR_INFO_MESSAGE:
        return InfoMessageData(index,role);
        break;
    case CommunicationData::VAR_ERROR_MESSAGE:
        return ErrorMessageData(index,role);
        break;

    default:
        return QVariant();
        break;
    }
}

QVariant 
CommunicationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::headerData");
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        //if (section == 0) return QString(tr(""));
        if (section == 0) return QString(tr("Time"));
        if (section == 1) return QString(tr("Identifier"));
        if (section == 2) return QString(tr("DLC"));
        if (section == 3) return QString(tr("Data"));
        if (section == 4) return QString(tr("Ascii"));
        if (section == 5) return QString(tr("Name"));
    }

    return QVariant();
}

Qt::ItemFlags 
CommunicationModel::flags(const QModelIndex &index) const
{
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::flags");
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    // Everything cannot be edited
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool 
CommunicationModel::insertRows(int position, int rows, const QModelIndex &index) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::insertRows");
    Q_UNUSED(index);
    QMutexLocker locker(&mutex);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        Can_Frame frame;
        m_communication.insert(position, frame);
    }
    emit frameNumberChanged(m_communication.count());
    endInsertRows();
    return true;
}



bool 
CommunicationModel::addFrame(const Can_Frame &frame) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::addFrame");
    QMutexLocker locker(&mutex);
    //beginInsertRows(QModelIndex(),  m_communication.count(), m_communication.count());
    m_communication_buffer.append(CommunicationData(frame));
    // also make sure to add the identifier in the id list
    id_model->addId(frame.id, frame.ext);
    // also make sure to update the identifiers stats
    id_stats_model->addFrame(frame);
    //emit frameNumberChanged(m_communication.count());
    traffic_counter++;
    // Let's emit a signal that can be used ot inform possible plugins.
    can_msg tmp_msg;
    tmp_msg.id = frame.id;
    tmp_msg.extended = frame.ext;
    tmp_msg.rtr = frame.rtr;
    tmp_msg.dlc = frame.dlc;
    for (unsigned int i=0; i<8; i++) {
        tmp_msg.data[i] = frame.data[i];
    }
    tmp_msg.timestamp = frame.time_pc;
    emit frameReceived(tmp_msg);   // Used to inform plugin
    //endInsertRows();
    return true;
}

bool 
CommunicationModel::addInfoMessage(const QString &message) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::addInfoMessage");
    QMutexLocker locker(&mutex);
    m_communication_buffer.append(CommunicationData(Info_Message(message,QDateTime::currentDateTime())));
    return true;
}

bool 
CommunicationModel::addErrorMessage(const QString &message) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::addErrorMessage");
    QMutexLocker locker(&mutex);
    m_communication_buffer.append(CommunicationData(Error_Message(message,QDateTime::currentDateTime())));
    return true;
}

CommunicationData::CommunicationDataType 
CommunicationModel::type(int row) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::CommunicationModel");
    //  QMutexLocker locker(&mutex);
    //  qDebug() << QString("<<Enter");
    CommunicationData::CommunicationDataType tmp;
    //	if (!m_communication.isEmpty())
    //		tmp = m_communication.at(row).type();
    //	else
    tmp = CommunicationData::VAR_UNDEFINED;
    //  qDebug() << QString(">>Exit");
    return tmp;
}

Can_Frame 
CommunicationModel::frame(int row) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::frame");
    QMutexLocker locker(&mutex);
    Can_Frame tmp;
    tmp = m_communication.at(row).frame();
    return tmp;
}

Info_Message 
CommunicationModel::infoMessage(int row) const {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::infoMessage");
    return m_communication.at(row).infoMessage();
}

Error_Message 
CommunicationModel::errorMessage(int row) const {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::errorMessage");
    return m_communication.at(row).errorMessage();
}

bool 
CommunicationModel::removeRows(int position, int rows, const QModelIndex &index) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::removeRows");
    Q_UNUSED(index);
    QMutexLocker locker(&mutex);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        m_communication.removeAt(position);
    }
    emit frameNumberChanged(m_communication.count());
    endRemoveRows();
    return true;
}

void 
CommunicationModel::clear() {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::clear");
    QMutexLocker locker(&mutex);
    if (!m_communication.count()) {
        return;
    }
    beginRemoveRows(QModelIndex(), 0, m_communication.count()-1);
    m_communication.clear();
    emit frameNumberChanged(m_communication.count());
    endRemoveRows();
}

//bool identifierLessThan(const Can_Frame &s1, const Can_Frame &s2)
//{
//    return s1.id < s2.id;
//}
//
//bool identifierHigherThan(const Can_Frame &s1, const Can_Frame &s2)
//{
//    return s1.id > s2.id;
//}
//
//void 
//        CommunicationModel::sort(int column, Qt::SortOrder order) {
//
//    // Sorting the identifiers
//    if (column == 1) {
//        if (order == Qt::AscendingOrder)
//            qSort(m_communication.begin(), m_communication.end(), identifierLessThan);
//        else
//            qSort(m_communication.begin(), m_communication.end(), identifierHigherThan);
//    }
//
//    emit layoutChanged();
//}

void 
CommunicationModel::clearHighlightList() {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::clearHighlightList");
    m_highlighted_items.clear();
    emit layoutChanged();
}

void 
CommunicationModel::addHighlight(unsigned int identifier) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::addHighlight");
    m_highlighted_items.insert(identifier, 0);
    emit layoutChanged();
}

bool 
CommunicationModel::highlight(unsigned int identifier) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::highlight");
    return m_highlighted_items.contains(identifier);
}

void 
CommunicationModel::setHighlightDataString(QString dataString) {
    m_dataHighlightStr = dataString;
}


void 
CommunicationModel::timerEvent(QTimerEvent *event) {
    DEBUG_COMMUNICATIONMODEL("CommunicationModel::timerEvent");
    QMutexLocker locker(&mutex);
    if (event->timerId() == m_trafficTimerId) {
        DEBUG_COMMUNICATIONMODEL("CommunicationModel::timerEvent --> m_trafficTimerId");
        if (Can_DriverFactory::instance(this)->actualDriver()->isCommunicating()) {
            //traffic_model->addTrafficSample(QDateTime::currentDateTime(),traffic_counter);
            emit frameNumberPerSecondChanged(traffic_counter);
        }
        //m_traffic.append(traffic_counter);
        traffic_counter = 0;
    } else if (event->timerId() == m_bufferCopyTimerId) {
        DEBUG_COMMUNICATIONMODEL("CommunicationModel::timerEvent --> m_bufferCopyTimerId");
        if (m_communication_buffer.count() > 0) {
            beginInsertRows(QModelIndex(),  m_communication.count(), m_communication.count()+m_communication_buffer.count()-1);
            m_communication.append(m_communication_buffer);
            m_communication_buffer.clear();
            emit frameNumberChanged(m_communication.count());
            endInsertRows();
        }
        if ((int)m_bufferCopyTimerTime != settings->displayRefreshRate()) {
            m_bufferCopyTimerTime = settings->displayRefreshRate();
            killTimer(m_bufferCopyTimerId);
            m_bufferCopyTimerId = startTimer(m_bufferCopyTimerTime);
        }
    } else {
        QAbstractTableModel::timerEvent(event);
    }
}
