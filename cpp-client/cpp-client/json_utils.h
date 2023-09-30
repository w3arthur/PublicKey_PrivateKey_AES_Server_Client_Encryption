#pragma once

#include <iostream>
#include <string>
#include <map>
#include <variant>
#include <vector>

using Value = std::variant<int, double, std::string>;

// Create a struct to represent key-value pairs
struct KeyValuePair {
    std::string key;
    Value value;
};

namespace client
{

    // Function to serialize a map to JSON format
    std::string SerializeMapToJson(const std::vector<KeyValuePair>& mapData) {
        std::string json = "{";

        for (size_t i = 0; i < mapData.size(); ++i) {
            const KeyValuePair& pair = mapData[i];

            // Add key
            json += "\"" + pair.key + "\":";

            // Determine the type of the value and add it to the JSON
            if (std::holds_alternative<int>(pair.value)) {
                json += std::to_string(std::get<int>(pair.value));
            }
            else if (std::holds_alternative<double>(pair.value)) {
                json += std::to_string(std::get<double>(pair.value));
            }
            else if (std::holds_alternative<std::string>(pair.value)) {
                json += "\"" + std::get<std::string>(pair.value) + "\"";
            }

            // Add a comma if it's not the last pair
            if (i < mapData.size() - 1) {
                json += ",";
            }
        }

        json += "}";
        return json;
    }




    // Function to deserialize a JSON response and populate a vector of KeyValuePair
    std::vector<KeyValuePair> DeserializeJson(const std::string& jsonResponse) {
        std::vector<KeyValuePair> mapData;

        size_t start = jsonResponse.find("{");
        size_t end = jsonResponse.rfind("}");


        if (start != std::string::npos && end != std::string::npos) {
            std::string jsonContent = jsonResponse.substr(start + 1, end - start - 1);
            size_t pos = 0;
            while ((pos = jsonContent.find(',')) != std::string::npos) {
                std::string pair = jsonContent.substr(0, pos);
                size_t colonPos = pair.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = pair.substr(1, colonPos - 2); // Remove surrounding quotes
                    std::string valueStr = pair.substr(colonPos + 1);

                    // Determine the type of the value and convert it accordingly
                    if (valueStr.find('.') != std::string::npos) {
                        double value = std::stod(valueStr);
                        mapData.push_back({ key, value });
                    }
                    else if (valueStr == "true" || valueStr == "false") {
                        bool value = (valueStr == "true");
                        mapData.push_back({ key, value });
                    }
                    else if (valueStr[0] == '"' && valueStr[valueStr.length() - 1] == '"') {
                        std::string value = valueStr.substr(1, valueStr.length() - 2);
                        mapData.push_back({ key, value });
                    }
                    else {
                        int value = std::stoi(valueStr);
                        mapData.push_back({ key, value });
                    }
                }
                jsonContent.erase(0, pos + 1);
            }

            // Handle the last pair
            size_t colonPos = jsonContent.find(':');
            if (colonPos != std::string::npos) {
                std::string key = jsonContent.substr(1, colonPos - 2); // Remove surrounding quotes
                std::string valueStr = jsonContent.substr(colonPos + 1);

                if (valueStr.find('.') != std::string::npos) {
                    double value = std::stod(valueStr);
                    mapData.push_back({ key, value });
                }
                else if (valueStr == "true" || valueStr == "false") {
                    bool value = (valueStr == "true");
                    mapData.push_back({ key, value });
                }
                else if (valueStr[0] == '"' && valueStr[valueStr.length() - 1] == '"') {
                    std::string value = valueStr.substr(1, valueStr.length() - 2);
                    mapData.push_back({ key, value });
                }
                else {
                    int value = std::stoi(valueStr);
                    mapData.push_back({ key, value });
                }
            }
        }

        return mapData;
    }




}
