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
#include "instance.h"

// Assigns sizes to arrays of elements.
void Instance::assignSizes()
{
    elems[DEMAND].size = numbCommods;
    elems[CAP_ARC].size = numbArcs;
    elems[CAP_COMMOD].size = numbArcs * numbCommods;
    elems[FIXED_COST].size = numbArcs;
    elems[VAR_COST].size = numbArcs * numbCommods;
}

// Allocates arrays.
void Instance::allocate()
{
    //int numbElem = 0;
    VectInt begin(MAX_TYPE, -1);

    elems.resize(ALL_TYPES + 1);

    for (int i = 0; i < MAX_TYPE; i++)
        elems[i].type = static_cast<ELEMS_TYPE>(i);

    elems[ALL_TYPES].type = ALL_TYPES;
    assignSizes();

    // elements are stored in separate arrays and therefore
    // start from index 0
    for (uint i = 0; i < params->allTypes.size(); i++)
    {
        ELEMS_TYPE elem = params->allTypes[i];
        begin[elem] = 0;
    }

    // setting up individual arrays for all types
    for (uint i = 0; i < params->allTypes.size(); i++)
    {
        ELEMS_TYPE elem = params->allTypes[i];
        // there is a distinct array for each type
        elems[elem].ptr = (double *)malloc(elems[elem].size * sizeof(double));
    }
	
	// initializing commodity-specific capacities to zero since
	// zero-valued figures are excluded from STD format and therefore
	// not read by InstanceRStd::read() and InstanceGStd::read(). 
	int countComm = 0;
	for (int a = 0; a < numbArcs; a++)
	{
		for (int k = 0; k < numbCommods; k++)
        {
			elems[CAP_COMMOD][countComm] = 0;
            countComm++;
		}		
	}

    // setting correspondences between pointer defined in class and
    // beginning of corresponding array (for each type)
    Arc::setPtrCapac(elems[CAP_ARC].ptr);
    Arc::setPtrCost(elems[FIXED_COST].ptr);
    Commodity::setPtr(elems[DEMAND].ptr);
    CommodOnArc::setPtrCapac(elems[CAP_COMMOD].ptr);
    CommodOnArc::setPtrCost(elems[VAR_COST].ptr);

    arcs.resize(numbArcs);
    commods.resize(numbCommods);
    nodes.resize(numbNodes);
}

// Reads a base deterministic MCFND network stored in DOW format.
void InstanceDow::read(istream &is)
{
	if (params->verbose)
		cout << "\n****** Begin reading DOW instance " << params->inputInstFileName << "." << endl;	
	
    string line, dummy;
    double capac, volume, varCost, fixedCost;
    int countArc = 0, countDemand = 0, countComm = 0;

    getline(is, line);
	
	if (line != " MULTIGEN.DAT:")
	{	
		cerr << "\nERROR:\n"
			<< "The file " << params->inputInstFileName << " specified with option key I\n"
			<< "does not conform to the expected DOW format specified with option key F.\n"
			<< "EXECUTION ABORTED" << endl;
			exit(1);	
	}

    is >> numbNodes >> numbArcs >> numbCommods;

    allocate();
	
    for (int a = 0; a < numbArcs; a++)
    {
        Arc arc(numbCommods);
        int idxArc;

        is >> arc.orig >> arc.dest >> varCost >> capac >> fixedCost >> dummy >> idxArc;			
			
		if (arc.orig > numbNodes || arc.dest > numbNodes || arc.orig < 1 || arc.dest < 1)
		{
			cerr << "\nERROR:\n"
				<< "At least one of the two node indexes (" << arc.orig << ", " << arc.dest << ") stated\n"
				<< "in connection with an arc listed in instance file " << params->inputInstFileName << "\n"
				<< "is negative or exceeds the total number of nodes stated at beginning of file.\n"
				<< "EXECUTION ABORTED" << endl;
				exit(1);		
		}			

        arc.orig--;
        arc.dest--;		
		
        elems[CAP_ARC][countArc] = capac;
        elems[FIXED_COST][countArc] = fixedCost;
        nodes[arc.orig].arcOut.push_back(countArc);
        nodes[arc.dest].arcIn.push_back(countArc);
        countArc++;
        idxArc--;

        for (int k = 0; k < numbCommods; k++)
        {
            elems[CAP_COMMOD][countComm] = capac;
            elems[VAR_COST][countComm] = varCost;
            countComm++;
        }

        arcs[idxArc] = arc;
    }
	
	if (countArc != numbArcs)
	{
		cerr << "\nERROR:\n"
			<< "The number of arcs " << countArc << " detected in instance file " << params->inputInstFileName << "\n"
			<< "does not agree with the number of arcs " << numbArcs << " stated at beginning of file.\n"
			<< "EXECUTION ABORTED" << endl;
			exit(1);		
	}

    for (int k = 0; k < numbCommods; k++)
    {
        is >> commods[k].orig >> commods[k].dest >> volume;

        commods[k].orig--;
        commods[k].dest--;
        elems[DEMAND][countDemand++] = volume;
    }
	
	if (params->verbose)
		cout << "\n****** End reading DOW instance " << params->inputInstFileName << ".\n" << endl;
}

// Reads a deterministic MCFNDP stored in restricted STD format.
// Specialized to handle cases where there is a single source node
// and a single sink node for each commodity.
void InstanceRStd::read(istream &is)
{
	if (params->verbose)
		cout << "\n****** Begin reading RSTD instance " << params->inputInstFileName << "." << endl;	
		
    string line, dummy;
    int numbComm, idxComm, idxNode;
	double capac, volume, varCost, fixedCost;
    int countArc = 0, countDemand = 0, countComm = 0;

    is >> numbNodes >> numbArcs >> numbCommods;

    allocate();

    for (int a = 0; a < numbArcs; a++)
    {
        Arc arc(numbCommods);

        is >> arc.orig >> arc.dest >> fixedCost >> capac >> numbComm;
			
		if (arc.orig > numbNodes || arc.dest > numbNodes || arc.orig < 1 || arc.dest < 1)
		{
			cerr << "\nERROR:\n"
				<< "At least one of the two node indexes (" << arc.orig << ", " << arc.dest << ") stated\n"
				<< "in connection with an arc listed in instance file " << params->inputInstFileName << "\n"
				<< "is negative or exceeds the total number of nodes stated at beginning of file.\n"
				<< "EXECUTION ABORTED" << endl;
				exit(1);		
		}	

		if (numbCommods < numbComm)
		{
			cerr << "\nERROR:\n"
				<< "In instance file " << params->inputInstFileName << " the number of commodities " << numbComm << "\n"
				<< "stated in connection with arc (" << arc.orig << ", " << arc.dest << ") for which the\n"
				<< "capacities are zero exceeds the number of commodities stated at beginning of file.\n"
				<< "EXECUTION ABORTED" << endl;
				exit(1);		
		}	

        arc.orig--;
        arc.dest--;
		
		if (capac < 0)
			capac = ASSIGNED_CAPAC_IF_READ_NEG;
		
        elems[CAP_ARC][countArc] = capac;
        elems[FIXED_COST][countArc] = fixedCost;
        nodes[arc.orig].arcOut.push_back(countArc);
        nodes[arc.dest].arcIn.push_back(countArc);
        countArc++;		
		
        for (int k = 0; k < numbComm; k++)
        {
            is >> idxComm;
			
			if (idxComm > numbCommods)
			{
				cerr << "\nERROR:\n"
					<< "The commodity index " << idxComm << " stated while listing the costs and capacities\n"
					<< "in connection with arc ("<< (arc.orig + 1) << ", " << (arc.dest + 1) << ") in instance file\n"
					<< params->inputInstFileName << " exceeds the total number of commodities at beginning of file.\n"
					<< "EXECUTION ABORTED" << endl;
					exit(1);
			}
			
            idxComm--;
            is >> varCost >> capac;
				
			if (capac < 0)
				capac = ASSIGNED_CAPAC_IF_READ_NEG;
		
            elems[CAP_COMMOD][a * numbCommods + (idxComm)] = capac;
            elems[VAR_COST][a * numbCommods + (idxComm)] = varCost;
            countComm++;
        }
		
        arcs[a] = arc;
    }
	
	if (countArc != numbArcs)
	{
		cerr << "\nERROR:\n"
			<< "The number of arcs detected in instance file " << params->inputInstFileName << "\n"
			<< "does not agree with the number of arcs stated at beginning of file.\n"
			<< "EXECUTION ABORTED" << endl;
			exit(1);		
	}	

    for (int k = 0; k < numbCommods * 2; k++)
    {
        is >> idxComm >> idxNode >> volume;

        idxComm--;
        idxNode--;

        if (volume < 0)
            commods[idxComm].dest = idxNode;
        else
        {
            commods[idxComm].orig = idxNode;
            elems[DEMAND][countDemand++] = volume;
        }
    }
	
	if (params->verbose)
		cout << "\n****** End reading RSTD instance " << params->inputInstFileName << ".\n" << endl;
}

// Reads a deterministic MCFNDP stored in generic STD format.
void InstanceGStd::read(istream &is)
{
	if (params->verbose)
		cout << "\n****** Begin reading GSTD instance " << params->inputInstFileName << "." << endl;
	
    string line, dummy;
    int numbComm, idxComm, idxNode;
	double capac, volume, varCost, fixedCost;
    int countArc = 0, countComm = 0;

	is >> numbNodes >> numbArcs >> numbCommods;	

    allocate();

    for (int a = 0; a < numbArcs; a++)
    {
        Arc arc(numbCommods);
		
		is >> arc.orig >> arc.dest >> fixedCost >> capac >> numbComm;
	
		if (arc.orig > numbNodes || arc.dest > numbNodes || arc.orig < 1 || arc.dest < 1)
		{	
			cerr << "\nERROR:\n"
				<< "At least one of the two node indexes (" << arc.orig << ", " << arc.dest << ") stated\n"
				<< "in cnonection with an arc listed in instance file " << params->inputInstFileName << "\n"
				<< "is negative or exceeds the total number of nodes stated at beginning of file.\n"
				<< "EXECUTION ABORTED" << endl;
				exit(1);		
		}	

		if (numbCommods < numbComm)
		{
			cerr << "\nERROR:\n"
				<< "In instance file " << params->inputInstFileName << " the number of commodities " << numbComm << "\n"
				<< "stated in connection with arc (" << arc.orig << ", " << arc.dest << ") for which the\n"
				<< "capacities are zero exceeds the number of commodities stated at beginning of file.\n"
				<< "EXECUTION ABORTED" << endl;
				exit(1);		
		}	

        arc.orig--;
        arc.dest--;
		
		if (capac < 0)
			capac = ASSIGNED_CAPAC_IF_READ_NEG;
		
        elems[CAP_ARC][countArc] = capac;
        elems[FIXED_COST][countArc] = fixedCost;
        nodes[arc.orig].arcOut.push_back(countArc);
        nodes[arc.dest].arcIn.push_back(countArc);
        countArc++;

        for (int k = 0; k < numbComm; k++)
        {					
			is >> idxComm;
			
			if (idxComm > numbCommods)
			{
				cerr << "\nERROR:\n"
					<< "The commodity index " << idxComm << " stated while listing the costs and capacities\n"
					<< "in connection with arc ("<< (arc.orig + 1) << ", " << (arc.dest + 1) << ") in instance file\n"
					<< params->inputInstFileName << " exceeds the total number of commodities at beginning of file.\n"
					<< "EXECUTION ABORTED" << endl;
					exit(1);
			}
			
            idxComm--;			
						
			is >> varCost // arc.comms[idxComm].cost
					>> capac;			
				
			if (capac < 0)
				capac = ASSIGNED_CAPAC_IF_READ_NEG;
            
			elems[CAP_COMMOD][a * numbCommods + (idxComm)] = capac;
            elems[VAR_COST][a * numbCommods + (idxComm)] = varCost;
			
            countComm++;
        }		

        arcs[a] = arc;
    }
	
	if (countArc != numbArcs)
	{
		cerr << "\nERROR:\n"
			<< "The number of arcs detected in instance file " << params->inputInstFileName << "\n"
			<< "does not agree with the number of arcs stated at beginning of file.\n"
			<< "EXECUTION ABORTED" << endl;
			exit(1);		
	}	

    for (int k = 0; k < numbCommods; k++)
    {
        commods[k].outflows.resize(numbNodes, 0);
        commods[k].inflows.resize(numbNodes, 0);
        commods[k].origVolume = 0;
    }

    while (is >> idxComm >> idxNode >> volume)
    {
        idxComm--;
        idxNode--;

        if (volume > 0)
        {
            commods[idxComm].outflows[idxNode] = volume;
            commods[idxComm].origVolume += volume;
        }
        else
            commods[idxComm].inflows[idxNode] = volume;
    }

    for (int k = 0; k < numbCommods; k++)
    {
        elems[DEMAND][k] = commods[k].origVolume;
    }
	
	if (params->verbose)
		cout << "\n****** End reading GSTD instance " << params->inputInstFileName << ".\n" << endl;
}


