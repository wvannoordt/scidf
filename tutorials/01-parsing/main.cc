#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "scidf.h"

int main(int argc, char** argv)
{
    //Data read into a scidf::node_t can be parsed to most common types easily via construction

    const std::string filename = "input.sdf";
    scidf::node_t data;
    scidf::read(filename, data);

    //Parsing various types: try introducing
    //bad data in "inoput.sdf" to see error messages
    const int         my_int     = data["my_int"];
    const double      my_double  = data["my_double"];
    const std::string my_string0 = data["my_string0"];
    const std::string my_string1 = data["my_string1"];
    const bool        my_boolean = data["my_boolean"];

    //We can parse vectors of data types too
    const std::vector<int>    my_int_vec    = data["my_int_vec"];
    const std::vector<double> my_double_vec = data["my_double_vec"];

    //Note of caution with std::vector<bool>:
    //https://stackoverflow.com/questions/8399417/why-vectorboolreference-doesnt-return-reference-to-bool
    const std::vector<bool>   my_bool_vec   = data["my_bool_vec"];

    //We can arbitrarily nest vector parses
    const std::vector<std::vector<std::vector<std::vector<int>>>> my_4d_vector
     = data["my_4d_int_vec"];

    std::cout << std::boolalpha;
    std::cout << "my_int     " << my_int     << std::endl;
    std::cout << "my_double  " << my_double  << std::endl;
    std::cout << "my_string0 " << my_string0 << std::endl;
    std::cout << "my_string1 " << my_string1 << std::endl;
    std::cout << "my_boolean " << my_boolean << std::endl;

    std::cout << "my_int_vec:" << std::endl;
    for (const auto& i: my_int_vec) std::cout << "  " << i << std::endl;

    std::cout << "my_double_vec:" << std::endl;
    for (const auto& i: my_double_vec) std::cout << "  " << i << std::endl;

    std::cout << "my_bool_vec:" << std::endl;
    for (const auto& i: my_bool_vec) std::cout << "  " << i << std::endl;

    //Not going to print the 4-D vector, but you get the idea.

    return 0;
}
