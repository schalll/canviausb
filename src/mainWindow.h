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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// QT includes
#include <QMainWindow>
#include <QTextEdit>

// Non QT includes
#include "SideBarModel.h"
#include "can/can_driverfactory.h"
#include "can/can_selectdriverdialog.h"
#include "can/can_frame.h"
#include "PreferencesDialog.h"
#include "identifiers/IdentifiersModel.h"
#include "CommunicationModel.h"
#include "identifiers/IdentifiersView.h"
#include "CommunicationView.h"
#include "filterDialog.h"
#include "QuickSend.h"
#include "infoWidget.h"
#include "IdentifiersStatsModel.h"
#include "pluginInterface.h"
#include "PluginModel.h"

class QAction;
class QMenu;

class QSplitter;
class SideBarTree;
class QStackedWidget;

class CreditsDialog : public QDialog
{
    Q_OBJECT
public:
    CreditsDialog(QWidget* parent = 0);
private:
    QTextEdit *credits;
    //QPlainTextEdit *credits;
};


class MainWindow : public QMainWindow 
{
    Q_OBJECT
public:
    static MainWindow* instance(QWidget* parent = 0 );
    QString currentProjectName() const;
    void setDefaultFocus();
    void addFilteredListFromSelection(const QString &name, QList<identifierStruct> filter);
    void addPlugin(PluginInterface* itf);

protected:
    virtual QSize sizeHint () const;
    void loadStyleSheet(const QString &sheetName);
    void closeEvent(QCloseEvent *event);

signals:
    void contentChanged();

public slots:
    void setFilter();

private slots:
    void newProject();
    void openProject();
    void openProject(const QString &path);
    void save();
    void saveAs();
    void saveProject(const QString &fileName);
    void openRecentProject();
    void print();

    void addFilteredList();
    void clearIdentifierList();
    void clearLog();
    void removeFilteredLists();
    void selectDriver();
    void setPreferences();
    void applyPreferences();

    void credits();
    void about();
    void debugOnStatus(int a);

    void startStopCommunication();
    void selectFromStartStopButtons(bool started);
    void enableStartStopButtons(bool enable);

    void setCurrentExplorer(const QModelIndex &);
    void enableWindowSpecificAct(int index);
    void enableSaveButton();

private:
    MainWindow(QWidget *parent = 0, const char *name = 0);

    void createActions();
    void createMenus();
    void createToolbars();

    bool m_saveButtonEnabled;
    bool okToContinue();

    void writeSettings();
    void readSettings();

    void centerApplication();

    // Toolbar
    QToolBar *toolBar;
    QToolBar *m_littleToolbar;

    // File menu actions
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *printAct;
    QAction *closeAct;

    // Edit menu actions
    QAction *addFilteredListAct;
    QAction *setFilterAct;
    QAction *clearIdentifiersAct;
    QAction *clearLogAct;
    QAction *selectDriverAct;
    QAction *preferencesAct;

    // View menu actions
    QAction *zoomInAct;
    QAction *zoomOutAct;

    // About menu actions
    QAction *creditsAct;
    QAction *aboutAct;
    QAction *debugAct;

    QAction *separatorAct;

    QString curProject;
    void setCurrentProject(const QString &filename);
    enum {MaxRecentProjects = 5 };
    QAction *recentProjectsActs[MaxRecentProjects];

    // recording actions
    QAction *startAct;
    QAction *stopAct;

    QActionGroup *startStopGroup;

    // List of menu entries
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *viewMenu;
    QMenu *aboutMenu;

    Can_SelectCanDriverDialog *canSelectDlg;
    PreferencesDialog *preferencesDlg;
    FilterDialog *filterDlg;

    void updateRecentProjectActions();
    QString strippedName(const QString &fullFileName);

    // Main interface
    QSplitter				*splitter;
    SideBarTree				*m_selector;		// Sidebar to select the explorer view
    QStackedWidget			*m_explorer;		// Main explorer to display the view
    QuickSend				*m_quickSend;		// QuickSend window displayed along communication views
    InfoWidget				*m_information;	// Information widget

    // Can Driver related
    Can_DriverFactory *canFactory();
    Can_Driver *can;

    //Interfaces to the different models
    SideBarModel* sideBar_model;
    IdentifiersModel* id_model;
    CommunicationModel* com_model;
    IdentifiersStatsModel* idStats_model;
    PluginModel* plugin_model;

    //Interfaces to the different views
    CommunicationView* com_view;

    CreditsDialog *creditsDlg;

    // For the application size at startup
    bool m_maximized;
    int m_width;
    int m_heigth;
    QPoint m_position;
};

#endif // MONITOR_H
