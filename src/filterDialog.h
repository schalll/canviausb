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

#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>

#include "filterListWidget.h"
#include "ui_filterDialog.h"
#include "identifiers\IdentifiersModel.h"

class FilterDialog : public QDialog, private Ui::FilterDialog
{
    Q_OBJECT

public:
    FilterDialog(QWidget *parent = 0);
    void clear();
    void addNonFilteredIdentifer(QString description, identifierStruct id);
    void addFilteredIdentifer(QString description, identifierStruct id);
    QList<identifierStruct> getFilter();

public slots:
    void on_leftButton_clicked();
    void on_rightButton_clicked();

private:
    void moveCurrentItem(FilterListWidget *source,
                         FilterListWidget *target);
};

#endif
