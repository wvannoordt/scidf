#include <print.h>
#include "scidf.h"

int main(int argc, char** argv)
{
    const std::string filename = "input.sdf";    
    // scidf::format_t format;
    scidf::node_t data;
    scidf::clargs_t clargs(argc, argv);
    data.read(filename);
    
    // scidf::bind(nxb, data["nxb"], );
    int nxb;
    
    return 0;
}