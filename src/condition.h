#pragma once

#include "sdf_exception.h"
#include <filesystem>

namespace scidf
{
    template <typename eval_t> struct lazy_condition_t
    {
        constexpr static bool scidf_value_verifiable_tag = true;
        const eval_t eval; //todo: check if can hold by reference here!
        bool inverse = false;

        lazy_condition_t(const eval_t& eval_in)
        : eval{eval_in} {}

        bool verify(const auto& val) const
        {
            return (inverse == !eval(val));
        }

        lazy_condition_t operator! () const
        {
            lazy_condition_t out = (*this);
            out.inverse = !out.inverse;
            return out;
        }
    };

    enum scidf_binary_op
    {
        binary_and,
        binary_or
    };

    template <typename lhs_t, typename rhs_t, const scidf_binary_op operation>
    requires (lhs_t::scidf_value_verifiable_tag && rhs_t::scidf_value_verifiable_tag)
    struct binary_op_t
    {
        constexpr static bool scidf_value_verifiable_tag = true;
        const lhs_t& lhs;
        const rhs_t& rhs;
        bool inverse = false;
        bool verify(const auto& val) const
        {
            bool ll = lhs.verify(val);
            bool rr = rhs.verify(val);
            if constexpr (operation == binary_and) return (!inverse) == (ll && rr);
            if constexpr (operation == binary_or ) return (!inverse) == (ll || rr);
            return false;
        }

        binary_op_t operator! () const
        {
            binary_op_t out = (*this);
            out.inverse = !out.inverse;
            return out;
        }
    };

    template <typename lhs_t, typename rhs_t>
    requires (lhs_t::scidf_value_verifiable_tag && rhs_t::scidf_value_verifiable_tag)
    auto operator &&(const lhs_t& lhs, const rhs_t& rhs)
    {
        return binary_op_t<lhs_t, rhs_t, binary_and>{lhs, rhs, false};
    }

    template <typename lhs_t, typename rhs_t>
    requires (lhs_t::scidf_value_verifiable_tag && rhs_t::scidf_value_verifiable_tag)
    auto operator ||(const lhs_t& lhs, const rhs_t& rhs)
    {
        return binary_op_t<lhs_t, rhs_t, binary_or >{lhs, rhs, false};
    }

    template <typename eval_t> static auto condition(const eval_t& eval)
    {
        return lazy_condition_t(eval);
    }

    template <typename eval_t> static auto condition(const eval_t& eval, const std::string& message)
    {
        return lazy_condition_t(eval);
    }

    template <typename value_t>
    static auto greater_than(const value_t& val)
    { return condition([=](const value_t& v){return v >  val;}, "value must be greater than " + std::to_string(val));}

    template <typename value_t>
    static auto greater_than_or_equal_to(const value_t& val)
    { return condition([=](const value_t& v){return v >= val;}, "value must be greater than or equal to " + std::to_string(val));}

    template <typename value_t>
    static auto less_than(const value_t& val)
    { return condition([=](const value_t& v){return v <  val;}, "value must be less than " + std::to_string(val));}

    template <typename value_t>
    static auto less_than_or_equal_to(const value_t& val)
    { return condition([=](const value_t& v){return v <= val;}, "value must be less than or equal to " + std::to_string(val));}

    static auto even = condition([](const auto& v){return (v % 2) == 0;}, "value must be even");

    static auto odd  = condition([](const auto& v){return (v % 2) == 1;}, "value must be odd");

    static auto exists        = condition([](const auto& v){return std::filesystem::exists(std::string(v));}, "value must be an existing filesystem object");

    static auto is_file       = condition([](const auto& v)
    {
        return std::filesystem::exists(std::string(v)) && !std::filesystem::is_directory(std::string(v));
    }, "value must be an existing file");

    static auto is_directory  = condition([](const auto& v)
    {
        return std::filesystem::exists(std::string(v)) && std::filesystem::is_directory(std::string(v));
    }, "value must be an existing file");
}