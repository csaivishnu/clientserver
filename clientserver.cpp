#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
using namespace std;

mutex mtx1; // Mutex for protecting shared resources
mutex mtx2; // Mutex for protecting shared resources
queue<int> commandMailBox; // Shared data between clients and server
queue<int> responseMailBox; // Shared data between clients and server
condition_variable cv; // Condition variable for synchronization

void clientThread() {
    for (int i = 0; i < 6; ++i) {
        mtx1.lock(); // Acquire the mutex lock
        commandMailBox.push(i); // Simulate adding data
        cout << "Client sent the command: " << i << endl;
        mtx1.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));

        while (!responseMailBox.empty()) {
            mtx2.lock(); // Acquire the mutex lock
            int data = responseMailBox.front();
            responseMailBox.pop();
            cout << "Client recieved the response: " << data << endl;
            mtx2.unlock(); // Release the mutex lock

            // Simulate some processing time
            this_thread::sleep_for(chrono::milliseconds(200));
        }
    }

    unique_lock<mutex> lock(mtx1);
    cv.wait(lock);

    while (!responseMailBox.empty()) {
        mtx2.lock(); // Acquire the mutex lock
        int data = responseMailBox.front();
        responseMailBox.pop();
        cout << "Client recieved the response: " << data << endl;
        mtx2.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void serverThread() {
    while (true) {
        if (!commandMailBox.empty()) {
            int data;
            mtx1.lock(); // Acquire the mutex lock

            data = commandMailBox.front();
            commandMailBox.pop();
            cout << "Server recieved the command: " << data << endl;

            mtx1.unlock(); // Release the mutex lock

            // Simulate some processing time
            this_thread::sleep_for(chrono::milliseconds(300));

            mtx2.lock(); // Acquire the mutex lock
            responseMailBox.push(data); // Simulate adding data
            cout << "Server sent the response: " << data << endl;
            mtx2.unlock(); // Release the mutex lock

            // Simulate some processing time
            this_thread::sleep_for(chrono::milliseconds(300));
        }

        // Exit the server loop when commandMailBox is empty
        if (commandMailBox.empty()) {
            break;
        }
    }
    cv.notify_one();
}

int main() {
    thread client(clientThread);
    thread server(serverThread);

    client.join();
    server.join();

    return 0;
}
