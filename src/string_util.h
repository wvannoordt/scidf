#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "sdf_exception.h"

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

    static inline std::vector<std::string>
    split(const std::string& str_in, const std::string dlm)
    {
        std::vector<std::string> subStrings;
        size_t pos = 0;
        std::string token;
        std::string str = str_in;
        while ((pos = str.find(dlm)) != std::string::npos)
        {
            token = str.substr(0, pos);
            subStrings.push_back(token);
            str.erase(0, pos + dlm.length());
        }
        subStrings.push_back(str);
        return subStrings;
    }

    static inline std::string
    read_contents(const std::string& filename)
    {
        if (!std::filesystem::exists(filename) || std::filesystem::is_directory(filename))
            throw sdf_exception("Cannot find file \"" + filename + "\"");
        std::ifstream t(filename);
        std::string str;
        t.seekg(0, std::ios::end);   
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        return str;
    }
}