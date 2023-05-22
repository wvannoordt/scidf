#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "scidf.h"

// We may want to add a way to parse a user-defined type using scidf.
// Perhaps we have a "named vector" type that includes a string (name)
// and data (vector), expressed as "name":[c0, c1, c2], where ci are
// e.g. integers (we can use templates for further generality). Let's
// also assume that we always want 3 elements, and it is an error to
// provide more or fewer elements.
struct named_vec_t
{
    // again, we can always be more general here but
    // for now we use a simple data structure
    std::string name;
    int data[3];
};

// We introduce custom parsing via the '>>' operator using scidf::iconversion_t
const scidf::iconversion_t& operator >> (const scidf::iconversion_t& icnv, named_vec_t& named_vec)
{
    // The "raw" string contents are held withing icnv
    const std::string str_contents = icnv.raw;

    // We can check for the ":" delimiter, and if it isn't present we simply throw an
    // sdf_exception. The exception handling is all done within the internals of scidf.
    const std::size_t dlm_pos = str_contents.find_first_of(':');
    if (dlm_pos == std::string::npos) throw scidf::sdf_exception("Expecting symbol \":\", but none found");

    // Split the string on ":" to extract the name and data
    const std::string name = str_contents.substr(0, dlm_pos);
    const std::string data = str_contents.substr(dlm_pos + 1, str_contents.length() - dlm_pos - 1);

    // We can directly assign the name since it is a string
    // (we could also parse it to a string using another iconversion_t
    // in the case we need to handle quote marks or something)
    named_vec.name = name;

    // We need to parse the second string as a vector of integers
    std::vector<int> iv;

    // We create a "sub parser" to fill the integer data without
    // handling the details ourselves
    scidf::iconversion_t converter(data);
    converter >> iv;

    // Here, we enforce that the size of the data provided is 3
    if (iv.size() != 3) throw scidf::sdf_exception("Expected 3 entries, but found " + std::to_string(data.size()));

    // Fill the data in the array in the named vector
    for (int i = 0; i < 3; ++i) named_vec.data[i] = iv[i];

    // Return the parser (required)
    return icnv;
}

int main(int argc, char** argv)
{
    const std::string filename = "input.sdf";
    scidf::node_t data;
    scidf::read(filename, data);

    named_vec_t named_vec = data["named_vec"];

    std::cout << "named_vec:" << std::endl;
    std::cout << "  name: " << named_vec.name << std::endl;
    std::cout << "  data: " << std::endl;
    for (int i = 0; i < 3; ++i) std::cout << "    " << named_vec.data[i] << std::endl;

    //We can also compose the custom type with vector parsing
    std::vector<named_vec_t> nvs = data["v_named_vec"];

    // In principle, this should work with menus as well, but
    // sometimes the parsers conflict!
    // scidf::menu_t<named_vec_t> mvs = data["m_named_vec"];

    return 0;
}
