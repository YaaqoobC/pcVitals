#pragma once
#include <windows.h>
#include <string>

class SerialPort {
    private:
        HANDLE hSerial;
        bool connected;

    public:
        SerialPort(const std::string& portName);
        ~SerialPort();

        bool IsConnected() const;
        bool Write(const char* data, DWORD size);
        bool WriteString(const std::string& str);
        int Read(char* buffer, DWORD bufferSize);
};