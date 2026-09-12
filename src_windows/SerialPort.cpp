#include "SerialPort.h"

#include <iostream>
#include <sstream>

namespace {
    std::string BuildStatsPayload(const PcStats& stats) {
        std::ostringstream stringStream;

        // Packet: S,cpuTemp,cpuLoad,cpuClk,gpuTemp,gpuLoad,gpuVram,ramUsage\n
        stringStream << "S,"
                     << stats.cpu.temp << "," << stats.cpu.load << "," << stats.cpu.clk << ","
                     << stats.gpu.temp << "," << stats.gpu.load << "," << stats.gpu.vram << ","
                     << stats.ram.usage << "\n";

        return stringStream.str();
    }
}

SerialPort::SerialPort(const std::string& portName)
    : hSerial(INVALID_HANDLE_VALUE),
      connected(false),
      portName(portName)
{
    Connect();
}

SerialPort::~SerialPort() {
    Disconnect();
}

bool SerialPort::Connect() {
    // The "\\\\.\\" prefix allows opening COM ports higher than COM9
    std::string fullPortName = "\\\\.\\" + this->portName;

    // Open the port
    hSerial = CreateFileA(fullPortName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_NOT_FOUND) {
            std::cerr << "[ERROR] Serial port " << this->portName << " not found.\n";
        } else {
            std::cerr << "[ERROR] Unknown error opening " << this->portName << ".\n";
        }
        return false;
    }

    DCB dcbSerialParams = {0};
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "[ERROR] Failed to get current serial parameters.\n";
        Disconnect();
        return false;
    }

    // Configure the baud rate and protocol (115200 baud, 8 data bits, no parity, 1 stop bit)
    dcbSerialParams.BaudRate = CBR_115200; 
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity   = NOPARITY;

    dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
    dcbSerialParams.fRtsControl = RTS_CONTROL_ENABLE;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        std::cerr << "[ERROR] Failed to set serial parameters.\n";
        Disconnect();
        return false;
    }

    EscapeCommFunction(hSerial, SETDTR);
    EscapeCommFunction(hSerial, SETRTS);

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        std::cerr << "[ERROR] Failed to set serial timeouts.\n";
        Disconnect();
        return false;
    }

    connected = true;
    
    // Clear any residual garbage data in the buffers
    PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
    return true;
}

void SerialPort::Disconnect() {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
    }

    connected = false;
}

bool SerialPort::IsConnected() const {
    return connected;
}

bool SerialPort::Reconnect() {
    Disconnect();
    return Connect();
}

bool SerialPort::Write(const char* data, DWORD size) {
    DWORD bytesWritten;
    if (!WriteFile(hSerial, data, size, &bytesWritten, NULL)) {
        ClearCommError(hSerial, NULL, NULL);
        Disconnect();
        return false;
    }

    if (bytesWritten != size) {
        Disconnect();
        return false;
    }

    return true;
}

bool SerialPort::WriteString(const std::string& str) {
    return Write(str.c_str(), str.length());
}

bool SerialPort::WriteStats(const PcStats& stats) {
    const std::string payload = BuildStatsPayload(stats);

    if (!WriteString(payload)) {
        std::cerr << "[WARN] Failed to write to serial port.\n";
        return false;
    }

    std::cout << "[pcVitals] Sent payload to RP2040: " << payload;
    return true;
}

// Use this to read the 'acknowledgement' from the pico when it receives the data.
int SerialPort::Read(char* buffer, DWORD bufferSize) {
    DWORD bytesRead = 0;
    ClearCommError(hSerial, NULL, NULL);
    
    // ReadFile will check the incoming serial buffer
    if (ReadFile(hSerial, buffer, bufferSize - 1, &bytesRead, NULL)) {
        buffer[bytesRead] = '\0'; // Ensure it's a valid C-string
        return bytesRead;
    }
    return 0;
}
