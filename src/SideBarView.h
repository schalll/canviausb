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

#ifndef SIDEBARVIEW_H
#define SIDEBARVIEW_H

// QT includes
#include <QTreeView>

// non QT includes
#include "SideBarModel.h"
#include "SideBarDelegate.h"

class SideBarTree : public QTreeView
{
    Q_OBJECT

public:
    SideBarTree(SideBarModel* model, QWidget* parent );
    ~SideBarTree();

protected:     
    virtual void currentChanged( const QModelIndex& current, const QModelIndex& previous );
    int rows(const QModelIndex &index) const;

    virtual QSize sizeHint () const;

    // These are reimplementations to make sure that certain item can not be selected
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
    virtual QModelIndex moveCursor ( CursorAction cursorAction, Qt::KeyboardModifiers modifiers );

signals:
    void selectionChanged(const QModelIndex &index);
    void setFilter();
    void selectDriver();
    void print();


public slots:
    void setDriverStatus(int status);
	void renameFilteredLog(unsigned int row);

private slots:
    void renameFilteredLog();
    void deleteFilteredLog();

private:
    SideBarModel* m_model;
    SideBarDelegate* m_delegate;

    int m_driverStatus;

    void createActions();
    void createContextMenu();

    QAction *selectDriverAct;
    QAction *renameFilteredLogAct;
    QAction *deleteFilteredLogAct;
    QAction *printAct;
    QAction *setFilterAct;
};

#endif
