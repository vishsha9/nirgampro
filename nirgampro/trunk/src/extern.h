#ifndef _Extern_H_
#define _Extern_H_

#include <string>
#include <vector>
#include <set>
#include "typedef.h"

//global clock and reset signal
//extern sc_clock g_CLK;	
//extern sc_signal<bool> g_RESETN;

//extern topology TOPO;		///< topology (NoC)
//extern int num_rows;		///< number of rows in topology
//extern int num_cols;		///< number of columns in topology
//extern int num_tiles;		///< number of tiles in topology

extern routing_type RT_ALGO;	///< routing algorithm

extern int LOG;			///< log level (0 - 4)

//extern int num_tiles;
extern int g_tileNum;

extern sc_clock *nw_clock;	///< pointer to clock

//////////////////////////////////////////////////////////////////////////
extern int NUM_BUFS;		///< buffer depth (number of buffers in i/p channel fifo)
extern int FLITSIZE;		///< size of flit (in bytes)
extern int HEAD_PAYLOAD;	///< payload size (in bytes) in head/hdt flit
extern int DATA_PAYLOAD;	///< payload size (in bytes) in data/tail flit
//////////////////////////////////////////////////////////////////////////

extern ULL g_warmUp;		///< g_warmUp period (in clock cycles) before traffic generation begins
extern ULL g_simNum;		///< Total simulation cycles
extern ULL g_tgNum;		///< Number of clock cycles until which traffic is generated
extern ULL g_calNum;

extern double g_clkFreq;		///< clock frequency (in GHz)
extern double g_clkPeriod;	///< clock period (in ns)

extern ofstream eventlog;	///< file stream to log events
extern sc_trace_file* tracefile;///< file stream to generate vcd trace
//extern ofstream g_resultsLog;	///< file stream to log results

extern vector<string> g_appLibName;
extern int * g_ckTable;

class AdjList;
extern AdjList * g_a;

class Tracker;
extern Tracker * g_tracker;

extern ofstream delaylogin;
extern ofstream delaylogout;
extern ofstream g_resultsLog;


//////////////////////////////////////////////////////////////////////////
extern ofstream flit_latency; 	// format added
extern ULL flit_num;		// format added
extern ULL flit_latency_total; 	// format added
extern set<ULL>* sent_flit; // format added
extern ULL* last_sent_flit; // format added
extern ULL g_calNum;  // format added 
extern ULL* flit_recved; 
//////////////////////////////////////////////////////////////////////////

extern string g_configDir;
extern string g_resultDir;
extern string g_topoFile;

extern bool g_isCheckMode;
extern bool g_isStepMode;

extern ULL g_simCount;
extern ULL g_stepSimNum;

extern string convertPathToWin32(string path);
extern void makeDirectory(string path);

#endif