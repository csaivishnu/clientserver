#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
using namespace std;

mutex mtx1; // Mutex for protecting shared resources
mutex mtx2; // Mutex for protecting shared resources
queue<int> commandMailBox; // Shared data between clients and server
queue<int> responseMailBox; // Shared data between clients and server

void clientThread() {
    for (int i = 0; i < 5; ++i) {
        mtx1.lock(); // Acquire the mutex lock
        commandMailBox.push(i); // Simulate adding data
        cout << "Client sent the command: " << i << endl;
        mtx1.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));

        mtx2.lock(); // Acquire the mutex lock
        while (!responseMailBox.empty()) {
            int data = responseMailBox.front();
            responseMailBox.pop();
            cout << "Client recieved the response: " << data << endl;
        }
        mtx2.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

void serverThread() {
    while (true) {
        int data;
        mtx1.lock(); // Acquire the mutex lock
        if (!commandMailBox.empty()) {
            data = commandMailBox.front();
            commandMailBox.pop();
            cout << "Server recieved the command: " << data << endl;
        }
        mtx1.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(300));

        mtx2.lock(); // Acquire the mutex lock
        responseMailBox.push(data); // Simulate adding data
        cout << "Server sent the response: " << data << endl;
        mtx2.unlock(); // Release the mutex lock

        // Simulate some processing time
        this_thread::sleep_for(chrono::milliseconds(300));

        // Exit the server loop when commandMailBox is empty
        if (commandMailBox.empty()) {
            break;
        }
    }
}

int main() {
    thread client(clientThread);
    thread server(serverThread);

    client.join();
    server.join();

    return 0;
}
