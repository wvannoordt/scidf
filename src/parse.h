#pragma once

#include "context.h"
#include "node.h"
#include "content_view.h"
#include "string_util.h"
#include "sdf_exception.h"
#include "extract_section_data.h"
#include "func_invocation.h"
#include "import.h"
#include "resolve.h"
#include "imperative.h"

#include <vector>
#include <tuple>
#include <string>

namespace scidf
{
    enum line_type
    {
        //used for the assignment of a variable, expecting a name and a value
        line_assignment,

        //used for manipulating the state of the context (e.g. definitions)
        line_imperative,

        //imports another file's data, expects a wildcard symbol
        line_import,

        //used to indicate an invalid line
        line_invalid
    };

    static std::string line_type_str(const line_type& t)
    {
        switch(t)
        {
            case line_assignment: return "line_assignment";
            case line_imperative: return "line_imperative";
            case line_import:     return "line_import";
            case line_invalid:    return "line_invalid";
            default: return "[INVALID]";
        }
    }

    //line-parsing strategy: check for the basic markers of each line, and if they are there, it is classified as such a line.
    //If a classification happens, parse it as the right type of line and base error messages on that.

    static line_type classify_line(const std::pair<std::string, std::size_t>& line_data, const context_t& context)
    {
        const std::string& line = line_data.first;
        if (line.length() < 1) return line_invalid;
        if (line[0] == context.get_syms().invoke_func)
        {
            std::size_t open_func_pos = line.find(context.get_syms().open_arg);
            if (open_func_pos != std::string::npos)
            {
                const std::string sub = line.substr(0, open_func_pos);
                if (sub.find(context.get_syms().import_symbol) != std::string::npos) return line_import;
            }
            return line_imperative;
        }
        if (line.find(context.get_syms().assignment) != std::string::npos) return line_assignment;
        return line_invalid;
    }

    static std::tuple<std::string, std::string>
    parse_assignment(const std::pair<std::string, std::size_t>& line, const context_t& context)
    {
        std::size_t eq_pos = line.first.find_first_of(context.get_syms().assignment);
        if (eq_pos == std::string::npos) throw sdf_line_exception(line, "missing assignment symbol in assignment statement");
        if (eq_pos == line.first.length()-1) throw sdf_line_exception(line, "empty assignment");
        std::string name = str::trim(line.first.substr(0,eq_pos), context.get_syms().whitespace);
        std::string rhs  = str::trim(line.first.substr(eq_pos+1,line.first.length()-1-eq_pos), context.get_syms().whitespace);
        if (rhs.length() == 0) throw sdf_line_exception(line, "empty assignment");
        return std::make_pair(name, rhs);
    }

    static void parse(const content_view& content, node_t& node, const context_t& parent_context)
    {
        auto [lines, subsections] = extract_section_data(content, parent_context);
        std::vector<line_type> line_types;
        for (const auto& l:lines)
        {
            line_types.push_back(classify_line(l, parent_context));
        }

        context_t child_context = parent_context;

        std::string invalid_line_message = "Detected instance(s) of invalid line content:\n";
        int invalid_count = 0;
        for (std::size_t i = 0; i < line_types.size(); ++i)
        {
            switch (line_types[i])
            {
                case line_assignment:
                {
                    auto [name, raw_value] = parse_assignment(lines[i], child_context);
                    if (node[name].has_value()) throw sdf_line_exception(lines[i], "attempted duplicate assignment");
                    //Todo: resolve the name here if there is a function invocation
                    node[name] = resolve(raw_value, child_context);
                    break;
                }
                case line_imperative:
                {
                    auto [func_name, func_args] = parse_func_invokation(lines[i], child_context);
                    try
                    {
                        execute_imperative(func_name, func_args, child_context);
                    }
                    catch (const std::exception& e)
                    {
                        throw sdf_line_exception(lines[i], "error executing imperative \"" + func_name + "\":\n" + std::string(e.what()));
                    }
                    //todo
                    break;
                }
                case line_import:
                {
                    auto [func_name, func_args] = parse_func_invokation(lines[i], child_context);
                    if (func_name != child_context.get_syms().import_symbol)
                        throw sdf_line_exception(lines[i], "paradox: parse import as " + func_name);
                    //Todo: resolve names here if need be
                    if (func_args.size() == 0) throw sdf_line_exception(lines[i], "cannot call import with no arguments ");
                    for (const auto& arg: func_args)
                    {
                        //function arguments are treated as resolved here
                        auto part_names = str::split(arg, child_context.get_syms().scope_operator);
                        if (part_names.size() == 0) throw sdf_line_exception(lines[i], "invalid argument to \"" + func_name + "\"");
                        //todo: a better job of this
                        const std::string sdf_name = part_names[0] + child_context.get_syms().file_extension;
                        //todo: replace with std::expected when the cows freeze over
                        std::string filename;
                        if (!child_context.path_search(sdf_name, filename))
                            throw sdf_line_exception(lines[i], "cannot find file \"" + sdf_name + "\"... missing path?");
                        const std::string file_contents = str::read_contents(filename);
                        content_view import_content(file_contents);
                        node_t imported;
                        try
                        {
                            parse(import_content, imported, child_context);
                            import(node, imported, arg, child_context);
                        }
                        catch (const std::exception& e)
                        {
                            throw sdf_line_exception(lines[i], "Error when attempting to import \"" + sdf_name + "\" at \"" + filename + "\":\n" + std::string(e.what()));
                        }
                    }
                    break;
                }
                case line_invalid:
                {
                    const std::string line_content = lines[i].first;
                    const std::size_t line_number  = lines[i].second;
                    invalid_count++;
                    invalid_line_message += "Attempted parse on line " + std::to_string(line_number) + ": --> " + line_content + "\n";
                    break;
                }
            }
        }
        if (invalid_count>0)
        {
            throw sdf_exception(invalid_line_message);
        }

        for (auto sub: subsections)
        {
            const std::string&  section_name    = sub.first;
            const content_view& section_content = sub.second;
            auto& new_node = node[section_name];
            parse(section_content, new_node, parent_context);
        }
    }
}