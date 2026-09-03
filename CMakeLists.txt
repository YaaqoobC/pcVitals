cmake_minimum_required(VERSION 3.15)

project(pcVitals LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Allow older dependencies to configure with modern CMake
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(json)

add_executable(pcVitals
    main.cpp
    HardwareMonitor.cpp
    Parser.cpp
)

target_link_libraries(pcVitals PRIVATE
    winhttp
    nlohmann_json::nlohmann_json
)