#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <regex>
#include <chrono>
#include <filesystem>

// Generates library of deterministic MCFNDPs

using namespace std;
namespace fs = filesystem;

const string OUTROOT = "/home/erlars/C++/GenDeterm/library/";

const int seedNumber = 50163844;

const int initStreamNumber = 33;

int noParalArc = 20;

vector<string> secTopoStrings = {"rand", "toral", "cyl", "flat", "circ"};

// Hub-and-satellite primary topology

vector<string> hs_vols_keys = {"minComVol", "maxComVol"};
vector<int> hs_vols_values = {20, 100};

vector<string> hs_costs_keys = {"minFixCos", "maxFixCos", "minVarCos", "maxVarCos", "minSatFixCos", "maxSatFixCos", "minSatVarCos", "maxSatVarCos"};
vector<int> hs_costs_values = {20, 100, 20, 100, 20, 100, 20, 100};

map <string, int> hs_costs = {{"minFixCos", 20}, {"maxFixCos", 100}, {"minVarCos", 20}, {"maxVarCos", 100}};

// Hub-and-satellite primary topology, flat secondary topology, one source node and one sink node for each commodity	

vector<vector<int>> hs_oneSrcOneSnk_grid_secTopoSpecs_values = {{2, 3, 2, 8, 2, 8, 3, 15, 30, 10, 15, 2, 15, 30, 10, 15, 2},
{2, 3, 2, 8, 2, 8, 6, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
{2, 3, 2, 8, 2, 8, 12, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2}, 
{4, 5, 2, 8, 2, 8, 10, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
{4, 5, 2, 8, 2, 8, 20, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
{4, 5, 2, 8, 2, 8, 40, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2}, 
{6, 7, 2, 8, 2, 8, 21, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
{6, 7, 2, 8, 2, 8, 42, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
{6, 7, 2, 8, 2, 8, 84, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
{8, 9, 2, 8, 2, 8, 36, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
{8, 9, 2, 8, 2, 8, 72, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
{8, 9, 2, 8, 2, 8, 144, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2}};
// {10, 11, 2, 8, 2, 8, 55, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
// {10, 11, 2, 8, 2, 8, 110, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
// {10, 11, 2, 8, 2, 8, 220, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
// {12, 13, 2, 8, 2, 8, 78, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
// {12, 13, 2, 8, 2, 8, 156, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2},
// {12, 13, 2, 8, 2, 8, 312, 15, 45, 10, 15, 2, 15, 45, 10, 15, 2}};

vector<string> hs_oneSrcOneSnk_grid_secTopoSpecs_keys = {"lengthX", "lengthY", "minSatsPerHub", "maxSatsPerHub", "minSatsRoundSize", "maxSatsRoundSize", "numbCom", "minArcCap", "maxArcCap", "minComCap", "maxComCap", "nbAddArcPerSampHead", "minSatArcCap", "maxSatArcCap", "minSatComCap", "maxSatComCap", "nbAddSatArcPerSampHead"};

vector<vector<double>> hs_oneSrcOneSnk_adjust_values = {{0.33, 0.33, 0.33, 0.33},
{0.33, 0.5, 0.33, 0.5},
{0.33, 1.0, 0.33, 1.0},
{0.33, 1.25, 0.33, 1.25},
{0.33, 1.5, 0.33, 1.5},
{0.5, 0.33, 0.5, 0.33},
{0.5, 0.5, 0.5, 0.5},
{0.5, 1.0, 0.5, 1.0},
{0.5, 1.25, 0.5, 1.25},
{0.5, 1.5, 0.5, 1.5},
{1.0, 0.33, 1.0, 0.33},
{1.0, 0.5, 1.0, 0.5},
{1.0, 1.0, 1.0, 1.0},
{1.0, 1.25, 1.0, 1.25},
{1.0, 1.5, 1.0, 1.5},
{1.25, 0.33, 1.25, 0.33},
{1.25, 0.5, 1.25, 0.5},
{1.25, 1.0, 1.25, 1.0},
{1.25, 1.25, 1.25, 1.25},
{1.25, 1.5, 1.25, 1.5},
{1.5, 0.33, 1.5, 0.33},
{1.5, 0.5, 1.5, 0.5},
{1.5, 1.0, 1.5, 1.0},
{1.5, 1.25, 1.5, 1.25},
{1.5, 1.5, 1.5, 1.5}};

vector<string> hs_oneSrcOneSnk_adjust_keys = {"adjCapacsDown", "adjFixCostsUp", "adjSatCapacsDown", "adjSatFixCostsUp"};


int main( int argc , char ** argv )
{	
		
	string folderPathStr, filePath , instName, optsStr, temp0;
	int streamNumber = initStreamNumber;	
	
	// Hub-and-satellite primary topology, flat secondary topology, one source node and one sink node for each commodity
	for (int indSecTopoSpecs = 0; indSecTopoSpecs < hs_oneSrcOneSnk_grid_secTopoSpecs_values.size(); indSecTopoSpecs++)
	{	
		for (int indSecAdjs = 0; indSecAdjs < hs_oneSrcOneSnk_adjust_values.size(); indSecAdjs++)
		{			
			// build cylindrical secondary topologies
			for (int indSecTopo = 3; indSecTopo < 4; indSecTopo++)
			{			
				optsStr = ("hubsAndSats true\n");
				
				instName = "hs_vols";
				
				for (int indVol = 0; indVol < hs_vols_keys.size(); indVol++)
				{	 
					optsStr += (hs_vols_keys[indVol] + " " + to_string(hs_vols_values[indVol]) + "\n");
					instName += ("-" + to_string(hs_vols_values[indVol]));		
				}
				
				instName += "_csts";
				
				for (int indCos = 0; indCos < hs_costs_keys.size(); indCos++)
				{	 
					optsStr += (hs_costs_keys[indCos] + " " + to_string(hs_costs_values[indCos]) + "\n");
					instName += ("-" + to_string(hs_costs_values[indCos]));		
				}
				
				optsStr += ("oneSrcOneSnk true\nsecTopology " + to_string(indSecTopo) + "\n");				
				instName += ("_oneSrcOneSnk_secTop-" + secTopoStrings[indSecTopo] + "_secTopSpe");
								
				for (int indElem0 = 0; indElem0 < hs_oneSrcOneSnk_grid_secTopoSpecs_keys.size(); indElem0++)
				{				
					optsStr += (hs_oneSrcOneSnk_grid_secTopoSpecs_keys[indElem0] + " " +  to_string(hs_oneSrcOneSnk_grid_secTopoSpecs_values[indSecTopoSpecs][indElem0]) + "\n");
					instName += ("-" + to_string( hs_oneSrcOneSnk_grid_secTopoSpecs_values[indSecTopoSpecs][indElem0]));			
				}
				
				optsStr += ("noParalArc " + to_string(noParalArc) + "\n");				
				instName += ("_noPar-" + to_string(noParalArc));								
				instName += ("_adjSpe");
				
				for (int indElem1 = 0; indElem1 < hs_oneSrcOneSnk_adjust_keys.size(); indElem1++)
				{				
					optsStr += (hs_oneSrcOneSnk_adjust_keys[indElem1] + " " +  to_string(hs_oneSrcOneSnk_adjust_values[indSecAdjs][indElem1]) + "\n");					
					temp0 = to_string(hs_oneSrcOneSnk_adjust_values[indSecAdjs][indElem1]);					
					replace(temp0.begin(), temp0.end(), '.', '~');
					temp0.resize(temp0.length() - 4);
					instName += ("-" + temp0);									
				}

				streamNumber += 100;
				instName += ("_seed-" + to_string(seedNumber) + "_strm-" +to_string(streamNumber));				
				optsStr = "// file name: " + instName + "\n\n" + "seed " + to_string(seedNumber) + "\n" + "stream " + to_string(streamNumber) + "\n" + optsStr;
				folderPathStr = OUTROOT + "hubsAndSats/oneSrcOneSnk/" + secTopoStrings[indSecTopo] + "/";				
				fs::path folderPath = folderPathStr;
				
				try
				{        
					fs::create_directories(folderPathStr);            
				}
				catch (const fs::filesystem_error& e)
				{
					cerr << "Error: " << e.what() << '\n';
				}

				filePath = folderPathStr + instName + ".par";				
				ofstream outFile(filePath);				
				outFile << optsStr << endl;			
			}			
		}
	}	
}
	

