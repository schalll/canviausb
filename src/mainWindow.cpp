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
#include <QStatusBar>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QCloseEvent>

// non QT includes
#include "mainWindow.h"
#include "SideBarView.h"
#include "SideBarItem.h"
#include "WidgetBase.h"
#include "WidgetFilteredLog.h"

MainWindow::MainWindow(QWidget *parent, const char *name)
    : QMainWindow(parent)
{	
    Q_UNUSED(name);

    setAttribute(Qt::WA_DeleteOnClose);

    // Select the appropriate style sheet
    loadStyleSheet("default");

    // Create Dialogs
    canSelectDlg = new Can_SelectCanDriverDialog(this);
    preferencesDlg = new PreferencesDialog(this);
    filterDlg = new FilterDialog(this);

    // Make a gradient in the status bar
    QPalette p = statusBar()->palette();
    QLinearGradient g(QPoint(0,0), QPoint(0,statusBar()->height()));
    QColor ref = p.color(QPalette::Active, QPalette::Window);
    ref.setHsv(ref.hue(), ref.saturation(), 255);
    g.setColorAt(0, ref.dark());
    g.setColorAt((1.0/statusBar()->height()), ref.dark(105));
    g.setColorAt((1.0/statusBar()->height()) * 2, ref.dark(120));
    g.setColorAt(1, ref.dark(140));
    QBrush b(g);
    b.setColor(p.color(QPalette::Active, QPalette::Window));
    p.setBrush(QPalette::Window, b);
    statusBar()->setPalette(p);
    statusBar()->setAutoFillBackground(true);

    readSettings();
    connect(canFactory(), SIGNAL(driverIsReady(bool)),this, SLOT(enableStartStopButtons(bool)));
    connect(canFactory(), SIGNAL(driverIsCommunicating(bool)),this, SLOT(selectFromStartStopButtons(bool)));


    m_explorer = new QStackedWidget(this);							// Explorer View
    m_littleToolbar = new QToolBar(this);							// Small Toolbar between Explorer and Information Widget
    m_information = new InfoWidget(this);								// Information widget
    com_model = CommunicationModel::instance(this);
    sideBar_model = new SideBarModel(m_explorer, this);	// Sidebar model
    connect(canFactory(), SIGNAL(driverIsCommunicating(bool)),sideBar_model, SIGNAL(informPluginAboutCommunicationStatus(bool)));

    m_selector = new SideBarTree(sideBar_model, this);	// Explorer View selector
    m_quickSend = new QuickSend(this);									// QuickSend Window
    m_quickSend->setVisible(false);

    m_explorer->setCurrentIndex(0);
    connect(m_selector, SIGNAL(selectionChanged(const QModelIndex &)),this, SLOT(setCurrentExplorer(const QModelIndex &)));
    connect(m_selector, SIGNAL(setFilter()),this, SLOT(setFilter()));
    connect(m_selector, SIGNAL(selectDriver()),this, SLOT(selectDriver()));
    connect(m_selector, SIGNAL(print()),this, SLOT(print()));
    connect(m_explorer, SIGNAL(currentChanged(int)),this, SLOT(enableWindowSpecificAct(int)));

    id_model = IdentifiersModel::instance(this);
    //com_model = CommunicationModel::instance(this);
    idStats_model = IdentifiersStatsModel::instance(this);
    connect(canFactory(), SIGNAL(driverBaudrate(int)),m_information, SLOT(setBaudrate(int)));
    connect(id_model, SIGNAL(identifierNumberChanged(int)),m_information, SLOT(setIdentifierNumber(int)));
    connect(com_model, SIGNAL(frameNumberChanged(int)),m_information, SLOT(setFrameNumber(int)));
    connect(com_model, SIGNAL(frameNumberPerSecondChanged(int)),m_information, SLOT(setFramePerSecondNumber(int)));

    QWidget *rightSide = new QWidget(this);
    QVBoxLayout *rightLayout = new QVBoxLayout(this);
    rightLayout->addWidget(m_explorer);
    rightLayout->addWidget(m_quickSend);
    rightLayout->setContentsMargins(0, 0, 0, 10);
    rightSide->setLayout( rightLayout );

    QWidget *leftSide = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(this);
    leftLayout->addWidget(m_selector);
    leftLayout->addWidget(m_littleToolbar);
    leftLayout->addWidget(m_information);
    leftLayout->setContentsMargins(0, 0, 0, 10);
    leftSide->setLayout(leftLayout);
    leftSide->setMaximumWidth(250);  // To limit the width, the left sidebar can take


    splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(leftSide);
    splitter->addWidget(rightSide);
    setCentralWidget(splitter);

    connect(preferencesDlg, SIGNAL(settingsChanged()), this, SLOT(applyPreferences()));
    //connect(preferencesDlg, SIGNAL(settingsChanged()), filteredLog, SLOT(applySettings()));

    createActions();
    createMenus();
    createToolbars();


    saveAct->setEnabled(false);
    m_saveButtonEnabled = false;

    // Open most recent project
    if (recentProjectsActs[0]->isVisible()) {
        openProject(recentProjectsActs[0]->data().toString());
    }

    connect(this, SIGNAL(contentChanged()),this, SLOT(enableSaveButton()));
    connect(sideBar_model, SIGNAL(contentChanged()),this, SLOT(enableSaveButton()));
    connect(id_model, SIGNAL(contentChanged()),this, SLOT(enableSaveButton()));
    connect(m_quickSend, SIGNAL(contentChanged()),this, SLOT(enableSaveButton()));

    plugin_model = PluginModel::instance(this);
    QList<PluginInterface *> pluginList;
    pluginList = plugin_model->getPlugins();
    for (int i = 0; i < pluginList.size(); ++i) {
        addPlugin(pluginList.at(i));
    }

    //traffic_model = TrafficModel::instance();

    //m_selector->setFocus();
    creditsDlg = NULL;

    if (!m_maximized) {
        resize (m_width, m_heigth);
        if (
                (m_position.x() < 0) ||
                (m_position.x() > QApplication::desktop()->width()) ||
                (m_position.y() < 0) ||
                (m_position.y() > QApplication::desktop()->height())
                ) {
            // Center the application if the stored points are outside of the windows boundary
            centerApplication();
            showNormal();
        } else {
            move(m_position);
            showNormal();
        }
    } else {
        showMaximized();
    }
}

void
MainWindow::centerApplication() {
    QDesktopWidget *desktop = QApplication::desktop();

    int screenWidth, width;
    int screenHeight, height;
    int x, y;
    QSize windowSize;

    screenWidth = desktop->width(); // get width of screen
    screenHeight = desktop->height(); // get height of screen

    windowSize = size(); // size of our application window
    width = windowSize.width();
    height = windowSize.height();

    // little computations
    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;
    y -= 50;

    // move window to desired coordinates
    move ( x, y );
}

void 
MainWindow::setDefaultFocus() {
    m_selector->setFocus();
}

MainWindow*
MainWindow::instance(QWidget* parent) {
    static MainWindow* singleton = 0;
    if (singleton == 0) {
        singleton = new MainWindow(parent);
    }
    return singleton;
}

QSize 
MainWindow::sizeHint() const
{
    return QSize( 2000, 1000 );
}

void
MainWindow::debugOnStatus(int a) {
    statusBar()->showMessage(QString("Debug info %1").arg(a));
}

void MainWindow::loadStyleSheet(const QString &sheetName)
{
    QFile file(":/" + sheetName.toLower() + ".qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);
}

void 
MainWindow::newProject() {
    clearIdentifierList();
    clearLog();
    removeFilteredLists();
    curProject = QString("");
    setWindowTitle(tr("CANviaUSB"));

    // The project is started with changes that need to be saved
    saveAct->setEnabled(true);
    m_saveButtonEnabled = true;
}

void 
MainWindow::openProject() {
    openProject("");
}

void 
MainWindow::openProject(const QString &path)
{
    if (okToContinue()) {
        unsigned int count;
        QString fileName;
        if (path == "") //if (path.isNull())
            fileName = QFileDialog::getOpenFileName(this, tr("Choose a data file"), "", "*.cvu");
        else
            fileName = path;

        if (!fileName.isEmpty()) {

            // open the file and check we can read from it
            QFile file( fileName );
            if ( !file.open( QIODevice::ReadOnly ) )
            {
                statusBar()->showMessage( QString("Failed to open '%1'").arg(fileName) , 2000);
                return;
            }

            // open an xml stream reader and load simulation data
            QXmlStreamReader  stream( &file );

            // Empty identifier model
            count = id_model->rowCount(QModelIndex());
            if (count > 0) id_model->removeRows(0, count, QModelIndex());

            // Remove filtered lists
            sideBar_model->removeFilteredLists();

            while ( !stream.atEnd() )
            {
                stream.readNext();
                if ( stream.isStartElement() )
                {
                    if ( stream.name() == "CANviaUSB" ) {
                        while ( !stream.atEnd() )
                        {
                            stream.readNext();
                            if (stream.isStartElement() && stream.name() == "identifier" ) id_model->readXMLStream( &stream );
                            if (stream.isStartElement() && stream.name() == "filteredList" ) sideBar_model->readXMLStream( &stream );
                        }
                    } else
                        stream.raiseError( QString("Unrecognised element '%1'").arg(stream.name().toString()) );
                }
            }

            // check if error occured
            if ( stream.hasError() )
            {
                file.close();
                statusBar()->showMessage( QString("Failed to load '%1' (%2)").arg(fileName).arg(stream.errorString()), 2000);

                // Empty identifier model
                count = id_model->rowCount(QModelIndex());
                if (count > 0) id_model->removeRows(0, count, QModelIndex());

                // Remove filtered lists
                sideBar_model->removeFilteredLists();

                return;
            }

            // close file, display new scene, delete old scene, and display useful message
            file.close();
            setCurrentProject(fileName);
            statusBar()->showMessage(tr("Loaded %1").arg(fileName), 2000);

            // The project is loaded without any changes that need to be saved
            saveAct->setEnabled(false);
            m_saveButtonEnabled = false;
        }
    }
}

void MainWindow::save()
{
    if (curProject.isEmpty())
        saveAs();
    else {
        saveProject(curProject);
        saveAct->setEnabled(false);
        m_saveButtonEnabled = false;
    }
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save project as"), "", "*.cvu");
    if (fileName.isEmpty())
        return;

    saveProject(fileName);
    saveAct->setEnabled(false);
    m_saveButtonEnabled = false;
}

void 
MainWindow::saveProject(const QString &fileName)
{
    if (!fileName.isEmpty()) {

        // open the file and check we can write to it
        QFile file( fileName );
        if ( !file.open( QIODevice::WriteOnly ) )
        {
            statusBar()->showMessage( QString("Failed to write to '%1'").arg(fileName) );
            return;
        }

        // open an xml stream writer and write simulation data
        QXmlStreamWriter  stream( &file );
        stream.setAutoFormatting( true );
        stream.writeStartDocument();
        stream.writeStartElement( "CANviaUSB" );
        stream.writeAttribute( "version", "0.1" );
        stream.writeAttribute( "user", QString(getenv("USER")) );
        stream.writeAttribute( "when", QDateTime::currentDateTime().toString(Qt::ISODate) );
        id_model->writeXMLStream( &stream );
        sideBar_model->writeXMLStream( &stream );
        stream.writeEndDocument();

        // close the file and display useful message
        file.close();
        setCurrentProject(fileName);
        statusBar()->showMessage(tr("Saved %1").arg(fileName), 2000);
    }
}

void MainWindow::openRecentProject()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        openProject(action->data().toString());
}

void MainWindow::print()
{

    printf("hello\n\r");
    qDebug() << QString("MainWindow::print test");

    WidgetBase* widget = static_cast<WidgetBase*>(m_explorer->widget(m_explorer->currentIndex()));
    widget->print();
}

void 
MainWindow::addFilteredList()
{
    unsigned int row = sideBar_model->addFilteredList();
    emit contentChanged();
    m_selector->renameFilteredLog(row);
}

void
MainWindow::clearIdentifierList()
{
    unsigned int count = id_model->rowCount(QModelIndex());
    if (count > 0) id_model->removeRows(0, count, QModelIndex());
    emit contentChanged();
}

void
MainWindow::clearLog()
{
    com_model->clear();
    idStats_model->clear();
    //traffic_model->clear();

}

void
MainWindow::removeFilteredLists()
{
    sideBar_model->removeFilteredLists();
}

void
MainWindow::selectDriver()
{
    // Si elle est deja affichee, on cache la boite de dialogue sinon on l'affiche
    canSelectDlg->setVisible(!canSelectDlg->isVisible());
    emit contentChanged();
}

void 
MainWindow::setPreferences()
{
    preferencesDlg->setVisible(!preferencesDlg->isVisible());
}

void 
MainWindow::applyPreferences()
{
    for (int i=0; i<m_explorer->count();i++) {
        WidgetBase* widget = static_cast<WidgetBase*>(m_explorer->widget(i));
        widget->applyPreferences();
    }
}

void 
MainWindow::setFilter()
{
    filterDlg->clear();
    WidgetFilteredLog* widget = static_cast<WidgetFilteredLog*>(m_explorer->widget(m_explorer->currentIndex()));
    // Get the list of identifiers including id, extended, color and name
    QList<identifierStruct> identifiersList = id_model->getIdentifiers();
    // Get the current filter list from the widget
    QList<identifierStruct> filterList = widget->getFilter();
    QListIterator<identifierStruct> i(identifiersList);
    identifierStruct id;
    while (i.hasNext()) {
        id = i.next();
        if (filterList.contains(id)) {
            filterDlg->addFilteredIdentifer(id_model->getIdentifierDescription(id.identifier, id.extended), id);
        } else {
            filterDlg->addNonFilteredIdentifer(id_model->getIdentifierDescription(id.identifier, id.extended), id);
        }
    }
    if (filterDlg->exec() == QDialog::Accepted) {
        // Get the new filter list and populate this one to the widget
        filterList = filterDlg->getFilter();


        // JUST FOR DEBUGGING PURPOSE
        qDebug() << QString("-- Filter --");
        QListIterator<identifierStruct> hopla(filterList);
        identifierStruct hopla_id;
        while (hopla.hasNext()) {
            hopla_id = hopla.next();
            qDebug() << QString("Filter contains %1 - %2").arg(hopla_id.identifier).arg((int)(hopla_id.extended));
        }


        widget->setFilter(filterList);
    }

    /*

    QList<unsigned int> identifiersList = id_model->getIdentifiersList();
    // Get the current filter list from the widget
    QList<unsigned int> filterList = widget->getFilter();
    QListIterator<unsigned int> i(identifiersList);
    unsigned int id;
    while (i.hasNext()) {
        id = i.next();
        if (filterList.contains(id)) {
            filterDlg->addFilteredIdentifer(id_model->getIdentifierDescription(id, true), id); // TODO
        } else {
            filterDlg->addNonFilteredIdentifer(id_model->getIdentifierDescription(id, true), id); // TODO
        }
    }
    if (filterDlg->exec() == QDialog::Accepted) {
        // Get the new filter list and populate this one to the widget
        filterList = filterDlg->getFilter();
        widget->setFilter(filterList);
    }

*/
    emit contentChanged();
}

void
MainWindow::startStopCommunication()
{
    if (canFactory()->actualDriver()->isCommunicating()) {
        // Stop communication
        canFactory()->actualDriver()->stopCommunication();

        newAct->setEnabled(true);
        openAct->setEnabled(true);
        if (m_saveButtonEnabled) saveAct->setEnabled(true);
        saveAsAct->setEnabled(true);
        clearIdentifiersAct->setEnabled(true);
        selectDriverAct->setEnabled(true);
    } else {
        // Start communication
        canFactory()->actualDriver()->startCommunication();
        newAct->setEnabled(false);
        openAct->setEnabled(false);
        saveAct->setEnabled(false);
        saveAsAct->setEnabled(false);
        clearIdentifiersAct->setEnabled(false);
        selectDriverAct->setEnabled(false);
    }
}

void
MainWindow::selectFromStartStopButtons(bool started)
{
    if (!started) {
        startAct->setEnabled(true);
        stopAct->setEnabled(false);

    } else {
        startAct->setEnabled(false);
        stopAct->setEnabled(true);

    }
}

void
MainWindow::enableStartStopButtons(bool enable) {
    startStopGroup->setDisabled(!enable);
}

void
MainWindow::enableSaveButton() {
    m_saveButtonEnabled = true;
    saveAct->setEnabled(true);
}

void 
MainWindow::createActions()
{
    // File menu actions
    newAct = new QAction(tr("&New project"), this);
    newAct->setIcon(QIcon(":/document.png"));
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Start a new project"));
    newAct->setVisible(true);
    connect(newAct, SIGNAL(triggered()), this, SLOT(newProject()));

    openAct = new QAction(tr("&Open project"), this);
    openAct->setIcon(QIcon(":/folder.png"));
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open a project"));
    openAct->setVisible(true);
    connect(openAct, SIGNAL(triggered()), this, SLOT(openProject()));

    saveAct = new QAction(tr("&Save project"), this);
    saveAct->setIcon(QIcon(":/disk.png"));
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the project"));
    saveAct->setVisible(true);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save project &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the project under a new name"));
    saveAsAct->setVisible(true);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    for (int i = 0; i < MaxRecentProjects; ++i) {
        recentProjectsActs[i] = new QAction(this);
        recentProjectsActs[i]->setVisible(false);
        connect(recentProjectsActs[i], SIGNAL(triggered()), this, SLOT(openRecentProject()));
    }

    printAct = new QAction(tr("&Print"), this);
    printAct->setIcon(QIcon(":/printer.png"));
    printAct->setStatusTip(tr("Print the content of the cutrent view"));
    printAct->setEnabled(false);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    closeAct = new QAction(tr("E&xit"), this);
    closeAct->setShortcut(tr("Ctrl+Q"));
    closeAct->setStatusTip(tr("Exit the application"));
    //connect(closeAct, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

    // Edit menu actions
    addFilteredListAct = new QAction(tr("Add a filtered log"), this);
    addFilteredListAct->setShortcut(tr("Ctrl+F"));
    addFilteredListAct->setIcon(QIcon(":/filter_add.png"));
    addFilteredListAct->setStatusTip(tr("Add a filtered communication log"));
    addFilteredListAct->setVisible(true);
    connect(addFilteredListAct, SIGNAL(triggered()), this, SLOT(addFilteredList()));

    setFilterAct = new QAction(tr("Set filter"), this);
    setFilterAct->setIcon(QIcon(":/filter_edit.png"));
    setFilterAct->setStatusTip(tr("Set the filter preferences"));
    setFilterAct->setEnabled(false);
    connect(setFilterAct, SIGNAL(triggered()), this, SLOT(setFilter()));

    clearIdentifiersAct = new QAction(tr("Clear Identifiers List"), this);
    clearIdentifiersAct->setIcon(QIcon(":/clear_identifiers.png"));
    clearIdentifiersAct->setStatusTip(tr("Clear Identifiers List"));
    clearIdentifiersAct->setVisible(true);
    connect(clearIdentifiersAct, SIGNAL(triggered()), this, SLOT(clearIdentifierList()));

    clearLogAct = new QAction(tr("Clear communication log"), this);
    clearLogAct->setIcon(QIcon(":/window_delete.png"));
    clearLogAct->setStatusTip(tr("Clear the communication log"));
    clearLogAct->setVisible(true);
    connect(clearLogAct, SIGNAL(triggered()), this, SLOT(clearLog()));

    preferencesAct = new QAction(tr("Preferences"), this);
    preferencesAct->setShortcut(tr("Ctrl+P"));
    preferencesAct->setIcon(QIcon(":/settings.png"));
    preferencesAct->setStatusTip(tr("Project preferences"));
    preferencesAct->setVisible(true);
    connect(preferencesAct, SIGNAL(triggered()), this, SLOT(setPreferences()));

    selectDriverAct = new QAction(tr("Select CAN interface"), this);
    selectDriverAct->setIcon(QIcon(":/PCI-card.png"));
    selectDriverAct->setStatusTip(tr("Select a CAN interface"));
    selectDriverAct->setVisible(true);
    connect(selectDriverAct, SIGNAL(triggered()), this, SLOT(selectDriver()));

    //    propertiesAct = new QAction(tr("Properties"), this);
    //    propertiesAct->setStatusTip(tr("Set the application properties"));
    //    propertiesAct->setVisible(true);
    //    connect(propertiesAct, SIGNAL(activated()), this, SLOT(setProperties()));

    // About menu actions
    aboutAct = new QAction(tr("About this application..."), this);
    aboutAct->setStatusTip(tr("About this application"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    creditsAct = new QAction(tr("Credits"), this);
    creditsAct->setStatusTip(tr("CANviaUSB credits"));
    connect(creditsAct, SIGNAL(triggered()), this, SLOT(credits()));

    // Recording actions
    startAct = new QAction(tr("Start"), this);
    startAct->setShortcut(tr("F5"));
    startAct->setIcon(QIcon(":/start_green.png"));
    startAct->setStatusTip(tr("Start the communication"));
    connect(startAct, SIGNAL(triggered()), this, SLOT(startStopCommunication()));

    stopAct = new QAction(tr("Stop"), this);
    stopAct->setShortcut(tr("Ctrl+F5"));
    stopAct->setIcon(QIcon(":/stop_red.png"));
    stopAct->setStatusTip(tr("Stop the communication"));
    stopAct->setEnabled(false);
    connect(stopAct, SIGNAL(triggered()), this, SLOT(startStopCommunication()));

    startStopGroup = new QActionGroup(this);
    startStopGroup->addAction(startAct);
    startStopGroup->addAction(stopAct);
}

void 
MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addSeparator();
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(printAct);
    //fileMenu->addAction(printIdentifiersAct);
    //fileMenu->addAction(printCommunicationLogAct);

    separatorAct = fileMenu->addSeparator();
    for (int i = 0; i < MaxRecentProjects; ++i) {
        fileMenu->addAction(recentProjectsActs[i]);
    }
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    updateRecentProjectActions();

    editMenu = menuBar()->addMenu(tr("Edit"));
    editMenu->addAction(startAct);
    editMenu->addAction(stopAct);
    editMenu->addSeparator();
    editMenu->addAction(addFilteredListAct);
    editMenu->addAction(setFilterAct);
    editMenu->addSeparator();
    editMenu->addAction(clearIdentifiersAct);
    editMenu->addAction(clearLogAct);
    editMenu->addSeparator();
    editMenu->addAction(preferencesAct);
    editMenu->addAction(selectDriverAct);
    //editMenu->addSeparator();
    //editMenu->addAction(propertiesAct);


    aboutMenu = menuBar()->addMenu(tr("About"));
    aboutMenu->addAction(creditsAct);
    aboutMenu->addAction(aboutAct);
}

void 
MainWindow::createToolbars() {
    toolBar = addToolBar(tr("&toolbar"));
    toolBar->addAction(newAct);
    toolBar->addAction(openAct);
    toolBar->addAction(saveAct);
    toolBar->addSeparator();
    toolBar->addAction(startAct);
    toolBar->addAction(stopAct);
    toolBar->addSeparator();
    //toolBar->addAction(clearIdentifiersAct);
    toolBar->addAction(clearLogAct);

    m_littleToolbar->addAction(addFilteredListAct);
    m_littleToolbar->addAction(setFilterAct);
}

void 
MainWindow::enableWindowSpecificAct(int index) {
    WidgetBase* widget = static_cast<WidgetBase*>(m_explorer->widget(index));
    printAct->setEnabled(widget->canPrint());
    setFilterAct->setEnabled(widget->isFilter());
    m_quickSend->setVisible(widget->canQuickSend());
}

void 
MainWindow::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        canFactory()->deleteAllDriver();
        writeSettings();
        event->accept();
        qApp->quit(); // To make sure that the debug window will also be closed
    } else {
        event->ignore();
    }
}

bool 
MainWindow::okToContinue()
{
    bool tmp = false;
    if (m_saveButtonEnabled) {
        QMessageBox msgBox;
        msgBox.setText("The project has been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Save:
            // Save was clicked
            save();
            tmp = true;
            break;
        case QMessageBox::Discard:
            // Don't Save was clicked
            return true;
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked
            break;
        default:
            // should never be reached
            break;
        }
    } else {
        // No changes have been made to the project
        tmp = true;
    }
    return tmp;
    
}

void 
MainWindow::writeSettings()
{
    qDebug("Write settings");
    QSettings settings;
    //settings.setValue("geometry",geometry());
    settings.setValue("sidebarWidth",m_selector->width());
    settings.setValue("driver",canFactory()->currentDriver());
    QSize m_size = size();
    settings.setValue("applicationMaximized",isMaximized());
    settings.setValue("applicationWidth",m_size.width());
    settings.setValue("applicationHeight",m_size.height());
    settings.setValue("applicationPosition",pos());
}

void MainWindow::readSettings()
{
    qDebug("Read settings");
    QSettings settings;
    QString driverName = settings.value("driver",canFactory()->defaultDriver()).toString();
    canFactory()->selectDriver(driverName);
    m_maximized = settings.value("applicationMaximized",false).toBool();
    m_width = settings.value("applicationWidth",800).toInt();
    m_heigth = settings.value("applicationHeight",600).toInt();
    m_position = settings.value("applicationPosition",QPoint(0,0)).toPoint();
}

void 
MainWindow::credits() {
    if (!creditsDlg) creditsDlg = new CreditsDialog(this);
    creditsDlg->setModal(true);
    creditsDlg->show();
}

void 
MainWindow::about()
{
    QMessageBox about(QCoreApplication::applicationName()+ tr(" - About"),
                      tr("CANviaUSB Monitor - Alpha 8 <br><br> %2<br><br> %3<br><br> %4<br><br>Build with Qt %5<br><br>")
                      .arg("")
                      .arg("Copyright (C) 2015 Laurent Schall")
                      .arg("THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETER IN AN ACTION OF CONTRACT, TORT OR OTHERWHISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.")
                      .arg(qVersion()),
                      QMessageBox::NoIcon,
                      QMessageBox::Ok,
                      QMessageBox::NoButton,
                      QMessageBox::NoButton,
                      this
                      );
    //about.setIconPixmap( QPixmap(":/images/clienticon.png") );
    about.exec();
}


void MainWindow::setCurrentProject(const QString &fileName)
{
    curProject = fileName;
    if (curProject.isEmpty())
        setWindowTitle(tr("CANviaUSB"));
    else
        setWindowTitle(tr("%1 - %2").arg(tr("CANviaUSB")).arg(strippedName(curProject)));

    QSettings settings;
    QStringList files = settings.value("recentProjectList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentProjects)
        files.removeLast();

    settings.setValue("recentProjectList", files);

    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentProjectActions();
    }
}


void
MainWindow::updateRecentProjectActions()
{
    QSettings settings;
    QStringList files = settings.value("recentProjectList").toStringList();

    int numRecentProjects = qMin(files.size(), (int)MaxRecentProjects);

    for (int i = 0; i < numRecentProjects; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentProjectsActs[i]->setText(text);
        recentProjectsActs[i]->setData(files[i]);
        recentProjectsActs[i]->setVisible(true);
    }
    for (int j = numRecentProjects; j < MaxRecentProjects; ++j)
        recentProjectsActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentProjects > 0);
}

QString
MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}


Can_DriverFactory *
MainWindow::canFactory() {
    return Can_DriverFactory::instance(this);
}

QString 
MainWindow::currentProjectName() const {
    return curProject;
}

void 
MainWindow::setCurrentExplorer(const QModelIndex &index) {
    WidgetBase *widget = (WidgetBase *)sideBar_model->getItem(index)->ui();
    if (widget)
        widget->activateWidget();
}


CreditsDialog::CreditsDialog(QWidget *parent)
    : QDialog(parent) {
    //credits = new QPlainTextEdit(this);
    credits = new QTextEdit;
    credits->setReadOnly(true);
    QFile file(":/credits.txt");
    if (file.open(QIODevice::ReadOnly))
        credits->setText(file.readAll());
    //credits->setHtml(file.readAll());
    // TO DO else in case file cannot be found

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(credits);
    setLayout(layout);
    setWindowTitle(QCoreApplication::applicationName() + tr(" - Credits"));

    resize(750, 400);
}

void 
MainWindow::addFilteredListFromSelection(const QString &name, QList<identifierStruct> filter) {
    unsigned int row = sideBar_model->addFilteredList(name, filter);
    m_selector->renameFilteredLog(row);
    emit contentChanged();
}

void 
MainWindow::addPlugin(PluginInterface* itf) {
    sideBar_model->addPlugin(itf);
    emit contentChanged();
}
