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
//
//

#include "classes.h"
#include <algorithm>
#include <iterator>
#include <math.h>
#include <set>

void checkIfValidOutputFile(ofstream& os, string fileName)
{
	if (!os.is_open())
        {
            cerr << "\nERROR:\n"
				<< "The specified output file " << fileName << " could not be opened.\n"
				<< "EXECUTION ABORTED" << endl;
            exit(1);
        }		
}

bool is_empty_or_whitespace(const string& str) {
    return str.empty() || all_of(str.begin(), str.end(), [](unsigned char c) {
        return isspace(c);
    });
}

bool isDigitsOnly(const string& str)
{   
    if (is_empty_or_whitespace(str))
		return false; 
    
    // cast to unsigned char prevents undefined behavior with negative values
    return all_of(str.begin(), str.end(), [](unsigned char c) { 
        return isdigit(c); 
    });
}

bool can_convert_to_int(double val, string underMess, string overMess)
{    
  
    if (val < static_cast<double>(INT_MIN))
	{
		cerr << "ERROR:\n" << underMess << "\n"		
			<< "EXECUTION ABORTED" << endl;
			exit(1);
		
	}
	else if (val > static_cast<double>(INT_MAX))
	{
		cerr << "ERROR:\n" << overMess << "\n"		
			<< "EXECUTION ABORTED" << endl;
			exit(1);
    }
	return true;
}

int CommodNode::count = 0;

// Constructor
Graph::Graph(const Data *_data) : data(_data), rnd(_data->seed, _data->stream)
{
	// total number of nodes if primary topology is point-to-point;
	// total number of hubs if primary topology is hubs-and-satellites
    numbNodes = data->numbNodes;
	
    // total number of arcs if primary topology is point-to-point;
	// total number of hub arcs (arcs between hubs)
	// if primary topology is hubs-and-satellites
	// numbArcs = numbRandomArcs + numbNonRandomArcs 
    numbArcs = 0;
	
	// total number of added random arcs if primary topology is point-to-point;
	// total number of added random hub arcs if primary topology is hubs-and-satellites
    numbRandomArcs = 0;
	
	// total number of non-random arcs if primary topology is point-to-point;
	// total number of non-random hub arcs if primary topology is hubs-and-satellites	
    numbNonRandomArcs = 0;	
	
	// total number of satellites if primary topology is hubs-and-satellites
	numbSatNodes = 0;	
	
	// total number of satellite arcs (arcs between satellites or hub and satellite)
	// used only if primary topology is hubs-and-satellites
	// numbSatArcs = numbSatRandomArcs + numbSatNonRandomArcs 
	numbSatArcs = 0;	

	// total number of random satellite arcs (between satellites or hub and satellite)
	// used only if primary topology is hubs-and-satellites
    numbSatRandomArcs = 0;

	// total number of non random satellite arcs (between satellites or hub and satellite)
	// used only if primary topology is hubs-and-satellites
    numbSatNonRandomArcs = 0;	

    numbCommods = data->numbCommods;

	// primary topology is point-to-point
	if (!data->hubSat)
	{
		// extends vector commods by subsituting instances of Commodity(numbNodes)
		commods.resize(numbCommods, numbNodes);

		defineVolumes();

		addArcs();

		adjustRandomArcs();

		defineCommodities();

		adjustArcCapacities();

		adjustFixedCosts();
		
		// if requested, equalize commodity-specific properties of all arcs
		equalizeCommodOnArcs();
		
	}
	// primary topology is hubs-and-satellites
	else
	{	
		commods.resize(numbCommods);

		defineVolumes();

		addArcs();
		
		addSatArcs();
		
		// handles arcs between hubs
		adjustRandomArcs();

		// handles arcs between satellites or between satellites and hubs
		adjustRandomArcs_sat();
		
		for (auto& comm : commods)
		{
			comm.commodNodes.resize(numbNodes + numbSatNodes); 
		}

		defineCommodities_sat();
		
		// handles arcs between hubs
		adjustArcCapacities();

		// handles arcs between satellites or between satellites and hubs
		adjustArcCapacities_sat();

		// handles arcs between hubs
		adjustFixedCosts();
		
		// handles arcs between satellites or between satellites and hubs
		adjustFixedCosts_sat();

		// if requested, equalize commodity-specific properties of hub arcs
		equalizeCommodOnArcs();
		
		// if requested, equalize commodity-specific properties of satellite arcs
		equalizeCommodOnArcs_sat();		
	}	
}

// Generates initial commodity volumes.
void Graph::defineVolumes()
{
    totalVolume = 0;

    for (int k = 0; k < numbCommods; k++)
    {		
        commods[k].volume = rnd.getRandom(data->minCommVol, data->maxCommVol - data->minCommVol + 1);
        totalVolume += commods[k].volume;
    }
}

// Adds arcs to graph.
// If point-to-point primary topology, adds all arcs;
// if hubs-and-satellites primary topology, adds only hub arcs 
// (arcs between hubs).
void Graph::addArcs()
{

    VectCommodOnArc commodsOnArc;

    // generate commodity related costs and capacities on arcs
    for (int k = 0; k < numbCommods; k++)
    {
        int cost = data->maxVarCost;
        int capac = commods[k].volume;

        commodsOnArc.push_back(CommodOnArc(cost, capac));
    }

    // generate grid like connections between nodes
    if (data->gridConnect)
    {
        for (int j = 0; j < data->lengthY; j++)
            for (int i = 0; i < data->lengthX; i++)
            {
                int node = j * data->lengthX + i;

                addHorizArcsInGrid(node, i, commodsOnArc);
                addVertArcsInGrid(node, i, j, commodsOnArc);
            }
    }
    // generate circular connections between nodes
    else if (data->circConnect)
    {
        for (int n = 0; n < numbNodes; n++)
        {
            int dest = (n < numbNodes - 1) ? n + 1 : 0;

            addNonRandomArc(Arc(n, dest, data->maxFixedCost, totalVolume, commodsOnArc));
        }
    }
    // read basic graph from file
    else if (data->fromFile)
    {
		try
		{
			readBasicGraph(data->longBasicGraphFileName, commodsOnArc);
		}		
		catch (...)
		{
			cerr << "ERROR:\n"
				<< "An exception occurred while attempting to read file " << data->longBasicGraphFileName << "\n"			
				<< "Ascertain (i) that the file exists and\n"
				<< "(ii) that it conforms to the format expected from a basic graph file.\n"
				<< "EXECUTION ABORTED" << endl;
			exit(1);		
		}
    }

    // generate additional arcs randomly by sampling both tail and head nodes
	if (data->numbAddRandArcs_SamplTailsHeads)
		addRandomArcs(-1, data->numbAddRandArcs_SamplTailsHeads);

    // generate additional arcs randomly at each node by sampling only head nodes
    if (data->numbAddRandArcsPerNode_SamplHeads)
    {
        for (int n = 0; n < numbNodes; n++)
        {
            addRandomArcs(n, data->numbAddRandArcsPerNode_SamplHeads);
        }
    }
}

// Adds horizontal arcs in grid.
// If point-to-point primary topology and grid-like secondary topology,
// adds horizontal arcs between nodes; if hubs-and-satellites primary topology
// and grid-like secondary topology of hubs, adds horizontal arcs between hub nodes.
void Graph::addHorizArcsInGrid(int node, int i, const VectCommodOnArc &commodsOnArc)
{
    if (data->lengthX > 1)
    {
        if (i > 0 && i < data->lengthX - 1)
        {
            addNonRandomArc(Arc(node, node + 1, data->maxFixedCost, totalVolume, commodsOnArc));

            addNonRandomArc(Arc(node, node - 1, data->maxFixedCost, totalVolume, commodsOnArc));
        }
        else if (i == 0)
        {
            addNonRandomArc(Arc(node, node + 1, data->maxFixedCost, totalVolume, commodsOnArc));
            if (data->lengthX > 2 && (data->toralConnect || data->cylConnect))
                addNonRandomArc(Arc(node, node + (data->lengthX - 1), data->maxFixedCost, totalVolume, commodsOnArc));
        }
        else
        {
            if (data->lengthX > 2 && (data->toralConnect || data->cylConnect))
                addNonRandomArc(Arc(node, node - (data->lengthX - 1), data->maxFixedCost, totalVolume, commodsOnArc));

            addNonRandomArc(Arc(node, node - 1, data->maxFixedCost, totalVolume, commodsOnArc));
        }
    }
}

// Adds vertical arcs in grid.
// If point-to-point primary topology and grid-like secondary topology,
// adds vertical arcs between nodes; if hubs-and-satellites primary topology and
// grid-like secondary topology of hubs, adds vertical arcs between hub nodes.
void Graph::addVertArcsInGrid(int node, int i, int j, const VectCommodOnArc &commodsOnArc)
{
    if (data->lengthY > 1)
    {
        if (j > 0 && j < data->lengthY - 1)
        {
            addNonRandomArc(Arc(node, node + data->lengthX, data->maxFixedCost, totalVolume, commodsOnArc));

            addNonRandomArc(Arc(node, node - data->lengthX, data->maxFixedCost, totalVolume, commodsOnArc));
        }
        else if (j == 0)
        {
            addNonRandomArc(Arc(node, node + data->lengthX, data->maxFixedCost, totalVolume, commodsOnArc));
            if (data->lengthY > 2 && data->toralConnect)
                addNonRandomArc(Arc(node, node + data->lengthX * (data->lengthY - 1), data->maxFixedCost, totalVolume,
                                    commodsOnArc));
        }
        else
        {
            if (data->lengthY > 2 && data->toralConnect)
                addNonRandomArc(Arc(node, i, data->maxFixedCost, totalVolume, commodsOnArc));

            addNonRandomArc(Arc(node, node - data->lengthX, data->maxFixedCost, totalVolume, commodsOnArc));
        }
    }
}

// Adds arcs to graph.
// Used only if hubs-and-satellites primary topology: adds satellite arcs
// (between satellites or hub and satellite).
void Graph::addSatArcs()
{
	
	int cost, capac, start, end, width, remain, roundSize;

    VectCommodOnArc commodsOnArc;

    // generate commodity related costs and capacities on satellite arcs
    for (int k = 0; k < numbCommods; k++)
    {
        cost = data->maxSatVarCost;
        capac = commods[k].volume;

        commodsOnArc.push_back(CommodOnArc(cost, capac));
    }

	// iterate over hubs
	for (int indHub = 0; indHub < numbNodes; indHub++)
    {
		// select number of satellite nodes tied to this hub
		remain = rnd.getRandom(data->minSatsPerHub, data->maxSatsPerHub - data->minSatsPerHub + 1);
		
		// create the rounds and their connecting arcs
		while (remain != 0)
		{		
			start = min(data->minSatsRoundSize, remain);
			end = min(data->maxSatsRoundSize, remain);			
			width = end - start;					
			roundSize =  rnd.getRandom(start, width + 1);			
			
			// create pair of arcs between hub and first satellite in round			
			addSatNonRandomArc(Arc(indHub, (numbNodes + numbSatNodes), data->maxSatFixedCost, totalVolume, commodsOnArc));
			addSatNonRandomArc(Arc((numbNodes + numbSatNodes), indHub, data->maxSatFixedCost, totalVolume, commodsOnArc));			
			numbSatNodes++;
			
			if (roundSize > 1)
			{
				// create pairs of arcs between satellite nodes
				for (int indSat = 1; indSat < roundSize; indSat++)
				{				
					addSatNonRandomArc(Arc((numbNodes + numbSatNodes - 1), (numbNodes + numbSatNodes) , data->maxSatFixedCost, totalVolume, commodsOnArc));
					addSatNonRandomArc(Arc((numbNodes + numbSatNodes), (numbNodes + numbSatNodes - 1) , data->maxSatFixedCost, totalVolume, commodsOnArc));				
					numbSatNodes++;
				}
			
				// create pair of arcs between last satellite in round and hub
				addSatNonRandomArc(Arc((numbNodes + numbSatNodes - 1), indHub, data->maxSatFixedCost, totalVolume, commodsOnArc));
				addSatNonRandomArc(Arc(indHub, (numbNodes + numbSatNodes - 1), data->maxSatFixedCost, totalVolume, commodsOnArc));	
			}			
			remain -= roundSize;			
		}
				
	}

    // generate satellite arcs randomly by sampling both tail and head nodes	
	if (data->numbAddSatRandArcs_SamplTailsHeads)
		addSatRandomArcs(-1, data->numbAddSatRandArcs_SamplTailsHeads);

    // generate satellite arcs randomly at each node by sampling only head nodes
    if (data->numbAddSatRandArcsPerNode_SamplHeads)
    {
        for (int n = numbNodes; n < numbNodes + numbSatNodes; n++)
        {
            addSatRandomArcs(n, data->numbAddSatRandArcsPerNode_SamplHeads);
        }
    }
}

// Returns true if arc has already been generated.
// If point-to-point primary topology, finds arc between any nodes;
// if hubs-and-satellites primary topology, finds arc between hub nodes.
bool Graph::findArc(int orig, int dest)
{
    bool found = false;

    for (int a = 0; a < numbArcs && !found; a++)
        if (arcs[a].orig == orig && arcs[a].dest == dest)
            found = true;

    return found;
}

// Returns true if arc has already been generated.
// Only used if hubs-and-satellites primary topology: finds arc between any nodes.
bool Graph::findSatArc(int orig, int dest)
{
    bool found = false;

    for (int a = 0; a < numbArcs + numbSatArcs && !found; a++)
        if (arcs[a].orig == orig && arcs[a].dest == dest)
            found = true;

    return found;
}

// Adds arc with random costs and capacities.
// If point-to-point primary topology, adds random arc between nodes;
// if hubs-and-satellites primary topology, adds arc between hub nodes.
void Graph::addRandomArc(int orig, int dest)
{
    int cost = rnd.getRandom(data->minFixedCost, data->maxFixedCost - data->minFixedCost + 1);
    int capac = rnd.getRandom(data->minArcCapac, data->maxArcCapac - data->minArcCapac + 1);

    Arc arc(orig, dest, cost, capac);

    for (int k = 0; k < numbCommods; k++)
    {
        cost = rnd.getRandom(data->minVarCost, data->maxVarCost - data->minVarCost + 1);
        capac = rnd.getRandom(data->minCommCapac, data->maxCommCapac - data->minCommCapac + 1);
        arc.addCommodOnArc(CommodOnArc(cost, capac));
    }

    addArc(arc);
    numbRandomArcs++;
}

// Adds arc with random costs and capacities.
// Only used if hubs-and-satellites primary topology: adds satellite arc
// (satellite arcs connect at least one satellite node).
void Graph::addSatRandomArc(int orig, int dest)
{
    int cost = rnd.getRandom(data->minSatFixedCost, data->maxSatFixedCost - data->minSatFixedCost + 1);
    int capac = rnd.getRandom(data->minSatArcCapac, data->maxSatArcCapac - data->minSatArcCapac + 1);

    Arc arc(orig, dest, cost, capac);

    for (int k = 0; k < numbCommods; k++)
    {
        cost = rnd.getRandom(data->minSatVarCost, data->maxSatVarCost - data->minSatVarCost + 1);
        capac = rnd.getRandom(data->minSatCommCapac, data->maxSatCommCapac - data->minSatCommCapac + 1);
        arc.addCommodOnArc(CommodOnArc(cost, capac));
    }

    addSatArc(arc);
    numbSatRandomArcs++;
}

// Adds arcs with random costs and capacities.
// If point-to-point primary topology, adds random arcs between nodes;
// if hubs-and-satellites primary topology, adds random arcs between hub nodes.
void Graph::addRandomArcs(int nodeInit, int numbAddArcs)
{
    int orig = nodeInit;
    int dest;

    for (int a = 0; a < numbAddArcs; a++)
    {
        int iter = data->maxIterNoParallelArcs;
        bool found = false;

        do
        {
            if (nodeInit < 0)
                orig = rnd.getRandom(0, numbNodes);

            do
                dest = rnd.getRandom(0, numbNodes);
            while (orig == dest);

            if (data->noParallelArcs)
            {
                found = findArc(orig, dest);

                if (found)
                    iter--;
                // if cannot find any arc while in noParallelArcs mode,
                // then try again for maxIterNoParallelArcs iterations.
                // If still not found, then the graph will have one less
                // random arc
            }
        } while (found && iter >= 0);

        if (iter >= 0)
        {
            addRandomArc(orig, dest);
        }
    }
}

// Adds satellite arcs with random costs and capacities.
// Only used if hubs-and-satellites primary topology: adds random satellite arcs
// (satellite arcs connect at least one satellite node).
void Graph::addSatRandomArcs(int nodeInit, int numbAddArcs)
{
    int orig = nodeInit;
    int dest;

    for (int a = 0; a < numbAddArcs; a++)
    {
        int iter = data->maxIterNoParallelArcs;
        bool found = false;

        do
        {
            if (nodeInit < 0)
                orig = rnd.getRandom(0, numbNodes + numbSatNodes);
			
			// if origin is a hub
			if (orig < numbNodes)
			{
				do
					dest = rnd.getRandom(numbNodes, numbSatNodes);
				while (orig == dest);
			}
			else
			{
				do
					dest = rnd.getRandom(0, numbNodes + numbSatNodes);
				while (orig == dest);				
			}
			
            if (data->noParallelArcs)
            {
                found = findSatArc(orig, dest);

                if (found)
                    iter--;
                // if cannot find any arc while in noParallelArcs mode,
                // then try again for maxIterNoParallelArcs iterations.
                // If still not found, then the graph will have one less
                // random arc
            }
        }
		while (found && iter >= 0);

        if (iter >= 0)
        {
            addSatRandomArc(orig, dest);
        }
    }
}

// Adjusts characteristics of random arcs according to parameters.
// If point-to-point primary topology, adjusts random arcs between nodes;
// if hubs-and-satellites primary topology, adjusts random arcs between hubs.
void Graph::adjustRandomArcs()
{
    if (data->ratioZeroFixedCost > 0)
    {
        // desired number of random arcs with fixed cost set to zero
        int numbArcsToModify = (int)(data->ratioZeroFixedCost * numbRandomArcs / 100);
		
		int vecIndex, arcIndex, currSize;		
		vector<int> tempVec(numbRandomArcs); 

		// fill with increasing numbers starting at numbNonRandomArcs
		iota(tempVec.begin(), tempVec.end(), numbNonRandomArcs);	
		
		currSize = numbRandomArcs;	
		for (int i =0; i <  numbArcsToModify; i++)	
		{		
			vecIndex = rnd.getRandom(0, currSize);
			
			// O(1) time
			swap(tempVec[vecIndex], tempVec.back());
			
			arcIndex = tempVec.back();
			tempVec.pop_back();	
			currSize--;
			arcs[arcIndex].cost = 0;		
		}		
    }

    if (data->ratioTotVolCapac > 0)
    {
        // desired number of random arcs with capacity set to total volume
        int numbArcsToModify = (int)(data->ratioTotVolCapac * numbRandomArcs / 100);

		int vecIndex, arcIndex, currSize;		
		vector<int> tempVec(numbRandomArcs); 

		// fill with increasing numbers starting at numbNonRandomArcs
		iota(tempVec.begin(), tempVec.end(), numbNonRandomArcs);	
		
		currSize = numbRandomArcs;	
		for (int i =0; i <  numbArcsToModify; i++)	
		{		
			vecIndex = rnd.getRandom(0, currSize);
			
			// O(1) time
			swap(tempVec[vecIndex], tempVec.back());
			
			arcIndex = tempVec.back();
			tempVec.pop_back();	
			currSize--;
			arcs[arcIndex].capac = totalVolume;		
		}		
    }

    if (data->ratioZeroCommCapac > 0)
    {
        // desired number of random arcs with one commodity-specific capacity
        // set to zero
        int numbArcsToModify = (int)(data->ratioZeroCommCapac * numbRandomArcs / 100);
		
		int vecIndex, arcIndex, currSize;		
		vector<int> tempVec(numbRandomArcs); 

		// fill with increasing numbers starting at numbNonRandomArcs
		iota(tempVec.begin(), tempVec.end(), numbNonRandomArcs);	
		
		currSize = numbRandomArcs;	
		for (int i =0; i <  numbArcsToModify; i++)	
		{		
			vecIndex = rnd.getRandom(0, currSize);
			
			// O(1) time
			swap(tempVec[vecIndex], tempVec.back());
			
			arcIndex = tempVec.back();
			tempVec.pop_back();	
			currSize--;
			int k = rnd.getRandom(0, numbCommods);
			arcs[arcIndex].commodsOnArc[k].capac = 0;	
		}		
    }

    if (data->ratioMaxCommCapac)
    {
        // desired number of random arcs with one commodity-specific capacity
        // set to maxArcCapac
        int numbArcsToModify = (int)(data->ratioMaxCommCapac * numbRandomArcs / 100);
		
		int vecIndex, arcIndex, currSize;		
		vector<int> tempVec(numbRandomArcs); 

		// fill with increasing numbers starting at numbNonRandomArcs
		iota(tempVec.begin(), tempVec.end(), numbNonRandomArcs);	
		
		currSize = numbRandomArcs;	
		for (int i =0; i <  numbArcsToModify; i++)	
		{		
			vecIndex = rnd.getRandom(0, currSize);
			
			// O(1) time
			swap(tempVec[vecIndex], tempVec.back());
			
			arcIndex = tempVec.back();
			tempVec.pop_back();	
			currSize--;
			int k = rnd.getRandom(0, numbCommods);
			arcs[arcIndex].commodsOnArc[k].capac = data->maxCommCapac;	
		}		
    }
}

// Adjusts characteristics of random satellite arcs according to parameters.
// Only used if hubs-and-satellites primary topology: adjusts random satellite arcs
// (satellite arcs connect at least one satellite node).
void Graph::adjustRandomArcs_sat()
{
    if (data->ratioSatZeroFixedCost > 0)
    {
        // desired number of random arcs with fixed cost set to zero
        int numbArcsToModify = (int)(data->ratioSatZeroFixedCost * numbSatRandomArcs / 100);
 		
		int vecIndex, arcIndex, currSize;		
		vector<int> tempVec(numbSatRandomArcs); 

		// fill with increasing numbers starting at numbArcs + numbSatNonRandomArcs
		iota(tempVec.begin(), tempVec.end(), numbArcs + numbSatNonRandomArcs);	
		
		currSize = numbSatRandomArcs;	
		for (int i =0; i <  numbArcsToModify; i++)	
		{		
			vecIndex = rnd.getRandom(0, currSize);
			
			// O(1) time
			swap(tempVec[vecIndex], tempVec.back());
			
			arcIndex = tempVec.back();
			tempVec.pop_back();	
			currSize--;
			arcs[arcIndex].cost = 0;		
		}		
    }

    if (data->ratioSatTotVolCapac > 0)
    {
        // desired number of random arcs with capacity set to total volume
        int numbArcsToModify = (int)(data->ratioSatTotVolCapac * numbSatRandomArcs / 100);
		
		int vecIndex, arcIndex, currSize;		
		vector<int> tempVec(numbSatRandomArcs); 

		// fill with increasing numbers starting at numbArcs + numbSatNonRandomArcs
		iota(tempVec.begin(), tempVec.end(), numbArcs + numbSatNonRandomArcs);	
		
		currSize = numbSatRandomArcs;	
		for (int i =0; i <  numbArcsToModify; i++)	
		{		
			vecIndex = rnd.getRandom(0, currSize);
			
			// O(1) time
			swap(tempVec[vecIndex], tempVec.back());
			
			arcIndex = tempVec.back();
			tempVec.pop_back();	
			currSize--;
			arcs[arcIndex].capac = totalVolume;		
		}		
    }

    if (data->ratioSatZeroCommCapac > 0)
    {
        // desired number of random arcs with one commodity-specific capacity
        // set to zero
        int numbArcsToModify = (int)(data->ratioSatZeroCommCapac * numbSatRandomArcs / 100);
			
		int vecIndex, arcIndex, currSize;		
		vector<int> tempVec(numbSatRandomArcs); 

		// fill with increasing numbers starting at numbArcs + numbSatNonRandomArcs
		iota(tempVec.begin(), tempVec.end(), numbArcs + numbSatNonRandomArcs);	
		
		currSize = numbSatRandomArcs;	
		for (int i =0; i <  numbArcsToModify; i++)	
		{		
			vecIndex = rnd.getRandom(0, currSize);
			
			// O(1) time
			swap(tempVec[vecIndex], tempVec.back());
			
			arcIndex = tempVec.back();
			tempVec.pop_back();	
			currSize--;
			int k = rnd.getRandom(0, numbCommods);
			arcs[arcIndex].commodsOnArc[k].capac = 0;	
		}        
    }

    if (data->ratioSatMaxCommCapac)
    {
        // desired number of random arcs with one commodity-specific capacity
        // set to maxArcCapac
        int numbArcsToModify = (int)(data->ratioSatMaxCommCapac * numbSatRandomArcs / 100);
  	
		int vecIndex, arcIndex, currSize;		
		vector<int> tempVec(numbSatRandomArcs); 

		// fill with increasing numbers starting at numbArcs + numbSatNonRandomArcs
		iota(tempVec.begin(), tempVec.end(), numbArcs + numbSatNonRandomArcs);	
		
		currSize = numbSatRandomArcs;	
		for (int i =0; i <  numbArcsToModify; i++)	
		{		
			vecIndex = rnd.getRandom(0, currSize);
			
			// O(1) time
			swap(tempVec[vecIndex], tempVec.back());
			
			arcIndex = tempVec.back();
			tempVec.pop_back();	
			currSize--;
			int k = rnd.getRandom(0, numbCommods);
			arcs[arcIndex].commodsOnArc[k].capac = data->maxSatCommCapac;	
		} 		
    }
}

// Defines source(s)/sink(s) of commodities.
// Only used if point-to-point primary topology: sources and sinks
// can be on any node.
void Graph::defineCommodities()
{	
    if (!data->oneSourceOneSink)
    {				
        if (!data->equSourcesEquSinks)
		{			
            defCommsDefault();
		}
        else
		{			
            defCommsEquSourcesEquSinks();			
		}
    }
    else
	{
        defCommsOneSourceOneSink();
	}
}

// Defines source(s)/sink(s) of commodities.
// Only used if hubs-and-satellites primary topology: sources and sinks
// are exclusively on satellite nodes.
void Graph::defineCommodities_sat()
{
    if (!data->oneSourceOneSink)
    {
        if (!data->equSourcesEquSinks)
            defCommsDefault_sat();
        else
            defCommsEquSourcesEquSinks_sat();
    }
    else
        defCommsOneSourceOneSink_sat();
}

// Randomly selects sources and sinks for all commodities, ensuring
// there is only one source and only one sink for each commodity.
// (source, sink) pairs must be different across all commodities.
// Only used if point-to-point primary topology: sources and sinks can be
// on any node.
void Graph::defCommsOneSourceOneSink()
{	
    VectBool2 odmark(numbNodes, VectBool(numbNodes, false));
    int orig, dest;

    for (int k = 0; k < numbCommods; k++)
        commods[k].numbOrig = commods[k].numbDest = 1;

    for (int k = 0; k < numbCommods; k++)
    {
        do
        {
            orig = rnd.getRandom(0, numbNodes);
            do
                dest = rnd.getRandom(0, numbNodes);
            while (orig == dest);
        } while (odmark[orig][dest]);

        odmark[orig][dest] = true;
        commods[k].commodNodes[orig].volume = commods[k].volume;
        commods[k].commodNodes[dest].volume = -commods[k].volume;
    }
}

// Randomly selects sources and sinks for all commodities, ensuring
// there is only one source and only one sink for each commodity.
// (source, sink) pairs must be different across all commodities.
// Only used if hubs-and-satellites primary topology: sources and sinks
// are exclusively on satellite nodes.
void Graph::defCommsOneSourceOneSink_sat()
{	
    VectBool2 odmark(numbNodes + numbSatNodes, VectBool(numbNodes + numbSatNodes, false));
    int orig, dest;

    for (int k = 0; k < numbCommods; k++)
        commods[k].numbOrig = commods[k].numbDest = 1;

    for (int k = 0; k < numbCommods; k++)
    {
        do
        {
            orig = rnd.getRandom(numbNodes, numbSatNodes);
            do
                dest = rnd.getRandom(numbNodes, numbSatNodes);
            while (orig == dest);
        } while (odmark[orig][dest]);
		
        odmark[orig][dest] = true;
        commods[k].commodNodes[orig].volume = commods[k].volume;
        commods[k].commodNodes[dest].volume = -commods[k].volume;
    }
}

// Randomly selects sources and sinks for all commodities, ensuring
// that sources are identical over all commodities
// and sinks are identical over all commodities.
// Only used if point-to-point primary topology: sources and sinks can be
// on any node.
void Graph::defCommsEquSourcesEquSinks()
{	
    VectBool omark(numbNodes, false);
    VectBool dmark(numbNodes, false);
    int nbSources = 0, nbSinks = 0, node, temp;
    int surplus = 0, unassSupply = 0, unassDemand = 0;

    nbSources = rnd.getRandom(data->minNumbSources, data->maxNumbSources - data->minNumbSources + 1);
    nbSinks = rnd.getRandom(data->minNumbSinks, data->maxNumbSinks - data->minNumbSinks + 1);

    for (int i = 0; i < nbSources; i++)
    {
        do
            node = rnd.getRandom(0, numbNodes);
        while (omark[node]);

        omark[node] = true;
    }

    for (int i = 0; i < nbSinks; i++)
    {
        do
            node = rnd.getRandom(0, numbNodes);
        while (dmark[node] || omark[node]);

        dmark[node] = true;
    }

    for (int k = 0; k < numbCommods; k++)
    {		
        int totalSupply = 0;
        int totalDemand = 0;

        surplus = 0;

		// note that totalSupply >= 0 and surplus >= 0 always
        for (int n = 0; n < numbNodes; n++)
        {			
            if (omark[n])
            {			
				// avoids sources with zero assigned supply when possible
				if (commods[k].volume >= nbSources)		
					temp = rnd.getRandom(1, (int)(commods[k].volume / nbSources));
				// temp == 0 in next case and volume will remain unassigned until ref:100 below
				else
					temp = rnd.getRandom(0, (int)(commods[k].volume / nbSources) + 1);
				
                commods[k].commodNodes[n].volume = surplus + temp;
                totalSupply += commods[k].commodNodes[n].volume;
                surplus = (int)(commods[k].volume / nbSources) - temp;
            }
        }
		
		do 
			node = rnd.getRandom(0, numbNodes);
		while (!omark[node]);
        commods[k].commodNodes[node].volume += surplus;
		
        totalSupply += surplus;		
		unassSupply = commods[k].volume - totalSupply;
		
		// ref:100
		while (unassSupply != 0)
		{		
			do 
				node = rnd.getRandom(0, numbNodes);
			while (!omark[node]);
			
			commods[k].commodNodes[node].volume++;
			totalSupply++;
			unassSupply--;
		}				
		
        surplus = 0;

		// note that totalDemand >= 0 and surplus >= 0 always
        for (int n = 0; n < numbNodes; n++)
        {			
            if (dmark[n])
            {				
				// avoids sinks with zero assigned demand when possible
				if (commods[k].volume >= nbSinks)		
					temp = rnd.getRandom(1, (int)(commods[k].volume / nbSinks));
				// temp == 0 in next case and volume will remain unassigned until ref:101 below
				else
					temp = rnd.getRandom(0, (int)(commods[k].volume / nbSinks) + 1);
							
                commods[k].commodNodes[n].volume = -(surplus + temp);
                totalDemand -= commods[k].commodNodes[n].volume;
                surplus = (int)(commods[k].volume / nbSinks) - temp;
            }
        }

		do 
			node = rnd.getRandom(0, numbNodes);
		while (!dmark[node]);
        commods[k].commodNodes[node].volume -= surplus;
		
        totalDemand += surplus;		
		unassDemand = commods[k].volume - totalDemand;		
		
		// ref:101
		while (unassDemand != 0)
		{		
			do 
				node = rnd.getRandom(0, numbNodes);
			while (!dmark[node]);
			
			commods[k].commodNodes[node].volume--;
			totalDemand--;
			unassDemand--;
		}		
    }	
}

// Randomly selects sources and sinks for all commodities, ensuring
// that sources are identical over all commodities
// and sinks are identical over all commodities.
// Only used if hubs-and-satellites primary topology: sources and sinks
// are exclusively on satellite nodes.
void Graph::defCommsEquSourcesEquSinks_sat()
{	
    VectBool omark(numbNodes + numbSatNodes, false);	
    VectBool dmark(numbNodes + numbSatNodes, false);
    int nbSources = 0, nbSinks = 0, node, temp;
    int surplus = 0, unassSupply = 0, unassDemand = 0;

    nbSources = rnd.getRandom(data->minNumbSources, data->maxNumbSources - data->minNumbSources + 1);	
    nbSinks = rnd.getRandom(data->minNumbSinks, data->maxNumbSinks - data->minNumbSinks + 1);

    for (int i = 0; i < nbSources; i++)
    {
        do
            node = rnd.getRandom(numbNodes, numbSatNodes);
        while (omark[node]);

        omark[node] = true;
    }

    for (int i = 0; i < nbSinks; i++)
    {
        do
            node = rnd.getRandom(numbNodes, numbSatNodes);
        while (dmark[node] || omark[node]);

        dmark[node] = true;
    }

    for (int k = 0; k < numbCommods; k++)
    {
        int totalSupply = 0;
        int totalDemand = 0;

        surplus = 0;
		
		// note that totalSupply >= 0 and surplus >= 0 always
        for (int n = numbNodes; n < numbNodes + numbSatNodes; n++)
        {
            if (omark[n])
            {				
				// avoids sources with zero assigned supply when possible
				if (commods[k].volume >= nbSources)		
					temp = rnd.getRandom(1, (int)(commods[k].volume / nbSources));
				// temp == 0 in next case and volume will remain unassigned until ref:200 below
				else
					temp = rnd.getRandom(0, (int)(commods[k].volume / nbSources) + 1);				
				
                commods[k].commodNodes[n].volume = surplus + temp;
                totalSupply += commods[k].commodNodes[n].volume;
                surplus = (int)(commods[k].volume / nbSources) - temp;
            }
        }

		do 
			node = rnd.getRandom(numbNodes, numbSatNodes);
		while (!omark[node]);
        commods[k].commodNodes[node].volume += surplus;
		
        totalSupply += surplus;		
		unassSupply = commods[k].volume - totalSupply;
		
		// ref:200
		while (unassSupply != 0)
		{		
			do 
				node = rnd.getRandom(numbNodes, numbSatNodes);
			while (!omark[node]);
			
			commods[k].commodNodes[node].volume++;		
			unassSupply--;
		}

        surplus = 0;
		
		// note that totalDemand >= 0 and surplus >= 0 always
        for (int n = numbNodes; n < numbNodes + numbSatNodes; n++)
        {
            if (dmark[n])
            {            				
				// avoids sinks with zero assigned demand when possible
				if (commods[k].volume >= nbSinks)		
					temp = rnd.getRandom(1, (int)(commods[k].volume / nbSinks));
				// temp == 0 in next case and volume will remain unassigned until ref:201 below
				else
					temp = rnd.getRandom(0, (int)(commods[k].volume / nbSinks) + 1);
				
                commods[k].commodNodes[n].volume = -(surplus + temp);
                totalDemand -= commods[k].commodNodes[n].volume;
                surplus = (int)(commods[k].volume / nbSinks) - temp;
            }
        }
		
		do 
			node = rnd.getRandom(numbNodes, numbSatNodes);
		while (!dmark[node]);
        commods[k].commodNodes[node].volume -= surplus;
		
        totalDemand += surplus;		
		unassDemand = commods[k].volume - totalDemand;		
		
		// ref:201
		while (unassDemand != 0)
		{		
			do 
				node = rnd.getRandom(numbNodes, numbSatNodes);
			while (!dmark[node]);
			
			commods[k].commodNodes[node].volume--;		
			unassDemand--;
		}			
    }	
}

// Randomly selects sources and sinks for all commodities without
// additional conditions.
// Only used if point-to-point primary topology: sources and sinks
// can be on any node.
void Graph::defCommsDefault()
{
    VectBool2 omark(numbCommods, VectBool(numbNodes, false));
	VectBool2 dmark(numbCommods, VectBool(numbNodes, false));

    for (int k = 0; k < numbCommods; k++)
    {
        commods[k].numbOrig = rnd.getRandom(data->minNumbSources, data->maxNumbSources - data->minNumbSources + 1);
        commods[k].numbDest = rnd.getRandom(data->minNumbSinks, data->maxNumbSinks - data->minNumbSinks + 1);
    }

    for (int k = 0; k < numbCommods; k++)
    {
        int surplus = 0, totalSupply = 0, totalDemand = 0, node, temp;
		int unassSupply = 0, unassDemand = 0;

		// note that totalSupply >= 0 and surplus >= 0 always		
        for (int s = 0; s < commods[k].numbOrig; s++)
        {
            do
                node = rnd.getRandom(0, numbNodes);
            while (omark[k][node]);

            omark[k][node] = true;			
			
			// avoids sources with zero assigned supply when possible
			if (commods[k].volume >= commods[k].numbOrig)		
				temp = rnd.getRandom(1, (int)(commods[k].volume / commods[k].numbOrig));
				// temp == 0 in next case and volume will remain unassigned until ref:300 below
			else
				temp = rnd.getRandom(0, (int)(commods[k].volume / commods[k].numbOrig) + 1);
			
            commods[k].commodNodes[node].volume = surplus + temp;
            totalSupply += commods[k].commodNodes[node].volume;
            surplus = (int)(commods[k].volume / commods[k].numbOrig) - temp;
        }        

		do 
			node = rnd.getRandom(0, numbNodes);
		while (!omark[k][node]);
        commods[k].commodNodes[node].volume += surplus;
		
        totalSupply += surplus;		
		unassSupply = commods[k].volume - totalSupply;
		
		// ref:300
		while (unassSupply != 0)
		{		
			do 
				node = rnd.getRandom(0, numbNodes);
			while (!omark[k][node]);
			
			commods[k].commodNodes[node].volume++;		
			unassSupply--;
		}		

        surplus = 0;
		
		// note that totalDemand >= 0 and surplus >= 0 always
        for (int s = 0; s < commods[k].numbDest; s++)
        {
            do
                node = rnd.getRandom(0, numbNodes);
            while (dmark[k][node] || omark[k][node]);

            dmark[k][node] = true;			
			            				
			// avoids sinks with zero assigned demand when possible
			if (commods[k].volume >= commods[k].numbDest)		
				temp = rnd.getRandom(1, (int)(commods[k].volume / commods[k].numbDest));
			// temp == 0 in next case and volume will remain unassigned until ref:301 below
			else
				temp = rnd.getRandom(0, (int)(commods[k].volume / commods[k].numbDest) + 1);
            
            commods[k].commodNodes[node].volume = -(surplus + temp);
            totalDemand -= commods[k].commodNodes[node].volume;
            surplus = (int)(commods[k].volume / commods[k].numbDest) - temp;
        }
		
		do 
			node = rnd.getRandom(0, numbNodes);
		while (!dmark[k][node]);
        commods[k].commodNodes[node].volume -= surplus;
		
        totalDemand += surplus;		
		unassDemand = commods[k].volume - totalDemand;		
		
		// ref:301
		while (unassDemand != 0)
		{		
			do 
				node = rnd.getRandom(0, numbNodes);
			while (!dmark[k][node]);
			
			commods[k].commodNodes[node].volume--;		
			unassDemand--;
		}				
    }	
}

// Randomly selects sources and sinks for all commodities without
// additional conditions.
// Only used if hubs-and-satellites primary topology: sources and sinks
// are exclusively on satellite nodes.
void Graph::defCommsDefault_sat()
{
    VectBool2 omark(numbCommods, VectBool(numbNodes + numbSatNodes, false));
	VectBool2 dmark(numbCommods, VectBool(numbNodes + numbSatNodes, false));

    for (int k = 0; k < numbCommods; k++)
    {
        commods[k].numbOrig = rnd.getRandom(data->minNumbSources, data->maxNumbSources - data->minNumbSources + 1);
        commods[k].numbDest = rnd.getRandom(data->minNumbSinks, data->maxNumbSinks - data->minNumbSinks + 1);
    }

    for (int k = 0; k < numbCommods; k++)
    {
        int surplus = 0, totalSupply = 0, totalDemand = 0, node, temp;
		int unassSupply = 0, unassDemand = 0;
		
        // note that totalSupply >= 0 and surplus >= 0 always
        for (int s = 0; s < commods[k].numbOrig; s++)
        {
            do
                node = rnd.getRandom(numbNodes, numbSatNodes);
            while (omark[k][node]);
			
            omark[k][node] = true;			
			
			// avoids sources with zero assigned supply when possible
			if (commods[k].volume >= commods[k].numbOrig)		
				temp = rnd.getRandom(1, (int)(commods[k].volume / commods[k].numbOrig));
			// temp == 0 in next case and volume will remain unassigned until ref:400 below
			else
				temp = rnd.getRandom(0, (int)(commods[k].volume / commods[k].numbOrig) + 1);
			
            commods[k].commodNodes[node].volume = surplus + temp;
            totalSupply += commods[k].commodNodes[node].volume;
            surplus = (int)(commods[k].volume / commods[k].numbOrig) - temp;
        }
		
		do 
			node = rnd.getRandom(numbNodes, numbSatNodes);
		while (!omark[k][node]);
        commods[k].commodNodes[node].volume += surplus;
		
        totalSupply += surplus;		
		unassSupply = commods[k].volume - totalSupply;
		
		// ref:400
		while (unassSupply != 0)
		{		
			do 
				node = rnd.getRandom(numbNodes, numbSatNodes);
			while (!omark[k][node]);
			
			commods[k].commodNodes[node].volume++;		
			unassSupply--;
		}
		
        surplus = 0;

		// note that totalDemand >= 0 and surplus >= 0 always
        for (int s = 0; s < commods[k].numbDest; s++)
        {
            do
                node = rnd.getRandom(numbNodes, numbSatNodes);
            while (dmark[k][node] || omark[k][node]);

            dmark[k][node] = true;
			
			// avoids sinks with zero assigned demand when possible
			if (commods[k].volume >= commods[k].numbDest)		
				temp = rnd.getRandom(1, (int)(commods[k].volume / commods[k].numbDest));
			// temp == 0 in next case and volume will remain unassigned until ref:401 below
			else
				temp = rnd.getRandom(0, (int)(commods[k].volume / commods[k].numbDest) + 1);
			
            commods[k].commodNodes[node].volume = -(surplus + temp);
            totalDemand -= commods[k].commodNodes[node].volume;
            surplus = (int)(commods[k].volume / commods[k].numbDest) - temp;
        }

		do 
			node = rnd.getRandom(numbNodes, numbSatNodes);
		while (!dmark[k][node]);
        commods[k].commodNodes[node].volume -= surplus;
		
        totalDemand += surplus;		
		unassDemand = commods[k].volume - totalDemand;		
		
		// ref:401
		while (unassDemand != 0)
		{		
			do 
				node = rnd.getRandom(numbNodes, numbSatNodes);
			while (!dmark[k][node]);
			
			commods[k].commodNodes[node].volume--;		
			unassDemand--;
		}		
    }
}

// Unused and obsolete
// Balances remaining discrepancy between supply and demand.
void Graph::balanceCommodity(int k, int totalSupply, int totalDemand)
{	
    int n;

    // assign difference to the first source or sink found
    if (totalSupply > totalDemand)
    {
        for (n = 0; n < numbNodes + numbSatNodes && commods[k].commodNodes[n].volume >= 0; n++)
            ;
		n = max(0, n-1);
        commods[k].commodNodes[n].volume -= (totalSupply - totalDemand);
    }
    else if (totalSupply < totalDemand)
    {
        for (n = 0; n < numbNodes + numbSatNodes && commods[k].commodNodes[n].volume <= 0; n++)
            ;
		n = max(0, n-1);
        commods[k].commodNodes[n].volume += (totalDemand - totalSupply);
    }	
}

// If point-to-point primary topology, adjusts all arcs;
// if hubs-and-satellites primary topology, adjusts arcs between hub nodes.
// Adjusts all arc capacities according to factor adjFactCapacs
// (downward if adjFactCapacs >= 1, upward if adjFactCapacs < 1).
void Graph::adjustArcCapacities()
{
    if (data->adjCapacsDown)
    {
        double totalCapac = 0.0;

        for (int a = 0; a < numbArcs; a++)
            totalCapac += arcs[a].capac;
		
		double q = (static_cast<double>(numbArcs) * totalVolume) / totalCapac;

		if (data->adjFactCapacs >= 1)
		{	
			
			for (int a = 0; a < numbArcs; a++)
			{				
				arcs[a].capac = min(totalVolume, static_cast<int>((q / data->adjFactCapacs) * arcs[a].capac) + 1);
				
				for(int k = 0; k < numbCommods; k++)
				{														
					arcs[a].commodsOnArc[k].capac = min(commods[k].volume, static_cast<int>((q / data->adjFactCapacs) * arcs[a].commodsOnArc[k].capac) + 1);
				}		
			}
		}
		else
		{
			string overMess = "Numerical overflow while increasing arc capacities: value of option\n"
								"adjCapacsDown is too small in relation to total volume and existing capacities.";
								
			for (int a = 0; a < numbArcs; a++)
			{
				if (can_convert_to_int((q / data->adjFactCapacs) * arcs[a].capac, "", overMess))				
					arcs[a].capac = static_cast<int>((q / data->adjFactCapacs) * arcs[a].capac) + 1;
				
				for(int k = 0; k < numbCommods; k++)
				{			
					if (can_convert_to_int((q / data->adjFactCapacs) * arcs[a].commodsOnArc[k].capac, "", overMess))			
						arcs[a].commodsOnArc[k].capac = static_cast<int>((q / data->adjFactCapacs) * arcs[a].commodsOnArc[k].capac) + 1;
				}		
			}			
		}				
    }
}

// Only used if hubs-and-satellites primary topology: adjust satellite arcs
// (satellite arcs connect at least one satellite node)
// Adjusts satellite arc capacities according to factor adjSatFactCapacs
// (downward if adjSatFactCapacs >= 1, upward if adjSatFactCapacs < 1).
void Graph::adjustArcCapacities_sat()
{	
    if (data->adjSatCapacsDown)
    {
		double totalCapac = 0.0;

        for (int a = numbArcs; a < numbArcs + numbSatArcs; a++)
            totalCapac += arcs[a].capac;

        double q = (static_cast<double>(numbSatArcs) * totalVolume) / totalCapac;

		if (data->adjSatFactCapacs >= 1)
		{
			for (int a = numbArcs; a < numbArcs + numbSatArcs; a++)
			{							
				arcs[a].capac = min(totalVolume, static_cast<int>((q / data->adjSatFactCapacs) * arcs[a].capac) + 1);				
			
				for(int k = 0; k < numbCommods; k++)
				{									
					arcs[a].commodsOnArc[k].capac = min(commods[k].volume, static_cast<int>((q / data->adjSatFactCapacs) * arcs[a].commodsOnArc[k].capac) + 1);		
				}
			}			
		}	
		else
		{	
			string overMess = "Numerical overflow while increasing arc capacities: value of option\n"
							"adjSatCapacsDown is too small in relation to total volume and existing capacities.";
	
			for (int a = numbArcs; a < numbArcs + numbSatArcs; a++)
			{				
				if (can_convert_to_int((q / data->adjSatFactCapacs) * arcs[a].capac, "", overMess))				
					arcs[a].capac = static_cast<int>((q / data->adjSatFactCapacs) * arcs[a].capac) + 1;
			
				for(int k = 0; k < numbCommods; k++)
				{					
					if (can_convert_to_int((q / data->adjSatFactCapacs) * arcs[a].commodsOnArc[k].capac, "", overMess))
						arcs[a].commodsOnArc[k].capac = static_cast<int>((q / data->adjSatFactCapacs) * arcs[a].commodsOnArc[k].capac) + 1;		
				}
			}			
		}			
    }
}

// If point-to-point primary topology, adjusts all arcs;
// if hubs-and-satellites primary topology, adjusts only arcs between hub nodes.
// Adjusts all fixed costs according to factor adjFactFixedCosts
// (upward if adjFactFixedCosts >= 1, downward if adjFactFixedCosts < 1).
void Graph::adjustFixedCosts()
{	
    if (data->adjFixCostsUp)
    {
        double totalFixedCost = 0.0, totalCommodCost = 0.0;

        for (int a = 0; a < numbArcs; a++)
        {
            totalFixedCost += arcs[a].cost;

            for (int k = 0; k < numbCommods; k++)
                totalCommodCost += arcs[a].commodsOnArc[k].cost;
        }

        totalCommodCost = totalVolume * (totalCommodCost / numbCommods);

        double q = totalFixedCost / totalCommodCost;
		
		string overMess = "Numerical overflow while adjusting relative magnitude of fixed and variable costs:\n"
						"value of option adjFixCostsUp is too large in relation to existing fixed and variable costs.";

        for (int a = 0; a < numbArcs; a++)
		{						
			if (can_convert_to_int((data->adjFactFixCosts / q) * arcs[a].cost, "", overMess))	
				arcs[a].cost = static_cast<int>((data->adjFactFixCosts / q) * arcs[a].cost) + 1;
		}
    }
}

// Only used if hubs-and-satellites primary topology: adjust satellite arcs
// (satellite arcs connect at least one satellite node).
// Adjusts all fixed costs over satellite arcs upward according to factor
// adjSatFactFixedCosts (upward if adjSatFactFixedCosts >= 1, downward if
// adjSatFactFixedCosts < 1).
void Graph::adjustFixedCosts_sat()
{
    if (data->adjSatFixCostsUp)
    {
        double totalFixedCost = 0.0, totalCommodCost = 0.0;

        for (int a = numbArcs; a < numbArcs + numbSatArcs; a++)
        {
            totalFixedCost += arcs[a].cost;

            for (int k = 0; k < numbCommods; k++)
                totalCommodCost += arcs[a].commodsOnArc[k].cost;
        }

        totalCommodCost = totalVolume * (totalCommodCost / numbCommods);

        double q = totalFixedCost / totalCommodCost;		
		
		string overMess = "Numerical overflow while adjusting relative magnitude of fixed and variable costs:\n"
							"value of option adjSatFixCostsUp is too large in relation to existing fixed and variable costs.";	

        for (int a = numbArcs; a < numbArcs + numbSatArcs; a++)
		{
			if (can_convert_to_int((data->adjSatFactFixCosts / q) * arcs[a].cost, "", overMess))
				arcs[a].cost = static_cast<int>((data->adjSatFactFixCosts / q) * arcs[a].cost) + 1;			
		}
    }
}

// Equalizes commodity-specific properties of arcs if requested.
// If point-to-point primary topology: adjust all arcs;
// if hubs-and-satellites primary topology: adjust only hub arcs.
void Graph::equalizeCommodOnArcs()
{
	if (data->equalizeComCaps)
        for (int a = 0; a < numbArcs; a++)
            for (int k = 0; k < numbCommods; k++)
                arcs[a].commodsOnArc[k].capac = min(arcs[a].capac, commods[k].volume);		
			
	if (data->equalizeVarCosts)
        for (int a = 0; a < numbArcs; a++)
            for (int k = 0; k < numbCommods; k++)
                arcs[a].commodsOnArc[k].cost = arcs[a].commodsOnArc[0].cost;
}

// Equalizes commodity-specific properties of satellite arcs if requested.
// Only used if hubs-and-satellites primary topology:
void Graph::equalizeCommodOnArcs_sat()
{
	if (data->equalizeSatComCaps)
        for (int a = numbArcs; a < numbArcs + numbSatArcs; a++)
            for (int k = 0; k < numbCommods; k++)
                arcs[a].commodsOnArc[k].capac = min(arcs[a].capac, commods[k].volume);		
			
	if (data->equalizeSatVarCosts)
        for (int a = numbArcs; a < numbArcs + numbSatArcs; a++)
            for (int k = 0; k < numbCommods; k++)
                arcs[a].commodsOnArc[k].cost = arcs[a].commodsOnArc[0].cost;
}

// Saves generated instance to file.
void Graph::output()
{	
    if (!data->hubSat && data->oneSourceOneSink && data->equalizeComCaps && data->equalizeVarCosts)
        outputDow(data->shortOutFileName + ".dow");
	
	else if (data->hubSat && data->oneSourceOneSink && data->equalizeComCaps && data->equalizeVarCosts && data->equalizeSatComCaps && data->equalizeSatVarCosts)
        outputDow(data->shortOutFileName + ".dow");

    if (data->generLpOutFile)
        outputLP(data->shortOutFileName + ".lp");

    if (data->generMpsOutFile)
        outputMPS(data->shortOutFileName + ".mps");

    if (!data->noStdOutFile)
        outputStd(data->shortOutFileName + ".std");

    if (!data->fromFile && !data->randConnect)
        outputBasicGraph(data->longBasicGraphFileName);
	
    if (data->generTexOutFile)
    {
        if (data->randConnect || data->fromFile || data->hubSat)
        {
			cout << "\nWARNING:\n"
				<< "LaTeX representations produced in connection with\n"
				<< "(i) hubs-and-satellites primary topologies (configuration option hubsAndSats),\n"
				<< "(ii) random secondary topologies (configuration option secTopology 0),\n"
				<< "(iii) topologies that are read from a file (configuration option secTopology 5)\n"
				<< "are treated as generic, unstructured graphs.\n"
				<< "For legibility, these representations are only available when total number of\n"
				<< "nodes is not greater than 12 and number of commodities is not greater than 4." << endl;			
				
            if ((numbNodes + numbSatNodes) <= 12 && numbCommods <= 4)
                outputTexGeneric(data->shortOutFileName + ".tex");
            else
                cout << "\nWARNING:\n"
					<< "LaTeX representations are available under a generic topology\n"
                    << "only when total number of nodes <= 12 and number of commodities <= 4.\n"
					<< "The LaTeX representation will not be generated." << endl;
        }
		// point-to-point primary topology and grid-like secondary topology
        else if (data->gridConnect)
        {
            if (data->lengthX <= 10 && data->lengthY <= 10 && numbCommods <= 15)
            {
				outputTexGrid(data->shortOutFileName + ".tex");
			
				cout << "\nWARNING:\n"
					<< "If the generated graphical output is complex,\n"
					<< "its processing by a LaTeX compilator might require several minutes later on." << endl;			
			}			
            else
                cout << "\nWARNING:\n"
					<< "LaTeX representations are available under a grid secondary topology\n"
                    << "only when lengthX <= 10, lengthY <= 10 and number of commodities <= 15.\n"
					<< "The LaTeX representation will not be generated." << endl;
        }
		// point-to-point primary topology and circular secondary topology
        else if (data->circConnect)
        {
            if (data->numbNodes <= 36 && numbCommods <= 15)
			{
                outputTexCirc(data->shortOutFileName + ".tex");
				
				cout << "\nWARNING:\n"
					<< "If the generated graphical output is complex,\n"
					<< "its processing by a LaTeX compilator might require several minutes later on." << endl;
			}	
            else
                cout << "\nWARNING:\n"
					<< "LaTeX representations are available under a circular secondary topology\n"
                    << "only when number of nodes <= 36 and number of commodities <= 15.\n"
					<< "The LaTeX representation will not be generated." << endl;
        }			
    }
}

// Reads basic graph (only nodes + arcs) from file without costs or capacities.
void Graph::readBasicGraph(string longBasicGraphFileName, const VectCommodOnArc &commodsOnArc)
{
    if (!filesystem::is_regular_file(longBasicGraphFileName))
    {
        cerr << "\nERROR:\n"
			<< "The basic graph input file " << longBasicGraphFileName << " could not be found.\n"
			<< "EXECUTION ABORTED" << endl;
        exit(1);
    }

    ifstream inFile(longBasicGraphFileName);

    if (!inFile.is_open())
    {
        cerr << "\nERROR:\n"
			<< "The basic graph input file " << longBasicGraphFileName << " could not be opened.\n"
			<< "EXECUTION ABORTED" << endl;
        exit(1);
    }

    char line[1024];
    string dummy;
    int low, high; // low and high values for nodes
    int orig, dest;
	string origStr = "", destStr = "", lowStr = "", highStr = "";

    inFile.getline(line, 1024);
    inFile.getline(line, 1024);
	
	inFile.getline(line, 1024);
	istringstream iss0(line);		
    iss0 >> dummy >> dummy >> lowStr;
	
	inFile.getline(line, 1024);
	istringstream iss1(line);	
    iss1 >> dummy >> dummy >> highStr;
	
	if (isDigitsOnly(lowStr) && isDigitsOnly(highStr))
	{
		low = stoi(lowStr);
		high = stoi(highStr);		
	}
	else
	{			
		cerr << "\nERROR:\n"
			<< "While reading " + longBasicGraphFileName + ", the tokens\n"
			<< "\"" << lowStr << "\" and \"" << highStr << "\" could not be interpreted\n"
			<< "as the low and high node numbers.\n"
			<< "EXECUTION ABORTED" << endl;
        exit(1);
	}

    inFile.getline(line, 1024);
    inFile.getline(line, 1024);
    inFile.getline(line, 1024);

    bool foundError = false;
	bool readArc = false;

	while(inFile.getline(line, 1024))
    {		
		origStr = "";
		destStr = "";

		istringstream iss3(line);		
		iss3 >> origStr >> destStr;	

		if (isDigitsOnly(origStr) && isDigitsOnly(destStr))
		{
			orig = stoi(origStr);
			dest = stoi(destStr);
			readArc = true;			
		}
		else
		{			
			cerr << "\nERROR:\n"
				<< "While reading arcs in " + longBasicGraphFileName + ", the tokens\n"
				<< "\"" << origStr << "\" and \"" << destStr << "\" could not be interpreted as node numbers." << endl;

            foundError = true;
			break;			
		}		
        if (orig < low)
        {
            cerr << "\nERROR:\n"
				<< "While reading " + longBasicGraphFileName + ", origin node number\n"
				<< orig << " is lower than specified low " << low << endl;

            foundError = true;
			break;
        }
        else if (dest < low)
        {
            cerr << "\nERROR:\n"
				<< "While reading " + longBasicGraphFileName + ", destination node number\n"
				<< dest << " is lower than specified low " << low << endl;

            foundError = true;
			break;
        }
        else if (orig > high)
        {
            cerr << "\nERROR:\n"
				<< "While reading " + longBasicGraphFileName + ", origin node number\n"
				<< orig << " is higher than specified high " << high << endl;

            foundError = true;
			break;
        }
        else if (dest > high)
        {
            cerr << "\nERROR:\n"
				<< "While reading " + longBasicGraphFileName + ", destination node number\n"
				<< dest << " is higher than specified high " << high << endl;

            foundError = true;
			break;
        }

        orig -= low;
        dest -= low;

        addNonRandomArc(Arc(orig, dest, data->maxFixedCost, totalVolume, commodsOnArc));

        //inFile >> orig >> dest;
    }

    inFile.close();  

	if (!readArc)
	{
		cerr << "\nERROR:\n"
			<< "While reading " + longBasicGraphFileName + ", no valid arc could be found.\n"
			<< "EXECUTION ABORTED" << endl;
        exit(1);		
	}	
	
	if (foundError)
    {
		cerr << "EXECUTION ABORTED" << endl;
        exit(1);
    }	

	int temp = high - low + 1;
	
	if (temp > numbNodes)
    {
        cerr << "\nERROR:\n"
			<< "While reading " + longBasicGraphFileName + ", " << temp << " nodes have been detected\n"
            << "whereas the value specified for option numbNodes is " << numbNodes << ".\n"
			<< "The value of numbNodes should be corrected to be at least as large\n"
			<< "as the number of detected nodes.\n"
			<< "EXECUTION ABORTED" << endl;
            exit(1);
    }
	
    numbNodes = temp;	
}

// Saves basic graph (only nodes + non-random arcs) to file without costs or
// capacities.
void Graph::outputBasicGraph(string longBasicGraphFileName)
{
    ofstream outFile(longBasicGraphFileName);	
	checkIfValidOutputFile(outFile, longBasicGraphFileName);	

    outFile << "1 Nodes" << endl;
    outFile << "Nodes are identified with integers in closed interval [low, "
               "high]. Program will perform appropriate renumbering."
            << endl;
    outFile << "low = " << 1 << endl;
    outFile << "high = " << numbNodes << endl << endl;

    outFile << "2 Arcs" << endl;
    outFile << "NodeFrom  NodeTo" << endl;

    for (int a = 0; a < numbNonRandomArcs; a++)
	{
        outFile << arcs[a].orig + 1 << "  " << arcs[a].dest + 1 << endl;
	}
	
	for (int a1 = numbArcs; a1 < numbArcs + numbSatNonRandomArcs; a1++)
	{
        outFile << arcs[a1].orig + 1 << "  " << arcs[a1].dest + 1 << endl;
	}

    outFile.close();
}

// Saves generated instance to file with DOW format.
// DOW format imposes additional constraints: for
// each arc, capacities and variable costs are identical
// over all commodities.
void Graph::outputDow(string fileName)
{
    ofstream outFile(fileName);
	checkIfValidOutputFile(outFile, fileName);
	
    int orig = 0, dest = 0, volume = 0;

    outFile << "MULTIGEN.DAT:" << endl;
    outFile << setw(8) << right << (numbNodes + numbSatNodes) << "    " << setw(8) << right << (numbArcs + numbSatArcs) << "    " << setw(8) << right
            << numbCommods << endl;
    
	for (int a = 0; a < numbArcs + numbSatArcs; a++)
	{            
		outFile << setw(8) << right << arcs[a].orig + 1 << "    " << setw(8) << right << arcs[a].dest + 1
				<< "    " << setw(8) << right << arcs[a].commodsOnArc[0].cost << "    " << setw(8) << right
				<< arcs[a].capac << "    " << setw(8) << right << arcs[a].cost << "    " << setw(8) << right
				<< 1 << "    " << setw(8) << right << a + 1 << endl;
	}

    for (int k = 0; k < numbCommods; k++)
    {
        for (int n = 0; n < numbNodes + numbSatNodes; n++)
        {
            if (commods[k].commodNodes[n].volume > 0)
            {
                orig = n;
                volume = commods[k].commodNodes[n].volume;
            }
            else if (commods[k].commodNodes[n].volume < 0)
                dest = n;
        }

        outFile << setw(8) << right << orig + 1 << "    " << setw(8) << right << dest + 1 << "    " << setw(8) << right
                << volume << endl;
    }

    outFile.close();
}

// Saves generated instance to file with STD format.
void Graph::outputStd(string fileName)
{	
    ofstream outFile(fileName);	
	checkIfValidOutputFile(outFile, fileName);
    outFile << setw(8) << right << (numbNodes + numbSatNodes) << "    " << setw(8) << right << (numbArcs + numbSatArcs) << "    " << setw(8) << right
            << numbCommods << endl;
    for (int n = 0; n < numbNodes + numbSatNodes; n++)
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            if (arcs[a].orig == n)
            {
                int cpt = 0;

                for (int k = 0; k < numbCommods; k++)
                    if (arcs[a].commodsOnArc[k].capac != 0)
                        cpt++;

                outFile << setw(8) << right << arcs[a].orig + 1 << "    " << setw(8) << right << arcs[a].dest + 1
                        << "    " << setw(8) << right << arcs[a].cost << "    " << setw(8) << right << arcs[a].capac
                        << "    " << setw(8) << right << cpt << endl;

                for (int k = 0; k < numbCommods; k++)
                    if (arcs[a].commodsOnArc[k].capac != 0)
                        outFile << setw(8) << right << k + 1 << "    " << setw(8) << right
                                << arcs[a].commodsOnArc[k].cost << "    " << setw(8) << right
                                << arcs[a].commodsOnArc[k].capac << endl;
            }
        }
    for (int k = 0; k < numbCommods; k++)
        for (int n = 0; n < numbNodes + numbSatNodes; n++)
            if (commods[k].commodNodes[n].volume != 0)
                outFile << setw(8) << right << k + 1 << "    " << setw(8) << right << n + 1 << "    " << setw(8)
                        << right << commods[k].commodNodes[n].volume << endl;

    outFile.close();	
}

// Saves generated instance to file with (human readable) LP format.
void Graph::outputLP(string fileName)
{
    ofstream outFile(fileName);
	checkIfValidOutputFile(outFile, fileName);

    outFile << "\\Problem name: " << fileName << endl << endl;

    string prefix = "Obj: ";
    int coef;
    string var;

    // objective function

    outFile << "Minimize" << endl;

    LPStringBuilder obj(prefix);

    for (int a = 0; a < numbArcs + numbSatArcs; a++)
    {
        for (int k = 0; k < numbCommods; k++)
        {			
            coef = arcs[a].commodsOnArc[k].cost;			
            
            obj.addVar(coef, "X", a, k);
        }
    }

    if (data->integCapacs || data->integCommCapacs)
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            obj.addVar(arcs[a].cost, "Y", a);
        }
    }

    outFile << obj << endl;

    outFile << "Subject To" << endl;

    // flow conservation constraints
    for (int n = 0; n < numbNodes + numbSatNodes; n++)
    {
        for (int k = 0; k < numbCommods; k++)
        {
            prefix = "N_" + to_string(n) + "_" + to_string(k) + ": ";

            LPStringBuilder builder(prefix, "=", commods[k].commodNodes[n].volume);

            for (int a = 0; a < numbArcs + numbSatArcs; a++)
            {
                if (arcs[a].orig == n)
                    builder.addVar(1, "X", a, k);
                else if (arcs[a].dest == n)
                    builder.addVar(-1, "X", a, k);
            }

            outFile << builder << endl;
        }
    }

    // arc capacities
    if (numbCommods > 1 || (numbCommods == 1 && (data->integCapacs || data->integCommCapacs)))
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            prefix = "A_" + to_string(a) + ": ";

            LPStringBuilder builder(prefix);

            for (int k = 0; k < numbCommods; k++)
                builder.addVar(1, "X", a, k);

            if ((data->integCapacs || (numbCommods == 1 && data->integCommCapacs)) && arcs[a].capac != 0)
            {
                builder.addVar(-arcs[a].capac, "Y", a);
                builder.addRhs("<=", 0);
            }
            else if (!data->integCapacs && numbCommods > 1)
                builder.addRhs("<=", arcs[a].capac);

            outFile << builder << endl;
        }
    }

    // commodity-specific capacities on arcs
    if (numbCommods > 1 && data->integCommCapacs)
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            for (int k = 0; k < numbCommods; k++)
            {
                prefix = "K_" + to_string(a) + "_" + to_string(k) + ": ";

                LPStringBuilder builder(prefix, "<=", 0);

                builder.addVar(1, "X", a, k);
                builder.addVar(-arcs[a].commodsOnArc[k].capac, "Y", a);

                outFile << builder << endl;
            }
        }
    }

    // Bounds
    outFile << "Bounds" << endl;
	
    if (numbCommods == 1 && (!(data->integCapacs || data->integCommCapacs)))
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            string name = "X_" + to_string(a) + "_0";

            outFile << "0 <= " << name << " <= " << arcs[a].capac << endl;
        }
    }

	if (!data->integCommCapacs && numbCommods > 1)
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            for (int k = 0; k < numbCommods; k++)
            {
                string name = "X_" + to_string(a) + "_" + to_string(k);

                outFile << "0 <= " << name << " <= " << arcs[a].commodsOnArc[k].capac << endl;
            }
        }
    }

    if (data->integCapacs || data->integCommCapacs)
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            string name = "Y_" + to_string(a);

            outFile << "0 <= " << name << " <= " << 1 << endl;
        }

        // Binary variables
        outFile << "Binaries" << endl;

        LPStringBuilder bounds;

        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            string name = "Y_" + to_string(a) + " ";

            bounds.addVar(name);
        }

        outFile << bounds << endl;
    }

    outFile << "End" << endl;
    outFile.close();
}

// Saves generated instance to file with MPS format.
void Graph::outputMPS(string fileName)
{
    ofstream outFile(fileName);
	checkIfValidOutputFile(outFile, fileName);
	
    int coef;
    string varID, ctrID;
	
	outFile << "NAME  " << fileName << endl;
	
    outFile << "ROWS  " << endl;

    outFile << " N  Obj" << endl;

    for (int n = 0; n < numbNodes + numbSatNodes; n++)
        for (int k = 0; k < numbCommods; k++)
            outFile << " E  N_" << to_string(n) << "_" << to_string(k) << endl;

    if (numbCommods > 1 || (numbCommods == 1 && (data->integCapacs || data->integCommCapacs)))
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
            outFile << " L  A_" << to_string(a) << endl;

    if (numbCommods > 1 && data->integCommCapacs)
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
            for (int k = 0; k < numbCommods; k++)
                outFile << " L  K_" << to_string(a) << "_" << to_string(k) << endl;

    outFile << "COLUMNS" << endl;

    // X variables
    for (int a = 0; a < numbArcs + numbSatArcs; a++)
        for (int k = 0; k < numbCommods; k++)
        {
            varID = "    X_" + to_string(a) + "_" + to_string(k);

            // objective coefficients

            ctrID = "Obj";
            coef = arcs[a].commodsOnArc[k].cost;
            outFile << setw(16) << left << varID << setw(15) << ctrID << setw(10) << coef << endl;

            // flow conservation constraints coefficients
            for (int n = 0; n < numbNodes + numbSatNodes; n++)
            {
                coef = (arcs[a].orig == n) ? 1 : ((arcs[a].dest == n) ? -1 : 0);

                if (coef != 0)
                {
                    ctrID = "N_" + to_string(n) + "_" + to_string(k);
                    outFile << setw(16) << left << varID << setw(15) << ctrID << setw(10) << coef << endl;
                }
            }

            // arc capacities constraints coefficients
            if (numbCommods > 1 || (numbCommods == 1 && (data->integCapacs || data->integCommCapacs)))
            {
                ctrID = "A_" + to_string(a);
                coef = 1;
                outFile << setw(16) << left << varID << setw(15) << ctrID << setw(10) << coef << endl;
            }

            // commodity-specific capacities on arcs coefficients
            if (numbCommods > 1 && data->integCommCapacs)
            {
                ctrID = "K_" + to_string(a) + "_" + to_string(k);
                coef = 1;
                outFile << setw(16) << left << varID << setw(15) << ctrID << setw(10) << coef << endl;
            }
        }

    // Y variables
    outFile << "    MARK0000  'MARKER'                 'INTORG'" << endl;

    bool test =
        (numbCommods > 1 && data->integCapacs) || (numbCommods == 1 && (data->integCapacs || data->integCommCapacs));

    for (int a = 0; a < numbArcs + numbSatArcs; a++)
    {
        varID = "    Y_" + to_string(a);

        // objective coefficient
        ctrID = "Obj";

        if (data->integCapacs || data->integCommCapacs)
        {
            coef = arcs[a].cost;
            outFile << setw(16) << left << varID << setw(15) << ctrID << setw(10) << coef << endl;
        }

        // arc capacities constraints coefficients
        if (test && arcs[a].capac != 0)
        {
            ctrID = "A_" + to_string(a);
            coef = -arcs[a].capac;
            outFile << setw(16) << left << varID << setw(15) << ctrID << setw(10) << coef << endl;
        }

        // commodity-specific capacities on arcs coefficients
        if (numbCommods > 1 && data->integCommCapacs)
        {
            for (int k = 0; k < numbCommods; k++)
            {
                ctrID = "K_" + to_string(a) + "_" + to_string(k);
                coef = -arcs[a].commodsOnArc[k].capac;
                outFile << setw(16) << left << varID << setw(15) << ctrID << setw(10) << coef << endl;
            }
        }
    }

    outFile << "    MARK0000  'MARKER'                 'INTEND'" << endl;

    // RHS

    outFile << "RHS" << endl;
    varID = "    rhs";

    // flow conservation constraints
    for (int n = 0; n < numbNodes + numbSatNodes; n++)
    {
        for (int k = 0; k < numbCommods; k++)
        {
            if (commods[k].commodNodes[n].volume != 0)
            {
                ctrID = "N_" + to_string(n) + "_" + to_string(k);
                coef = commods[k].commodNodes[n].volume;
                outFile << setw(16) << left << varID << setw(15) << ctrID << setw(10) << coef << endl;
            }
        }
    }

    // arc capacities
    if (numbCommods > 1 && !data->integCapacs)
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            ctrID = "A_" + to_string(a);
            coef = arcs[a].capac;
            outFile << setw(16) << left << varID << setw(15) << ctrID << setw(10) << coef << endl;
        }
    }

    // BOUNDS
    outFile << "BOUNDS" << endl;

    if (numbCommods == 1 && !(data->integCapacs || data->integCommCapacs))
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            varID = "X_" + to_string(a) + "_0";
            outFile << setw(16) << left << " UP BOUND" << setw(15) << varID << setw(10) << arcs[a].capac << endl;
        }
    }

    if (numbCommods > 1 && !data->integCommCapacs)
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            for (int k = 0; k < numbCommods; k++)
            {
                varID = "X_" + to_string(a) + "_" + to_string(k);
                outFile << setw(16) << left << " UP BOUND" << setw(15) << varID << setw(10)
                        << arcs[a].commodsOnArc[k].capac << endl;
            }
        }
    }

    if (data->integCapacs || data->integCommCapacs)
    {
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            varID = "Y_" + to_string(a);
            outFile << setw(16) << left << " BV BOUND" << setw(15) << varID << endl;
        }
    }

    outFile << "ENDATA" << endl;
}

// Outputs the code for depicting the graph of the generated instance with
// a LaTeX compiler, either (i) when primary topology is point-to-point 
// as well as purely random or retrieved from a basic graph file (i.e.,
// results from hubSat == 0 and secondary topology == 0 or 5) or (ii) when primary topology is
// hubs-and-satellites (hubSat == 1).
void Graph::outputTexGeneric(string fileName)
{

	ofstream outFile(fileName);	
	checkIfValidOutputFile(outFile, fileName);

    // progressively loaded with the LaTeX code;
    // once finished, its content is saved to file
    stringstream outStr{};

    // store the flow data related to the nodes
    VectStr strNodes(numbNodes + numbSatNodes);
    for (int n = 0; n < numbNodes + numbSatNodes; n++)
    {
        strNodes[n] = "";
    }

    // update the information for the source or sink nodes
    float eps = 0.000001;
    for (int n = 0; n < numbNodes + numbSatNodes; n++)
    {
        for (int k = 0; k < numbCommods; k++)
        {
            if (commods[k].commodNodes[n].volume > eps || commods[k].commodNodes[n].volume < -eps)
            {
                strNodes[n] += (to_string(k + 1) + " & " + to_string(commods[k].commodNodes[n].volume) + " \\\\\n");
            }
        }
    }

    // set the shape of the matrix in which the nodes are placed
    int calcLengthX = (int)sqrt(1.0 * (numbNodes + numbSatNodes));
    int inFullRows = ((int)(1.0 * (numbNodes + numbSatNodes) / calcLengthX)) * calcLengthX;
    int remain = numbNodes + numbSatNodes - inFullRows;

    // set the sizes and spacing of the nodes
    string spacing = "", nodeSize = "", nodeFont = "";
    if (data->oneSourceOneSink)
    {
        nodeSize = "10mm";
        nodeFont = "";
        if (numbNodes + numbSatNodes <= 64)
            spacing = "25mm";
        else
            spacing = "10mm";
    }
    else
    {
        nodeFont = "\\tiny";
        if (numbNodes + numbSatNodes <= 64 && numbCommods <= 10)
        {
            nodeSize = "10mm";
            spacing = "25mm";
        }
        else if (numbNodes + numbSatNodes <= 64 && numbCommods <= 15)
        {
            nodeSize = "5mm";
            spacing = "15mm";
        }
        else if (numbCommods <= 12)
        {
            nodeSize = "5mm";
            spacing = "10mm";
            cout << "\nWARNING: Compilation of LaTeX output might fail"
                    " when numbers of nodes and commodities are high."
                 << endl;
        }
        else
        {
            cout << "\nWARNING:\n"
				<< "LaTeX representations are unavailable with generic topology and !oneSrcOneSnk,\n"
				<< "if number of nodes > 64 and number of commodities > 12.\n"
				<< "The LaTeX representation will not be generated." << endl;
            return;
        }
    }

    outStr << "\\documentclass{article}\n\\usepackage{longtable}\n"
              "\\usepackage{rotating}\n\\usepackage{adjustbox}\n"
              "\\usepackage{geometry}\n\\geometry{paperwidth=14in,paperheight="
              "14in,lmargin=1in,"
              "rmargin=1in,bmargin=1in,tmargin=1in}\n\\usepackage{tikz}\n"
              "\\usepackage{multirow}\n\\begin{document}\n\\usetikzlibrary"
              "{shapes.misc, arrows.meta, quotes, decorations.text}\n"
              "\\section{Generated instance with unspecified topology}\n";

    outStr << "\\begin{center}\n\\adjustbox{max size={11.5in}{11.5in}}{\n";

    // begin the generation of graph
    outStr << "\\begin{tikzpicture}[inout/.style={rectangle, minimum size=" + nodeSize + ",very thick,font={" +
                  nodeFont +
                  "\\sffamily}, blue},through/.style={circle,"
                  "minimum size=" +
                  nodeSize +
                  ",very thick,font={\\sffamily}},post/.style={->,shorten <=1pt,"
                  "shorten >=1pt,>={Stealth[round]},thick},randpost/.style={->,"
                  "shorten <=1pt,shorten >=1pt,>={Stealth[round]},thick,red}]\n";

    // placing the nodes in a matrix
    outStr << "\\matrix[row sep=" + spacing + ",column sep=" + spacing +
                  ",nodes=draw,ampersand "
                  "replacement=\\&]{\n";

    string temp = "";
    int counter0 = 0;
    for (int n0 = 0; n0 < inFullRows; n0++)
    {
        // cout << "n: " << to_string(n) << endl;
        counter0++;
        temp = strNodes[n0];

        if (counter0 != 1)
        {
            outStr << "\\& ";
        }
        if (temp == "")
        {
            outStr << "\\node [through] (" + to_string(n0 + 1) + ") {node " + to_string(n0 + 1) + "};";
        }
        else
        {
            outStr << "\\node [inout, align=center] (" + to_string(n0 + 1) +
                          "){\\begin{tabular}{l|r}\\multicolumn{2}{l}{node " + to_string(n0 + 1) +
                          "}\\\\\n comm. & flow \\\\\n\\hline\n" + strNodes[n0] + "\\end{tabular}};";
        }
        if (counter0 == calcLengthX)
        {
            outStr << "\\\\";
            counter0 = 0;
        }

        outStr << "\n";
    }

    int counter1 = 0;
    for (int n1 = inFullRows; n1 < numbNodes + numbSatNodes; n1++)
    {
        // cout << "n: " << to_string(n) << endl;
        counter1++;
        temp = strNodes[n1];

        if (counter1 != 1)
        {
            outStr << "\\& ";
        }
        if (temp == "")
        {
            outStr << "\\node [through] (" + to_string(n1 + 1) + ") {node " + to_string(n1 + 1) + "};";
        }
        else
        {
            outStr << "\\node [inout, align=center] (" + to_string(n1 + 1) +
                          "){\\begin{tabular}{l|r}\\multicolumn{2}{l}{node " + to_string(n1 + 1) +
                          "}\\\\\n comm. & flow \\\\\n\\hline\n" + strNodes[n1] + "\\end{tabular}};";
        }
    }

    for (int i = 0; i < calcLengthX - remain; i++)

        outStr << "\\& ";

    outStr << "\\\\\n";

    outStr << "};\n";

    // handle two cases
    if (data->oneSourceOneSink)
    {		
		// when primary topology is point-to-point: add arcs connecting nodes
		// when primary topology is hub-and-satallite: add arcs connecting hub nodes
        for (int a0 = 0; a0 < numbNonRandomArcs; a0++)
        {
            // add all arcs
            outStr << "\\path[post, bend left=20] (" + to_string(arcs[a0].orig + 1) +
                          ") edge [postaction={decorate,decoration={raise=1ex,"
                          "text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a0].capac) + " F=" + to_string(arcs[a0].cost) +
                          " V=" + to_string(arcs[a0].commodsOnArc[0].cost) + "}}}] (" + to_string(arcs[a0].dest + 1) +
                          ");\n";
        }
		
        for (int a1 = numbNonRandomArcs; a1 < numbArcs; a1++)
        {
            outStr << "\\path[randpost, bend left=20] (" + to_string(arcs[a1].orig + 1) +
                          ") edge [postaction={decorate,decoration={text "
                          "color=red,"
                          "raise=1ex,text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a1].capac) + " F=" + to_string(arcs[a1].cost) +
                          " V=" + to_string(arcs[a1].commodsOnArc[0].cost) + "}}}] (" + to_string(arcs[a1].dest + 1) +
                          ");\n";
        }	

		// when primary topology is hubs-and-satellites: add arcs connecting either satellite node and hub node or two satellite nodes
		for (int a2 = numbArcs; a2 < numbArcs + numbSatNonRandomArcs; a2++)
        {
            // add all arcs
            outStr << "\\path[post, bend left=20] (" + to_string(arcs[a2].orig + 1) +
                          ") edge [postaction={decorate,decoration={raise=1ex,"
                          "text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a2].capac) + " F=" + to_string(arcs[a2].cost) +
                          " V=" + to_string(arcs[a2].commodsOnArc[0].cost) + "}}}] (" + to_string(arcs[a2].dest + 1) +
                          ");\n";
        }
		
        for (int a3 = numbArcs + numbSatNonRandomArcs; a3 < numbArcs + numbSatArcs; a3++)
        {
            outStr << "\\path[randpost, bend left=20] (" + to_string(arcs[a3].orig + 1) +
                          ") edge [postaction={decorate,decoration={text "
                          "color=red,"
                          "raise=1ex,text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a3].capac) + " F=" + to_string(arcs[a3].cost) +
                          " V=" + to_string(arcs[a3].commodsOnArc[0].cost) + "}}}] (" + to_string(arcs[a3].dest + 1) +
                          ");\n";
        }
		
        // end generation of graph
        outStr << "\\end{tikzpicture}\n";

        outStr << "}\n\\end{center}\n";

        // set up notes
        outStr << "\\pagebreak\n\\section{Notes}\n";

        outStr << "\\begin{itemize}\n"
                  "\\item{Black arcs, if present, have been read from basic graph "
                  "file.}\n"
                  "\\item{Red arcs are additional, randomly generated.}\n"
                  "\\item{$C=x$ along arc: overall capacity of arc is equal to "
                  "$x$.}\n"
                  "\\item{$F=y$ along arc: fixed cost of arc is equal to $y$.}\n"
                  "\\item{$V=z$ along arc: variable cost of arc is equal to "
                  "$z$.}\n"
                  "\\item{Capacities and variable costs are not differentiated "
                  "between commodities.}\n"
                  "\\item{Circular nodes are transit nodes.}\n"
                  "\\item{Rectangular nodes are source ($flow > 0$) or sink "
                  "($flow < 0$) nodes for stated commodities.}\n"
                  "\\end{itemize}\n";
    }
    else
    {
        // when primary topology is point-to-point: add arcs connecting nodes
		// when primary topology is hub-and-satallite: add arcs connecting hub nodes
        for (int a4 = 0; a4 < numbNonRandomArcs; a4++)
        {
            outStr << "\\path[post, bend left=20] (" + to_string(arcs[a4].orig + 1) +
                          ") edge [postaction={decorate,decoration={raise=1ex,"
                          "text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a4].capac) + " F=" + to_string(arcs[a4].cost) + "}}}] (" +
                          to_string(arcs[a4].dest + 1) + ");\n";
        }

        for (int a5 = numbNonRandomArcs; a5 < numbArcs; a5++)
        {
            outStr << "\\path[randpost, bend left=20] (" + to_string(arcs[a5].orig + 1) +
                          ") edge [postaction={decorate,decoration={text "
                          "color=red,"
                          "raise=1ex,text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a5].capac) + " F=" + to_string(arcs[a5].cost) + "}}}] (" +
                          to_string(arcs[a5].dest + 1) + ");\n";
        }		
		
		// when primary topology is hubs-and-satellites: add arcs connecting either satellite node and hub node or two satellite nodes
		for (int a6 = numbArcs; a6 < numbArcs + numbSatNonRandomArcs; a6++)
        {
            outStr << "\\path[post, bend left=20] (" + to_string(arcs[a6].orig + 1) +
                          ") edge [postaction={decorate,decoration={raise=1ex,"
                          "text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a6].capac) + " F=" + to_string(arcs[a6].cost) + "}}}] (" +
                          to_string(arcs[a6].dest + 1) + ");\n";
        }

        for (int a7 = numbArcs + numbSatNonRandomArcs; a7 < numbArcs + numbSatArcs; a7++)
        {
            outStr << "\\path[randpost, bend left=20] (" + to_string(arcs[a7].orig + 1) +
                          ") edge [postaction={decorate,decoration={text "
                          "color=red,"
                          "raise=1ex,text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a7].capac) + " F=" + to_string(arcs[a7].cost) + "}}}] (" +
                          to_string(arcs[a7].dest + 1) + ");\n";
        }		

        // end generation of graph
        outStr << "\\end{tikzpicture}\n";

        outStr << "}\n\\end{center}\n";

        // set up notes
        outStr << "\\pagebreak\n\\section{Notes}\n";

        outStr << "\\begin{itemize}\n"
                  "\\item{Black arcs, if present, have been read from basic graph "
                  "file.}\n"
                  "\\item{Red arcs are additional, randomly generated.}\n"
                  "\\item{$C=x$ along arc: overall capacity of arc is equal to "
                  "$x$.}\n"
                  "\\item{$F=y$ along arc: fixed cost of arc is equal to $y$.}\n"
                  "\\item{See below for capacities and variable costs per arc, "
                  "per commodity.}\n"
                  "\\item{Circular nodes are transit nodes.}\n"
                  "\\item{Rectangular nodes are source ($flow > 0$) or sink "
                  "($flow < 0$) nodes for stated commodities.}\n"
                  "\\end{itemize}\n";

        outStr << "\\pagebreak\n";

        // set up table of capacities and variable costs per arc, per commodity
        outStr << "\\section{Capacities and variable costs per arc, per commodity}"
                  "\n\\small\n\\begin{longtable}"
                  "{||l||c|c|c||c|c|c||c|c|c||}\n\\hline\n{Arc}"
                  " & {Comm.} & {Cap.} & {Var. cost} & {Comm.} & {Cap.} & "
                  "{Var. cost} & {Comm.} & {Cap.} & {Var. cost}"
                  "\\\\\n\\hline\n\\endhead\n";

        // https://www.geeksforgeeks.org/how-to-create-vector-of-pairs-in-cpp/

        vector<pair<int, int>> v;
        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            v.push_back(make_pair(arcs[a].orig, arcs[a].dest));
        }

        // https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
        // https://stackoverflow.com/questions/36622704/how-to-sort-elements-of-pairs-inside-vector

        // sort all arcs in lexicographic order
        vector<size_t> idx(v.size());
        iota(idx.begin(), idx.end(), 0);
        sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {
            return (v[i1].first != v[i2].first) ? v[i1].first < v[i2].first : v[i1].second < v[i2].second;
        });

        // https://stackoverflow.com/questions/9751932/displaying-contents-of-a-vector-container-in-c
        // std::copy(idx.begin(), idx.end(),
        // std::ostream_iterator<int>(std::cout, " "));

        int numbFullRows = numbCommods / 3;
        int lengthLast = numbCommods - 3 * numbFullRows;
        int comm = 0;

        for (int a = 0; a < numbArcs + numbSatArcs; a++)
        {
            outStr << "(" + to_string(arcs[idx[a]].orig + 1) + ", " + to_string(arcs[idx[a]].dest + 1) + ")";

            if (numbCommods < 3)
            {
                for (int i0 = 0; i0 < lengthLast; i0++)
                    outStr << " & " + to_string(i0 + 1) + " & " + to_string(arcs[idx[a]].commodsOnArc[i0].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[i0].cost);

                for (int i1 = 0; i1 < 3 - lengthLast; i1++)
                    outStr << " & & &";
            }
            else if (numbCommods == 3)
            {
                for (int i7 = 0; i7 < 3; i7++)

                    outStr << " & " + to_string(i7 + 1) + " & " + to_string(arcs[idx[a]].commodsOnArc[i7].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[i7].cost);
            }
            else
            {
                for (int i2 = 0; i2 < 3; i2++)

                    outStr << " & " + to_string(i2 + 1) + " & " + to_string(arcs[idx[a]].commodsOnArc[i2].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[i2].cost);

                outStr << " \\\\\n";

                comm = 4;
                for (int i3 = 0; i3 < numbFullRows - 1; i3++)
                {
                    for (int i4 = 0; i4 < 3; i4++)
                    {
                        outStr << " & " + to_string(comm) + " & " +
                                      to_string(arcs[idx[a]].commodsOnArc[comm - 1].capac) + " & " +
                                      to_string(arcs[idx[a]].commodsOnArc[comm - 1].cost);
                        comm++;
                    }
                    outStr << " \\\\\n";
                }
                for (int i5 = 0; i5 < lengthLast; i5++)
                {
                    outStr << " & " + to_string(comm) + " & " + to_string(arcs[idx[a]].commodsOnArc[comm - 1].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[comm - 1].cost);
                    comm++;
                }
                for (int i6 = 0; i6 < 3 - lengthLast; i6++)
                    outStr << " & & &";
            }

            outStr << " \\\\\n\\hline\n";
        }

        outStr << "\\end{longtable}\n";
    }

    outStr << "\\end{document}";    

    outFile << outStr.str();

    outFile.close();
}

// Outputs the code for depicting the graph of
// the generated instance with a LaTeX compiler,
// when primary topology is point-to-point and secondary topology is grid-like (i.e., results
// from secTopology == 1, 2 or 3).
void Graph::outputTexGrid(string fileName)
{
	ofstream outFile(fileName);
	checkIfValidOutputFile(outFile, fileName);
	
    // progressively loaded with the LaTeX code;
    // once finished, its content is saved to file
    stringstream outStr{};

    // store the flow data related to the nodes
    VectStr strNodes(numbNodes);
    for (int n = 0; n < numbNodes; n++)
    {
        strNodes[n] = "";
    }

    // update the information about the source and sink nodes
    float eps = 0.000001;
    for (int n = 0; n < numbNodes; n++)
    {
        for (int k = 0; k < numbCommods; k++)
        {
            if (commods[k].commodNodes[n].volume > eps || commods[k].commodNodes[n].volume < -eps)
            {
                strNodes[n] += (to_string(k + 1) + " & " + to_string(commods[k].commodNodes[n].volume) + " \\\\\n");
            }
        }
    }

    // set the sizes and spacing of the nodes
    string spacing = "", nodeSize = "", nodeFont = "";
    if (data->oneSourceOneSink)
    {
        nodeSize = "10mm";
        nodeFont = "";
        if (data->lengthY <= 8 && data->lengthX <= 8)
            spacing = "25mm";
        else
            spacing = "10mm";
    }
    else
    {
        nodeFont = "\\tiny";
        if (data->lengthY <= 8 && data->lengthX <= 8 && numbCommods <= 10)
        {
            nodeSize = "10mm";
            spacing = "25mm";
        }
        else if (data->lengthY <= 8 && data->lengthX <= 8 && numbCommods <= 15)
        {
            nodeSize = "5mm";
            spacing = "15mm";
        }
        else if (numbCommods <= 12)
        {
            nodeSize = "5mm";
            spacing = "10mm";
            cout << "\nWARNING:\n"
				<< "Compilation of LaTeX output might fail when numbNodes and numbCom are high." << endl;
        }
        else
        {
            cout << "\nWARNING:\n"
				<< "LaTeX representations are unavailable with grid topology and !oneSrcOneSnk,\n"
				<< "if numbNodes > 64 and numbCom > 12.\n"
				<< "The LaTeX representation will not be generated." << endl;
            return;
        }
    }

    outStr << "\\documentclass{article}\n\\usepackage{longtable}\n"
              "\\usepackage{rotating}\n\\usepackage{adjustbox}\n"
              "\\usepackage{geometry}\n\\geometry{paperwidth=14in,paperheight="
              "14in,lmargin=1in,"
              "rmargin=1in,bmargin=1in,tmargin=1in}\n\\usepackage{tikz}\n"
              "\\usepackage{multirow}\n\\begin{document}\n\\usetikzlibrary"
              "{shapes.misc, arrows.meta, quotes, decorations.text}\n"
              "\\section{Generated instance based on grid topology}\n";

    outStr << "\\begin{center}\n\\adjustbox{max size={11.5in}{11.5in}}{\n";

    // begin the generation of graph
    outStr << "\\begin{tikzpicture}[inout/.style={rectangle, minimum size=" + nodeSize + ",very thick,font={" +
                  nodeFont +
                  "\\sffamily}, blue},through/.style={circle,"
                  "minimum size=" +
                  nodeSize +
                  ",very thick,font={\\sffamily}},post/.style={->,shorten <=1pt,"
                  "shorten >=1pt,>={Stealth[round]},thick},randpost/.style={->,"
                  "shorten <=1pt,shorten >=1pt,>={Stealth[round]},thick,red}]\n";

    // placing the nodes in a matrix
    outStr << "\\matrix[row sep=" + spacing + ",column sep=" + spacing +
                  ",nodes=draw,ampersand "
                  "replacement=\\&]{\n";

    string temp = "";
    int counter = 0;
    for (int n = 0; n < numbNodes; n++)
    {      
        counter++;
        temp = strNodes[n];

        if (counter != 1)
        {
            outStr << "\\& ";
        }
        if (temp == "")
        {
            outStr << "\\node [through] (" + to_string(n + 1) + ") {node " + to_string(n + 1) + "};";
        }
        else
        {
            outStr << "\\node [inout, align=center] (" + to_string(n + 1) +
                          "){\\begin{tabular}{l|r}\\multicolumn{2}{l}{node " + to_string(n + 1) +
                          "}\\\\\n comm. & flow \\\\\n\\hline\n" + strNodes[n] + "\\end{tabular}};";
        }
        if (counter == data->lengthX)
        {
            outStr << "\\\\";
            counter = 0;
        }

        outStr << "\n";
    }
    outStr << "};\n";

    // handle two cases
    if (data->oneSourceOneSink)
    {
        // add all arcs
        for (int a0 = 0; a0 < numbNonRandomArcs; a0++)
        {
            outStr << "\\path[post, bend left=20] (" + to_string(arcs[a0].orig + 1) +
                          ") edge [postaction={decorate,decoration={raise=1ex,"
                          "text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a0].capac) + " F=" + to_string(arcs[a0].cost) +
                          " V=" + to_string(arcs[a0].commodsOnArc[0].cost) + "}}}] (" + to_string(arcs[a0].dest + 1) +
                          ");\n";
        }

        for (int a1 = numbNonRandomArcs; a1 < numbArcs; a1++)
        {
            outStr << "\\path[randpost, bend left=20] (" + to_string(arcs[a1].orig + 1) +
                          ") edge [postaction={decorate,decoration={text "
                          "color=red,"
                          "raise=1ex,text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a1].capac) + " F=" + to_string(arcs[a1].cost) +
                          " V=" + to_string(arcs[a1].commodsOnArc[0].cost) + "}}}] (" + to_string(arcs[a1].dest + 1) +
                          ");\n";
        }
        // end generation of graph
        outStr << "\\end{tikzpicture}\n";

        outStr << "}\n\\end{center}\n";

        // set up notes
        outStr << "\\pagebreak\n\\section{Notes}\n";

        outStr << "\\begin{itemize}\n"
                  "\\item{Black arcs are part of the base, grid secondary topology.}\n"
                  "\\item{Red arcs are additional, randomly generated.}\n"
                  "\\item{$C=x$ along arc: overall capacity of arc is equal to "
                  "$x$.}\n"
                  "\\item{$F=y$ along arc: fixed cost of arc is equal to $y$.}\n"
                  "\\item{$V=z$ along arc: variable cost of arc is equal to "
                  "$z$.}\n"
                  "\\item{Capacities and variable costs are not differentiated "
                  "between commodities.}\n"
                  "\\item{Circular nodes are transit nodes.}\n"
                  "\\item{Rectangular nodes are source ($flow > 0$) or sink "
                  "($flow < 0$) nodes for stated commodities.}\n"
                  "\\end{itemize}\n";
    }
    else
    {
        // add all arcs
        for (int a3 = 0; a3 < numbNonRandomArcs; a3++)
        {
            outStr << "\\path[post, bend left=20] (" + to_string(arcs[a3].orig + 1) +
                          ") edge [postaction={decorate,decoration={raise=1ex,"
                          "text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a3].capac) + " F=" + to_string(arcs[a3].cost) + "}}}] (" +
                          to_string(arcs[a3].dest + 1) + ");\n";
        }

        for (int a4 = numbNonRandomArcs; a4 < numbArcs; a4++)
        {
            outStr << "\\path[randpost, bend left=20] (" + to_string(arcs[a4].orig + 1) +
                          ") edge [postaction={decorate,decoration={text "
                          "color=red,"
                          "raise=1ex,text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a4].capac) + " F=" + to_string(arcs[a4].cost) + "}}}] (" +
                          to_string(arcs[a4].dest + 1) + ");\n";
        }

        // end generation of graph
        outStr << "\\end{tikzpicture}\n";

        outStr << "}\n\\end{center}\n";

        // set up notes
        outStr << "\\pagebreak\n\\section{Notes}\n";

        outStr << "\\begin{itemize}\n"
                  "\\item{Black arcs are part of the base, grid secondary topology.}\n"
                  "\\item{Red arcs are additional, randomly generated.}\n"
                  "\\item{$C=x$: overall capacity of arc is equal to $x$.}\n"
                  "\\item{$F=y$: fixed cost of arc is equal to $y$.}\n"
                  "\\item{See below for capacities and variable costs per arc, "
                  "per commodity.}\n"
                  "\\item{Circular nodes are transit nodes.}\n"
                  "\\item{Rectangular nodes are source ($flow > 0$) or sink "
                  "($flow < 0$) nodes for stated commodities.}\n"
                  "\\end{itemize}\n";

        outStr << "\\pagebreak\n";

        // set up table of capacities and variable costs per arc, per commodity
        outStr << "\\section{Capacities and variable costs per arc, per commodity}"
                  "\n\\small\n\\begin{longtable}"
                  "{||l||c|c|c||c|c|c||c|c|c||}\n\\hline\n{Arc}"
                  " & {Comm.} & {Cap.} & {Var. cost} & {Comm.} & {Cap.} & "
                  "{Var. cost} & {Comm.} & {Cap.} & {Var. cost}"
                  "\\\\\n\\hline\n\\endhead\n";

        // https://www.geeksforgeeks.org/how-to-create-vector-of-pairs-in-cpp/

        vector<pair<int, int>> v;
        for (int a = 0; a < numbArcs; a++)
        {
            v.push_back(make_pair(arcs[a].orig, arcs[a].dest));
        }

        // https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
        // https://stackoverflow.com/questions/36622704/how-to-sort-elements-of-pairs-inside-vector

        // sort all arcs in lexicographic order
        vector<size_t> idx(v.size());
        iota(idx.begin(), idx.end(), 0);
        sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {
            return (v[i1].first != v[i2].first) ? v[i1].first < v[i2].first : v[i1].second < v[i2].second;
        });

        // https://stackoverflow.com/questions/9751932/displaying-contents-of-a-vector-container-in-c
        // std::copy(idx.begin(), idx.end(),
        // std::ostream_iterator<int>(std::cout, " "));

        int numbFullRows = numbCommods / 3;
        int lengthLast = numbCommods - 3 * numbFullRows;
        int comm = 0;

        for (int a = 0; a < numbArcs; a++)
        {
            outStr << "(" + to_string(arcs[idx[a]].orig + 1) + ", " + to_string(arcs[idx[a]].dest + 1) + ")";

            if (numbCommods < 3)
            {
                for (int i0 = 0; i0 < lengthLast; i0++)
                    outStr << " & " + to_string(i0 + 1) + " & " + to_string(arcs[idx[a]].commodsOnArc[i0].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[i0].cost);

                for (int i1 = 0; i1 < 3 - lengthLast; i1++)
                    outStr << " & & &";
            }
            else if (numbCommods == 3)
            {
                for (int i7 = 0; i7 < 3; i7++)

                    outStr << " & " + to_string(i7 + 1) + " & " + to_string(arcs[idx[a]].commodsOnArc[i7].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[i7].cost);
            }
            else
            {
                for (int i2 = 0; i2 < 3; i2++)

                    outStr << " & " + to_string(i2 + 1) + " & " + to_string(arcs[idx[a]].commodsOnArc[i2].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[i2].cost);

                outStr << " \\\\\n";

                comm = 4;
                for (int i3 = 0; i3 < numbFullRows - 1; i3++)
                {
                    for (int i4 = 0; i4 < 3; i4++)
                    {
                        outStr << " & " + to_string(comm) + " & " +
                                      to_string(arcs[idx[a]].commodsOnArc[comm - 1].capac) + " & " +
                                      to_string(arcs[idx[a]].commodsOnArc[comm - 1].cost);
                        comm++;
                    }
                    outStr << " \\\\\n";
                }
                for (int i5 = 0; i5 < lengthLast; i5++)
                {
                    outStr << " & " + to_string(comm) + " & " + to_string(arcs[idx[a]].commodsOnArc[comm - 1].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[comm - 1].cost);
                    comm++;
                }
                for (int i6 = 0; i6 < 3 - lengthLast; i6++)
                    outStr << " & & &";
            }

            outStr << " \\\\\n\\hline\n";
        }

        outStr << "\\end{longtable}\n";
    }

    outStr << "\\end{document}";   

    outFile << outStr.str();

    outFile.close();
}

// Outputs the code for depicting the graph of
// the generated instance with a LaTeX compiler,
// when base primary topology is point-to-point and secondary topology is circular (i.e., results
// from secTopology == 4).
void Graph::outputTexCirc(string fileName)
{
	
	ofstream outFile(fileName);
	checkIfValidOutputFile(outFile, fileName);
	
    // top, bottom, left and right lengths of the trapezoid
    // on which the nodes are placed; sum of lengths =
    // numbNodes = index+1
    std::vector<std::vector<int>> sides = {
        {0, 0, 0, 0}, {0, 0, 1, 1}, {1, 0, 1, 1}, {1, 1, 1, 1}, {2, 1, 1, 1},  {2, 2, 1, 1},
        {3, 2, 1, 1}, {3, 3, 1, 1}, {3, 2, 2, 2}, {3, 3, 2, 2}, {4, 3, 2, 2},  {4, 4, 2, 2},
        {4, 3, 3, 3}, {4, 4, 3, 3}, {5, 4, 3, 3}, {5, 5, 3, 3}, {5, 4, 4, 4},  {5, 5, 4, 4},
        {6, 5, 4, 4}, {6, 6, 4, 4}, {6, 5, 5, 5}, {6, 6, 5, 5}, {7, 6, 5, 5},  {7, 7, 5, 5},
        {7, 6, 6, 6}, {7, 7, 6, 6}, {8, 7, 6, 6}, {8, 8, 6, 6}, {8, 7, 7, 7},  {8, 8, 7, 7},
        {9, 8, 7, 7}, {9, 9, 7, 7}, {9, 8, 8, 8}, {9, 9, 8, 8}, {10, 9, 8, 8}, {10, 10, 8, 8}};

    // progressively loaded with the LaTeX code;
    // once finished, its content is saved to file
    stringstream outStr{};

    // store the flow data related to the nodes
    VectStr strNodes(numbNodes);
    for (int n = 0; n < numbNodes; n++)
    {
        strNodes[n] = "";
    }

    // update the information about the source and sink nodes
    float eps = 0.000001;
    for (int n = 0; n < numbNodes; n++)
    {
        for (int k = 0; k < numbCommods; k++)
        {
            if (commods[k].commodNodes[n].volume > eps || commods[k].commodNodes[n].volume < -eps)
            {
                strNodes[n] += (to_string(k + 1) + " & " + to_string(commods[k].commodNodes[n].volume) + " \\\\\n");
            }
        }
    }

    // assign nodes to top, bottom, left and right segments
    set<int> top = {};
    for (int i0 = 0; i0 < sides[numbNodes - 1][0]; i0++)
        top.insert(i0 + 1);

    set<int> right = {};
    for (int i1 = sides[numbNodes - 1][0]; i1 < sides[numbNodes - 1][0] + sides[numbNodes - 1][2]; i1++)
        right.insert(i1 + 1);

    set<int> bot = {};
    for (int i2 = sides[numbNodes - 1][0] + sides[numbNodes - 1][2];
         i2 < sides[numbNodes - 1][0] + sides[numbNodes - 1][2] + sides[numbNodes - 1][1]; i2++)
        bot.insert(i2 + 1);

    set<int> left = {};
    for (int i3 = sides[numbNodes - 1][0] + sides[numbNodes - 1][2] + sides[numbNodes - 1][1];
         i3 < sides[numbNodes - 1][0] + sides[numbNodes - 1][2] + sides[numbNodes - 1][1] + sides[numbNodes - 1][3];
         i3++)
        left.insert(i3 + 1);

    // determine the curvature of each arc
    VectStr direct(numbArcs);
    for (int a5 = numbNonRandomArcs; a5 < numbArcs; a5++)
    {
        if (right.count(arcs[a5].orig + 1) && top.count(arcs[a5].dest + 1))
            direct[a5] = ", bend left=20";

        else if (right.count(arcs[a5].dest + 1) && top.count(arcs[a5].orig + 1))
            direct[a5] = "";

        else if (right.count(arcs[a5].orig + 1) && bot.count(arcs[a5].dest + 1))
            direct[a5] = ", bend right=20";

        else if (right.count(arcs[a5].dest + 1) && bot.count(arcs[a5].orig + 1))
            direct[a5] = "";

        else if (left.count(arcs[a5].orig + 1) && top.count(arcs[a5].dest + 1))
            direct[a5] = ", bend right=20";

        else if (left.count(arcs[a5].dest + 1) && top.count(arcs[a5].orig + 1))
            direct[a5] = "";

        else if (left.count(arcs[a5].orig + 1) && bot.count(arcs[a5].dest + 1))
            direct[a5] = ", bend left=20";

        else if (left.count(arcs[a5].dest + 1) && bot.count(arcs[a5].orig + 1))
            direct[a5] = "";

        else
            direct[a5] = ", bend left=20";
    }

    // set the sizes and spacing of the nodes
    string spacing = "", nodeSize = "", nodeFont = "";

    if (data->oneSourceOneSink)
    {
        if (numbNodes <= 25 && numbCommods <= 10)
        {
            nodeSize = "10mm";
            spacing = "20mm";
        }
        else if (numbNodes <= 30 && numbCommods <= 15)
        {
            nodeSize = "5mm";
            spacing = "15mm";
        }
        else if (numbCommods <= 15)
        {
            nodeFont = "\\tiny";
            nodeSize = "5mm";
            spacing = "10mm";
            cout << "\nWARNING:\n"
				<< "Compilation of LaTeX output might fail when numbNodes and numbCom are high." << endl;
        }
        else
        {
            cout << "\nWARNING:\n"
				<< "LaTeX representations are unavailable with circular secondary topology\n"
                << "and oneSrcOneSnk, if numbNodes > 36 or numbCom > 15.\n"
				<< "The LaTeX representation will not be generated." << endl;
            return;
        }
    }
    else
    {
        nodeFont = "\\tiny";
        if (numbNodes <= 25 && numbCommods <= 10)
        {
            nodeSize = "10mm";
            spacing = "25mm";
        }
        else if (numbNodes <= 25 && numbCommods <= 15)
        {
            nodeSize = "5mm";
            spacing = "15mm";
        }
        else if (numbCommods <= 12)
        {
            nodeSize = "5mm";
            spacing = "10mm";
            cout << "\nWARNING:\n"
				<< "Compilation of LaTeX output might fail when numbNodes and numbCom are high." << endl;
        }
        else
        {
            cout << "\nWARNING:\n"
				<< "LaTeX representations are unavailable with circular secondary topology\n"
				<< "and !oneSrcOneSnk, if numbNodes > 25 and numbCom > 12." << endl;
            return;
        }
    }

    outStr << "\\documentclass{article}\n\\usepackage{longtable}\n"
              "\\usepackage{rotating}\n\\usepackage{adjustbox}\n"
              "\\usepackage{geometry}\n\\geometry{paperwidth=14in,paperheight="
              "14in,lmargin=1in,"
              "rmargin=1in,bmargin=1in,tmargin=1in}\n\\usepackage{tikz}\n"
              "\\usepackage{multirow}\n\\begin{document}\n\\usetikzlibrary"
              "{shapes.misc, arrows.meta, quotes, decorations.text}\n"
              "\\section{Generated instance based on circular topology}\n";

    outStr << "\\begin{center}\n\\adjustbox{max size={11.5in}{11.5in}}{\n";

    // begin the generation of graph
    outStr << "\\begin{tikzpicture}[inout/.style={rectangle, minimum size=" + nodeSize + ",very thick,font={" +
                  nodeFont +
                  "\\sffamily}, blue},through/.style={circle,"
                  "minimum size=" +
                  nodeSize +
                  ",very thick,font={\\sffamily}},post/.style={->,shorten <=1pt,"
                  "shorten >=1pt,>={Stealth[round]},thick},randpost/.style={->,"
                  "shorten <=1pt,shorten >=1pt,>={Stealth[round]},thick,red}]\n";

    // placing the nodes in a matrix
    outStr << "\\matrix[row sep=" + spacing + ",column sep=" + spacing +
                  ",nodes=draw,ampersand "
                  "replacement=\\&]{\n";

    // placing the nodes in the top row
    int counter = 0; // counts from first node
    string temp0 = "";
    for (int topcol = 0; topcol < sides[numbNodes - 1][0]; topcol++)
    {
        temp0 = strNodes[counter];

        outStr << "\\& ";

        if (temp0 == "")
        {
            outStr << "\\node [through] (" + to_string(counter + 1) + ") {node " + to_string(counter + 1) + "};";
        }
        else
        {
            outStr << "\\node [inout, align=center] (" + to_string(counter + 1) +
                          "){\\begin{tabular}{l|r}\\multicolumn{2}{l}{node " + to_string(counter + 1) +
                          "}\\\\\n comm. & flow \\\\\n\\hline\n" + temp0 + "\\end{tabular}};";
        }

        counter++;
    }

    outStr << "\\\\\n";

    // placing the nodes in the intermediate rows
    string temp1 = "", temp2 = "", temp3 = "", temp4 = "", temp5 = "";

    int counter2 = 0; // counts from last node
    for (int inrow = 0; inrow < sides[numbNodes - 1][2]; inrow++)
    {
        // place node at left
        temp1 = strNodes[numbNodes - counter2 - 1];
        if (temp1 == "")
        {
            outStr << "\\node [through] (" + to_string(numbNodes - counter2) + ") {node " +
                          to_string(numbNodes - counter2) + "};";
        }
        else
        {
            outStr << "\\node [inout, align=center] (" + to_string(numbNodes - counter2) +
                          "){\\begin{tabular}{l|r}\\multicolumn{2}{l}{node " + to_string(numbNodes - counter2) +
                          "}\\\\\n comm. & flow \\\\\n\\hline\n" + temp1 + "\\end{tabular}};";
        }

        counter2++;

        // inserting spaces inside current row
        temp2 = "";
        for (int col = 0; col < sides[numbNodes - 1][0] + 1; col++)
        {
            temp2 += " \\& ";
        }

        outStr << temp2;

        // place node at right
        temp3 = strNodes[counter];
        if (temp3 == "")
        {
            outStr << "\\node [through] (" + to_string(counter + 1) + ") {node " + to_string(counter + 1) + "};";
        }
        else
        {
            outStr << "\\node [inout, align=center] (" + to_string(counter + 1) +
                          "){\\begin{tabular}{l|r}\\multicolumn{2}{l}{node " + to_string(counter + 1) +
                          "}\\\\\n comm. & flow \\\\\n\\hline\n" + temp3 + "\\end{tabular}};";
        }
        counter++;

        outStr << " \\&\\\\\n";
    }

    // place nodes in bottom row
    for (int botcol0 = 0; botcol0 < sides[numbNodes - 1][1]; botcol0++)
    {
        temp4 = strNodes[numbNodes - counter2 - 1];

        outStr << "\\& ";

        if (temp4 == "")
        {
            outStr << "\\node [through] (" + to_string(numbNodes - counter2) + ") {node " +
                          to_string(numbNodes - counter2) + "};";
        }
        else
        {
            outStr << "\\node [inout, align=center] (" + to_string(numbNodes - counter2) +
                          "){\\begin{tabular}{l|r}\\multicolumn{2}{l}{node " + to_string(numbNodes - counter2) +
                          "}\\\\\n comm. & flow \\\\\n\\hline\n" + temp4 + "\\end{tabular}};";
        }

        counter2++;
    }
    temp5 = "";

    int rem = sides[numbNodes - 1][1] - sides[numbNodes - 1][0];

    // add spacing in bottom row, if required
    for (int botcol1 = 0; botcol1 < rem; botcol1++)
    {
        temp5 += " \\& ";
    }
    outStr << temp5 + "\\\\\n";

    outStr << "};\n";

    // handle two cases
    if (data->oneSourceOneSink)
    {
        // add all arcs
        for (int a0 = 0; a0 < numbNonRandomArcs; a0++)
        {
            outStr << "\\path[post, bend left=20] (" + to_string(arcs[a0].orig + 1) +
                          ") edge [postaction={decorate,decoration={raise=1ex,"
                          "text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a0].capac) + " F=" + to_string(arcs[a0].cost) +
                          " V=" + to_string(arcs[a0].commodsOnArc[0].cost) + "}}}] (" + to_string(arcs[a0].dest + 1) +
                          ");\n";
        }
        for (int a1 = numbNonRandomArcs; a1 < numbArcs; a1++)
        {
            outStr << "\\path[randpost" + direct[a1] + "] (" + to_string(arcs[a1].orig + 1) +
                          ") edge [postaction={decorate,decoration={text "
                          "color=red,"
                          "raise=1ex,text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a1].capac) + " F=" + to_string(arcs[a1].cost) +
                          " V=" + to_string(arcs[a1].commodsOnArc[0].cost) + "}}}] (" + to_string(arcs[a1].dest + 1) +
                          ");\n";
        }
        // end generation of graph
        outStr << "\\end{tikzpicture}\n";

        outStr << "}\n\\end{center}\n";

        // set up notes
        outStr << "\\pagebreak\n\\section{Notes}\n";

        outStr << "\\begin{itemize}\n"
                  "\\item{Black arcs are part of the base, circular secondary topology.}\n"
                  "\\item{Red arcs are additional, randomly generated.}\n"
                  "\\item{$C=x$ along arc: overall capacity of arc is equal to "
                  "$x$.}\n"
                  "\\item{$F=y$ along arc: fixed cost of arc is equal to $y$.}\n"
                  "\\item{$V=z$ along arc: variable cost of arc is equal to "
                  "$z$.}\n"
                  "\\item{Capacities and variable costs are not differentiated "
                  "between commodities.}\n"
                  "\\item{Circular nodes are transit nodes.}\n"
                  "\\item{Rectangular nodes are source ($flow > 0$) or sink "
                  "($flow < 0$) nodes for stated commodities.}\n"
                  "\\end{itemize}\n";
    }
    else
    {
        // add all arcs
        for (int a3 = 0; a3 < numbNonRandomArcs; a3++)
        {
            outStr << "\\path[post, bend left=20] (" + to_string(arcs[a3].orig + 1) +
                          ") edge [postaction={decorate,decoration={raise=1ex,"
                          "text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a3].capac) + " F=" + to_string(arcs[a3].cost) + "}}}] (" +
                          to_string(arcs[a3].dest + 1) + ");\n";
        }

        for (int a4 = numbNonRandomArcs; a4 < numbArcs; a4++)
        {
            outStr << "\\path[randpost" + direct[a4] + "] (" + to_string(arcs[a4].orig + 1) +
                          ") edge [postaction={decorate,decoration={text "
                          "color=red,"
                          "raise=1ex,text along path,text "
                          "align=center,text={|\\tiny\\sffamily|C=" +
                          to_string(arcs[a4].capac) + " F=" + to_string(arcs[a4].cost) + "}}}] (" +
                          to_string(arcs[a4].dest + 1) + ");\n";
        }

        // end generation of graph
        outStr << "\\end{tikzpicture}\n";

        outStr << "}\n\\end{center}\n";

        // set up notes
        outStr << "\\pagebreak\n\\section{Notes}\n";

        outStr << "\\begin{itemize}\n"
                  "\\item{Black arcs are part of the base, circular secondary topology.}\n"
                  "\\item{Red arcs are additional, randomly generated.}\n"
                  "\\item{$C=x$: overall capacity of arc is equal to $x$.}\n"
                  "\\item{$F=y$: fixed cost of arc is equal to $y$.}\n"
                  "\\item{See below for capacities and variable costs per arc, "
                  "per commodity.}\n"
                  "\\item{Circular nodes are transit nodes.}\n"
                  "\\item{Rectangular nodes are source ($flow > 0$) or sink "
                  "($flow < 0$) nodes for stated commodities.}\n"
                  "\\end{itemize}\n";

        outStr << "\\pagebreak\n";

        // set up table of capacities and variable costs per arc, per commodity
        outStr << "\\section{Capacities and variable costs per arc, per commodity}"
                  "\n\\small\n\\begin{longtable}"
                  "{||l||c|c|c||c|c|c||c|c|c||}\n\\hline\n{Arc}"
                  " & {Comm.} & {Cap.} & {Var. cost} & {Comm.} & {Cap.} & "
                  "{Var. cost} & {Comm.} & {Cap.} & {Var. cost}"
                  "\\\\\n\\hline\n\\endhead\n";

        // https://www.geeksforgeeks.org/how-to-create-vector-of-pairs-in-cpp/

        vector<pair<int, int>> v;
        for (int a = 0; a < numbArcs; a++)
        {
            v.push_back(make_pair(arcs[a].orig, arcs[a].dest));
        }

        // https://stackoverflow.com/questions/1577475/c-sorting-and-keeping-track-of-indexes
        // https://stackoverflow.com/questions/36622704/how-to-sort-elements-of-pairs-inside-vector

        // sort all arcs in lexicographic order
        vector<size_t> idx(v.size());
        iota(idx.begin(), idx.end(), 0);
        sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {
            return (v[i1].first != v[i2].first) ? v[i1].first < v[i2].first : v[i1].second < v[i2].second;
        });

        // https://stackoverflow.com/questions/9751932/displaying-contents-of-a-vector-container-in-c
        // std::copy(idx.begin(), idx.end(),
        // std::ostream_iterator<int>(std::cout, " "));

        int numbFullRows = numbCommods / 3;
        int lengthLast = numbCommods - 3 * numbFullRows;
        int comm = 0;

        for (int a = 0; a < numbArcs; a++)
        {
            outStr << "(" + to_string(arcs[idx[a]].orig + 1) + ", " + to_string(arcs[idx[a]].dest + 1) + ")";

            if (numbCommods < 3)
            {
                for (int i0 = 0; i0 < lengthLast; i0++)
                    outStr << " & " + to_string(i0 + 1) + " & " + to_string(arcs[idx[a]].commodsOnArc[i0].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[i0].cost);

                for (int i1 = 0; i1 < 3 - lengthLast; i1++)
                    outStr << " & & &";
            }
            else if (numbCommods == 3)
            {
                for (int i7 = 0; i7 < 3; i7++)

                    outStr << " & " + to_string(i7 + 1) + " & " + to_string(arcs[idx[a]].commodsOnArc[i7].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[i7].cost);
            }
            else
            {
                for (int i2 = 0; i2 < 3; i2++)

                    outStr << " & " + to_string(i2 + 1) + " & " + to_string(arcs[idx[a]].commodsOnArc[i2].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[i2].cost);

                outStr << " \\\\\n";

                comm = 4;
                for (int i3 = 0; i3 < numbFullRows - 1; i3++)
                {
                    for (int i4 = 0; i4 < 3; i4++)
                    {
                        outStr << " & " + to_string(comm) + " & " +
                                      to_string(arcs[idx[a]].commodsOnArc[comm - 1].capac) + " & " +
                                      to_string(arcs[idx[a]].commodsOnArc[comm - 1].cost);
                        comm++;
                    }
                    outStr << " \\\\\n";
                }
                for (int i5 = 0; i5 < lengthLast; i5++)
                {
                    outStr << " & " + to_string(comm) + " & " + to_string(arcs[idx[a]].commodsOnArc[comm - 1].capac) +
                                  " & " + to_string(arcs[idx[a]].commodsOnArc[comm - 1].cost);
                    comm++;
                }
                for (int i6 = 0; i6 < 3 - lengthLast; i6++)
                    outStr << " & & &";
            }

            outStr << " \\\\\n\\hline\n";
        }

        outStr << "\\end{longtable}\n";
    }

    outStr << "\\end{document}";    

    outFile << outStr.str();

    outFile.close();
}
