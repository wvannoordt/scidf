#pragma once

#include <map>
#include <string>
#include <iostream>

#include "string_util.h"
#include "string_conversion.h"
#include "context.h"

namespace scidf
{
    struct node_t
    {
        static std::string parent_str() {return "..";}
        static std::string default_name() {return "[[NONAME]]";}
        static std::string default_value() {return "[[NOVALUE]]";}
        node_t* parent;
        std::string name, value;
        std::map<std::string, node_t> children;
        int level;
        
        node_t()
        {
            parent = nullptr;
            name  = default_name();
            value = default_value();
            level = 0;
        }
        
        node_t(node_t* parent_in, const std::string& name_in, const std::string& value_in)
        : parent{parent_in}, name{name_in}, value{value_in}, level{parent_in->level+1} {}
        
        node_t(node_t* parent_in, const std::string& name_in)
        : parent{parent_in}, name{name_in}, value{default_value()}, level{parent_in->level+1} {}
        
        template <typename rhs_t> node_t& operator = (const rhs_t& rhs)
        {
            value = str::convert_to_string(rhs);
            return *this;
        }
        
        bool is_root() const {return parent == nullptr;}
        bool has_value() const {return value != default_value();}
        bool is_terminal() const {return children.size() == 0;}
        
        void set_value(const std::string& val) { value = val; }
        const std::string& get_value() const { return value; }
        
        node_t& operator [] (const std::string& key)
        {
            if (children.find(key) == children.end())
            {
                children.insert({key, node_t(this, key)});
            }
            return children.at(key);
        }

        

        
    };
    
    namespace detail
    {
        constexpr static int tabwid = 4;
        void rprint_node(std::ostream& os, const node_t& node, const int space)
        {
            for (auto p: node.children)
            {
                os << str::fill(space, ' ') << p.first;
                if (p.second.has_value()) os << " = " << p.second.get_value();
                os << "\n";
                detail::rprint_node(os, p.second, space+tabwid);
            }
        }
    }
    
    std::ostream& operator << (std::ostream& os, const node_t& node)
    {
        detail::rprint_node(os, node, 0);
        return os;
    }
}