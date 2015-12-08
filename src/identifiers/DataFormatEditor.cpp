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

// non QT includes
#include "DataFormatEditor.h"

DataFormatEditor::DataFormatEditor(QWidget *widget) : QComboBox(widget)
{
    populateList();
}

unsigned int DataFormatEditor::format() const
{
	return itemData(currentIndex(), Qt::UserRole).toUInt();
}

void DataFormatEditor::setFormat(unsigned int f)
{
    setCurrentIndex(findData(f, int(Qt::UserRole)));
}

void DataFormatEditor::populateList()
{
    QList<unsigned int> formatList;

    formatList << 0x00000000;	//[8][8][8][8][8][8][8][8]
	formatList << 0x10101010;	//[16][16][16][16]
	formatList << 0x20002000;	//[32][32]

    formatList << 0x00000010;	//[8][8][8][8][8][8][16]
    formatList << 0x00001010;	//[8][8][8][8][16][16]
    formatList << 0x00101010;	//[8][8][16][16][16]
    formatList << 0x10000000;	//[16][8][8][8][8][8][8]
    formatList << 0x10100000;	//[16][16][8][8][8][8]
    formatList << 0x10101000;	//[16][16][16][8][8]
	formatList << 0x00002000;	//[8][8][8][8][32]
	formatList << 0x00102000;	//[8][8][16][32]
	formatList << 0x20000000;	//[32][8][8][8][8]
	formatList << 0x20001000;	//[32][16][8][8]
	formatList << 0x10102000;	//[16][16][32]
	formatList << 0x10200010;	//[16][32][16]
	formatList << 0x20001010;	//[32][16][16]
	formatList << 0xf0000000;	//[b][8][8][8][8][8][8][8]
	formatList << 0xff000000;	//[b][b][8][8][8][8][8][8]
	formatList << 0xfff00000;	//[b][b][b][8][8][8][8][8]
	formatList << 0xffff0000;	//[b][b][b][b][8][8][8][8]
	formatList << 0xfffff000;	//[b][b][b][b][b][8][8][8]
	formatList << 0xffffff00;	//[b][b][b][b][b][b][8][8]
	formatList << 0xfffffff0;	//[b][b][b][b][b][b][b][8]
	formatList << 0xffffffff;	//[b][b][b][b][b][b][b][b]
	formatList << 0x0000000f;	//[8][8][8][8][8][8][8][b}
	formatList << 0x000000ff;	//[8][8][8][8][8][8][b][b]
	formatList << 0x00000fff;	//[8][8][8][8][8][b][b][b]
	formatList << 0x0000ffff;	//[8][8][8][8][b][b][b][b]
	formatList << 0x000fffff;	//[8][8][8][b][b][b][b][b]
	formatList << 0x00ffffff;	//[8][8][b][b][b][b][b][b]
	formatList << 0x0fffffff;	//[8][b][b][b][b][b][b][b]
    for (int i = 0; i < formatList.size(); ++i) {
        insertItem(i, formatAsString(formatList[i]));
		setItemData(i, formatList[i], Qt::UserRole);
    }

}

QString DataFormatEditor::formatAsString(unsigned int format)
{
	QString tmp;
	
	unsigned int i;
	i=1;
	do {
		// 0xXXXXXXXX
		// shift 4 bits per position
		// position 1 shift 4 * 7
		// position 2 shift 4 * 6
		// position 3 shift 4 * 5
		// position 4 shift 4 * 4
		// position 1 shift 4 * 3
		// position 2 shift 4 * 2
		// position 3 shift 4 * 1
		// position 4 shift 4 * 0
		switch((format >> 4*(8-i))& 0xf) {
			case 0: // 8 bits
				tmp = tmp + QString("[8]");
				i = i+1;
				break;
			case 1: // 16 bits
				if (i<8) {
					tmp = tmp + QString("[16]");
					i = i+2;
				} else {
					tmp = tmp + QString("[8]");
					i = i+1;
				}
				break;
			case 2: // 32 bits
				if (i<6) {
					tmp = tmp + QString("[32]");
					i = i+4;
				} else {
					tmp = tmp + QString("[8]");
					i = i+1;
				}
				break;
			case 3: // 64 bits
				if (i<2) {
					tmp = tmp + QString("[64]");
					i = i+8;
				} else {
					tmp = tmp + QString("[8]");
					i = i+1;
				}
				break;
			case 15: // binary
				tmp = tmp + QString("[b]");
				i = i+1;
				break;
			default:
				tmp = tmp + QString("[8]");
				i = i+1;
				break;	
		}

	} while (i<9);
	
	return tmp;
}
