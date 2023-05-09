#pragma once

#include <filesystem>
#include <functional>
#include "clargs.h"
#include "expression.h"
#include "sdf_exception.h"
#include "native_functions.h"
#include "syms.h"
namespace scidf
{
    struct context_t
    {
        using func_type = std::function<std::string(const std::vector<std::string>&)>;
        std::map<std::string, func_type> functions;
        std::vector<std::filesystem::path> paths;
        std::map<std::string, expression_t> expressions;
        const syms_t syms{glob_syms};

        const context_t* parent = nullptr;

        context_t()
        {
            parent = nullptr;
            add_defaults();
        }
        context_t(const clargs_t& clargs)
        {
            parent = nullptr;
            add_defaults();
            for (const auto& p: clargs.raw)
            {
                if (str::str_starts_at(p, 0, syms.cli_def))
                {
                    std::size_t start = syms.cli_def.length();
                    std::size_t end   = p.length();
                    std::string content = p.substr(start, end - start);
                    std::size_t eq_pos = content.find_first_of(syms.assignment);
                    std::string var_content = "";
                    std::string name = "";
                    if (eq_pos == std::string::npos)
                    {
                        name        = content;
                        var_content = "";
                    }
                    else if (eq_pos >= content.length() - 1)
                    {
                        name        = content.substr(0, eq_pos);;
                        var_content = "";
                    }
                    else
                    {
                        name        = content.substr(0, eq_pos);
                        var_content = content.substr(eq_pos + 1, content.length() - eq_pos - 1);
                    }
                    expression_t cli_exp;
                    cli_exp.content = var_content;
                    cli_exp.name = name;
                    add_expression(cli_exp);
                }
            }
        }

        context_t fork() const
        {
            context_t output;
            output.functions = functions;
            output.paths     = paths;
            output.parent    = this;
            return output;
        }

        void add_defaults()
        {
            paths.push_back(".");
            add_native_functions(functions);
        }

        const auto& find_function(const std::string& name) const
        {
            if (functions.find(name) == functions.end())
                throw sdf_exception("cannot find requested function \"" + name + "\"");
            return functions.at(name);
        }

        void add_expression(const expression_t& new_expression)
        {
            if (expressions.find(new_expression.name) != expressions.end())
            {
                std::string message= "attempted duplicate expression definition \"" + new_expression.name + "\":";
                message += "\nOriginal definition:    " + expressions[new_expression.name].content;
                message += "\nConflicting definition: " + new_expression.content;
                throw sdf_exception(message);
            }
            expressions.insert({new_expression.name, new_expression});
        }

        const expression_t& get_expression(const std::string& name) const
        {
            if (expressions.find(name) == expressions.end())
            {
                if (parent == nullptr)
                    throw sdf_exception("attempted to lookup undefined expression \"" + name + "\"");
                else return parent->get_expression(name);
            }
            else return expressions.at(name);
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