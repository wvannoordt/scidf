#pragma once

#include <vector>
#include <tuple>
#include <string>
#include "node.h"
#include "resolve.h"

namespace scidf
{
    static void import(node_t& dest, const node_t& external, const std::string& wildcard_designator, const context_t& context)
    {
        for (const auto& c: external.children)
        {
            if (dest.contains_child(c.second.get_name()))
            {
                throw sdf_exception("attempted to override \"" + c.second.get_name() + "\"");
            }
            if (c.second.is_terminal())
            {
                dest[c.second.get_name()] = c.second.get_value();
            }
            else
            {
                import(dest[c.second.get_name()], c.second, wildcard_designator, context);
            }
        }
    }
}