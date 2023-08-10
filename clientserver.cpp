#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#define MAXSIZE 64
using namespace std;

struct CircularBuffer {
    vector<unsigned int> buffer;
    int front;
    int rear;
    int maxSize;

    CircularBuffer(int size) : buffer(size), front(0), rear(0), maxSize(size) {}
};

mutex mtx1; // Mutex for protecting shared resources
mutex mtx2; // Mutex for protecting shared resources
condition_variable cv; // Condition variable for synchronization
CircularBuffer commandMailBox(MAXSIZE); // Circular buffer for commands
CircularBuffer responseMailBox(MAXSIZE); // Circular buffer for responses

int circularIncrement(int value, int max) {
    return (value + 1) % max;
}

void enqueue(CircularBuffer& cb, unsigned int data) {
    if (circularIncrement(cb.rear, cb.maxSize) != cb.front) {
        cb.buffer[cb.rear] = data;
        cb.rear = circularIncrement(cb.rear, cb.maxSize);
    }
}

unsigned int dequeue(CircularBuffer& cb, int &success) {
    if (cb.front != cb.rear) {
        unsigned int data = cb.buffer[cb.front];
        cb.front = circularIncrement(cb.front, cb.maxSize);
        success = 1;
        return data;
    }
    success = 0;
    return 0; // Empty buffer
}

void clientThread() {
    for (unsigned int i = 0; i < 6; ++i) {
        mtx1.lock(); // Acquire the mutex lock
        enqueue(commandMailBox, i); // Simulate adding data
        cout << "Client sent the command: " << i << endl;
        mtx1.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));

        while (true) {
            mtx2.lock(); // Acquire the mutex lock
            int success;
            unsigned int data = dequeue(responseMailBox, success);
            mtx2.unlock(); // Release the mutex lock
            if (success == 0) {
                break;
            }
            cout << "Client received the response: " << data << endl;

            // Simulate some processing time
            this_thread::sleep_for(chrono::milliseconds(200));
        }
    }

    unique_lock<mutex> lock(mtx1);
    cv.wait(lock);

    while (true) {
        mtx2.lock(); // Acquire the mutex lock
        int success;
        unsigned int data = dequeue(responseMailBox, success);
        mtx2.unlock(); // Release the mutex lock
        if (success == 0) {
            break;
        }
        cout << "Client received the response: " << data << endl;

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void serverThread() {
    while (true) {
        mtx1.lock(); // Acquire the mutex lock
        int success;
        unsigned int data = dequeue(commandMailBox, success);
        mtx1.unlock(); // Release the mutex lock
        if (success == 0) {
            break;
        }

        cout << "Server received the command: " << data << endl;

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(400));

        mtx2.lock(); // Acquire the mutex lock
        enqueue(responseMailBox, data); // Simulate adding data
        cout << "Server sent the response: " << data << endl;
        mtx2.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(400));
    }

    // Signal that the server has completed using the condition variable
    cv.notify_one();
}

int main() {
    thread client(clientThread);
    thread server(serverThread);

    client.join();
    server.join();

    return 0;
}
