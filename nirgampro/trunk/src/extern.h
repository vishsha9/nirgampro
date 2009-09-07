#ifndef _Extern_H_
#define _Extern_H_

#include <string>
#include <vector>
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

extern int num_tiles;

extern sc_clock *nw_clock;	///< pointer to clock

extern int NUM_BUFS;		///< buffer depth (number of buffers in i/p channel fifo)
//extern int FLITSIZE;		///< size of flit (in bytes)
//extern int HEAD_PAYLOAD;	///< payload size (in bytes) in head/hdt flit
//extern int DATA_PAYLOAD;	///< payload size (in bytes) in data/tail flit

extern ULL WARMUP;		///< warmup period (in clock cycles) before traffic generation begins
extern ULL SIM_NUM;		///< Total simulation cycles
extern ULL TG_NUM;		///< Number of clock cycles until which traffic is generated
extern ULL CAL_NUM;

extern double CLK_FREQ;		///< clock frequency (in GHz)
extern double CLK_PERIOD;	///< clock period (in ns)

extern ofstream eventlog;	///< file stream to log events
//extern sc_trace_file* tracefile;///< file stream to generate vcd trace
//extern ofstream results_log;	///< file stream to log results

extern vector<string> g_appLibName;
extern int * g_ckTable;

class AdjList;
extern AdjList * g_a;

extern ofstream delaylogin;
extern ofstream delaylogout;


#endif