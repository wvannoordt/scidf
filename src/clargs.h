#pragma once

#include <vector>
#include <string>

namespace scidf
{
    struct clargs_t
    {
        std::vector<std::string> raw;
        clargs_t(int argc, char** argv)
        {
            for (int i = 0; i < argc; ++i)
            {
                raw.push_back(std::string(argv[i]));
            }
        }
    };
    
}