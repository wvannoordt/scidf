#pragma once

#include <string>
#include <vector>

#include "context.h"
#include "resolve.h"
#include "sdf_exception.h"

namespace scidf
{
    static void execute_imperative(const std::string& func_name, const std::vector<std::string>& func_args, context_t& context)
    {
        //imperatives modify the state of the context
        if(func_name == context.get_syms().path_append)
        {
            for (auto arg: func_args)
            {
                auto path = resolve(arg, context);
                context.add_path(path);
            }
        }
        else if (func_name == context.get_syms().cli_require)
        {
            for (const auto& ar: func_args)
            {
                if (!context.has_cli_arg(ar)) throw sdf_exception("required command-line argument \"" + ar + "\" not found!");
            }
        }
        else
        {
            throw sdf_exception("undefined imperative \"" + func_name + "\"");
        }
    }
}