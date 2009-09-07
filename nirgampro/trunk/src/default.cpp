#include "extern.h"



sc_clock *nw_clock;

double CLK_FREQ = 1;			///< clock frequency (in GHz)
double CLK_PERIOD = (1/CLK_FREQ);	///< clock period (in ns)

ofstream delaylogin;
ofstream delaylogout;

ofstream eventlog;
int  LOG = 0;				///< log level

routing_type RT_ALGO = XY;

int NUM_BUFS = 8;	///< buffer depth (number of buffers) in input channel fifo
//int FLITSIZE = 5;	///< size of flit in bytes
//int HEAD_PAYLOAD = 1;	///< payload size (in bytes) in head/hdt flit
//int DATA_PAYLOAD = 4;	///< payload size (in bytes) in data/tail flit

ULL WARMUP = 5;		///< warmup period (in clock cycles)
ULL SIM_NUM = 300;	///< Simulation cycles
ULL TG_NUM = 100;	///< clock cycles until which traffic is generated
ULL CAL_NUM = 150;

vector<string> g_appLibName;
int num_tiles;

int* g_ckTable;
AdjList * g_a;