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

// non QT includes
#include "PluginModel.h"
#include "mainWindow.h"

PluginModel::PluginModel(QObject *parent)
        : QAbstractTableModel(parent)
{
	id_model = IdentifiersModel::instance();
	m_settings = AppSettings::instance();

    // So that signal/slot works on can_msg
    qRegisterMetaType<can_msg>("can_msg");

	loadPlugin();
}

PluginModel*
PluginModel::instance(QObject* parent) {
	static PluginModel* singleton = 0;
	if (singleton == 0) {
		singleton = new PluginModel(parent);
	}
	return singleton;
}

PluginModel::~PluginModel()
{

}

int
PluginModel::rowCount( const QModelIndex &index) const
{
	return 0;
}

int
PluginModel::columnCount( const QModelIndex &index) const
{
	return 0;
}

QVariant 
PluginModel::data(const QModelIndex &index, int role) const
{
	return QVariant();
}

QVariant 
PluginModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

Qt::ItemFlags 
PluginModel::flags( const QModelIndex& ) const
{
	return 0;
}

bool 
PluginModel::loadPlugin()
{
	bool pluginFound = false;
	PluginInterface *pluginInterface;

	QDir pluginsDir(qApp->applicationDirPath());
 #if defined(Q_OS_WIN)
     if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
         pluginsDir.cdUp();
 #elif defined(Q_OS_MAC)
     if (pluginsDir.dirName() == "MacOS") {
         pluginsDir.cdUp();
         pluginsDir.cdUp();
         pluginsDir.cdUp();
     }
 #endif
     pluginsDir.cd("plugins");
	
     foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
         QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
         QObject *plugin = pluginLoader.instance();
         if (plugin) {
             pluginInterface = qobject_cast<PluginInterface *>(plugin);
			 if (pluginInterface) {
				 pluginInterface->setAppInterface(this);
				 pluginList.append(pluginInterface);

//				 mainWindow->addPlugin(pluginInterface);
//               addPlugin(pluginInterface);
				 //addPlugin(pluginInterface->name(),pluginInterface->ui(stackedWidget));
				 pluginFound = true;
			 }
		 }
     }
	 return pluginFound;
}

QList<PluginInterface *> 
PluginModel::getPlugins() {
	return pluginList;
}


QColor  
PluginModel::getIdColor(unsigned int identifier, bool extended) {
	return id_model->getIdColor(identifier, extended);
}

QIcon   
PluginModel::getIdColorIcon(unsigned int identifier, bool extended) {
	return id_model->getColorIcon(identifier, extended);
}

QString 
PluginModel::getIdName(unsigned int identifier, bool extended) {
	return id_model->getIdName(identifier, extended);
}

bool 
PluginModel::displayTimestamp() {
	return m_settings->displayPCTimestamp();
}

CanIdFormat
PluginModel::displayIDFormat() {
	return (CanIdFormat)(m_settings->displayIDFormat());
}

CanMessageFormat
PluginModel::displayMessageFormat() {
	return (CanMessageFormat)(m_settings->displayMessageFormat());
}

bool
PluginModel::displayMessageAscii() {
	return m_settings->displayMessageAscii();
}

bool
PluginModel::displayMessageName() {
	return m_settings->displayMessageName();
}

unsigned int
PluginModel::displayRefreshRate() {
	return (unsigned int)m_settings->displayRefreshRate();
}
