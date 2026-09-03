cmake_minimum_required(VERSION 3.15)

# Allow older subprojects (like the JSON library) to configure under modern CMake
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

project(pcVitals CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(json)

add_executable(pcVitals main.cpp)
target_link_libraries(pcVitals PRIVATE winhttp nlohmann_json::nlohmann_json)