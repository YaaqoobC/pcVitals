#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include <sstream>

#include "HardwareMonitor.h"
#include "Parser.h"
#include "SerialPort.h"

#define COM_PORT "COM6"

// Function Declaration:
void PrintBanner();
void PrintStats(const PcStats& stats);
void Sleep(int value, bool isMs);

int main() {
    PrintBanner();

    // Initialize networking
    HardwareMonitor monitor;
    if (!monitor.Initialize()) {
        std::cerr << "[ERROR] Failed to initialize Hardware Monitor.\n";
        return 1;
    }
    std::cout << "[pcVitals] Connected to LibreHardwareMonitor!\n";

    // Init serial port
    SerialPort serial(COM_PORT);
    if (serial.IsConnected()) {
        std::cout << "[pcVitals] Successfully connected to " << COM_PORT << "!\n";
    } else {
        std::cerr << "[WARN] Failed to connect to " << COM_PORT << ". Continuing without serial...\n";
    }

    // Enter main loop
    while (true) {
        std::string responseText;

        if (monitor.FetchJson(responseText)) {
            std::cout << "[pcVitals] Received " << responseText.size() << " bytes\n";

            PcStats stats = ParseJSON(responseText);
            PrintStats(stats);

            // TODO: Send stats to RP2040
            if (serial.IsConnected()) {
                std::ostringstream stringStream;

                // The Packet is [S,cpuTemp,cpuLoad,cpuClk,gpuTemp,gpuLoad,gpuVram,ramUsage,E\n]
                stringStream << "S,"
                             << stats.cpu.temp << "," << stats.cpu.load << "," << stats.cpu.clk << ","
                             << stats.gpu.temp << "," << stats.gpu.load << "," << stats.gpu.vram << ","
                             << stats.ram.usage << ",E\n";

                if (serial.WriteString(stringStream.str())) {
                    std::cout << "[pcVitals] Sent payload to RP2040: " << stringStream.str();

                    // Sleep(50, true);
                    // char rxBuffer[256];
                    // int bytesRead = serial.Read(rxBuffer, sizeof(rxBuffer));
                    // if (bytesRead > 0) {
                    //     std::cout << "[pcVitals] Pico has responded with: " << rxBuffer;
                    // }

                } else {
                    std::cerr << "[WARN] Failed to write to serial port.\n";
                }
            }
        } else {
            std::cout << "[WARN] No data received.\n";
        }

        Sleep(1, false);
    }

    return 0;
}

void PrintBanner() {
    std::cout << "=========================================\n";
    std::cout << "[pcVitals] Monitor Starting ... \n";
    std::cout << "=========================================\n";
}

void PrintStats(const PcStats& stats) {

    std::cout << "\n===== PC Stats =====\n";

    std::cout << "CPU Temperature: " << stats.cpu.temp << " C\n";
    std::cout << "CPU Usage: " << stats.cpu.load << " %\n";
    std::cout << "CPU Clock: " << stats.cpu.clk << " MHz\n";

    std::cout << "GPU Temperature: " << stats.gpu.temp << " C\n";
    std::cout << "GPU Usage: " << stats.gpu.load << " %\n";
    std::cout << "GPU VRAM: " << stats.gpu.vram << " MB\n";

    std::cout << "RAM Usage: " << stats.ram.usage << " MB\n";

    std::cout << "====================\n";
}

void Sleep(int value, bool isMs) {
    // Use chrono library since it is platform agnostic
    if (isMs)
        std::this_thread::sleep_for(std::chrono::milliseconds(value));
    else
        std::this_thread::sleep_for(std::chrono::seconds(value));
}