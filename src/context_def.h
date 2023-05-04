#pragma once

#include <tuple>
#include <string>
#include <vector>
#include "context.h"

namespace scidf
{
    static void context_def(const std::pair<std::string, std::size_t>& line, context_t context)
    {
        //allows for the definition of a function
        const std::string& raw = line.first;
        if (raw[0] != context.get_syms().invoke_var) throw sdf_exception("no variable invocation symbol");
        std::size_t i_assign = raw.find_first_of(context.get_syms().assignment);
        if (i_assign == std::string::npos || i_assign == 0) throw sdf_exception("cannot find assignment symbol");

        std::string raw_fname = str::trim(raw.substr(1, i_assign - 1), context.get_syms().whitespace);

        i_assign ++;
        if (i_assign >= raw.length()-1) throw sdf_exception("empty variable assignment");
        std::string raw_rhs   = str::trim(raw.substr(i_assign, raw.length() - i_assign), context.get_syms().whitespace);
        print(raw_fname);
        print(raw_rhs);
        expression_t new_expression;
        new_expression.content = raw_rhs;
        new_expression.parameter_names.clear();
        new_expression.name = "unnamed_expression";
        context.add_expression(new_expression);
    }
}