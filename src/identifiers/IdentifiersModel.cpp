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

/* Developper mode
*
*  A/ DataChanged signal was not emitted in Edit strategie On FieldChange
*	 The base model was not emitting the dataChanged signal when the edit strategie was set to OnFieldChange
*  This signal is used by the view to resize columns and rows to theier contents
*/

// QT includes
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QPainter>

// Non QT includes
#include "IdentifiersModel.h"

IdentifiersModel::IdentifiersModel(QObject *parent) : QAbstractTableModel(parent) 
{	
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::IdentifiersModel")
		settings = AppSettings::instance();
}

IdentifiersModel*
        IdentifiersModel::instance(QWidget* parent) {
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::instance")
    static IdentifiersModel* singleton = 0;
    if (singleton == 0) {
        singleton = new IdentifiersModel((QObject *)parent);
    }
    return singleton;
}

int
        IdentifiersModel::rowCount( const QModelIndex &index) const
{
    Q_UNUSED(index);
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::rowCount")
    return m_identifiers.count();
}

int
        IdentifiersModel::columnCount( const QModelIndex &index) const
{
    Q_UNUSED(index);
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::columnCount")
    return 4;
}

QVariant 
        IdentifiersModel::headerData (int section, Qt::Orientation orientation, int role) const
{
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::headerData")
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
                        case 0:
            return QString(tr("CAN Identifier"));
                        case 1:
            return QString(tr("Color"));
                        case 2:
            return QString(tr("Data display format"));
                        case 3:
            return QString(tr("Name"));
						default:
            return QString("");
        }
    }

    return QString("");
}


 QString formatAsString(unsigned int format)
{
	QString tmp;
	
	unsigned int i;
	i=1;
	do {
		// 0xXXXXXXXX
		// shift 4 bits per position
		// position 1 shift 4 * 7
		// position 2 shift 4 * 6
		// position 3 shift 4 * 5
		// position 4 shift 4 * 4
		// position 1 shift 4 * 3
		// position 2 shift 4 * 2
		// position 3 shift 4 * 1
		// position 4 shift 4 * 0
		switch((format >> 4*(8-i))& 0xf) {
			case 0: // 8 bits
				tmp = tmp + QString("[8]");
				i = i+1;
				break;
			case 1: // 16 bits
				if (i<8) {
					tmp = tmp + QString("[16]");
					i = i+2;
				} else {
					tmp = tmp + QString("[8]");
					i = i+1;
				}
				break;
			case 2: // 32 bits
				if (i<6) {
					tmp = tmp + QString("[32]");
					i = i+4;
				} else {
					tmp = tmp + QString("[8]");
					i = i+1;
				}
				break;
			case 3: // 64 bits
				if (i<2) {
					tmp = tmp + QString("[64]");
					i = i+8;
				} else {
					tmp = tmp + QString("[8]");
					i = i+1;
				}
				break;
			case 15: // binary
				tmp = tmp + QString("[b]");
				i = i+1;
				break;
			default:
				tmp = tmp + QString("[8]");
				i = i+1;
				break;	
		}

	} while (i<9);
	
	return tmp;
}


QVariant
        IdentifiersModel::data(const QModelIndex &index, int role ) const
{
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::data")
    QMutexLocker locker(&mutex);
		int row = index.row();
    int col = index.column();

    if (!index.isValid())
        return QVariant();

    const identifierExtendedStruct m_idStruct = m_identifiers.at(row);

    if (col == 0) {
        if (role == Qt::DisplayRole) {
						QString infoLabel;
						if (!m_idStruct.id.extended) {
							infoLabel = QString("(STD)");
						} else {
							infoLabel = QString("(EXT)");
						}

            switch(settings->displayIDFormat()) {
                                        case 0:
                return QString("%1  %2  ").arg(infoLabel).arg(m_idStruct.id.identifier);
                break;
                                        case 1:
                return QString("%1  0x%2  ").arg(infoLabel).arg(m_idStruct.id.identifier, 8, 16, QLatin1Char('0'));
                break;
                                        case 2:
                                        default:
                return QString("%1  0x%2 / %3  ").arg(infoLabel).arg(m_idStruct.id.identifier, 8, 16, QLatin1Char('0')).arg(m_idStruct.id.identifier);
            }
        }
        if (role == Qt::EditRole)		 return m_idStruct.id.identifier;
				if (role == Qt::UserRole)		 return m_idStruct.id.extended;
    }

    if (col == 1) {
        if (role == Qt::DisplayRole) return m_idStruct.color;
        if (role == Qt::EditRole)		 return m_idStruct.color;
    }

    if (col == 2) {
		if (role == Qt::DisplayRole) return formatAsString(m_idStruct.format);
        if (role == Qt::EditRole) return m_idStruct.format;
    }

    if (col == 3) {
        if (role == Qt::DisplayRole) return m_idStruct.name;
        if (role == Qt::EditRole) return m_idStruct.name;
    }

    return QVariant();
}

Qt::ItemFlags 
        IdentifiersModel::flags(const QModelIndex &index) const
{
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::flags")
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    // Identified cannot be edited
    if (index.column() == 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    // Name and Color are also editable
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}


bool IdentifiersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::setData")
    //QMutexLocker locker(&mutex); - already done in addId
		int row;
		identifierExtendedStruct currentId;

		if (index.isValid() && role == Qt::EditRole) {
        row = index.row();
        currentId = m_identifiers.value(row);

        if (index.column() == 0)
            currentId.id.identifier = value.toUInt();
		else if (index.column() == 1) {
            // value is a QVariant that cannot be a QColor
            // So we have to use the QVariant::value() function
            // See http://doc.trolltech.com/4.5/qvariant.html - A Note on GUI Types
            currentId.color = value.value<QColor>();
			currentId.icon = createColorIcon(currentId.color);
		} else if (index.column() == 2) {
			currentId.format = value.toUInt();
		} else if (index.column() == 3) {
            currentId.name = value.toString();
		} else
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

bool IdentifiersModel::insertRows(int position, int rows, const QModelIndex &index)
{
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::insertRows")
    //QMutexLocker locker(&mutex);  - already done in addId
		Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; row++) {
        identifierExtendedStruct newId;
        newId.id.identifier = 0;
        newId.id.extended = false;
        newId.color = QColor(Qt::white);
        newId.name = QString("");
		newId.format = 0;
        m_identifiers.insert(position, newId);
    }

    endInsertRows();
    emit identifierNumberChanged(m_identifiers.count());
    emit contentChanged();
		return true;
}

bool IdentifiersModel::removeRows(int position, int rows, const QModelIndex &index)
{
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::removeRows")
    QMutexLocker locker(&mutex);
		Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        m_identifiers.removeAt(position);
    }

    endRemoveRows();
    emit identifierNumberChanged(m_identifiers.count());
		emit contentChanged();
    return true;
}


bool identifierLessThan(const identifierExtendedStruct &s1, const identifierExtendedStruct &s2)
{
    return s1.id.identifier < s2.id.identifier;
}

bool identifierHigherThan(const identifierExtendedStruct &s1, const identifierExtendedStruct &s2)
{
    return s1.id.identifier > s2.id.identifier;
}

bool colorLessThan(const identifierExtendedStruct &s1, const identifierExtendedStruct &s2)
{
    return s1.color.name() < s2.color.name();
}

bool colorHigherThan(const identifierExtendedStruct &s1, const identifierExtendedStruct &s2)
{
    return s1.color.name() > s2.color.name();
}

bool nameLessThan(const identifierExtendedStruct &s1, const identifierExtendedStruct &s2)
{
    return s1.name < s2.name;
}

bool nameHigherThan(const identifierExtendedStruct &s1, const identifierExtendedStruct &s2)
{
    return s1.name > s2.name;
}

void 
        IdentifiersModel::sort(int column, Qt::SortOrder order) {
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::sort")
    QMutexLocker locker(&mutex);
		// Sorting the identifiers
    if (column == 0) {
        if (order == Qt::AscendingOrder)
            qSort(m_identifiers.begin(), m_identifiers.end(), identifierLessThan);
        else
            qSort(m_identifiers.begin(), m_identifiers.end(), identifierHigherThan);
    }

    // Sorting the colors
    if (column == 1) {
        if (order == Qt::AscendingOrder)
            qSort(m_identifiers.begin(), m_identifiers.end(), colorLessThan);
        else
            qSort(m_identifiers.begin(), m_identifiers.end(), colorHigherThan);
    }

    // Sorting the name
    if (column == 3) {
        if (order == Qt::AscendingOrder)
            qSort(m_identifiers.begin(), m_identifiers.end(), nameLessThan);
        else
            qSort(m_identifiers.begin(), m_identifiers.end(), nameHigherThan);
    }

    emit layoutChanged();
}

// Identifiers accessing related functions
QList<identifierStruct> 
        IdentifiersModel::getIdentifiers()
{
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::getIdentifiers")
    QMutexLocker locker(&mutex);
		QList<identifierStruct> tmp;
    QListIterator<identifierExtendedStruct> i(m_identifiers);

    identifierExtendedStruct tmpId;
    while (i.hasNext()) {
        tmpId = i.next();
        tmp.append(tmpId.id);
    }
    return tmp;
}

QList<identifierExtendedStruct> 
        IdentifiersModel::getVerboseIdentifiers()
{
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::getVerboseIdentifiers")
    return m_identifiers;
}


//QList<unsigned int> This funtion does not make sense anymore as it cannot permit to distinguish between standard and exentend
//        IdentifiersModel::getIdentifiersList()
//{
//    QList<unsigned int> tmp;
//    QListIterator<identifierStruct> i(m_identifiers);
//
//    identifierStruct id;
//    while (i.hasNext()) {
//        id = i.next();
//        tmp.append(id.identifier);
//    }
//
//    return tmp;
//}

bool 
        IdentifiersModel::addId(unsigned int identifier, bool extended, QColor color, QString name) {
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::addId")
    QMutexLocker locker(&mutex);
		QListIterator<identifierExtendedStruct> i(m_identifiers);

    identifierExtendedStruct tmpId;
    while (i.hasNext()) {
        tmpId = i.next();
        if ((tmpId.id.identifier == identifier) && (tmpId.id.extended == extended))
            return false;
    }

    // The id was not in the list - so we are going to add it
    insertRows(0, 1, QModelIndex());
    QModelIndex m_index = index(0, 0, QModelIndex());
    setData(m_index, identifier, Qt::EditRole);
		setData(m_index, extended, Qt::UserRole);
    m_index = index(0, 1, QModelIndex());
    setData(m_index, color, Qt::EditRole);
    m_index = index(0, 3, QModelIndex());
    setData(m_index, name, Qt::EditRole);
    return true;
}

QColor 
        IdentifiersModel::getIdColor(unsigned int identifier, bool extended) { 
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::getIdColor")
		QMutexLocker locker(&mutex);
		QListIterator<identifierExtendedStruct> i(m_identifiers);
    identifierExtendedStruct tmpId;
    while (i.hasNext()) {
        tmpId = i.next();
        if ((tmpId.id.identifier == identifier) && (tmpId.id.extended == extended))
            return tmpId.color;
    }
    return QColor(Qt::white);
}

QIcon 
IdentifiersModel::getColorIcon(unsigned int identifier, bool extended) { 
		DEBUG_IDENTIFIERSMODEL("IdentifiersModel::getColorIcon")
		QMutexLocker locker(&mutex);
		QListIterator<identifierExtendedStruct> i(m_identifiers);
    identifierExtendedStruct tmpId;
    while (i.hasNext()) {
        tmpId = i.next();
        if ((tmpId.id.identifier == identifier) && (tmpId.id.extended == extended))
            return tmpId.icon;
    }
    return QIcon();
}


QString 
        IdentifiersModel::getIdName(unsigned int identifier, bool extended) {
    DEBUG_IDENTIFIERSMODEL("IdentifiersModel::getIdName")
		QMutexLocker locker(&mutex);
		QListIterator<identifierExtendedStruct> i(m_identifiers);
    identifierExtendedStruct tmpId;
    while (i.hasNext()) {
        tmpId = i.next();
        if ((tmpId.id.identifier == identifier) && (tmpId.id.extended == extended))
            return tmpId.name;
    }
    return QString("");
}

unsigned int 
IdentifiersModel::getIdFormat(unsigned int identifier, bool extended) {
    DEBUG_IDENTIFIERSMODEL("IdentifiersModel::getIdFormat")
		QMutexLocker locker(&mutex);
		QListIterator<identifierExtendedStruct> i(m_identifiers);
    identifierExtendedStruct tmpId;
    while (i.hasNext()) {
        tmpId = i.next();
        if ((tmpId.id.identifier == identifier) && (tmpId.id.extended == extended))
			return tmpId.format;
    }
    return 0;
}

void  
        IdentifiersModel::writeXMLStream( QXmlStreamWriter* stream )
{
    DEBUG_IDENTIFIERSMODEL("IdentifiersModel::writeXMLStream")
    for (int row = 0; row < rowCount(QModelIndex()); ++row) {
        stream->writeEmptyElement( "identifier" );
        stream->writeAttribute( "id", QString("%1").arg(data(index(row, 0, QModelIndex()), Qt::EditRole).toString()) );
        stream->writeAttribute( "extended", QString("%1").arg(data(index(row, 0, QModelIndex()), Qt::UserRole).toBool()) );
        stream->writeAttribute( "color", QString("%1").arg(data(index(row, 1, QModelIndex()), Qt::EditRole).toString()) );
#ifdef VIEW_FORMAT_COLUMN
		stream->writeAttribute( "format", QString("%1").arg(data(index(row, 2, QModelIndex()), Qt::EditRole).toString()) );
#endif
		stream->writeAttribute( "name", QString("%1").arg(data(index(row, 3, QModelIndex()), Qt::EditRole).toString()) );
    }
}

void 
        IdentifiersModel::readXMLStream(QXmlStreamReader* stream)
{
    DEBUG_IDENTIFIERSMODEL("IdentifiersModel::readXMLStream")
    QMutexLocker locker(&mutex);
		int row = m_identifiers.count();

    insertRows(row, 1, QModelIndex());
    foreach( QXmlStreamAttribute attribute, stream->attributes() )
    {
        if ( attribute.name() == "id" ) setData(index(row, 0, QModelIndex()),attribute.value().toString());
		if ( attribute.name() == "extended" ) setData(index(row, 0, QModelIndex()),attribute.value().toString(), Qt::UserRole);   
		if ( attribute.name() == "color" ) setData(index(row, 1, QModelIndex()),attribute.value().toString(), Qt::EditRole);
        if ( attribute.name() == "format" ) setData(index(row, 2, QModelIndex()),attribute.value().toString(), Qt::EditRole);
		if ( attribute.name() == "name" ) setData(index(row, 3, QModelIndex()),attribute.value().toString(), Qt::EditRole);
    }
}

QString 
        IdentifiersModel::getIdentifierDescription(unsigned int id, bool extended) {
    DEBUG_IDENTIFIERSMODEL("IdentifiersModel::getIdentifierDescription")
		QString infoLabel;
		if (!extended) {
			infoLabel = QString("(STD)");
		} else {
			infoLabel = QString("(EXT)");
		}

    switch(settings->displayIDFormat()) {
                case 0:
        return QString("%1  %2  - %3").arg(infoLabel).arg(id).arg(getIdName(id, extended));
        break;
                case 1:
        return QString("%1  0x%2  - %3").arg(infoLabel).arg(id, 8, 16, QLatin1Char('0')).arg(getIdName(id, extended));
        break;
                case 2:
                default:
        return QString("%1  0x%2 / %3  - %4").arg(infoLabel).arg(id, 8, 16, QLatin1Char('0')).arg(id).arg(getIdName(id, extended));
    }
}

 QIcon IdentifiersModel::createColorIcon(QColor color)
 {
  DEBUG_IDENTIFIERSMODEL("IdentifiersModel::createColorIcon")
	QPixmap pixmap(20, 20);
    QPainter painter(&pixmap);
	painter.setPen(Qt::NoPen);
    painter.fillRect(QRect(0, 0, 20, 20), color);

    //QPixmap pixmap(16, 16);
	//QPainter painter(&pixmap);
    //QRadialGradient gradient(5, 5, 5, 3, 3);
    //gradient.setColorAt(0.2, Qt::white);
    //gradient.setColorAt(0.8, color);
    //painter.setBrush(gradient);
    //painter.drawRect(0, 0, 16, 16);

    return QIcon(pixmap);
 }
