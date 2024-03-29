#include <print.h>
#include "scidf.h"

int main(int argc, char** argv)
{
    std::string filename = "clean.sdf";
    if (argc>1) filename = std::string(argv[1]);    
    scidf::node_t data;
    scidf::clargs_t clargs(argc, argv);
    scidf::read(filename, data, clargs);
    print(data);
    return 0;
}
