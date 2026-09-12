#include <iostream>
#include <thread>
#include <chrono>

#include "HardwareMonitor.h"
#include "Parser.h"
#include "SerialPort.h"

constexpr const char* COM_PORT = "COM6";
constexpr int SERIAL_SETTLE_SECONDS = 2;
constexpr int POLL_INTERVAL_SECONDS = 1;
constexpr int SERIAL_RECONNECT_RETRY_SECONDS = 5;

void PrintBanner();
void WaitForSerialConnection(SerialPort& serial);
void SleepSeconds(int value);

int main() {
    PrintBanner();

    HardwareMonitor monitor;
    if (!monitor.Initialize()) {
        std::cerr << "[ERROR] Failed to initialize Hardware Monitor.\n";
        return 1;
    }
    std::cout << "[pcVitals] Connected to LibreHardwareMonitor!\n";

    SerialPort serial(COM_PORT);
    WaitForSerialConnection(serial);

    while (true) {
        std::string responseText;

        if (monitor.FetchJson(responseText)) {
            std::cout << "[pcVitals] Received " << responseText.size() << " bytes\n";

            PcStats stats = ParseJSON(responseText);
            PrintStats(stats);

            if (!serial.WriteStats(stats)) {
                WaitForSerialConnection(serial);
                continue;
            }
        } else {
            std::cout << "[WARN] No data received.\n";
        }

        SleepSeconds(POLL_INTERVAL_SECONDS);
    }

    return 0;
}

void PrintBanner() {
    std::cout << "=========================================\n";
    std::cout << "[pcVitals] Monitor Starting ... \n";
    std::cout << "=========================================\n";
}

void WaitForSerialConnection(SerialPort& serial) {
    while (!serial.IsConnected()) {
        std::cerr << "[pcVitals] Serial unavailable. Retrying " << COM_PORT << " in " << SERIAL_RECONNECT_RETRY_SECONDS << "s.\n";
        SleepSeconds(SERIAL_RECONNECT_RETRY_SECONDS);
        serial.Reconnect();
    }

    std::cout << "[pcVitals] Connected to " << COM_PORT << "!\n";
    std::cout << "[pcVitals] Waiting for RP2040 USB serial to settle...\n";
    SleepSeconds(SERIAL_SETTLE_SECONDS);
}

void SleepSeconds(int value) {
    std::this_thread::sleep_for(std::chrono::seconds(value));
}