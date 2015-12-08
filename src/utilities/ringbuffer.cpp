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

#include "ringbuffer.h"
#include <string.h>

RingBuffer::RingBuffer() {
	head = 0;
	tail = 0;
}

RingBuffer::~RingBuffer() {
}

unsigned int
RingBuffer::getSpaceAvailable(void) const { 
	QMutexLocker locker(&mutex);
	return (RX_BUFFER_SIZE + head - tail) % RX_BUFFER_SIZE;
}

unsigned int 
RingBuffer::getDataAvailable(void) const {
    QMutexLocker locker(&mutex);
	unsigned int ret = 0;

    if (tail < head)
        ret = head - tail;
    else if (head < tail)
        ret = RX_BUFFER_SIZE + head - tail;

    return ret;
}

void 
RingBuffer::nonSecureAddData(char c) { 
	int i = (head + 1) % RX_BUFFER_SIZE; 
	if (i != tail) { 
		buffer[head] = c; 
		head = i; 
	} 
} 

void 
RingBuffer::addData(char* buffer, unsigned int amount) {
	for (unsigned int i = 0; i<amount; i++)
			nonSecureAddData(buffer[i]);
}


char
RingBuffer::nonSecureRemoveData(void) { 
	// if the head isn't ahead of the tail, we don't have any characters 
	if (head == tail) { 
		return 0; 
	} else { 
		char c = buffer[tail]; 
		tail = (tail + 1) % RX_BUFFER_SIZE;
		return c; 
	}
} 

void 
RingBuffer::trashOneData(void) {
	QMutexLocker locker(&mutex);
	nonSecureRemoveData();
}

void 
RingBuffer::removeData(char* buffer, unsigned int amount) {
	QMutexLocker locker(&mutex);
	for (unsigned int i = 0; i<amount; i++)
			buffer[i] = nonSecureRemoveData();
}

char
RingBuffer::consultFirstData(void) { 
	QMutexLocker locker(&mutex);
	// if the head isn't ahead of the tail, we don't have any characters 
	if (head == tail) { 
		return 0; 
	} else { 
		char c = buffer[tail];
		return c; 
	}
} 
