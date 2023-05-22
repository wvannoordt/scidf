#include <iostream>
#include "scidf.h"

int main(int argc, char** argv)
{
    //This is the most basic way of using scidf, were data is simply
    //read into the provided data structure and regurgitated to the
    //screen.

    //Take a look at "input.sdf" for more information

    //We'll read from file "input.sdf"
    const std::string filename = "input.sdf";

    //The node_t type will contain all data read from the requested file
    scidf::node_t data;

    //Read the data from the file
    scidf::read(filename, data);

    //We can print content from the file directly
    std::cout << data["variable"] << std::endl;

    std::cout << data["other_variable"] << std::endl;

    //We can organize data into subsections
    std::cout << data["Section"]["variable_in_section"] << std::endl;

    //Subsections can be nested arbitrarily deeply
    std::cout << data["Section"]["NestedSection"]["another_variable"] << std::endl;

    return 0;
}
