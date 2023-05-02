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
    static void read(const std::string& filename, node_t& node, const context_t& parent_context)
    {
        const std::string file_contents = str::read_contents(filename);
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