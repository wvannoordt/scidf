#pragma once

#include <string>
#include <vector>
#include "conversion.h"
#include "sdf_exception.h"

namespace scidf
{
    template <typename data_t> struct menu_t
    {
        std::vector<data_t> items;
        std::size_t selected;
        std::size_t   selected_index() const {return selected;}
        const data_t& selected_value() const {return items[selected];}
    };

    template <typename data_t> const iconversion_t& operator >> (const iconversion_t& cnv, menu_t<data_t>& data)
    {
        std::string mod = "[" + cnv.raw + "]";
        iconversion_t tmp(mod);
        std::vector<std::string> list;
        tmp >> list;
        data.selected = std::string::npos;
        std::string last_selected;
        for (std::size_t i = 0; i < list.size(); ++i)
        {
            const std::string tr_line = str::trim(list[i], cnv.syms.whitespace);
            std::size_t s0 = tr_line.find(cnv.syms.open_array);
            std::size_t s1 = tr_line.find(cnv.syms.close_array);
            if (s0 == std::string::npos) throw sdf_exception("bad menu item parse: cannot find open-array symbol in option \"" + tr_line + "\"");
            if (s1 == std::string::npos) throw sdf_exception("bad menu item parse: cannot find close-array symbol in option \"" + tr_line + "\"");
            if (s1 <= s0) throw sdf_exception("bad menu item parse: array symbols in wrong order in option \"" + tr_line + "\"");
            s0++;
            std::string select_stuff = str::trim(tr_line.substr(s0, s1-s0), cnv.syms.whitespace);
            if (s1 == tr_line.length()-1) throw sdf_exception("bad menu item parse: detected list item (item " + std::to_string(i) + ") with no content");
            s1++;
            std::string name = str::trim(tr_line.substr(s1, tr_line.length()-s1), cnv.syms.whitespace);
            if (name.length()==0) throw sdf_exception("bad menu item parse: detected list item (item " + std::to_string(i) + ") with no content");
            data_t data_item;
            iconversion_t temp(name);
            try { temp >> data_item; }
            catch (const std::exception& e)
            {
                throw sdf_exception("bad menu item \"" + tr_line + "\".\nMessage:\n" + e.what());
            }
            data.items.push_back(data_item);
            if (select_stuff.length()>0)
            {
                if (data.selected != std::string::npos) throw sdf_exception("detected conflicting selections \"" + tr_line + "\" and \"" + last_selected + "\"");
                data.selected = i;
                last_selected = tr_line;
            }
        }
        if (data.selected == std::string::npos) throw sdf_exception("cannot find selected menu item.\nContent:\n" + cnv.raw);
        return cnv;
    }
}