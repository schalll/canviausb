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

#ifndef PRINTABLETABLEVIEW_H
#define PRINTABLETABLEVIEW_H

// QT includes
#ifdef USEWEBKIT
	#include <QtWebKit>
#endif
#include <QTableView>
#include <QUrl>

// non QT includes
#ifndef USEWEBKIT
	#include "textprinter.h"
#endif

class PrintableTableView : public QTableView {
    Q_OBJECT

public:	
    PrintableTableView ( QWidget * parent = 0 );
    ~PrintableTableView();

public slots:
    virtual void print();
    //void printHtml(bool ok);

protected:
    // Webkit things for printing the identifiers
    QString readFile(const QString &name);
#ifdef USEWEBKIT
	QWebView *m_webview;
#else
    TextPrinter *textprinter_;
#endif
    QString m_htmlTemplate;
    QUrl m_baseUrl;
};

#endif 

