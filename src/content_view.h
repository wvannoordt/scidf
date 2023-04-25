#pragma once

#include <string>
#include <tuple>
#include <optional>

namespace scidf
{
    struct content_view
    {
        const std::string& raw;
        std::size_t start, end;
        content_view(const std::string& raw_in, std::size_t start_in, std::size_t end_in)
        : raw{raw_in}, start{start_in}, end{end_in} {}
        content_view(const std::string& raw_in)
        : raw{raw_in}, start{0}, end{raw_in.length()} {}

        std::string to_string() const { return raw.substr(start, length()); }
        std::size_t length() const {return end-start;}
        const auto& operator[] (const std::size_t& i) const { return raw[i+start]; }
        auto&       operator[] (const std::size_t& i)       { return raw[i+start]; }
        std::size_t absolute(const std::size_t& i)    const { return i+start; }

        content_view subview(const std::size_t& new_start, const std::size_t& new_end) const
        {
            return content_view(raw, absolute(new_start), absolute(new_end));
        }
        std::string substr(const std::size_t& start, const std::size_t& len) const {return raw.substr(absolute(start), len);}

        std::tuple<std::size_t, std::string> seek_line_at(const std::size_t& relative_pos) const
        {
            std::size_t line_number;
            std::string line;
            std::size_t abs = absolute(relative_pos);
            if (abs > raw.length()) return std::make_tuple(0, "[FATAL ERROR]");
            std::size_t line_start = abs;
            std::size_t line_end   = abs;
            while (line_end   < raw.length() && raw[line_end  ] != '\n') line_end++;
            while (line_start > 0            && raw[line_start] != '\n') line_start--;
            if (raw[line_start] == '\n') line_start++;
            line = raw.substr(line_start, line_end - line_start);
            line_number = 1;
            while (abs --> 0) line_number += raw[abs]=='\n'?1:0;
            return std::make_pair(line_number, line);
        };
    };

    std::ostream& operator << (std::ostream& os, const content_view& content)
    {
        os << content.to_string();
        return os;
    }
}