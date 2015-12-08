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
#include <QTableWidgetSelectionRange>
#include <QPainter>

// non QT includes
#include "IdentifiersView.h"
#include "IdentifiersModel.h"
#include "../mainWindow.h"



IdentifiersView::IdentifiersView(QWidget* parent ) :
        PrintableTableView(parent)
{		
    // Set the model that has to be used for this view
    m_model = IdentifiersModel::instance(this);
    m_delegate = new IdentifiersDelegate(this);

    setModel(m_model);
    setItemDelegate(m_delegate);

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
    // set the first column width to a fixed value
    setColumnWidth(0,150);
    // set the third column width to a fixed value
    setColumnWidth(2,150);
	// set the row height
    int rowHeight = (int)(fontMetrics().height() * 1.4);
    verticalHeader()->setDefaultSectionSize(rowHeight);

#ifdef VIEW_FORMAT_COLUMN
	showColumn(2);
#else
	hideColumn(2);
#endif

    createActions();
    createContextMenu();
}

IdentifiersView::~IdentifiersView() {
    //delete m_webview;
}

void 
        IdentifiersView::createActions() {
    // File menu actions
    deleteIdentifierAct = new QAction(tr("&Remove from list"), this);
    //deleteIdentifierAct->setIcon(QIcon(":/document_plain_new.png"));
    deleteIdentifierAct->setShortcut(tr("Ctrl+D"));
    deleteIdentifierAct->setStatusTip(tr("Remove from the list"));
    deleteIdentifierAct->setVisible(true);
    connect(deleteIdentifierAct, SIGNAL(triggered()), this, SLOT(deleteIdentifier()));

	createFilteredListFromSelectionAct = new QAction(tr("Filtered log from selection"), this);
    createFilteredListFromSelectionAct->setStatusTip(tr("Create filtered log from selection"));
    createFilteredListFromSelectionAct->setVisible(true);
    connect(createFilteredListFromSelectionAct, SIGNAL(triggered()), this, SLOT(createFilteredListFromSelection()));

}

void
        IdentifiersView::createContextMenu() {
    addAction(deleteIdentifierAct);
    //Add separator here
    QAction *act2 = new QAction(this);
    act2->setSeparator(true);
    addAction(act2);
    addAction(createFilteredListFromSelectionAct);

	setContextMenuPolicy(Qt::ActionsContextMenu);
}

void 
        IdentifiersView::deleteIdentifier() {
    // FROM http://www.qtcentre.org/forum/f-qt-programming-2/t-get-list-of-selected-rows-from-qtableview-739.html/?highlight=selectedIndexes

    QMap<int, int> rows;
    foreach (QModelIndex index, selectedIndexes())
        rows.insert(index.row(), 0);
    QMapIterator<int, int> r(rows);
    r.toBack();
    while (r.hasPrevious()) {
        r.previous();
        model()->removeRow(r.key());
    } // while
} // removeSelected

void 
IdentifiersView::createFilteredListFromSelection() {
    
	QList<identifierStruct> filter;
	identifierStruct tmp_identifier;
	
	foreach (QModelIndex index, selectedIndexes()) {
        if (index.column() == 0) {
            //qDebug() << QString("add following identifier to filtered list %1 %2").arg(model()->data(index, Qt::EditRole).toUInt()).arg(model()->data(index, Qt::UserRole+1).toUInt());
			tmp_identifier.identifier = model()->data(index, Qt::EditRole).toUInt();
			tmp_identifier.extended = model()->data(index, Qt::UserRole).toBool();
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
IdentifiersView::print() {

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
    tableBegin = tableBegin + QString("<th>") + model()->headerData(1,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
#ifdef VIEW_FORMAT_COLUMN
	tableBegin = tableBegin + QString("<th>") + model()->headerData(2,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
#endif
	tableBegin = tableBegin + QString("<th>") + model()->headerData(3,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>");
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
			line = line + QString("<td width=25%>");
			line = line + model()->data(index,Qt::DisplayRole).toString();
			line = line + QString("</td>");

			index = model()->index(row, 1, QModelIndex());
			color = model()->data(index,Qt::DisplayRole).toString();
			if (color != QString("#ffffff")) {
				line = line + QString("<td width=10% align=center style=background-color:") + color + QString(">     </td>");
			} else {
				line = line + QString("<td width=10% align=center>     </td>");
			}

#ifdef VIEW_FORMAT_COLUMN
			index = model()->index(row, 2, QModelIndex());
			line = line + QString("\n<td width=20%>");
			line = line + model()->data(index,Qt::DisplayRole).toString();
			line = line + QString("</td>");
#endif

			index = model()->index(row, 3, QModelIndex());
			line = line + QString("\n<td>");
			line = line + model()->data(index,Qt::DisplayRole).toString();
			line = line + QString("</td>");

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
		"    <td align=\"left\"><h1>Identifiers %1</h1></td>"
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

void IdentifiersView::printTable(QPrinter* printer, QPainter* painter, const QRect& area)
{
        const int rows = model()->rowCount();
        const int columns = model()->columnCount();
	double totalWidth = 0.0;
	double totalPageHeight = 0.0;
	double totalHeight = 0.0;
	for (int c = 0; c < columns; ++c)
	{
		totalWidth += columnWidth(c);
	}
 
	for (int p = 0; p < 45; ++p)
	{
		totalPageHeight += rowHeight(p);
	}
 
	for (int r = 0; r<rows; ++r)
	{
		totalHeight += rowHeight(r);
	}
 
	const double xscale = area.width() / totalWidth;
        const double yscale = area.height() / totalHeight; 
	const double pscale = area.height() / totalPageHeight;
 
	//painter->scale(xscale, yscale);
	qDebug() << QString("Hopla = %1 %2").arg(xscale).arg(yscale);
	painter->scale(10, 10);
	painter->translate(area.x() + xscale, area.y() + pscale);
 
	bool ok;
	int x=0;
 
    QStyleOptionViewItem option;
   
    for (int r = 0; r < rows; ++r) {
        ++x;
        for (int c = 0; c < columns; ++c) {
            QModelIndex idx = model()->index(r,c);
            option = viewOptions();
            option.rect = visualRect(idx);
            itemDelegate()->paint(painter, option, idx);
        }

        if (x == 45)
        {
            ok = printer->newPage();
            x=0;
            painter->translate(0, -1350);
        }
    } 
}

void IdentifiersView::printDocument(QPrinter *printer)
{
    qDebug() << QString("printer->paperRect() = %1 %2 %3 %4").arg(printer->paperRect().left()).arg(printer->paperRect().top()).arg( printer->paperRect().width()).arg(printer->paperRect().height());
    qDebug() << QString("printer->pageRect() = %1 %2 %3 %4").arg(printer->pageRect().left()).arg(printer->pageRect().top()).arg( printer->pageRect().width()).arg(printer->pageRect().height());
	 
	MainWindow *mainWindow = MainWindow::instance();

	QRect pageRect = printer->pageRect(); // pageRect is the area of the paper used for printing that can be painted on.
	 
	QString headerTitleText = QString("Identifiers");
	QString headerLine1Text = QString("Project: %1").arg(mainWindow->currentProjectName());
	QString headerLine2Text = QString("Printed on %1").arg(QDate::currentDate().toString());

	QPainter printPainter(printer);
	printPainter.setPen(Qt::black);


    QRect headerRect = pageRect;
	QRect footerRect = pageRect;
	footerRect.translate(0, pageRect.height() - 1000);


	// Header
    printPainter.setFont(QFont("Arial", 14));
	printPainter.drawText(headerRect,  Qt::AlignLeft | Qt::AlignTop, headerTitleText);
    headerRect.translate(0,150);
	printPainter.setFont(QFont("Arial", 10));
	printPainter.drawText(headerRect, Qt::AlignLeft | Qt::AlignTop, headerLine1Text);
	headerRect.translate(0,100);
	printPainter.drawText(headerRect, Qt::AlignLeft | Qt::AlignTop, headerLine2Text);
	
	// Footer
    printPainter.setFont(QFont("Arial", 14));
	printPainter.drawText(footerRect,  Qt::AlignLeft | Qt::AlignTop, "123");
    footerRect.translate(0,150);
	printPainter.setFont(QFont("Arial", 10));
	printPainter.drawText(footerRect, Qt::AlignLeft | Qt::AlignTop, "456");
	footerRect.translate(0,100);
	printPainter.drawText(footerRect, Qt::AlignLeft | Qt::AlignTop, "789");	


//    printPainter.setPen(Qt::black);
//    printPainter.setFont(QFont("Arial", 14));
//	QRect headerTitle = printPainter.boundingRect(pageRect, Qt::AlignLeft | Qt::AlignTop, headerTitleText);
//	printPainter.drawText(pageRect, Qt::AlignLeft | Qt::AlignTop, headerTitleText);
//
//	printPainter.setFont(QFont("Arial", 10));
//	pageRect.setTop(pageRect.top() + 17);// adding  a spacing 
//	QRect headerLine1 = printPainter.boundingRect(pageRect, Qt::AlignLeft | Qt::AlignTop, headerLine1Text);
//	printPainter.drawText(pageRect, Qt::AlignLeft | Qt::AlignTop, headerLine1Text);
//
//	QRect headerLine2 = printPainter.boundingRect(pageRect, Qt::AlignLeft | Qt::AlignTop, headerLine2Text);



	
//	//QTextDocument *document = editor->document();
//    //Header & Footer .... just building the strings...
// 
//    QPainter printPainter(printer);
//    //printPainter.setFont(f); //<- f: is the font used as standard in the QTextEdit
//    QRect pageRect = printer->pageRect();
//    QRect headerRect = printPainter.boundingRect(printPainter.window()/*pageRect*/,Qt::AlignLeft |Qt::AlignTop, printHeader);
//    QRect footerRect = printPainter.boundingRect(printPainter.window()/*pageRect*/,Qt::AlignBottom |Qt::AlignHCenter, footer);
//    QRect numbersRect = printPainter.boundingRect(printPainter.window()/*pageRect*/,Qt::AlignLeft,
//                                               QString::number(document->blockCount()));
//    pageRect.setTop(headerRect.height() +17);// adding  a spacing 
//    pageRect.setLeft(numbersRect.width() +8);
//    pageRect.setBottom(footerRect.top() -10);
//    pageRect.setRight(/*pageRect.left() + printer->pageRect().width()*/ printPainter.window().right());
//    document->setPageSize(pageRect.size());
 
//    QAbstractTextDocumentLayout *lay = document->documentLayout();
//    lay->setPaintDevice(printer);
//    int pages = lay->pageCount();
// 
//    QTextBlock block = document->begin();
//    int pageNumber = 0;
//    int yOffset = 0;
//    bool newPage = true;
//    printPainter.setClipping(true); 
}
