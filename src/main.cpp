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
#include <QApplication>
#include <QMutex>
#include <QTextBrowser>
#include <QMessageBox>

// non QT includes
#include "mainWindow.h"

//#include <vld.h>

// Application debug handling
// use qDebug, qCritical to get a debug Message in here
// The output of the debug output is redirected to this windows due to the Message Handler installed in main()
void debugOutput(QtMsgType type, const char *msg) {
    QMutex mutex;
    mutex.lock();
    static QTextBrowser* browser = new QTextBrowser();
    browser->setWindowTitle("Debug Window");
    browser->move(0,0);
    browser->show();
    switch(type) {
    case QtDebugMsg:
        browser->append(QString("<font color=black>%1</font>").arg(msg));
        break;
    case QtWarningMsg:
        browser->append(QString("<font color=blue>%1</font>").arg(msg));
        break;
    case QtCriticalMsg:
        browser->append(QString("<font color=orange>%1</font>").arg(msg));
        break;
    case QtFatalMsg:
        browser->append(QString("<font color=red>%1</font>").arg(msg));
        break;
    }
    mutex.unlock();
}

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QApplication::setWindowIcon(QIcon(":/logo.png"));
    QApplication::addLibraryPath(qApp->applicationDirPath());

    QCoreApplication::setOrganizationName("CANviaUSB");
    QCoreApplication::setOrganizationDomain("www.canviausb.com");
    QCoreApplication::setApplicationName("CANviaUSB");

    MainWindow *mainWindow = MainWindow::instance();
    mainWindow->show();
    mainWindow->setDefaultFocus();
    return app.exec();
}
