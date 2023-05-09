#pragma once

#include <string>

namespace scidf
{
    struct syms_t
    {
        char open_section              = '{';
        char close_section             = '}';
        char open_arg                  = '(';
        char close_arg                 = ')';
        char open_array                = '[';
        char close_array               = ']';
        std::string whitespace         = " \t\r";
        char line_break                = '\n';
        char open_string               = '\"';
        char terminate                 = ';';
        char line_continue             = '\\';
        std::string line_comment       = "//";
        std::string start_long_comment = "/*";
        std::string end_long_comment   = "*/";
        char invoke_func               = '@';
        char invoke_var                = '$';
        char assignment                = '=';
        char argument_separator        = ',';
        std::string scope_operator     = ".";
        std::string file_extension     = ".sdf";
        std::string import_symbol      = "import";
        std::string path_append        = "addpath";
        std::string builtin_invoke     = ".invoke_var";
        std::string cli_def            = "-D";
    };

    static syms_t glob_syms;
}