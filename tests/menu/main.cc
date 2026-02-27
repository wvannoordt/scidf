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
    std::string filename0 = "err-bad.sdf";
    std::string filename1 = "err-good.sdf";
    scidf::node_t data0, data1;
    scidf::clargs_t clargs(argc, argv);
    scidf::read(filename0, data0, clargs);
    scidf::read(filename1, data1, clargs);
    print("=================");
    print(data0);
    print("=================");
    print(data1);
    print("=================");
    return 0;
}
