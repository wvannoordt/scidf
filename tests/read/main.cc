#include <print.h>
#include "scidf.h"

int main(int argc, char** argv)
{
    const std::string filename = "input.sdf";    
    scidf::node_t data;
    scidf::clargs_t clargs(argc, argv);
    scidf::read(filename, data, clargs);
    
    return 0;
}
