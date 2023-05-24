#pragma once

#include "sdf_exception.h"
#include <filesystem>
#include <optional>

namespace scidf
{
    template <typename eval_t> struct lazy_condition_t
    {
        constexpr static bool scidf_value_verifiable_tag = true;
        const eval_t eval; //todo: check if can hold by reference here!
        bool inverse = false;
        std::string message;

        lazy_condition_t(const eval_t& eval_in)
        : eval{eval_in}, message{"[unspecified conditional]"} {}

        lazy_condition_t(const eval_t& eval_in, const std::string& message_in)
        : eval{eval_in}, message{message_in} {}

        bool verify(const auto& val) const
        {
            return (inverse == !eval(val));
        }

        std::string get_message() const
        {
            return inverse?("NOT {" + message + "}"):message;
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
        const lhs_t lhs;
        const rhs_t rhs;
        bool inverse = false;
        bool verify(const auto& val) const
        {
            bool ll_b = lhs.verify(val);
            bool rr_b = rhs.verify(val);
            bool good_result = false;
            if constexpr (operation == binary_and) { good_result = (!inverse) == (ll_b && rr_b); }
            if constexpr (operation == binary_or ) { good_result = (!inverse) == (ll_b || rr_b); }
            return good_result;
        }

        std::string get_message() const
        {
            std::string message = "** internal error **";
            if constexpr (operation == binary_and)
            {
                message = "{" + lhs.get_message() + "} AND {" + rhs.get_message() + "}";
            }
            if constexpr (operation == binary_or)
            {
                message = "{" + lhs.get_message() + "} OR {" + rhs.get_message() + "}";
            }
            return inverse?("NOT {" + message + "}"):message;
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
        return lazy_condition_t(eval, message);
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

    static auto size_is(const std::size_t& rsize)
    { return condition([=](const auto& v){return v.size() == rsize;}, "size must be " + std::to_string(rsize));}

    static auto all_elements(const auto& verifiable)
    {
        return condition([=](const auto& iterable)
        {
            for (const auto& i: iterable)
            {
                if (!verifiable.verify(i)) return false;
            }
            return true;
        },
        "all elements must satisfy {" + verifiable.get_message() + "}");
    }
}