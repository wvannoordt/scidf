#pragma once

#include <map>
#include <string>
#include <iostream>

#include "string_util.h"
#include "string_conversion.h"
#include "context.h"
#include "conversion.h"

namespace scidf
{
    struct node_t
    {
        static std::string parent_str() {return "..";}
        static std::string default_name() {return "[[NONAME]]";}
        static std::string default_value() {return "[[NOVALUE]]";}
        node_t* parent;
        std::string name, value;
        bool assigned_value = false;
        std::map<std::string, node_t> children;
        int level;
        
        node_t()
        {
            parent = nullptr;
            name  = default_name();
            value = default_value();
            level = 0;
        }

        template <typename criterion_t, typename action_t>
        void visit(const criterion_t& crit, const action_t action) const
        {
            if (crit(*this)) action(*this);
            for (const auto& c: children) c.second.visit(crit, action);
        }
        
        node_t(node_t* parent_in, const std::string& name_in, const std::string& value_in)
        : parent{parent_in}, name{name_in}, value{value_in}, level{parent_in->level+1} {}
        
        node_t(node_t* parent_in, const std::string& name_in)
        : parent{parent_in}, name{name_in}, value{default_value()}, level{parent_in->level+1} {}

        template <typename converted_t> operator converted_t() const
        {
            if (!assigned_value) throw sdf_exception("attempted to perform illegal conversion on unassigned node with name \"" + get_path(glob_syms) + "\"");
            if (!is_terminal())  throw sdf_exception("attempted to perform illegal conversion on section node with name \"" + get_path(glob_syms) + "\"");
            try
            {
                iconversion_t icv(value);
                converted_t val;
                icv >> val;
                return val;
            }
            catch (const std::exception& e)
            {
                throw sdf_exception("error in conversion of target \"" + get_path(glob_syms) + "\" with value \"" + value + "\":\n" + std::string(e.what()));
            }
        }

        std::string get_path(const syms_t& syms = glob_syms) const
        {
            if (is_root()) return "";
            if (parent->is_root()) return name;
            return parent->get_path(syms) + syms.scope_operator + name;
        }
        
        bool is_root() const {return parent == nullptr;}
        bool has_value() const {return value != default_value();}
        bool is_terminal() const {return children.size() == 0;}
        bool contains_child(const std::string& key) const
        {
            return children.find(key) != children.end();
        }
        
        void set_value(const std::string& val) { value = val; assigned_value = true; }
        const std::string& get_value() const { return value; }
        std::string get_name() const {return name;}
        
        node_t& operator [] (const std::string& key)
        {
            if (children.find(key) == children.end())
            {
                children.insert({key, node_t(this, key)});
            }
            return children.at(key);
        }
        
        //Added because nvcc thinks everything is an int
        template <const int csize>
        node_t& operator[] (const char (&str)[csize])
        {
            return (*this)[std::string(str)];
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
        if (node.is_terminal())
        {
            os << node.get_value();
        }
        else
        {
            detail::rprint_node(os, node, 0);
        }
        return os;
    }
}