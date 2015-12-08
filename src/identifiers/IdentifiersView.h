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

#ifndef IDENTIFIERSVIEW_H
#define IDENTIFIERSVIEW_H

// QT includes
#include <QTableView>
//#include <QItemEditorFactory>

// non QT includes
#include "IdentifiersModel.h"
#include "IdentifiersDelegate.h"
#include "../PrintableTableView.h"

class IdentifiersView : public PrintableTableView
{
    Q_OBJECT

public:
    IdentifiersView(QWidget* parent = 0 );
    ~IdentifiersView();

public slots:
    void print();

private slots:
    void deleteIdentifier();
    void createFilteredListFromSelection();

private:
    void createActions();
    void createContextMenu();

    void printTable(QPrinter* printer, QPainter* painter, const QRect& area);
    void printDocument(QPrinter *printer);

    QAction *deleteIdentifierAct;
    QAction *createFilteredListFromSelectionAct;

    IdentifiersModel* m_model;
    IdentifiersDelegate* m_delegate;

};
#endif
