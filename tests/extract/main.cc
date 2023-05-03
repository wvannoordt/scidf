#include <print.h>
#include "scidf.h"

int main(int argc, char** argv)
{
    std::string filename = "data.sdf";
    scidf::node_t data;
    scidf::clargs_t clargs(argc, argv);
    scidf::read(filename, data, clargs);
    std::vector<std::vector<std::string>> temp = data["section"]["politewords"];
    for (auto& p: temp)
    {
        print(p[0], p[1], p[2]);
    }
    return 0;
}
