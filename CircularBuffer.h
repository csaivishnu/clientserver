#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <vector>

struct CircularBuffer {
    std::vector<unsigned int> buffer;
    int front;
    int rear;
    int maxSize;

    CircularBuffer(int size);
};

void enqueue(CircularBuffer& cb, unsigned int data);
unsigned int dequeue(CircularBuffer& cb, int& success);

#endif
