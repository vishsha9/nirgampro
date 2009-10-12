#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <vector>
#include <set>
#include "typedef.h"

using namespace std;

//global clock and reset signal
//extern sc_clock g_CLK;	
//extern sc_signal<bool> g_RESETN;

//extern topology TOPO;		///< topology (NoC)
//extern int num_rows;		///< number of rows in topology
//extern int num_cols;		///< number of columns in topology
//extern int num_tiles;		///< number of tiles in topology

/*
//////////////////////////////////////////////////////////////////////////
//old
extern vector<string> g_appLibName;


//////////////////////////////////////////////////////////////////////////
extern ofstream flit_latency; 	// format added
extern ULL flit_num;		// format added
extern ULL flit_latency_total; 	// format added
extern set<ULL>* sent_flit; // format added
extern ULL* last_sent_flit; // format added
extern ULL g_calNum;  // format added 
extern ULL* flit_recved; 
//////////////////////////////////////////////////////////////////////////

extern ULL g_simCount;
extern ULL g_stepSimNum;

extern int * g_ckTable;
//////////////////////////////////////////////////////////////////////////
*/


//////////////////////////////////////////////////////////////////////////
// config.h, config.cpp
// Defining parameters of nirgam running
// gc-prefixed are loaded from the Main Config File, standing for 'G'lobal 'C'onfig parameters.
// ga-prefixed are loaded from argument list, stand for 'G'lobal 'A'rgument parameters.
// g-prefixed parameters in this file is caculated by gc and/or ga. Other g-prefixed global variables should be defined in other files.


//////////////////////////////////////////////////////////////////////////
extern double gc_simFreq;		///< clock frequency (in GHz)
extern double g_clkPeriod;	///< clock period (in ns)
extern double g_simPeriod;	///< simulation period (in ns)
// parameters of time points of simulation
extern ULL gc_simWarm;		///< gc_simWarm period (in clock cycles) before traffic generation begins
extern ULL gc_simTg;		///< Number of clock cycles until which traffic is generated
extern ULL gc_simNum;		///< Total simulation cycles
//  parameters for statistic
extern ULL gc_statOp;
extern ULL gc_statEd;
extern ULL gc_statRes;
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
extern UI gc_simMode;
/*
1. standard
2. debug
3. selfcheck
*/
extern UI gc_tileType;
/*
1. mount generic IP
2. mount soclib IP which must comfirm to VCI protocal. 
   The interface bewteen nirgam and soclib is writen by MeiNanxiang
*/

//////////////////////////////////////////////////////////////////////////
extern string gc_route;
//////////////////////////////////////////////////////////////////////////
// Other config file
extern string gc_topoFile;		///< topology file defining NoC topo. The file must correspond with TopoAnalyzer by WangHan
extern string gc_appFile;		///< if tileType == 1. Tile-IP mapping definition 
extern string gc_ipmapFile;		///< if tileType == 2. Tile-soclibIP mapping definition
extern string gc_secmapFile;	///< if tileType == 2. 
//////////////////////////////////////////////////////////////////////////
extern UI gc_VC_BUFS;		///< buffer depth (number of buffers in i/p channel fifo)
extern UI gc_FLITSIZE;		///< size of flit (in bytes)
extern UI gc_HEAD_PAYLOAD;	///< payload size (in bytes) in head/hdt flit
extern UI gc_DATA_PAYLOAD;	///< payload size (in bytes) in data/tail flit
//////////////////////////////////////////////////////////////////////////
extern string ga_configPath;	///< Path of the Main config file 
extern string ga_elfPath;
extern string g_configHome;		///< Directory of the Main config file
extern string gc_resultHome;	///< Directory of result
extern string g_nirgamHome;		///< current Directory
//////////////////////////////////////////////////////////////////////////

extern int g_tileNum;


#endif