
// QT includes
#include <QMutex>

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#define RX_BUFFER_SIZE 10000 

class RingBuffer
{
public:
	RingBuffer();
    virtual ~RingBuffer();

	unsigned int getSpaceAvailable(void) const;
	unsigned int getDataAvailable(void) const;
	char consultFirstData(void);

	void addData(char* buffer, unsigned int amount);
	void removeData(char* buffer, unsigned int amount);
	void trashOneData(void);

private:
	void nonSecureAddData(char c);
	char nonSecureRemoveData(void);

    mutable QMutex mutex;
	char buffer[RX_BUFFER_SIZE];
	int head;
	int tail;
};

#endif // RINGBUFFER_H
