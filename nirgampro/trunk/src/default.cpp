#include "extern.h"


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

ULL g_warmUp = 5;		///< g_warmUp period (in clock cycles)
ULL g_simNum = 300;	///< Simulation cycles
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

