#pragma once

#include <filesystem>
#include "clargs.h"

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
        char assignment                = '=';
        char argument_separator        = ',';
        std::string scope_operator     = ".";
        std::string file_extension     = ".sdf";
        std::string import_symbol      = "import";
        std::string path_append        = "addpath";

    };

    struct context_t
    {
        std::vector<std::filesystem::path> paths;
        syms_t syms;
        context_t()
        {
            add_default_paths();
        }
        context_t(const clargs_t& clargs)
        {
            add_default_paths();
        }

        void add_default_paths()
        {
            paths.push_back(".");
        }

        void add_path(const std::filesystem::path& path)
        {
            paths.push_back(path);
        }

        const syms_t& get_syms() const {return syms;}

        bool path_search(const std::string& sdf_name, std::string& filename_out) const
        {
            //sdf_name will be of the form "xxyy.sdf"
            for (const auto& pth: paths)
            {
                auto srch = pth / sdf_name;
                if (std::filesystem::exists(srch) && !std::filesystem::is_directory(srch))
                {
                    filename_out = std::string(srch);
                    return true;
                }
            }
            return false;
        }
    };
}