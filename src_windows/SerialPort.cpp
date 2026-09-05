#include "SerialPort.h"
#include <iostream>

SerialPort::SerialPort(const std::string& portName) : connected(false) {
    // The "\\\\.\\" prefix allows opening COM ports higher than COM9
    std::string fullPortName = "\\\\.\\" + portName;

    // Open the port
    hSerial = CreateFileA(fullPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            std::cerr << "[ERROR] Serial port " << portName << " not found.\n";
        } else {
            std::cerr << "[ERROR] Unknown error opening " << portName << ".\n";
        }
        return;
    }

    // Device control block to configure the hardware UART settings
    // 115200baud, 8 data bits, no parity, 1 stop bit ===> 8115200 8N1
    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "[ERROR] Failed to get current serial parameters.\n";
        CloseHandle(hSerial);
        return;
    }

    // Configure the baud rate and protocol (115200 baud, 8 data bits, no parity, 1 stop bit)
    dcbSerialParams.BaudRate = CBR_115200; 
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "[ERROR] Failed to set serial parameters.\n";
        CloseHandle(hSerial);
        return;
    }

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "[ERROR] Failed to set serial timeouts.\n";
        CloseHandle(hSerial);
        return;
    }

    connected = true;
    
    // Clear any residual garbage data in the buffers
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

SerialPort::~SerialPort() {
    if (connected) {
        connected = false;
        CloseHandle(hSerial);
    }
}

bool SerialPort::IsConnected() const {
    return connected;
}

bool SerialPort::Write(const char* data, DWORD size) {
    DWORD bytesWritten;
    if (!WriteFile(hSerial, data, size, &bytesWritten, NULL)) {
        ClearCommError(hSerial, NULL, NULL);
        return false;
    }
    return (bytesWritten == size);
}

bool SerialPort::WriteString(const std::string& str) {
    return Write(str.c_str(), str.length());
}