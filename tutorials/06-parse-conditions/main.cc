#include <vector>
#include <string>
#include <cmath>
#include "scidf.h"

int main(int argc, char** argv)
{
    const std::string filename = "input.sdf";
    scidf::node_t data;
    scidf::clargs_t clargs(argc, argv);
    scidf::read(filename, data, clargs);

    // In this tutorial, we make use of parsing conditions to validate input.
    // Note that complex input validation is best left to external logic,
    // but scidf's conditional operators will

    // Here is an unconditional parse
    std::string file0 = data["file"];

    // Here, we throw an error if the variable 'file' isn't found.
    // Note that this is identical behaviour as above!!!
    std::string file1 = scidf::required<std::string>(data["file"]);

    // The advantage of using "required" is that it allows us to create
    // conditions on the content of "file". For example, the following
    // Will throw an error if the value if "file" doesn't exist as a file.
    // Try changing the value of "file" in the input file!!
    std::string file2 = scidf::required<std::string>(data["file"])
        >> scidf::is_file;
    
    // See also scidf::is_directory and scidf::exists

    // We can constrain integers and doubles, etc:
    double dvalue0 = scidf::required<double>(data["dvalue"])
        >> scidf::less_than(0.0);

    // We can also negate conditions:
    double dvalue1 = scidf::required<double>(data["dvalue"])
        >> !scidf::greater_than(0.0);
    
    // See also scidf::greater_than_or_equal_to, scidf::less_than_or_equal_to

    // We can also use boolean operations on conditions:
    int my_int = scidf::required<int>(data["my_int"])
        >> ((scidf::even && !scidf::odd) && (scidf::greater_than_or_equal_to(9) && scidf::even));

    // Vector size constraints
    std::vector<int> vec0 = scidf::required<std::vector<int>>(data["vec"])
        >> scidf::size_is(5);
    
    // We can apply conditions to every element of the vector:
    std::vector<int> vec1 = scidf::required<std::vector<int>>(data["vec"])
        >> (scidf::size_is(5) && !scidf::all_elements(scidf::even));
    
    // Note that we can introduce an optional, defaulted value that may
    // or may not be specified in the input file:
    int defaulted_int0 = scidf::default_to(15, data["defaulted_int"]);

    // Defaulted values can also use constraints
    int defaulted_int1 = scidf::default_to(15, data["defaulted_int"])
        >> scidf::odd;
    
    // We can store a condition for use on multiple options
    auto multi = scidf::greater_than(10) && scidf::even;
    int v0     = scidf::required<int>(data["value0"]) >> multi;
    int v1     = scidf::required<int>(data["value1"]) >> multi;
    int v2     = scidf::required<int>(data["value2"]) >> multi;

    // Conditions can be checked directly by calling verify:
    int test_val = 101;
    bool success = multi.verify(test_val);
    std::cout << std::boolalpha << test_val << " --> " << success << std::endl;

    // We can print the condition's "message":
    if (!success) std::cout << multi.get_message() << std::endl;

    // Finally, in the most general case, we can define any custom condition we want:
    const double dval = std::sqrt(2.0);

    //Note that it is generally good practice to use value-capturing here.
    auto my_condition = scidf::condition([=](const auto& v)
    {
        return (v[1] > (dval - 1e-3)) && (v[1] < (dval + 1e-3));
    }, "second element must be within 1e-3 of sqrt(2)");

    std::vector<double> test_vec = scidf::required<std::vector<double>>(data["test_vec"])
        >> my_condition;
    
    // Note that there are currently no conditions that apply to menus or booleans.

    return 0;
}