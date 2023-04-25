#pragma once

#include <stdexcept>
#include "content_view.h"

namespace scidf
{
    struct sdf_exception : public std::exception
    {
        std::string message;
        sdf_exception(const std::string& message_in) : message{message_in} {}
        const char* what() const throw()
        {
            return message.c_str();
        }
    };

    struct sdf_content_exception : public std::exception
    {
        std::string error_message;
        sdf_content_exception(const content_view& content, const std::size_t relative_pos, const std::string& message_in)
        {
            error_message = "scidf content exception: " + message_in;
            auto [line_number, line] = content.seek_line_at(relative_pos);
            error_message += "\nLocation: \nLine " + std::to_string(line_number) + ":  -->  " + line;
        }
        const char* what() const throw()
        {
            return error_message.c_str();
        }
    };
}