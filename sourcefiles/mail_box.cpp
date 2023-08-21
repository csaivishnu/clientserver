#include "..\headerfiles\mail_box.h"

MailBox::MailBox() : head(0), tail(0), bufferSizeUsed(0) {}

void enqueue(MailBox& mailBox, std::vector<uint32_t>& commandData) {
    uint8_t commandSize;
    commandSize = commandData.size();
    uint8_t bufferSizeUsed;
    bufferSizeUsed = uint8_t((mailBox.head - mailBox.tail + 238) % 238);
    if ((bufferSizeUsed + (commandSize * 4)) < 237) {
        for (int commandIndex = 0; commandIndex < commandSize; commandIndex++) {
            uint32_t dword;
            dword = commandData[commandIndex];
            mailBox.buffer[mailBox.head++] = uint8_t(dword >> 24);
            mailBox.buffer[mailBox.head++] = uint8_t((dword >> 16) & 255);
            mailBox.buffer[mailBox.head++] = uint8_t((dword >> 8) & 255);
            mailBox.buffer[mailBox.head++] = uint8_t(dword & 255);
        }
    }
}

std::vector<uint32_t> dequeue(MailBox& mailBox, int& success) {
    std::vector<uint32_t> commandData;
    success = 0;
    uint8_t bufferSizeUsed;
    bufferSizeUsed = uint8_t((mailBox.head - mailBox.tail + 238) % 238);
    if (bufferSizeUsed != 0) {
        uint8_t commandSize;
        uint8_t payloadSize;
        payloadSize = uint8_t(mailBox.buffer[mailBox.tail + 3] & 255);
        commandSize = payloadSize + 1;
        for (int commandIndex = 0; commandIndex < commandSize; commandIndex++) {
            uint32_t dword;
            dword = 0;
            dword |= ((uint32_t(mailBox.buffer[mailBox.tail++])) << 24);
            dword |= ((uint32_t(mailBox.buffer[mailBox.tail++])) << 16);
            dword |= ((uint32_t(mailBox.buffer[mailBox.tail++])) << 8);
            dword |= uint32_t(mailBox.buffer[mailBox.tail++]);
            commandData.push_back(dword);
        }
        success = 1;
    }
    return commandData; // Empty buffer
}