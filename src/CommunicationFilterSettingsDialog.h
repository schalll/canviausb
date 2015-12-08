/***************************************************************************
*   Copyright (C) 2009 by
*   Laurent Schall
***************************************************************************/

#ifndef COMMUNICATIONFILTERSETTINGSDIALOG_H
#define COMMUNICATIONFILTERSETTINGSDIALOG_H

// QT includes
#include <QDialog>

// non QT includes

class CommunicationFilterSettingsDialog : public QDialog
{
   Q_OBJECT

public:
   CommunicationFilterSettingsDialog(QWidget *parent = 0);

signals:
	 void settingsChanged();

public slots:
	 void accept(); 
	 void reject(); 

private:

};

#endif
