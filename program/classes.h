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

#ifndef __CLASS_H__
#define __CLASS_H__

#include <climits>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>
#include <vector>

#include "param_processing.h"
#include "pcg_random.hpp"

using namespace std;
using namespace PARAM;

typedef unsigned int uint;
typedef vector<int> VectInt;
typedef vector<double> VectDbl;
typedef vector<bool> VectBool;
typedef vector<VectBool> VectBool2;
typedef vector<string> VectStr;

const int max_width = 78;

// Random number generation
// Calling Permuted Congruential Generator:
// https://www.pcg-random.org/download.html

typedef uint32_t SEED_TYPE;


void checkIfValidOutputFile(ofstream&, string);

struct Random
{
    pcg32 rng;

    Random() : rng(pcg_extras::seed_seq_from<std::random_device>{})
    {
    }

    Random(SEED_TYPE _seed, SEED_TYPE _stream)
    {
        SetSeed(_seed, _stream);
    }

    void SetSeed(SEED_TYPE _seed, SEED_TYPE _stream)
    {
        rng.seed(_seed, _stream);		
		// to initialize the generator properly
        rng();
    }

    uint32_t getRandom()
    {
        return rng();
    }

	// selects integer from 0 to width-1
    uint32_t getRandom(int width)
    {
        return (width > 0) ? rng(width) : 0;
    }

	// selects integer from start to start + width -1
    uint32_t getRandom(int start, int width)
    {
        return getRandom(width) + start;
    }

    double getRandom01()
    {
        return (double)rng() / rng.max();
    }
};

struct MinMax
{
    int min;
    int max;
};

// Defines the basic program variables and handles their initialization with
// configuration data
struct Data
{
    ParamProcessing paramProcessing;

	// 1 Configuration variables that are used when the primary topology is 
	// either point-to-point (hubsat==false) or hubs-and-satellites (hubsat==true)
	
    // their values are set in instruction line or
    // configuration file (see Data::Data() for default and
    // admissible values).
	
	// when hubSat==false, the variables of Section 2 are inactive and the
	// the variables in Section 1 pertain to all nodes and arcs; when hubSat==true,
	// the variables in Section 1 pertain exclusively to hub nodes and arcs between
	// hub nodes whereas the variables below pertain to satellite nodes and
	// to satellite arcs (arcs involving at least one satellite node)	

	// secondary topology
	int secTopology;	

    // flags creation of base graph with grid like arc
    // connections between nodes; the grid is laid on a torus, a
    // cylinder or a flat surface (see Remark 1 below)
    bool gridConnect;

    // flags creation of base graph with grid like arc
    // connections between nodes; the grid is laid on a torus
	// (see Remark 1 below)
    bool toralConnect;

    // flags creation of base graph with grid like arc
    // connections between nodes; the grid is laid on a cylinder
	// (see Remark 1 below)
    bool cylConnect;

    // flags creation of base graph with grid like arc
    // connections between nodes; the grid is laid on a flat
    // surface (see Remark 1 below)
    bool flatConnect;

    // flags creation of base graph with circular arc
    // connections between nodes (see Remark 1 below)
    bool circConnect;

    // flags creation of purely random arc connections between
    // nodes (see Remark 1 below)
    bool randConnect;

    // flags reading basic graph (nodes + arcs without costs and
    // capacities) from file (see Remark 1 below)
    bool fromFile;

    // flags avoidance of parallel arcs when adding random arcs
    // (will attempt maxIterNoParallelArcs times to create a non
    // parallel random arc and decrement numbAddArcs if
    // unsuccessful)
    bool noParallelArcs;

    // flags ensuring there is only one source and only one sink
    // for each commodity (complies with DOW output format)
    bool oneSourceOneSink;

    //  flags ensuring that sources are identical over all
    //  commodities and sinks are identical over all commodities
    bool equSourcesEquSinks;

    // flags NO generation of STD format output file
    bool noStdOutFile;

    // flags generation of LP format output file
    bool generLpOutFile;

    // flags generation of MPS format output file
    bool generMpsOutFile;

    // flags generation of TEX format output file
    bool generTexOutFile;

    // when saving in LP or MPS format, ensure that (i) design
    // variables will appear with fixed costs in objective, (ii)
    // design variables will appear on the RHS of arc-capacity
    // constraints
    bool integCapacs;
	
    // when saving in LP or MPS format, ensure that (i) design
    // variables will appear with fixed costs in objective, (ii)
    // when there is a single commodity, design variables will
    // appear on the RHS of arc-capacity constraints, (iii) when
    // there are multiple commodities, commodity-specific
    // capacity constraints (a.k.a. strong forcing constraints)
    // will be included and that design variables will appear on
    // their RHS
    bool integCommCapacs;

    // Note: In constrast with DOW, STD and TEX formats, LP and
    // MPS formats describe the mixed integer linear program
    // (MILP) solving the generated instance of MCFND problem;
    // hence, integCapacs and integCommCapacs
    // determine the particular forms taken by the LP and MPS
    // representations (see Graph::outputLP() and
    // Graph::outputMPS() in graph.cpp)

    // flags equalization of commodity-specific capacities over arcs:
	// for all k and a, capacity for commodity k over arc a is equated to
	// minimum of volume of commodity k and overall capacity of arc a	
    bool equalizeComCaps;
	
	// flags equalization of commodity-specific variable costs over arcs:
	// for all k and a, variable cost for commodity k over arc a is equated to
	// variable cost for commodity 0 over arc a
	bool equalizeVarCosts;

    // stream from which random numbers will be extracted
    int stream;

    // random seed
    int seed;

    // length of grid along x axis (effective only when
    // gridConnect == true) (see Remark 1 below)
    int lengthX;

    // length of grid along y axis (effective only when
    // gridConnect == true) (see Remark 1 below)
    int lengthY;

    // number of nodes (effective only when gridConnect == false
    // AND fromFile == false) (see Remark 1 below)
    int numbNodes;

    // number of commodities
    int numbCommods;

    // total number of random arcs generated by sampling over
    // tail and head nodes (see Remark 1 below)
    int numbAddRandArcs_SamplTailsHeads;

    // number of random arcs per tail node generated by sampling
    // over head nodes (see Remark 1 below)
    int numbAddRandArcsPerNode_SamplHeads;

    // minimum number of sources
    int minNumbSources;

    // maximum number of sources
    int maxNumbSources;

    // minimum number of sinks
    int minNumbSinks;

    // maximum number of sinks
    int maxNumbSinks;

    // minimum commodity volume when initially created,
    // before ensuing adjustments
    int minCommVol;

    // maximum commodity volume when initially created,
    // before ensuing adjustments
    int maxCommVol;

    // lower bound for fixed cost over random arcs when
    // initially generated (see Remark 1 below)
    int minFixedCost;

    // upper bound for fixed cost over random arcs when
    // initially generated; fixed cost over non-random arc when
    // initially generated (see Remark 1 below)
    int maxFixedCost;

    // lower bound for variable cost for commodity over arc when
    // initially created, before ensuing adjustments
	// (see Remark 1 below)
    int minVarCost;

    // maximum variable cost for commodity over arc when
    // initially created, before ensuing adjustments
	// (see Remark 1 below)
    int maxVarCost;

    // lower bound for overall capacity of random arc when
    // initially created, before ensuing adjustments
	// (see Remark 1 below)
    int minArcCapac;

    // upper bound for overall capacity of random arc when
    // initially created, before ensuing adjustments
	// (see Remark 1 below)
    int maxArcCapac;

    // minimum capacity of arc for commodity when initially
    // created, before ensuing adjustments (see Remark 1 below)
    int minCommCapac;

    // maximum capacity of arc for commodity when initially
    // created, before ensuing adjustments (see Remark 1 below)
    int maxCommCapac;

    // flags uniform downward adjustment (equal to
    // adjFactCapacs) of capacities over arcs (see Remark 1 below)
    bool adjCapacsDown;

    // magnitude of uniform downward adjustment of overall
    // capacities of all arcs (non-random and random); effective
    // only when adjCapacsDown == true (see Remark 1 below)
    double adjFactCapacs;

    // flags uniform upward adjustment (equal to
    // adjFactFixCosts) of fixed costs over arcs (see Remark 1 below)
    bool adjFixCostsUp;

    // magnitude of uniform upward adjustment of fixed costs all
    // arcs (non-random and random); effective only when
    // adjFixCostsUp == true (see Remark 1 below)
    double adjFactFixCosts;

    // percent of random arcs with fixed cost set equal to zero
	// (see Remark 1 below)
    int ratioZeroFixedCost;

    // percent of random arcs with capacity set equal to total
    // volume (see Remark 1 below)
    int ratioTotVolCapac;

    // percent of random arcs with one of the commodity-specific
    // capacities set equal to zero (see Remark 1 below)
    int ratioZeroCommCapac;

    // percent of random arcs with one of the commodity-specific
    // capacities set equal to maxArcCapac (see Remark 1 below)
    int ratioMaxCommCapac;

    // number of attempts to create a non-parallel random arc
    // before decrementing numbAddArcs (effective only when
    // noParallelArcs == true) (see Remark 1 below)
    int maxIterNoParallelArcs;

    // short output file name (excluding extension)
    string shortOutFileName;

    // long basic graph file name (including extension)
    string longBasicGraphFileName;

    Data(int argc, char **argv, string fileName = "test.par");

    void displayError(string error);

    // Methods handling input parameters

    void crossValidateParams();
	
	// Verifies that output path is valid.
	void verifyOutputPath(string path);

    template <class T> void verifyParam(string name, const T &var, const T minVal, const T maxVal);

    template <class T>
    void readParam(string nameOnCommLine, string nameInFile, T &var, const T minVal, const T maxVal);

    template <class T>
    void readParam(string nameOnCommLine, string nameInFile, bool &flag, T &var, const T minVal, const T maxVal);

    void readParam(string nameOnCommLine, string nameInFile, string &var);

    void readParam(string nameOnCommLine, string nameInFile, char *var);

    void readParam(string nameOnCommLine, string nameInFile, bool &var);
	
	void readParam(string nameOnCommLine, string nameInFile, double &var);
	
	void readParam(string nameOnCommLine, string nameInFile, int &var);

    void printHelpAndExit();

    void printUsageAndExit(char ExecName[], bool anonymous);
	
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	
	// 2 Configuration variables that are used only when the primary topology is
	// hubs-and-satellites (hubsat==true)
	
	// their values are set in instruction line or
    // configuration file (see Data::Data() for default and
    // admissible values).	
	
	// flags application of hubs-and-satellites rather than point-to-point
	// primary topology 
    bool hubSat;	
	
	// when hubSat==false, the variables of Section 2 are inactive and the
	// the variables in Section 1 pertain to all nodes and arcs; when hubSat==true,
	// the variables in Section 1 pertain exclusively to hub nodes and arcs between
	// hub nodes whereas the variables below pertain to satellite nodes and
	// to satellite arcs (arcs involving at least one satellite node)

	// minimum number of sats for each hub
	int minSatsPerHub;
	
	// maximum number of sats for each hub
	int maxSatsPerHub;
	
	// minimum size of round
	int minSatsRoundSize;
	
	// maximum size of round
	int maxSatsRoundSize;

	// total number of random arcs generated by sampling over
    // tail and head nodes
    int numbAddSatRandArcs_SamplTailsHeads;

    // number of random arcs per tail node generated by sampling
    // over head nodes
    int numbAddSatRandArcsPerNode_SamplHeads;

    // lower bound for fixed cost over random satellite arcs when
    // initially generated
    int minSatFixedCost;

    // upper bound for fixed cost over random satellite arcs when
    // initially generated; fixed cost over non-random satellite arc when
    // initially generated
    int maxSatFixedCost;

    // lower bound for variable cost for commodity over satellite arc when
    // initially created, before ensuing adjustments
    int minSatVarCost;

    // maximum variable cost for commodity over satellite arc when
    // initially created, before ensuing adjustments
    int maxSatVarCost;

    // lower bound for overall capacity of random satellite arc when
    // initially created, before ensuing adjustments
    int minSatArcCapac;

    // upper bound for overall capacity of random satellite arc when
    // initially created, before ensuing adjustments
    int maxSatArcCapac;

    // minimum capacity of satellite arc for commodity when initially
    // created, before ensuing adjustments
    int minSatCommCapac;

    // maximum capacity of satellite arc for commodity when initially
    // created, before ensuing adjustments
    int maxSatCommCapac;

    // flags uniform downward adjustment (equal to
    // adjSatFactCapacs) of capacities over satellite arcs
    bool adjSatCapacsDown;

    // magnitude of uniform downward adjustment of overall
    // capacities of all satellite arcs (non-random and random); effective
    // only when adjSatCapacsDown == true
    double adjSatFactCapacs;

    // flags uniform upward adjustment (equal to
    // adjSatFactFixCosts) of fixed costs over satellite arcs
    bool adjSatFixCostsUp;

    // magnitude of uniform upward adjustment of fixed costs all
    // satellite arcs (non-random and random); effective only when
    // adjSatFixCostsUp == true
    double adjSatFactFixCosts;	
	
	// percent of random satellite arcs with fixed cost set equal to zero	
    int ratioSatZeroFixedCost;

    // percent of random satellite arcs with capacity set equal to total
    // volume
    int ratioSatTotVolCapac;

    // percent of random satellite arcs with one of the commodity-specific
    // capacities set equal to zero
    int ratioSatZeroCommCapac;

    // percent of random satellite arcs with one of the commodity-specific
    // capacities set equal to maxArcCapac
    int ratioSatMaxCommCapac;

	// flags equalization of commodity-specific capacities of satellite arcs:
	// for all k and a, capacity for commodity k over satellite arc a is equated to
	// minimum of volume of commodity k and overall capacity of arc a	
    bool equalizeSatComCaps;
	
	// flags equalization of commodity-specific variable costs of satellite arcs:
	// for all k and a, variable cost for commodity k over satellite arc a is equated to
	// variable cost for commodity 0 over arc a
	bool equalizeSatVarCosts;	
};

// Defining commodity-specific characteristics of arc
struct CommodOnArc
{
    int cost;

    int capac;

    friend ostream &operator<<(ostream &os, const CommodOnArc &c);

    CommodOnArc(int _cost = 0, int _capac = 0) : cost(_cost), capac(_capac)
    {
    }
};

inline ostream &operator<<(ostream &os, const CommodOnArc &c)
{
    os << setw(8) << c.cost << setw(8) << c.capac;

    return os;
}

typedef vector<CommodOnArc> VectCommodOnArc;

// Defining arc
struct Arc
{
    int orig;

    int dest;

    int cost;

    int capac;

    VectCommodOnArc commodsOnArc;

    bool operator<(const Arc &a) const
    {
        return orig < a.orig;
    }

    friend ostream &operator<<(ostream &os, const Arc &a);

    Arc(int _orig, int _dest, int _cost, int _capac) : orig(_orig), dest(_dest), cost(_cost), capac(_capac)
    {
    }

    Arc(int _orig, int _dest, int _cost, int _capac, const vector<CommodOnArc> &_comm)
        : orig(_orig), dest(_dest), cost(_cost), capac(_capac), commodsOnArc(_comm)
    {
    }

    void addCommodOnArc(const CommodOnArc &_comm)
    {
        commodsOnArc.push_back(_comm);
    }
};

inline ostream &operator<<(ostream &os, const Arc &a)
{
    os << setw(8) << a.orig << setw(8) << a.dest << setw(8) << a.cost << setw(8) << a.capac << setw(8)
       << a.commodsOnArc.size() << endl;

    for (unsigned int k = 0; k < a.commodsOnArc.size(); k++)
        os << setw(8) << k + 1 << a.commodsOnArc[k] << endl;

    return os;
}

typedef vector<Arc> VectArc;

// Defining commodity node
struct CommodNode
{
    static int count;

    int index;

    int volume;

    CommodNode() : index(count++), volume(0)
    {
    }

    bool operator<(const CommodNode &c) const
    {
        return index < c.index;
    }
};

typedef vector<CommodNode> VectCommodNode;

// Defining commodity
struct Commodity
{
    int index;

    int volume;

    int numbOrig;

    int numbDest;

    int supply;

    int demand;

    int surplus;

    int avgSupply;

    int avgDemand;

    VectCommodNode commodNodes;

    void init(VectInt &sources, VectInt &sinks);

    void identifyOrig(VectInt &sources);

    void identifyDest(VectInt &sinks);

    void assignSurplus();

    Commodity(int numbNodes)
        : index(-1), volume(-1), numbOrig(-1), numbDest(-1), supply(-1), demand(-1), surplus(-1), avgSupply(-1),
          avgDemand(-1)
    {
        commodNodes.resize(numbNodes);
    }
	
	Commodity()
        : index(-1), volume(-1), numbOrig(-1), numbDest(-1), supply(-1), demand(-1), surplus(-1), avgSupply(-1),
          avgDemand(-1)
	{
	}

    friend ostream &operator<<(ostream &os, const Commodity &c);
};

inline ostream &operator<<(ostream &os, const Commodity &c)
{
    for (unsigned int n = 0; n < c.commodNodes.size(); n++)
        os << setw(8) << c.index << setw(8) << c.commodNodes[n].index << setw(8) << c.commodNodes[n].volume << endl;

    return os;
}

typedef vector<Commodity> VectCommodity;

// Supporting display of output in LP format
struct LPStringBuilder
{
    string id;

    VectStr vars;

    string rhs;

    LPStringBuilder() : id(""), rhs("")
    {
    }

    LPStringBuilder(string _prefix) : id(_prefix), rhs("")
    {
    }

    LPStringBuilder(string _prefix, string sense, int val) : id(_prefix)
    {
        addRhs(sense, val);
    }

    LPStringBuilder(string _prefix, const VectStr &_vars, string _rhs) : id(_prefix), vars(_vars), rhs(_rhs)
    {
    }

    void addPrefix(string _prefix)
    {
        id = _prefix;
    }

    void addVar(string name)
    {
        vars.push_back(name);
    }

    void addVar(int coeff, string name, int idx)
    {
        string var, coef_;
        string sign = (coeff < 0) ? " - " : " + ";

        coeff = abs(coeff);
        coef_ = (coeff == 1) ? "" : to_string(coeff) + " ";
        var = sign + coef_ + name + "_" + to_string(idx);
        vars.push_back(var);
    }

    void addVar(int coeff, string name, int idx1, int idx2)
    {
        string var, coef_;
        string sign = (coeff < 0) ? " - " : " + ";

        coeff = abs(coeff);
        coef_ = (coeff == 1) ? "" : to_string(coeff) + " ";
        var = sign + coef_ + name + "_" + to_string(idx1) + "_" + to_string(idx2);
        vars.push_back(var);
    }

    void addRhs(string sense, int val)
    {
        rhs = " " + sense + " " + to_string(val);
    }
};

inline ostream &operator<<(ostream &os, LPStringBuilder &builder)
{
    int len = builder.id.length();

    os << builder.id;

    for (uint i = 0; i < builder.vars.size(); i++)
    {
        if (len + builder.vars[i].length() > max_width)
        {
            os << endl;

            if (builder.id.length() > 0)
                os << setw(builder.id.length()) << " ";

            len = builder.id.length();
        }

        len += builder.vars[i].length();
        os << builder.vars[i];
    }

    if (builder.rhs.length() > 0)
        os << builder.rhs;

    return os;
}

// Defines, builds and saves the network
struct Graph
{
    const Data *data;

    VectArc arcs;

    VectCommodity commods;

	// If point-to-point primary topology: all arcs
	// If hub-and-sat primary topology: only arcs between hub nodes
    // total number of arcs
    int numbArcs;

	// If point-to-point primary topology: all random arcs added to secondary topology
	// If hub-and-sat primary topology: all random hub arcs added to secondary topology
    int numbRandomArcs;
	
	// If point-to-point primary topology: all arcs defined according to secondary topology
	// If hub-and-sat primary topology: only hub arcs defined according to secondary topology
    int numbNonRandomArcs;

    int numbCommods;

    int numbNodes;

    int totalVolume;

    Random rnd;

    Graph(const Data *_data);

    void addRandomArc(int orig, int dest);

    // add arc while updating total number of arcs
    void addArc(const Arc &arc)
    {
        arcs.push_back(arc);
        numbArcs++;
    }

    // add arc while updating number of non-random arcs (i.e.,
    // defined while building a grid-like or a circular
    // secondary primary topology, or when reading a basic graph from file
    void addNonRandomArc(const Arc &arc)
    {
        addArc(arc);
        numbNonRandomArcs++;
    }

    // Generates initial commodity volumes.
    void defineVolumes();

    // Adds arcs to graph.
    void addArcs();

    void addHorizArcsInGrid(int node, int i, const VectCommodOnArc &commodsOnArc);

    void addVertArcsInGrid(int node, int i, int j, const VectCommodOnArc &commodsOnArc);

    void addCircArcs();

    bool findArc(int orig, int dest);

    void addRandomArcs(int nodeInit, int numbAddArcs);

    void adjustRandomArcs();

    void defineCommodities();

    void defCommsOneSourceOneSink();

    void defCommsEquSourcesEquSinks();

    void defCommsDefault();

    void balanceCommodity(int k, int totalSupply, int totalDemand);

    void adjustArcCapacities();

    void adjustFixedCosts();
	
	void equalizeCommodOnArcs();

    void readBasicGraph(string longBasicGraphFileName, const VectCommodOnArc &commodsOnArc);

    void outputBasicGraph(string longBasicGraphFileName);

    void output();

    void outputDow(string fileName);

    void outputStd(string fileName);

    void outputLP(string fileName);

    void outputMPS(string fileName);

    void outputTexGrid(string fileName);

    void outputTexCirc(string fileName);

    void outputTexGeneric(string fileName);
	
	void outputTexGeneric_orig(string fileName);

    void outputVar(ofstream &outFile, int &len, string id, string var);

    void outputVar(ofstream &outFile, int &len, string id, int coeff, string name, int val);

    void outputVar(ofstream &outFile, int &len, string id, int coeff, string name, int val1, int val2);
	
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	
	// Variables and methods that are used only when the primary primary topology is hubs-and-satellites
	
	// number of satellite arcs (involving at least one satellite node) when primary topology is hubs-and-satellites
	int numbSatArcs = 0;	

    // total number of satellite random arcs added
    int numbSatRandomArcs = 0;
	
    // total number of non-random satellite arcs
    int numbSatNonRandomArcs = 0;	
	
	// number of satellite nodes when primary topology is hubs-and-satellites
	int numbSatNodes = 0;
	
	// add local satellite arcs (connected to at most one hub) when primary topology is hubs-and-satellites
	void addSatArcs();	

	void addSatRandomArc(int orig, int dest);
	
	void addSatRandomArcs(int nodeInit, int numbAddSatArcs);

    // add satellite arc while updating total number of arcs
    void addSatArc(const Arc &satArc)
    {
        arcs.push_back(satArc);
        numbSatArcs++;
    }

    // add satellite arc while updating number of satellite non-random arcs
    void addSatNonRandomArc(const Arc &satArc)
    {
        addSatArc(satArc);
        numbSatNonRandomArcs++;
    }	
	
	bool findSatArc(int orig, int dest);
	
	void defineCommodities_sat();

    void defCommsOneSourceOneSink_sat();

    void defCommsEquSourcesEquSinks_sat();

    void defCommsDefault_sat();

    void adjustArcCapacities_sat();

    void adjustFixedCosts_sat();

    void adjustRandomArcs_sat();

	void equalizeCommodOnArcs_sat();	
	
};

#endif
