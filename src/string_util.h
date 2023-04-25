#pragma once

#include <iostream>
#include <string>

namespace scidf::str
{
    static std::string fill(const int len, const char c)
    {
        return std::string(len, c);
    }

    static std::string trim(const std::string& str, const std::string& whitespace)
    {
        std::size_t start = 0;
        while (whitespace.find(str[start]) != std::string::npos) start++;

        std::size_t end = str.length()-1;
        while (whitespace.find(str[end]) != std::string::npos) end--;
        end += 1;
        return str.substr(start, end-start);
    }
}