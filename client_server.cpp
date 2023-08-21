#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "headerfiles\mail_box.h"
#define MAXSIZE 64
using namespace std;

mutex mtx1; // Mutex for protecting shared resources
mutex mtx2; // Mutex for protecting shared resources
mutex mtx3; // Mutex for protecting shared resources
condition_variable cv; // Condition variable for synchronization
MailBox commandMailBox; // Circular buffer for commands
MailBox responseMailBox; // Circular buffer for responses

void clientThread() {
    for (uint32_t i = 0; i < 6; ++i) {
        mtx1.lock(); // Acquire the mutex lock
        std::vector<uint32_t> commandData;
        commandData.push_back(i);
        uint8_t commandSize;
        commandSize = i + 1;
        for (uint32_t j = 1; j < commandSize; j++) {
            commandData.push_back(j);
        }
        enqueue(commandMailBox, commandData); // Simulate adding data
        mtx3.lock(); // Acquire the mutex lock
        cout << "Client sent the command: " << commandData[0] << " with payloads:";
        for (uint32_t j = 1; j < commandSize; j++) {
            cout << " " << commandData[j];
        }
        cout << endl;
        mtx3.unlock(); // Release the mutex lock
        mtx1.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));

        while (true) {
            mtx2.lock(); // Acquire the mutex lock
            int success;
            std::vector<uint32_t> responseData;
            responseData = dequeue(responseMailBox, success);
            if (success == 0) {
                mtx2.unlock(); // Release the mutex lock
                break;
            }
            mtx3.lock(); // Acquire the mutex lock
            cout << "Client received the response: " << responseData[0] << " with payloads:";
            uint8_t responseSize;
            responseSize = uint8_t(responseData.size());
            for (uint32_t j = 1; j < responseSize; j++) {
                cout << " " << responseData[j];
            }
            cout << endl;
            mtx3.unlock(); // Release the mutex lock
            mtx2.unlock(); // Release the mutex lock

            // Simulate some processing time
            this_thread::sleep_for(chrono::milliseconds(200));
        }
    }

    unique_lock<mutex> lock(mtx1);
    cv.wait(lock);

    while (true) {
        mtx2.lock(); // Acquire the mutex lock
        int success;
        std::vector<uint32_t> responseData;
        responseData = dequeue(responseMailBox, success);
        if (success == 0) {
            mtx2.unlock(); // Release the mutex lock
            break;
        }
        mtx3.lock(); // Acquire the mutex lock
        cout << "Client received the response: " << responseData[0] << " with payloads:";
        uint8_t responseSize;
        responseSize = uint8_t(responseData.size());
        for (uint32_t j = 1; j < responseSize; j++) {
            cout << " " << responseData[j];
        }
        cout << endl;
        mtx3.unlock(); // Release the mutex lock
        mtx2.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void serverThread() {
    while (true) {
        mtx1.lock(); // Acquire the mutex lock
        int success;
        std::vector<uint32_t> commandData;
        commandData = dequeue(commandMailBox, success);
        if (success == 0) {
            mtx1.unlock(); // Release the mutex lock
            break;
        }
        mtx3.lock(); // Acquire the mutex lock
        cout << "Server received the command: " << commandData[0] << " with payloads:";
        uint8_t commandSize;
        commandSize = uint8_t(commandData.size());
        for (uint32_t j = 1; j < commandSize; j++) {
            cout << " " << commandData[j];
        }
        cout << endl;
        mtx3.unlock(); // Release the mutex lock
        mtx1.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));

        mtx2.lock(); // Acquire the mutex lock
        std::vector<uint32_t> responseData;
        responseData = commandData;
        uint8_t responseSize;
        responseSize = commandSize;
        enqueue(responseMailBox, responseData); // Simulate adding data
        mtx3.lock(); // Acquire the mutex lock
        cout << "Server sent the response: " << responseData[0] << " with payloads:";
        for (uint32_t j = 1; j < responseSize; j++) {
            cout << " " << responseData[j];
        }
        cout << endl;
        mtx3.unlock(); // Release the mutex lock
        mtx2.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));
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
