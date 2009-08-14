#include <stdlib.h>
#include <stdio.h>


#include "NoC.h"
//#include "systemc.h"

sc_trace_file *tracefile;	// use to generate vcd trace
sc_clock *nw_clock;

double CLK_FREQ = 1;			///< clock frequency (in GHz)
double CLK_PERIOD = (1/CLK_FREQ);	///< clock period (in ns)

ofstream readlog;
ofstream writelog;
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
ULL SIM_NUM = 30;	///< Simulation cycles
ULL TG_NUM = 1000;	///< clock cycles until which traffic is generated

string* app_libname;
int num_tiles;

void loadApp(int num_tiles){
	app_libname = new string[num_tiles];
	for(int i = 0; i < num_tiles; i++) {
		app_libname[i] = string("NULL");
	}
	string app_filename = string("config/application.config");
	ifstream app_fil;
	app_fil.open(app_filename.c_str());
	if(!app_fil.is_open()){
		cout<<"Error: File application.config could not be opened." << endl;
		exit(-1);
	}
	int id;
	string libname;
	while(app_fil >> id >> libname) {
		app_libname[id] = libname;
		if(app_fil.fail()){
			cout<<"Error: Load Application Config Error." << endl;
			exit(-1);
		}
	}
	app_fil.close();
}

int sc_main(int argc, char *argv[]) {
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
	cout<<"---------------------------------------------------------------------------"<<endl;
	cout<<"  NIRGAM: Simulator for NoC lite Interconnect RoutinG and Application Modeling\n";
	cout<<"---------------------------------------------------------------------------"<<endl;

	eventlog.open("eventlog.log");
	if(!eventlog.is_open()){
		cout<<"cannot open eventlog.log"<< endl;
	}

	readlog.open("readlog.log");
	if(!readlog.is_open()){
		cout<<"cannot open readlog.log"<<endl;
	}
	writelog.open("writelog.log");
	if(!writelog.is_open()){
		cout<<"cannot open writelog.log"<<endl;
	}
	delaylogin.open("delaylogin.log");
	if(!delaylogin.is_open()){
		cout<<"cannot open delaylogin.log"<<endl;
	}
	delaylogout.open("delaylogout.log");
	if(!delaylogout.is_open()){
		cout<<"cannot open delaylogout.log"<<endl;
	}

	nw_clock = new sc_clock("NW_CLOCK",CLK_PERIOD,SC_NS);	// create global clock

	AdjList* a = analyze("a.nfn", cout);
	num_tiles = a->nodeNum;
	
	loadApp(num_tiles);

	NoC noc("noc", a);
	noc.switch_cntrl(*nw_clock);
	
	string trace_filename = string("trace");
	tracefile = sc_create_vcd_trace_file(trace_filename.c_str());

	sc_trace(tracefile, noc.switch_cntrl, "clk");
	sc_start();	// begin simulation
	sc_close_vcd_trace_file(tracefile); 

	readlog.close();
	writelog.close();
	delaylogin.close();
	delaylogout.close();

	eventlog.close();

	return 0;
}
