#pragma once

#include "context.h"
#include "sdf_exception.h"
#include "string_util.h"
#include <string>
#include <vector>

namespace scidf
{
    static std::vector<std::string>
    extract_vector(const std::string& raw, const syms_t& syms)
    {
        if (raw.length() < 2) throw sdf_exception("string value too short");
        if (raw[0] != syms.open_array)               throw sdf_exception(std::string("missing vector open symbol \"") + syms.open_array  + "\" as first character");
        if (raw[raw.length()-1] != syms.close_array) throw sdf_exception(std::string("missing vector open symbol \"") + syms.close_array + "\" as final character");

        int level = 0;
        bool string_escaped = false;
        std::string current = "";
        std::vector<std::string> args;
        for (std::size_t j = 0; j < raw.length(); ++j)
        {
            if (raw[j] == syms.open_string) string_escaped = !string_escaped;
            if (level < 0) throw sdf_exception("unmatched close-array symbol");

            if (raw[j] == syms.close_array && !string_escaped) level --;
            if ((level == 1) && (raw[j] == syms.argument_separator) && !string_escaped)
            {
                args.push_back(str::trim(current, syms.whitespace));
                current = "";
            }
            else if (level >= 1)
            {
                current += raw[j];
            }
            if (raw[j] == syms.open_array  && !string_escaped) level ++;
        }
        if (level != 0) throw sdf_exception("unmatched open-array symbol");
        args.push_back(str::trim(current, syms.whitespace));
        return args;
    }
}