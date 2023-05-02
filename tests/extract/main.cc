#include <print.h>
#include "scidf.h"

int main(int argc, char** argv)
{
    std::string filename = "data.sdf";
    scidf::node_t data;
    scidf::clargs_t clargs(argc, argv);
    scidf::read(filename, data, clargs);
    std::string temp = data["value1"];
    print(temp);
    return 0;
}
