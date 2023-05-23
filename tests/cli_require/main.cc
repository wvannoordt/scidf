#include <print.h>
#include <iomanip>
#include <vector>
#include <string>
#include "scidf.h"

int main(int argc, char** argv)
{
    const std::string filename = "input.sdf";
    scidf::node_t data;
    scidf::clargs_t clargs(argc, argv);
    scidf::read(filename, data, clargs);

    std::string val1 = data["variable1"];
    std::string val2 = data["val2"];
    print(val1, val2);

    return 0;
}
