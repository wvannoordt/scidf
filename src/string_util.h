#pragma once

#include <iostream>
#include <string>

namespace scidf::str
{
    static std::string fill(const int len, const char c)
    {
        return std::string(len, c);
    }
}