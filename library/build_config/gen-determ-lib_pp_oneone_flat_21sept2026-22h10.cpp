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

const int initStreamNumber = 13;

int noParalArc = 20;

vector<string> secTopoStrings = {"rand", "toral", "cyl", "flat", "circ"};

// Point-to-point primary topology

vector<string> pp_vols_keys = {"minComVol", "maxComVol"};
vector<int> pp_vols_values = {20, 100};

vector<string> pp_costs_keys = {"minFixCos", "maxFixCos", "minVarCos", "maxVarCos"};
vector<int> pp_costs_values = {20, 100, 20, 100};

map <string, int> pp_costs = {{"minFixCos", 20}, {"maxFixCos", 100}, {"minVarCos", 20}, {"maxVarCos", 100}};

// Point-to-point primary topology, flat secondary topology, one source node and one sink node for each commodity

vector<vector<int>> pp_oneSrcOneSnk_grid_secTopoSpecs_values = {{2, 3, 3, 15, 30, 10, 15, 2},
{2, 3, 6, 15, 45, 10, 15, 2},
{2, 3, 12, 15, 45, 10, 15, 2}, 
{4, 5, 10, 15, 45, 10, 15, 2},
{4, 5, 20, 15, 45, 10, 15, 2},
{4, 5, 40, 15, 45, 10, 15, 2}, 
{6, 7, 21, 15, 45, 10, 15, 2},
{6, 7, 42, 15, 45, 10, 15, 2},
{6, 7, 84, 15, 45, 10, 15, 2},
{8, 9, 36, 15, 45, 10, 15, 2},
{8, 9, 72, 15, 45, 10, 15, 2},
{8, 9, 144, 15, 45, 10, 15, 2},
{10, 11, 55, 15, 45, 10, 15, 2},
{10, 11, 110, 15, 45, 10, 15, 2},
{10, 11, 220, 15, 45, 10, 15, 2},
{12, 13, 78, 15, 45, 10, 15, 2},
{12, 13, 156, 15, 45, 10, 15, 2},
{12, 13, 312, 15, 45, 10, 15, 2}};

vector<string> pp_oneSrcOneSnk_grid_secTopoSpecs_keys = {"lengthX", "lengthY", "numbCom", "minArcCap", "maxArcCap", "minComCap", "maxComCap", "nbAddArcPerSampHead"};


vector<vector<double>> pp_oneSrcOneSnk_adjust_values = {{0.33, 0.33},
{0.33, 0.5},
{0.33, 1.0},
{0.33, 2.0},
{0.33, 3.0},
{0.5, 0.33},
{0.5, 0.5},
{0.5, 1.0},
{0.5, 2.0},
{0.5, 3.0},
{1.0, 0.33},
{1.0, 0.5},
{1.0, 1.0},
{1.0, 2.0},
{1.0, 3.0},
{2.0, 0.33},
{2.0, 0.5},
{2.0, 1.0},
{2.0, 2.0},
{2.0, 3.0},
{3.0, 0.33},
{3.0, 0.5},
{3.0, 1.0},
{3.0, 2.0},
{3.0, 3.0}};

vector<string> pp_oneSrcOneSnk_adjust_keys = {"adjCapacsDown", "adjFixCostsUp"};


int main( int argc , char ** argv )
{		
	string folderPathStr, filePath , instName, optsStr, temp0;
	int streamNumber = initStreamNumber;
	
	// Point-to-point primary topology, flat secondary topology, one source node and one sink node for each commodity
	for (int indSecTopoSpecs = 0; indSecTopoSpecs < pp_oneSrcOneSnk_grid_secTopoSpecs_values.size(); indSecTopoSpecs++)
	{	
		for (int indSecAdjs = 0; indSecAdjs < pp_oneSrcOneSnk_adjust_values.size(); indSecAdjs++)
		{			
			// build flat secondary topology
			for (int indSecTopo = 3; indSecTopo < 4; indSecTopo++)
			{			
				optsStr = ("hubsAndSats false\n");
				
				instName = "pp_vols";
				
				for (int indVol = 0; indVol < pp_vols_keys.size(); indVol++)
				{	 
					optsStr += (pp_vols_keys[indVol] + " " + to_string(pp_vols_values[indVol]) + "\n");
					instName += ("-" + to_string(pp_vols_values[indVol]));		
				}
				
				instName += "_csts";
				
				for (int indCos = 0; indCos < pp_costs_keys.size(); indCos++)
				{	 
					optsStr += (pp_costs_keys[indCos] + " " + to_string(pp_costs_values[indCos]) + "\n");
					instName += ("-" + to_string(pp_costs_values[indCos]));		
				}
				
				optsStr += ("oneSrcOneSnk true\nsecTopology " + to_string(indSecTopo) + "\n");				
				instName += ("_oneSrcOneSnk_secTop-" + secTopoStrings[indSecTopo] + "_secTopSpe");
								
				for (int indElem0 = 0; indElem0 < pp_oneSrcOneSnk_grid_secTopoSpecs_keys.size(); indElem0++)
				{				
					optsStr += (pp_oneSrcOneSnk_grid_secTopoSpecs_keys[indElem0] + " " +  to_string(pp_oneSrcOneSnk_grid_secTopoSpecs_values[indSecTopoSpecs][indElem0]) + "\n");
					instName += ("-" + to_string( pp_oneSrcOneSnk_grid_secTopoSpecs_values[indSecTopoSpecs][indElem0]));			
				}
				
				optsStr += ("noParalArc " + to_string(noParalArc) + "\n");				
				instName += ("_noPar-" + to_string(noParalArc));								
				instName += ("_adjSpe");
				
				for (int indElem1 = 0; indElem1 < pp_oneSrcOneSnk_adjust_keys.size(); indElem1++)
				{				
					optsStr += (pp_oneSrcOneSnk_adjust_keys[indElem1] + " " +  to_string(pp_oneSrcOneSnk_adjust_values[indSecAdjs][indElem1]) + "\n");					
					temp0 = to_string(pp_oneSrcOneSnk_adjust_values[indSecAdjs][indElem1]);					
					replace(temp0.begin(), temp0.end(), '.', '~');
					temp0.resize(temp0.length() - 4);					
					instName += ("-" + temp0);									
				}

				streamNumber += 100;
				instName += ("_seed-" + to_string(seedNumber) + "_strm-" + to_string(streamNumber));				
				optsStr = "// file name: " + instName + "\n\n" + "seed " + to_string(seedNumber) + "\n" + "stream " + to_string(streamNumber) + "\n" + optsStr;
				folderPathStr = OUTROOT + "pointToPoint/oneSrcOneSnk/" + secTopoStrings[indSecTopo] + "/";				
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
	

