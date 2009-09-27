#include "global.h"


//////////////////////////////////////////////////////////////////////////
// old

int LOG;			///< log level (0 - 4)
ofstream eventlog;
//////////////////////////////////////////////////////////////////////////

class Tracker;
Tracker * g_tracker;

class AdjList;
AdjList * g_a;

ULL g_simCount;
ULL g_simExtNum;

int * g_ckTable;

vector<string> g_appLibName;

sc_clock *g_clock;