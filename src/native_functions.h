#pragma once

#include <functional>
#include <vector>
#include <string>
#include <cstdlib>

#include "sdf_exception.h"
#include "conversion.h"

namespace scidf
{
    static void add_native_functions(auto& list)
    {
        list.insert({"env", [](const std::vector<std::string>& args)
        {
            if (args.size() != 1)
                throw sdf_exception("expecting 1 argument, received " + std::to_string(args.size()));
            const char* v = std::getenv(args[0].c_str());
            if (v == nullptr)
                throw sdf_exception("undefined environment variable \"" + args[0] + "\"");
            return std::string(v);
        }});

        list.insert({"choose", [](const std::vector<std::string>& args)
        {
            if (args.size() < 2)
                throw sdf_exception("expecting at least 2 arguments, received " + std::to_string(args.size()));
            int index;
            iconversion_t icv(args[0]);
            icv >> index;
            int ar_size = ((int)args.size()) - 1;
            if (index < 0 || index >= ar_size) throw sdf_exception("index " + std::to_string(index) + " out of range [0, " + std::to_string(ar_size) + ")");
            return args[index+1];
        }});
    }
}