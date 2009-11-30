#ifndef __TRACKER_H__
#define __TRACKER_H__

#include "../flit.h"
#include "../../global.h"

#include "systemc.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <map>

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
	
	IcProbe();

	//////////////////////////////////////////////////////////////////////////
	ULL flits_in;
	ULL pkts_in;

	float avg_inputRate;
	float avg_inputRate2;

	int beg_cycle;
	int end_cycle;

	void updateStat();
	//////////////////////////////////////////////////////////////////////////
};

class OcProbe{
public:
	UI portId;
	vector<switch_reg> * r_in;
	vector<switch_reg> * r_vc;
	OutputChannel * ocPtr;

	OcProbe();

	//////////////////////////////////////////////////////////////////////////
	ULL pkts_out;			///< total number of packets
	ULL flits_out;			///< total number of flits
	ULL enterIc_time[NUM_VCS];			///< generation timestamp of head flit of a packet
	ULL enterNoc_time[NUM_VCS];

	ULL total_pkt_delay;			///< total latency
	ULL total_flit_delay;

	float avg_pkt_delay;		///< average latency (in clock cycles) per packet
	float avg_flit_delay;		///< average latency (in clock cycles) per flit

	float avg_outputRate;		///< average throughput (in Gbps)
	float avg_outputRate2;

	int beg_cycle;			///< clock cycle in which first flit is recieved in the channel
	int end_cycle;			///< clock cycle in which last flit leaves the channel

	void updateStat();
	//////////////////////////////////////////////////////////////////////////
};


class NwtileProbe{
public:
	NwtileProbe();

	UI tileId;
	int io_num;
	vector<IcProbe> icProbes;
	vector<OcProbe> ocProbes;
	
	vector<vector<bool> > * VA_vcFree;
	
	map<int, int> * rtTable;
	vector<UI> * portTable;
	NWTile * tilePtr;

	//////////////////////////////////////////////////////////////////////////
	ULL total_flits_in;
	ULL	total_flits_out;
	ULL	total_pkts_in;
	ULL	total_pkts_out;

	ULL total_pkt_delay;			///< total latency
	ULL total_flit_delay;

	float avg_inputRate;
	float avg_outputRate;		///< average throughput (in Gbps)
	float avg_pkt_delay;		///< average latency (in clock cycles) per packet
	float avg_flit_delay;		///< average latency (in clock cycles) per flit

	vector<ULL> flitFrom;
	vector<ULL> flitTo;
	vector<ULL> pktFrom;
	vector<ULL> pktTo;

	void updateStat();
	//////////////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////////
	//set<ULL> flitPool;

	ULL total_flit_latency;
	ULL total_pkt_latency;
	ULL total_flit_hops;

	ULL flits_send;
	ULL pkts_send;
	ULL flits_recved;
	ULL pkts_recved;
	
	float avg_pkt_swDelay;		// package switch Delay
	float avg_flit_swDelay;		// flit switch delay

	float avg_flit_latency;		// flit latency
	float avg_pkt_latency;		// package latency
	float avg_flit_hops;

	float avg_inputRate;
	float avg_outputRate;
	
	//////////////////////////////////////////////////////////////////////////
	
	void updateStat();
	//////////////////////////////////////////////////////////////////////////
	//
	//
	void printTileStat(ostream & xout);
	void printTileStat(UI tileId, ostream & xout);

	void printTitleForOcOverview(ostream & xout);
	void printOcOverview(UI tileId, UI portId, ostream & xout);
	void printOcOverviewTitled(UI tileId, UI portId, ostream & xout);
	void printOCsOverview(UI tileId, ostream & xout);
	void printOCsOverviewTitled(ostream & xout);
	void printOCsOverviewTitled(UI tileId, ostream & xout);

	void printTitleForIcOverview(ostream & xout);
	void printIcOverview(UI tileId, UI portId, ostream & xout);
	void printIcOverviewTitled(UI tileId, UI portId, ostream & xout);
	void printICsOverview(UI tileId, ostream & xout);
	void printICsOverviewTitled(ostream & xout);
	void printICsOverviewTitled(UI tileId, ostream & xout);

	void printPortTable(UI tileId, ostream & xout);
	void printVaVcFree(UI tileId, ostream & xout);
	void printRoutePath(UI src, UI dest, ostream & xout);
	void printRouteTable(UI tileId, ostream & xout);

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