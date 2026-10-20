// Copyright (c) 2026 Eric Larsen
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ****************************************************************************
// ****************************************************************************
//
//
#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <string_view>

using namespace std;


// prefix of all parameters instruction line parameters, except help and verbosity
#define PARAM_PREFIX '-'

// prefix of options related to help and verbosity
#define CONFIG_PREFIX '+'

// requesting help
#define CONFIG_HELP_MODE "+H"

// requesting verbosity when parsing instruction line
#define CONFIG_VERBOSE_MODE "+v"

// input and output formats available
enum IO_FORMAT
{
    DOW = 0,
    GSTD, // generic STD
    RSTD, // restricted STD
};

constexpr string_view to_string(IO_FORMAT format) {
    switch (format) {
        case IO_FORMAT::DOW:   return "DOW";
        case IO_FORMAT::GSTD: return "GSTD";
        case IO_FORMAT::RSTD:  return "RSTD";
    }
    return "Unknown";
}

// Types of elements (i.e. commodity volumes, total arc
// capacities, capacities per arc and commodity, fixed costs per arc,
// variable costs per arc and commodity) that specify a deterministic instance.
// MAX_TYPE will be used as an index upper bound when looping over
// ELEMS_TYPE. ALL_TYPES is used to identify arrays containing
// elements from all types.
enum ELEMS_TYPE
{
    DEMAND = 0,
    CAP_ARC,
    CAP_COMMOD,
    FIXED_COST,
    VAR_COST,
    MAX_TYPE,
    ALL_TYPES = MAX_TYPE
};

const string formats[] = {"dow", "std"};

const string elements[] = {"demand", "capacity of arc", "capacity of comm. on arc", "fixed cost of arc",
                           "variable cost of comm. on arc"};

// Holding the elements belonging to a particular type in ELEMS_TYPE.
struct Elem
{
    // pointer of array containing the elements
    double *ptr;

    // type of the stochastic element
    ELEMS_TYPE type;

    // size of array
    uint size;

    // operators retrieving individual elements stored in the array
    double &operator[](int index)
    {
        return ptr[index];
    }
    const double &operator[](int index) const
    {
        return ptr[index];
    }
};

typedef vector<Elem> VectElem;
typedef vector<ELEMS_TYPE> VectTypes;
typedef vector<double> VectDbl;
typedef vector<VectDbl> MatDbl;

// (See main.cpp for details on configuration parameters and construction
// of command line.)
struct Configuration
{
    // number of arguments appearing in command line
    int nbArg;

    // char array containing the command line
    char **vectArg;

    // flags displaying parameter values when parsing
    bool verbose;
	
    // format of input and output instances: DOW, GSTD or RSTD
    IO_FORMAT instanceFormat;
    
    // name of file containing determistic MCFND instance (default or
    // modified with option key I)
    string inputInstFileName;

    // vector listing the individual types
    VectTypes allTypes;    

    Configuration(int argc, char **argv, string configFileName);
	
    void printHelpAndExit();

	// unused
    void printUsageAndExit(char ExecName[], bool anonymous);	

    // Parses command line.
    void parseCmdLine();	
	
	// While traversing command line, seeks flags for verbose or help modes.
    void parseConfigModesFromCmdLine();	

	// unused
    // Removes quotes from value in (key, value) pair on a non-comment line
    // of the configuration file.
    string removeQuotes(istringstream &configLine);

    // Verifies configuration values.
    void verifyInputInstance();	
	
};

#endif
