/**
 * Copyright (c) 2009, Benjamin C. Meyer
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "searchlineedit.h"

#include "clearbutton.h"
#include "searchbutton.h"

#include <QMenu>
/*
SearchLineEdit*
SearchLineEdit::instance(QWidget* parent) {
	static SearchLineEdit* singleton = 0;
    if (singleton == 0) {
        singleton = new SearchLineEdit(parent);
    }
    return singleton;
}
*/
SearchLineEdit::SearchLineEdit(QWidget *parent)
    : LineEdit(parent)
{
    init();
}

void SearchLineEdit::init()
{
    // search button on the left
    m_searchButton = new SearchButton(this);
	m_searchButton->setShowMenuTriangle(true);
	connect(m_searchButton, SIGNAL(clicked()), this, SLOT(showSearchMenu()));
    addWidget(m_searchButton, LeftSide);

    // clear button on the right
    m_clearButton = new ClearButton(this);
    connect(m_clearButton, SIGNAL(clicked()), this, SLOT(clear()));
    connect(this, SIGNAL(textChanged(const QString&)), m_clearButton, SLOT(textChanged(const QString&)));
    addWidget(m_clearButton, RightSide);
    m_clearButton->hide();

    updateTextMargins();
    setInactiveText(tr("Search in data"));

	searchHighlightAction = new QAction(tr("&Highlight data sets"), this);
	searchHighlightAction->setCheckable(true);
    connect(searchHighlightAction, SIGNAL(triggered()), this, SLOT(highlight()));
	
	searchFilterAction = new QAction(tr("&Filter data sets"), this);
	searchFilterAction->setCheckable(true);
    connect(searchFilterAction, SIGNAL(triggered()), this, SLOT(filter()));

	// Put them in Action group with the hope that the menu will show a mark or a radio button
	searchGroup = new QActionGroup(this);
    searchGroup->addAction(searchHighlightAction);
    searchGroup->addAction(searchFilterAction);

	menu.addAction(searchHighlightAction);
	menu.addAction(searchFilterAction);
	searchHighlightAction->setChecked(true);
}

ClearButton *SearchLineEdit::clearButton() const
{
    return m_clearButton;
}

SearchButton *SearchLineEdit::searchButton() const
{
    return m_searchButton;
}

void 
SearchLineEdit::showSearchMenu() {

	QWidget *parent = parentWidget();
	if (!parent) return;

	//QPoint pos = parent->mapToGlobal(QPoint(0,parent->height()));
	QPoint pos = mapToGlobal(QPoint(0,height()));
	
	menu.exec(pos);	

}


void 
SearchLineEdit::highlight() {
	searchHighlightAction->setChecked(true);
	emit searchByHighlighting();
	emit textChanged(text());

}

void 
SearchLineEdit::filter() {
	searchFilterAction->setChecked(true);
	emit searchByFiltering();
	emit textChanged(text());
}
