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
#include <QModelIndex>
#include <QPainter>
#include <QDebug>

#include <QTextDocument>
#include <QTextCharFormat>
#include <QTextCursor>

// non QT includes
#include "CommunicationModel.h"
#include "CommunicationDelegate.h"

CommunicationDelegate::CommunicationDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
	settings = AppSettings::instance();
}

QSize
CommunicationDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& i ) const
{
	CommunicationModel* m_model = (CommunicationModel*)i.model(); 
	CommunicationData::CommunicationDataType type = m_model->type(i.row());
		if (type == CommunicationData::VAR_INFO_MESSAGE) {
            //int const w = QStyledItemDelegate::sizeHint( option, i ).width();
			return QStyledItemDelegate::sizeHint( option, i );
			//return QSize( w, 34 );
		} else {
			return QStyledItemDelegate::sizeHint( option, i );
		}
		
		/*
    int const w = QStyledItemDelegate::sizeHint( option, i ).width();

    using namespace SideBar;
    
    SideBarItem *item = getItem(i);
    int itemType = item->type();

    switch (itemType)
    {
    case Spacer1:
    case Spacer2:
    case Spacer3:
        return QSize( w, 24 );
    case device:
    case identifiers:
    case comLog:
    case filteredList:
    case traffic:
        return QSize( w, 20 );
    default:
        return QSize( w, 20 );
    }
		*/
}

#define GRAY_TEXT QColor( 120, 120, 120 )
#define X 13

void
CommunicationDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& i ) const
{
	CommunicationModel* m_model = (CommunicationModel*)i.model(); 
	CommunicationData::CommunicationDataType type = m_model->type(i.row());
	
	// Variable needed for the difference highlighting
	QString frame;
	QString previous_frame;
	int char_width;
	QRect highlight_rect;
	unsigned int absolute_left;
	unsigned int string_begin_left;
	unsigned int string_width;

    int col = i.column();
	
	if (type == CommunicationData::VAR_INFO_MESSAGE) {
		QFont f = option.font;
		f.setPixelSize( 11 );
		f.setBold( true );
		painter->save();
		painter->setFont( f );
		painter->setPen( GRAY_TEXT );
		QStyledItemDelegate::paint( painter, option, i );
		painter->restore();

	} else if (col == 3) {
		if (settings->displayDifferences()) {
			frame = m_model->data(i, Qt::DisplayRole).toString();
			previous_frame = m_model->data(i, Qt::UserRole+1).toString();

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);
			painter->setPen(Qt::NoPen);
			//painter->setBrush(QBrush(QColor("#FFFF00"), Qt::SolidPattern));
			painter->setBrush(QBrush(QColor("#FF663A"), Qt::Dense3Pattern));

			bool string_init_found = false;
			bool string_difference_found = false;

			absolute_left = 2;
			string_begin_left = 0;
			string_width = 0;
			
			// None of the frame are empty and frame are different - so we are interested in finding what is different
			if ((frame != "") && (previous_frame != "") && frame != previous_frame) {
				for (unsigned int j=0; j < frame.length(); j++) {
					char_width = painter->fontMetrics().width(frame.at(j));

					// Search for a string (i.e not blanks)
					if (frame.at(j) != QChar(' ')) {
						// Do we already have detected the first character of this string?
						if (!string_init_found) {
							string_init_found = true;
							string_begin_left = absolute_left;
						}
						// Is there any different between the current character of both strings
						if (frame.at(j) != previous_frame.at(j)) {
							string_difference_found = true;
						}
						string_width += char_width;
					} else {
						// This char is a blank - so do soemthing only if we already had a string AND a difference in this string
						if (string_init_found) {
							if (string_difference_found) {
								// Draw highlight here
								//highlight_rect = QRect(option.rect.left() + string_begin_left, option.rect.top(), string_width, option.rect.height());
								highlight_rect = QRect(option.rect.left() + string_begin_left, option.rect.top() + option.rect.height() - 2 , string_width + 1, 2);
								painter->drawRect(highlight_rect);
							}
						}
						string_init_found = false;
						string_difference_found = false;
						string_width = 0;
					}
					absolute_left += char_width;
				}
				// Last packet
				if (string_difference_found) {
					// Draw highlight here
					//highlight_rect = QRect(option.rect.left() + string_begin_left, option.rect.top(), string_width, option.rect.height());
					highlight_rect = QRect(option.rect.left() + string_begin_left, option.rect.top() + option.rect.height() - 2 , string_width + 1, 2);
					painter->drawRect(highlight_rect);
				}
			}
			painter->restore();
		}
		QStyledItemDelegate::paint( painter, option, i );

//	} else if (col == 4) {
//		if (settings->displayDifferences()) {
//			frame = m_model->data(i, Qt::DisplayRole).toString();
//			previous_frame = m_model->data(i, Qt::UserRole+1).toString();

//			painter->save();
//			painter->setRenderHint(QPainter::Antialiasing, true);
//			painter->setPen(Qt::NoPen);
//			//painter->setBrush(QBrush(QColor("#FFFF00"), Qt::SolidPattern));
//			painter->setBrush(QBrush(QColor("#FF663A"), Qt::Dense3Pattern));

//			absolute_left = 2;
//			string_begin_left = 0;
//			string_width = 0;
//			// None of the frame are empty and frame are different - so we are interested in finding what is different

//			if ((frame != "") && (previous_frame != "") && frame != previous_frame) {
//			//qDebug() << QString("Frame ascii = %1 - Previous frame ascii = %3").arg(frame).arg(previous_frame) << endl;

//				for (unsigned int j=0; j < frame.length(); j++) {
//					char_width = painter->fontMetrics().width(frame.at(j));
//					if (frame.at(j) != QChar(' ') && frame.at(j) != previous_frame.at(j)) {
//						// Draw highlight here
//						//highlight_rect = QRect(option.rect.left() + absolute_left, option.rect.top(), char_width, option.rect.height());
//						highlight_rect = QRect(option.rect.left() + absolute_left, option.rect.top() + option.rect.height() - 2 , char_width + 1, 2);
//						painter->drawRect(highlight_rect);
//					}
//					absolute_left += char_width;
//				}
//			}
//			painter->restore();
//		}
//		QStyledItemDelegate::paint( painter, option, i );
	} else {
		QStyledItemDelegate::paint( painter, option, i );
	}
		

	/*

#define LIGHT_BLUE_TEXT QColor( 130, 147, 174 )
#define GRAY_TEXT QColor( 120, 120, 120 )

#ifdef Q_WS_MAC
#define X 11
#else
#define X 13
#endif
    int y;

    using namespace SideBar;

    SideBarItem *item = getItem(i);
    int itemType = item->type();

    switch (itemType)
    {
        // Special treatment for Spacer 1
                case Spacer1:
        {
            y = option.rect.top() + 13;
            QFont f = option.font;
            f.setPixelSize( 11 );
            f.setBold( true );
            painter->save();
            painter->setFont( f );
            painter->setPen( GRAY_TEXT );
            painter->drawText( X, y + 5, tr("SETTINGS") );
            painter->restore();
            return;
        }

        // Special treatment for Spacer 2
                case Spacer2:
        {
            y = option.rect.top() + 13;
            QFont f = option.font;
            f.setPixelSize( 11 );
            f.setBold( true );
            painter->save();
            painter->setFont( f );
            painter->setPen( GRAY_TEXT );
            painter->drawText( X, y + 5, tr("CAN TRAFFIC") );
            painter->restore();
            return;
        }

        // Special treatment for Spacer 3
                case Spacer3:
        {
            y = option.rect.top() + 13;
            QFont f = option.font;
            f.setPixelSize( 11 );
            f.setBold( true );
            painter->save();
            painter->setFont( f );
            painter->setPen( GRAY_TEXT );
            painter->drawText( X, y + 5, tr("STATISTICS") );
            painter->restore();
            return;
        }
    }
*/
    
}
