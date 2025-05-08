#pragma once

#include <vector>
#include <tuple>
#include <set>
#include <string>
#include "node.h"
#include "resolve.h"

namespace scidf
{
    static std::tuple<std::string, std::string>
    get_next_wildcard(const std::string& current_wildcard, const context_t& context)
    {
        if (current_wildcard.length() == 0) return std::make_tuple(std::string(""), std::string(""));
        std::size_t dlm_pos = current_wildcard.find_first_of(context.get_syms().scope_operator);
        if (dlm_pos == std::string::npos) return std::make_tuple(std::string(current_wildcard), std::string(""));
        const std::string next_root = current_wildcard.substr(0, dlm_pos);
        if (dlm_pos == current_wildcard.length() - 1) return std::make_tuple(std::string(next_root), std::string(""));
        const std::string next_wildcard = current_wildcard.substr(dlm_pos + 1, current_wildcard.length() - dlm_pos - 1);
        return std::make_tuple(next_root, next_wildcard);
    }

    static bool wildcard_match(const node_t& candidate, const std::string& pattern, const context_t& context)
    {
        // print("<" + candidate.name + ">", "<"+pattern+">");
        if (pattern.length() == 0) return true;
        if (pattern.find(context.get_syms().open_array) != std::string::npos)
        {
            std::vector<std::string> all_wildcards;
            iconversion_t icnv(pattern);
            try
            {
                icnv >> all_wildcards;
                for (const auto& w: all_wildcards)
                {
                    if (wildcard_match(candidate, w, context)) return true;
                }
                return false;
            }
            catch (const std::exception& e)
            {
                throw sdf_exception("bad wildcard parse: " + std::string(e.what()));
            }
        }
        std::size_t star_pos = pattern.find(context.get_syms().wildcard_any);
        if (star_pos != std::string::npos)
        {
            throw sdf_exception("wildcard match not yet fully implemented");
        }
        return candidate.name == pattern;
    }

    static void import(node_t& dest, const node_t& external, const std::string& wildcard_designator, const context_t& context)
    {
        const auto [root, next_wildcard] = get_next_wildcard(wildcard_designator, context);
        for (const auto& c: external.children)
        {
            if (wildcard_match(c.second, root, context))
            {
                if (dest.contains_child(c.second.get_name()))
                {
                    throw sdf_exception("attempted to override \"" + c.second.get_name() + "\"");
                }
                if (c.second.is_terminal())
                {
                    dest[c.second.get_name()].set_value(c.second.get_value());
                }
                else
                {
                    import(dest[c.second.get_name()], c.second, next_wildcard, context);
                }
            }
        }
    }
    
    static void copy_values(node_t& dest, const node_t& external, std::set<std::string> disallow_dst, std::set<std::string> disallow_src)
    {
        if (disallow_dst.find(dest.get_path())     != disallow_dst.end()) return; // prevent infinite recursion
        if (disallow_src.find(external.get_path()) != disallow_src.end()) return; // prevent infinite recursion
        for (const auto& c: external.children)
        {
            if (c.second.is_terminal() && !(dest.contains_child(c.first))) dest[c.first] = c.second.value;
            else copy_values(dest[c.first], c.second, disallow_dst, disallow_src);
        }
    }
    
    static void copy_values(node_t& dest, const node_t& external)
    {
        copy_values(dest, external, {external.get_path()}, {});
    }
}