#pragma once

#include "clargs.h"

namespace scidf
{
    struct syms_t
    {
        char open_section      = '{';
        char close_section     = '}';
        char open_arg          = '(';
        char close_arg         = ')';
        char open_array        = '[';
        char close_array       = ']';
        std::string whitespace = " \t\r";
        char line_break        = '\n';
        char open_string       = '\"';
        char terminate         = ';';
        char line_continue     = '\\';
    };

    struct context_t
    {
        syms_t syms;
        context_t(){}
        context_t(const clargs_t& clargs)
        {
            
        }
        const syms_t& get_syms() const {return syms;}
    };
}