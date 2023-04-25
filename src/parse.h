#pragma once

#include "context.h"
#include "node.h"
#include "content_view.h"
#include "string_util.h"
#include "sdf_exception.h"

#include <vector>
#include <tuple>
#include <string>

namespace scidf
{
    namespace detail
    {
        static std::vector<std::pair<std::size_t, std::size_t>>
        seek_section_delimiters(const content_view& content, const context_t& context)
        {
            std::size_t start, end, last_quote, last_open, last_close;
            int level = 0;
            std::vector<std::pair<std::size_t, std::size_t>> output;
            //Things that can escape the section delimiters: strings
            bool escaped = false;
            for (std::size_t i = 0; i < content.length(); ++i)
            {
                if (content[i] == context.get_syms().open_string)
                {
                    last_quote = i;
                    escaped    = !escaped;
                }
                bool open_detected  = (content[i] == context.get_syms().open_section);
                bool close_detected = (content[i] == context.get_syms().close_section);
                if (open_detected  && !escaped)
                {
                    level++;
                    last_open  = i;
                }
                if (close_detected && !escaped)
                {
                    level--;
                    last_close = i;
                }
                if (open_detected  && (level == 1) && !escaped) start = i;
                if (close_detected && (level == 0) && !escaped)
                {
                    end = i;
                    output.push_back({start, end});
                }
                if (level < 0)
                {
                    throw sdf_content_exception(content, i, "unmatched close-section brace");
                }
            }
            if (escaped)
            {
                throw sdf_content_exception(content, last_quote, "unterminated string delimiter");
            }
            if (level > 0)
            {
                throw sdf_content_exception(content, last_open, "unterminated section");
            }
            return output;
        }

        static std::tuple<std::string, std::size_t>
        get_section_name_at(const content_view& content, const std::size_t& brace_index, const context_t& context)
        {
            std::size_t pos = brace_index;
            if (pos == 0) throw sdf_content_exception(content, pos, "unnamed section");
            auto exclude_section_name = [&](char i)
            {
                return (i==context.get_syms().line_break)
                    || (i==context.get_syms().terminate)
                    || (i==context.get_syms().open_string)
                    || (i==context.get_syms().close_section)
                    || (i==context.get_syms().open_section)
                    || (context.get_syms().whitespace.find(i) != std::string::npos);
            };
            std::size_t epos = brace_index;
            while ((epos > 0) &&  exclude_section_name(content[epos])) epos--;
            pos = epos;
            while ((pos > 0) && !exclude_section_name(content[pos])) pos--;
            epos++;
            if (exclude_section_name(content[pos])) pos++;
            return std::make_tuple(content.substr(pos, epos-pos), pos);
        }

        static void
        add_lines(std::vector<std::string>& lines, const content_view& content, const context_t& context)
        {
            auto is_dlm = [&](char i) { return (i==context.get_syms().terminate) || (i==context.get_syms().line_break);};
            std::string current = "";
            auto is_whitespace = [&](const std::string& stuff)
            {
                for (std::size_t u = 0; u < stuff.length(); ++u)
                {
                    if (context.get_syms().whitespace.find(stuff[u]) == std::string::npos) return false;
                }
                return true;
            };
            auto addline = [&](const std::string& str)
            {
                if (!is_whitespace(str))
                lines.push_back(str::trim(str, context.get_syms().whitespace));
            };
            for (std::size_t j = 0; j < content.length(); ++j)
            {
                if  (!is_dlm(content[j]))
                {
                    if (content[j] != context.get_syms().line_continue) current += content[j];
                }
                else
                {
                    bool skip = false;
                    if (j > 0) skip = (content[j-1] == context.get_syms().line_continue);
                    if (!skip)
                    {
                        addline(current);
                        current = "";
                    }
                }
            }
            addline(current);
        }
    }

    static
    std::tuple<std::vector<std::string>, std::vector<std::pair<std::string, content_view>>>
    extract_data(const content_view& content, const context_t& context)
    {
        std::vector<std::string> lines;
        std::vector<std::pair<std::string, content_view>> subsections;
        std::vector<std::pair<std::size_t, std::size_t>> positions = detail::seek_section_delimiters(content, context);
        for (auto& p: positions)
        {
            auto [section_name, location] = detail::get_section_name_at(content, p.first, context);
            subsections.push_back({section_name, content.subview(p.first+1, p.second)});

            //We update the open brace positions now to include the section name!
            p.first = location;
            p.second++;
        }
        std::vector<std::pair<std::size_t, std::size_t>> line_datas;
        std::size_t start = 0;
        std::size_t end   = content.length();
        for (auto p:positions)
        {
            end = p.first;
            line_datas.push_back({start, end});
            start = p.second;
        }
        end = content.length();
        line_datas.push_back({start, end});
        for (auto l: line_datas)
        {
            auto subv = content.subview(l.first, l.second);
            detail::add_lines(lines, subv, context);
        }
        return std::make_tuple(lines, subsections);
    }

    static void parse(const content_view& content, node_t& node, const context_t& parent_context)
    {
        auto [lines, subsections] = extract_data(content, parent_context);
        for (auto pp: lines) print(pp);
        // for (auto pp: subsections)
        // {
        //     print(pp.first);
        //     print(pp.second.length(), "==>", pp.second);
        // }
    }
}