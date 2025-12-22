// Copyright (c) 2025 Eric Larsen
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
#include "classes.h"

// Constructor
Data::Data(int argc, char **argv, string fileName)
    : // paramProcessing() launches the initial processing of configuration
      // settings: builds key, value maps for parameters specified in files
      // and in instruction line.
      paramProcessing(argc, argv, fileName), gridConnect(false), toralConnect(false), cylConnect(false),
      flatConnect(false), circConnect(false), randConnect(false), fromFile(false), noParallelArcs(false),
      oneSourceOneSink(false), equSourcesEquSinks(false), noStdOutFile(false), generLpOutFile(false),
      generMpsOutFile(false), generTexOutFile(false), integCapacs(false), integCommCapacs(false), noCommCapacs(false),
      stream(101), seed(101), lengthX(2), lengthY(2), numbNodes(4), numbCommods(1), numbAddRandArcs_SamplTailsHeads(0),
      numbAddRandArcsPerNode_SamplHeads(0), minNumbSources(1), maxNumbSources(1), minNumbSinks(1), maxNumbSinks(1),
      minCommVol(0), maxCommVol(10000), minFixedCost(0), maxFixedCost(10000), minVarCost(0), maxVarCost(10000),
      minArcCapac(1), maxArcCapac(10000), minCommCapac(0), maxCommCapac(10000), adjCapacsDown(false), adjFactCapacs(1),
      adjFixCostsUp(false), adjFactFixCosts(1), ratioZeroFixedCost(0), ratioTotVolCapac(0), ratioZeroCommCapac(0),
      ratioMaxCommCapac(0), maxIterNoParallelArcs(0), shortOutFileName("./outfile"),
      longBasicGraphFileName("./basic_graph.gra")
{

    int topology;
    bool help = false;

    readParam("help", "help", help);

    if (help)
    {
        printHelpAndExit();
    }

    // reading all parameters from file(s) and overriding with instruction line if
    // present therein; see main.cpp and classes.h for details

    // sets the base topology of the network (see switch below for details)
    readParam("topology", "topology", topology, 0, 5);

    // useful to activate only if grid-like topology
    if (topology >= 1 && topology <= 3)
    {
        readParam("lengthX", "lengthX", lengthX, 1, INT_MAX);
        readParam("lengthY", "lengthY", lengthY, 1, INT_MAX);
    }
    readParam("oneSrcOneSnk", "oneSrcOneSnk", oneSourceOneSink);

    // these are useless to activate if oneSourceOneSink
    if (!oneSourceOneSink)
    {
        readParam("minNbSrc", "minNbSrc", minNumbSources, 1, INT_MAX);
        readParam("maxNbSrc", "maxNbSrc", maxNumbSources, 1, INT_MAX);
        readParam("minNbSnk", "minNbSnk", minNumbSinks, 1, INT_MAX);
        readParam("maxNbSnk", "maxNbSnk", maxNumbSinks, 1, INT_MAX);
    }

    readParam("eqSrcsEqSnks", "eqSrcsEqSnks", equSourcesEquSinks);
    readParam("stream", "stream", stream, 0, INT_MAX);
    readParam("seed", "seed", seed, 0, INT_MAX);
    readParam("numbNodes", "numbNodes", numbNodes, 1, INT_MAX);
    readParam("numbCom", "numbCom", numbCommods, 0, INT_MAX);
    readParam("nbAddArcSampTailHead", "nbAddArcSampTailHead", numbAddRandArcs_SamplTailsHeads, 0, INT_MAX);
    readParam("nbAddArcPerSampHead", "nbAddArcPerSampHead", numbAddRandArcsPerNode_SamplHeads, 0, INT_MAX);
    readParam("minComVol", "minComVol", minCommVol, 1, INT_MAX);
    readParam("maxComVol", "maxComVol", maxCommVol, 1, INT_MAX);
    readParam("minVarCos", "minVarCos", minVarCost, 0, INT_MAX);
    readParam("maxVarCos", "maxVarCos", maxVarCost, 0, INT_MAX);
    readParam("minArcCap", "minArcCap", minArcCapac, 1, INT_MAX);
    readParam("maxArcCap", "maxArcCap", maxArcCapac, 1, INT_MAX);
    readParam("minFixCos", "minFixCos", minFixedCost, 0, INT_MAX);
    readParam("maxFixCos", "maxFixCos", maxFixedCost, 0, INT_MAX);
    readParam("minComCap", "minComCap", minCommCapac, 0, INT_MAX);
    readParam("maxComCap", "maxComCap", maxCommCapac, 0, INT_MAX);
    readParam("ratZeroFix", "ratZeroFix", ratioZeroFixedCost, 0, 100);
    readParam("ratTotVolCap", "ratTotVolCap", ratioTotVolCapac, 0, 100);
    readParam("ratZeroComCap", "ratZeroComCap", ratioZeroCommCapac, 0, 100);
    readParam("ratMaxComCap", "ratMaxComCap", ratioMaxCommCapac, 0, 100);
    readParam("noParalArc", "noParalArc", noParallelArcs, maxIterNoParallelArcs, 1, INT_MAX);
    // adjCapacsDown is a boolean set to true if option adjCapacsDown appears
    // in instruction line or configuration file; adjFactCapacs is set to value
    // appearing in instruction line or configuration file next to option
    // adjCapacsDown.
    readParam("adjCapacsDown", "adjCapacsDown", adjCapacsDown, adjFactCapacs, 1, INT_MAX);
    // adjFixCostsUp is a boolean set to true if option adjFixCostsUp appears
    // in instruction line or configuration file; adjFactCapacs is set to value
    // appearing in instruction line or configuration file next to option
    // adjFactFixCosts
    readParam("adjFixCostsUp", "adjFixCostsUp", adjFixCostsUp, adjFactFixCosts, 1, INT_MAX);
    readParam("shortOutFileName", "shortOutFileName", shortOutFileName);
	verifyOutputPath(shortOutFileName);
    readParam("longBasicGraphFileName", "longBasicGraphFileName", longBasicGraphFileName);
	verifyOutputPath(longBasicGraphFileName);
    readParam("mpsOut", "mpsOut", generMpsOutFile);
    readParam("lpOut", "lpOut", generLpOutFile);
    readParam("texOut", "texOut", generTexOutFile);
    readParam("inclDesignObjCap", "inclDesignObjCap", integCapacs);
    readParam("inclDesignObjCommCap", "inclDesignObjCommCap", integCommCapacs);
    readParam("noComCaps", "noComCaps", noCommCapacs);
    readParam("noStdOut", "noStdOut", noStdOutFile);

    switch (topology)
    {
    case 0:
        gridConnect = false;
        toralConnect = false;
        cylConnect = false;
        flatConnect = false;
        circConnect = false;
        randConnect = true;
        fromFile = false;
        break;

    case 1:
        gridConnect = true;
        toralConnect = true;
        cylConnect = false;
        flatConnect = false;
        circConnect = false;
        randConnect = false;
        fromFile = false;
        numbNodes = lengthX * lengthY;
        cout << "numbNodes has automatically been set to " << numbNodes
             << " = lengthX * lengthY since grid topology has been selected." << endl;
        break;

    case 2:
        gridConnect = true;
        toralConnect = false;
        cylConnect = true;
        flatConnect = false;
        circConnect = false;
        randConnect = false;
        fromFile = false;
        numbNodes = lengthX * lengthY;
        cout << "numbNodes has automatically been set to " << numbNodes
             << " = lengthX * lengthY since grid topology has been selected." << endl;
        break;

    case 3:
        gridConnect = true;
        toralConnect = false;
        cylConnect = false;
        flatConnect = true;
        circConnect = false;
        randConnect = false;
        fromFile = false;
        numbNodes = lengthX * lengthY;
        cout << "numbNodes has automatically been set to " << numbNodes
             << " = lengthX * lengthY since grid topology has been selected." << endl;
        break;

    case 4:
        gridConnect = false;
        toralConnect = false;
        cylConnect = false;
        flatConnect = false;
        circConnect = true;
        randConnect = false;
        fromFile = false;
        break;

    case 5:
        gridConnect = false;
        toralConnect = false;
        cylConnect = false;
        flatConnect = false;
        circConnect = false;
        randConnect = false;
        fromFile = true;
        break;

    default:
        cerr << "\ntopology must fall in {0, 1, 2, 3, 4, 5}." << endl;
        cerr << "EXECUTION ABORTED\n" << endl;
        printUsageAndExit(0, true);
        exit(1);
    }

    crossValidateParams();	
}

void Data::displayError(string error)
{
    cerr << "\nERROR: " + error << endl;
    cerr << "EXECUTION ABORTED\n" << endl;
    printUsageAndExit(0, true);
    exit(1);
}

// Verifies data that should lie inside an interval.
template <class T> void Data::verifyParam(string name, const T &var, const T minVal, const T maxVal)
{
    if (var < minVal || var > maxVal)
    {
        cerr << "\n" << name << " must be within [" << minVal << " , " << maxVal << "]." << endl;
        cerr << "EXECUTION ABORTED\n" << endl;
        printUsageAndExit(0, true);
        exit(1);
    }
}

// Cross-validates data.
void Data::crossValidateParams()
{

    if (minCommVol > maxCommVol)
        displayError("minCommVol > maxCommVol");

    if (minVarCost > maxVarCost)
        displayError("minVarCos > maxVarCos");

    if (minFixedCost > maxFixedCost)
        displayError("minFixCos > maxFixCos");

    if (minArcCapac > maxArcCapac)
        displayError("minArcCap > maxArcCap");

    if (minCommCapac > maxCommCapac)
        displayError("minComCap > maxComCap");

    if (maxCommCapac > minCommVol)
        displayError("maxComCap > minComVol");

    if (maxCommCapac > minArcCapac)
        displayError("maxComCap > minArcCap");

    if (maxArcCapac > numbCommods * minCommCapac)
        displayError("maxArcCap > numbCom * minComCap");

    if (oneSourceOneSink)
    {
        if (equSourcesEquSinks)
        {
            displayError("oneSrcOneSnk and eqSrcsEqSnks cannot be both active.");
        }
        if (gridConnect)
        {
            if ((lengthX * lengthY) * ((lengthX * lengthY) - 1) < numbCommods)
                displayError("(lengthX * lengthY)* (lengthX * lengthY - 1) < numbCom");
        }
        else
        {
            if (numbNodes * (numbNodes - 1) < numbCommods)
                displayError("numbNodes * (numbNodes - 1) < numbCom");
        }
    }
    else
    {
        if (minNumbSources > maxNumbSources)
            displayError("minNbSrc > maxNbSrc");

        if (minNumbSinks > maxNumbSinks)
            displayError("minNbSnk > maxNbSnk");

        if (gridConnect)
        {
            if (maxNumbSources + maxNumbSinks > lengthX * lengthY)
                displayError("maxNbSrc + maxNbSnk > lengthX * lengthY");
        }
        else
        {
            if (maxNumbSources + maxNumbSinks > numbNodes)
                displayError("maxNbSrc + maxNbSnk > numbNodes");
        }
    }
    if (gridConnect)
    {
        if (lengthX * lengthY < 2)
            displayError("lengthX * lengthY < 2");
    }
    else
    {
        if (numbNodes < 2)
            displayError("numbNodes < 2");
    }
		
	
	
}

// Verifies that output path is valid.
void Data::verifyOutputPath(string fullPath)
{
	size_t last_sep = fullPath.find_last_of("/");

	if(last_sep != string::npos)
	{
		string folderPath = fullPath.substr(0, last_sep + 1);	
	
		if (!filesystem::exists(folderPath) || !filesystem::is_directory(folderPath))			
		{			
			cerr << "\nERROR: " << endl;    
			cerr << "Output file path " << fullPath << " was specified.\n" 
				<< "However, folder " << folderPath << " is unreachable.\n"
				<< "Make sure to create individual folders on output file path\nand to grant sufficient access rights." << endl;
			cerr << "EXECUTION ABORTED\n" << endl;
			exit(1);
		}
	}	
}

// Methods reading parameter values from parameter file and instruction line
// (first parameter is name used on the instruction line,
// second parameter is name used in parameter file)

template <class T>
void Data::readParam(string nameOnCommLine, string nameInFile, T &var, const T minVal, const T maxVal)
{
    paramProcessing.assignParamValue(nameOnCommLine, nameInFile, var);
    verifyParam(nameInFile, var, minVal, maxVal);
}

template <class T>
void Data::readParam(string nameOnCommLine, string nameInFile, bool &flag, T &var, const T minVal, const T maxVal)
{
    flag = paramProcessing.assignParamValue(nameOnCommLine, nameInFile, var);
    verifyParam(nameInFile, var, minVal, maxVal);
}

void Data::readParam(string nameOnCommLine, string nameInFile, string &var)
{
    paramProcessing.assignParamValue(nameOnCommLine, nameInFile, var);
}

void Data::readParam(string nameOnCommLine, string nameInFile, char *var)
{
    paramProcessing.assignParamValue(nameOnCommLine, nameInFile, var);
}

void Data::readParam(string nameOnCommLine, string nameInFile, bool &var)
{
    paramProcessing.assignParamValue(nameOnCommLine, nameInFile, var);
}

void Data::printHelpAndExit()
{

    printf("\nHELP REQUESTED: DISPLAYING DOCUMENTATION\n");
    //
    printf("\nGENERATOR OF INSTANCES OF THE LINEAR, DETERMINISTIC MULTI-COMMODITY,\n");
    printf("FIXED CHARGE, CAPACITATED NETWORK DESIGN PROBLEM\n");
    //
    printf("\nThis application randomly generates instances of the linear deterministic\n");
    printf("multi-commodity, capacitated, fixed charge network design (MCFND) problem.\n");
    printf("Details of the process are governed by user-specified configuration\n");
    printf("settings.\n");
    //
    printf("\nFor detailed background, see\n");
    printf("Larsen, E., Bisaillon, S., Cordeau, J.F. and Frejinger, E.\n");
    printf("Pseudo-random Instance Generators in C++ for Deterministic and Stochastic\n");
    printf("Multi-commodity Network Design Problems.\n");
    //
    printf("\n1 Pseudo-random generation and available topologies\n");
    printf("***************************************************\n");
    //
    printf("\nAll pseudo-random sampling operations occurring in the program are performed\n");
    printf("with the permuted congruential generator (PCG). (See O'Neill, M. PCG, a\n");
    printf("family of better random number generators.\n");
    printf("https://www.pcg-random.org/, 2023.) All generated numerical characteristics\n");
    printf("are integer-valued: commodity volumes, fixed costs, overall capacities,\n");
    printf("commodity-specific variable costs, commodity-specific capacities.\n");
    //
    printf("\nCommodity volumes are pseudo-randomly generated independently and uniformly\n");
    printf("between minComVol and maxComVol.\n");
    //
    printf("\nThe topology of the graph can either be based on a grid (the latter can be\n");
    printf("laid either on a torus, a cylinder or a flat bounded surface), on a circle,\n");
    printf("be explicited in a file, or be entirely random generated. Under the toral\n");
    printf("topology, non-random arcs are initially created to build the structure of\n");
    printf("a torus with major and minor circumferences respectively equal to lengthX\n");
    printf("and lengthY. Under the cylindrical topology, non-random arcs are initially\n");
    printf("created to build the structure of a cylinder with circumference equal to\n");
    printf("lengthX and height equal to lengthY. Under the flat topology, non-random\n");
    printf("arcs are initially created to build the structure of a bounded flat surface\n");
    printf("of width and height respectively equal to lengthX and lengthY. Under the\n");
    printf("circular topology, non-random arcs are initially created in order to chain\n");
    printf("the nodes. A basic graph (including nodes and arcs but excluding costs and\n");
    printf("capacities) can also be read from a file. Likewise, non-random arcs are\n");
    printf("initially created. Each one of the aforementioned non-random arcs has\n");
    printf("overall capacity equal to total volume and fixed cost equal to maxFixedCos.\n");
    printf("Over the non-random arcs, commodity-specific capacities are equal to\n");
    printf("commodity volumes and commodity-specific variable costs are equal to\n");
    printf("maxVarCos.\n");
    //
    printf("\nAddition of random arcs to the aforementioned structures can be performed\n");
    printf("in either one or both of the two following ways: (i) A specified number of\n");
    printf("additional arcs whose head and tail are generated pseudo-randomly. (ii) A\n");
    printf("specified number of additional arcs starting from each existing node are\n");
    printf("created whereas only their heads are generated pseudo-randomly. Each one of\n");
    printf("the arcs in (i) and (ii) has fixed cost pseudo-randomly generated\n");
    printf("independently and uniformly between minFixCos and maxFixCos and overall\n");
    printf("capacity pseudo-randomly generated independently and uniformly between\n");
    printf("minArcCap and maxArcCap. The commodity-specific variable costs and\n");
    printf("capacities are pseudo-randomly generated independently and uniformly between\n");
    printf("minVarCos and maxVarCos and between minComCap and maxComCap, respectively.\n");
    //
	printf("\n2 Building and running the program directly from the Linux command line\n");
	printf("***********************************************************************\n");
	//
	printf("\n2.1 Building the program from the Linux command line\n");
	printf("****************************************************\n");
	//
	printf("\nFollow these steps to build the program:\n");
	//
	printf("\na- Either (i) use Git to clone locally the remote repository\n");
	printf("https://github.com/larseeri/determ-MCFNDP-gener, or (ii) generate, download\n");
	printf("locally to an appropriate folder and uncompress a .zip archive containing\n");
	printf("the remote repository file tree.\n");
	printf("b- In a Linux terminal, go to the /program subfolder just created.\n");
	printf("c- Make sure that the gcc C++ compiler and the make C++ build tool are\n");
	printf("available.\n");
	printf("d- Run this instruction to build the program. An executable file named\n");
	printf("'determ_gen' will be created inside the /program folder.\n");
	//
	printf("\nmake\n");
	//
	printf("\nBuilding with versions 11.5.0, 12.2.0, 13.2.0, 14.2.0 and 15.1.0 of the gcc\n");
	printf("compiler has been verified.\n");
	//
	printf("\n2.2 Running the program with Linux command line instructions\n");
	printf("************************************************************\n");
	//
	printf("\nThe program can be run as follows from the /program folder just created\n");
	printf("along with default option parameter values:\n");
	//
	printf("\n./determ_gen\n");
	//
	printf("\nOptions for running the program can be specified directly in the command\n");
	printf("line or in a configuration file. By default, the program will read option\n");
	printf("values from file determ_config.par. (This default setting can be updated by\n");
	printf("modifying line 792 of file main.cpp.) The default file name can be\n");
	printf("overridden with option +F:\n");
	//
	printf("\n./determ_gen +F newParamfile.par\n");
	//
	printf("\nThis assumes that determ_config.par and newParamfile.par are located at the\n");
	printf("root of the /program folder. Otherwise, their absolute paths or their paths\n");
	printf("relative to the /program folder must be explicited.\n");
	//
	printf("\nThe default configuration file determ_config.par has initially been filled\n");
	printf("with illustrative values and and should be edited appropriately if no\n");
	printf("replacement configuration file is indicated on the command line with option\n");
	printf("key +F.\n");
	//
	printf("\nVerbosity option +v should be used to obtain more information about all\n");
	printf("options that have been set in the parameter file or in the command line:\n");
	//
	printf("\n./determ_gen +v +F newParamfile.par\n");
	//
	printf("\nA configuration file should have this format:\n");
	//
	printf("\noptionKey1   <value>\n");
	printf("optionKey2   <value>\n");
	printf(".\n");
	printf(".\n");
	printf("optionKeyN   <value>\n");
	//
	printf("\nIt may specify at most one option per line. Option keys may be prefixed or\n");
	printf("not with '-'. Comments may appear either at the beginning of a line, in\n");
	printf("which case they must start with a non-alphabetic character (but not with\n");
	printf("'-'), or at the end of a line.\n");
	//
	printf("\nOnce the configuration file has been processed, options appearing in the\n");
	printf("command line (other than +F and +v) are processed. Option values specified\n");
	printf("in the command line supersede those specified in the configuration file.\n");
	printf("That is, if included, they replace, modify and supplement the latter.\n");
	printf("Option keys appearing in the command line must be prefixed with '-' as\n");
	printf("follow:\n");
	//
	printf("\n-optionkeyA valueA -optionKeyB valueB...\n");
	//
	printf("\nBy default, boolean values are set to false and presence of option key is\n");
	printf("sufficient to set the corresponding value to true. For other options\n");
	printf("expecting string, integer or double values, values should be specified.\n");
	//
	printf("\nExample of a valid command line:\n");
	//
	printf("\n./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10\n");
	//
	printf("\nIn this example, the final value for parameters stream, seed and\n");
	printf("numbCom will be retrieved from command line, and all other parameters\n");
	printf("appearing in file newParamfile.par will have their value taken from there.\n");
	printf("Parameters that appear neither in command line, nor in this parameter file\n");
	printf("will be set to the initialization values hard-coded in lines 28-41 of\n");
	printf("data.cpp.\n");
	//
	printf("\nSetting a parameter value more than once in a location will not cause an\n");
	printf("error. In this case, the last value assigned will be retained. For example,\n");
	printf("from the command line\n");
	//
	printf("\n./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10 -seed 1212\n");
	//
	printf("\nthe value retained for option parameter seed will be 1212 (not 4567).\n");
	//
	printf("\nTo display the complete documentation about the application and the\n");
	printf("available options, run\n");
	//
	printf("\n./determ_gen -help\n");
	//
	printf("\nSumming up, option values are successively updated from these locations in\n");
	printf("this order:\n");
	printf("(i) hard-coded initialization values in lines 28-41 of data.cpp\n");
	printf("(ii) values appearing in configuration file whose name is specified in\n");
	printf("command line with +F (if it is specified and if it exists); if no valid\n");
	printf("configuration file is specified with +F in command line, values will be\n");
	printf("taken from default configuration file whose name is specified in line 776 of\n");
	printf("main.cpp; at this point, the latter should exist\n");
	printf("(iii) values specified in the command line.\n");
	printf("Notice that location (i) is insufficiently transparent to the user and\n");
	printf("should not be relied upon. Parameter values should be set through (ii) and\n");
	printf("(iii).\n");
	//
	printf("\nRemark:\n");
	printf("See Section 5 below for examples of configuration files and instructions\n");
	printf("when the program is run directly from the Linux command line.\n");
	//
	printf("\n2.2.1 Reading and writing the basic graph from and to a file\n");
	printf("************************************************************\n");
	//
	printf("\nSetting topology option to 5 instructs the program to read the basic graph\n");
	printf("(nodes + arcs without costs and capacities) from a plain text file. The\n");
	printf("default name of this file is basic_graph.gra. An alternative long name\n");
	printf("(including extension) may be specified with the option\n");
	printf("longBasicGraphFileName. To be read correctly, this file must be structured\n");
	printf("as follows:\n");
	//
	printf("\n(beginning of file)\n");
	printf("Two blank or comment rows (these are disregarded when reading)\n");
	printf("low = lowNodeNumber (lowNodeNumber is smallest node number appearing in rows below)\n");
	printf("high = highNodeNumber (highNodeNumber is highest node number appearing in rows below)\n");
	printf("Three blank or comment rows (these are disregarded when reading)\n");
	printf("origNodeNumber destNodeNumber (a row identifies an arc from origin to destination)\n");
	printf("origNodeNumber destNodeNumber\n");
	printf("origNodeNumber destNodeNumber\n");
	printf(".\n");
	printf(".\n");
	printf(".\n");
	printf("(end of file)\n");
	//
	printf("\nWhen topology is set between 1 and 4, the program will save the basic graph\n");
	printf("(i.e., including nodes and arcs, but excluding random arcs, costs and\n");
	printf("capacities) in a plain text file whose default name, basicGraph.gra, may be\n");
	printf("replaced using the parameter longBasicGraphFileName. The name specified\n");
	printf("using the latter should include the extension. The content of the file will\n");
	printf("appear like this:\n");
	//
	printf("\n(beginning of file)\n");
	printf("1 Nodes\n");
	printf("Nodes are identified with integers in closed interval [low, high].  Program will perform appropriate renumbering.\n");
	printf("low = 0\n");
	printf("high = 11\n");
	//
	printf("\n2 Arcs\n");
	printf("NodeFrom  NodeTo\n");
	printf("0  1\n");
	printf("0  2\n");
	printf("0  3\n");
	printf("0  9\n");
	printf(".\n");
	printf(".\n");
	printf(".\n");
	printf("(end of file)\n");
	//
	printf("\nRemark:\n");
	printf("See Section 5 below for examples of basic graph files.\n");
	//
	printf("\n3 Running the program through Docker instructions\n");
	printf("*************************************************\n");
	//
	printf("\nAn advantage of running the executable determ_gen inside a Docker container\n");
	printf("is the possibility to avoid selection and installation of suitable C++\n");
	printf("compiler, C++ make build tool and required C++ libraries. It suffices that\n");
	printf("the Docker engine be installed on the host where the computation will be\n");
	printf("run. For this, see https://docs.docker.com/engine/install/.\n");
	//
	printf("\nOnce the Docker engine is installed, follow these steps:\n");
	//
	printf("\na- Open a terminal and retrieve from the GitHub container registry the\n");
	printf("Docker image ghcr.io/larseeri/determ_gen_image:latest as follows:\n");
	//
	printf("\ndocker pull ghcr.io/larseeri/determ_gen_image:latest\n");
	//
	printf("\n(From this image, Docker containers running the executable 'determ_gen' will\n");
	printf("be generated.)\n");
	printf("b- For simplicity, define the alias 'determ_gen_image' through\n");
	//
	printf("docker tag ghcr.io/larseeri/determ_gen_image:latest determ_gen_image\n");
	//
	printf("\nRemark:\n");
	printf("If the Docker image determ_gen_image were instead available as the tar ball\n");
	printf("determ_gen_image.tar, then it could be loaded through\n");
	//
	printf("\ndocker load --input determ_gen_image.tar\n");
	//
	printf("\nc- Either (i) use Git to clone locally the remote repository\n");
	printf("https://github.com/larseeri/determ-MCFNDP-gener, or (ii) generate, download\n");
	printf("locally to an appropriate folder and uncompress a .zip archive containing\n");
	printf("the remote repository file tree. The /docker folder contains (i) the\n");
	printf("primitive files named 'Dockerfile' and 'makefile' based on which the\n");
	printf("Docker image was originally generated, (ii) a subfolder /inout used\n");
	printf("for exchanging files between the program running inside a Docker container\n");
	printf("and the directory structure of the host, (iii) some auxiliary files, and\n");
	printf("(iv) the subfolder /examples containing a number of configuration files and\n");
	printf("their resulting outputs (examined in Section 5 below).\n");
	printf("d- In a terminal, go to the /docker folder just created.\n");
	//
	printf("\nThen, instructions similar to those appearing in Section 2.2 can be run\n");
	printf("from the terminal on AMD and Intel CPUs by adding this prefix (other CPUs\n");
	printf("are adressed further below):\n");
	//
	printf("\ndocker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image\n");
	//
	printf("\nand by replacing the executable location ./determ_gen with /determ_gen (i.e.\n");
	printf("removing the initial dot), since the executable determ_gen is located at the\n");
	printf("root of the Docker container being generated and since locations inside a\n");
	printf("Docker container must be accessed through absolute paths.\n");
	//
	printf("\nHence, the following Linux command line instructions:\n");
	//
	printf("\n./determ_gen\n");
	//
	printf("\n./determ_gen +v +F newParamfile.par\n");
	//
	printf("\n./determ_gen -help\n");
	//
	printf("\n./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10\n");
	//
	printf("\nrespectively correspond to the following Docker instructions:\n");
	//
	printf("\ndocker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen\n");
	//
	printf("\ndocker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen \\\n");
	printf("+v +F /inout/newParamfile.par\n");
	//
	printf("\ndocker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout \\\n");
	printf("determ_gen_image /determ_gen -help\n");
	//
	printf("\ndocker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen \\\n");
	printf("+F /inout/newParamfile.par -stream 1234 -seed 4567 -numbCom 10\n");
	//
	printf("\nThe default configuration file is located at ./inout/determ_config.par. It\n");
	printf("has initially been filled with illustrative values and it should imperatively\n");
	printf("be edited appropriately if no replacement configuration file is indicated on\n");
	printf("the Docker instruction line with option key +F.\n");
	//
	printf("\nOption parameter values are successively updated from these locations in\n");
	printf("this order:\n");
	printf("(i) initialization values hard-coded in lines 28-41 of data.cpp,\n");
	printf("(ii) either replacement configuration file specified in the Docker\n");
	printf("instruction line with option key +F, or default configuration file\n");
	printf("./inout/determ_config.par if no replacement configuration file is\n");
	printf("specified in the Docker instruction line with option key +F,\n");
	printf("(iii) values specified in the Docker instruction.\n");
	printf("Notice that location (i) is insufficiently transparent to the user and\n");
	printf("should not be relied upon. Parameter values should be set through (ii) and\n");
	printf("(iii).\n");
	//
	printf("\nImportant:\n");
	printf("The supplied Docker image conforms to the amd64 CPU instructions standard\n");
	printf("and will run natively on AMD and Intel CPUs. On CPUs conforming instead to\n");
	printf("the arm64 standard, e.g., on recent Apple computers, the supplied Docker\n"); 
	printf("image can be run through emulation. Essentially, this may be achieved as\n");
	printf("follows:\n");
	printf("a- Running the following preliminary instruction once. (For details, see\n");
	printf("https://hub.docker.com/r/tonistiigi/binfmt.)\n");
	//
	printf("\ndocker run --privileged --rm tonistiigi/binfmt --install all\n"); 
	//
	printf("\nb- Adding the option --platform linux/amd64 to the Docker instruction line\n");
	printf("prefix as follows:\n");
	//
	printf("\ndocker run --platform linux/amd64 --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image\n");
	//
	printf("\nThe following characteristics are specific to the operation of the program\n");
	printf("through Docker instructions:\n");
	printf("- An empty folder absolutePathToDockerFolder/inout must already exist on\n");
	printf("the host for the Docker instructions to work correctly.\n");
	printf("- Option --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout\n");
	printf("connects subfolder docker/inout on the host to folder /inout\n");
	printf("of the Docker container. Hence, the connection between\n");
	printf("absolutePathToDockerFolder/inout on the host and folder /inout in the\n");
	printf("container is used to supply input files to and retrieve output files from\n");
	printf("the container.\n");
	printf("- Option --rm ensures suppression of the container implicitly generated by\n");
	printf("docker run. Note that the container created by docker run for running the\n");
	printf("executable determ_gen stops running immediately after the latter finishes\n");
	printf("and that it cannot be reused afterwards, for instance through a docker exec\n");
	printf("instruction. Immediate suppression with --rm avoids uselessly cluttering\n");
	printf("Docker's container repository.\n");
	//
	printf("\nThe following Docker instructions may be useful:\n");
	//
	printf("\ndocker image ls (list all currently existing docker images)\n");
	//
	printf("\ndocker image rm --force imageName (forcefully remove imageName)\n");
	//
	printf("\ndocker container ls (list all running containers)\n");
	//
	printf("\ndocker container ls --all (list all containers, running or not)\n");
	//
	printf("\nSee Section 5 below for examples of configuration files and instructions\n");
	printf("when the program is run through Docker.\n");
	//
    printUsageAndExit(0, true);
    exit(1);
}

void Data::printUsageAndExit(char ExecName[], bool anonymous)
{
    if (!anonymous)
    {
        printf("\nUsage: %s [options]\n", ExecName);
    }

    printf("\n4 Usage of options\n");
    printf("******************\n");
    //
    printf("\nNotation:\n");
    printf("For the purposes of the current discussion, <...> indicates that some value\n");
    printf("or values of the type specified inside the brackets is expected. For\n");
    printf("instance, <string> means that a string is expected whereas <> means that no\n");
    printf("value is expected.\n");
    //
    printf("\n4.1 Options available in instruction line only (Linux or Docker)\n");
    printf("****************************************************************\n");
    //
    printf("\n+F <string>; name of configuration file supplying option parameter values;\n");
    printf("this replaces the default file whose name is hard-coded in line 738 of\n");
    printf("main.cpp; any option value supplied in instruction line replaces value of\n");
    printf("same option supplied in configuration file\n");
    //
    printf("\n+v <> (no value supplied); requests verbosity while parsing options in\n");
    printf("instruction line or configuration file\n");
    //
    printf("\nRemark:\n");
    printf("An unknown option key following '+' in the instruction line is disregarded.\n");
    //
    printf("\n4.2 Options available in instruction line (Linux or Docker) and in configuration file\n");
    printf("*************************************************************************************\n");
    //
    printf("\nRemarks:\n");
    printf("- When the following options are called from the instruction line, dash\n");
    printf("prefixes '-' must be present. The latter can be omitted when the options are\n");
    printf("called from a configuration file.\n");
    printf("- To supply an option, a line of a configuration file must begin with a dash\n");
    printf("'-' or a letter. A line beginning with any other character will be\n");
    printf("considered a comment.\n");
    printf("- The remainder of a line beyond the stated option and its value (if any is\n");
    printf("expected) is disregarded and may therefore be used for adding comments.\n");
    printf("- Unknown option keys following '-' in the instruction line are disregarded.\n");
    printf("- Unknown option keys (following '-' or not) at beginning of lines in a\n");
    printf("configuration file are disregarded.\n");
    //
    printf("\n-help <> (no value supplied); display help message\n");
    //
    printf("\n-shortOutFileName <string>; short name (without extension) of output file;\n");
    printf("default value is './outfile' (Linux instructions) or './inout/outfile'\n");
    printf("(Docker instructions); extension will differ according to format:\n");
    printf("dow, std (a.k.a. Canad format), lp, mps, tex; when using Linux instructions,\n");
    printf("absolute path or path relative to the /program folder may be added\n");
    //
    printf("\nRemark:\n");
    printf("DOW, STD and TEX formats describe the generated network; DOW and STD\n");
    printf("(a.k.a. Canad format) are text formats specified in files\n");
    printf("DOW-format-desc.txt and STD-format-desc.txt; DOW imposes that (i) for each\n");
    printf("arc, capacities and variable costs are identical for all commodities and\n");
    printf("that (ii) each commodity has a single source node (at which volume > 0) and\n");
    printf("a single sink node (at which volume < 0); TEX format is a graphical\n");
    printf("representation that can be processed by a LaTeX text and graphics compiler;\n");
    printf("LP (human readable) and MPS formats describe the MCFND linear programming\n");
    printf("problem resulting from the generated network\n");
    //
    printf("\n-mpsOut <> (no value supplied); save output with MPS format\n");
    //
    printf("\n-lpOut <> (no value supplied); save output with human readable LP format\n");
    //
    printf("\n-texOut <> (no value supplied); generate and save LaTeX code representing\n");
    printf("network; such representations are unavailable (i) if lengthX > 10 or\n");
    printf("lengthY > 10, when 1 <= topology <= 3, (ii) if numbNodes > 100, when\n");
    printf("topology == 0 or topology >= 4, (iii) if numbCom > 15; other specific\n");
    printf("conditions depending on values of oneSrcOneSnk, lengthX, lengthY,\n");
    printf("numbNodes and numbCom are managed and reported by the program; ulterior\n");
    printf("processing of the TEX file by a LaTeX compiler may require up to several\n");
    printf("minutes depending on complexity.\n");
    //
    printf("\n-noStdOut <> (no value supplied); do not save output with STD format\n");
    //
    printf("\n-stream <int: 0, INT_MAX>; random stream\n");
    //
    printf("\n-seed <int: 0, INT_MAX>; random seed\n");
    //
    printf("\n-topology <int: 0, 1, 2, 3, 4, 5>; 0 calls for a graph topology that is\n");
    printf("entirely random and where arcs are set according to nbAddArcSampTailHead\n");
    printf("and/or nbAddArcPerSampHead; 1 to 5 call for a particular basic graph\n");
    printf("topology (before additional, random arcs are eventually added according to\n");
    printf("nbAddArcSampTailHead and/or nbAddArcPerSampHead): 1 grid on a torus, 2 grid\n");
    printf("on a cylinder, 3 grid on a bounded surface, 4 circular, 5 read (nodes + arcs\n");
    printf("without costs and capacities) from file\n");
    //
    printf("\n-longBasicGraphFileName <string>; long name (including extension) of file\n");
    printf("where basic graph (excluding random arcs, capacities and costs ) will be\n");
    printf("read if topology == 5 and will be written if 1 <= topology == 4; default\n");
    printf("value is './basic_graph.gra' (Linux instructions) or\n");
    printf("'./inout/basic_graph.gra' (Docker instructions); when using Linux\n");
    printf("instructions, absolute path or path relative to the /program folder may be\n");
    printf("added\n");
    //
    printf("\n-lengthX <int: 1, INT_MAX>; length along X axis under grid topology\n");
    printf("(inactive otherwise)\n");
    //
    printf("\n-lengthY <int: 1, INT_MAX>; length along Y axis under grid topology\n");
    printf("(inactive otherwise)\n");
    //
    printf("\n-numbNodes <int: 1, INT_MAX>; number of nodes under random or circular\n");
    printf("topology; (overriden by lengthX * lengthY under grid topology; also\n");
    printf("overriden when reading basic graph from file)\n");
    //
    printf("\n-oneSrcOneSnk <> (no value supplied); select one source and one sink for\n");
    printf("each commodity; (source, sink) pairs must be different across all\n");
    printf("commodities; remove differentiation between variable costs across individual\n");
    printf("commodities, prior to saving in any format DOW, STD, LP, MPS or TEX; reduce\n");
    printf("as follows differentiation between capacities across individual commodities,\n");
    printf("prior to saving in any format DOW, LP, STD, MPS or TEX: for all k and a,\n");
    printf("capacity for commodity k over arc a is equated to minimum of volume of\n");
    printf("commodity k and overall capacity of arc a; will be saved at least using DOW\n");
    printf("format\n");
    //
    printf("\n-eqSrcsEqSnks <> (no value supplied); impose identical sources over all\n");
    printf("commodities and identical sinks over all commodities\n");
    //
    printf("\nRemark:\n");
    printf("oneSrcOneSnk and eqSrcsEqSnks cannot be both active; if oneSrcOneSnk and\n");
    printf("eqSrcsEqSnks are both inactive, then sources and sinks are selected\n");
    printf("randomly\n");
    //
    printf("\n-inclDesignObjCap <> (no value supplied); when saving in LP or MPS format,\n");
    printf("ensure that (i) design variables will appear with fixed costs in objective,\n");
    printf("(ii) design variables will appear on the RHS of arc-capacity constraints\n");
    //
    printf("\n-inclDesignObjCommCap <> (no value supplied); when saving in LP or MPS\n");
    printf("format, ensure that (i) design variables will appear with fixed costs in\n");
    printf("objective, (ii) when there is a single commodity, design variables will\n");
    printf("appear on the RHS of arc-capacity constraints, (iii) when there are multiple\n");
    printf("commodities, commodity-specific capacity constraints (a.k.a. strong forcing\n");
    printf("constraints) will be included and design variables will appear on their\n");
    printf("RHS\n");
    //
    printf("\nRemark:\n");
    printf("In constrast with DOW, STD and TEX formats, LP and MPS formats describe the\n");
    printf("mixed integer linear program (MILP) solving the generated instance of MCFND\n");
    printf("problem; hence, inclDesignObjCap and inclDesignObjCommCap determine the\n");
    printf("particular forms taken by the LP and MPS representations (see\n");
    printf("Graph::outputLP() and Graph::outputMPS() in graph.cpp)\n");
    //
    printf("\n-noComCaps <> (no value supplied); reduce as follows differentiation\n");
    printf("between capacities across individual commodities, prior to saving in\n");
    printf("any format DOW, LP, STD, MPS or TEX: for all k and a, capacity for\n");
    printf("commodity k over arc a is equated to minimum of volume of commodity k\n");
    printf("and overall capacity of arc a\n");
    //
    printf("\n-numbCom <int: 0, INT_MAX>; number of commodities\n");
    //
    printf("\n-nbAddArcSampTailHead <int: 0, INT_MAX>; total number of additional random\n");
    printf("arcs generated by sampling over tail (origin) and head (destination) nodes;\n");
    printf("if this option is inactive, default value is 0\n");
    //
    printf("\n-nbAddArcPerSampHead <int: 0, INT_MAX>; number of additional random arcs\n");
    printf("per tail (origin) node generated by sampling over head (destination) nodes;\n");
    printf("if this option is inactive, default value is 0\n");
    //
    printf("\n-minNbSrc <int: 1, INT_MAX>; min number of sources; inactive under\n");
    printf("oneSrcOneSnk\n");
    //
    printf("\n-maxNbSrc <int: 1, INT_MAX>; max number of sources; inactive under\n");
    printf("oneSrcOneSnk\n");
    //
    printf("\n-minNbSnk <int: 1, INT_MAX>; min number of sinks; inactive under\n");
    printf("oneSrcOneSnk\n");
    //
    printf("\n-maxNbSnk <int: 1, INT_MAX>; max number of sinks; inactive under\n");
    printf("oneSrcOneSnk\n");
    //
    printf("\n-minComVol <int: 1, INT_MAX>; min commodity volume\n");
    //
    printf("\n-maxComVol <int: 1, INT_MAX>; max commodity volume\n");
    //
    printf("\n-minVarCos <int: 0, INT_MAX>; min commodity-specific variable cost\n");
    //
    printf("\n-maxVarCos <int: 0, INT_MAX>; max commodity-specific variable cost\n");
    //
    printf("\n-minArcCap <int: 1, INT_MAX>; min arc overall capacity\n");
    //
    printf("\n-maxArcCap <int: 1, INT_MAX>; max arc overall capacity\n");
    //
    printf("\n-minFixCos <int: 0, INT_MAX>; min fixed cost\n");
    //
    printf("\n-maxFixCos <int: 0, INT_MAX>; max fixed cost\n");
    //
    printf("\n-minComCap <int: 0, INT_MAX>; min commodity-specific arc capacity\n");
    //
    printf("\n-maxComCap <int: 0, INT_MAX>; max commodity-specific arc capacity\n");
    //
    printf("\n-ratZeroFix <int: 0, 100>; percent of random arcs with fixed cost\n");
    printf("set to zero; if this option is inactive, default percentage is 0\n");
    //
    printf("\n-ratTotVolCap <int: 0, 100>; percent of random arcs with total\n");
    printf("capacity set to total volume; if this option is inactive, default\n");
    printf("percentage is 0\n");
    //
    printf("\n-ratZeroComCap <int: 0, 100>; percent of random arcs with one of the\n");
    printf("commodity-specific capacities set to zero; if this option is inactive,\n");
    printf("default percentage is 0\n");
    //
    printf("\n-ratMaxComCap <int: 0, 100>; percent of random arcs with one of the\n");
    printf("commodity-specific capacities set to maxComCap; if this option is inactive,\n");
    printf("default percentage is 0\n");
    //
    printf("\n-noParalArc <int: 1, INT_MAX>; when adding random arcs, avoid parallel arcs\n");
    printf("and set to this figure the maximum number of failed attempts to add a non-\n");
    printf("parallel arc until number of random arcs to be added is decreased by one;\n");
    printf("if this option is inactive, no attempt will be made to avoid parallel arcs\n");
    //
    printf("\n-adjCapacsDown <int: 1, INT_MAX>; require uniform downward adjustment of\n");
    printf("overall capacities of all arcs (non-random and random) whose magnitude is\n");
    printf("proportional to the specified figure (see Graph::adjustArcCapacities() in\n");
    printf("graph.cpp); if this option is inactive, no adjustment will be made\n");
    //
    printf("\nRemark:\n");
    printf("Setting adjCapacsDown to 1 is not equivalent to no adjustment\n");
    //
    printf("\n-adjFixCostsUp <int: 1, INT_MAX>; require uniform upward adjustment of\n");
    printf("fixed costs of all arcs (non-random and random) whose magnitude is\n");
    printf("proportional to the specified figure (see Graph::adjustFixedCosts() in\n");
    printf("graph.cpp); if this option is inactive, no adjustment will be made\n");
    //
    printf("\nRemark:\n");
    printf("Setting adjFixCostsUp to 1 is not equivalent to no adjustment\n");
    //
    printf("\n4.3 Joint restrictions\n");
    printf("**********************\n");
    //
    printf("\nif oneSrcOneSnk\n");
    printf("{\n");
    printf("     if 1 <= topology <= 3\n");
    printf("         (lengthX * lengthY) * (lengthX * lengthY - 1) >= numbCom\n");
    printf("     else\n");
    printf("         numbNodes * (numbNodes - 1) >= numbCom\n");
    printf("}\n");
    printf("else\n");
    printf("{\n");
    printf("     minNbSrc <= maxNbSrc\n");
    printf("     minNbSnk <= maxNbSnk\n");
    //
    printf("\n     if 1 <= topology <= 3\n");
    printf("     {\n");
    printf("         maxNbSrc + maxNbSnk <= lengthX * lengthY\n");
    printf("     }\n");
    printf("     else\n");
    printf("     {\n");
    printf("         maxNbSrc + maxNbSnk <= numbNodes\n");
    printf("     }\n");
    printf("}\n");
    //
    printf("\nminComVol <= maxComVol\n");
    printf("minVarCos <= maxVarCos\n");
    printf("minFixCos <= maxFixCos\n");
    printf("minArcCap <= maxArcCap\n");
    printf("minComCap <= maxComCap\n");
    //
    printf("\nmaxComCap <= minComVol\n");
    printf("maxComCap <= minArcCap\n");
    printf("maxArcCap <= numbCom * minComCap\n");
    //
    printf("\nif 1 <= topology <= 3\n");
    printf("     lengthX * lengthY > 1\n");
    printf("else\n");
    printf("     numbNodes > 1\n");
    //
    printf("\n5 Examples\n");
    printf("**********\n");
    //	
	printf("\nThe program/examples subfolder holds examples of configuration files and\n"); 
	printf("instruction lines occurring when the executable of the program is run\n");
	printf("directly from the Linux instruction line. It also contains the output files\n");
	printf("resulting from each pair of configuration file and instruction line.\n");
	//	
	printf("\nSimilarly, the docker/examples subfolder holds examples of configuration\n");
	printf("files and instruction lines occurring when the executable of the program is\n");
	printf("run through Docker instructions. These examples are identified by the\n");
	printf("addition of the prefix 'docker_'. The subfolder also contains the output\n");
	printf("files resulting from each pair of configuration file and instruction line.\n");
	printf("The former are identical to the files generated when the executable is run\n");
	printf("from the Linux instruction line.\n");
	//
	printf("\nRemark:\n");
	printf("The output examples contained in the /examples subfolders will also be used\n");
	printf("to illustrate the operation of the separate application generating stochastic\n");
	printf("MCFND instances.\n");
    //
    printf("\nIn all included examples, the generated MCFND instance is characterized by a\n");
    printf("(3 x 4) grid topology applied to a bounded surface and by 3 commodities.\n");
    //
    printf("\n5.1 One source and one sink for each commodity\n");
    printf("**********************************************\n");
    //
    printf("\nFolder ./examples/oneSrcOneSnk/\n");
    //
    printf("\nIn this case, there is one source and one sink for each commodity and no\n");
    printf("differentiation of variable costs and capacities according to commodities.\n");
    printf("The generated networks can be saved in the DOW and STD formats.\n");
    //
    printf("\n5.1.1 Instructions\n");
    printf("******************\n");
    //
    printf("\nLinux command line instruction:\n");
	//
    printf("\n./determ_gen +v +F determ_config_oneSrcOneSnk_10oct2025-18h10.par\n");
    //
    printf("\nDocker instruction:\n");
	//
    printf("\ndocker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen +v \\\n");
    printf("+F /inout/docker_determ_config_oneSrcOneSnk_10oct2025-18h10.par\n");
    //
    printf("\n5.1.2 Configuration files (input)\n");
    printf("*********************************\n");
    //
    printf("\nconfiguration file used with Linux command line:\n");
    printf("determ_config_oneSrcOneSnk_10oct2025-18h10.par\n");
    //
    printf("\nconfiguration file used with Docker instruction:\n");
    printf("docker_determ_config_oneSrcOneSnk_10oct2025-18h10.par\n");
    //
    printf("\n5.1.3 Generated files\n");
    printf("*********************\n");
    //
    printf("\nnumerical representation of the network in DOW format:\n");
    printf("determ_oneSrcOneSnk_10oct2025-18h10.dow\n");
    //
    printf("\nnumerical representation of the network in STD format:\n");
    printf("determ_oneSrcOneSnk_10oct2025-18h10.std\n");
    //
    printf("\nnumerical representation of the basic graph:\n");
    printf("determ_oneSrcOneSnk_10oct2025-18h10.gra\n");
    //
    printf("\nhuman-readable representation of the associated linear programming problem\n");
    printf("in LP format:\n");
    printf("determ_oneSrcOneSnk_10oct2025-18h10.lp\n");
    //
    printf("\nnumerical representation of the associated linear programming problem in\n");
    printf("MPS format:\n");
    printf("determ_oneSrcOneSnk_10oct2025-18h10.mps\n");
    //
    printf("\ngraphical representation of the network in TEX format:\n");
    printf("determ_oneSrcOneSnk_10oct2025-18h10.tex\n");
    //
    printf("\ncompiled graphical representation:\n");
    printf("determ_oneSrcOneSnk_10oct2025-18h10.pdf\n");
    //
    printf("\n5.2 Multiple but equal sources and multiple but equal sinks\n");
    printf("***********************************************************\n");
    //
    printf("\nFolder ./examples/eqSrcsEqSnks/\n");
    //
    printf("\nIn this case, there are multiple sources and multiple sinks for each\n");
    printf("commodity. However, sources and sinks are identical across commodities.\n");
    printf("There is differentiation of variable costs and capacities according to\n");
    printf("commodities. The generated networks cannot be saved in the DOW format.\n");
    //
    printf("\n5.2.1 Instructions\n");
    printf("******************\n");
    //
    printf("\nLinux command line instruction:\n");
	//
    printf("\n./determ_gen +v +F determ_config_eqSrcsEqsnks_10oct2025-18h10.par\n");
    //
    printf("\nDocker instruction:\n");
	//
    printf("\ndocker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen +v \\\n");
    printf("+F /inout/docker_determ_config_eqSrcsEqsnks_10oct2025-18h10.par\n");
    //
    printf("\n5.2.2 Configuration files (input)\n");
    printf("*********************************\n");
    //
    printf("\nconfiguration file used with Linux command line:\n");
    printf("determ_config_eqSrcsEqsnks_10oct2025-18h10.par\n");
    //
    printf("\nconfiguration file used with Docker instruction:\n");
    printf("docker_determ_config_eqSrcsEqsnks_10oct2025-18h10.par\n");
    //
    printf("\n5.2.3 Generated files\n");
    printf("*********************\n");
    //
    printf("\nnumerical representation of the network in STD format:\n");
    printf("determ_eqSrcsEqsnks_10oct2025-18h10.std\n");
    //
    printf("\nnumerical representation of the basic graph:\n");
    printf("determ_eqSrcsEqsnks_10oct2025-18h10.gra\n");
    //
    printf("\nhuman-readable representation of the associated linear programming problem\n");
    printf("in LP format:\n");
    printf("determ_eqSrcsEqsnks_10oct2025-18h10.lp\n");
    //
    printf("\nnumerical representation of the associated linear programming problem in\n");
    printf("MPS format:\n");
    printf("determ_eqSrcsEqsnks_10oct2025-18h10.mps\n");
    //
    printf("\ngraphical representation of the network in TEX format:\n");
    printf("determ_eqSrcsEqsnks_10oct2025-18h10.tex\n");
    //
    printf("\ncompiled graphical representation:\n");
    printf("determ_eqSrcsEqsnks_10oct2025-18h10.pdf\n");
    //
    exit(1);
}
