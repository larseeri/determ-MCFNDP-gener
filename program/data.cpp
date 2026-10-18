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
#include "classes.h"

// Constructor
Data::Data(int argc, char **argv, string fileName)
    : // paramProcessing() launches the initial processing of configuration
      // settings: builds key, value maps for parameters specified in files
      // and in instruction line.
    paramProcessing(argc, argv, fileName), secTopology(3), gridConnect(false), toralConnect(true), cylConnect(false),
    flatConnect(false), circConnect(false), randConnect(false), fromFile(false), noParallelArcs(false),
    oneSourceOneSink(false), equSourcesEquSinks(false), noStdOutFile(false), generLpOutFile(false),
    generMpsOutFile(false), generTexOutFile(false), integCapacs(false), integCommCapacs(false), equalizeComCaps(false),
	equalizeVarCosts(false),
    stream(101), seed(101), lengthX(2), lengthY(2), numbNodes(4), numbCommods(1), numbAddRandArcs_SamplTailsHeads(0),
    numbAddRandArcsPerNode_SamplHeads(0), minNumbSources(1), maxNumbSources(1), minNumbSinks(1), maxNumbSinks(1),
    minCommVol(1), maxCommVol(10000), minFixedCost(0), maxFixedCost(10000), minVarCost(0), maxVarCost(10000),
    minArcCapac(1), maxArcCapac(10000), minCommCapac(1), maxCommCapac(10000), adjCapacsDown(false), adjFactCapacs(1),
    adjFixCostsUp(false), adjFactFixCosts(1), ratioZeroFixedCost(0), ratioTotVolCapac(0), ratioZeroCommCapac(0),
    ratioMaxCommCapac(0), maxIterNoParallelArcs(10), shortOutFileName("./outfile"),
    longBasicGraphFileName("./basic_graph.gra"), hubSat(false), minSatsPerHub(2),maxSatsPerHub(4),	
	minSatsRoundSize(1),maxSatsRoundSize(4),	
	numbAddSatRandArcs_SamplTailsHeads(0),numbAddSatRandArcsPerNode_SamplHeads(0),minSatFixedCost(0),maxSatFixedCost(10000),minSatVarCost(0),maxSatVarCost(10000),
	minSatArcCapac(1),maxSatArcCapac(10000),
	minSatCommCapac(1),maxSatCommCapac(10000),adjSatCapacsDown(false),adjSatFactCapacs(1),adjSatFixCostsUp(false),
	adjSatFactFixCosts(1),ratioSatZeroFixedCost(0),ratioSatTotVolCapac(0),ratioSatZeroCommCapac(0),ratioSatMaxCommCapac(0), equalizeSatComCaps(false),
	equalizeSatVarCosts(false)	  
{
    if (paramProcessing.getRequestHelp())
    {
        printHelpAndExit();
    }

    // reading all parameters from file(s) and overriding with instruction line if
    // present therein; see main.cpp and classes.h for details
	
	// selects point-to-point or hubs-and-satellites primary topology
	readParam("hubsAndSats", "hubsAndSats", hubSat);

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	// 1 
	// These parameters are always active (i.e. when option parameter hubsAndSats is
	// either absent, present, explicitly set to true, explicitly set to false)	

	readParam("oneSrcOneSnk", "oneSrcOneSnk", oneSourceOneSink);

    // these are inactive if oneSourceOneSink
    if (!oneSourceOneSink)
    {
        readParam("minNbSrc", "minNbSrc", minNumbSources, 1, 1000);
        readParam("maxNbSrc", "maxNbSrc", maxNumbSources, 1, 1000);
        readParam("minNbSnk", "minNbSnk", minNumbSinks, 1, 1000);
        readParam("maxNbSnk", "maxNbSnk", maxNumbSinks, 1, 1000);
    }

    readParam("eqSrcsEqSnks", "eqSrcsEqSnks", equSourcesEquSinks);
    readParam("stream", "stream", stream, 0, INT_MAX);
    readParam("seed", "seed", seed, 0, INT_MAX);
	readParam("numbCom", "numbCom", numbCommods, 1, 1000);
	readParam("shortOutFileName", "shortOutFileName", shortOutFileName);
	verifyOutputPath(shortOutFileName);
    readParam("longBasicGraphFileName", "longBasicGraphFileName", longBasicGraphFileName);
	verifyOutputPath(longBasicGraphFileName);
    readParam("mpsOut", "mpsOut", generMpsOutFile);
    readParam("lpOut", "lpOut", generLpOutFile);
    readParam("texOut", "texOut", generTexOutFile);
    readParam("inclDesignObjCap", "inclDesignObjCap", integCapacs);
    readParam("inclDesignObjCommCap", "inclDesignObjCommCap", integCommCapacs);
    readParam("equalComCaps", "equalComCaps", equalizeComCaps);
	readParam("equalVarCosts", "equalVarCosts", equalizeVarCosts);
    readParam("noStdOut", "noStdOut", noStdOutFile);
	readParam("minComVol", "minComVol", minCommVol, 1, 10000);
    readParam("maxComVol", "maxComVol", maxCommVol, 1, 10000);
	readParam("noParalArc", "noParalArc", noParallelArcs, maxIterNoParallelArcs, 1, 100);
	
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	// 2 
	// Under point-to-point primary topology (when option parameter hubsAndSats
	// is absent or explicitly set to false), these parameters set all details of the graph.
	// Under hubs-and-satellites (when parameter hubsAndSats is present
	// or explicitly set to true), they set details pertaining to the hubs.	
	
    readParam("secTopology", "secTopology", secTopology, 0, 5);

    // useful to activate only if grid-like secTopology
    if (secTopology >= 1 && secTopology <= 3)
    {
        readParam("lengthX", "lengthX", lengthX, 1, 1000); //EL:
        readParam("lengthY", "lengthY", lengthY, 1, 1000); //EL:
    }  	
	
    readParam("numbNodes", "numbNodes", numbNodes, 1, 10000);    
    readParam("nbAddArcSampTailHead", "nbAddArcSampTailHead", numbAddRandArcs_SamplTailsHeads, 0, 1000);
    readParam("nbAddArcPerSampHead", "nbAddArcPerSampHead", numbAddRandArcsPerNode_SamplHeads, 0, 1000);
    readParam("minVarCos", "minVarCos", minVarCost, 0, 10000);
    readParam("maxVarCos", "maxVarCos", maxVarCost, 0, 10000);
    readParam("minArcCap", "minArcCap", minArcCapac, 0, 10000);
    readParam("maxArcCap", "maxArcCap", maxArcCapac, 0, 10000);
    readParam("minFixCos", "minFixCos", minFixedCost, 0, 10000);
    readParam("maxFixCos", "maxFixCos", maxFixedCost, 0, 10000);
    readParam("minComCap", "minComCap", minCommCapac, 0, 10000);
    readParam("maxComCap", "maxComCap", maxCommCapac, 0, 10000);
    readParam("ratZeroFix", "ratZeroFix", ratioZeroFixedCost, 0, 100);
    readParam("ratTotVolCap", "ratTotVolCap", ratioTotVolCapac, 0, 100);
    readParam("ratZeroComCap", "ratZeroComCap", ratioZeroCommCapac, 0, 100);
    readParam("ratMaxComCap", "ratMaxComCap", ratioMaxCommCapac, 0, 100);
    
    // adjCapacsDown is a boolean set to true if option adjCapacsDown appears
    // in instruction line or configuration file; adjFactCapacs is set to value
    // appearing in instruction line or configuration file next to option
    // adjCapacsDown.
    readParam("adjCapacsDown", "adjCapacsDown", adjCapacsDown, adjFactCapacs, 0.0001, 10000.);
    // adjFixCostsUp is a boolean set to true if option adjFixCostsUp appears
    // in instruction line or configuration file; adjFactFixCosts is set to value
    // appearing in instruction line or configuration file next to option
    // adjFixCostsUp
    readParam("adjFixCostsUp", "adjFixCostsUp", adjFixCostsUp, adjFactFixCosts, 0.0001, 10000.);
    
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	// 3
	// These parameters are active only when the primary topology is set to hubs-and-satellites:
	// (i.e., when parameter hubsAndSats is present or explicitly set to true)
	
	if (hubSat)
	{		
		readParam("minSatsPerHub", "minSatsPerHub", minSatsPerHub, 0, 1000);
		readParam("maxSatsPerHub", "maxSatsPerHub", maxSatsPerHub, 0, 1000);	
		readParam("minSatsRoundSize", "minSatsRoundSize", minSatsRoundSize, 1, 1000);
		readParam("maxSatsRoundSize", "maxSatsRoundSize", maxSatsRoundSize, 1, 1000);	
		readParam("adjSatCapacsDown", "adjSatCapacsDown", adjSatCapacsDown, adjSatFactCapacs, 0.0001, 10000.);
		readParam("adjSatFixCostsUp", "adjSatFixCostsUp", adjSatFixCostsUp, adjSatFactFixCosts, 0.0001, 10000.);	
		readParam("nbAddSatArcSampTailHead", "nbAddSatArcSampTailHead", numbAddSatRandArcs_SamplTailsHeads, 0, 1000);
		readParam("nbAddSatArcPerSampHead", "nbAddSatArcPerSampHead", numbAddSatRandArcsPerNode_SamplHeads, 0, 1000);
		readParam("minSatVarCos", "minSatVarCos", minSatVarCost, 0, 10000);
		readParam("maxSatVarCos", "maxSatVarCos", maxSatVarCost, 0, 10000);
		readParam("minSatArcCap", "minSatArcCap", minSatArcCapac, 0, 10000);
		readParam("maxSatArcCap", "maxSatArcCap", maxSatArcCapac, 0, 10000);
		readParam("minSatFixCos", "minSatFixCos", minSatFixedCost, 0, 10000);
		readParam("maxSatFixCos", "maxSatFixCos", maxSatFixedCost, 0, 10000);
		readParam("minSatComCap", "minSatComCap", minSatCommCapac, 0, 10000);
		readParam("maxSatComCap", "maxSatComCap", maxSatCommCapac, 0, 10000);
		readParam("ratSatZeroFix", "ratSatZeroFix", ratioSatZeroFixedCost, 0, 100);
		readParam("ratSatTotVolCap", "ratSatTotVolCap", ratioSatTotVolCapac, 0, 100);
		readParam("ratSatZeroComCap", "ratSatZeroComCap", ratioSatZeroCommCapac, 0, 100);
		readParam("ratSatMaxComCap", "ratSatMaxComCap", ratioSatMaxCommCapac, 0, 100);
		readParam("equalSatComCaps", "equalSatComCaps", equalizeSatComCaps);
		readParam("equalSatVarCosts", "equalSatVarCosts", equalizeSatVarCosts);		
	}

    switch (secTopology)
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
             << " = lengthX * lengthY since grid secondary topology has been selected." << endl;
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
             << " = lengthX * lengthY since grid secondary topology has been selected." << endl;
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
             << " = lengthX * lengthY since grid secondary topology has been selected." << endl;
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
		cerr << "\nERROR:" << endl;
        cerr << "secTopology must fall in {0, 1, 2, 3, 4, 5}." << endl;
        cerr << "Configuration file name: " << paramProcessing.getConfigFileName() << endl;	
		cerr << "Content of command line:" << endl;	
		for (const auto& [option, value] : paramProcessing.getParamToValueMapFromCmdLine())
		{
			cerr << option + " " + value << endl;	
		}
		cerr << "EXECUTION ABORTED\n" << endl;
		
		cerr << "Information about this error and its remedies is available\n"
			<< "either onscreen by launching the program with the \"+H\" option\n"
			<< "or examine the document USERGUIDE_DETERM.md in the online repository." << endl;		
		exit(1);
	}

    crossValidateParams();	
}

void Data::displayError(string error)
{
    cerr << "\nERROR: " + error << endl;    	
	cerr << "Configuration file name: " << paramProcessing.getConfigFileName() << endl;	
	cerr << "Content of command line:" << endl;	
	for (const auto& [option, value] : paramProcessing.getParamToValueMapFromCmdLine())
	{
		cerr << option + " " + value << endl;	
	}
	cerr << "EXECUTION ABORTED\n" << endl;
	
	cerr << "To learn about the sources of this error and its remedies,\n"
			<< "either launch the program with the \"+H\" option to display the user guide,\n"
			<< "or examine the document USERGUIDE_DETERM.md in the online repository." << endl;		
	exit(1);
}

// Verifies data that should lie inside an interval.
template <class T> void Data::verifyParam(string name, const T &var, const T minVal, const T maxVal)
{
    if (var < minVal || var > maxVal)
    {
		cerr << "\nERROR:" << endl;
        cerr << name << " must be within [" << minVal << " , " << maxVal << "]." << endl;
        cerr << "Configuration file name: " << paramProcessing.getConfigFileName() << endl;	
		cerr << "Content of command line:" << endl;	
		for (const auto& [option, value] : paramProcessing.getParamToValueMapFromCmdLine())
		{
			cerr << option << " " << value << endl;	
		}
		cerr << "EXECUTION ABORTED\n" << endl;
		
		cerr << "To learn about the sources of this error and its remedies,\n"
			<< "either launch the program with the \"+H\" option to display the user guide,\n"
			<< "or examine the document USERGUIDE_DETERM.md in the online repository." << endl;		
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

	if (!equalizeComCaps)
	{
		if (minCommCapac > maxCommCapac)
			displayError("minComCap > maxComCap, whereas equalizeComCaps does not hold");

		if (maxCommCapac > minCommVol)
			displayError("maxComCap > minComVol, whereas equalizeComCaps does not hold");

		if (maxCommCapac > minArcCapac)
			displayError("maxComCap > minArcCap, whereas equalizeComCaps does not hold");

		if (maxArcCapac > numbCommods * minCommCapac)
			displayError("maxArcCap > numbCom * minComCap, whereas equalizeComCaps does not hold");
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
	
	// primary topology is point-to-point
	if (!hubSat)
	{
		if (oneSourceOneSink)
		{
			if (equSourcesEquSinks)
				displayError("oneSrcOneSnk and eqSrcsEqSnks cannot be both active.");
			
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
	}
	// primary topology is hubs-and-satellites
	else
	{
		if (secTopology == 5)
			displayError("hubsAndSats && secTopology 5");
			
		if (minSatsPerHub > maxSatsPerHub)
			displayError("minSatsPerHub > maxSatsPerHub");		
			
		if (minSatsRoundSize > maxSatsRoundSize)
			displayError("minSatsRoundSize > maxSatsRoundSize");
			
		if (minSatFixedCost > maxSatFixedCost)
			displayError("minSatFixCos > maxSatFixCos");
			
		if (minSatVarCost > maxSatVarCost)
			displayError("minSatVarCos > maxSatVarCos");
		
		if (minSatArcCapac > maxSatArcCapac)
			displayError("minSatArcCap > maxSatArcCap");
		
		if (!equalizeSatComCaps)
		{
			if (minSatCommCapac > maxSatCommCapac)
				displayError("minSatComCap > maxSatComCap, whereas equalizeSatComCaps does not hold");	
			
			if (maxSatCommCapac > minCommVol)
				displayError("maxSatComCap > minComVol, whereas equalizeSatComCaps does not hold");

			if (maxSatCommCapac > minSatArcCapac)
				displayError("maxSatComCap > minSatArcCap, whereas equalizeSatComCaps does not hold"); //El:

			if (maxSatArcCapac > numbCommods * minSatCommCapac)
				displayError("maxSatArcCap > numbCom * minSatComCap, whereas equalizeSatComCaps does not hold");
		}
			
		if (oneSourceOneSink)
		{
			if (equSourcesEquSinks)
				displayError("oneSrcOneSnk and eqSrcsEqSnks cannot be both active.");

			if (gridConnect)
			{
				if (lengthX * lengthY * minSatsPerHub * (lengthX * lengthY * minSatsPerHub - 1) < numbCommods)
					displayError("lengthX * lengthY * minSatsPerHub * (lengthX * lengthY * minSatsPerHub - 1) < numbCommods");
			}
			else
			{
				if (numbNodes * minSatsPerHub * (numbNodes * minSatsPerHub - 1) < numbCommods)
					displayError("numbNodes * minSatsPerHub * (numbNodes * minSatsPerHub - 1) < numbCommods");
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
				if (maxNumbSources + maxNumbSinks > lengthX * lengthY * minSatsPerHub)
					displayError("maxNbSrc + maxNbSnk > lengthX * lengthY * minSatsPerHub");
			}
			else
			{
				if (maxNumbSources + maxNumbSinks > numbNodes * minSatsPerHub)
					displayError("maxNbSrc + maxNbSnk > numbNodes * minSatsPerHub");
			}
		}
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

void Data::readParam(string nameOnCommLine, string nameInFile, int &var)
{
    paramProcessing.assignParamValue(nameOnCommLine, nameInFile, var);
}

void Data::readParam(string nameOnCommLine, string nameInFile, double &var)
{
    paramProcessing.assignParamValue(nameOnCommLine, nameInFile, var);
}



void Data::printHelpAndExit()
{
	printf("\nHELP REQUESTED: DISPLAYING DOCUMENTATION\n");
	printf("\n"); 
	printf("GENERATOR OF INSTANCES OF THE LINEAR, DETERMINISTIC, MULTI-COMMODITY, FIXED\n"); 
	printf("CHARGE, CAPACITATED NETWORK DESIGN PROBLEM\n");
	printf("\n"); 
	printf("This application generates instances of the linear deterministic multi-\n"); 
	printf("commodity, capacitated, fixed charge network design problem (MCFNDP). The\n"); 
	printf("generation process is based on pseudo-random determinations governed by user-\n"); 
	printf("specified configuration settings.\n");
	printf("\n");
	printf("For detailed background, see\n"); 
	printf("Larsen, E., Bisaillon, S., Cordeau, J.F. and Frejinger, E.\n"); 
	printf("Pseudo-random Instance Generators in C++ for Deterministic and Stochastic\n"); 
	printf("Multi-commodity Network Design Problems.\n");
	printf("\n"); 
	printf("This user guide comprises the following sections:\n"); 
	printf("1 Pseudo-random generation and available topologies\n"); 
	printf("2 Building and running the program directly from the Linux command line\n"); 
	printf("3 Running the program through Docker instructions\n"); 
	printf("4 Usage of options\n"); 
	printf("5 Detailed examples\n");
	printf("\n"); 
	printf("1 Pseudo-random generation and available topologies\n"); 
	printf("***************************************************\n");
	printf("\n"); 
	printf("All pseudo-random sampling operations occurring in the program are performed\n"); 
	printf("with the permuted congruential generator (PCG). (See O'Neill, M. PCG, a\n"); 
	printf("family of better random number generators.\n"); 
	printf("https://www.pcg-random.org/, 2023.) All generated numerical characteristics\n"); 
	printf("are integer-valued: commodity volumes, fixed costs, overall capacities,\n"); 
	printf("commodity-specific variable costs, commodity-specific capacities.\n");
	printf("\n"); 
	printf("Two primary topologies are available: point-to-point and\n"); 
	printf("hubs-and-satellites. Notice that the hub-and-spoke topology is a special\n"); 
	printf("case of the latter.\n"); 
	printf("- Under the point-to-point primary topology, any node can be a source or a\n"); 
	printf("sink of commodity volume. The secondary topology option specifies basic\n"); 
	printf("connections between nodes. Additional arcs can be created randomly as\n"); 
	printf("specified.\n"); 
	printf("- Under the hubs-and-satellites primary topology (option hubsAndSats),\n"); 
	printf("there are two classes of nodes: hub nodes and satellite nodes. Only\n"); 
	printf("satellite nodes can be sources or sinks of commodity volumes. Two classes\n"); 
	printf("of arcs must also are distinguished: (i) hub arcs connecting hub nodes and\n"); 
	printf("(ii) satellite arcs connecting two satellite nodes or a satellite node and\n"); 
	printf("a hub node. In this context, the secondary topology option specifies basic\n"); 
	printf("connections between hub nodes. Sets of satellite nodes, called rounds, are\n"); 
	printf("assigned to each hub node. The satellite nodes in a round are connected\n"); 
	printf("sequentially with satellite arcs. The cardinality of each round is\n"); 
	printf("determined randomly according to specified option values. The end satellite\n"); 
	printf("nodes in the round(s) are connected to the hub node through satellite arcs.\n"); 
	printf("Additional hub arcs and satellite arcs can be created randomly if\n"); 
	printf("specified.\n");
	printf("\n"); 
	printf("The secondary topology (option secTopology) concerns all nodes in the\n"); 
	printf("point-to-point primary topology and concerns only hub nodes in the\n"); 
	printf("hubs-and-satellites primary topology. It can either (i) be grid-like (the\n"); 
	printf("grid can be laid either on a torus, a cylinder or a flat bounded surface),\n"); 
	printf("(ii) be based on a circle, (iii) be explicited in a file, or (iv) be\n"); 
	printf("entirely random generated. (ia) Under the toral secondary topology, non-\n"); 
	printf("random arcs are initially created to build the structure of a torus with\n"); 
	printf("major and minor circumferences respectively equal to values of options\n"); 
	printf("lengthX and lengthY. (ib) Under the cylindrical secondary topology, non-\n"); 
	printf("random arcs are initially created to build the structure of a cylinder with\n"); 
	printf("circumference and height respectively equal to values of options lengthX and\n"); 
	printf("lengthY. (ic) Under the flat secondary topology, non-random arcs are\n"); 
	printf("initially created to build the structure of a bounded flat surface of width\n"); 
	printf("and height respectively equal to values of options lengthX and lengthY. (ii)\n"); 
	printf("Under the circular secondary topology, non-random arcs are initially created\n"); 
	printf("in order to chain the nodes. (iii) A basic graph (including nodes and arcs\n"); 
	printf("but excluding costs and capacities) can be read from a file. Each non-random\n"); 
	printf("arc initially created in connection with (i), (ii) and (iii) has overall\n"); 
	printf("capacity equal to total volume over all commodities and fixed cost equal to\n"); 
	printf("the value specified as the maximum fixed cost (value of option maxFixedCos).\n"); 
	printf("Over these non-random arcs, overall capacities are equal to total volume\n"); 
	printf("over all commodities, each commodity-specific capacity is equal to commodity\n"); 
	printf("volume and commodity-specific variable costs are equal to the value\n"); 
	printf("specified as the maximum commodity-specific variable cost (value of option\n"); 
	printf("maxVarCos).\n");
	printf("\n"); 
	printf("Addition of random arcs to the aforementioned structures can be performed\n"); 
	printf("in one or both of the two following ways: (i) A specified total number of\n"); 
	printf("additional arcs whose head and tail are generated pseudo-randomly. (ii) A\n"); 
	printf("specified number of additional arcs starting from each existing node are\n"); 
	printf("created whereas only their heads are generated pseudo-randomly. Each one of\n"); 
	printf("the arcs in (i) and (ii) has fixed cost pseudo-randomly generated\n"); 
	printf("independently and uniformly within the interval specified by the values of\n"); 
	printf("options minFixCos and maxFixCos and overall capacity pseudo-randomly\n"); 
	printf("generated independently and uniformly within the interval specified by\n"); 
	printf("the values of options minArcCap and maxArcCap. The commodity-specific\n"); 
	printf("variable costs and capacities are pseudo-randomly generated independently\n"); 
	printf("and uniformly between the values of options minVarCos and maxVarCos and\n"); 
	printf("the values of options minComCap and maxComCap, respectively.\n");
	printf("\n"); 
	printf("Section 4 below details the usage of every option available under the\n"); 
	printf("point-to-point and hubs-and-satellites primary topologies.\n");
	printf("\n"); 
	printf("2 Building and running the program directly from the Linux command line\n"); 
	printf("***********************************************************************\n");
	printf("\n"); 
	printf("2.1 Building the program from the Linux command line\n"); 
	printf("****************************************************\n");
	printf("\n"); 
	printf("Follow these steps to build the program:\n");
	printf("\n"); 
	printf("a- Either (i) use Git to clone locally the remote repository\n"); 
	printf("https://github.com/larseeri/determ-MCFNDP-gener, or (ii) generate, download\n"); 
	printf("locally to an appropriate folder and uncompress a .zip archive containing\n"); 
	printf("the remote repository file tree.\n"); 
	printf("b- In a Linux terminal, go to the /program subfolder just created.\n"); 
	printf("c- Make sure that the gcc C++ compiler and the make C++ build tool are\n"); 
	printf("available.\n"); 
	printf("d- Run this instruction to build the program. An executable file named\n"); 
	printf("'determ_gen' will be created inside the /program folder.\n");
	printf("\n"); 
	printf("make\n");
	printf("\n"); 
	printf("Building with versions 11.5.0, 12.2.0, 13.2.0, 14.2.0 and 15.1.0 of the gcc\n"); 
	printf("compiler has been verified.\n");
	printf("\n"); 
	printf("2.2 Running the program with Linux command line instructions\n"); 
	printf("************************************************************\n");
	printf("\n"); 
	printf("The program can be run as follows from the /program folder just created\n"); 
	printf("along with default option parameter values:\n");
	printf("\n"); 
	printf("./determ_gen\n");
	printf("\n"); 
	printf("Options for running the program can be specified directly in the command\n"); 
	printf("line or in a configuration file. By default, the program will read option\n"); 
	printf("values from file determ_config.par. (This default setting can be updated by\n"); 
	printf("modifying line 1210 of file main.cpp.) The default file can be\n"); 
	printf("overridden with option +F:\n");
	printf("\n"); 
	printf("./determ_gen +F newParamfile.par\n");
	printf("\n"); 
	printf("This assumes that determ_config.par and newParamfile.par are located at the\n"); 
	printf("root of the /program folder. Otherwise, their absolute paths or their paths\n"); 
	printf("relative to the /program folder must be explicited.\n");
	printf("\n"); 
	printf("The default configuration file determ_config.par has initially been filled\n"); 
	printf("with illustrative values and and should be edited appropriately if no\n"); 
	printf("replacement configuration file is indicated on the command line with option\n"); 
	printf("key +F.\n");
	printf("\n"); 
	printf("Verbosity option +v should be used to obtain more information about all\n"); 
	printf("options that have been set in the parameter file or in the command line:\n");
	printf("\n"); 
	printf("./determ_gen +v +F newParamfile.par\n");
	printf("\n"); 
	printf("A configuration file should have this format:\n");
	printf("\n"); 
	printf("optionKey1	value1\n"); 
	printf("optionKey2   value2\n"); 
	printf(".\n"); 
	printf(".\n"); 
	printf("optionKeyN   value3\n");
	printf("\n"); 
	printf("It may specify at most one option per line. Option keys may be prefixed or\n"); 
	printf("not with '-'. Comments may appear either at the beginning of a line, in\n"); 
	printf("which case they must start with a non-alphabetic character (but not with\n"); 
	printf("'-'), or at the end of a line.\n");
	printf("\n"); 
	printf("Once the configuration file has been processed, options appearing in the\n"); 
	printf("command line (other than +F and +v) are processed. Option values specified\n"); 
	printf("in the command line supersede those specified in the configuration file.\n"); 
	printf("That is, if included, they replace, modify and supplement the latter.\n"); 
	printf("Option keys appearing in the command line must be prefixed with '-' as\n"); 
	printf("follow:\n");
	printf("\n"); 
	printf("-optionkeyA valueA -optionKeyB valueB...\n");
	printf("\n"); 
	printf("By default, boolean values are set to false and presence of option key is\n"); 
	printf("sufficient to set the corresponding value to true. For other options\n"); 
	printf("expecting other types of values, values should be specified.\n");
	printf("\n"); 
	printf("Example of a valid command line:\n");
	printf("\n"); 
	printf("./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10\n");
	printf("\n"); 
	printf("In this example, the final value for parameters stream, seed and\n"); 
	printf("numbCom will be retrieved from command line, and all other parameters\n"); 
	printf("appearing in file newParamfile.par will have their value taken from there.\n"); 
	printf("Parameters that appear neither in command line, nor in this parameter file\n"); 
	printf("will be set to the initialization values hard-coded in lines 28-48 of\n"); 
	printf("data.cpp.\n");
	printf("\n"); 
	printf("Setting a parameter value more than once in a location will not cause an\n"); 
	printf("error. In this case, the last value assigned will be retained. For example,\n"); 
	printf("from the command line\n");
	printf("\n"); 
	printf("./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10 -seed 1212\n");
	printf("\n"); 
	printf("the value retained for option parameter seed will be 1212 (not 4567).\n");
	printf("\n"); 
	printf("To display the complete documentation about the application and the\n"); 
	printf("available options, run\n");
	printf("\n"); 
	printf("./determ_gen -help\n");
	printf("\n"); 
	printf("Summing up, option values are successively updated from these locations in\n"); 
	printf("this order:\n"); 
	printf("(i) hard-coded initialization values in lines 28-48 of data.cpp\n"); 
	printf("(ii) values appearing in configuration file whose name is specified in\n"); 
	printf("command line with +F (if it is specified and if it exists); if no valid\n"); 
	printf("configuration file is specified with +F in command line, values will be\n"); 
	printf("taken from default configuration file whose name is specified in line 1210\n"); 
	printf("of main.cpp; at this point, the latter should exist\n"); 
	printf("(iii) values specified in the command line\n"); 
	printf("Notice that location (i) is insufficiently transparent to the user and\n"); 
	printf("should not be relied upon. Parameter values should be set through (ii) and\n"); 
	printf("(iii).\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("See Section 5 below for examples of configuration files and instructions\n"); 
	printf("when the program is run from the Linux command line.\n");
	printf("\n"); 
	printf("2.2.1 Reading and writing the basic graph from and to a file\n"); 
	printf("************************************************************\n");
	printf("\n"); 
	printf("Setting secondary topology option to 5 instructs the program to read the\n"); 
	printf("basic graph (nodes + arcs without costs and capacities) from a plain text\n"); 
	printf("file. The default name of this file is basic_graph.gra. An alternative\n"); 
	printf("long name (including extension) may be specified with the option\n"); 
	printf("longBasicGraphFileName. To be read correctly, this file must be structured\n"); 
	printf("as follows:\n");
	printf("\n"); 
	printf("(beginning of file)\n"); 
	printf("Two blank or comment rows (these are disregarded when reading)\n"); 
	printf("low = lowNodeNumber (lowNodeNumber is smallest node number appearing in rows below)\n"); 
	printf("high = highNodeNumber (highNodeNumber is highest node number appearing in rows below)\n"); 
	printf("Three blank or comment rows (these are disregarded when reading)\n"); 
	printf("origNodeNumberA destNodeNumberA (a row identifies an arc from origin to destination)\n"); 
	printf("origNodeNumberB destNodeNumberB\n"); 
	printf("origNodeNumberC destNodeNumberC\n"); 
	printf(".\n"); 
	printf(".\n"); 
	printf(".\n"); 
	printf("(end of file)\n");
	printf("\n"); 
	printf("When secondary topology is set between 1 and 4, the program will save the\n"); 
	printf("basic graph (i.e., including nodes and arcs, but excluding random arcs,\n"); 
	printf("costs and capacities) in a plain text file whose default name,\n"); 
	printf("basicGraph.gra, may be replaced using the parameter longBasicGraphFileName.\n"); 
	printf("The name specified using the latter should include the extension. The\n"); 
	printf("content of the file will appear like this:\n");
	printf("\n"); 
	printf("(beginning of file)\n"); 
	printf("1 Nodes\n"); 
	printf("Nodes are identified with integers in closed interval [low, high].  Program will perform appropriate renumbering.\n"); 
	printf("low = 0\n"); 
	printf("high = 11\n");
	printf("\n"); 
	printf("2 Arcs\n"); 
	printf("NodeFrom  NodeTo\n"); 
	printf("0  1\n"); 
	printf("0  2\n"); 
	printf("0  3\n"); 
	printf("0  9\n"); 
	printf(".\n"); 
	printf(".\n"); 
	printf(".\n"); 
	printf("(end of file)\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("See Section 5 below for examples of basic graph files.\n");
	printf("\n"); 
	printf("3 Running the program through Docker instructions\n"); 
	printf("*************************************************\n");
	printf("\n"); 
	printf("An advantage of running the executable determ_gen inside a Docker container\n"); 
	printf("is the possibility to avoid selection and installation of suitable C++\n"); 
	printf("compiler, C++ make build tool and required C++ libraries. It suffices that\n"); 
	printf("the Docker engine be installed on the host where the computation will be\n"); 
	printf("run. For this, see https://docs.docker.com/engine/install/.\n");
	printf("\n"); 
	printf("Once the Docker engine is installed, follow these steps:\n");
	printf("\n"); 
	printf("a- Open a terminal and retrieve from the GitHub container registry the\n"); 
	printf("Docker image ghcr.io/larseeri/determ_gen_image:latest as follows:\n");
	printf("\n"); 
	printf("docker pull ghcr.io/larseeri/determ_gen_image:latest\n");
	printf("\n"); 
	printf("(From this image, Docker containers running the executable 'determ_gen' will\n"); 
	printf("be generated.)\n");
	printf("\n"); 
	printf("b- For simplicity, define the alias 'determ_gen_image' through\n");
	printf("\n"); 
	printf("docker tag ghcr.io/larseeri/determ_gen_image:latest determ_gen_image\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("If the Docker image determ_gen_image were instead available as the tar ball\n"); 
	printf("determ_gen_image.tar, then it could be loaded through\n");
	printf("\n"); 
	printf("docker load --input determ_gen_image.tar\n");
	printf("\n"); 
	printf("c- Either (i) use Git to clone locally the remote repository\n"); 
	printf("https://github.com/larseeri/determ-MCFNDP-gener, or (ii) generate, download\n"); 
	printf("locally to an appropriate folder and uncompress a .zip archive containing\n"); 
	printf("the remote repository file tree. The /docker folder contains (i) the\n"); 
	printf("primitive files named 'Dockerfile' and 'makefile' based on which the\n"); 
	printf("Docker image was originally generated, (ii) a subfolder /inout used\n"); 
	printf("for exchanging files between the program running inside a Docker container\n"); 
	printf("and the directory structure of the host, (iii) some auxiliary files, and\n"); 
	printf("(iv) the subfolder /examples containing a number of configuration files and\n"); 
	printf("their resulting outputs (examined in Section 5 below).\n");
	printf("\n"); 
	printf("d- In a terminal, go to the /docker folder just created.\n");
	printf("\n"); 
	printf("Then, instructions similar to those appearing in Section 2.2 can be run from\n"); 
	printf("the terminal on AMD and Intel CPUs (other CPUs are addressed further below).\n"); 
	printf("For example, the default Docker instruction\n");
	printf("\n"); 
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen\n");
	printf("\n"); 
	printf("is equivalent to the default Linux command line instruction\n");
	printf("\n"); 
	printf("./determ_gen\n");
	printf("\n"); 
	printf("In general, Docker instructions can be formulated from those appearing in\n"); 
	printf("Section 2.2 by (i) adding the prefix\n"); 
	printf("\n"); 
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image\n");
	printf("\n"); 
	printf("and by (ii) replacing the executable location ./determ_gen with /determ_gen\n"); 
	printf("(i.e. removing the initial dot), since the executable determ_gen is located\n"); 
	printf("at the root of the Docker container being generated and since locations\n"); 
	printf("inside a Docker container must be accessed through absolute paths.\n"); 
	printf("\n"); 
	printf("Hence, the following Linux command line instructions:\n");
	printf("\n"); 
	printf("./determ_gen +v +F newParamfile.par\n");
	printf("\n"); 
	printf("./determ_gen -help\n");
	printf("\n"); 
	printf("./determ_gen +F newParamfile.par -stream 1234 -seed 4567 -numbCom 10\n");
	printf("\n"); 
	printf("respectively correspond to the following Docker instructions:\n");
	printf("\n"); 
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen \\n"); 
	printf("+v +F /inout/newParamfile.par\n");
	printf("\n"); 
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout \\n"); 
	printf("determ_gen_image /determ_gen -help\n");
	printf("\n"); 
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen \\n"); 
	printf("+F /inout/newParamfile.par -stream 1234 -seed 4567 -numbCom 10\n");
	printf("\n"); 
	printf("Important:\n"); 
	printf("When running the program through Docker instructions, all input and output\n"); 
	printf("files must be located inside the docker/inout subfolder. Within the Docker\n"); 
	printf("instructions and the configuration files, this subfolder must be denoted\n"); 
	printf("/inout since it is located at the root of the Docker container. Hence, a\n"); 
	printf("replacement configuration file located at docker/inout/newParamFile.par\n"); 
	printf("will be specified in the Docker instruction with +F /inout/newParamFile.par.\n"); 
	printf("Similarly, a short output file name ./shortOutputFileName will be specified\n"); 
	printf("in the Docker instruction or inside the configuration file with\n"); 
	printf("-shortOutFileName /inout/shortOutputFileName.\n");
	printf("\n"); 
	printf("The default configuration file is located at docker/inout/determ_config.par.\n"); 
	printf("It has initially been filled with illustrative values and should be edited\n"); 
	printf("appropriately if no replacement configuration file is indicated on the\n"); 
	printf("Docker instruction line with option key +F.\n"); 
	printf("\n"); 
	printf("Option parameter values are successively updated from these locations in\n"); 
	printf("this order:\n"); 
	printf("(i) initialization values hard-coded in lines 28-48 of data.cpp,\n"); 
	printf("(ii) either replacement configuration file specified in the Docker\n"); 
	printf("instruction line with option key +F, or default configuration file \n"); 
	printf("./inout/determ_config.par if no replacement configuration file is\n"); 
	printf("specified in the Docker instruction line with option key +F,\n"); 
	printf("(iii) values specified in the Docker instruction.\n"); 
	printf("Notice that location (i) is insufficiently transparent to the user and\n"); 
	printf("should not be relied upon. Parameter values should be set through (ii) and\n"); 
	printf("(iii).\n");
	printf("\n"); 
	printf("Important:\n"); 
	printf("The supplied Docker image conforms to the amd64 CPU instructions standard\n"); 
	printf("and will run natively on AMD and Intel CPUs. On CPUs conforming instead to\n"); 
	printf("the arm64 standard, e.g., on recent Apple computers, the supplied Docker\n"); 
	printf("image can be run through emulation. Essentially, this may be achieved as\n"); 
	printf("follows:\n");
	printf("\n"); 
	printf("a- Running the following preliminary instruction once. (For details, see\n"); 
	printf("https://hub.docker.com/r/tonistiigi/binfmt.)\n");
	printf("\n"); 
	printf("docker run --privileged --rm tonistiigi/binfmt --install all\n");
	printf("\n"); 
	printf("b- Adding the option --platform linux/amd64 to the Docker instruction line\n"); 
	printf("prefix as follows:\n");
	printf("\n"); 
	printf("docker run --platform linux/amd64 --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image\n");
	printf("\n"); 
	printf("The following characteristics are specific to the operation of the program\n"); 
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
	printf("\n"); 
	printf("The following Docker instructions may be useful:\n");
	printf("\n"); 
	printf("docker image ls (list all currently existing docker images)\n");
	printf("\n"); 
	printf("docker image rm --force imageName (forcefully remove imageName)\n");
	printf("\n"); 
	printf("docker container ls (list all running containers)\n");
	printf("\n"); 
	printf("docker container ls --all (list all containers, running or not)\n");
	printf("\n"); 
	printf("See Section 5 below for examples of configuration files and instructions\n"); 
	printf("when the program is run through Docker.\n");
	printf("\n"); 
	printf("4 Detailed usage of options\n"); 
	printf("***************************\n");
	printf("\n"); 
	printf("Notation:\n"); 
	printf("For the purposes of the current discussion, <...> indicates that some value\n"); 
	printf("or values of the type specified inside the brackets is expected. For\n"); 
	printf("instance, <string> means that a string is expected whereas <> means that no\n"); 
	printf("value is expected.\n");
	printf("\n"); 
	printf("4.1 Options available in instruction line only (Linux or Docker)\n"); 
	printf("****************************************************************\n");
	printf("\n"); 
	printf("+F <string>; name of configuration file supplying option parameter values;\n"); 
	printf("this replaces the default file whose name is hard-coded in line 1210 of\n"); 
	printf("main.cpp; any option value supplied in instruction line replaces value of\n"); 
	printf("same option supplied in configuration file\n");
	printf("\n"); 
	printf("+v <> (no value supplied); requests verbosity while parsing options in\n"); 
	printf("instruction line or configuration file\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("An unknown option key following '+' in the instruction line is disregarded.\n");
	printf("\n"); 
	printf("4.2 Options available in instruction line (Linux or Docker) and in configuration file\n"); 
	printf("*************************************************************************************\n");
	printf("\n"); 
	printf("Remarks:\n"); 
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
	printf("\n"); 
	printf("4.2.1 Options that are available under the point-to-point or hubs-and-satellites primary topologies\n"); 
	printf("***************************************************************************************************\n"); 
	printf("(That is, when the option hubsAndSats is either (i) absent and implicity set\n"); 
	printf("to false, or (ii) present without value and implicity set to true or (iii)\n"); 
	printf("present and explicitly set to true or false.)\n");
	printf("\n"); 
	printf("-help <> (no value supplied); display help message\n");
	printf("\n"); 
	printf("-shortOutFileName <string>; short name (without extension) of output file;\n"); 
	printf("default value is './outfile' (Linux instructions) or './inout/outfile'\n"); 
	printf("(Docker instructions); extension will differ according to format:\n"); 
	printf("dow, std (a.k.a. Canad format), lp, mps, tex; when using Linux instructions,\n"); 
	printf("absolute path or path relative to the /program folder may be added\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("DOW, STD and TEX formats describe the generated network; DOW and STD\n"); 
	printf("(a.k.a. Canad format) are text formats specified in files\n"); 
	printf("DOW-format-desc.txt and STD-format-desc.txt; STD is a generic format valid\n"); 
	printf("for any MCFNDP, DOW requires that (i) for each arc, capacities and variable\n"); 
	printf("costs are identical for all commodities and that (ii) each commodity has a\n"); 
	printf("single source node (at which volume > 0) and a single sink node (at which\n"); 
	printf("volume < 0); TEX format is a graphical representation that can be processed\n"); 
	printf("by a LaTeX text and graphics compiler; LP (human readable) and MPS formats\n"); 
	printf("state linear programming formulations of the generated MCFNDP instance.\n");
	printf("\n"); 
	printf("Remarks:\n"); 
	printf("- The output will automatically be saved in STD format unless option\n"); 
	printf("noStdOut is specified.\n"); 
	printf("- Under the point-to-point primary topology, the output will automatically\n"); 
	printf("be saved in DOW format if the options oneSrcOneSnk, equalComCaps,\n"); 
	printf("equalVarCosts all hold.\n"); 
	printf("- Under the hubs-and-satellites primary topology, the output will\n"); 
	printf("automatically be saved in DOW format if the options oneSrcOneSnk,\n"); 
	printf("equalComCaps, equalVarCosts, equalSatComCaps, equalSatVarCosts all hold.\n");
	printf("\n"); 
	printf("-mpsOut <> (no value supplied); save output with MPS format\n");
	printf("\n"); 
	printf("-lpOut <> (no value supplied); save output with human readable LP format\n");
	printf("\n"); 
	printf("-texOut <> (no value supplied); generate and save LaTeX code representing\n"); 
	printf("network;\n");
	printf("\n"); 
	printf("Remarks:\n"); 
	printf("- Under the point-to-point primary topology, the LaTeX representations are\n"); 
	printf(" unavailable (i) if lengthX > 10 or lengthY > 10,\n"); 
	printf(" when 1 <= secTopology <= 3, (ii) if numbNodes > 100, when secTopology == 0\n"); 
	printf("or secTopology >= 4, (iii) if numbCom > 15; other specific conditions\n"); 
	printf("depending on values of oneSrcOneSnk, lengthX, lengthY, numbNodes and numbCom\n"); 
	printf("are managed and reported by the program;\n"); 
	printf("- Ulterior processing of the TEX file by a LaTeX compiler may require up to\n"); 
	printf("several minutes depending on complexity.\n"); 
	printf("- The LaTeX representations produced in connection with\n"); 
	printf("(i) hubs-and-satellites primary topologies (configuration option hubSat),\n"); 
	printf("(ii) random secondary topologies (configuration option secTopology 0),\n"); 
	printf("(iii) topologies that are read from a file (configuration option\n"); 
	printf("secTopology 5) are treated as generic, unstructured graphs. For legibility,\n"); 
	printf("these representations are only available when total number of nodes is not\n"); 
	printf("greater than 12 and number of commodities is not greater than 4.\n");
	printf("\n"); 
	printf("-noStdOut <> (no value supplied); do not save output with STD format\n");
	printf("\n"); 
	printf("-seed <int: 0, INT_MAX>; seed of the pseudo-random generator\n");
	printf("\n"); 
	printf("-stream <int: 0, INT_MAX>; stream of the pseudo-random generator\n");
	printf("\n"); 
	printf("-numbCom <int: 0, INT_MAX>; number of commodities\n");
	printf("\n"); 
	printf("-oneSrcOneSnk <> (no value supplied); select randomly one source and one\n"); 
	printf("sink for each commodity; (source, sink) pairs must be different across all\n"); 
	printf("commodities\n");
	printf("\n"); 
	printf("-eqSrcsEqSnks <> (no value supplied); impose identical sources over all\n"); 
	printf("commodities and identical sinks over all commodities; common source nodes\n"); 
	printf("and common sink nodes are selected randomly according to minNbSrc, maxNbSrc,\n"); 
	printf("minNbSnk, maxNbSnk\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("oneSrcOneSnk and eqSrcsEqSnks cannot be both active; if oneSrcOneSnk and\n"); 
	printf("eqSrcsEqSnks are both inactive, then sources and sinks are selected\n"); 
	printf("randomly\n");
	printf("\n"); 
	printf("-minNbSrc <int: 1, INT_MAX>; min number of sources; inactive under\n"); 
	printf("oneSrcOneSnk\n");
	printf("\n"); 
	printf("-maxNbSrc <int: 1, INT_MAX>; max number of sources; inactive under\n"); 
	printf("oneSrcOneSnk\n");
	printf("\n"); 
	printf("-minNbSnk <int: 1, INT_MAX>; min number of sinks; inactive under\n"); 
	printf("oneSrcOneSnk\n");
	printf("\n"); 
	printf("-maxNbSnk <int: 1, INT_MAX>; max number of sinks; inactive under\n"); 
	printf("oneSrcOneSnk\n");
	printf("\n"); 
	printf("-inclDesignObjCap <> (no value supplied); when saving in LP or MPS format,\n"); 
	printf("(i) design variables will appear with fixed costs in objective,\n"); 
	printf("(ii) design variables will appear in overall arc-capacity constraints\n");
	printf("\n"); 
	printf("-inclDesignObjCommCap <> (no value supplied); when saving in LP or MPS\n"); 
	printf("format, (i) design variables will appear with fixed costs in objective,\n"); 
	printf("(ii) design variables will appear in commodity-specific capacity\n"); 
	printf("constraints.\n");
	printf("\n"); 
	printf("Important:\n"); 
	printf("Whereas the DOW, STD and TEX formats store sufficient basic information\n"); 
	printf("to uniquely identify a MCFNDP instance, the LP and MPS formats state a\n"); 
	printf("formulation of this problem (objective and constraints). LP and MPS are\n"); 
	printf("respectively human- and machine-readable. Options inclDesignObjCap and\n"); 
	printf("inclDesignObjCommCap allow to select particular formulations of the\n"); 
	printf("linear programming statement of the MCFNDP.\n");
	printf("\n"); 
	printf("By default, (i.e, when options inclDesignObjCap and inclDesignObjCommCap\n"); 
	printf("are omitted or explicitly set to false) design variables are altogether\n"); 
	printf("absent: (i) the objective omits fixed costs as if all existing arcs are\n"); 
	printf("costlessly opened), (ii) overall capacity constraints for all arcs are\n"); 
	printf("included, (iii) commodity-specific capacity constraints for all arcs\n"); 
	printf("and commodities are included.\n");
	printf("\n"); 
	printf("When only inclDesignObjCap is active, design variables are included: (i)\n"); 
	printf("objective includes fixed costs of opening arcs, (ii) overall capacity\n"); 
	printf("constraints where design variables multiply overall capacity are included\n"); 
	printf("for each arc, (iii) commodity-specific capacity constraints, where design\n"); 
	printf("variables do not appear, are included.\n");
	printf("\n"); 
	printf("When only inclDesignObjCommCap is active, design variables are included:\n"); 
	printf("(i) the objective includes fixed costs of opening arcs, (ii) overall\n"); 
	printf("capacity constraints where design variables do not appear are included\n"); 
	printf("for each arc, (iii) commodity-specific capacity constraints, where design\n"); 
	printf("variables multiply overall capacity are included for each arc and commodity.\n");
	printf("\n"); 
	printf("Producing the standard formulation of the MCFNDP requires that both\n"); 
	printf("inclDesignObjCap and inclDesignObjCommCap be active.\n");
	printf("\n"); 
	printf("4.2.2 Options available under the point-to-point primary topology\n"); 
	printf("*****************************************************************\n"); 
	printf("(That is, when option hubsAndSats is either (i) absent and implicity set to\n"); 
	printf("false, or (ii) present and explicitly set to false.)\n");
	printf("\n"); 
	printf("-secTopology <int: 0, 1, 2, 3, 4, 5>;\n"); 
	printf("0 calls for a base secondary topology that is entirely random and where arcs\n"); 
	printf("are set between nodes according to nbAddArcSampTailHead and/or\n"); 
	printf("nbAddArcPerSampHead; 1 to 5 call for a particular base secondary graph\n"); 
	printf("topology (before additional, random arcs are eventually added according to\n"); 
	printf("nbAddArcSampTailHead and/or nbAddArcPerSampHead): 1 grid on a torus, 2 grid\n"); 
	printf("on a cylinder, 3 grid on a bounded surface, 4 circular, 5 read (nodes + arcs\n"); 
	printf("without costs and capacities) from file\n");
	printf("\n"); 
	printf("Important:\n"); 
	printf("Whenever secTopology is set to 5 so that a basic graph is read from a file\n"); 
	printf("(whose name is specified with option longBasicGraphFileName), value of\n"); 
	printf("option numbNodes must be set to a number larger than the exact number\n"); 
	printf("of nodes found in this file.\n");
	printf("\n"); 
	printf("-longBasicGraphFileName <string>; long name (including extension) of file\n"); 
	printf("where basic graph (excluding random arcs, capacities and costs ) will be\n"); 
	printf("read if secTopology == 5 and will be written if 1 <= secTopology <= 4;\n"); 
	printf("default value is './basic_graph.gra' (Linux instructions) or\n"); 
	printf("'./inout/basic_graph.gra' (Docker instructions); when using Linux\n"); 
	printf("instructions, absolute path or path relative to the /program folder may be\n"); 
	printf("added\n");
	printf("\n"); 
	printf("-lengthX <int: 1, INT_MAX>; length along X axis if secTopology is in\n"); 
	printf("{1, 2, 3} (inactive otherwise)\n");
	printf("\n"); 
	printf("-lengthY <int: 1, INT_MAX>; length along Y axis if secTopology is in\n"); 
	printf("{1, 2, 3} (inactive otherwise)\n");
	printf("\n"); 
	printf("-numbNodes <int: 1, INT_MAX>; number of nodes under random or circular\n"); 
	printf("secondary topology; (overriden by lengthX * lengthY under grid secondary\n"); 
	printf("topology)\n");
	printf("\n"); 
	printf("-nbAddArcSampTailHead <int: 0, INT_MAX>; total number of additional random\n"); 
	printf("arcs generated by sampling over tail (origin) and head (destination) nodes;\n"); 
	printf("if this option is inactive, default value is 0\n");
	printf("\n"); 
	printf("-nbAddArcPerSampHead <int: 0, INT_MAX>; number of additional random arcs\n"); 
	printf("per tail (origin) node generated by sampling over head (destination) nodes;\n"); 
	printf("if this option is inactive, default value is 0\n");
	printf("\n"); 
	printf("-minComVol <int: 1, INT_MAX>; min commodity volume\n");
	printf("\n"); 
	printf("-maxComVol <int: 1, INT_MAX>; max commodity volume\n");
	printf("\n"); 
	printf("-minVarCos <int: 0, INT_MAX>; min commodity-specific variable cost\n");
	printf("\n"); 
	printf("-maxVarCos <int: 0, INT_MAX>; max commodity-specific variable cost\n");
	printf("\n"); 
	printf("-minArcCap <int: 1, INT_MAX>; min arc overall capacity\n");
	printf("\n"); 
	printf("-maxArcCap <int: 1, INT_MAX>; max arc overall capacity\n");
	printf("\n"); 
	printf("-minFixCos <int: 0, INT_MAX>; min fixed cost\n");
	printf("\n"); 
	printf("-maxFixCos <int: 0, INT_MAX>; max fixed cost\n");
	printf("\n"); 
	printf("-minComCap <int: 0, INT_MAX>; min commodity-specific arc capacity\n");
	printf("\n"); 
	printf("-maxComCap <int: 0, INT_MAX>; max commodity-specific arc capacity\n");
	printf("\n"); 
	printf("-ratZeroFix <int: 0, 100>; percent of random arcs with fixed cost\n");
	printf("set to zero; if this option is inactive, default percentage is 0\n");
	printf("\n"); 
	printf("-ratTotVolCap <int: 0, 100>; percent of random arcs with total\n"); 
	printf("capacity set to total volume; if this option is inactive, default\n"); 
	printf("percentage is 0\n");
	printf("\n"); 
	printf("-ratZeroComCap <int: 0, 100>; percent of random arcs with one of the\n"); 
	printf("commodity-specific capacities set to zero; if this option is inactive,\n"); 
	printf("default percentage is 0\n");
	printf("\n"); 
	printf("-ratMaxComCap <int: 0, 100>; percent of random arcs with one of the\n"); 
	printf("commodity-specific capacities set to maxComCap; if this option is inactive,\n"); 
	printf("default percentage is 0\n");
	printf("\n"); 
	printf("-noParalArc <int: 1, INT_MAX>; when adding random arcs, avoid parallel arcs\n"); 
	printf("and set to this figure the maximum number of failed attempts to add a non-\n"); 
	printf("parallel arc. If reached, decrease the number of random arcs to be added\n"); 
	printf("by one; if this option is inactive, no attempt will be made to avoid\n"); 
	printf("parallel arcs\n");
	printf("\n"); 
	printf("-adjCapacsDown <dbl: 0.0001, 10000>; normalize overall capacities of all\n"); 
	printf("arcs (non-random and random) by uniform adjustment (downward if value of\n"); 
	printf("adjCapacsDown >= 1, upward otherwise), whose magnitude is proportional to\n"); 
	printf("the specified figure (see Graph::adjustArcCapacities() in graph.cpp); if\n"); 
	printf("this option is omitted, no adjustment will be made\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("Setting adjCapacsDown to 1 is not equivalent to no adjustment. This\n"); 
	printf("setting balances the sum of total commodity volumes flowing on all arcs\n"); 
	printf("with the sum of all arc capacities. It might be viewed as a neutral\n"); 
	printf("setting.\n");
	printf("\n"); 
	printf("-adjFixCostsUp <dbl: 0.0001, 10000>; normalize fixed costs of all arcs\n"); 
	printf("(non-random and random) by uniform adjustment (downward if value of\n"); 
	printf("adjFixCostsUp <= 1, upward otherwise) whose magnitude is proportional to\n"); 
	printf("the specified figure (see Graph::adjustFixedCosts() in graph.cpp); if this\n"); 
	printf("option is inactive, no adjustment will be made\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("Setting adjFixCostsUp to 1 is not equivalent to no adjustment. This\n"); 
	printf("setting balances the sum of fixed costs over all arcs with the total\n"); 
	printf("variable cost that would be incurred by circulating average commodity\n"); 
	printf("volume on all arcs. It might be viewed as a neutral setting.\n");
	printf("\n"); 
	printf("-equalComCaps <> (no value supplied); equalize commodity-specific\n"); 
	printf("capacities over arcs as follows: for all k and a, capacity for\n"); 
	printf("commodity k over arc a is equated to minimum of volume of commodity k\n"); 
	printf("and overall capacity of arc a\n");
	printf("\n"); 
	printf("-equalVarCosts <> (no value supplied); equalize commodity-specific\n"); 
	printf("variable costs over arcs as follows: for all k and a, variable cost for\n"); 
	printf("commodity k over arc a is equated to variable cost for commodity 0 over\n"); 
	printf("arc a\n");
	printf("\n"); 
	printf("4.2.3 Options available under the hubs-and-satellites primary topology\n"); 
	printf("**********************************************************************\n"); 
	printf("(That is, when option hubsAndSats is (i) present and implicity set to true, or\n"); 
	printf("(ii) present and explicitly set to true.)\n");
	printf("\n"); 
	printf("-minComVol <int: 1, INT_MAX>; min commodity volume\n");
	printf("\n"); 
	printf("-maxComVol <int: 1, INT_MAX>; max commodity volume\n");
	printf("\n"); 
	printf("-longBasicGraphFileName <string>; long name (including extension) of file\n"); 
	printf("where basic graph (excluding random arcs, capacities and costs ) will be\n"); 
	printf("written if 1 <= secTopology <= 4; default value is './basic_graph.gra'\n"); 
	printf("(Linux instructions) or './inout/basic_graph.gra' (Docker instructions);\n"); 
	printf("when using Linux instructions, absolute path or path relative to the\n"); 
	printf("/program folder may be added\n");
	printf("\n"); 
	printf("Important:\n"); 
	printf("Under the hubs-and-satellite primary topology, secTopology cannot be\n"); 
	printf("set to 5. That is, the topology cannot be read from a basic graph file.\n"); 
	printf("However, a basic graph file will be written if 1 <= secTopology <= 4.\n"); 
	printf("In this case, the graph will be saved unstructured, without information\n"); 
	printf("about its original (i.e., hubs-and-satellites) primary topology.\n"); 
	printf("Subsequently, this basic graph file could be read in a new run in which\n"); 
	printf("the primary topology would be set to point-to-point. Obviously, the\n"); 
	printf("original hubs-and-satellites characterization would be lost.\n");
	printf("\n"); 
	printf("-noParalArc <int: 1, INT_MAX>; when adding random arcs, avoid parallel arcs\n"); 
	printf("and set to this figure the maximum number of failed attempts to add a non-\n"); 
	printf("parallel arc. If reached, decrease the number of random arcs to be added\n"); 
	printf("by one; if this option is inactive, no attempt will be made to avoid\n"); 
	printf("parallel arcs\n");
	printf("\n"); 
	printf("4.2.3.1 Options describing hub nodes and hub arcs\n"); 
	printf("*************************************************\n");
	printf("\n"); 
	printf("Definition:\n"); 
	printf("A hub arc connects two hub nodes.\n");
	printf("\n"); 
	printf("-secTopology <int: 0, 1, 2, 3, 4>; 0 calls for a secondary topology that is\n"); 
	printf("entirely random and where arcs are set between hub nodes according to\n"); 
	printf("nbAddArcSampTailHead and/or nbAddArcPerSampHead; 1 to 4 call for a\n"); 
	printf("particular basic secondary topology (before additional, random arcs are\n"); 
	printf("eventually added according to nbAddArcSampTailHead and/or\n"); 
	printf("nbAddArcPerSampHead): 1 grid on a torus, 2 grid on a cylinder, 3 grid on a\n"); 
	printf("bounded surface, 4 circular.\n");
	printf("\n"); 
	printf("Important:\n"); 
	printf("Under the hubs-and-satellite primary topology, option secTopology cannot be\n"); 
	printf("set to 5. That is, the topology cannot be read from a basic graph file.\n");
	printf("\n"); 
	printf("-lengthX <int: 1, INT_MAX>; length along X axis if hub topology option\n"); 
	printf("secTopology is in {1, 2, 3} (inactive otherwise)\n");
	printf("\n"); 
	printf("-lengthY <int: 1, INT_MAX>; length along Y axis if hub topology option\n"); 
	printf("secTopology is in {1, 2, 3} (inactive otherwise)\n");
	printf("\n"); 
	printf("-numbNodes <int: 1, INT_MAX>; number of hub nodes under random or circular\n"); 
	printf("hub topology; (overriden by lengthX * lengthY under grid secondary topology)\n");
	printf("\n"); 
	printf("-nbAddArcSampTailHead <int: 0, INT_MAX>; total number of additional random\n"); 
	printf("arcs between hub nodes generated by sampling over tail (origin) and head\n"); 
	printf("(destination) hub nodes; if this option is inactive, default value is 0\n");
	printf("\n"); 
	printf("-nbAddArcPerSampHead <int: 0, INT_MAX>; number of additional random arcs\n"); 
	printf("per tail (origin) hub node generated by sampling over head (destination)\n"); 
	printf("hub nodes; if this option is inactive, default value is 0\n");
	printf("\n"); 
	printf("-minVarCos <int: 0, INT_MAX>; min commodity-specific variable cost over hub\n"); 
	printf("arc\n");
	printf("\n"); 
	printf("-maxVarCos <int: 0, INT_MAX>; max commodity-specific variable cost over hub\n"); 
	printf("arc\n");
	printf("\n"); 
	printf("-minArcCap <int: 1, INT_MAX>; min arc overall capacity over hub arc\n");
	printf("\n"); 
	printf("-maxArcCap <int: 1, INT_MAX>; max arc overall capacity over hub arc\n");
	printf("\n"); 
	printf("-minFixCos <int: 0, INT_MAX>; min fixed cost over hub arc\n");
	printf("\n"); 
	printf("-maxFixCos <int: 0, INT_MAX>; max fixed cost over hub arc\n");
	printf("\n"); 
	printf("-minComCap <int: 0, INT_MAX>; min commodity-specific capacity over hub arc\n");
	printf("\n"); 
	printf("-maxComCap <int: 0, INT_MAX>; max commodity-specific capacity over hub arc\n");
	printf("\n"); 
	printf("-ratZeroFix <int: 0, 100>; percent of random hub arcs with fixed cost set to\n"); 
	printf("zero; if this option is inactive, default percentage is 0\n");
	printf("\n"); 
	printf("-ratTotVolCap <int: 0, 100>; percent of random hub arcs with total capacity\n"); 
	printf("set to total volume; if this option is inactive, default percentage is 0\n");
	printf("\n"); 
	printf("-ratZeroComCap <int: 0, 100>; percent of random hub arcs with one of the\n"); 
	printf("commodity-specific capacities set to zero; if this option is inactive,\n"); 
	printf("default percentage is 0\n");
	printf("\n"); 
	printf("-ratMaxComCap <int: 0, 100>; percent of random hub arcs with one of the\n"); 
	printf("commodity-specific capacities set to maxComCap; if this option is inactive,\n"); 
	printf("default percentage is 0\n");
	printf("\n"); 
	printf("-adjCapacsDown <dbl: 0.0001, 10000>; normalize overall capacities of all hub\n"); 
	printf("arcs (non-random and random) by uniform adjustment (downward if value of\n"); 
	printf("adjCapacsDown >= 1, upward otherwise), whose magnitude is proportional to\n"); 
	printf("the specified figure (see Graph::adjustArcCapacities() in graph.cpp); if\n"); 
	printf("this option is omitted, no adjustment will be made\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("Setting adjCapacsDown to 1 is not equivalent to no adjustment. This\n"); 
	printf("setting balances the sum of total commodity volumes flowing on all hub arcs\n"); 
	printf("with the sum of all hub arc capacities. It might be viewed as a neutral\n"); 
	printf("setting.\n");
	printf("\n"); 
	printf("-adjFixCostsUp <dbl: 0.0001, 10000>; normalize fixed costs of all hub arcs\n"); 
	printf("(non-random and random) by uniform adjustment (downward if value of\n"); 
	printf("adjFixCostsUp <= 1, upward otherwise) whose magnitude is proportional to the\n"); 
	printf("specified figure (see Graph::adjustFixedCosts() in graph.cpp); if this\n"); 
	printf("option is inactive, no adjustment will be made\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("Setting adjFixCostsUp to 1 is not equivalent to no adjustment. This\n"); 
	printf("setting balances the sum of fixed costs over all hub arcs with the total\n"); 
	printf("variable cost that would be incurred by circulating average commodity\n"); 
	printf("volume on all hub arcs. It might be viewed as a neutral setting.\n");
	printf("\n"); 
	printf("-equalComCaps <> (no value supplied); equalize commodity-specific capacities\n"); 
	printf("over hub arcs as follows: for all k and a, capacity for commodity k over arc\n"); 
	printf("a is equated to minimum of volume of commodity k and overall capacity of hub\n"); 
	printf("arc a\n");
	printf("\n"); 
	printf("-equalVarCosts <> (no value supplied); equalize commodity-specific\n"); 
	printf("variable costs over hub arcs as follows: for all k and a, variable cost for\n"); 
	printf("commodity k over arc a is equated to variable cost for commodity 0 over hub\n"); 
	printf("arc a\n");
	printf("\n"); 
	printf("4.2.3.2 Options describing satellite nodes and satellite arcs\n"); 
	printf("*************************************************************\n");
	printf("\n"); 
	printf("Definitions:\n"); 
	printf("(i) A satellite arc connects either two satellite nodes or one hub node and\n"); 
	printf("one satellite node.\n"); 
	printf("(ii) A round is a set of satellite nodes that are assigned to a hub node and\n"); 
	printf("that are sequentially connected. End nodes in the round are connected to the\n"); 
	printf("hub node.\n");
	printf("\n"); 
	printf("-minSatsPerHub <int: 0, INT_MAX); minimum number of satellites per hub\n");
	printf("\n"); 
	printf("-maxSatsPerHub <int: 0, INT_MAX); maximum number of satellites per hub\n");
	printf("\n"); 
	printf("-minSatsRoundSize <int: 1, INT_MAX); minimum number of satellites in a round\n");
	printf("\n"); 
	printf("-maxSatsRoundSize <int: 1, INT_MAX);	maximum number of satellites in a round\n");
	printf("\n"); 
	printf("-nbAddSatArcSampTailHead <int: 0, INT_MAX>; total number of additional random\n"); 
	printf("arcs between satellite nodes generated by sampling over tail (origin) and\n"); 
	printf("head (destination) nodes; if this option is inactive, default value is 0\n");
	printf("\n"); 
	printf("-nbAddSatArcPerSampHead <int: 0, INT_MAX>; number of additional random arcs\n"); 
	printf("between satellite nodes per tail (origin) node generated by sampling over\n"); 
	printf("head (destination) nodes; if this option is inactive, default value is 0\n");
	printf("\n"); 
	printf("-minSatVarCos <int: 0, INT_MAX>; min commodity-specific variable cost for\n"); 
	printf("arcs involving at least one satellite node\n");
	printf("\n"); 
	printf("-maxSatVarCos <int: 0, INT_MAX>; max commodity-specific variable cost for\n"); 
	printf("arcs involving at least one satellite node\n");
	printf("\n"); 
	printf("-minSatArcCap <int: 1, INT_MAX>; min overall capacity for arcs involving\n"); 
	printf("at least one satellite node\n");
	printf("\n"); 
	printf("-maxSatArcCap <int: 1, INT_MAX>; max overall capacity for arcs involving at\n"); 
	printf("least one satellite node\n");
	printf("\n"); 
	printf("-minSatFixCos <int: 0, INT_MAX>; min fixed cost for arcs involving at least\n"); 
	printf("one satellite node\n");
	printf("\n"); 
	printf("-maxSatFixCos <int: 0, INT_MAX>; max fixed cost for arcs involving at least\n"); 
	printf("one satellite node\n");
	printf("\n"); 
	printf("-minSatComCap <int: 0, INT_MAX>; min commodity-specific capacity for arcs\n"); 
	printf("involving at least one satellite node\n");
	printf("\n"); 
	printf("-maxSatComCap <int: 0, INT_MAX>; max commodity-specific capacity for arcs\n"); 
	printf("involving at least one satellite node\n");
	printf("\n"); 
	printf("-ratSatZeroFix <int: 0, 100>; percentage of random satellite arcs with fixed\n"); 
	printf("cost forced to zero; if this option is inactive, default percentage is 0\n");
	printf("\n"); 
	printf("-ratSatTotVolCap <int: 0, 100>; percentage of random satellite arcs with\n"); 
	printf("total capacity forced to total volume; if this option is inactive, default\n"); 
	printf("percentage is 0\n");
	printf("\n"); 
	printf("-ratSatZeroComCap <int: 0, 100>; percentage of random satellite arcs with\n"); 
	printf("one of the commodity-specific capacities forced to zero; if this option is\n"); 
	printf("inactive, default percentage is 0\n");
	printf("\n"); 
	printf("-ratSatMaxComCap <int: 0, 100>; percentage of random satellite arcs with one\n"); 
	printf("of the commodity-specific capacities forced to maxComCap; if this option is\n"); 
	printf("inactive, default percentage is 0\n");
	printf("\n"); 
	printf("-adjSatCapacsDown <dbl: 0.0001, 10000>; normalize overall capacities of all\n"); 
	printf("satellite arcs (non-random and random) by uniform adjustment (downward if\n"); 
	printf("value of adjSatCapacsDown >= 1, upward otherwise), whose magnitude is\n"); 
	printf("proportional to the specified figure (see Graph::adjustArcCapacities_sat()\n"); 
	printf("in graph.cpp); if this option is omitted, no adjustment will be made\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("Setting adjSatCapacsDown to 1 is not equivalent to no adjustment. This\n"); 
	printf("setting balances the sum of total commodity volumes flowing on all arcs with\n"); 
	printf("the sum of all arc capacities. It might be viewed as a neutral setting.\n"); 
	printf("\n"); 
	printf("-adjSatFixCostsUp <dbl: 0.0001, 10000>; normalize fixed costs of all\n"); 
	printf("satellite arcs (non-random and random) by uniform adjustment (downward if\n"); 
	printf("value of adjSatFixCostsUp <= 1, upward otherwise) whose magnitude is\n"); 
	printf("proportional to the specified figure (see Graph::adjustFixedCosts_sat() in\n"); 
	printf("graph.cpp); if this  option is inactive, no adjustment will be made\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("Setting adjSatFixCostsUp to 1 is not equivalent to no adjustment. This\n"); 
	printf("setting balances the sum of fixed costs over all arcs with the total\n"); 
	printf("variable cost that would be incurred by circulating average commodity\n"); 
	printf("volume on all arcs. It might be viewed as a neutral setting.\n");
	printf("\n"); 
	printf("-equalSatComCaps <> (no value supplied); equalize commodity-specific\n"); 
	printf("capacities over satellite arcs as follows: for all k and a, capacity for\n"); 
	printf("commodity k over satellite arc a is equated to minimum of volume of\n"); 
	printf("commodity k and overall capacity of satellite arc a\n");
	printf("\n"); 
	printf("-equalSatVarCosts <> (no value supplied); equalize commodity-specific\n"); 
	printf("variable costs over satellite arcs as follows: for all k and a, variable\n"); 
	printf("cost for commodity k over satellite arc a is equated to variable cost for\n"); 
	printf("commodity 0 over satellite arc a\n");
	printf("\n"); 
	printf("4.3 Joint restrictions\n"); 
	printf("**********************\n");
	printf("\n"); 
	printf("minComVol <= maxComVol\n"); 
	printf("minVarCos <= maxVarCos\n"); 
	printf("minFixCos <= maxFixCos\n"); 
	printf("minArcCap <= maxArcCap\n"); 
	printf("\n"); 
	printf("if (!equalizeComCaps)\n"); 
	printf("{\n"); 
	printf("		minComCap <= maxComCap\n"); 
	printf("		maxComCap <= minComVol\n"); 
	printf("		maxComCap <= minArcCap\n"); 
	printf("		maxArcCap <= numbCom * minComCap\n"); 
	printf("}\n"); 
	printf("\n"); 
	printf("if (gridConnect)\n"); 
	printf("{\n"); 
	printf("    	lengthX * lengthY >= 2\n"); 
	printf("}\n"); 
	printf("else\n"); 
	printf("{\n"); 
	printf("    	numbNodes >= 2\n"); 
	printf("}\n"); 
	printf("\n"); 
	printf("primary topology is point-to-point\n"); 
	printf("if (!hubsAndSats)\n"); 
	printf("{\n");
	printf("		if (oneSrcOneSnk)\n"); 
	printf("		{\n"); 
	printf("			if (1 <= secTopology <= 3)\n");
	printf("			{\n");
	printf("				(lengthX * lengthY) * ((lengthX * lengthY) - 1) >= numbCom\n");					
	printf("			}\n");
	printf("			else\n");
	printf("			{\n");
	printf("				numbNodes * (numbNodes - 1) >= numbCom\n");					
	printf("			}\n");
	printf("		}\n");
	printf("		else\n");
	printf("		{\n");
	printf("			minNbSrc <= maxNbSrc\n");
	printf("			minNbSnk <= maxNbSnk\n");				
	printf("\n");
	printf("			if (1 <= secTopology <= 3)\n");
	printf("			{\n");
	printf("				maxNbSrc + maxNbSnk <= lengthX * lengthY\n");					
	printf("			}\n");
	printf("			else\n");
	printf("			{\n");
	printf("				maxNbSrc + maxNbSnk <= numbNodes\n");					
	printf("			}\n");
	printf("		}\n"); 
	printf("}\n"); 
	printf("\n"); 
	printf("primary topology is hubs-and-satellites\n"); 
	printf("else\n"); 
	printf("{\n");
	printf("		secTopology != 5\n");
	printf("		minSatsPerHub <= maxSatsPerHub\n");			
	printf("		minSatsRoundSize <= maxSatsRoundSize\n");
	printf("		minSatFixCos <= maxSatFixCos\n");			
	printf("		minSatVarCos <= maxSatVarCos\n");		
	printf("		minSatArcCap <= maxSatArcCap\n");			
	printf("\n");		
	printf("		if (!equalComCaps)\n");
	printf("		{\n");
	printf("			minSatComCap <= maxSatComCap\n");			
	printf("			maxSatComCap <= minComVol\n");
	printf("			maxSatComCap <= minSatArcCap\n");
	printf("			maxSatArcCap <= numbCom * minSatComCap\n");				
	printf("		}\n");
	printf("\n");			
	printf("		if (oneSrcOneSnk)\n");
	printf("		{\n");
	printf("			if (equSrcsEquSnks)\n");
	printf("\n");				
	printf("			if (1 <= secTopology <= 3)\n");
	printf("			{\n");
	printf("				lengthX * lengthY * minSatsPerHub * (lengthX * lengthY * minSatsPerHub - 1) >= numbCom\n");					
	printf("			}\n");
	printf("			else\n");
	printf("			{\n");
	printf("				numbNodes * minSatsPerHub * (numbNodes * minSatsPerHub - 1) >= numbCom\n");					
	printf("			}\n");				
	printf("		}\n");
	printf("		else\n");
	printf("		{\n");
	printf("			minNbSrc <= maxNbSrc\n");
	printf("			minNbSnk <= maxNbSnk\n");				
	printf("\n");
	printf("			if (1 <= secTopology <= 3)\n");
	printf("			{\n");
	printf("				maxNbSrc + maxNbSnk <= lengthX * lengthY * minSatsPerHub\n");					
	printf("			}\n");
	printf("			else\n");
	printf("			{\n");
	printf("				maxNbSrc + maxNbSnk <= numbNodes * minSatsPerHub\n");					
	printf("			}\n");
	printf("		}\n");
	printf("}\n"); 
	printf("\n"); 
	printf("5 Detailed examples\n"); 
	printf("*******************\n");
	printf("\n"); 
	printf("The program/examples subfolder holds examples of configuration files and\n"); 
	printf("instruction lines occurring when the executable of the program is run\n"); 
	printf("directly from the Linux instruction line. It also contains the output files\n"); 
	printf("resulting from each pair of configuration file and instruction line.\n");
	printf("\n"); 
	printf("Similarly, the docker/examples subfolder holds examples of configuration\n"); 
	printf("files and instruction lines occurring when the executable of the program is\n"); 
	printf("run through Docker instructions. These examples are identified by the\n"); 
	printf("addition of the prefix 'docker_'. The subfolder also contains the output\n"); 
	printf("files resulting from each pair of configuration file and instruction line.\n"); 
	printf("The former are identical to the files generated when the executable is run\n"); 
	printf("from the Linux instruction line.\n");
	printf("\n"); 
	printf("Remark:\n"); 
	printf("The output examples contained in the /examples subfolders will also be used\n"); 
	printf("to illustrate the operation of the separate application generating \n"); 
	printf("stochastic MCFNDP instances.\n");
	printf("\n"); 
	printf("In all included examples, the generated MCFNDP instance is characterized by\n"); 
	printf("a point-to-point primary topology, a (3 x 4) grid secondary topology applied\n"); 
	printf("to a bounded surface and by 3 commodities.\n");
	printf("\n"); 
	printf("5.1 One source and one sink for each commodity\n"); 
	printf("**********************************************\n");
	printf("\n"); 
	printf("Folder /examples/oneSrcOneSnk/\n");
	printf("\n"); 
	printf("In this case, there is one source and one sink for each commodity and no\n"); 
	printf("differentiation of variable costs and capacities according to commodities.\n"); 
	printf("The generated networks can be saved in the DOW and STD formats.\n");
	printf("\n"); 
	printf("5.1.1 Instructions\n"); 
	printf("******************\n");
	printf("\n"); 
	printf("Linux command line instruction:\n");
	printf("\n"); 
	printf("./determ_gen +v +F determ_config_oneSrcOneSnk_12oct2026-17h09.par\n");
	printf("\n"); 
	printf("Docker instruction:\n");
	printf("\n"); 
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen +v \\n"); 
	printf("+F /inout/docker_determ_config_oneSrcOneSnk_12oct2026-17h09.par\n");
	printf("\n"); 
	printf("5.1.2 Configuration files (input)\n"); 
	printf("*********************************\n");
	printf("\n"); 
	printf("configuration file used with Linux command line:\n"); 
	printf("determ_config_oneSrcOneSnk_12oct2026-17h09.par\n");
	printf("\n"); 
	printf("configuration file used with Docker instruction:\n"); 
	printf("docker_determ_config_oneSrcOneSnk_12oct2026-17h09.par\n");
	printf("\n"); 
	printf("5.1.3 Generated files\n"); 
	printf("*********************\n");
	printf("\n");
	printf("numerical representation of the network in DOW format:\n");
	printf("determ_oneSrcOneSnk_12oct2026-17h09.dow\n");
	printf("\n");	
	printf("numerical representation of the network in STD format:\n"); 
	printf("determ_oneSrcOneSnk_12oct2026-17h09.std\n");
	printf("\n"); 
	printf("numerical representation of the basic graph:\n"); 
	printf("determ_oneSrcOneSnk_12oct2026-17h09.gra\n");
	printf("\n"); 
	printf("human-readable representation of the associated linear programming problem\n"); 
	printf("in LP format:\n"); 
	printf("determ_oneSrcOneSnk_12oct2026-17h09.lp\n");
	printf("\n"); 
	printf("numerical representation of the associated linear programming problem in\n"); 
	printf("MPS format:\n"); 
	printf("determ_oneSrcOneSnk_12oct2026-17h09.mps\n");
	printf("\n"); 
	printf("graphical representation of the network in TEX format:\n"); 
	printf("determ_oneSrcOneSnk_12oct2026-17h09.tex\n");
	printf("\n"); 
	printf("compiled graphical representation:\n"); 
	printf("determ_oneSrcOneSnk_12oct2026-17h09.pdf\n");
	printf("\n"); 
	printf("5.2 Multiple but equal sources and multiple but equal sinks\n"); 
	printf("***********************************************************\n");
	printf("\n"); 
	printf("Folder /examples/eqSrcsEqSnks/\n");
	printf("\n"); 
	printf("In this case, there are multiple sources and multiple sinks for each\n"); 
	printf("commodity. However, sources and sinks are identical across commodities.\n"); 
	printf("There is differentiation of variable costs and capacities according to\n"); 
	printf("commodities. The generated networks cannot be saved in the DOW format.\n");
	printf("\n"); 
	printf("5.2.1 Instructions\n"); 
	printf("******************\n");
	printf("\n"); 
	printf("Linux command line instruction:\n");
	printf("\n"); 
	printf("./determ_gen +v +F determ_config_eqSrcsEqSnks_12oct2026-17h09.par\n");
	printf("\n"); 
	printf("Docker instruction:\n");
	printf("\n"); 
	printf("docker run --rm --mount type=bind,src=absolutePathToDockerFolder/inout,dst=/inout determ_gen_image /determ_gen +v \\n"); 
	printf("+F /inout/docker_determ_config_eqSrcsEqSnks_12oct2026-17h09.par\n");
	printf("\n"); 
	printf("5.2.2 Configuration files (input)\n"); 
	printf("*********************************\n");
	printf("\n"); 
	printf("configuration file used with Linux command line:\n"); 
	printf("determ_config_eqSrcsEqSnks_12oct2026-17h09.par\n");
	printf("\n"); 
	printf("configuration file used with Docker instruction:\n"); 
	printf("docker_determ_config_eqSrcsEqSnks_12oct2026-17h09.par\n");
	printf("\n"); 
	printf("5.2.3 Generated files\n"); 
	printf("*********************\n");
	printf("\n"); 
	printf("numerical representation of the network in STD format:\n"); 
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.std\n");
	printf("\n"); 
	printf("numerical representation of the basic graph:\n"); 
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.gra\n");
	printf("\n"); 
	printf("human-readable representation of the associated linear programming problem\n"); 
	printf("in LP format:\n"); 
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.lp\n");
	printf("\n"); 
	printf("numerical representation of the associated linear programming problem in\n"); 
	printf("MPS format:\n"); 
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.mps\n");
	printf("\n"); 
	printf("graphical representation of the network in TEX format:\n"); 
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.tex\n");
	printf("\n"); 
	printf("compiled graphical representation:\n"); 
	printf("determ_eqSrcsEqSnks_12oct2026-17h09.pdf\n");
	printf("\n");
    
    exit(0);
}




