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

#ifndef IDENTIFIERSMODEL_H
#define IDENTIFIERSMODEL_H

#define DEBUG_IDENTIFIERSMODEL_ON 0 // 0 to ignore, 1 to display debug messages
#define DEBUG_IDENTIFIERSMODEL(x) { if (DEBUG_IDENTIFIERSMODEL_ON) qDebug(x); }

// QT includes
#include <QAbstractItemModel>
#include <QStandardItemModel>
#include <QColor>
#include <QMutex>

// non QT includes
#include "../appSettings.h"

struct identifierStruct 
{
    unsigned int identifier;	// identifier
    bool extended;						// standard versus extended

    bool operator==(const identifierStruct id) const
    {
        return ((identifier == id.identifier) && (extended == id.extended));
    }
};

typedef unsigned int dataFormat;
Q_DECLARE_METATYPE(dataFormat) // _DECLARE_METATYPE() makes the type known to all template based functions, including QVariant

struct identifierExtendedStruct
{
    identifierStruct id;			// identifier structure (id + standard vs extended)
    QColor color;					// color associated to this identifier
    QIcon icon;					// icon made of the color associated to the identifier
    QString name;					// name associated to this identifer
    dataFormat format;			// display scheme associated to this identifier
};

class QXmlStreamWriter;
class QXmlStreamReader;

class IdentifiersModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    static IdentifiersModel* instance(QWidget* parent = 0 );
    
    /** Reimplimentations */
    int rowCount( const QModelIndex &index) const;
    int columnCount( const QModelIndex &index) const;

    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    Qt::ItemFlags flags( const QModelIndex& ) const ;

    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool insertRows(int position, int rows, const QModelIndex &index=QModelIndex());
    bool removeRows(int position, int rows, const QModelIndex &index=QModelIndex());

    void sort (int column, Qt::SortOrder order = Qt::AscendingOrder );

    // Identifiers accessing related functions
    QList<identifierStruct> getIdentifiers();									// Simple version of the id list (only id + standard vs extended)
    QList<identifierExtendedStruct> getVerboseIdentifiers();	// Extended version of the id list

    //QList<unsigned int> getIdentifiersList();
    QString getIdentifierDescription(unsigned int id, bool extended);
    bool addId(unsigned int identifier, bool extended, QColor color = QColor(Qt::white), QString name = QString(""));

    QColor getIdColor(unsigned int identifier, bool extended);
    QIcon getColorIcon(unsigned int identifier, bool extended); // This will be used instead of getIdColor because of QT bug 8357 http://bugreports.qt.nokia.com/browse/QTBUG-8357
    QString getIdName(unsigned int identifier, bool extended);
    unsigned int getIdFormat(unsigned int identifier, bool extended);

    // TO Save/Load from project file
    void writeXMLStream(QXmlStreamWriter* stream);
    void readXMLStream(QXmlStreamReader* stream);

signals:
    void identifierNumberChanged(int number);
    void contentChanged();

private:
    mutable QMutex mutex;
    IdentifiersModel(QObject *parent = 0);
    QList<identifierExtendedStruct> m_identifiers;

    AppSettings* settings;

    QIcon createColorIcon(QColor color);
};

#endif
