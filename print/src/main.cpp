// main.cpp

#include "IPrintable.h"

// utilities
#include "JsonSerializer.h"
#include "files.h"
using utilities::OpenIfstream;

// layers
#include "layers.h"
using layers::Map;

#include<iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;

#include <stdexcept>
using std::runtime_error;

#include <memory>
using std::dynamic_pointer_cast;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        cerr << "usage: print.exe <layers file>\n";
        return 1;
    }

    try
    {
        // open file
        auto layersFile = argv[1];
        ifstream layersFStream = OpenIfstream(layersFile);
        auto map = layers::Io::Read(layersFStream);

        // cast to printable map
        auto printableMap = dynamic_pointer_cast<IPrintable>(map);
        printableMap->Print(cout);

    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << std::endl;
    }

    // the end
    return 0;
}
