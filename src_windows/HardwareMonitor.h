#pragma once

#include <string>
#include <windows.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

class HardwareMonitor 
{
private:
    HINTERNET m_session;
    HINTERNET m_connection;

public:
    HardwareMonitor();
    ~HardwareMonitor();

    bool Initialize();
    bool FetchJson(std::string& json);
};

