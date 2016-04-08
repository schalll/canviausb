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

// non QT includes
#include "IdentifiersStatsModel.h"


IdentifiersStatsModel::IdentifiersStatsModel(QObject *parent): QAbstractTableModel(parent)
{
    id_model = IdentifiersModel::instance();
    settings = AppSettings::instance();
}

IdentifiersStatsModel*
IdentifiersStatsModel::instance(QWidget* parent) {

    DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::instance")

            static IdentifiersStatsModel* singleton = 0;
    if (singleton == 0) {
        singleton = new IdentifiersStatsModel(parent);
    }
    return singleton;
}

int
IdentifiersStatsModel::rowCount(const QModelIndex &index) const
{
    DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::rowCount")
            Q_UNUSED(index);
    //QMutexLocker locker(&mutex); <- will produce dead lock when called from IdentifiersStatsModel::insertRows
    return m_identifiersStats.count();
}

int
IdentifiersStatsModel::columnCount(const QModelIndex &index) const
{
    DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::columnCount")
            Q_UNUSED(index);
    return 7;
}

QVariant
IdentifiersStatsModel::data(const QModelIndex &index, int role) const
{
    QMutexLocker locker(&mutex);
    DEBUG_IDENTIFIERSTATSMODEL(">>IdentifiersStatsModel::data")

            int row = index.row();
    int col = index.column();

    identifierCommunicationStats tmpStat = m_identifiersStats.at(row);


    // Last seen
    if (col == 0) {
        if (role == Qt::DisplayRole) {
            return QString("  %1 ").arg(tmpStat.latestFrame.time_pc.toString("hh:mm:ss.zzz"));
        }
    }

    // Identifier
    if (col == 1) {
        if (role == Qt::DecorationRole) return id_model->getColorIcon(tmpStat.id.identifier, tmpStat.id.extended);  // getColorIcon is used instead of getIdColor because of QT bug 8357 http://bugreports.qt.nokia.com/browse/QTBUG-8357
        if (role == Qt::DisplayRole) {

            // TODO - Make the diff between RTR and non RTR frames
            QString infoLabel;
            if (!tmpStat.id.extended) {
                infoLabel = QString("(STD)");
            } else {
                infoLabel = QString("(EXT)");
            }
            // Keep this for once we will have made the diff between RTR and non RTR frames
            //						if (!m_frame.ext) {
            //							if (!m_frame.rtr) infoLabel = QString("(STD)");
            //							else infoLabel = QString("(Remote STD)");
            //						} else {
            //							if (!m_frame.rtr) infoLabel = QString("(EXT)");
            //							else infoLabel = QString("(Remote EXT)");
            //						}
            switch(settings->displayIDFormat()) {
            case 0:
                return QString("%1 %2 ").arg(infoLabel).arg(tmpStat.id.identifier);
                break;
            case 1:
                return QString("%1 0x%2 ").arg(infoLabel).arg(tmpStat.id.identifier, 8, 16, QLatin1Char('0'));
                break;
            case 2:
            default:
                return QString("%1 0x%2 / %3 ").arg(infoLabel).arg(tmpStat.id.identifier, 8, 16, QLatin1Char('0')).arg(tmpStat.id.identifier);
            }
        }
        if (role == Qt::EditRole)			  return tmpStat.id.identifier;
        if (role == Qt::UserRole+1)			  return tmpStat.id.extended;
        if (role == Qt::UserRole+2)			  return id_model->getIdColor(tmpStat.id.identifier, tmpStat.id.extended);
    }

    // Counts
    if (col == 2) {
        if (role == Qt::DisplayRole) {
            return QString("%1").arg(tmpStat.counts);
        }
        if (role == Qt::TextAlignmentRole) return Qt::AlignHCenter;
    }

    // DLC
    if (col == 3) {
        if (role == Qt::DisplayRole)
            if (!tmpStat.latestFrame.rtr) return QString("(%1)").arg(tmpStat.latestFrame.dlc);
            else return QString("");
        if (role == Qt::TextAlignmentRole) return Qt::AlignHCenter;
    }

    if (col == 4) {
        /*
    QString tmp = QString("");
    if (!tmpStat.latestFrame.rtr) {

     bool MessageHex;
     if (settings->displayMessageFormat() == 1)
       MessageHex = true;
     else
       MessageHex = false;

     unsigned int format = id_model->getIdFormat(tmpStat.latestFrame.id, tmpStat.latestFrame.ext);
     unsigned int i;
     i=1;
     do {
      switch((format >> 4*(8-i))& 0xf) {
       case 0: // 8 bits
        //qDebug() << QString("format at position %1 is 8 bits").arg(i) << endl;
        if (i < (unsigned int)(tmpStat.latestFrame.dlc+1)) {
         if (MessageHex)
           tmp =  tmp + QString("%1 ").arg(tmpStat.latestFrame.data[i], 2, 16, QLatin1Char('0'));
         else
           tmp =  tmp + QString("%1 ").arg(tmpStat.latestFrame.data[i], 3, 10, QLatin1Char('0'));
        }
        i = i+1;
        break;
       case 1: // 16 bits
        //qDebug() << QString("format at position %1 is 16 bits ...").arg(i);
        if ((i<8) && (i+1 < (unsigned int)(tmpStat.latestFrame.dlc+1))) {
         //qDebug() << QString("... let's display 16 bits") << endl;
         if (MessageHex) {
          tmp =  tmp + QString("%1 ").arg((unsigned long)((tmpStat.latestFrame.data[i] << 8) + tmpStat.latestFrame.data[i+1]), 4, 16, QLatin1Char('0'));
         } else {
          tmp =  tmp + QString("%1 ").arg((unsigned long)((tmpStat.latestFrame.data[i] << 8) + tmpStat.latestFrame.data[i+1]), 5, 10, QLatin1Char('0'));
         }
         i = i+2;
        } else {
         //qDebug() << QString("... let's display 8 bits") << endl;
         if (i < (unsigned int)(tmpStat.latestFrame.dlc+1)) {
          if (MessageHex)
            tmp =  tmp + QString("%1 ").arg(tmpStat.latestFrame.data[i], 2, 16, QLatin1Char('0'));
          else
            tmp =  tmp + QString("%1 ").arg(tmpStat.latestFrame.data[i], 3, 10, QLatin1Char('0'));
         }
         i = i+1;
        }
        break;
       case 2: // 32 bits
        //qDebug() << QString("format at position %1 is 32 bits").arg(i) << endl;
        if ((i<6) && (i+3 < (unsigned int)(tmpStat.latestFrame.dlc+1))) {
         if (MessageHex)
          tmp =  tmp + QString("%1 ").arg((unsigned long)((tmpStat.latestFrame.data[i] << 24) + (tmpStat.latestFrame.data[i+1] << 16) + (tmpStat.latestFrame.data[i+2] << 8) + tmpStat.latestFrame.data[i+3]), 8, 16, QLatin1Char('0'));
         else
          tmp =  tmp + QString("%1 ").arg((unsigned long)((tmpStat.latestFrame.data[i] << 24) + (tmpStat.latestFrame.data[i+1] << 16) + (tmpStat.latestFrame.data[i+2] << 8) + tmpStat.latestFrame.data[i+3]), 10, 10, QLatin1Char('0'));
         i = i+4;
        } else {
         if (i < (unsigned int)(tmpStat.latestFrame.dlc+1)) {
          if (MessageHex)
            tmp =  tmp + QString("%1 ").arg(tmpStat.latestFrame.data[i], 2, 16, QLatin1Char('0'));
          else
            tmp =  tmp + QString("%1 ").arg(tmpStat.latestFrame.data[i], 3, 10, QLatin1Char('0'));
         }
         i = i+1;
        }
        break;
       case 15: // binary
        //qDebug() << QString("format at position %1 is binary").arg(i) << endl;
        tmp =  tmp + QString("%1b ").arg(tmpStat.latestFrame.data[i], 8, 2, QLatin1Char('0'));
        i = i+1;
        break;
       default:
        if (i < (unsigned int)(tmpStat.latestFrame.dlc+1)) {
         if (MessageHex)
           tmp =  tmp + QString("%1 ").arg(tmpStat.latestFrame.data[i], 2, 16, QLatin1Char('0'));
         else
           tmp =  tmp + QString("%1 ").arg(tmpStat.latestFrame.data[i], 3, 10, QLatin1Char('0'));
        }
        i = i+1;
        break;
      }

     } while (i<9);
    } else {
     tmp = QString("(length = %1)").arg(tmpStat.latestFrame.dlc);
    }
    if (role == Qt::DisplayRole) {
     return tmp;
    }
*/

        if (role == Qt::DisplayRole) {
            QString tmp;
            if (!tmpStat.latestFrame.rtr) {
                tmp = QString("%1 %2 %3 %4 %5 %6 %7 %8");
                bool MessageHex;
                if (settings->displayMessageFormat() == 1)
                    MessageHex = true;
                else
                    MessageHex = false;
                for (unsigned int i = 0; i < 8; ++i) {
                    if (i < tmpStat.latestFrame.dlc) {
                        // Message in HExadecimal
                        if (MessageHex)
                            tmp =  tmp.arg(tmpStat.latestFrame.data[i], 2, 16, QLatin1Char('0'));
                        else
                            tmp =  tmp.arg(tmpStat.latestFrame.data[i], 3, 10, QLatin1Char('0'));
                    }
                    else {
                        if (MessageHex)
                            tmp = tmp.arg("  ");
                        else
                            tmp = tmp.arg("   ");
                    }
                }
            } else {
                tmp = QString("(length = %1)").arg(tmpStat.latestFrame.dlc);
            }
            return tmp;
        }

    }

    if (col == 5) {
        if (role == Qt::DisplayRole) {
            QString ascii = QString("%1%2%3%4%5%6%7%8");
            for (unsigned int i=0; i<8; ++i) {
                if (i < tmpStat.latestFrame.dlc) {
                    if ((tmpStat.latestFrame.data[i]>31) && (tmpStat.latestFrame.data[i]<127))
                        ascii = ascii.arg(QLatin1Char(tmpStat.latestFrame.data[i]));
                    else
                        ascii = ascii.arg(".");
                }
                else {
                    ascii = ascii.arg(" ");
                }
            }
            return ascii;
        }
    }

    if (col == 6) {
        if (role == Qt::DisplayRole)		return id_model->getIdName(tmpStat.id.identifier, tmpStat.id.extended);
    }

    return QVariant();
}

QVariant
IdentifiersStatsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::headerData")
            if (role != Qt::DisplayRole)
            return QVariant();

    if (orientation == Qt::Horizontal) {
        if (section == 0) return QString(tr("Last seen"));	// Time this packet was last received/send
        if (section == 1) return QString(tr("Identifier"));	// IDentifier
        if (section == 2) return QString(tr("Count"));			// Number of occurencies
        if (section == 3) return QString(tr("DLC"));
        if (section == 4) return QString(tr("Data"));				// Last received datas
        if (section == 5) return QString(tr("Ascii"));
        if (section == 6) return QString(tr("Name"));
    }

    return QVariant();
}

Qt::ItemFlags
IdentifiersStatsModel::flags(const QModelIndex &index) const
{
    DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::flags")
            if (!index.isValid())
            return Qt::ItemIsEnabled;

    // Nothing can be edited
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

/*

bool
IdentifiersStatsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::setData")


    int row;
  identifierExtendedStruct currentId;

  if (index.isValid() && role == Qt::EditRole) {
        row = index.row();
        currentId = m_identifiers.value(row);

        if (index.column() == 0)
            currentId.id.identifier = value.toUInt();
        else if (index.column() == 1)
            // value is a QVariant that cannot be a QColor
            // So we have to use the QVariant::value() function
            // See http://doc.trolltech.com/4.5/qvariant.html - A Note on GUI Types
            currentId.color = value.value<QColor>();
        else if (index.column() == 2)
            currentId.name = value.toString();
        else
            return false;

        m_identifiers.replace(row, currentId);
        emit(dataChanged(index, index));
    emit contentChanged();
        return true;
    }

  // User role
    if (index.isValid() && role == Qt::UserRole) {
        row = index.row();
        currentId = m_identifiers.value(row);
        if (index.column() == 0)
            currentId.id.extended = value.toBool();
        else
            return false;

        m_identifiers.replace(row, currentId);
        emit(dataChanged(index, index));
    emit contentChanged();
        return true;
    }


    return false;
}

*/

bool
IdentifiersStatsModel::insertRows(int position, int rows, const QModelIndex &index) {
    DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::insertRows")
            Q_UNUSED(index);
    //QMutexLocker locker(&mutex); <- will produce dead lock when called from IdentifiersStatsModel::addFrame
    beginInsertRows(QModelIndex(), position, position+rows-1);

    //for (int row=0; row < rows; row++) {
    //    identifierCommunicationStats tmpId;
    //    m_identifiersStats.insert(position, tmpId);
    //}
    endInsertRows();
    return true;
}



bool
IdentifiersStatsModel::addFrame(const Can_Frame &frame) {

    QString tmp = QString(">>IdentifiersStatsModel::addFrame id = %1").arg(frame.id);
    QMutexLocker locker(&mutex);

    QListIterator<identifierCommunicationStats> i(m_identifiersStats);
    identifierCommunicationStats tmpId;
    bool foundIdInStats = false;
    unsigned int tmpIndex = 0;
    while (i.hasNext()) {
        tmpId = i.next();

        // TODO - Make the diff between RTR and non RTR frames

        if ((tmpId.id.identifier == frame.id) && (tmpId.id.extended == frame.ext)) {
            tmpId.counts = tmpId.counts + 1;
            tmpId.latestFrame = frame;
            foundIdInStats = true;
            m_identifiersStats.replace(tmpIndex,tmpId);
            QModelIndex transposedIndex1 = createIndex(tmpIndex,0);
            QModelIndex transposedIndex2 = createIndex(tmpIndex,6);
            emit dataChanged(transposedIndex1,transposedIndex2);
            break; // exit while
        }
        tmpIndex++;
    }
    DEBUG_IDENTIFIERSTATSMODEL(">>IdentifiersStatsModel::addFrame - 2")


            if (!foundIdInStats) {
        tmpId.id.identifier = frame.id;
        tmpId.id.extended = frame.ext;
        tmpId.counts = 1;
        tmpId.latestFrame = frame;
        m_identifiersStats.append(tmpId);
        //qDebug() << QString("Add new Identifier %1 %2 %3").arg(tmpId.id.identifier).arg(frame.ext).arg(tmpId.counts);
        // The id was not in the list so we are going to add this id
        insertRows(0,1,QModelIndex());
        DEBUG_IDENTIFIERSTATSMODEL(">>IdentifiersStatsModel::addFrame - 3")
    }
    return true;
}

bool
IdentifiersStatsModel::findDLC(const int identifier, const bool extended, int &dlc) {
    DEBUG_IDENTIFIERSTATSMODEL(">>IdentifiersStatsModel::findDLC")
            QMutexLocker locker(&mutex);
    QListIterator<identifierCommunicationStats> i(m_identifiersStats);
    identifierCommunicationStats tmpId;

    while (i.hasNext()) {
        tmpId = i.next();

        if ((tmpId.id.identifier == identifier) && (tmpId.id.extended == extended)) {
            dlc = tmpId.latestFrame.dlc;
            DEBUG_IDENTIFIERSTATSMODEL(">>IdentifiersStatsModel::findDLC - 2")
                    return true;
        }
    }
    DEBUG_IDENTIFIERSTATSMODEL(">>IdentifiersStatsModel::findDLC - 3")
            return false;
}


bool
IdentifiersStatsModel::removeRows(int position, int rows, const QModelIndex &index) {
    DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::removeRows")
            Q_UNUSED(index);
    QMutexLocker locker(&mutex);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        m_identifiersStats.removeAt(position);
    }
    endRemoveRows();
    return true;
}

void
IdentifiersStatsModel::clear() {
    DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::clear")
            QMutexLocker locker(&mutex);
    if (!m_identifiersStats.count()) {
        return;
    }
    beginRemoveRows(QModelIndex(), 0, m_identifiersStats.count()-1);
    m_identifiersStats.clear();
    endRemoveRows();
}


bool identifierLessThan(const identifierCommunicationStats &s1, const identifierCommunicationStats &s2)
{
    return s1.id.identifier < s2.id.identifier;
}

bool identifierHigherThan(const identifierCommunicationStats &s1, const identifierCommunicationStats &s2)
{
    return s1.id.identifier > s2.id.identifier;
}

bool countsLessThan(const identifierCommunicationStats &s1, const identifierCommunicationStats &s2)
{
    return s1.counts < s2.counts;
}

bool countsHigherThan(const identifierCommunicationStats &s1, const identifierCommunicationStats &s2)
{
    return s1.counts > s2.counts;
}

bool dateLessThan(const identifierCommunicationStats &s1, const identifierCommunicationStats &s2)
{
    return s1.latestFrame.time_pc < s2.latestFrame.time_pc;
}

bool dateHigherThan(const identifierCommunicationStats &s1, const identifierCommunicationStats &s2)
{
    return s1.latestFrame.time_pc > s2.latestFrame.time_pc;
}

bool dlcLessThan(const identifierCommunicationStats &s1, const identifierCommunicationStats &s2)
{
    return s1.latestFrame.dlc < s2.latestFrame.dlc;
}

bool dlcHigherThan(const identifierCommunicationStats &s1, const identifierCommunicationStats &s2)
{
    return s1.latestFrame.dlc > s2.latestFrame.dlc;
}

void
IdentifiersStatsModel::sort(int column, Qt::SortOrder order) {
    DEBUG_IDENTIFIERSTATSMODEL("IdentifiersStatsModel::sort")

            // Sorting the last seen
            if (column == 0) {
        if (order == Qt::AscendingOrder)
            qSort(m_identifiersStats.begin(), m_identifiersStats.end(), dateLessThan);
        else
            qSort(m_identifiersStats.begin(), m_identifiersStats.end(), dateHigherThan);
    }

    // Sorting the identifiers
    if (column == 1) {
        if (order == Qt::AscendingOrder)
            qSort(m_identifiersStats.begin(), m_identifiersStats.end(), identifierLessThan);
        else
            qSort(m_identifiersStats.begin(), m_identifiersStats.end(), identifierHigherThan);
    }

    // Sorting the counts
    if (column == 2) {
        if (order == Qt::AscendingOrder)
            qSort(m_identifiersStats.begin(), m_identifiersStats.end(), countsLessThan);
        else
            qSort(m_identifiersStats.begin(), m_identifiersStats.end(), countsHigherThan);
    }

    // Sorting the DLC
    if (column == 3) {
        if (order == Qt::AscendingOrder)
            qSort(m_identifiersStats.begin(), m_identifiersStats.end(), dlcLessThan);
        else
            qSort(m_identifiersStats.begin(), m_identifiersStats.end(), dlcHigherThan);
    }

    // Sorting the data - NOT IMPLEMENTED (DOES THIS MAKE SENSE)
    // Sorting the ascii - NOT IMPLEMENTED (DOES THIS MAKE SENSE)
    // Sorting the name - (Name is retreived from the id model, so this could be tricky and so high in CPU usage)

    emit layoutChanged();

}

QString
IdentifiersStatsModel::getIdentifierStatistics(unsigned int identifier, bool extended) {
    Q_UNUSED(identifier)
    Q_UNUSED(extended)
    QString tmp(tr("Some statistics"));
    return tmp;

}
