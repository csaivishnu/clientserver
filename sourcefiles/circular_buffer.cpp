#include "..\headerfiles\circular_buffer.h"

CircularBuffer::CircularBuffer(int size) : buffer(size), front(0), rear(0), maxSize(size) {}

int circularIncrement(int value, int max) {
    return (value + 1) % max;
}

void enqueue(CircularBuffer& cb, unsigned int data) {
    if (circularIncrement(cb.rear, cb.maxSize) != cb.front) {
        cb.buffer[cb.rear] = data;
        cb.rear = circularIncrement(cb.rear, cb.maxSize);
    }
}

unsigned int dequeue(CircularBuffer& cb, int& success) {
    if (cb.front != cb.rear) {
        unsigned int data = cb.buffer[cb.front];
        cb.front = circularIncrement(cb.front, cb.maxSize);
        success = 1;
        return data;
    }
    success = 0;
    return 0; // Empty buffer
}
