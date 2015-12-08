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
#include <QDebug>
#include <QHeaderView>
#include <QMouseEvent>
#include <QStandardItemModel>

// non QT includes
#include "SideBarItem.h"
#include "SideBarView.h"
#include "SideBarDelegate.h"
#include "WidgetDrivers.h"

#ifdef Q_WS_MAC
static const int MyProfileFontSize = 12;
#else
static const int MyProfileFontSize = 11;
#endif

SideBarTree::SideBarTree(SideBarModel* m_model, QWidget* parent ) :
        QTreeView( parent )
{	
    this->m_model = m_model;
    m_delegate = new SideBarDelegate(this);

    //m_model->setParent(this);
    //m_delegate->setParent(this);

    setModel(m_model);
    setItemDelegate(m_delegate);

    // Define the size of the displayed icons
    setIconSize(QSize( 16, 16 ));
    // Don't show the header
    header()->hide();
    // Don't use any scroll bar
    setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

    setItemsExpandable (false);
    expandAll();
    // Select the default index
    setCurrentIndex(model()->index(1, 1, rootIndex()));

    //installEventFilter(this);
    //qDebug("SideBarTree constructor");

    setSizePolicy ( QSizePolicy::Minimum, QSizePolicy::Expanding);

    createActions();
    createContextMenu();
}

SideBarTree::~SideBarTree()
{
}


QSize 
        SideBarTree::sizeHint() const
{
    return QSize( 200, 1000 );
}

int 
        SideBarTree::rows(const QModelIndex &index) const
{
    return model()->rowCount(model()->parent(index));
}

QModelIndex 
        SideBarTree::moveCursor ( CursorAction cursorAction, Qt::KeyboardModifiers modifiers )
{
    Q_UNUSED(modifiers);

    // We make sure here that moving up and down the cursor is not going
    // to select non selectable items

    int currentItemType, nextItemType, previousItemType;

    QModelIndex current = currentIndex();
    SideBarItem *currentItem = m_model->getItem(current);
    currentItemType = currentItem->type();

    QModelIndex previous = m_model->index(current.row()-1,0);
    if (previous.isValid()) {
        SideBarItem *previousItem = m_model->getItem(previous);
        previousItemType = previousItem->type();
    } else {
        previousItemType = SideBar::TypeUnknown;
    }

    QModelIndex next = m_model->index(current.row()+1,0);
    if (next.isValid()) {
        SideBarItem *nextItem = m_model->getItem(next);
        nextItemType = nextItem->type();
    } else {
        nextItemType = SideBar::TypeUnknown;
    }

    switch (cursorAction) {
                case MoveLeft:
                case MoveUp:
                    // Special treatment when beeing after Spacer 1
                    if (previousItemType == SideBar::Spacer1) {
                        return current;
                    }

                    // Special treatment when beeing after Spacer 2
                    if (previousItemType == SideBar::Spacer2) {
                        current = model()->index(current.row() - 2, current.column(), rootIndex());
                        return current;
                    }

                    // Special treatment when beeing after Spacer 3
                    if (previousItemType == SideBar::Spacer3) {
                        current = model()->index(current.row() - 2, current.column(), rootIndex());
                        return current;
                    }

                    // Regular treatment
                    current = model()->index(current.row() - 1, current.column(), rootIndex());
                    break;
                case MoveRight:
                case MoveDown:
                    // Special treatment when beeing before Spacer 2
                    if (nextItemType == SideBar::Spacer2) {
                        current = model()->index(current.row() + 2, current.column(), rootIndex());
                        return current;
                    }

                    // Special treatment when beeing before Spacer 3
                    if (nextItemType == SideBar::Spacer3) {
                        // Allow to go down Spacer 3 only if there is some plugins there
						if (m_model->pluginsCount() > 0) {
							current = model()->index(current.row() + 2, current.column(), rootIndex());
						}
                        return current;
                    }

                    // Special treatment when beeing before end
                    if (nextItemType == SideBar::end) {
                        return current;
                    }

                    // Regular treatment
                    if (current.row() < rows(current) - 1)
                        current = model()->index(current.row() + 1, current.column(), rootIndex());
                    break;
                default:
                    break;
                }
    viewport()->update();
    return current;
}

void 
        SideBarTree::mousePressEvent(QMouseEvent *event)
{
    // We make sure here that the mousePressEvent is not going
    // to be handled on non selectable Item
    QPoint pos = event->pos();
    QModelIndex index = indexAt(pos);
    SideBarItem *item = m_model->getItem(index);
    int itemType = item->type();

    if (itemType == SideBar::Spacer1) return;
    if (itemType == SideBar::Spacer2) return;
    if (itemType == SideBar::Spacer3) return;
	if (itemType == SideBar::end) return;

    selectDriverAct->setEnabled(itemType == SideBar::device);
    selectDriverAct->setVisible(itemType == SideBar::device);

    printAct->setEnabled(itemType == SideBar::comLog || itemType == SideBar::filteredList || itemType == SideBar::identifiers || itemType == SideBar::occurences);
    printAct->setVisible(itemType == SideBar::comLog || itemType == SideBar::filteredList || itemType == SideBar::identifiers || itemType == SideBar::occurences);

    //Enable the possibility to delete the filtered log only when we are on a filtered log item
    deleteFilteredLogAct->setEnabled(itemType == SideBar::filteredList);
    deleteFilteredLogAct->setVisible(itemType == SideBar::filteredList);
    renameFilteredLogAct->setEnabled(itemType == SideBar::filteredList);
    renameFilteredLogAct->setVisible(itemType == SideBar::filteredList);
    setFilterAct->setEnabled(itemType == SideBar::filteredList);
    setFilterAct->setVisible(itemType == SideBar::filteredList);
    QTreeView::mousePressEvent(event);
}

void 
        SideBarTree::mouseMoveEvent(QMouseEvent *event){
    // We make sure here that the mouseMoveEvent is not going
    // to be handled on non selectable Item
    QPoint pos = event->pos();
    QModelIndex index = indexAt(pos);
    SideBarItem *item = m_model->getItem(index);
    int itemType = item->type();

    if (itemType == SideBar::Spacer1) return;
    if (itemType == SideBar::Spacer2) return;
    if (itemType == SideBar::Spacer3) return;

    QTreeView::mouseMoveEvent(event);
}  

void 
        SideBarTree::mouseReleaseEvent(QMouseEvent *event)
{
    // We make sure here that the mouseReleaseEvent is not going
    // to be handled on non selectable Item
    QPoint pos = event->pos();
    QModelIndex index = indexAt(pos);
    SideBarItem *item = m_model->getItem(index);
    int itemType = item->type();

    if (itemType == SideBar::Spacer1) return;
    if (itemType == SideBar::Spacer2) return;
    if (itemType == SideBar::Spacer3) return;

    QTreeView::mouseReleaseEvent(event);
}   

void 
        SideBarTree::wheelEvent(QWheelEvent *event) {

    //qDebug() << QString("SideBarTree::wheelEvent with delta %1").arg(event->delta());
    if (event->delta()>0) {
        setCurrentIndex(moveCursor(QAbstractItemView::MoveUp, Qt::NoModifier));
    } else {
        setCurrentIndex(moveCursor(QAbstractItemView::MoveDown, Qt::NoModifier));
    }
}

void 
SideBarTree::keyPressEvent(QKeyEvent *event) {
    //qDebug() << QString("SideBarTree::keyPressEvent with delta %1").arg(event->delta());
    switch (event->key()) {
			case Qt::Key_Up:
      case Qt::Key_Left:
      case Qt::Key_PageUp:
      case Qt::Key_Minus:
        setCurrentIndex(moveCursor(QAbstractItemView::MoveUp, Qt::NoModifier));
				return;
			case Qt::Key_Down:
      case Qt::Key_Right:
      case Qt::Key_PageDown:
			case Qt::Key_Plus:
        setCurrentIndex(moveCursor(QAbstractItemView::MoveDown, Qt::NoModifier));
				return;
			default:
				QTreeView::keyPressEvent(event); 
			}
}

void
        SideBarTree::currentChanged( const QModelIndex& current, const QModelIndex&)
{
    //qDebug() << QString("Emit selectionChanged %1").arg(current.row());
    emit selectionChanged(current);
}

void SideBarTree::setDriverStatus(int status)
{
    if (status != m_driverStatus) {
        m_driverStatus = status;
    }
}

void
        SideBarTree::createActions() {
    selectDriverAct = new QAction(tr("Select CAN interface"), this);
    selectDriverAct->setVisible(true);
    connect(selectDriverAct, SIGNAL(triggered()), this, SIGNAL(selectDriver()));

    renameFilteredLogAct = new QAction(tr("Rename"), this);
    renameFilteredLogAct->setVisible(true);
    connect(renameFilteredLogAct, SIGNAL(triggered()), this, SLOT(renameFilteredLog()));

    deleteFilteredLogAct = new QAction(tr("&Remove filtered log"), this);
    deleteFilteredLogAct->setVisible(true);
    connect(deleteFilteredLogAct, SIGNAL(triggered()), this, SLOT(deleteFilteredLog()));

    printAct = new QAction(tr("Print"), this);
    printAct->setVisible(true);
    connect(printAct, SIGNAL(triggered()), this, SIGNAL(print()));

    setFilterAct = new QAction(tr("Set filter"), this);
    setFilterAct->setVisible(true);
    connect(setFilterAct, SIGNAL(triggered()), this, SIGNAL(setFilter()));

}

void
        SideBarTree::createContextMenu() {
    addAction(selectDriverAct);
    addAction(renameFilteredLogAct);
    addAction(setFilterAct);
    addAction(printAct);
    QAction *act = new QAction(this);
    act->setSeparator(true);
    addAction(act);
    addAction(deleteFilteredLogAct);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void
        SideBarTree::deleteFilteredLog() {
    if (m_model->filteredListsCount()) {
        QModelIndex index = currentIndex();
        m_model->removeFilteredList(index);
    }
}

void
        SideBarTree::renameFilteredLog() {
    QModelIndex index = currentIndex();
    edit(index);
}

void
SideBarTree::renameFilteredLog(unsigned int row) {
	setCurrentIndex(model()->index(row, 1, rootIndex()));
	renameFilteredLog() ;
}
