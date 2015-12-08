/***************************************************************************
*   Copyright (C) 2010 by
*   Laurent Schall
***************************************************************************/

#ifndef PLUGININTERFACEPRIVATE_H
#define PLUGININTERFACEPRIVATE_H

// QT includes

// non QT includes
#include "pluginInterface.h"
#include "CommunicationModel.h"

// This class provides read-only access to the data holded by the communication model
class CommunicationModelInterfacePrivate : public CommunicationModelInterface
{
    Q_OBJECT
public:
	CommunicationModelInterfacePrivate(QObject *parent = 0);

    Qt::ItemFlags flags( const QModelIndex& ) const ;
    
	QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
	int rowCount( const QModelIndex &index) const;
    int columnCount( const QModelIndex &index) const;
private:
	CommunicationModel* com_model;
};

#endif // PLUGININTERFACEPRIVATE_H