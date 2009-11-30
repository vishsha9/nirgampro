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
	ULL flit_in;
	ULL pkt_in;

	float avg_inputRate;
	float avg_inputRate2;

	int beg_cycle;
	int end_cycle;


	ULL flit_in_inst;
	ULL pkt_in_inst;
	float inst_inputRate;

	void updateStat();
	void cleanStat();
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
	ULL pkt_out;			///< total number of packets
	ULL flit_out;			///< total number of flits
	ULL enterIc_time[NUM_VCS];			///< generation timestamp of head flit of a packet
	ULL enterNoc_time[NUM_VCS];

	ULL pkt_delay_sum;			///< total latency
	ULL flit_delay_sum;

	float avg_pkt_delay;		///< average latency (in clock cycles) per packet
	float avg_flit_delay;		///< average latency (in clock cycles) per flit

	float avg_outputRate;		///< average throughput (in Gbps)
	float avg_outputRate2;

	int beg_cycle;			///< clock cycle in which first flit is recieved in the channel
	int end_cycle;			///< clock cycle in which last flit leaves the channel

	ULL flit_out_inst;
	ULL pkt_out_inst;
	ULL pkt_delay_sum_inst;
	ULL flit_delay_sum_inst;
	float inst_pkt_delay;
	float inst_flit_delay;
	float inst_outputRate;

	void updateStat();
	void cleanStat();
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
	ULL total_flit_in;
	ULL total_flit_out;
	ULL	total_pkt_in;
	ULL	total_pkt_out;

	ULL total_pkt_delay;			///< total latency
	ULL total_flit_delay;

	float avg_inputRate;
	float avg_outputRate;		///< average throughput (in Gbps)
	float avg_pkt_delay;		///< average latency (in clock cycles) per packet
	float avg_flit_delay;		///< average latency (in clock cycles) per flit

	ULL inst_total_flit_in;
	ULL inst_total_flit_out;
	ULL inst_total_pkt_in;
	ULL inst_total_pkt_out;

	ULL inst_total_pkt_delay;			///< total latency
	ULL inst_total_flit_delay;

	float inst_inputRate;
	float inst_outputRate;
	float inst_pkt_delay;
	float inst_flit_delay;
//	ULL flit_send;
//	ULL pkt_send;
//	ULL flit_recved;
//	ULL pkt_recved;

	vector<ULL> flitFrom;
	vector<ULL> flitTo;
	vector<ULL> pktFrom;
	vector<ULL> pktTo;

	vector<ULL> flit_latency;
	vector<ULL> pkt_latency;
	vector<ULL> flit_hops;

	vector<ULL> avg_flit_latency;
	vector<ULL> avg_pkt_latency;
	vector<ULL> avg_flit_hops;

	ULL total_flit_latency;
	ULL total_pkt_latency;
	ULL total_flit_hops;

	ULL flit_latency_sum_inst;
	ULL pkt_latency_sum_inst;
	ULL flit_hops_sum_inst;

	void updateStat();
	void cleanStat();
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
	void printStat();
	void printRouteTables();
	void enterDebuger();
	void run();
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
	ofstream simResults;
	ofstream trackerOut;
	ofstream flitSeqOut;
	ofstream rtTable;
	
	ofstream timeTable;
	void printTitleForTimet(ostream & xout);
	//////////////////////////////////////////////////////////////////////////
	//set<ULL> flitPool;

	ULL total_flit_latency;
	ULL total_pkt_latency;
	ULL total_flit_hops;

	ULL total_flit_send;
	ULL total_pkt_send;
	ULL total_flit_recved;
	ULL total_pkt_recved;
	
	float avg_pkt_swDelay;		// package switch Delay
	float avg_flit_swDelay;		// flit switch delay

	float avg_flit_latency;		// flit latency
	float avg_pkt_latency;		// package latency
	float avg_flit_hops;

	float avg_inputRate;
	float avg_outputRate;
	

	ULL inst_total_flit_latency;
	ULL inst_total_pkt_latency;
	ULL inst_total_flit_hops;

	ULL inst_total_flit_send;
	ULL inst_total_pkt_send;
	ULL inst_total_flit_recved;
	ULL inst_total_pkt_recved;

	float inst_pkt_swDelay;		// package switch Delay
	float inst_flit_swDelay;		// flit switch delay
	float inst_flit_latency;
	float inst_pkt_latency;
	float inst_flit_hops;
	float inst_inputRate;
	float inst_outputRate;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	
	void updateStat();
	void cleanStat();
	//////////////////////////////////////////////////////////////////////////
	//
	//
	void printStat(ostream & xout);

	void printTitleForTlStat(ostream & xout);
	void printTileStat(UI tileId, ostream & xout);
	void printTileStatTitled(ostream & xout);
	void printTileStatTitled(UI tileId, ostream & xout);
	void printTileDataDis(UI tileId, ostream & xout);

	void printTitleForOcStat(ostream & xout);
	void printOcStat(UI tileId, UI portId, ostream & xout);
	void printOcStatTitled(UI tileId, UI portId, ostream & xout);
	void printOCsStat(UI tileId, ostream & xout);
	void printOCsStatTitled(UI tileId, ostream & xout);

	void printTitleForIcStat(ostream & xout);
	void printIcStat(UI tileId, UI portId, ostream & xout);
	void printIcStatTitled(UI tileId, UI portId, ostream & xout);
	void printICsStat(UI tileId, ostream & xout);
	void printICsStatTitled(UI tileId, ostream & xout);

	void printPortTable(UI tileId, ostream & xout);
	void printVaVcFree(UI tileId, ostream & xout);
	void printRoutePath(UI src, UI dest, ostream & xout);
	void printRouteTable(UI tileId, ostream & xout);

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