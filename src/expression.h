#pragma once

#include <vector>
#include <iostream>
#include <string>

namespace scidf
{
    struct expression_t
    {
        std::string name;
        std::vector<std::string> parameter_names;
        std::string content;
    };

    static std::ostream& operator << (std::ostream& os, const expression_t& expr)
    {
        os << "Name: " << expr.name << "\nContent: " << expr.content << "\nParameters:";
        for (const auto& p: expr.parameter_names) os << "\n" << p;
        return os;
    }
}