#pragma once

#include <string>
#include <fstream>
#include <stdexcept>

#include "node.h"
#include "sdf_exception.h"

#include "third_party/json.hpp"

namespace scidf
{
    // Recursive helper to walk the JSON DOM and build the scidf string tree
    static void populate_scidf_node(const nlohmann::json& j, node_t& current_node)
    {
        if (j.is_object())
        {
            // If it's a dictionary, iterate and create named branches
            for (const auto& item : j.items())
            {
                populate_scidf_node(item.value(), current_node[item.key()]);
            }
        }
        else if (j.is_array())
        {
            // Flatten the array into a single bracketed string: "[val1, val2, val3]"
            std::string array_str = "[";
            for (size_t i = 0; i < j.size(); ++i)
            {
                if (j[i].is_string())
                {
                    // Extract strings cleanly without literal quotes (e.g., dxmin)
                    array_str += j[i].get<std::string>(); 
                }
                else
                {
                    // Convert numbers/booleans to strings
                    array_str += j[i].dump(); 
                }
                
                // Add comma and space between elements
                if (i < j.size() - 1)
                {
                    array_str += ", ";
                }
            }
            array_str += "]";
            
            // Assign the constructed string to your scidf node
            current_node = array_str;
        }
        else if (j.is_string())
        {
            // Extract single strings cleanly
            current_node = j.get<std::string>();
        }
        else
        {
            // Convert single numbers, booleans, or nulls natively back to strings
            current_node = j.dump();
        }
    }

    static void json_read(const std::string& filename, node_t& node, const context_t& parent_context)
    {
        // 1. Open the file
        std::ifstream file(filename);
        if (!file.is_open())
        {
            throw std::runtime_error("scidf::json_read - Could not open file: " + filename);
        }

        // 2. Parse the JSON file into a temporary DOM tree
        nlohmann::json raw_json;
        try
        {
            file >> raw_json;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            throw sdf_exception("scidf::json_read - JSON parse error: " + std::string(e.what()));
        }

        // 3. Recursively populate your custom node_t tree
        populate_scidf_node(raw_json, node);
    }
}