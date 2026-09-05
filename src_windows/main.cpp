#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include <sstream>

#include "HardwareMonitor.h"
#include "Parser.h"
#include "SerialPort.h"

// Function Declaration:
void PrintBanner();
void PrintStats(const PcStats& stats);
void Sleep(int seconds);

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
    std::string comPort = "COM5";
    SerialPort serial(comPort);
    if (serial.IsConnected()) {
        std::cout << "[pcVitals] Successfully connected to " << comPort << "!\n";
    } else {
        std::cerr << "[WARN] Failed to connect to " << comPort << ". Continuing without serial...\n";
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
                } else {
                    std::cerr << "[WARN] Failed to write to serial port.\n";
                }
            }
        } else {
            std::cout << "[WARN] No data received.\n";
        }

        Sleep(1);
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

void Sleep(int seconds) {
    // Use chrono library since it is platform agnostic
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}