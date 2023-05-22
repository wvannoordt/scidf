#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "scidf.h"

int main(int argc, char** argv)
{
    // In order to pass arguments into the parser at
    // runtime, we can use values passed on the command line.
    // To do this, we need to create a clargs_t object:
    const std::string filename = "input.sdf";
    scidf::clargs_t clargs(argc, argv);
    scidf::node_t data;

    // We then pass clargs to the read function to 
    // pick up on command line definitions
    scidf::read(filename, data, clargs);

    // NOTE: in order for this example to run, you must pass
    // the command line argument "-Dcli_value=something"
    // to the executable, or else scidf will throw an
    // error
    std::string cli_variable = data["cli_variable"];
    std::cout << "cli_variable = " << cli_variable << std::endl;

    std::string data_in_module0 = data["data_in_module0"];
    std::cout << "data_in_module0 = " << data_in_module0 << std::endl;

    std::string data_in_module0_v2 = data["Section0"]["data_in_module0"];
    std::cout << "data_in_module0 (again) = " << data_in_module0_v2 << std::endl;

    std::string data_in_module1 = data["data_in_module1"];
    std::cout << "data_in_module1 = " << data_in_module1 << std::endl;

    std::string this_works_too = data["this_works_too"];
    std::cout << "this_works_too = " << this_works_too << std::endl;

    
    return 0;
}
