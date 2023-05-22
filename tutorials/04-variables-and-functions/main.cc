#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "scidf.h"

int main(int argc, char** argv)
{
    // For this tutorial, relevant comments are
    // moved to the input file rather than here!
    const std::string filename = "input.sdf";
    scidf::node_t data;
    scidf::read(filename, data);

    std::string val = data["variable_val"];
    std::cout << "variable_val: " << val << std::endl;

    std::string my_filename = data["my_filename"];
    std::cout << "my_filename: " << my_filename << std::endl;

    std::vector<int> my_pair = data["my_pair"];
    std::cout << "my_pair: " << my_pair[0] << ", " << my_pair[1] << std::endl;

    const std::string env_var = data["env_var"];    
    std::cout << "env_var = " << env_var << std::endl;

    const std::string chosen = data["chosen"];    
    std::cout << "chosen = " << chosen << std::endl;

    return 0;
}
