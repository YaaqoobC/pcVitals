#pragma once
#include <windows.h>
#include <string>

#include "PcStats.h"

class SerialPort {
    private:
        HANDLE hSerial;
        bool connected;
        std::string portName;

        bool Connect();
        void Disconnect();

    public:
        SerialPort(const std::string& portName);
        ~SerialPort();

        bool IsConnected() const;
        bool Reconnect();
        bool Write(const char* data, DWORD size);
        bool WriteString(const std::string& str);
        bool WriteStats(const PcStats& stats);
        int Read(char* buffer, DWORD bufferSize);
};
