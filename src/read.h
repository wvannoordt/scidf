#pragma once

#include "node.h"
#include "context.h"
#include "clargs.h"
#include "parse.h"

#include <string>
#include <fstream>
#include <streambuf>
#include <tuple>
#include <optional>

namespace scidf
{
    namespace detail
    {
        static void filter_bad_chars(std::string& str)
        {
            const std::string nbsp = "\xC2\xA0"; // --> potential bad characters
            size_t pos = 0;
            while ((pos = str.find(nbsp, pos)) != std::string::npos) {
                // Replace the 2-byte NBSP with a 1-byte standard space (" ")
                str.replace(pos, nbsp.length(), " ");
                pos += 1; // Advance past the newly inserted standard space
            }
        }
    }
    
    static void read(const std::string& filename, node_t& node, const context_t& parent_context)
    {
        std::string file_contents = str::read_contents(filename);
        detail::filter_bad_chars(file_contents);
        content_view content(file_contents);
        parse(content, node, parent_context);
    }
    
    static void read(const std::string& filename, node_t& node)
    {
        context_t c;
        read(filename, node, c);
    }

    static void read(const std::string& filename, node_t& node, const clargs_t& args)
    {
        context_t c(args);
        read(filename, node, c);
    }
}