#include "HardwareMonitor.h"

#include <iostream>
#include <vector>

HardwareMonitor::HardwareMonitor()
    : m_session(nullptr),
      m_connection(nullptr)
{   
}

HardwareMonitor::~HardwareMonitor()
{
    if (m_connection)
        WinHttpCloseHandle(m_connection);

    if (m_session)
        WinHttpCloseHandle(m_session);
}

bool HardwareMonitor::Initialize()
{
    m_session = WinHttpOpen(
        L"PCMonitor/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );
    
    if (!m_session) {
        std::cerr << "[ERROR] Failed to create WinHTTP session\n";
        return false;
    }

    m_connection = WinHttpConnect(
        m_session,
        L"localhost",
        8085,
        0
    );

    if (!m_connection) {
        std::cerr << "[ERROR] Failed to connect to localhost:8085\n";
        return false;
    }

    return true;
}

bool HardwareMonitor::FetchJson(std::string& json) {
    // Build the GET request
    HINTERNET request = WinHttpOpenRequest(
        m_connection,
        L"GET",
        L"/data.json",
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0
    );
    
    if (!request)
        return false;

    std::string response;
    bool success = false;

    // Send the request and block till the response
    if (WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) &&  WinHttpReceiveResponse(request, nullptr)) {
        DWORD bytesAvailable = 0;
        success = true;

        do {
            if (!WinHttpQueryDataAvailable(request, &bytesAvailable) || bytesAvailable == 0)
                break;

            std::vector<char> buffer(bytesAvailable);
            DWORD bytesRead = 0;

            // Copy the data into the buffer and then append to our response string
            if (WinHttpReadData(request, buffer.data(), bytesAvailable, &bytesRead)) {
                response.append(buffer.data(), bytesRead);
            } else {
                break; // Stop if reading fails
            }

        } while (bytesAvailable > 0);
    }
    
    // Close the Request and return the response
    WinHttpCloseHandle(request);

    if (success)
        json = response;
        
    return success;
}
