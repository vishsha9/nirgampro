#include "tracker.h"

#include <limits>

#include "../nwtile/InputChannel.h"
#include "../nwtile/NWTile.h"
#include "../noc.h"
#include "../nwtile/OutputChannel.h"

Tracker::Tracker(){
	string outname = gc_resultHome + "trackerOut";
	trackerOut.open(outname.c_str(), ostream::out);
	if(!trackerOut.is_open()){
		cout << "Can't open trackOut file" << endl;
	}

	outname = gc_resultHome + "flitSeqOut";
	flitSeqOut.open(outname.c_str(), ostream::out);
	if(!flitSeqOut.is_open()){
		cout << "Can't open flitSeqOut file" << endl;
	}

	sequence = 0;
	//xout =cout;
	//xout = cout;
}

Tracker::~Tracker(){
	if (trackerOut.is_open())
		trackerOut.close();
	if (flitSeqOut.is_open())
		flitSeqOut.close();
}


void Tracker::addProbes(NoC & noc){
	for (int i=0; i<noc.nwtile.size(); i++)
	{
		NwtileProbe nProbe;
		NWTile* nPtr = (NWTile *)noc.nwtile[i];
		nProbe.io_num = nPtr->io_num;
		nProbe.icProbes.resize(nPtr->io_num);
		nProbe.ocProbes.resize(nPtr->io_num);
		nProbe.tileId = nPtr->tileID;
		nProbe.tilePtr = nPtr;

		nProbe.portTable = &nPtr->portTable;
		nProbe.VA_vcFree = &nPtr->vcAlloc.vcFree;

		for (int j =0; j< nPtr->io_num; j++)
		{
			nProbe.icProbes[j].portId = nPtr->Ichannel[j]->cntrlID;
			nProbe.icProbes[j].icPtr = nPtr->Ichannel[j];
			nProbe.icProbes[j].vc = &nPtr->Ichannel[j]->vc;

			nProbe.ocProbes[j].portId = nPtr->Ochannel[j]->cntrlID;
			nProbe.ocProbes[j].ocPtr = nPtr->Ochannel[j];
			nProbe.ocProbes[j].r_in = &nPtr->Ochannel[j]->r_in;
			nProbe.ocProbes[j].r_vc = &nPtr->Ochannel[j]->r_vc;
		}
		tileProbes.push_back(nProbe);
	}
}

//////////////////////////////////////////////////////////////////////////

/*
*	Cin ingores write chars like \n, space and so on.
*	cin >> x would be blocked if you enter write chars.
*	this function unblock this.
*	other space char will be ignored.
*/
bool Tracker::isEnterInCin(){
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

vector<string> Tracker::parseCmd(){
	vector<string> cmds;
	while(!isEnterInCin()){
		string cmd;
		cin >> cmd;
		cmds.push_back(cmd);
	}
	cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
	return cmds;
}

/*
*	return size() == 0 if hier's format is wrong
*
*/
vector<ULL> Tracker::parseHier(string hier){
	vector<ULL> ret;
	int index;
	if (hier.find_first_not_of("0123456789.") != string::npos)
		return ret;
	if (hier.at(0) == '.')
		ret.push_back(0);	// absoluted path
	else
		ret.push_back(1);	// related path
	while(hier.size() > 0){
		index = hier.find_first_of(".");
		if (index == 0)
			hier.replace(0, 1, "");
		else{
			string seg = hier.substr(0, index);
			ret.push_back(atoi(seg.c_str()));
			hier.replace(0, index, "");
		}
	}
	return ret;
}

int Tracker::isfout(vector<string> cmd, string & fileName, char & mode){
	vector<string>::reverse_iterator riter = cmd.rbegin();
	while (riter != cmd.rend())
	{
		string s1 = riter->substr(0, 1);
		string s2 = riter->substr(0, 2);

		if (s1 == ">"){
			mode = ofstream::out;
			fileName = riter->substr(1, riter->size()-1);
		}
		else if (s2 == "a>" || s2 == "A>"){
			mode = ofstream::app;
			fileName = riter->substr(2, riter->size()-2);
		}
		else{
			riter++;
			continue;
		}

		if (fileName.size() == 0)
		{
			if (riter != cmd.rbegin())
			{
				fileName = *(riter-1);
				return 1;
			}
			else {
				return -1;
			}
		}
		return 1;
	}
	return 0;
}

/* 
return true: up
return false: quit
*/
bool Tracker::enterDebugerTile(UI tileId){
	if (tileId < 0 || tileId >= tileProbes.size())
	{
		cout << "No such tile with ID: " << tileId << ". Tile id from 0~" << tileProbes
			.size()-1<< endl;
		return true;
	}
	string cmd;
	while (true)
	{
		cout << "NoC.Tile_"<<tileId<<">"; 
		vector<string> cmd = parseCmd();
		if (cmd.size() == 0)
			continue;

		if (strcasecmp(cmd[0].c_str(), "up") == 0)
			return true;
		else if (strcasecmp(cmd[0].c_str(), "help")==0)
			printDebugerHelp(1, cout);
		else if (strcasecmp(cmd[0].c_str(), "quit")==0)
			return false;
	}
	return true;
}

/*
*	note:  commandView only accept absolute path, that hier[0] == 0
*/
void Tracker::commandView(string content, vector<ULL> hier, ostream & xout){
/*
view ic
view oc
view flit
view regin
view regvc
view vc
view port
*/
	if (hier[0] != 0)
	{
		// commandView only accept absolute path
		cerr << "Error occured in NIRGAM." << endl;
		return ;
	}
	// remove absolute path flag
	hier.erase(hier.begin());

	if (strcasecmp(content.c_str(), "ic") == 0)
	{
		if (hier.size() == 2)
			printIcOverview(hier[0], hier[1], xout);
		else 
			cerr << "Can't recognize location you input" << endl;
	}
	else if (strcasecmp(content.c_str(), "oc") == 0)
	{
		if (hier.size() == 0)
			printOCsOverviewTitled(xout);
		else if (hier.size() == 1)
			printOCsOverviewTitled(hier[0], xout);
		else if (hier.size() == 2)
			printOcOverviewTitled(hier[0], hier[1], xout);
		else 
			cerr << "Can't recognize location you input" << endl;
	}
	else if (strcasecmp(content.c_str(), "flit") == 0)
	{
		if (hier.size() == 1)
			searchAndPrintFlit(hier[0], xout);
		else
			cerr << "Flit sequence error" << endl;
	}
	else if (strcasecmp(content.c_str(), "regin") == 0)
	{
		if (hier.size() == 2)
			printOcRegin(hier[0], hier[1], xout);
		else 
			cerr << "Can't recognize location you input" << endl;
	}
	else if (strcasecmp(content.c_str(), "regvc") == 0)
	{
		if (hier.size() == 2)
			printOcRegvc(hier[0], hier[1], xout);
		else 
			cerr << "Can't recognize location you input" << endl;
	}
	else if (strcasecmp(content.c_str(), "vc") == 0)
	{
		if (hier.size() == 0)
			printIcVcs(xout);
		else if (hier.size() == 3)
			printIcVc(hier[0], hier[1], hier[2], xout);
		else 
			cerr << "Can't recognize location you input" << endl;
	}
	else if (strcasecmp(content.c_str(), "port") == 0)
	{
		if (hier.size() == 1)
			printPortTable(hier[0], xout);
		else 
			cerr << "Can't recognize location you input" << endl;
	}
	else
		cerr << content << " is not recognized" << endl;
}

void Tracker::printDebugerHelp(UI pos, ostream & xout){
	if (pos == 0) // top level
	{
		xout << "Commands:" << endl
			<<	"help\tPrint these info" <<endl
			<<	"set\tPlease do not use this command. It's experimental" << endl
			<<	"dl\tCheck whether DEADLOCK exsit and Print info."	<< endl
				<<"\tdl [>|a> filename]"	<< endl
			<< 	"view\tView information" << endl
				<<"\tview ic|oc|flit|regin|regvc|vc|port location [>|a> filename]"	<< endl
				<<"\tLocation: TILE_1.IC_3.VC_0 expressed as 1.3.0 or .1.3.0" << endl
			<<	"quit\tQuit debuger" << endl;
	}
	else if (pos == 1)
	{
		xout << "Commans:" << endl
			<< "help\t" << endl
			<< "up\t" << endl
			<< "quit\t" << endl;
			
	}
	else
	{/*xout << "Options:"<< endl 
		<< "0. Print this menu" <<  endl
		<< "1. view Oc overview" << endl 
		<< "2. view VC" << endl 
		<< "3. view Oc Regin" << endl 
		<< "4. view Oc Regvc" << endl
		<< "5. search and view flit" << endl
		<< "6. Port Table" << endl
		<< "7. QUIT" << endl;*/}
}

void Tracker::enterDebuger(){
	string cmd;
	//cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
	while (true)
	{
		cout << "NoC"<<">";
		vector<string> cmd = parseCmd();
		if (cmd.size() == 0)
			continue;
		
		if (strcasecmp(cmd[0].c_str(), "quit")==0)
			break;
		else if (strcasecmp(cmd[0].c_str(), "help")==0)
			printDebugerHelp(0, cout);
		else if (strcasecmp(cmd[0].c_str(), "dl") == 0){
			char mode=0; string fileName="";
			int ff = isfout(cmd, fileName, mode);
			if (ff == 1)
			{
				ofstream fout;
				if(mode == (char)ofstream::app)
					fout.open((gc_resultHome + fileName).c_str(), ofstream::app);
				else
					fout.open((gc_resultHome + fileName).c_str(), ofstream::out);
				if (!fout.is_open())
					cout<<"Cannot open "<< (gc_resultHome + fileName) << "__" <<endl;
				else{
					printDeadlock(fout);
					fout.close();
				}
			}
			else if(ff == -1){
				cout << "Miss output file name" << endl;
				// error, filename not input
			}
			else if (ff == 0)
				printDeadlock(cout);
			else;
			
		}
		else if (strcasecmp(cmd[0].c_str(), "set")==0){
			bool noerr = true;
			if (cmd.size() == 3){
				if (strcasecmp(cmd[1].c_str(), "tile")==0)
				{
					if (cmd[2].find_first_not_of("0123456789") != string::npos)
						// not a integer
						noerr = false;
					else{
						int tileId = atoi(cmd[2].c_str());
						bool ret = enterDebugerTile(tileId);
						if (ret == false)
							break; // exit debugger
					}
				}
				else
					noerr = false;
			}
			else
				noerr = false;
			if (!noerr)
				cout << "set: set tile [tileId]" << endl;
		}
		else if (strcasecmp(cmd[0].c_str(), "view")==0)
		{
			bool noerr = true;
			if (cmd.size() >= 3)
			{
				vector<ULL> hier = parseHier(cmd[2]);
				if (hier.size() == 0){
					cout << "Can't recognize location you input" << endl;
					noerr = false;
				}
				else{
					if (hier[0] == 1)	// rel path to abs path
						hier[0] = 0;

					char mode=0; string fileName="";
					int ff = isfout(cmd, fileName, mode);
					if (ff == 1)
					{
						ofstream fout;
						if(mode == (char)ofstream::app)
							fout.open((gc_resultHome + fileName).c_str(), ofstream::app);
						else
							fout.open((gc_resultHome + fileName).c_str(), ofstream::out);
						if (!fout.is_open())
							cout<<"Cannot open "<< (gc_resultHome + fileName) << "__" <<endl;
						else{
							commandView(cmd[1], hier, fout);
							fout.close();
						}
					}
					else if(ff == -1){
						cout << "Miss output file name" << endl;
						// error, filename not input
					}
					else if (ff == 0)
						commandView(cmd[1], hier, cout);
					else;
				}
			}
			else
				cerr << "view: miss arguments" << endl;
		}
		else 
			cout << "No such command" << endl; 
	}
}

void Tracker::updateStat(){
	if(flit_recved != 0 )
		avg_latency = total_latency / flit_recved;
	if (g_simCount >= gc_statEd)
		if ( (gc_statEd - gc_statOp) *g_tileNum !=0 )
			avg_throughput = (double)flit_recved * gc_FLITSIZE * 8.0/( (gc_statEd - gc_statOp) *g_tileNum);
	else 
		if ( (g_simCount - gc_statOp) *g_tileNum !=0 )
			avg_throughput = (double)flit_recved * gc_FLITSIZE * 8.0/( (g_simCount - gc_statOp) *g_tileNum);
}

void Tracker::printStat(ostream & xout){
	printOCsOverviewTitled(xout);
	updateStat();
	xout<<"\nOverall caled flit num = "<< flit_recved << endl;
	xout<<"\nOverall total flit latency = "<< total_latency << endl;
	xout <<"\nOverall average flit latency (in clock cycles per flit) = "<< avg_latency << endl;

	xout <<"\nOverall average flit throughput (Gbps) = "<< avg_throughput << endl;
}

void Tracker::printOCsOverviewTitled(ostream & xout){
	printTitleForOcOverview(xout);
	for(int i =0; i<tileProbes.size(); i++)
		printOCsOverview(tileProbes[i].tileId, xout);
}

//////////////////////////////////////////////////////////////////////////

void Tracker::printTitleForOcOverview(ostream & xout){
	xout<<"Tile\t"<<"Output\t\t"<<"Total no.\t"<<"Total no.\t"<<"Clock cycle\t"<<"avg. latency\t"<<"avg. latency\t"<<"average\n";
	xout<<"ID\t"<<"channel\t\t"<<"of packets\t"<<"of flits\t"<<"last leave\t"<<"per packet\t"<<"per flit\t"<<"throughput\n";
	xout<<"\t\t\t\t\t\t\t\t\t(clock cycles)\t(clock cycles)\t(Gbps)\n";
}

void Tracker::printOCsOverview(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	UI io_num = tileProbe->io_num;
	for(int i=0; i< io_num; i++)
		printOcOverview(tileId, tileProbe->ocProbes[i].portId , xout);
}

void Tracker::printOCsOverviewTitled(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	printTitleForOcOverview(xout);
	printOCsOverview(tileId, xout);
}

void Tracker::printOcOverview(UI tileId, UI portId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	OcProbe* ocprobe = getOcProbe(tileId, portId);
	if (ocprobe == NULL)
		return ;
	ocprobe->updateInfo();

	if (portId == tileProbe->io_num - 1)
	{
		xout	<<tileId<<"\t"<<portId<<"(IP)\t\t";
		xout	<<ocprobe->num_pkts<<"\t\t"
			<<ocprobe->num_flits<<"\t\t"
			<<ocprobe->end_cycle<<"\t\t"
			<<ocprobe->avg_latency<<"\t\t"
			<<ocprobe->avg_latency_flit<<"\t\t"
			<<ocprobe->avg_throughput<<endl;
	}
	else{
		xout	<<tileId<<"\t"<<portId<<"\t\t";
		xout	<<ocprobe->num_pkts<<"\t\t"
			<<ocprobe->num_flits<<"\t\t"
			<<ocprobe->end_cycle<<"\t\t"
			<<ocprobe->avg_latency<<"\t\t"
			<<ocprobe->avg_latency_flit<<"\t\t"
			<<ocprobe->avg_throughput<<endl;
	}
}

void Tracker::printOcOverviewTitled(UI tileId, UI portId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	OcProbe* ocprobe = getOcProbe(tileId, portId);
	if (ocprobe == NULL)
		return ;
	printTitleForOcOverview(xout);
	printOcOverview(tileId, portId, xout);
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//

void Tracker::printTitleForIcOverview(ostream & xout){
	// add here
}

void Tracker::printICsOverview(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	UI io_num = tileProbe->io_num;
	for(int i=0; i< io_num; i++)
		printIcOverview(tileId, tileProbe->ocProbes[i].portId , xout);
}

void Tracker::printICsOverviewTitled(UI tileId, ostream & xout){
	printTitleForIcOverview(xout);
	printICsOverview(tileId, xout);
}

void Tracker::printIcOverview(UI tileId, UI portId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if ( tileProbe == NULL )
		return ;
	IcProbe* icprobe = getIcProbe(tileId, portId);
	if (icprobe == NULL)
		return ;
	icprobe->updateInfo();

	if (portId == tileProbe->io_num - 1)
	{
		// add here
	}
	else{
		// add here
	}
}

void Tracker::printIcOverviewTitled(UI tileId, UI portId, ostream & xout){
	printTitleForIcOverview(xout);
	printIcOverview(tileId, portId, xout);
}

void Tracker::printPortTable(UI tileId, ostream & xcout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if ( tileProbe == NULL )
		return ;
	xcout << "\tPort\t"<< "NextTileId" << endl;
	for (int i =0; i < tileProbe->portTable->size(); i++)
		xcout << "\t" << i << "\t" << tileProbe->portTable->at(i) << endl;
}

void Tracker::printIcVcs(ostream & xcout){
	for(int i=0; i<tileProbes.size(); i++)
		printIcVcs(i, xcout);
}

void Tracker::printIcVcs(UI tileId, ostream & xcout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	for(int i=0; i<tileProbe->io_num; i++)
		printIcVcs(tileId, i, xcout);
}

void Tracker::printIcVcs(UI tileId, UI portId, ostream & xcout){
	for(int i =0; i<NUM_VCS; i++)
		printIcVc(tileId, portId, i, xcout);
}

void Tracker::printIcVc(UI tileId, UI portId, UI vcId, ostream & xcout){
	IcProbe * icprobe = getIcProbe(tileId, portId);
	if (icprobe == NULL)
		return ;
	if (vcId<0 || vcId >= NUM_VCS)
	{
		cerr << "Each IC only has " << NUM_VCS << " VCs in the network" << endl;
		return ;
	}
	VC* vc = &(icprobe->vc->at(vcId));
	NwtileProbe * tileprobe = getNwtileProbe(tileId);
	xcout << "Tile_" << tileId << ".IC_"<<portId<<".VC_" << vc->id 
		<< "\tFrom_Tile: " ;
	if(portId == tileprobe->portTable->size())
		xcout << "IP";
	else if(portId > tileprobe->portTable->size())
		xcout << "UD";
	else
		xcout << tileprobe->portTable->at(portId);
	xcout	<< "\tVC_nextId: " << vc->vc_next_id 
		<< "\tBufSize: " << vc->vcQ.pntr 
		<< "/" << vc->vcQ.num_bufs 
		<< "\tRoutePort: " << vc->vc_route;
	if(vc->vc_route == tileprobe->portTable->size())
		xcout << "(IP)";
	else if(vc->vc_route > tileprobe->portTable->size())
		xcout << "(UD)";
	else
		xcout << "(" << tileprobe->portTable->at(vc->vc_route) << ")";
	xcout << endl;
	if(vc->vcQ.full)
		xcout <<  "\tfull\t->";
	else if (vc->vcQ.empty)
		xcout <<  "\tempty\t->";
	else
		xcout << "\tnorm\t->";
	for (int i = vc->vcQ.pntr-1; i>=0; i--)
	{
		xcout << " | " << vc->vcQ.regs[i].simdata.sequence ;
	}
	xcout << " |->" << endl;
}

void Tracker::printOcRegin(UI tileId, UI portId, ostream & xcout){
	OcProbe * ocprobe = getOcProbe(tileId, portId);
	if (ocprobe == NULL)
		return ;
	xcout << "\tIc\tflitState" << endl;
	for (int i=0; i< ocprobe->r_in->size(); i++)
	{
		xcout << "\t" << i << "\t";
		if (ocprobe->r_in->at(i).free == false)
			xcout << ocprobe->r_in->at(i).val.simdata.sequence << endl;
		else
			xcout << "free" << endl;
	}
}

void Tracker::printOcRegvc(UI tileId, UI portId, ostream & xcout){
	OcProbe * ocprobe = getOcProbe(tileId, portId);
	if (ocprobe == NULL)
		return ;
	xcout << "\tVC\tflitState" << endl;
	for (int i=0; i< ocprobe->r_vc->size(); i++)
	{
		xcout << "\t" << i << "\t";
		if (ocprobe->r_vc->at(i).free == false)
			xcout << ocprobe->r_vc->at(i).val.simdata.sequence << endl;
		else
			xcout << "free" << endl;
	}
}
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
/*
all simdata of flit should be edit in Tracker
*/
//////////////////////////////////////////////////////////////////////////
void Tracker::processFlitWhenBorn(flit & f){
	f.simdata.gtime = sc_time_stamp();
	f.simdata.ctime = sc_time_stamp();
	f.simdata.atime = SC_ZERO_TIME;
	f.simdata.atimestamp = 0;
	f.simdata.num_waits = 0;
	f.simdata.num_sw = 0;
	f.simdata.gtimestamp = g_simCount;
	f.simdata.sequence = sequence;
	
	if(g_simCount >= gc_statOp && g_simCount <= gc_statEd)
		flitPool.insert(sequence);
	
	sequence++;
	flitSeqOut	<< "SQ " << f.simdata.sequence
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tPK " << f.pktid
		<< "\tFL " << f.flitid
		<< "\tSR " << f.src 
		<< "\tDS " << f.dest << endl;
}

void Tracker::enterInputChannel(UI tileId, UI portId, flit & f){
	trackerOut	<< "EtIC" 
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tSQ " << f.simdata.sequence
		<< "\tTL " << tileId 
		<< "\tPT " << portId << endl;
	f.simdata.ICtimestamp = g_simCount - 1;

	IcProbe * icprobe = getIcProbe(tileId, portId);
	icprobe->num_flits++;
	if(f.type == TAIL || f.type == HDT) 
		icprobe->num_pkts++;

	if(icprobe->beg_cycle == 0)
		icprobe->beg_cycle = g_simCount - 1;
}

void Tracker::exitInputChannel(UI tileId, UI portId, flit & f){
	trackerOut	<< "ExIC" 
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tSQ " << f.simdata.sequence
		<< "\tTL " << tileId 
		<< "\tPT " << portId << endl;
	f.simdata.num_sw ++;
	f.simdata.ctime = sc_time_stamp();
}

void Tracker::enterOutputChannel(UI tileId, UI portId, flit & f){
	OcProbe* ocprobe = getOcProbe(tileId, portId);
	trackerOut	<< "EtOC" 
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tSQ " << f.simdata.sequence
		<< "\tTL " << tileId 
		<< "\tPT " << portId << endl;
	if(ocprobe->beg_cycle == 0)
		//beg_cycle = input_time;
		ocprobe->beg_cycle = f.simdata.ICtimestamp;
}
void Tracker::getVcAtOutputChannel(UI tileId, UI portId, flit & f){
	OcProbe* ocprobe = getOcProbe(tileId, portId);
	if(f.type == HDT || f.type == HEAD)
		ocprobe->input_time[f.vcid] = f.simdata.ICtimestamp;
}

void Tracker::exitOutputChannel(UI tileId, UI portId, flit & f){
	trackerOut	<< "ExOC" 
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tSQ " << f.simdata.sequence
		<< "\tTL " << tileId 
		<< "\tPT " << portId << endl;
	f.simdata.ctime = sc_time_stamp();

	OcProbe* ocprobe = getOcProbe(tileId, portId);
	if(f.type == TAIL || f.type == HDT) 
	{
		ocprobe->latency += g_simCount -1 - ocprobe->input_time[f.vcid];
		ocprobe->num_pkts++;
		ocprobe->end_cycle = g_simCount - 1;
	}
	ocprobe->num_flits++;
	
	if(portId == ocprobe->ocPtr->io_num-1){
	// to IP
		if(flitPool.count(f.simdata.sequence) >0){
			flitPool.erase(f.simdata.sequence);
			total_latency += g_simCount - f.simdata.gtimestamp;
			flit_recved ++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

NwtileProbe* Tracker::getNwtileProbe(UI tileId){
	if (tileId < 0 || tileId >= tileProbes.size()){
		cerr << "No such tile with ID:" << tileId << ". Tile id from 0~" << tileProbes
			.size()-1<< endl;
		return NULL;
	}
	return &tileProbes[tileId];
}

IcProbe* Tracker::getIcProbe(UI tileId, UI portId){
	NwtileProbe* tileprobe = getNwtileProbe(tileId);
	if (tileprobe != NULL)
	{
		if (portId < 0 || portId >= tileprobe->io_num)
		{
			cerr << "No such IC with ID:" << portId << " in tile_" << tileId << ". Tile_" << tileId << " has " << tileprobe->io_num << " ICs" << endl;
			return NULL;
		}
		else
			return &(getNwtileProbe(tileId)->icProbes[portId]);
	}
	else
		return NULL;
} 

OcProbe* Tracker::getOcProbe(UI tileId, UI portId){
	NwtileProbe* tileprobe = getNwtileProbe(tileId);
	if (tileprobe != NULL)
	{
		if (portId < 0 || portId >= tileprobe->io_num)
		{
			cerr << "No such OC with ID:" << portId << "vin tile_" << tileId << ". Tile_" << tileId << " has " << tileprobe->io_num << " OCs" << endl;
			return NULL;
		}
		else
			return &(getNwtileProbe(tileId)->ocProbes[portId]);
	}
	else
		return NULL;
}

//////////////////////////////////////////////////////////////////////////
// IcProbe
IcProbe::IcProbe(){
	this->avg_inputRate = 0;
}

void IcProbe::updateInfo(){
	total_cycles = end_cycle - beg_cycle;
	if( total_cycles != 0)
		avg_inputRate = (float)(num_flits * gc_FLITSIZE * 8) / (total_cycles * g_clkPeriod);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OcProbe
OcProbe::OcProbe(){
	this->latency = 0;
	this->num_flits = 0;
	this->num_pkts = 0;

	this->avg_latency = 0;
	this->avg_latency_flit = 0;
	this->avg_throughput = 0;

	this->beg_cycle = 0;
	this->end_cycle = 0;
	this->total_cycles = 0;
	for (int i=0; i<NUM_VCS; i++)
		this->input_time[i] = 0;
}

void OcProbe::updateInfo(){
	if(num_pkts != 0)
		avg_latency = (float)latency/num_pkts;
	if(num_flits != 0)
		avg_latency_flit = (float)latency/num_flits;
	total_cycles = end_cycle - beg_cycle;
	if(total_cycles != 0)
		avg_throughput = (float)(num_flits * gc_FLITSIZE * 8) / (total_cycles * g_clkPeriod);	// Gbps
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// 

void Tracker::printFlit(flit & f, ostream & xout){ 
	if (f.type == HDT)
		xout << "HDT";
	else if (f.type == HEAD)
		xout << "HEAD";
	else if (f.type == DATA)
		xout << "DATA";
	else if (f.type == TAIL)
		xout << "TAIL";
	else
		xout << "UNKN";
	xout	<< "\tpkId: " << f.pktid 
		<< "\t\tftId: " << f.flitid 
		<< "\t\tvcId: " << f.vcid 
		<< "\t\tsrc: " << f.src  << endl
		<< "\tdest: " << f.field1 
		<< "\t\tfld2: " << f.field2 
		<< "\t\tfld3: " << f.field3 
		<< "\t\tdata: " << f.field4 << endl;
	xout << "Sim " 
		<< "\tgt: " << f.simdata.gtime 
		<< "\t\tct: " << f.simdata.ctime 
		<< "\t\tat: " << f.simdata.atime 
		<< "\t\tgts: " << f.simdata.gtimestamp << endl
		<< "\ticts: " << f.simdata.ICtimestamp
		<< "\t\tats: " << f.simdata.atimestamp 
		<< "\t\tnsw: " << f.simdata.num_sw 
		<< "\t\tnwt: " << f.simdata.num_waits 
		<< "\t\tseq: " << f.simdata.sequence << endl;
}

bool Tracker::searchAndPrintFlit(ULL sequence, ostream & xout){
	flit* ftemp = NULL;
	for (int i = 0; i < tileProbes.size(); i++)
	{
		ftemp =  searchFlitInTile(tileProbes.at(i).tileId, sequence, xout );
		if (ftemp!=NULL) break;
	}
	if(ftemp ==  NULL){
		xout << "Can't find flit $" << sequence << endl;
		return false;
	}
	else{
		printFlit(*ftemp, xout);
		return true;
	}
}

flit* Tracker::searchFlitInTile(UI tileId, ULL sequence, ostream & xout){
	NwtileProbe * tileprobe = getNwtileProbe(tileId);
	flit* ret = NULL;
	for (int i=0; i<tileprobe->io_num; i++){
		ret = searchFlitInIc(tileId, tileprobe->icProbes.at(i).portId, sequence, xout);
		if (ret!=NULL) return ret;
	}
	for (int i=0; i<tileprobe->io_num; i++){
		ret = searchFlitInOc(tileId, tileprobe->ocProbes.at(i).portId, sequence, xout);
		if (ret!=NULL) return ret;
	}
	return NULL;
}

flit* Tracker::searchFlitInIc(UI tileId, UI portId, ULL sequence, ostream & xout){
	IcProbe* icprobe = getIcProbe(tileId, portId);
	for (int i=0; i<NUM_VCS; i++)
	{
		fifo* f =&(icprobe->vc->at(i).vcQ);
		for (int j =0; j<f->pntr; j++)
		{
			if(f->regs[j].simdata.sequence == sequence){
				xout << "Flit $" << sequence << " in the Buffer of Tile_" << tileId <<".IC_" << portId << ".VC_" << i << endl;
				//printFlit(f->regs[i], xout);
				return &(f->regs[i]);
			}
		}
	}
	return NULL;
}

flit* Tracker::searchFlitInOc(UI tileId, UI portId, ULL sequence, ostream & xout){
	OcProbe* ocprobe = getOcProbe(tileId, portId);
	for (int i=0; i<ocprobe->r_in->size(); i++)
	{
		if (ocprobe->r_in->at(i).free == false && ocprobe->r_in->at(i).val.simdata.sequence == sequence)
		{
			xout << "Flit $" << sequence << " in Tile_" << tileId <<".OC_" << portId << ".RegIn_" << i << endl;
			//printFlit(ocprobe->r_in->at(i).val, xout);
			return &(ocprobe->r_in->at(i).val);
		}
	}
	for (int i=0; i<ocprobe->r_vc->size(); i++)
	{
		if (ocprobe->r_vc->at(i).free == false && ocprobe->r_vc->at(i).val.simdata.sequence == sequence)
		{
			xout << "Flit $" << sequence << "in Tile_" << tileId <<".OC_" << portId << "RegVc_" << i << endl;
			//printFlit(ocprobe->r_vc->at(i).val, xout);
			return &(ocprobe->r_vc->at(i).val);
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
void Tracker::printDeadlock(ostream & xout){
	vector<vector<string> > rings = checkDeadlock();
	if(rings.size()==0)
		cout << "Cong! No Deadlock exsit" << endl;
	else{
		for(int i =0; i<rings.size(); i++){
			vector<string> ring = rings.at(i);
			for(int j=0; j<ring.size(); j++){
				xout << ring.at(j) << "\t";
			}
			xout << endl;
		}
	}
}

vector<vector<string> > Tracker::checkDeadlock(){
	vector<vector<string> > rings;
	for(int i = 0; i < tileProbes.size(); i++){
		int j = getNwtileProbe(i)->io_num-1; // IP core is the start
		//for(int j = 0; j < getNwtileProbe(i)->io_num; j++){
		for(int k =0; k < NUM_VCS; k++){
			vector<string> ring;
			UI tl = i,
				ic = j,
				vi = k;
			while(true){
				char buf[20];
				sprintf(buf, "%d.%d.%d", tl, ic, vi);
				string str(buf);
				ring.push_back(str);
				int l;
				for(l=0; l<ring.size(); l++)
					if(ring.at(l).compare(str) == 0)	break;
				if(l!=ring.size()-1)
					break;
				
				VC* vc = &(getIcProbe(tl, ic)->vc->at(vi));
				// find next hop tile
				int ttl = tl;
				int port = vc->vc_route;
				if(port == getNwtileProbe(tl)->io_num){ // route undetermined
					ring.clear();
					break;
				}
				else if(port == getNwtileProbe(tl)->io_num-1){ // route to IP
					ring.clear();
					break;
				}
				else 
					tl = getNwtileProbe(tl)->portTable->at(port);
					
				getNwtileProbe(tl)->tilePtr->idToDir(ttl, &ic);
				if (vc->vcQ.empty == false)	// fifo no empty
					vi = vc->vcQ.flit_read().vcid;
				else{					// fifo empty
					ring.clear();
					break;
				}
			}
			if(ring.size()!=0)
				rings.push_back(ring);
		}
		//}
	}
	return rings;
}