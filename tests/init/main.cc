#include <print.h>
#include "scidf.h"

int main(int argc, char** argv)
{
    const std::string filename = "input.sdf";    
    // scidf::format_t format;
    scidf::node_t data;
    data["p0"];
    data["p1"]["p11"]["p0p0p0"];
    data["p1"]["p10"]["p1p1p1"];
    data["p1"]["p10"]["aaappp"] = 3.145;
    print(data);
    // scidf::bind(nxb, data["nxb"], );
    int nxb;
    
    return 0;
}
