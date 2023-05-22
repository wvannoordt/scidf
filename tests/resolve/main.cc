#include <iostream>
#include "scidf.h"

int main(int argc, char** argv)
{
    const std::string filename = "data.sdf";
    scidf::node_t data;
    scidf::read(filename, data);
    
    return 0;
}
