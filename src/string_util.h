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

    template <typename string_t>
    static bool str_starts_at(const string_t& content, const std::size_t& j, const std::string& query)
    {
        if (j + query.length() - 1 >= content.length()) return false;
        std::size_t p = 0;
        while (p < query.length())
        {
            if (query[p] != content[j+p]) return false;
            p++;
        }
        return true;
    }

    template <typename string_t>
    static bool str_ends_at  (const string_t& content, const std::size_t& j, const std::string& query)
    {
        if (j < query.length()-1) return false;
        std::size_t p = 0;
        while(p < query.length())
        {
            if (query[query.length() - 1 - p] != content[j - p]) return false;
            p++;
        }
        return true;
    }
}