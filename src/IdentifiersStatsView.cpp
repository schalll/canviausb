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
#include <QTableWidgetSelectionRange>

// non QT includes
#include "IdentifiersStatsView.h"
#include "IdentifiersStatsModel.h"
#include "mainWindow.h"

IdentifiersStatsView::IdentifiersStatsView(QWidget* parent ) :
        PrintableTableView(parent)
{		
    // Set the model that has to be used for this view
    m_model = IdentifiersStatsModel::instance(this);

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
    //connect(m_model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),this, SLOT(resizeRowsToContents()));
    // set the columns width to a fixed value
    setColumnWidth(0,80);		// Last seen zime 
	setColumnWidth(1,200);	// Identifier
    setColumnWidth(2,50);		// Counts
    setColumnWidth(3,30);		// DLC
    setColumnWidth(4,200);	// Data
    setColumnWidth(5,100);	// ascii
    setColumnWidth(6,100);	// Name

    // set the row height
    int rowHeight = (int)(fontMetrics().height() * 1);
    verticalHeader()->setDefaultSectionSize(rowHeight);

    createActions();
    createContextMenu();

	settings = AppSettings::instance();
	applyPreferences();
}

IdentifiersStatsView::~IdentifiersStatsView() {
    //delete m_webview;
}

void 
IdentifiersStatsView::createActions() {
	createFilteredListFromSelectionAct = new QAction(tr("Filtered log from selection"), this);
    createFilteredListFromSelectionAct->setStatusTip(tr("Create filtered log from selection"));
    createFilteredListFromSelectionAct->setVisible(true);
    connect(createFilteredListFromSelectionAct, SIGNAL(triggered()), this, SLOT(createFilteredListFromSelection()));
}

void
IdentifiersStatsView::createContextMenu() {
    addAction(createFilteredListFromSelectionAct);
	setContextMenuPolicy(Qt::ActionsContextMenu);
}


void 
IdentifiersStatsView::createFilteredListFromSelection() {
    
	QList<identifierStruct> filter;
	identifierStruct tmp_identifier;
	
	foreach (QModelIndex index, selectedIndexes()) {
        if (index.column() == 1) {
            //qDebug() << QString("add following identifier to filtered list %1 %2").arg(model()->data(index, Qt::EditRole).toUInt()).arg(model()->data(index, Qt::UserRole+1).toUInt());
			tmp_identifier.identifier = model()->data(index, Qt::EditRole).toUInt();
			tmp_identifier.extended = model()->data(index, Qt::UserRole+1).toBool();
			// Do not append to the filter if it is an empty id (i.e. 0)
			if (tmp_identifier.identifier) {
				filter.append(tmp_identifier);
			}
        }
    }
	QString tmp_name = QString(tr("Filtered log from selection"));
	MainWindow *mainWindow = MainWindow::instance();
	mainWindow->addFilteredListFromSelection(tmp_name, filter);
}

void 
        IdentifiersStatsView::print() {

    QModelIndex index;
    QString color;

	// Find out what can be printed and what should be printed
	bool hasSelection;
	bool printOnlySelection;

	QModelIndexList selectionList;
	QItemSelectionModel *selection = selectionModel();	// Get the selection model
	hasSelection = selection->hasSelection();
	if (hasSelection) selectionList = selection->selectedRows();
	textprinter_->preparePrint("Printing identifiers", hasSelection);
	printOnlySelection = textprinter_->isPrintSelection();

	QString selectionText;
	if (printOnlySelection) { 
		selectionText = QString("(Extract)");
	} else {
		selectionText = QString("");
	}

	// Prepare the document that has to be printed
    QString html;

    QString tableBegin;
    QString tableEnd;
    QString line;

    MainWindow *mainWindow = MainWindow::instance();

    // Table
    // TABLE BEGIN
    tableBegin = QString("\n <div>");
    tableBegin = tableBegin + QString("\n<table width=100% border=0 cellpadding=2 cellspacing=1>");
    tableBegin = tableBegin + QString("\n<tr>");
    tableBegin = tableBegin + QString("<th>") + model()->headerData(0,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
	tableBegin = tableBegin + QString("<th>Color</th>");
    tableBegin = tableBegin + QString("<th>") + model()->headerData(1,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
    tableBegin = tableBegin + QString("<th>") + model()->headerData(2,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
    tableBegin = tableBegin + QString("<th>") + model()->headerData(3,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
    tableBegin = tableBegin + QString("<th>") + model()->headerData(4,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
    if (settings->displayMessageAscii()) tableBegin = tableBegin + QString("<th>") + model()->headerData(5,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
    if (settings->displayMessageName())  tableBegin = tableBegin + QString("<th>") + model()->headerData(6,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
	tableBegin = tableBegin + QString("</tr>");
    html = tableBegin;

    // LINES
	bool printThisRow;
	unsigned int printedRowCount = 0;
    const int rows = model()->rowCount();

    for (unsigned int row=0; row<rows; row++) {

		if (printOnlySelection) {
			// verify if this row should be printed
			index = model()->index(row, 0, QModelIndex());
			if (selectionList.contains(index)) {
				printThisRow = true;
			} else {
				printThisRow = false;
			}
		} else {
			printThisRow = true;
		}

		if (printThisRow) {
			if (printedRowCount++ % 2) {
				line = QString("\n<tr bgcolor = #eeeeee >");
			} else {
				line = QString("\n<tr bgcolor = #fcfcfc >");
			}

			index = model()->index(row, 0, QModelIndex());
			line = line + QString("<td>");
			line = line + model()->data(index,Qt::DisplayRole).toString();
			line = line + QString("</td>");

			index = model()->index(row, 1, QModelIndex());
			color = model()->data(index,Qt::UserRole+2).toString();
			if (color != QString("#ffffff")) {
				line = line + QString("<td width=5% align=center style=background-color:") + color + QString(">     </td>");
			} else {
				line = line + QString("<td width=5% align=center>     </td>");
			}

			index = model()->index(row, 1, QModelIndex());
			line = line + QString("<td>");
			line = line + model()->data(index,Qt::DisplayRole).toString();
			line = line + QString("</td>");

			index = model()->index(row, 2, QModelIndex());
			line = line + QString("\n<td align=center>");
			line = line + model()->data(index,Qt::DisplayRole).toString();
			line = line + QString("</td>");

			index = model()->index(row, 3, QModelIndex());
			line = line + QString("<td align=center>");
			line = line + model()->data(index,Qt::DisplayRole).toString();
			line = line + QString("</td>");

			index = model()->index(row, 4, QModelIndex());
			line = line + QString("<td>");
			line = line + model()->data(index,Qt::DisplayRole).toString();
			line = line + QString("</td>");

			if (settings->displayMessageAscii()) {
				index = model()->index(row, 5, QModelIndex());
				line = line + QString("<td>");
				line = line + model()->data(index,Qt::DisplayRole).toString();
				line = line + QString("</td>");
			}

			if (settings->displayMessageName()) {
				index = model()->index(row, 6, QModelIndex());
				line = line + QString("<td>");
				line = line + model()->data(index,Qt::DisplayRole).toString();
				line = line + QString("</td>");
			}


			line = line + QString("</tr>");
	        
			html = html + line;
		}
    }

    // TABLE END
    tableEnd = QString("\n</table>");
    tableEnd = tableEnd + QString("\n</div>");
    html = html + tableEnd;

    m_htmlTemplate = readFile(QLatin1String(":/identifiers.html"));
    m_htmlTemplate.replace(QLatin1String("<!-- CONTENT -->"), html);
    m_baseUrl = QUrl(QLatin1String("qrc:/identifiers.html"));

#ifdef USEWEBKIT
    connect(m_webview, SIGNAL(loadFinished(bool)),this, SLOT(printHtml(bool)));
    m_webview->setHtml(m_htmlTemplate, m_baseUrl);
#else

	QString headertext = 
		"<table width=\"100%\">"
		"  <tr>"
		"    <td align=\"left\"><h1>Identifiers occurences %1</h1></td>"
		"  </tr>"
		"  <tr>"
		"    <td align=\"left\"><strong>%2</strong></td>"
		"    <td align=\"right\"><strong>&date;</strong></td>"
		"  </tr>"
		"</table>";

	QString footertext = 
		"<table width=\"100%\">"
		"  <tr>"
		"    <td align=\"left\"><strong>CANviaUSB</strong></td>"
		"    <td align=\"right\"><strong>&page;</strong></td>"
		"  </tr>"
		"</table>";

    textprinter_->setHeaderText(headertext.arg(selectionText).arg(mainWindow->currentProjectName()));
    textprinter_->setFooterText(footertext);
    QTextDocument *document = new QTextDocument(this);
	document->setHtml(m_htmlTemplate);
	textprinter_->print(document);
#endif
}

void 
        IdentifiersStatsView::applyPreferences() {

    if (settings->displayMessageAscii())
        showColumn(5);
    else
        hideColumn(5);

    if (settings->displayMessageName()) {
        showColumn(6);
        setColumnWidth(4,100);	// ascii
    } else
        hideColumn(6);

}
