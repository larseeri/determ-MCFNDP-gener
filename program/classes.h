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
        rng(); // to initialize the generator
               // properly
    }

    uint32_t getRandom()
    {
        return rng();
    }

    uint32_t getRandom(int width)
    {
        return (width > 0) ? rng(width) : 0;
    }

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

// Processing of configuration data

struct Data
{
    ParamProcessing paramProcessing;

    // Parameters whose values are set in instruction line or
    // configurationt file (see Data::Data() for default and
    // admissible values).

    // flags creation of base graph with grid like arc
    // connections between nodes; the grid is laid on a torus, a
    // cylinder or a flat surface
    bool gridConnect;

    // flags creation of base graph with grid like arc
    // connections between nodes; the grid is laid on a torus
    bool toralConnect;

    // flags creation of base graph with grid like arc
    // connections between nodes; the grid is laid on a cylinder
    bool cylConnect;

    // flags creation of base graph with grid like arc
    // connections between nodes; the grid is laid on a flat
    // surface
    bool flatConnect;

    // flags creation of base graph with circular arc
    // connections between nodes
    bool circConnect;

    // flags creation of purely random arc connections between
    // nodes
    bool randConnect;

    // flags reading basic graph (nodes + arcs without costs and
    // capacities) from file
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
    // there are multiple commodities, commodity- specific
    // capacity constraints (a.k.a. strong forcing constraints)
    // will be included and that design variables will appear on
    // their RHS
    bool integCommCapacs;

    // Note: In constrast with DOW, STD and TEX formats, LP and
    // MPS formats describe the mixed integer linear program
    // (MILP) solving the generated instance of MCFND problem;
    // hence, inclDesignObjCap and inclDesignObjCommCap
    // determine the particular forms taken by the LP and MPS
    // representations (see Graph::outputLP() and
    // Graph::outputMPS() in graph.cpp)

    // flags absence of specific capacities for individual
    // commodities over arcs
    bool noCommCapacs;

    // stream from which random numbers will be extracted
    int stream;

    // random seed
    int seed;

    // length of grid along x axis (effective only when
    // gridConnect == true)
    int lengthX;

    // length of grid along y axis (effective only when
    // gridConnect == true)
    int lengthY;

    // number of nodes (effective only when gridConnect == false
    // AND fromFile == false)
    int numbNodes;

    // number of commodities
    int numbCommods;

    // total number of random arcs generated by sampling over
    // tail and head nodes
    int numbAddRandArcs_SamplTailsHeads;

    // number of random arcs per tail node generated by sampling
    // over head nodes
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
    // initially generated
    int minFixedCost;

    // upper bound for fixed cost over random arcs when
    // initially generated; fixed cost over non-random arc when
    // initially generated
    int maxFixedCost;

    // lower bound for variable cost for commodity over arc when
    // initially created, before ensuing adjustments
    int minVarCost;

    // maximum variable cost for commodity over arc when
    // initially created, before ensuing adjustments
    int maxVarCost;

    // lower bound for overall capacity of random arc when
    // initially created, before ensuing adjustments
    int minArcCapac;

    // upper bound for overall capacity of random arc when
    // initially created, before ensuing adjustments
    int maxArcCapac;

    // minimum capacity of arc for commodity when initially
    // created, before ensuing adjustments
    int minCommCapac;

    // maximum capacity of arc for commodity when initially
    // created, before ensuing adjustments
    int maxCommCapac;

    // flags uniform downward adjustment (equal to
    // adjFactCapacs) of capacities over arcs
    bool adjCapacsDown;

    // magnitude of uniform downward adjustment of overall
    // capacities of all arcs (non-random and random); effective
    // only when adjCapacsDown == true
    int adjFactCapacs;

    // flags uniform upward adjustment (equal to
    // adjFactFixCosts) of fixed costs over arcs
    bool adjFixCostsUp;

    // magnitude of uniform upward adjustment of fixed costs all
    // arcs (non-random and random); effective only when
    // adjFixCostsUp == true
    int adjFactFixCosts;

    // percent of random arcs with fixed cost set equal to zero
    int ratioZeroFixedCost;

    // percent of random arcs with capacity set equal to total
    // volume
    int ratioTotVolCapac;

    // percent of random arcs with one of the commodity specific
    // capacities set equal to zero
    int ratioZeroCommCapac;

    // percent of random arcs with one of the commodity specific
    // capacities set equal to maxArcCapac
    int ratioMaxCommCapac;

    // number of attempts to create a non-parallel random arc
    // before decrementing numbAddArcs (effective only when
    // noParallelArcs == true)
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

    void printHelpAndExit();

    void printUsageAndExit(char ExecName[], bool anonymous);
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

// Defining arcs

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

// Defining commodity nodes

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

// Defining commodities

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

// Building graph

struct Graph
{
    const Data *data;

    VectArc arcs;

    VectCommodity commods;

    // total number of arcs
    int numbArcs;

    // total number of random arcs added
    int numbRandomArcs;

    // total number of arcs defined while building a grid-like
    // or a circular topology, or when reading a basic graph
    // from file
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
    // topology, or when reading a basic graph from file
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

    void outputVar(ofstream &outFile, int &len, string id, string var);

    void outputVar(ofstream &outFile, int &len, string id, int coeff, string name, int val);

    void outputVar(ofstream &outFile, int &len, string id, int coeff, string name, int val1, int val2);
};

#endif
