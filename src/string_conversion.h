#pragma once

#include <string>
#include <iostream>
#include <vector>

namespace scidf::str
{
    template <typename data_t> static std::string convert_to_string(const data_t& data)
    {
        return std::to_string(data);
    }
}