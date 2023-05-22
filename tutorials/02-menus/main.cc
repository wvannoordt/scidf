#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "scidf.h"

int main(int argc, char** argv)
{
    const std::string filename = "input.sdf";
    scidf::node_t data;
    scidf::read(filename, data);

    //Scidf provides a composable menu type, see the input file for more details
    //Menus are experimental and may not be the best way to express multiple options
    scidf::menu_t<std::string> str_menu = data["string_menu"];
    std::cout << "string_menu" << std::endl;
    std::cout << "Selected index: " << str_menu.selected_index() << std::endl;
    std::cout << "Selected value: " << str_menu.selected_value() << std::endl;

    scidf::menu_t<int> int_menu = data["int_menu"];
    std::cout << "int_menu" << std::endl;
    std::cout << "Selected index: " << int_menu.selected_index() << std::endl;
    std::cout << "Selected value: " << int_menu.selected_value() << std::endl;

    return 0;
}
