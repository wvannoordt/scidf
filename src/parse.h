#pragma once

#include "context.h"
#include "node.h"
#include "content_view.h"
#include "string_util.h"
#include "sdf_exception.h"
#include "extract_section_data.h"

#include <vector>
#include <tuple>
#include <string>

namespace scidf
{
    static void parse(const content_view& content, node_t& node, const context_t& parent_context)
    {
        auto [lines, subsections] = extract_section_data(content, parent_context);
        
        for (auto sub: subsections)
        {
            const std::string&  section_name    = sub.first;
            const content_view& section_content = sub.second;
            auto& new_node = node[section_name];
            parse(section_content, new_node, parent_context);
        }
    }
}