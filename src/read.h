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
        std::ifstream t(filename);
        std::string str;
        t.seekg(0, std::ios::end);   
        str.reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        content_view content(str);
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