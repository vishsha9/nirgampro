#include "global.h"
#include "dirtool.h"
#include "core/topoAnalyzer.h"
#include "cfgLoader.h"
#include "core/noc.h"
#include "core/tracker/tracker.h"


#ifdef SL_TILE
#include "core/nwtile/slTile.h"
void loadApp(int num_tiles){
	loadSocLibConfig();
}

#else
void loadApp(int num_tiles){
	for(int i = 0; i < num_tiles; i++) {
		g_appLibName.push_back(string("NULL"));
	}
	ifstream app_fil;
	app_fil.open(gc_appFile.c_str());
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
#endif
//////////////////////////////////////////////////////////////////////////

void loadSelfcheckApp(int tileNum){
	for(int i=0; i<tileNum; i++){
		g_appLibName.push_back(string("selfCheck.so"));
	}
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

void printCheckTable(ostream & xout){
	string filename = "checkTable";
	for (int i=0; i< g_tileNum; i++)
	{
		for (int j=0; j<g_tileNum; j++)
		{
			xout << g_ckTable[i*g_tileNum+j]<<"\t";
		}
		xout << "\n";
	}
}
//////////////////////////////////////////////////////////////////////////

bool initStandardMode(){
	loadApp(g_tileNum);
}

bool initDebugMode(){
	loadApp(g_tileNum);
	gc_simNum = 0;
	g_simExtNum = 0;
}
 
bool initSelfcheckMode(){
	loadSelfcheckApp(g_tileNum);
	g_ckTable = new int[g_tileNum * g_tileNum];
	for (int i=0; i<g_tileNum * g_tileNum; i++)
		g_ckTable[i] = -1;
	gc_simNum = g_tileNum * g_tileNum + 300;
	g_simPeriod = gc_simNum * g_clkPeriod; 
}

//////////////////////////////////////////////////////////////////////////

bool doStandardMode(){
	sc_start(g_simPeriod, SC_NS);
	cout << endl;
}


bool isEnterInCin(){
	char firstc;
	do
	{
		cin.get(firstc);
		if (firstc == '\n'){
			cin.putback(firstc);
			return true;
		}
	}
	while (isspace(firstc));
	cin.putback(firstc);
	return false;
}
#include <limits>
bool doDebugMode(){
	while (true)
	{
		string timestr;
		cout << "Input simulation cycles or duration: ";
		while(!isEnterInCin()){
			cin >> timestr;
		}
		if(cin.fail()){
			cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
			continue;
		}
		int val = atoi(timestr.c_str());
		cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
		int index = strspn(timestr.c_str(), "0123456789");
		if(index == timestr.size()){
			cout << "Simulation continue for " << val << " Cycle" << endl;
			g_simExtNum = val;
			gc_simNum += g_simExtNum;
			sc_start(val*g_clkPeriod, SC_NS);
		}
		else if (index != 0)
		{
			char c = timestr.at(index);
			if (c == 'f' || c == 'F'){
				g_simExtNum = val/( 1000*1000*g_clkPeriod); 
				cout << "Simulation continue for " << val << " fs, " <<g_simExtNum << " Cycle"<< endl;
				sc_start(val, SC_FS);
			}
			else if (c == 'p' || c == 'P'){
				g_simExtNum = val/( 1000*g_clkPeriod);
				cout << "Simulation continue for " << val << " ps, " << g_simExtNum << " Cycle"<< endl;
				sc_start(val, SC_PS);
			}
			else if (c == 'n' || c == 'N'){
				g_simExtNum = val / g_clkPeriod;
				cout << "Simulation continue for " << val << " ns, " << g_simExtNum << " Cycle" << endl;
				sc_start(val, SC_NS);
			}
			else if (c == 'u' || c == 'U'){
				g_simExtNum = val / g_clkPeriod * 1000;
				cout << "Simulation continue for " << val << " us, " << g_simExtNum << " Cycle" << endl;
				sc_start(val, SC_US);
			}
			else if (c == 'm' || c == 'M'){
				g_simExtNum = val / g_clkPeriod * 1000 * 1000;
				cout << "Simulation continue for " << val << " ms, " << g_simExtNum << " Cycle" << endl;
				sc_start(val, SC_MS);
			}
			else if (c == 's' || c == 'S'){
				g_simExtNum = val / g_clkPeriod * 1000 * 1000 * 1000;
				cout << "Simulation continue for " << val << " sec, " << g_simExtNum << " Cycle" << endl; 
				sc_start(val, SC_SEC);
			}
			else{
				g_simExtNum = 0;
				cout << "Input Error. Please input cycle number or duration of simultion. Enter quit to quit." << endl;
			}
			gc_simNum += g_simExtNum;
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
		cout << endl;
		cout << "Sim Count:" << g_simCount <<  endl;
	}
}

bool doSelfcheckMode(){
	sc_start(g_simPeriod, SC_NS);
	cout << endl;
}
//////////////////////////////////////////////////////////////////////////

bool finaStandardMode(){
	ofstream fff;
	string filename = "sim_results";
	filename = gc_resultHome + filename;
	fff.open(filename.c_str());
	g_tracker->printStat(fff);
	fff.close();
}

bool finaDebugMode(){
	ofstream fff;
	string filename = "sim_results";
	filename = gc_resultHome + filename;
	fff.open(filename.c_str());
	g_tracker->printStat(fff);
	fff.close();
}

bool finaSelfcheckMode(){
	if(doSelfCheck())
		cout << "Network is ok" << endl;
	else
		cout << "Bad network" << endl;
	{
		ofstream fff;
		string filename = "checkTable";
		filename = gc_resultHome + filename;
		fff.open(filename.c_str());
		printCheckTable(fff);
		fff.close();
	}
}

//////////////////////////////////////////////////////////////////////////

void printHeader(){
	cout<<"---------------------------------------------------------------------------"<<endl;
	cout<<"  NIRGAM: Simulator for NoC Interconnect RoutinG and Application Modeling\n";
	cout<<"---------------------------------------------------------------------------"<<endl;
}

int sc_main(int argc, char ** argv){
	sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
	system("set SC_SIGNAL_WRITE_CHECK=\"DISABLE\"");

	printHeader();

	for (int i = 1; i< argc; i++)
	{
		string args = string(argv[i]);
		if (args == "-f")
		{
			i++;
			if (i<argc && argv[i][0] !='-')
				ga_configPath = string(argv[i]);
			else {
				cout << "Argument to '-f' is missing" << endl; 
				return 0;
			}
		}
		else if (args == "-e")
		{
			i++;
			if (i<argc && argv[i][0] !='-')
				ga_elfPath = string(argv[i]);
			else {
				cout << "Argument to '-e' is missing" << endl; 
				return 0;
			}
		}
		else{
			cout << args <<" Recognized option" << endl;
			return 0;
		}
	}
	
	char nirgamHomeTempStr[100];
	g_nirgamHome = rwDirInLinuxStyl(string(getcwd(nirgamHomeTempStr, 100)));
	g_configHome = getDirOfFile(ga_configPath);
	if(loadConfig(ga_configPath) == false)
		return 1;
	if(checkConfig() ==  false)
		return 1;
	printConfig(cout);

	tryDir(gc_resultHome);

	g_clkPeriod = 1/gc_simFreq;
	g_simPeriod = g_clkPeriod * gc_simNum;
	g_clock = new sc_clock("NW_CLOCK",g_clkPeriod, SC_NS);
	g_a = analyze(gc_topoFile, cout);	
	if(g_a == NULL){
		cout << "Error occurred. Exit Simulation" << endl; return 1;
	}
	g_tileNum = g_a->nodeNum;


	if (gc_simMode == 1)
		initStandardMode();
	else if (gc_simMode == 2)
		initDebugMode();
	else if (gc_simMode == 3)
		initSelfcheckMode();

	NoC noc("noc", g_a);
	noc.switch_cntrl(*g_clock);

	g_tracker = new Tracker();
	g_tracker->addProbes(noc);

//////////////////////////////////////////////////////////////////////////
// soclib tile
#ifdef SL_TILE
	if (gc_tileType == 1){
		cerr << "SLNIRGAM is used for soclib IP" << endl;
		return 1;
	}
	sc_start(sc_time(0, SC_NS));
	g_resetN = false;
	sc_start(sc_time(1, SC_NS));
	g_resetN = true;
#else
	if (gc_tileType == 2){
		cerr << "NIRGAM is used for generic IP" << endl;
		return 1;
	}
#endif
//////////////////////////////////////////////////////////////////////////

	if (gc_simMode == 1)
		doStandardMode();
	else if (gc_simMode == 2)
		doDebugMode();
	else if (gc_simMode == 3)
		doSelfcheckMode();

	if (gc_simMode == 1)
		finaStandardMode();
	else if (gc_simMode == 2)
		finaDebugMode();
	else if (gc_simMode == 3)
		finaSelfcheckMode();
}
