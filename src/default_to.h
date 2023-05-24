#pragma once

#include "node.h"
#include "lazy_conversion.h"

namespace scidf
{
    template <typename value_t>
    static lazy_conversion_t<value_t> default_to(const value_t& value, const node_t& node)
    {
        return lazy_conversion_t(value, node);
    }

    template <typename out_t>
    static lazy_conversion_t<out_t> required(const node_t& node)
    {
        if (!node.has_value()) throw sdf_exception("cannot find required parameter \"" + node.get_path() + "\"");
        return lazy_conversion_t(out_t(), node);
    }
}