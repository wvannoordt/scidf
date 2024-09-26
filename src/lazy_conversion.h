#pragma once

#include "sdf_exception.h"
#include "node.h"
#include <concepts>
#include <optional>

namespace scidf
{
    template <typename value_t> struct lazy_conversion_t
    {
        const value_t default_value;
        const node_t& node;

        lazy_conversion_t(const value_t& value_in, const node_t& node_in)
        : default_value{value_in}, node{node_in} {}

        operator value_t() const
        {
            if (node.has_value())
            {
                node.extracted_value = true;
                return value_t(node);
            }
            else                  return value_t(default_value);
        }

        template <typename condition_t>
        auto operator >> (const condition_t& cond) const
        {
            try
            {
                value_t out = (*this);
                if (!cond.verify(out)) throw sdf_exception("failed conditional evaluation:\n" + cond.get_message());
                return out;
            }
            catch (const std::exception& e)
            {
                throw sdf_exception("bad conditional evaluation of \"" + node.get_path() + std::string("\" with value \"" + node.get_value() + "\": ") + e.what());
            }
        }
    };
}