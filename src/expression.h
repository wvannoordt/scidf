#pragma once

namespace scidf
{
    struct expression_t
    {
        std::string name;
        std::vector<std::string> parameter_names;
        std::string content;
    };
}