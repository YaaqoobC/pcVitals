#include "Parser.h"

#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    bool FindSensor(const json& node,
                    const std::string& type,
                    const std::string& name,
                    float& result)
    {
        if (!node.is_object())
            return false;

        if (node.value("Type", "") == type &&
            node.value("Text", "") == name)
        {
            if (!node.contains("RawValue") || node["RawValue"].is_null())
                return false;

            try
            {
                if (node["RawValue"].is_number())
                {
                    result = node["RawValue"].get<float>();
                }
                else
                {
                    result = std::stof(node["RawValue"].get<std::string>());
                }

                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        if (node.contains("Children"))
        {
            for (const auto& child : node["Children"])
            {
                if (FindSensor(child, type, name, result))
                    return true;
            }
        }

        return false;
    }
}

PcStats ParseJSON(const std::string& responseText) {
    PcStats stats;

    try {
        json root = json::parse(responseText);

        std::cout << "JSON parsed successfully\n";

        FindSensor(root, "Temperature", "Core (Tctl/Tdie)", stats.cpuTemperature);
        std::cout << "Found CPU Temp\n";
        FindSensor(root, "Load", "CPU Total", stats.cpuUsage);
        std::cout << "Found CPU load\n";
        FindSensor(root, "Clock", "Cores (Average)", stats.cpuClockMHz);
        std::cout << "Found CPU clk\n";

        FindSensor(root, "Temperature", "GPU Core", stats.gpuTemperature);
        std::cout << "Found GPU Temp\n";
        FindSensor(root, "Load", "GPU Core", stats.gpuUsage);
        std::cout << "Found CPU load\n";
        FindSensor(root, "Clock", "GPU Core", stats.gpuClockMHz);
        std::cout << "Found GPU clk\n";
        
        FindSensor(root, "SmallData", "GPU Memory Used", stats.gpuMemoryUsedMB);
        std::cout << "Found GPU Vram\n";
    }
    catch (const json::exception& e) {
        std::cerr << "[ERROR] JSON exception: "
                << e.what() << '\n';
    }
    return stats;
}