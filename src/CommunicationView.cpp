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

// non QT includes
#include "CommunicationView.h"
#include "mainWindow.h"

CommunicationView::CommunicationView( QWidget* parent, bool useFilter) :
    PrintableTableView(parent)
{

    m_useFilter = useFilter;

    // Model
    m_model = CommunicationModel::instance(this);
    proxyModel = new CommunicationProxyModel(this, useFilter);
    proxyModel->setSourceModel(m_model);
    setModel(proxyModel);
    id_model = IdentifiersModel::instance(this);

    // Delegate
    m_delegate = new CommunicationDelegate(this);
    setItemDelegate(m_delegate);

    // Selection model
    m_selection = selectionModel();

    connect(m_model,SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(scrollToBottom( )) );

    // Flat mode
    setFrameShape(QFrame::NoFrame);
    // Use alternate row color
    setAlternatingRowColors(true);
    // No Grid
    setShowGrid(false);
    // Selecting on cell will make the entire row to be selected
    setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(m_selection,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),this,SLOT(computeStatistics(QItemSelection, QItemSelection)));
    // Hide the vertical header (we are only interested in the CAN Identifiers)
    verticalHeader()->hide();
    // The last visible section in the header takes up all the available space
    horizontalHeader()->setStretchLastSection(true);
    // List cannot be sorted
    setSortingEnabled(false);
    // set the columns width to a fixed value
    //setColumnWidth(0,20);		// Color
    setColumnWidth(0,80);		// Time
    setColumnWidth(1,200);		// Identifier
    setColumnWidth(2,30);		// DLC
    setColumnWidth(3,200);	// Data
    setColumnWidth(4,100);	// ascii
    setColumnWidth(5,100);	// Name

    // set the row height
    int rowHeight = (int)(fontMetrics().height() * 1);
    verticalHeader()->setDefaultSectionSize(rowHeight);

    createActions();
    createContextMenu();

    settings = AppSettings::instance();
    applyPreferences();

    //setUpdatesEnabled(false);				// No we don't want to do this as there is some kind of memory leak
    //m_timerId = startTimer(400);	  // appearing when the view gets updated even of the model in not chnging

    m_search_method = search_by_highlighting;
}

CommunicationView::~CommunicationView()
{
    delete proxyModel;
}

void 
CommunicationView::createActions() {
    highlightLogEntryAct = new QAction(tr("&Highlight same identifiers"), this);
    //highlightLogEntryAct->setIcon(QIcon(":/document_plain_new.png"));
    highlightLogEntryAct->setShortcut(tr("Ctrl+H"));
    highlightLogEntryAct->setStatusTip(tr("Highlight these identifiers in the communication log"));
    highlightLogEntryAct->setVisible(true);
    connect(highlightLogEntryAct, SIGNAL(triggered()), this, SLOT(highlightLogEntry()));

    deleteHighlightLogEntryAct = new QAction(tr("&Remove highlighting"), this);
    //deleteHighlightLogEntryAct->setIcon(QIcon(":/document_plain_new.png"));
    deleteHighlightLogEntryAct->setShortcut(tr("Ctrl+R"));
    deleteHighlightLogEntryAct->setStatusTip(tr("Remove the highlighting in the communication log"));
    deleteHighlightLogEntryAct->setVisible(true);
    connect(deleteHighlightLogEntryAct, SIGNAL(triggered()), this, SLOT(deleteHighlightLogEntry()));

    deleteLogEntryAct = new QAction(tr("&Remove from log"), this);
    //deleteLogEntryAct->setIcon(QIcon(":/document_plain_new.png"));
    deleteLogEntryAct->setShortcut(tr("Ctrl+D"));
    deleteLogEntryAct->setStatusTip(tr("Remove from the communication log"));
    deleteLogEntryAct->setVisible(true);
    connect(deleteLogEntryAct, SIGNAL(triggered()), this, SLOT(deleteLogEntry()));

    createFilteredListFromSelectionAct = new QAction(tr("Filtered log from selection"), this);
    createFilteredListFromSelectionAct->setStatusTip(tr("Create filtered log from selection"));
    createFilteredListFromSelectionAct->setVisible(true);
    connect(createFilteredListFromSelectionAct, SIGNAL(triggered()), this, SLOT(createFilteredListFromSelection()));
}

void
CommunicationView::createContextMenu() {
    addAction(highlightLogEntryAct);
    addAction(deleteHighlightLogEntryAct);
    //Add separator here
    QAction *act1 = new QAction(this);
    act1->setSeparator(true);
    addAction(act1);
    addAction(deleteLogEntryAct);
    //Add separator here
    QAction *act2 = new QAction(this);
    act2->setSeparator(true);
    addAction(act2);
    addAction(createFilteredListFromSelectionAct);
    setContextMenuPolicy(Qt::ActionsContextMenu);
}

void 
CommunicationView::deleteLogEntry() {
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
}

void 
CommunicationView::highlightLogEntry() {
    m_model->clearHighlightList();

    foreach (QModelIndex index, selectedIndexes()) {
        if (index.column() == 1) {
            m_model->addHighlight(model()->data(index, Qt::EditRole).toUInt());
        }
    }
}

void 
CommunicationView::deleteHighlightLogEntry() {
    m_model->clearHighlightList();
}

void 
CommunicationView::createFilteredListFromSelection() {
    
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
CommunicationView::print() {
    qDebug() << QString("CommunicationView::print test");

    // Find out what can be printed and what should be printed
    bool hasSelection;
    hasSelection = m_selection->hasSelection();
    if (hasSelection) selectionList = m_selection->selectedRows();
    textprinter_->preparePrint("Printing identifiers", hasSelection);
    printOnlySelection = textprinter_->isPrintSelection();

    MainWindow *mainWindow = MainWindow::instance();

    QString selectionText;
    if (printOnlySelection) {
        selectionText = QString("(Extract)");
    } else {
        selectionText = QString("");
    }

    QString headertext;
    QString html;

    if (!m_useFilter) {
        headertext = QString(
                    "<table width=\"100%\">"
                    "  <tr>"
                    "    <td align=\"left\"><h1>Communication log %1</h1></td>"
                    "  </tr>"
                    "  <tr>"
                    "    <td align=\"left\"><strong>%2</strong></td>"
                    "    <td align=\"right\"><strong>&date;</strong></td>"
                    "  </tr>"
                    "</table>");
        // Content
        html = htmlTable();
    } else {
        headertext = QString(
                    "<table width=\"100%\">"
                    "  <tr>"
                    "    <td align=\"left\"><h1>Filtered communication log %1</h1></td>"
                    "  </tr>"
                    "  <tr>"
                    "    <td align=\"left\"><strong>%2</strong></td>"
                    "    <td align=\"right\"><strong>&date;</strong></td>"
                    "  </tr>"
                    "</table>");
        // Content
        html = htmlFilter() + htmlTable();
    }

    m_htmlTemplate = readFile(QLatin1String(":/identifiers.html"));
    m_htmlTemplate.replace(QLatin1String("<!-- CONTENT -->"), html);
    m_baseUrl = QUrl(QLatin1String("qrc:/identifiers.html"));

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
}


QString CommunicationView::htmlTable() {
    QModelIndex index;
    QString html;
    QString tableBegin;
    QString tableEnd;
    QString color;
    QString textColor;
    QString line;

    // Table
    // TABLE BEGIN
    tableBegin = QString("\n <div>");
    tableBegin = tableBegin + QString("\n<table width=100% border=0 cellpadding=2 cellspacing=1>");
    tableBegin = tableBegin + QString("\n<tr>");
    if (settings->displayPCTimestamp()) tableBegin = tableBegin + QString("<th width=5%>") + model()->headerData(0,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>"); //Time
    tableBegin = tableBegin + QString("<th>Color</th>"); // Color
    tableBegin = tableBegin + QString("<th>") + model()->headerData(1,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>"); //Identifier
    tableBegin = tableBegin + QString("<th width=5%>") + model()->headerData(2,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>"); //DLC
    tableBegin = tableBegin + QString("<th>") + model()->headerData(3,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>"); //Data
    if (settings->displayMessageAscii()) tableBegin = tableBegin + QString("<th>") + model()->headerData(4,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>"); //Ascii
    if (settings->displayMessageName())  tableBegin = tableBegin + QString("<th>") + model()->headerData(5,Qt::Horizontal,Qt::DisplayRole).toString() + QString("</th>"); //Name
    tableBegin = tableBegin + QString("</tr>");
    html = html + tableBegin;

    // LINES
    bool printThisRow;
    unsigned int printedRowCount = 0;
    const unsigned int rows = model()->rowCount();
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

            // Get the foregorund color
            index = model()->index(row, 0, QModelIndex());
            textColor = model()->data(index,Qt::ForegroundRole).toString();
            if (printedRowCount++ % 2) {
                line = QString("\n<tr bgcolor = #eeeeee style=color:") + textColor + QString(">");
            } else {
                line = QString("\n<tr bgcolor = #fcfcfc style=color:") + textColor + QString(">");
            }

            //Time
            if (settings->displayPCTimestamp()) {
                index = model()->index(row, 0, QModelIndex());
                line = line + QString("<td align=center>");
                line = line + model()->data(index,Qt::DisplayRole).toString();
                line = line + QString("</td>");
            }

            //Color
            index = model()->index(row, 1, QModelIndex());
            color = model()->data(index,Qt::UserRole+2).toString();
            if (color != QString("#ffffff")) {
                line = line + QString("<td width=5% align=center style=background-color:") + color + QString(">     </td>");
            } else {
                line = line + QString("<td width=5% align=center>     </td>");
            }

            //ID
            index = model()->index(row, 1, QModelIndex());
            line = line + QString("\n<td>");
            line = line + model()->data(index,Qt::DisplayRole).toString();
            line = line + QString("</td>");

            //DLC
            index = model()->index(row, 2, QModelIndex());
            line = line + QString("\n<td align=center>");
            line = line + model()->data(index,Qt::DisplayRole).toString();
            line = line + QString("</td>");

            //Datas
            index = model()->index(row, 3, QModelIndex());
            line = line + QString("\n<td>");
            line = line + model()->data(index,Qt::DisplayRole).toString();
            line = line + QString("</td>");

            //Ascii
            if (settings->displayMessageAscii()) {
                index = model()->index(row, 4, QModelIndex());
                line = line + QString("<td>");
                line = line + model()->data(index,Qt::DisplayRole).toString();
                line = line + QString("</td>");
            }

            //Name
            if (settings->displayMessageName()) {
                index = model()->index(row, 5, QModelIndex());
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

    return html;
}

QString
CommunicationView::htmlFilter() {
    QModelIndex index;
    QString html;
    QString tableBegin;
    QString tableEnd;
    QString color;
    QString textColor;
    QString line;

    // Table
    // TABLE BEGIN
    tableBegin = QString("\n <div>");
    tableBegin = tableBegin + QString("\n<h3>Filter content:</h3>");
    tableBegin = tableBegin + QString("\n<table width=70% border=0 cellpadding=2 cellspacing=1>");
    tableBegin = tableBegin + QString("\n<tr>");
    tableBegin = tableBegin + QString("<th>Identifier</th>");	// Identifier
    tableBegin = tableBegin + QString("<th>Color</th>");		// Color
    if (settings->displayMessageName()) tableBegin = tableBegin + QString("<th>Name</th>"); //Name
    tableBegin = tableBegin + QString("</tr>");
    html = html + tableBegin;

    // LINES
    unsigned int printedRowCount = 0;
    QList<identifierStruct> filter = getFilter();
    QListIterator<identifierStruct> i(filter);
    identifierStruct id;
    while (i.hasNext()) {
        id = i.next();

        // Get the foregorund color
        if (printedRowCount++ % 2) {
            line = QString("\n<tr bgcolor = #eeeeee>");
        } else {
            line = QString("\n<tr bgcolor = #fcfcfc>");
        }

        //ID
        line = line + QString("\n<td width=35%>");
        QString infoLabel;
        if (!id.extended) {
            infoLabel = QString("(STD)");
        } else {
            infoLabel = QString("(EXT)");
        }
        switch(settings->displayIDFormat()) {
        case 0:
            line = line +  QString("%1 %2 ").arg(infoLabel).arg(id.identifier);
            break;
        case 1:
            line = line +  QString("%1 0x%2 ").arg(infoLabel).arg(id.identifier, 8, 16, QLatin1Char('0'));
            break;
        case 2:
        default:
            line = line + QString("%1 0x%2 / %3 ").arg(infoLabel).arg(id.identifier, 8, 16, QLatin1Char('0')).arg(id.identifier);
        }
        line = line + QString("</td>");


        //Color
        color = QVariant(id_model->getIdColor(id.identifier,id.extended)).toString();
        if (color != QString("#ffffff")) {
            line = line + QString("<td width=5% align=center style=background-color:") + color + QString(">     </td>");
        } else {
            line = line + QString("<td width=5% align=center>     </td>");
        }

        //Name
        if (settings->displayMessageName()) {
            line = line + QString("<td>");
            line = line + id_model->getIdName(id.identifier,id.extended);
            line = line + QString("</td>");
        }


        line = line + QString("</tr>");

        html = html + line;
    }

    // TABLE END
    tableEnd = QString("\n</table>");
    tableEnd = tableEnd + QString("\n</div><p>   </p>");
    html = html + tableEnd;

    return html;
}

void 
CommunicationView::applyPreferences() {
    if (settings->displayPCTimestamp())
        showColumn(0);
    else
        hideColumn(0);

    if (settings->displayMessageAscii())
        showColumn(4);
    else
        hideColumn(4);

    if (settings->displayMessageName()) {
        showColumn(5);
        setColumnWidth(4,100);	// ascii
    } else
        hideColumn(5);

}

void 
CommunicationView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    //qDebug() <<  QString("rowsInserted %1 %2").arg(start).arg(end);
    QModelIndex index;

    for (int i=start; i<=end; i++) {
        index = model()->index(i, 0, parent);
        QVariant type = model()->data(index,Qt::UserRole);
        if (
                (type.toUInt() == CommunicationData::VAR_INFO_MESSAGE)
                ||
                (type.toUInt() == CommunicationData::VAR_ERROR_MESSAGE)
                ) {
            //					setSpan(i,1,1,5);
        }
    }

    PrintableTableView::rowsInserted(parent, start, end);
}

QList<identifierStruct> 
CommunicationView::getFilter() {
    return proxyModel->getFilter();
}

void 
CommunicationView::setFilter(QList<identifierStruct> filter) {
    proxyModel->setFilter(filter);
}

void CommunicationView::redraw() {
    proxyModel->invalidate();
}

void 
CommunicationView::setSearchString(const QString &searchStr) {
    m_searchString = searchStr;
    if (m_search_method == search_by_filtering) {
        proxyModel->setFilterString(searchStr);
        m_model->setHighlightDataString(QString(""));
    } else {
        proxyModel->setFilterString(QString(""));
        m_model->setHighlightDataString(searchStr);
    }
}

void 
CommunicationView::setSearchByFiltering() {
    m_search_method = search_by_filtering;
    proxyModel->invalidate();
}

void 
CommunicationView::setSearchByHighlighting() {
    m_search_method = search_by_highlighting;
    proxyModel->invalidate();
}

void 
CommunicationView::computeStatistics(QItemSelection selected, QItemSelection deselected)
{
    if (selected.indexes().size() == 0) return;
    
}
