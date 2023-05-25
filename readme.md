# SCIDF (Scientific Computing Input Data Format)

A needlessly over-engineered data format for sicentific computing
and spiritual successor to [PTL](https://github.com/wvannoordt/PTL).

## Installation and Build

SCIDF is a single-include, header-only C++20 implementation. To build a
project with SCIDF, simply add the `src` directory to the compiler's
include path, e.g.:

`g++ -std=c++20 -I/path/to/scidf/src main.cc -o out.x`

Note that the C++20 standard must be supported by your compiler.

## Usage

SCIDF is intended to be lightweight and easy to use. The following example
shows the most basic usage of SCIDF:

```c++
#include "scidf.h"

int main(int argc, char** argv)
{
    const std::string filename = "input.sdf";
    scidf::node_t data;
    scidf::read(filename, data);

    int value1 = data["value1"];
    int value2 = data["Section"]["value2"];

    // For logging purposes, any node_t
    // can be printed directly
    std::cout << data << std::endl;

    return 0;
}

```

In this case, the file `input.sdf` may look something like:

```
value1 = 100
Section
{
    value2 = 99
}
```

## Features

### Data Organization

SCIDF allows for arbitrarily nested data, e.g.

```
Section00
{
    Section10
    {
        Section20
        {
            ...
        }
    }
}
//This is a comment
Section01
{
    Section11
    {
        Section21
        {
            ...
        }
    }
}
```

This makes it easy to organize and compartmentalize data.
See tutorial `00-getting-started` for more info.

### Comprehensive Parsing

SCIDF contains functions for parsing many kinds of data types, including POD types
such as `int`, `double`, `string`, `float`, `bool`, etc. and vectors of such types.
Parsing is as simple as

```c++

int                          int_val = data["int_val"];
bool                        bool_val = data["bool_val"];
std::string               string_val = data["string_val"];
std::vector<int>             int_vec = data["int_vec"];
std::vector<bool>           bool_vec = data["bool_vec"];
std::vector<std::string>  string_vec = data["string_vec"];

```

Tutorial `01-parsing` has more information on this.

### Menus

SCIDF has a fun type called `menu_t` that allows for users of the input file to select
options from a "menu":

```
my_menu = [ ] option0, [ ] option1, [x] option2, [ ] option3
```

or

```
my_menu = [ ] option0, \
          [ ] option1, \
          [x] option2, \
          [ ] option3
```

This is an experimental feature that may or may not be useful to some people.
See tutorial `02-menus` for more information.

### User-Defined Parsers

SCIDF makes it relatively easy to define parsers for custom data structures. This process
is too detailed to go into here, but is clearly demonstrated in tutorial `03-user-defined-parse`.
Note that parsing in SCIDF is composable, so that if you define how to parse some custom type
`custom_t`, then you can automatically parse `std::vector<custom_t>`, `scidf::menu_t<custom_t>`,
`std::vector<std::vector<custom_t>>`, etc. for free.

### Variables and Functions

Input files can declare variables that can be used in multiple places. Let's say you have an
input file for a case that you've called `CASE0`, and two output files that need to have names
containing the case name. We can do this as follows:

```

$ casename    = CASE0

output_file_0 = "output/$(casename).csv"
output_file_1 = "debug/debug_$(casename).log"

```

The parsing process is blind to the existence of variables, and so when we parse it in
the executable, we simply get

```c++

// Value is "debug/debug_CASE0.log"
std::string filename = data["output_file_1"];

```

Variables in SCIDF files can also be treated like functions. In the above case, we may
decide that the case name always has the form `CASE#` (where # is some integer), in which case
we can have

```
$ id = 0
$ casename(id) = CASE$(id)

output_file_0  = "output/$(casename).csv"
output_file_1  = "debug/debug_$(casename).log"

```

Note that SCIDF also has some build-in utility functions (currently an incomplete list):

```

value0 = @choose(1, op0, op1, op2) // Chooses option 1 from [op0, op1, op2], i.e. "op1"
value1 = @env(HOME)                // Evaluates the environment variable "HOME"

```

Variables can take arbitrarily many parameters, e.g.

```

$ vec(a0, a1, a2, a3, a4) = [$(a0), $(a1), $(a2), $(a3), $(a4)]

```

See tutorial `04-variables-and-functions` for more.

### Modules and Command-Line-Interface

SCIDF has the ability to import content from other files, allowing for more modularity
in the expression of input data. Variables, expressions, and data can all be imported.

```
data0 = value0
data1 = value1

@import(module) //This essentially copies the content of "module.sdf" here
```

SCIDF has a series of paths that it searches for files (by default, it's just the current
directory), and we can add to its path easily:


```
data0 = value0
data1 = value1

@addpath(./subdirectory)
@import(module_from_subdirectory)
```

We can import content into a section using the following:

```
data0 = value0
Section
{
    @import(module)
}
```

Data will then be available in the corresponding section.
We can also perform partial importing. Let's say `module.sdf`
looks like this:

```
Section0
{
    value0 = v0
}
Section1
{
    value1 = v1
}
Section2
{
    value2 = v2
}
```

and we only want to import `Section0` and `Section2` (perhaps we have a conflicting 
definition of `Section1`), then we can do so:

```
@import(module.[Section0, Section2])
Section1
{
    value1 = z1
}
```

SCIDF also allows for the passage of command-line arguments when parsing. Command
line arguments are passed similar to GNU Make, i.e. `-Dvariable=value`. This is
equivalent to the declaration `$ variable = value` in the input file. This allows
for automated parameter sweeps.

To make it more readable when requiring values passed on the command line, we can
use `cli_require` to indicate that there is a CLI variable that is required, e.g.

```
// throws an error with a descriptive message if 'clival' not passed on command line
@cli_require(clival)
number = $(clival)

```


For more info, take a look at tutorial `05-modules-and-cli`.

### Value Conditions and Defaults

SCIDF allows for setting default values for input parameters. We can do this as follows:

```c++
int int_val = scidf::default_to(15, data["int_val"]);
```

This means that `int_val` will be set to 15 if it isn't present in the input file. We can also
use `required` to require that input parameters are specified:

```c++
int int_val = scidf::required<int>(data["int_val"]);
```

although this is the default behaviour anyway. However, after using one of these two constructs,
we can then apply conditions to their values, e.g.

```c++
int int_val = scidf::required<int>(data["int_val"])
    >> scidf::even;
```

This will require that the value for `int_val` is even, and if it isn't then an exception will be thrown.
These conditions are negatable and can be operated on with binary logical operators, e.g.

```c++
int int_val = scidf::required<int>(data["int_val"])
    >> (!scidf::even && scidf::greater_than(3));
```

It is easy to implement your own conditions, e.g.:

```c++
auto my_condition = scidf::condition([=](const auto& v)
{
    return (v[1] > (dval - 1e-3)) && (v[1] < (dval + 1e-3));
}, "second element must be within 1e-3 of sqrt(2)");

std::vector<double> test_vec = scidf::required<std::vector<double>>(data["test_vec"])
    >> my_condition;
```

See tutorial `06-parse-conditions` for further information.