#include "Parser.h"

#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    bool SearchChildrenNodes(float& result, const json& node, const std::vector<std::string>& path, size_t depth);
    
    bool FindSensor(float& result, const json& node, const std::vector<std::string>& path, size_t depth = 0) {
        if (!node.is_object())
            return false;

        const std::string text = node.value("Text", "");

        // Did this node match the current path element?
        if (text == path[depth]) {

            // If this is the final node try and get the value?
            if (depth == path.size() - 1) {
                if (!node.contains("Value") || node["Value"].is_null())
                    return false;

                try {
                    if (node["Value"].is_number()) {
                        result = node["Value"].get<float>();
                    }
                    else {
                        result = std::stof(node["Value"].get<std::string>());
                    }

                    return true;
                } catch (...) {
                    return false;
                }
            }

            // Continue searching children for the NEXT path element (newDepth = oldDepth + 1)
            return SearchChildrenNodes(result, node, path, depth + 1);
        }

        // Current node didn't match so keep searching children at the SAME depth. (newDepth = oldDepth)
        return SearchChildrenNodes(result, node, path, depth);
    }
    
    bool SearchChildrenNodes(float& result, const json& node, const std::vector<std::string>& path, size_t depth) {
        if (node.contains("Children") && node["Children"].is_array()) {
            for (const auto& child : node["Children"]) {
                if (FindSensor(result, child, path, depth))
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
        std::cout << "JSON parsed successfully, begining search...\n";
        
        // Get the CPU Info:
        std::vector<std::string> cpuTemp = {"AMD Ryzen 5 7600X", "Temperatures", "Package"};
        std::vector<std::string> cpuClk = {"AMD Ryzen 5 7600X", "Clocks", "Cores (Average)"};
        std::vector<std::string> cpuLoad = {"AMD Ryzen 5 7600X", "Load", "CPU Total"};
        FindSensor(stats.cpuTemp, root, cpuTemp);
        FindSensor(stats.cpuClk, root, cpuClk);
        FindSensor(stats.cpuLoad, root, cpuLoad);

        // Get the GPU Info:
        std::vector<std::string> gpuTemp = {"NVIDIA GeForce RTX 4070 SUPER", "Temperatures", "GPU Core"};
        std::vector<std::string> gpuVRAM = {"NVIDIA GeForce RTX 4070 SUPER", "Data", "GPU Memory Used"};
        std::vector<std::string> gpuLoad = {"NVIDIA GeForce RTX 4070 SUPER", "Load", "GPU Core"};
        FindSensor(stats.gpuTemp, root, gpuTemp);
        FindSensor(stats.gpuVRAM, root, gpuVRAM);
        FindSensor(stats.gpuLoad, root, gpuLoad);

        // Get the RAM Info:
        std::vector<std::string> ramUsage = {"Total Memory", "Data", "Memory Used"};
        FindSensor(stats.ramUsage, root, ramUsage);
    }
    catch (const json::exception& e) {
        std::cerr << "[ERROR] JSON exception: " << e.what() << '\n';
    }
    return stats;
}