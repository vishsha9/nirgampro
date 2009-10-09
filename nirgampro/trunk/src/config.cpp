#include "config.h"

//////////////////////////////////////////////////////////////////////////
/*
sc_clock *nw_clock;

double g_clkFreq = 1;			///< clock frequency (in GHz)
double g_clkPeriod = (1/g_clkFreq);	///< clock period (in ns)

ofstream delaylogin;
ofstream delaylogout;

ofstream g_resultsLog;

ofstream eventlog;

int  LOG = 0;				///< log level

routing_type RT_ALGO = XY;

int NUM_BUFS = 8;	///< buffer depth (number of buffers) in input channel fifo
int FLITSIZE = 5;	///< size of flit in bytes
int HEAD_PAYLOAD = 1;	///< payload size (in bytes) in head/hdt flit
int DATA_PAYLOAD = 4;	///< payload size (in bytes) in data/tail flit

ULL gc_simWarm = 5;		///< gc_simWarm period (in clock cycles)
ULL gc_simNum = 300;	///< Simulation cycles
ULL g_tgNum = 100;	///< clock cycles until which traffic is generated
ULL g_calNum = 150;

ULL g_simCount = 0;
ULL g_stepSimNum = 0;

vector<string> g_appLibName;
//int num_tiles;
int g_tileNum;

int* g_ckTable;
AdjList * g_a;
Tracker * g_tracker;

bool g_isStepMode;
bool g_isCheckMode;
string g_configDir;
string g_resultDir;
string g_topoFile;
*/
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// config.h, config.cpp
// Defining parameters of nirgam running
// gc-prefixed are loaded from the Main Config File, standing for 'G'lobal 'C'onfig parameters.
// ga-prefixed are loaded from argument list, stand for 'G'lobal 'A'rgument parameters.
// g-prefixed parameters in this file is caculated by gc and/or ga. Other g-prefixed global variables should be defined in other files.

//////////////////////////////////////////////////////////////////////////
double gc_simFreq;				///< clock frequency (in GHz)
double g_clkPeriod;				///< clock period (in ns)
double g_simPeriod;				///< simulation period (in ns)
//////////////////////////////////////////////////////////////////////////
ULL gc_simWarm;					///< gc_simWarm period (in clock cycles) before traffic generation begins
ULL gc_simTg;                   ///< Number of clock cycles until which traffic is generated
ULL gc_simNum;                  ///< Total simulation cycles
//////////////////////////////////////////////////////////////////////////
ULL gc_statOp;					///< parameters for statistic
ULL gc_statEd;					///< parameters for statistic
ULL gc_statRes;					///< parameters for statistic
/*
diagram for time points of simulation and statistic
0	gc_simWarm						gc_simTg	gc_simNum
|------|-------------------------------|------------|

0		gc_statOp					gc_statEd
|-----------|----|----|----|----|----|--------------|
					  |----|
					gc_statRes
*/
//////////////////////////////////////////////////////////////////////////
UI gc_simMode;
/*
1. standard
2. debug
3. selfcheck
*/
UI gc_tileType;
/*
1. mount generic IP
2. mount soclib IP which must comfirm to VCI protocal. 
   The interface bewteen nirgam and soclib is writen by MeiNanxiang
*/
//////////////////////////////////////////////////////////////////////////
string gc_route;
//////////////////////////////////////////////////////////////////////////
string gc_topoFile;				///< topology file defining NoC topo. The file must correspond with TopoAnalyzer by WangHan
string gc_appFile;         	 	///< if tileType == 1. Tile-IP mapping definition 
string gc_ipmapFile;        	///< if tileType == 2. Tile-soclibIP mapping definition
string gc_secmapFile;       	///< if tileType == 2. 
//////////////////////////////////////////////////////////////////////////
UI gc_VC_BUFS;					///< buffer depth (number of buffers in i/p channel fifo)
UI gc_FLITSIZE;		            ///< size of flit (in bytes)
UI gc_HEAD_PAYLOAD;             ///< payload size (in bytes) in head/hdt flit
UI gc_DATA_PAYLOAD;             ///< payload size (in bytes) in data/tail flit
//////////////////////////////////////////////////////////////////////////
string ga_configPath;			///< Path of the Main config file 
string g_configHome;			///< Directory of the Main config file
string gc_resultHome;			///< Directory of result
string g_nirgamHome;			///< current Directory
//////////////////////////////////////////////////////////////////////////

int g_tileNum;
