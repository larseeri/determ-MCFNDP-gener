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

const int initStreamNumber = 23;

int noParalArc = 20;

vector<string> secTopoStrings = {"rand", "toral", "cyl", "flat", "circ"};

// Point-to-point primary topology

vector<string> pp_vols_keys = {"minComVol", "maxComVol"};
vector<int> pp_vols_values = {20, 100};

vector<string> pp_costs_keys = {"minFixCos", "maxFixCos", "minVarCos", "maxVarCos"};
vector<int> pp_costs_values = {20, 100, 20, 100};

map <string, int> pp_costs = {{"minFixCos", 20}, {"maxFixCos", 100}, {"minVarCos", 20}, {"maxVarCos", 100}};

// Point-to-point primary topology, flat secondary topology, identical sources and identical sinks for all commodities

vector<vector<int>> pp_eqSrcsEqSnks_grid_secTopoSpecs_values = {{2, 3, 1, 1, 1, 1, 3, 15, 30, 10, 15, 2},
{2, 3, 1, 2, 1, 2, 6, 15, 45, 10, 15, 2},
{2, 3, 2, 3, 2, 3, 12, 15, 45, 10, 15, 2}, 
{4, 5, 3, 5, 3, 5, 10, 15, 45, 10, 15, 2},
{4, 5, 5, 7, 5, 7, 20, 15, 45, 10, 15, 2},
{4, 5, 7, 10, 7, 10, 40, 15, 45, 10, 15, 2}, 
{6, 7, 6, 9, 6, 9, 21, 15, 45, 10, 15, 2},
{6, 7, 9, 14, 9, 14, 42, 15, 45, 10, 15, 2},
{6, 7, 14, 21, 14, 21, 84, 15, 45, 10, 15, 2},
{8, 9, 11, 16, 11, 16, 36, 15, 45, 10, 15, 2},
{8, 9, 16, 24, 16, 24, 72, 15, 45, 10, 15, 2},
{8, 9, 24, 36, 24, 36, 144, 15, 45, 10, 15, 2},
{10, 11, 16, 24, 16, 24, 55, 15, 45, 10, 15, 2},
{10, 11, 24, 36, 24, 36, 110, 15, 45, 10, 15, 2},
{10, 11, 36, 55, 36, 55, 220, 15, 45, 10, 15, 2},
{12, 13, 23, 35, 23, 35, 78, 15, 45, 10, 15, 2},
{12, 13, 35, 52, 35, 52, 156, 15, 45, 10, 15, 2},
{12, 13, 52, 78, 52, 78, 312, 15, 45, 10, 15, 2}};

vector<string> pp_eqSrcsEqSnks_grid_secTopoSpecs_keys = {"lengthX", "lengthY", "minNbSrc", "maxNbSrc", "minNbSnk", "maxNbSnk", "numbCom", "minArcCap", "maxArcCap", "minComCap", "maxComCap", "nbAddArcPerSampHead"};

vector<vector<double>> pp_eqSrcsEqSnks_adjust_values = {{0.33, 0.33},
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

vector<string> pp_eqSrcsEqSnks_adjust_keys = {"adjCapacsDown", "adjFixCostsUp"};



int main( int argc , char ** argv )
{	
	string folderPathStr, filePath , instName, optsStr, temp0;
	int streamNumber = initStreamNumber;
			
	// Point-to-point primary topology, flat secondary topology, identical sources and identical sinks for all commodities
	for (int indSecTopoSpecs = 0; indSecTopoSpecs < pp_eqSrcsEqSnks_grid_secTopoSpecs_values.size(); indSecTopoSpecs++)
	{	
		for (int indSecAdjs = 0; indSecAdjs < pp_eqSrcsEqSnks_adjust_values.size(); indSecAdjs++)
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
				
				optsStr += ("eqSrcsEqSnks true\nsecTopology " + to_string(indSecTopo) + "\n");				
				instName += ("_eqSrcsEqSnks_secTop-" + secTopoStrings[indSecTopo] + "_secTopSpe");
								
				for (int indElem0 = 0; indElem0 < pp_eqSrcsEqSnks_grid_secTopoSpecs_keys.size(); indElem0++)
				{				
					optsStr += (pp_eqSrcsEqSnks_grid_secTopoSpecs_keys[indElem0] + " " +  to_string(pp_eqSrcsEqSnks_grid_secTopoSpecs_values[indSecTopoSpecs][indElem0]) + "\n");
					instName += ("-" + to_string( pp_eqSrcsEqSnks_grid_secTopoSpecs_values[indSecTopoSpecs][indElem0]));			
				}
				
				optsStr += ("noParalArc " + to_string(noParalArc) + "\n");				
				instName += ("_noPar-" + to_string(noParalArc));								
				instName += ("_adjSpe");
				
				for (int indElem1 = 0; indElem1 < pp_eqSrcsEqSnks_adjust_keys.size(); indElem1++)
				{				
					optsStr += (pp_eqSrcsEqSnks_adjust_keys[indElem1] + " " +  to_string(pp_eqSrcsEqSnks_adjust_values[indSecAdjs][indElem1]) + "\n");					
					temp0 = to_string(pp_eqSrcsEqSnks_adjust_values[indSecAdjs][indElem1]);					
					replace(temp0.begin(), temp0.end(), '.', '~');
					temp0.resize(temp0.length() - 4);
					instName += ("-" + temp0);									
				}

				streamNumber += 100;
				instName += ("_seed-" + to_string(seedNumber) + "_strm-" + to_string(streamNumber));				
				optsStr = "// file name: " + instName + "\n\n" + "seed " + to_string(seedNumber) + "\n" + "stream " + to_string(streamNumber) + "\n" + optsStr;
				folderPathStr = OUTROOT + "pointToPoint/eqSrcsEqSnks/" + secTopoStrings[indSecTopo] + "/";				
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
	

