#pragma once

#include <fstream>

#include "node.h"

namespace scidf
{
    namespace detail
    {
        static void recurse_write_node(std::ostream& os, const node_t& node, int rlev)
        {
            std::string tab(4*rlev, ' ');
            for (const auto& [name, child] : node.children)
            {
                if (child.is_terminal() && child.assigned_value)
                {
                    os << tab << name << " = " << child.get_value() << "\n";
                }
                else if (!child.children.empty())
                {
                    os << tab << name << "\n";
                    os << tab << "{" << "\n";
                    recurse_write_node(os, child, rlev + 1);
                    os << tab << "}" << "\n";
                }
            }
        }
    }
    
    static void write(const std::string& fname, const node_t& node)
    {
        std::ofstream ofile(fname);
        detail::recurse_write_node(ofile, node, 0);
    }
}
