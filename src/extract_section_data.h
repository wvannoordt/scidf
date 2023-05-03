#pragma once


#include "context.h"
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
            std::size_t start, end, last_quote, last_open, last_close, last_open_comment;
            int level = 0;
            std::vector<std::pair<std::size_t, std::size_t>> output;
            //Things that can escape the section delimiters: strings, comments
            bool string_escaped        = false;
            bool short_comment_escaped = false;
            bool long_comment_escaped  = false;
            for (std::size_t i = 0; i < content.length(); ++i)
            {
                if (content[i] == context.get_syms().open_string && !(short_comment_escaped || long_comment_escaped))
                {
                    last_quote = i;
                    string_escaped = !string_escaped;
                }

                bool open_detected  = (content[i] == context.get_syms().open_section);
                bool close_detected = (content[i] == context.get_syms().close_section);

                if (str::str_starts_at(content, i, context.get_syms().start_long_comment) && !long_comment_escaped)
                {
                    last_open_comment = i;
                    long_comment_escaped = true;
                }
                if (str::str_ends_at  (content, i, context.get_syms().end_long_comment))
                {
                    if (!long_comment_escaped) throw sdf_content_exception(content, i, "unmatched long-comment delimiter");
                    long_comment_escaped = false;
                }

                if (str::str_starts_at(content, i, context.get_syms().line_comment)) short_comment_escaped = true;
                if (content[i] == context.get_syms().line_break) short_comment_escaped = false;

                bool escaped = string_escaped || short_comment_escaped || long_comment_escaped;

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
            if (string_escaped)
            {
                throw sdf_content_exception(content, last_quote, "unterminated string delimiter");
            }
            if (long_comment_escaped)
            {
                throw sdf_content_exception(content, last_open_comment, "unterminated long-comment");
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
        append_lines(
            std::vector<std::pair<std::string, std::size_t>>& lines,
            const content_view& content,
            const context_t& context)
        {
            auto is_dlm = [&](char i) { return (i==context.get_syms().terminate) || (i==context.get_syms().line_break);};
            std::string current = "";
            std::size_t last_push_char = 0;
            auto is_whitespace = [&](const auto& stuff)
            {
                for (std::size_t u = 0; u < stuff.length(); ++u)
                {
                    if (context.get_syms().whitespace.find(stuff[u]) == std::string::npos) return false;
                }
                return true;
            };
            auto addline = [&](const std::string& str, const std::size_t char_number)
            {
                if (!is_whitespace(str))
                {
                    const auto [line, line_str] = content.seek_line_at(char_number);
                    lines.push_back({str::trim(str, context.get_syms().whitespace), line});
                }
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
                        addline(current, last_push_char);
                        current = "";
                        last_push_char = j+1;
                        if (last_push_char >= content.length()) last_push_char = content.length();
                    }
                }
            }
            addline(current, last_push_char);
        }
    }

    static std::vector<std::pair<std::string, std::size_t>>
    clean_lines(const std::vector<std::pair<std::string, std::size_t>>& raw, const context_t& context)
    {
        std::vector<std::pair<std::string, std::size_t>> output;
        std::size_t line_source_index = 0;
        if (raw.size() == 0) return output;
        std::size_t current_line = raw[0].second;
        std::string concat = raw[0].first;
        for (std::size_t i = 1; i < raw.size(); ++i) concat += (context.get_syms().terminate+raw[i].first);
        std::string current_str = "";
        bool string_escaped = false;
        bool long_comment_escaped  = false;
        bool short_comment_escaped = false;
        auto is_whitespace = [&](const auto& stuff)
        {
            for (std::size_t u = 0; u < stuff.length(); ++u)
            {
                if (context.get_syms().whitespace.find(stuff[u]) == std::string::npos) return false;
            }
            return true;
        };
        auto addline = [&](const std::string& line, const std::size_t& line_number)
        {
            if (!is_whitespace(line))
            {
                output.push_back({line, line_number});
            }
        };
        for (std::size_t j = 0; j < concat.size(); ++j)
        {
            char c = concat[j];
            if (c == context.get_syms().open_string && !long_comment_escaped && !short_comment_escaped) string_escaped = !string_escaped;
            bool linebreak = (c==context.get_syms().terminate);
            if (str::str_starts_at(concat, j, context.get_syms().line_comment)) short_comment_escaped = true;
            if (linebreak && short_comment_escaped) short_comment_escaped = false;
            if (str::str_starts_at(concat, j, context.get_syms().start_long_comment)) long_comment_escaped = true;
            

            if (!(short_comment_escaped || long_comment_escaped))
            {
                if (!linebreak) current_str += c;
                if (linebreak && string_escaped) current_str += '\n';
            }
            if (linebreak) line_source_index++;
            if (linebreak && !string_escaped && !long_comment_escaped)
            {
                addline(current_str, current_line);
                current_str = "";
                current_line = raw[line_source_index].second;
            }
            if (str::str_ends_at  (concat, j, context.get_syms().end_long_comment))   long_comment_escaped = false;
        }
        addline(current_str, current_line);
        return output;
    }

    static
    std::tuple<std::vector<std::pair<std::string, std::size_t>>, std::vector<std::pair<std::string, content_view>>>
    extract_section_data(const content_view& content, const context_t& context)
    {
        std::vector<std::pair<std::string, std::size_t>> lines;
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
            detail::append_lines(lines, subv, context);
        }
        std::vector<std::pair<std::string, std::size_t>> cleaned_lines = clean_lines(lines, context);
        return std::make_tuple(cleaned_lines, subsections);
    }
}