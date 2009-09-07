#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "core/noc.h"
#include "extern.h"
//#include "systemc.h"

sc_trace_file *tracefile;	// use to generate vcd trace

bool isCheckMode;
string configDir;
string resultDir;
string topoFile;
string app_filename;


void loadApp(int num_tiles){
	for(int i = 0; i < num_tiles; i++) {
		g_appLibName.push_back(string("NULL"));
	}
	ifstream app_fil;
	app_fil.open(app_filename.c_str());
	if(!app_fil.is_open()){
		cout<<"Error: File application.config could not be opened." << endl;
		exit(-1);
	}
	int id;
	string libname;
	while(app_fil >> id >> libname) {
		g_appLibName[id] = libname;
		if(app_fil.fail()){
			cout<<"Error: Load Application Config Error." << endl;
			exit(-1);
		}
	}
	app_fil.close();
}

bool doSelfCheck(){
	bool selfCorrect = true;
	for (int i=0; i< num_tiles*num_tiles; i++)
	{
		int src = i / num_tiles;
		int dest = i % num_tiles;
		if (g_ckTable[i] == -1){
			cout << "Path from "<< src <<" to " << dest <<" is abnormal" << endl;
			selfCorrect = false;
		}
		else;
		//cout << "Path from "<< src <<" to " << dest <<" is normal" << endl;
	}
	return selfCorrect;
} 

void loadSelfcheckApp(int num_tiles){
	for(int i=0; i<num_tiles; i++){
		g_appLibName.push_back(string("selfCheck.so"));
	}
}

void showConfig(){
	cout << "Config directory set to: " << configDir << endl;
	cout << "Topology definition file path: " << topoFile << endl;
	cout << "Result directory set to: " << resultDir << endl;
	if (isCheckMode == true)
		cout << "Self Check Mode enabled" << endl;
	else
		cout << "Normal Mode enabled" << endl;
	cout << endl;
}

string convertPathToWin32(string path){
	int pos;
	while ((pos = path.find_first_of('/')) != string::npos)
	{
		path.replace(pos, 1, "\\");
	}
	return path;
}

void makeDirectory(){
	ifstream tmp;
	tmp.open(resultDir.c_str());
	if(!tmp.is_open()){
		string cmd = "mkdir " + stringPath(resultDir);
		system(cmd.c_str());
	}
}

int sc_main(int argc, char *argv[]) {
	cout<<"---------------------------------------------------------------------------"<<endl;
	cout<<"  NIRGAM: Simulator for NoC lite Interconnect RoutinG and Application Modeling\n";
	cout<<"---------------------------------------------------------------------------"<<endl;


	// default configuration
	{
		isCheckMode = false;
		configDir = "config/";
		resultDir = configDir + "result/";
		topoFile = configDir + "topo.nfn";
	}
	
	for (int i =1; i< argc; i++)
	{
		string args = string(argv[i]);
		if (args == "-h")
		{
			cout << "Nirgam help" << endl;
			cout << "-c\t\tEnable check mode" << endl;
			cout << "-i [path]\tSet configuration directory path" << endl;
			cout << "-r [filename]\tSet result directory name" << endl;
			cout << "-t [filename]\tTopology file name" << endl;
			return 0;
		}
		else if (args == "-c")
		{
			isCheckMode = true;
		}
		else if (args == "-i") // configDir
		{
			i++;
			if (i<argc)
				configDir = string(argv[i]) + "/";
			else {
				cout << "Argument to '-i' is missing" << endl; 
				return 0;
			}
		}
		else if (args == "-r")	// resultDir
		{
			i++;
			if (i<argc)
				resultDir = configDir + string(argv[i]) + "/";
			else{
				cout << "Argument to '-r' is missing" << endl; 
				return 0;
			}
		}
		else if (args == "-t") // nestlist_file
		{
			i++;
			if (i<argc)
				topoFile = configDir + string(argv[i]);
			else{
				cout << "Argument to '-t' is missing" << endl; 
				return 0;
			}
		}
		else{
			cout << args <<"Recognized option" << endl;
			return 0;
		}
	}

	makeDirectory();
	showConfig();
	
	app_filename = configDir + string("application.config");

	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);

	system("set SC_SIGNAL_WRITE_CHECK=\"DISABLE\"");

	string file = resultDir + "eventlog.log";
	eventlog.open(file.c_str());
	if(!eventlog.is_open()){
		cout<<"cannot open eventlog.log"<< endl;
	}

	file = resultDir + "delaylogin.log";
	delaylogin.open(file.c_str());
	if(!delaylogin.is_open()){
		cout<<"cannot open delaylogin.log"<<endl;
	}

	file = resultDir + "delaylogout.log";
	delaylogout.open(file.c_str());
	if(!delaylogout.is_open()){
		cout<<"cannot open delaylogout.log"<<endl;
	}

	nw_clock = new sc_clock("NW_CLOCK",CLK_PERIOD,SC_NS);	// create global clock

	g_a = analyze(topoFile, cout);
	if(g_a == NULL){
		cout << "Error occurred. Exit Simulation" << endl;
		exit(0);
	}
	num_tiles = g_a->nodeNum;


	g_ckTable = new int[num_tiles * num_tiles];
	for (int i=0; i<num_tiles * num_tiles; i++)
		g_ckTable[i] = -1;

	if(isCheckMode)
		loadSelfcheckApp(num_tiles);
	else
		loadApp(num_tiles);
	NoC noc("noc", g_a);

	//g_noc = &noc;
	noc.switch_cntrl(*nw_clock);

	string trace_filename = resultDir + string("trace");
	tracefile = sc_create_vcd_trace_file(trace_filename.c_str());

	sc_trace(tracefile, noc.switch_cntrl, "clk");
	for (int i=0; i<noc.sigs.size(); i++)
	{
		string name =	string("wire_from_") +
			string(1, noc.sigs[i]->tileID_1+'0') +
			string("_to_") + 
			string(1, noc.sigs[i]->tileID_2+'0');
		sc_trace(tracefile, noc.sigs[i]->sig_from1, name);
		name =	string("wire_from_") +
			string(1, noc.sigs[i]->tileID_2+'0') +
			string("_to_") + 
			string(1, noc.sigs[i]->tileID_1+'0');
		sc_trace(tracefile, noc.sigs[i]->sig_from2, name);
	}
	sc_start();	// begin simulation


	if(isCheckMode){
		if(doSelfCheck())
			cout << "Network is ok" << endl;
		else{
			cout << "Bad network" << endl;
			for (int i=0; i< num_tiles; i++)
			{
				for (int j=0; j<num_tiles; j++)
				{
					cout << g_ckTable[i*num_tiles+j]<<"\t";
				}
				cout << "\n";
			}
		}
		exit(0);
	}

	sc_close_vcd_trace_file(tracefile); 

	delaylogin.close();
	delaylogout.close();

	eventlog.close();

	return 0;
}
