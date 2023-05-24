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

    int val1 = scidf::default_to(15, data["value"])
        >> !((scidf::greater_than(4) && scidf::less_than(10)) || !scidf::even);
    
    std::string output_file = scidf::required<std::string>(data["file"])
        >> scidf::is_file;
    return 0;
}
