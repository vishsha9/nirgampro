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

int sc_main(int argc, char *argv[]) {

	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
	cout<<"---------------------------------------------------------------------------"<<endl;
	cout<<"  NIRGAM: Simulator for NoC lite Interconnect RoutinG and Application Modeling\n";
	cout<<"---------------------------------------------------------------------------"<<endl;

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

	AdjList* a;

	if(argc > 1){
		a = analyze(string(argv[argc-1]), cout);
	}else
		a = analyze("a.nfn", cout);

	if(a == NULL){
		cout << "Error occurred. Exit Simulation" << endl;
		exit(0);
	}
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

	return 0;
}
