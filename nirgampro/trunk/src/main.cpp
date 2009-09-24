#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "core/noc.h"
#include "extern.h"

#include "core/tracker/tracker.h"
//#include "systemc.h"

sc_trace_file *tracefile;	// use to generate vcd trace

string app_filename;



//////////////////////////////////////////////////////////////////////////
ofstream flit_latency;
ULL flit_num = 0;	    // format
ULL flit_latency_total = 0; // format
set<ULL>* sent_flit; // format added
ULL* last_sent_flit; // format added
ULL* flit_recved;    // format added
//////////////////////////////////////////////////////////////////////////

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
		if ( id >= num_tiles )
			break;
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
	for (int i=0; i< g_tileNum*g_tileNum; i++)
	{
		int src = i / g_tileNum;
		int dest = i % g_tileNum;
		if (g_ckTable[i] == -1){
			cout << "Path from "<< src <<" to " << dest <<" is abnormal" << endl;
			selfCorrect = false;
		}
		else;
		//cout << "Path from "<< src <<" to " << dest <<" is normal" << endl;
	}
	return selfCorrect;
} 

void loadSelfcheckApp(int g_tileNum){
	for(int i=0; i<g_tileNum; i++){
		g_appLibName.push_back(string("selfCheck.so"));
	}
}

void showConfig(){
	cout << "Config directory set to: " << g_configDir << endl;
	cout << "Topology definition file path: " << g_topoFile << endl;
	cout << "Result directory set to: " << g_resultDir << endl;
	if (g_isCheckMode == true)
		cout << "Self Check Mode enabled" << endl;
	else
		cout << "Normal Mode enabled" << endl;
	cout << "SIM_TIME: " << g_simNum << endl;
	cout << "g_tgNum: " << g_tgNum << endl;
	cout << "g_calNum: " << g_calNum << endl;
	cout << "g_warmUp: " << g_warmUp << endl;
	cout << "g_clkFreq: " << g_clkFreq << endl;
	cout << "NUM_VCS: " << NUM_VCS <<  endl;
	cout << "LOG: " << LOG << endl;
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

void makeDirectory(string path){
	ifstream tmp;
	tmp.open(path.c_str());
	if(!tmp.is_open()){
		string cmd = "mkdir " + stringPath(path);
		system(cmd.c_str());
	}
	tmp.close();
}

string rewriteDirectory(string path){
	if (path.at(path.size()-1) == '/' || path.at(path.size()-1) == '\\')
		return path;
	else
		return path + "/";
}

void printCheckTable(){
	ofstream fff;
	string filename = "checkTable";
	filename = g_resultDir + filename;
	fff.open(filename.c_str());
	for (int i=0; i< g_tileNum; i++)
	{
		for (int j=0; j<g_tileNum; j++)
		{
			fff << g_ckTable[i*g_tileNum+j]<<"\t";
		}
		fff << "\n";
	}
	fff.close();
}
void readSimulationConfig(){
	// read simulator configuration (nirgam.config)
	string nirgam_filename = g_configDir + string("nirgam.config");
	ifstream fil1;
	fil1.open(nirgam_filename.c_str());

	if(!fil1.is_open()){
		cout<<"nirgam.config does not exist, using default parameter values."<<endl;
	}
	else{
		cout <<"\nLoading parameters from nirgam.config file...";
		while(!fil1.eof()){
			string name;
			fil1 >> name ;
			if(name=="LOG"){
				int value; fil1 >> value; LOG = value;
			}
			else if(name=="SIM_NUM"){
				ULL value; fil1 >> value; g_simNum = value;
			}
			else if(name=="WARMUP"){
				ULL value; fil1 >> value; g_warmUp = value;
			}
			else if(name=="TG_NUM"){
				ULL value; fil1 >> value; g_tgNum = value;
			}
			else if(name=="CAL_NUM"){ // format added, new variable
				ULL value; fil1 >> value; g_calNum = value;
			}
			else if(name=="RT_ALGO"){
				fil1 >> name;
				if(name == "XY")
					RT_ALGO = XY; 
				else if(name == "OE")
					RT_ALGO = OE;
				else if(name == "SOURCE")
					RT_ALGO = SOURCE;
			}
			else if(name=="NUM_BUFS"){
				int value; fil1 >> value; NUM_BUFS = value;
			}
			else if(name=="FLITSIZE"){
				int value; fil1 >> value; FLITSIZE = value;
			}
			else if(name=="HEAD_PAYLOAD"){
				int value; fil1 >> value; HEAD_PAYLOAD = value;
			}
			else if(name=="DATA_PAYLOAD"){
				int value; fil1 >> value; DATA_PAYLOAD = value;
			}
			else if(name=="CLK_FREQ"){
				double value; fil1 >> value; g_clkFreq = value;
				g_clkPeriod = (1/g_clkFreq);
			}
		}
		cout<<"done"<<endl;
	}
	fil1.close();
}

void initFormatsStat(int size){
	sent_flit = new set<ULL>[size];
	last_sent_flit = new ULL[size];
	flit_recved = new ULL[size];

	string flit_latency_file = g_resultDir + string("flit_latency.log");
	flit_latency.open(flit_latency_file.c_str());
	if(!flit_latency.is_open()){
		cout<<"cannot open flit_latency.log"<<endl;
	}
}

void writeResult(){
	//////////////////////////// flit latency calculte
	// format
	UL recved_flits_num = flit_num;
	g_resultsLog<<"\nOverall recved flit num = "<< recved_flits_num << endl;
	for(int i = 0 ;i<g_tileNum; i++){
		// caculate one tile 

		// caculte unrecved flits on the path
		// because the g_simNum is much great than g_calNum, flits on the uncrowd path should be all recved, 
		// flits on path have a very large latency
		set<ULL> ::iterator si;
		UL ct=0;
		ULL sum_lat=0;
		for(si = sent_flit[i].begin(); si!=sent_flit[i].end(); si++){
			ct++;
			ULL latency =  g_simNum - *si;
			sum_lat += latency;
			flit_latency<<"src "<< i << ", dest UNKNOWN" <<", latency "<< latency << ", sendt "<< *si <<",recved S_ON_PATH_FLITS"<< endl;
		}

		// caculte unsent flits, their latency approximately is (g_simNum - due_send_time)
		/*if(last_sent_flit[i] < g_calNum){
			char str_id[3];
			sprintf(str_id, "%d", i);
			string traffic_filename = resultDir + string("traffic/tile-") + string(str_id);
			ifstream trafstream;
			trafstream.open(traffic_filename.c_str());

			ULL pkt_start_due = 0;
			string field;
			int flit_interval;
			int next_pkt_time;
			int num_flits;
			int dest;	
			// write inter-flit interval
			trafstream>>field>>flit_interval;
			trafstream>>field>> pkt_start_due;

			while(pkt_start_due < g_calNum){
				trafstream >> field >> pkt_start_due;
				// read inter-pkt interval
				trafstream >> field >> next_pkt_time;
				// read no. of flits in packet
				trafstream >> field >> num_flits;
				// read destination or route code
				trafstream >> field >> dest;
				if(pkt_start_due > last_sent_flit[i]){
					ct += num_flits;
					for(int j=0; j< num_flits; j++){
						//flit_latency<<"src "<< i << ", dest "<< dest <<", latency "<< g_simNum - pkt_start_due - i*flit_interval << endl;
						flit_latency<<"src "<< j << ", dest "<< dest <<", latency "<< g_simNum - last_sent_flit[i] << ", sendt UNSENT"<<", recved UNSENT_FILTS"<<endl;
						//sum_lat += g_simNum - pkt_start_due - i*flit_interval;
						sum_lat += g_simNum - last_sent_flit[i];
					}
				}
			}
		}*/ // @ end of unsent
		flit_num += ct;
		flit_latency_total += sum_lat;
	}

	g_resultsLog<<"\nOverall cal-latency flit num = "<< flit_num << endl;
	g_resultsLog<<"\nOverall total flit latency = "<< flit_latency_total << endl;
	g_resultsLog<<"\nOverall average flit latency (in clock cycles per flit) = "<< flit_latency_total/flit_num << endl;

	g_resultsLog<<"\nOverall average flit throughput (Gbps) = "<< recved_flits_num*FLITSIZE*8.0/(g_calNum*g_tileNum) << endl;
	////////////////////////////
}

void openResultFile(){
	string file = g_resultDir + "eventlog.log";
	eventlog.open(file.c_str());
	if(!eventlog.is_open()){
		cout<<"cannot open eventlog.log"<< endl;
	}

	file = g_resultDir + "delaylogin.log";
	delaylogin.open(file.c_str());
	if(!delaylogin.is_open()){
		cout<<"cannot open delaylogin.log"<<endl;
	}

	file = g_resultDir + "delaylogout.log";
	delaylogout.open(file.c_str());
	if(!delaylogout.is_open()){
		cout<<"cannot open delaylogout.log"<<endl;
	}

	string results_file= g_resultDir + string("/sim_results");
	g_resultsLog.open(results_file.c_str());
	if(!g_resultsLog.is_open())
		cout<<"Cannot open "<<results_file.c_str()<<endl;
}

void closeResultFile(){
	delaylogin.close();
	delaylogout.close();
	g_resultsLog.close();
	eventlog.close();
	flit_latency.close();
}

int sc_main(int argc, char *argv[]) {
	cout<<"---------------------------------------------------------------------------"<<endl;
	cout<<"  NIRGAM: Simulator for NoC lite Interconnect RoutinG and Application Modeling\n";
	cout<<"---------------------------------------------------------------------------"<<endl;

	// default configuration
	{
		g_isStepMode = false;
		g_isCheckMode = false;
		g_configDir = "config/";
		g_resultDir = "config/result/";
		g_topoFile = "config/topo.nfn";
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
			g_isCheckMode = true;
		}
		else if (args == "-s")
		{
			g_isStepMode = true;
		}
		else if (args == "-i") // configDir
		{
			i++;
			if (i<argc)
				g_configDir = rewriteDirectory(string(argv[i]));
			else {
				cout << "Argument to '-i' is missing" << endl; 
				return 0;
			}
		}
		else if (args == "-r")	// resultDir
		{
			i++;
			if (i<argc)
				g_resultDir = rewriteDirectory(string(argv[i]));
			else{
				cout << "Argument to '-r' is missing" << endl; 
				return 0;
			}
		}
		else if (args == "-t") // nestlist_file
		{
			i++;
			if (i<argc)
				g_topoFile = string(argv[i]);
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

	readSimulationConfig();

	makeDirectory(g_resultDir);
	
	app_filename = g_configDir + string("application.config");
	
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);

	system("set SC_SIGNAL_WRITE_CHECK=\"DISABLE\"");

	openResultFile();

	nw_clock = new sc_clock("NW_CLOCK",g_clkPeriod,SC_NS);	// create global clock

	g_a = analyze(g_topoFile, cout);
	if(g_a == NULL){
		cout << "Error occurred. Exit Simulation" << endl;
		exit(0);
	}
	g_tileNum = g_a->nodeNum;
	

	initFormatsStat(g_tileNum);

	g_ckTable = new int[g_tileNum * g_tileNum];
	for (int i=0; i<g_tileNum * g_tileNum; i++)
		g_ckTable[i] = -1;
		
	string trace_filename = g_resultDir + string("trace");
	tracefile = sc_create_vcd_trace_file(trace_filename.c_str());
	
	if(g_isCheckMode){
		loadSelfcheckApp(g_tileNum);
		g_simNum = g_tileNum * g_tileNum + 200;
		g_tgNum = g_simNum;
	}
	else
		loadApp(g_tileNum);
		
	

	showConfig();
	
	NoC noc("noc", g_a);

	g_tracker = new Tracker();
	g_tracker->addProbes(noc);

	//g_noc = &noc;
	noc.switch_cntrl(*nw_clock);

	
	sc_trace(tracefile, noc.switch_cntrl, "clk");
	for (int i=0; i<noc.sigs.size(); i++)
	{
		char name[50];
		sprintf(name, "wire_from_%d_to_%d", noc.sigs[i]->tileID_1, noc.sigs[i]->tileID_2);
		sc_trace(tracefile, noc.sigs[i]->sig_from1, name);
		sprintf(name, "wire_from_%d_to_%d", noc.sigs[i]->tileID_2, noc.sigs[i]->tileID_1);
		sc_trace(tracefile, noc.sigs[i]->sig_from2, name);
	}

	if (g_isStepMode == false)
		sc_start();	// begin simulation
	else{
		g_simNum = 0;
		g_stepSimNum = 0;
		sc_start(0, SC_FS);
		while (true)
		{
			cout << "Input simulation cycles or duration: ";
			string timestr;
			cin >> timestr;
			int val = atoi(timestr.c_str());
			int index = strspn(timestr.c_str(), "0123456789");
			if(index == timestr.size()){
				cout << "Simulation continue for " << val << " Cycle" << endl;
				g_stepSimNum = val;
				g_simNum += g_stepSimNum;
				sc_start(val*g_clkPeriod, SC_NS);
			}
			else if (index != 0)
			{
				char c = timestr.at(index);
				if (c == 'f' || c == 'F'){
					g_stepSimNum = val/( 1000*1000*g_clkPeriod); 
					cout << "Simulation continue for " << val << " fs, " <<g_stepSimNum << " Cycle"<< endl;
					sc_start(val, SC_FS);
				}
				else if (c == 'p' || c == 'P'){
					g_stepSimNum = val/( 1000*g_clkPeriod);
					cout << "Simulation continue for " << val << " ps, " << g_stepSimNum << " Cycle"<< endl;
					sc_start(val, SC_PS);
				}
				else if (c == 'n' || c == 'N'){
					g_stepSimNum = val / g_clkPeriod;
					cout << "Simulation continue for " << val << " ns, " << g_stepSimNum << " Cycle" << endl;
					sc_start(val, SC_NS);
				}
				else if (c == 'u' || c == 'U'){
					g_stepSimNum = val / g_clkPeriod * 1000;
					cout << "Simulation continue for " << val << " us, " << g_stepSimNum << " Cycle" << endl;
					sc_start(val, SC_US);
				}
				else if (c == 'm' || c == 'M'){
					g_stepSimNum = val / g_clkPeriod * 1000 * 1000;
					cout << "Simulation continue for " << val << " ms, " << g_stepSimNum << " Cycle" << endl;
					sc_start(val, SC_MS);
				}
				else if (c == 's' || c == 'S'){
					g_stepSimNum = val / g_clkPeriod * 1000 * 1000 * 1000;
					cout << "Simulation continue for " << val << " sec, " << g_stepSimNum << " Cycle" << endl; 
					sc_start(val, SC_SEC);
				}
				else{
					g_stepSimNum = 0;
					cout << "Input Error. Please input cycle number or duration of simultion. Enter quit to quit." << endl;
				}
				g_simNum += g_stepSimNum;
			}
			else {
				if (strcasecmp(timestr.c_str(), "quit") == 0 ||
					strcasecmp(timestr.c_str(), "q") == 0 ||
					strcasecmp(timestr.c_str(), "exit") == 0 )
					break;
				else if(strcasecmp(timestr.c_str(), "d") == 0)
					g_tracker->enterDebuger();
				else
					cout << "Input Error. Please input cycle number or duration of simultion. Enter quit to quit." << endl;
			}
			cout << "Sim Count:" << g_simCount <<  endl;
		}
	}
	if(g_simCount == 0){
		cout << "Not simulated, No result generated" << endl;
		return 0;
	}
	//noc.closeLogs();
	g_tracker->printOcOverviewAll(g_resultsLog);

	if(g_isCheckMode){
		if(doSelfCheck())
			cout << "Network is ok" << endl;
		else{
			cout << "Bad network" << endl;
			
		}
		printCheckTable();
		exit(0);
	}

	writeResult();
	sc_close_vcd_trace_file(tracefile); 
	
	closeResultFile();

	return 0;
}
