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

//#define VIEW_FORMAT_COLUMN // to show format column

// QT includes
#include <QHeaderView>

// non QT includes
#include "filterActivationView.h"
#include "filterActivationModel.h"


FilterActivationView::FilterActivationView(CommunicationProxyModel* communicationModel, QWidget* parent ) :
        QTableView(parent)
{		
    // Set the model that has to be used for this view
	m_comModel = communicationModel;			// communication model (to retrieve the folter list)
	m_model = new FilterActivationModel(m_comModel, this);

    setModel(m_model);

    // Flat mode
    setFrameShape(QFrame::NoFrame);
    // Alternate row color
    setAlternatingRowColors(true);
    // No Grid
    setShowGrid(false);
    // Selecting on cell will make the entire row to be selected
    setSelectionBehavior(QAbstractItemView::SelectRows);
    // Hide the vertical header (we are only interested in the CAN Identifiers)
    verticalHeader()->hide();
    // The last visible section in the header takes up all the available space
    horizontalHeader()->setStretchLastSection(true);
    // List can be sorted
    setSortingEnabled(true);
    //connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this, SLOT(resize()));
    // set the first column width to a fixed value
    setColumnWidth(0,100);
    // set the third column width to a fixed value
    setColumnWidth(2,50);
	// set the row height
    int rowHeight = (int)(fontMetrics().height() * 1.4);
    verticalHeader()->setDefaultSectionSize(rowHeight);

    createActions();
    createContextMenu();

	connect(this, SIGNAL(clicked(const QModelIndex &)),this, SLOT(toogleActivation(const QModelIndex &)));
}

FilterActivationView::~FilterActivationView() {
    //delete m_webview;
}

void 
FilterActivationView::createActions() {
}

void
FilterActivationView::createContextMenu() {
}

void
FilterActivationView::toogleActivation(const QModelIndex &index) {
	identifierStruct tmpId;
	tmpId.identifier = m_model->data(index,Qt::UserRole).toUInt();
	tmpId.extended = m_model->data(index,Qt::UserRole+1).toBool();
	m_comModel->toggleExclusion(tmpId);
}
