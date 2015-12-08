/***************************************************************************
*   Copyright (C) 2011 by
*   Laurent Schall
***************************************************************************/

#ifndef PLUGINMODEL_H
#define PLUGINMODEL_H

#define DEBUG_PLUGINMODEL_ON 0 // 0 to ignore, 1 to display debug messages
#define DEBUG_PLUGINMODEL(x) { if (DEBUG_PLUGINMODEL_ON) qDebug(x); }

// QT includes
#include <QAbstractTableModel>

// non QT includes
#include "pluginInterface.h"
#include "identifiers\IdentifiersModel.h"
#include "appSettings.h"

class PluginModel : public QAbstractTableModel, public AppInterface
{
    Q_OBJECT

public:
    static PluginModel* instance(QObject* parent = 0);
	~PluginModel();

	QList<PluginInterface *> getPlugins();

    int rowCount( const QModelIndex &index) const;
    int columnCount( const QModelIndex &index) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags( const QModelIndex& ) const;

	// Plugin interface - TODO Place the upcoming functions in a different file (f.e. AppInterface.cpp)
	bool displayTimestamp();		
    CanIdFormat displayIDFormat(); 
    CanMessageFormat displayMessageFormat();	
    bool displayMessageAscii();	
    bool displayMessageName();
    unsigned int displayRefreshRate();

	QColor getIdColor(unsigned int identifier, bool extended);
	QIcon getIdColorIcon(unsigned int identifier, bool extended);
	QString getIdName(unsigned int identifier, bool extended);

private:
    PluginModel(QObject *parent = 0);
	bool loadPlugin();

	QList<PluginInterface *> pluginList;
	IdentifiersModel* id_model;
	AppSettings* m_settings;
};

#endif
