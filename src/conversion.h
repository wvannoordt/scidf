#pragma once

#include <concepts>
#include <string>
#include <vector>
#include <sstream>
#include "sdf_exception.h"
#include "syms.h"
#include "extract_vector.h"

namespace scidf
{
    template <typename T, typename... Ts>
    concept is_any_of = (std::same_as<T, Ts> || ...);

    struct iconversion_t
    {
        const std::string& raw;
        const syms_t& syms{glob_syms};
        iconversion_t(const std::string& raw_in) : raw{raw_in} {}

        sdf_exception get_default_exception() const {return sdf_exception("Bad parse: \"" + raw + "\"");}

        template <typename data_t>
        requires (is_any_of<data_t,
            char,
            float,
            double,
            int,
            unsigned int,
            unsigned long,
            unsigned long long,
            bool>)
        const iconversion_t& operator >> (data_t& data) const
        {
            std::istringstream iss(raw);
            if constexpr (std::same_as<data_t, bool>)
            {
                iss >> std::boolalpha >> data;
            }
            else
            {
                iss >> data;
            }
            if (iss.fail()) throw get_default_exception();
            return *this;
        }

        const iconversion_t& operator >> (std::string& data) const
        {
            if (raw.find(syms.open_string) == std::string::npos)
            {
                data = raw;
                return *this;
            }
            if (raw.length() == 1) throw sdf_exception("incomprehensible string parse: \"" + raw + "\"");
            std::size_t start = 1;
            std::size_t end   = raw.length() - 1;
            if (raw[0] == syms.open_string && raw[raw.length()-1] == syms.open_string)
            {
                data = raw.substr(start, end-start);
                return *this;
            }
            throw get_default_exception();
        }

        template <typename data_t> const iconversion_t& operator >> (std::vector<data_t>& data) const
        {
            try
            {
                std::vector<std::string> individual = extract_vector(raw, syms);
                data.clear();
                if (individual.size() == 0) return *this;
                if (str::trim(individual[0], syms.whitespace).length() == 0) return *this;
                data.resize(individual.size());
                for (std::size_t i = 0; i < individual.size(); ++i)
                {
                    iconversion_t icv(individual[i]);
                    icv >> data[i];
                }
                return *this;
            }
            catch(const std::exception& e)
            {
                throw sdf_exception(std::string("bad vector parse: \"" + raw + "\"\nMessage: ") + e.what());
            }
        }
    };
}