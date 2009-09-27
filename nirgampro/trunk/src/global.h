#ifndef _Extern_H_
#define _Extern_H_

#include <fstream>
#include <vector>
using namespace std;

#include "config.h"
#include "systemc.h"

//////////////////////////////////////////////////////////////////////////
// old

extern int LOG;			///< log level (0 - 4)
extern ofstream eventlog;	///< file stream to log events
//////////////////////////////////////////////////////////////////////////


class Tracker;
extern Tracker * g_tracker;

class AdjList;
extern AdjList * g_a;

extern ULL g_simCount;
extern ULL g_simExtNum;

extern int * g_ckTable;

extern vector<string> g_appLibName;

extern sc_clock *g_clock;	///< pointer to clock



#endif