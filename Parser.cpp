#include "Parser.h"

#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    void FindSensors(const json& node) {
        // Recursively look for and print out all the sensors
        if (!node.is_object())
            return;

        std::string type = node.value("Type", "");
        std::string text = node.value("Text", "");
        std::string value = node.value("Value", "");

        if (!type.empty() && !text.empty() && !value.empty()) {
            std::cout
                << "[" << type << "] "
                << text << " = "
                << value
                << '\n';
        }

        if (node.contains("Children") && node["Children"].is_array()) {
            for (const auto& child : node["Children"]) {
                FindSensors(child);
            }
        }
    }
}

PcStats ParseJSON(const std::string& responseText) {
    PcStats stats;

    try {
        json root = json::parse(responseText);

        std::cout << "----- Sensors -----\n";
        FindSensors(root);
        std::cout << "-------------------\n";

    } catch (const json::parse_error& e) {
        std::cerr << "[ERROR] JSON parsing failed: "
                  << e.what()
                  << '\n';
    }

    return stats;
}