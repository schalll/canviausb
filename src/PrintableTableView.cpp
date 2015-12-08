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
#include "PrintableTableView.h"
#include "mainWindow.h"

PrintableTableView::PrintableTableView( QWidget * parent ): QTableView(parent){	
#ifdef USEWEBKIT
	m_webview = new QWebView(this);
	m_webview->setVisible(false);
#else
    textprinter_ = new TextPrinter(this);
    textprinter_->setHeaderSize(10);
    textprinter_->setFooterSize(10);
    textprinter_->setDateFormat("MMMM dd yyyy");
#endif
}

PrintableTableView::~PrintableTableView(){
}

void 
        PrintableTableView::print() {

    #ifdef USEWEBKIT
    QString html;

    QString header;
    QString footer;

    MainWindow *mainWindow = MainWindow::instance();

    // Header
    header = QString("\
                     <div id=header>\
                             <div id=logo>\
                                     <table border=0>\
                                                   <tr>\
                                                   <th><h1>Empty page</h1></th>\
                                                   <th><h2>from %1</h2></th>\
                                                   </tr>\
                                                   </table>\
                                                   <h3>printed on %2</h3>\
                                                   </div>\
                                                   </div>").arg(mainWindow->currentProjectName()).arg(QDate::currentDate().toString());
    html = header;


    // Footer
    footer = QString("\
                     <div id=footer>\
                             <p>Copyright (c) 2009 CANviaUSB. All rights reserved.</p>\
                             </div>");
    html = html + footer;
    m_htmlTemplate = readFile(QLatin1String(":/identifiers.html"));
    m_htmlTemplate.replace(QLatin1String("<!-- CONTENT -->"), html);
    m_baseUrl = QUrl(QLatin1String("qrc:/identifiers.html"));
    connect(m_webview, SIGNAL(loadFinished(bool)),this, SLOT(printHtml(bool)));
    m_webview->setHtml(m_htmlTemplate, m_baseUrl);
		#endif
}

/*
void 
        PrintableTableView::printHtml(bool ok) {
    #ifdef USEWEBKIT	
		disconnect(m_webview, SIGNAL(loadFinished(bool)),this, SLOT(printHtml(bool)));
    QPrinter printer;
    QPrintDialog printDialog(&printer, this);
    //Do not allow to print only the selected identifiers
    printDialog.setOption(QAbstractPrintDialog::PrintSelection,false);
    printDialog.setOption(QAbstractPrintDialog::PrintPageRange,false);

    if (printDialog.exec() == QDialog::Accepted) {
        m_webview->print(&printer);
    }
		#endif
}
*/

QString 
        PrintableTableView::readFile(const QString &name)
{
    QFile f(name);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning("Unable to open %s: %s", name.toUtf8().constData(), f.errorString().toUtf8().constData());
        return QString();
    }
    QTextStream ts(&f);
    return ts.readAll();
}
