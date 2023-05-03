#include <print.h>
#include "scidf.h"


struct custom_t
{
    int x; bool y;
};

const scidf::iconversion_t& operator >> (const scidf::iconversion_t& i, custom_t& c)
{
    std::vector<std::string> arr = scidf::str::split(i.raw, ":");
    if (arr.size() < 2) throw scidf::sdf_exception("not enough elements in custom parse");
    scidf::iconversion_t j(arr[0]);
    scidf::iconversion_t k(arr[1]);
    j >> c.x;
    k >> c.y;
    return i;
}

int main(int argc, char** argv)
{
    std::string filename = "data.sdf";
    scidf::node_t data;
    scidf::clargs_t clargs(argc, argv);
    scidf::read(filename, data, clargs);
    scidf::menu_t<std::string>           menu1 = data["menu1"];
    scidf::menu_t<int>                   menu2 = data["menu2"];
    scidf::menu_t<std::vector<int>>      menu3 = data["menu3"];
    custom_t                             thing = data["custom"];
    scidf::menu_t<std::vector<custom_t>> cmenu = data["cmenu"];
    print(cmenu.selected_value()[0].x, cmenu.selected_value()[0].y);
    return 0;
}
