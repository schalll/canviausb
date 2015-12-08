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

#ifndef TOOLFRAME_H
#define TOOLFRAME_H

// QT includes
#include <QFrame>

// non QT includes
class QLabel;
class QToolBar;
class SearchLineEdit;


class ToolFrame : public QFrame
{
    Q_OBJECT

public:
    ToolFrame( QWidget* parent = 0);
    ~ToolFrame();

	void makeSearchBarVisible(bool visible);
	void emptySearchBar();

	void addAction(QAction *act);
	void addSeparator();

protected:
    void paintEvent(QPaintEvent *);


signals:
    void searchStringChanged(const QString &searchStr);
    void searchByFiltering();
	void searchByHighlighting();


private:
	QToolBar *m_toolbar;
	SearchLineEdit *m_search;
	QLabel *m_info;
};


#endif
