#pragma once

#include <vector>
#include <tuple>
#include <string>

#include "context.h"
#include "string_util.h"
#include "sdf_exception.h"

namespace scidf
{
    static std::tuple<std::string, std::vector<std::string>>
    parse_func_invokation(const std::string& line, const context_t& context)
    {
        std::size_t spos = std::string::npos, epos = std::string::npos;

        const std::string& raw = line;
        std::vector<std::string> args_out;
        int level = 0;
        int array_level = 0;
        bool string_escaped = false;
        std::string current_arg = "";
        for (std::size_t i = 0; i < raw.length(); ++i)
        {
            if (raw[i] == context.get_syms().open_array) array_level++;
            if (raw[i] == context.get_syms().close_array) array_level--;
            if (raw[i] == context.get_syms().open_string) string_escaped = !string_escaped;

            if (raw[i] == context.get_syms().open_arg  && !string_escaped)
            {
                if (level == 0) spos = i;
                level++;
            }
            if (raw[i] == context.get_syms().close_arg && !string_escaped)
            {
                level--;
                if (level == 0) epos = i;
            }

            if ((raw[i] == context.get_syms().argument_separator) && level == 1 && array_level == 0)
            {
                args_out.push_back(str::trim(current_arg, context.get_syms().whitespace));
                current_arg = "";
            }
            bool add_char = (level > 1);
            add_char = add_char || ((level == 1) && (raw[i] != context.get_syms().open_arg) && !(raw[i] == context.get_syms().argument_separator));
            add_char = add_char || array_level > 0;
            
            if (add_char) current_arg += raw[i];

            if (level < 0) throw sdf_exception("unbalanced argument delimiters (close-arg)");
        }
        args_out.push_back(str::trim(current_arg, context.get_syms().whitespace));
        if (spos == std::string::npos) throw sdf_exception("cannot invoke function name without arguments");
        std::size_t atpos = raw.find_first_of(context.get_syms().invoke_func);
        if (atpos == std::string::npos) throw sdf_exception("ill-formatted function invocation");
        std::size_t start = atpos + 1;
        std::size_t end   = spos;
        if (end < start) throw sdf_exception("cannot parse function name");
        std::string fname = raw.substr(start, end - start);
        if (level != 0) throw sdf_exception("unbalanced argument delimiters (open-arg)");
        return std::make_tuple(str::trim(fname, context.get_syms().whitespace), args_out);
    }
}