// #include <iostream>
// #include <string>
// #include <windows.h>
// #include <winhttp.h>
// #include <nlohmann/json.hpp>

// #pragma comment(lib, "winhttp.lib")

// using json = nlohmann::json;

// std::string FetchHardwareData() {
//     HINTERNET hSession = WinHttpOpen(L"pcVitals/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
//     if (!hSession) return "";

//     HINTERNET hConnect = WinHttpConnect(hSession, L"localhost", 8085, 0);
//     if (!hConnect) { WinHttpCloseHandle(hSession); return ""; }

//     HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", L"/data.json", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
//     if (!hRequest) { WinHttpCloseHandle(hConnect); WinHttpCloseHandle(hSession); return ""; }

//     std::string response = "";
//     if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&
//         WinHttpReceiveResponse(hRequest, NULL)) {
        
//         DWORD bytesAvailable = 0;
//         DWORD bytesRead = 0;
//         do {
//             if (!WinHttpQueryDataAvailable(hRequest, &bytesAvailable) || bytesAvailable == 0) break;
//             char* buffer = new char[bytesAvailable + 1];
//             if (WinHttpReadData(hRequest, (LPVOID)buffer, bytesAvailable, &bytesRead)) {
//                 buffer[bytesRead] = '\0';
//                 response.append(buffer, bytesRead);
//             }
//             delete[] buffer;
//         } while (bytesAvailable > 0);
//     }

//     WinHttpCloseHandle(hRequest);
//     WinHttpCloseHandle(hConnect);
//     WinHttpCloseHandle(hSession);
//     return response;
// }

// // Recursive function to print all sensors so we can inspect names
// void PrintAllSensors(const json& node) {
//     if (node.is_object()) {
//         std::string type = node.value("Type", "");
//         std::string text = node.value("Text", "");
//         std::string value = node.value("Value", "");

//         // If it's a leaf sensor node (has a Value and Type)
//         if (!type.empty() && !text.empty() && !value.empty()) {
//             std::cout << "  [" << type << "] " << text << " = " << value;
//             if (node.contains("RawValue") && !node["RawValue"].is_null()) {
//                 std::cout << " (Raw: " << node["RawValue"] << ")";
//             }
//             std::cout << std::endl;
//         } else if (!text.empty()) {
//             // Print category/hardware headers
//             std::cout << "Node: " << text << " (Type: " << type << ")" << std::endl;
//         }

//         // Recurse into children
//         if (node.contains("Children") && node["Children"].is_array()) {
//             for (const auto& child : node["Children"]) {
//                 PrintAllSensors(child);
//             }
//         }
//     }
// }

// int main() {
//     std::cout << "Fetching data from LibreHardwareMonitor..." << std::endl;
    
//     std::string jsonStr = FetchHardwareData();
//     if (jsonStr.empty()) {
//         std::cerr << "Failed to fetch data. Is the LHM Remote Web Server running on port 8085?" << std::endl;
//         return 1;
//     }

//     try {
//         json root = json::parse(jsonStr);
//         std::cout << "--- Full LHM Sensor Tree ---" << std::endl;
//         PrintAllSensors(root);
//     } catch (json::parse_error& e) {
//         std::cerr << "JSON parsing error: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }

// std::string getPcData() {

// }


// using json = nlohmann::json;


// Ensure you have included these header-only libraries in your project paths
// #define CPPHTTPLIB_OPENSSL_SUPPORT // Only if you plan to use HTTPS later


#include <iostream>
#include <string>
#include <windows.h>
#include <winhttp.h>
#include <nlohmann/json.hpp>

#pragma comment(lib, "winhttp.lib")

// aliases the json type
using json = nlohmann::json;

constexpr int UPDATE_INTERVAL_MS = 1000;

#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>


#include "HardwareMonitor.h"
#include "Parser.h"

// Function Declaration:
void PrintBanner();
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

void Sleep(int seconds) {
    // Use chrono library since it is platform agnostic
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}