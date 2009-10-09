#ifndef __TRACKER_H__
#define __TRACKER_H__

#include "../flit.h"
#include "../../global.h"

#include "systemc.h"
#include <fstream>
#include <vector>
#include <iostream>


struct VC;
struct switch_reg;
struct NoC;

struct InputChannel;
struct OutputChannel;
struct NWTile;

class IcProbe{
public:
	UI portId;
	vector<VC> * vc;
	InputChannel * icPtr;

	ULL num_flits;
	ULL num_pkts;

	float avg_inputRate;
	
	int beg_cycle;
	int end_cycle;
	int total_cycles;

	IcProbe();
	void updateInfo();
};

class OcProbe{
public:
	UI portId;
	vector<switch_reg> * r_in;
	vector<switch_reg> * r_vc;
	OutputChannel * ocPtr;

	ULL latency;			///< total latency
	ULL num_pkts;			///< total number of packets
	ULL num_flits;			///< total number of flits
	ULL input_time[NUM_VCS];			///< generation timestamp of head flit of a packet

	float avg_latency;		///< average latency (in clock cycles) per packet
	float avg_latency_flit;		///< average latency (in clock cycles) per flit
	float avg_throughput;		///< average throughput (in Gbps)

	int beg_cycle;			///< clock cycle in which first flit is recieved in the channel
	int end_cycle;			///< clock cycle in which last flit leaves the channel
	int total_cycles;		///< total number of clock cycles
	OcProbe();

	void updateInfo();
};

class NwtileProbe{
public:
	UI tileId;
	int io_num;
	vector<IcProbe> icProbes;
	vector<OcProbe> ocProbes;
	
	vector<vector<bool> > * VA_vcFree;
	vector<UI> * portTable;
	NWTile * tilePtr;
};

class Tracker{
//////////////////////////////////////////////////////////////////////////
public:
	Tracker();
	~Tracker();

	//////////////////////////////////////////////////////////////////////////
	// probe functions
	void addProbes(NoC & noc);
	void enterDebuger();
	void printStat(ostream & xout);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// log functions
	void processFlitWhenBorn(flit & f);
	void enterInputChannel(UI tileId, UI portId, flit & f);
	void exitInputChannel(UI tileId, UI portId, flit & f);

	void enterOutputChannel(UI tileId, UI portId, flit & f);
	void getVcAtOutputChannel(UI tileId, UI portId, flit & f);
	void exitOutputChannel(UI tileId, UI portId, flit & f);

	void enterWire(UI tileId1, UI tileId2, flit & f);
	void exitWire(UI tileId1, UI tileId2, flit & f);
	//////////////////////////////////////////////////////////////////////////

	int LOG_level;

//////////////////////////////////////////////////////////////////////////
protected:
	vector<NwtileProbe> tileProbes;
	
	ULL sequence;
	ofstream trackerOut;
	ofstream flitSeqOut;

	set<ULL> flitPool;
	ULL total_latency;
	ULL flit_recved;
	double avg_latency;
	double avg_throughput;
	
	void updateStat();
//////////////////////////////////////////////////////////////////////////
	//
	//
	void printOCsOverviewTitled(ostream & xout);
	void printTitleForOcOverview(ostream & xout);
	void printOcOverview(UI tileId, UI portId, ostream & xout);
	void printOcOverviewTitled(UI tileId, UI portId, ostream & xout);
	void printOCsOverview(UI tileId, ostream & xout);
	void printOCsOverviewTitled(UI tileId, ostream & xout);

	void printTitleForIcOverview(ostream & xout);
	void printIcOverview(UI tileId, UI portId, ostream & xout);
	void printIcOverviewTitled(UI tileId, UI portId, ostream & xout);
	void printICsOverview(UI tileId, ostream & xout);
	void printICsOverviewTitled(UI tileId, ostream & xout);

	void printPortTable(UI tileId, ostream & xout);

	void printIcVcs(ostream & xout);
	void printIcVcs(UI tileId, ostream & xout);
	void printIcVcs(UI tileId, UI portId, ostream & xout);
	void printIcVc(UI tileId, UI portId, UI vcId, ostream & xout);
	
	void printOcRegin(UI tileId, UI portId, ostream & xout);
	void printOcRegvc(UI tileId, UI portId, ostream & xout);
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//
	//
	void printFlit(flit & f, ostream & xout);
	bool searchAndPrintFlit(ULL sequence, ostream & xout);
	flit* searchFlitInTile(UI tileId, ULL sequence, ostream & xout);
	flit* searchFlitInIc(UI tileId, UI portId, ULL sequence, ostream & xout);
	flit* searchFlitInOc(UI tileId, UI portId, ULL sequence, ostream & xout);

	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//
	//
	void commandView(string content, vector<ULL> hier, ostream & xout);
	int isfout(vector<string> cmd, string & fileName, char & mode);
	vector<string> parseCmd();
	vector<ULL> parseHier(string hier);	// parse hierarchy statement like 1.2.3, return 1,2,3
	bool isEnterInCin();
	bool enterDebugerTile(UI tileId);
	void printDebugerHelp(UI pos, ostream & xout);
	OcProbe* getOcProbe(UI tileId, UI portId);
	IcProbe* getIcProbe(UI tileId, UI portId);
	NwtileProbe* getNwtileProbe(UI tileId);
	
	vector<vector<string> > checkDeadlock();
	void printDeadlock(ostream & xout);
	//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
};

#endif