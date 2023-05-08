#pragma once

#include <tuple>
#include <string>
#include <vector>
#include "context.h"

namespace scidf
{
    static void context_def(const std::string& line, context_t& context)
    {
        //allows for the definition of a function
        const std::string& raw = line;
        if (raw[0] != context.get_syms().invoke_var) throw sdf_exception("no expression invocation symbol");
        std::size_t i_assign = raw.find_first_of(context.get_syms().assignment);
        if (i_assign == std::string::npos || i_assign == 0) throw sdf_exception("cannot find assignment symbol");

        std::string raw_fname = str::trim(raw.substr(1, i_assign - 1), context.get_syms().whitespace);

        i_assign ++;
        if (i_assign >= raw.length()-1) throw sdf_exception("empty expression assignment");
        std::string raw_rhs   = str::trim(raw.substr(i_assign, raw.length() - i_assign), context.get_syms().whitespace);
        expression_t new_expression;
        new_expression.content = raw_rhs;
        new_expression.parameter_names.clear();
        if (raw_fname.find(context.get_syms().open_arg) == std::string::npos)
        {
            new_expression.name = raw_fname;
        }
        else
        {
            try
            {
                std::string fake = context.get_syms().invoke_func + raw_fname;
                auto [name, params] = parse_func_invokation(fake, context);
                new_expression.name = name;
                for (auto& p: params) new_expression.parameter_names.push_back(p);
            }
            catch (const std::exception& e)
            {
                throw sdf_exception(std::string("cannot parse expression definition: ") + e.what());
            }
        }

        //assert good things about the new expression
        auto disallow = [&](const auto& c)
        {
            if (new_expression.name.find(c) != std::string::npos)
                throw sdf_exception(std::string("disallowed character \'") + c + "\' in expression name \"" + new_expression.name + "\"");
            for (const auto& n: new_expression.parameter_names)
            {
                if (n.find(c) != std::string::npos)
                    throw sdf_exception(std::string("disallowed character \'") + c + "\' in expression parameter \"" + n + "\"");
            }
        };

        disallow(' ');
        disallow(context.get_syms().open_array);
        disallow(context.get_syms().close_array);
        disallow(context.get_syms().scope_operator);
        context.add_expression(new_expression);
    }
}