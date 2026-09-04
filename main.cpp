#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>


#include "HardwareMonitor.h"
#include "Parser.h"

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

    // Enter main loop
    while (true) {
        std::string responseText;

        if (monitor.FetchJson(responseText))
        {
            std::cout << "[pcVitals] Received "
                    << responseText.size()
                    << " bytes\n";

            PcStats stats = ParseJSON(responseText);
            PrintStats(stats);

            // TODO: Send stats to RP2040
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

    std::cout << "CPU Temperature: " << stats.cpuTemperature << " C\n";
    std::cout << "CPU Usage: " << stats.cpuUsage << " %\n";
    std::cout << "CPU Clock: " << stats.cpuClockMHz << " MHz\n";

    std::cout << "GPU Temperature: " << stats.gpuTemperature << " C\n";
    std::cout << "GPU Usage: " << stats.gpuUsage << " %\n";
    std::cout << "GPU Clock: " << stats.gpuClockMHz << " MHz\n";

    std::cout << "GPU VRAM: " << stats.gpuMemoryUsedMB << " MB\n";

    std::cout << "====================\n";
}

void Sleep(int seconds) {
    // Use chrono library since it is platform agnostic
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}