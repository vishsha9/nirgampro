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

	outname = gc_resultHome + "routerTable";
	rtTable.open(outname.c_str(), ostream::out);
	if(!rtTable.is_open()){
		cout << "Can't open routerTable file" << endl;
	}

	outname = gc_resultHome + "simResults";
	simResults.open(outname.c_str(), ostream::out);
	if(!simResults.is_open()){
		cout << "Can't open simResults file" << endl;
	}

	outname = gc_resultHome + "timeTable";
	timeTable.open(outname.c_str(), ostream::out);
	if(!timeTable.is_open()){
		cout << "Can't open timeTable file" << endl;
	}
	printTitleForTimet(timeTable);

	sequence = 0;

	total_flit_latency = 0;
	total_pkt_latency = 0;

	total_flit_hops = 0;

	total_flit_send = 0;
	total_pkt_send = 0;
	total_flit_recved = 0;
	total_pkt_recved = 0;
	
	avg_pkt_swDelay = 0;		
	avg_flit_swDelay = 0;		

	avg_flit_latency = 0;		
	avg_pkt_latency = 0;		
	avg_flit_hops = 0;

	avg_inputRate = 0;
	avg_outputRate = 0;

	//
	{
		inst_total_flit_latency = 0;
		inst_total_pkt_latency = 0;
		inst_total_flit_hops = 0;

		inst_total_flit_send = 0;
		inst_total_pkt_send = 0;
		inst_total_flit_recved = 0;
		inst_total_pkt_recved = 0;

		inst_pkt_swDelay = 0;		// package switch Delay
		inst_flit_swDelay = 0;		// flit switch delay
		inst_flit_latency = 0;
		inst_pkt_latency = 0;
		inst_flit_hops = 0;
		inst_inputRate = 0;
		inst_outputRate = 0;
	}
}

Tracker::~Tracker(){
	if (trackerOut.is_open())
		trackerOut.close();
	if (flitSeqOut.is_open())
		flitSeqOut.close();
	if (simResults.is_open())
		simResults.close();
	if (timeTable.is_open())
		timeTable.close();
	// rtTable closed in printRouteTable(xout);
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

		nProbe.rtTable= nPtr->ctr.rtable->opTable;
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
			cerr << "No such OC with ID:" << portId << " in tile_" << tileId << ". Tile_" << tileId << " has " << tileprobe->io_num << " OCs" << endl;
			return NULL;
		}
		else
			return &(getNwtileProbe(tileId)->ocProbes[portId]);
	}
	else
		return NULL;
}

void Tracker::printStat(){
	printStat(simResults);
}

void Tracker::printRouteTables(){
	for (int i=0; i<g_tileNum; i++)
		printRouteTable(i, rtTable);
	if (rtTable.is_open())
		rtTable.close();
}

void Tracker::printTitleForTimet(ostream & xout){
	xout  << "cycle\t"
		<< "flit_send\t"
		<< "pkts_send\t"
		<< "aInputRate\t"
		<< "iInputRate\t"

		<< "flit_recved\t"
		<< "pkts_recved\t"
		<< "aOutputRate\t" 
		<< "iOutputRate\t" 

		<< "aFlit_latcy\t"
		<< "iFlit_latcy\t"
		<< "aPkt_latcy\t"
		<< "iPkt_latcy\t"

		<< "flit_delay\t"
		<< "pkt_delay\t"
		<< "hops\t\t"

		<< endl;
}

void Tracker::run(){
	cout <<"\r" << g_simCount << " of " << gc_simNum << " cycles";
	if (g_simCount >= gc_statOp && g_simCount <= gc_statEd)
	{
		if ( (g_simCount - gc_statOp) % gc_statRes == 0 )
		{
			updateStat();
			timeTable << g_simCount << "\t"
				<< total_flit_send << "\t\t"
				<< total_pkt_send << "\t\t"
				<< avg_inputRate << "\t\t"
				<< inst_inputRate << "\t\t"

				<< total_flit_recved << "\t\t"
				<< total_pkt_recved << "\t\t"
				<< avg_outputRate << "\t\t"
				<< inst_outputRate << "\t\t"

				<< avg_flit_latency << "\t\t"
				<< inst_flit_latency << "\t\t"
				<< avg_pkt_latency << "\t\t"
				<< inst_pkt_latency << "\t\t"

				<< avg_flit_swDelay << "\t\t"
				<< avg_pkt_swDelay << "\t\t"
				<< avg_flit_hops << "\t\t"

				<< endl;
			cleanStat();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// IcProbe
IcProbe::IcProbe(){
	flit_in = 0;
	pkt_in = 0;

	avg_inputRate = 0;
	avg_inputRate2 = 0;

	beg_cycle = 0;
	end_cycle = 0;

	flit_in_inst = 0;
	pkt_in_inst = 0;
	inst_inputRate = 0;
}
//////////////////////////////////////////////////////////////////////////



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
			printIcStat(hier[0], hier[1], xout);
		else 
			cerr << "Can't recognize location you input" << endl;
	}
	else if (strcasecmp(content.c_str(), "oc") == 0)
	{
		if (hier.size() == 0){
			printTitleForOcStat(xout);
			for (int i=0; i< g_tileNum; i++)
				printOCsStatTitled(i, xout);
		}
		else if (hier.size() == 1)
			printOCsStatTitled(hier[0], xout);
		else if (hier.size() == 2)
			printOcStatTitled(hier[0], hier[1], xout);
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
		if (hier.size() == 0){
			for (int i=i; i<g_tileNum; i++)
				printIcVcs(i, xout);
		}
		else if (hier.size() == 1)
			printIcVcs(hier[0], xout);
		else if (hier.size() == 2)
			printIcVcs(hier[0], hier[1], xout);
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
	else if (strcasecmp(content.c_str(), "va") == 0)
	{
		if (hier.size() == 1)
			printVaVcFree(hier[0], xout);
		else 
			cerr << "Can't recognize location you input" << endl;
	}
	else if (strcasecmp(content.c_str(), "rt") == 0)
	{
		if (hier.size() == 1)
			printRouteTable(hier[0], xout);
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
			<<"\tview ic|oc|flit|regin|regvc|vc|port|va|rt location [>|a> filename]"	<< endl
			<<"\tLocation: TILE_1.IC_3.VC_0 expressed as 1.3.0 or .1.3.0" << endl
			<< "rt\tPrint route path" << endl
			<< "\trt srcId destId" << endl
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
	 << "1. view Oc Stat" << endl 
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
		else if (strcasecmp(cmd[0].c_str(), "now")==0)
			cout << g_simCount << endl;
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
		else if (strcasecmp(cmd[0].c_str(), "rt") == 0){
			bool noerr = true;
			if (cmd.size() == 3){
				if (cmd[1].find_first_not_of("0123456789") == string::npos &&
					cmd[2].find_first_not_of("0123456789") == string::npos){
						int srcId =  atoi(cmd[1].c_str());
						int destId =  atoi(cmd[2].c_str());
						printRoutePath(srcId, destId, cout);
				}
				else
					noerr = false;
			}
			else
				noerr = false;
			if (!noerr)
				cout << "rt: rt srcId destId" << endl;
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



void Tracker::printStat(ostream & xout){
	updateStat();
	xout << "Stat. starts at " << gc_statOp << " cycles, ends at " 
		<< (g_simCount>gc_statEd?gc_statEd:g_simCount) << " cycles." << endl << endl;
	xout << "################################### Overall Stat. Data ####################################" << endl;
	xout << "Overall flits input num = " << total_flit_send << endl;
	xout << "Overall flits output num = " << total_flit_recved << endl;
	xout << "Overall average flit delay within a tile (in clock cycles per flit) = " << avg_flit_swDelay << endl;
	xout << "Overall average flit latency (in clock cycles per flit) = " << avg_flit_latency << endl;
	xout << "Overall average flit hops = " << avg_flit_hops << endl;
	xout << endl;
	xout << "Overall packets input num = " << total_pkt_send << endl;
	xout << "Overall packets output num = " << total_pkt_recved << endl;
	xout << "Overall average packet delay within a tile (in clock cycles per packet) = " << avg_pkt_swDelay << endl;
	xout << "Overall average packet latency (in clock cycles per flit) = " << avg_pkt_latency << endl;
	xout << endl;
	xout << "Overall average flit inputRate (Gbps) = " << avg_inputRate << endl; 
	xout << "Overall average flit outputRate (Gbps) = " << avg_outputRate << endl << endl;
	xout << "#################################### Tiles Stat. Data ####################################" << endl;
	printTitleForTlStat(xout);
	for (int i=0; i<g_tileNum; i++)
		printTileStat(i, xout);
	xout << endl;	
	xout << "################################## Stat. Data For each Tile################################" << endl;
	for (int i=0; i<g_tileNum; i++)
	{
		xout << "###################### Tile " << i << " ###################" << endl;
		printTileStatTitled(i, xout);
		xout << "# IP" << endl;
		printTileDataDis(i, xout);
		xout << "# IC" << endl;
		printTitleForIcStat(xout);
		for (int j=0; j<getNwtileProbe(i)->io_num; j++)
			printIcStat(i, j, xout);
		xout << "# OC" << endl;
		printTitleForOcStat(xout);
		for (int j=0; j<getNwtileProbe(i)->io_num; j++)
			printOcStat(i, j, xout);
		xout << endl;
	}
}

void Tracker::printTitleForTlStat(ostream & xout){
	xout << "Tile\t"	
		<< "Ports\t"
		<< "Trans_flits\t" 
		<< "Trans_packet\t"
		<< "Avg_input\t" 
		<< "Trans_flits\t" 
		<< "Trans_packet\t"
		<< "Avg_output\t"
		<< "Avg._delay\t"
		<< "Avg._delay\t"
		<< "IP_flits\t"
		<< "IP_packet\t"
		<< "Avg_IP_send\t" 
		<< "IP_flits\t"
		<< "IP_packet\t"
		<< "Avg_IP_recved\t"
		<< endl;
	xout << "\t"
		<< "\t"
		<< "input_num\t"
		<< "input_num\t"
		<< "Rate(Gbps)\t"
		<< "output_num\t"
		<< "output_num\t"
		<< "Rate(Gbps)\t"
		<< "per_flit\t" 
		<< "per_packet\t"
		<< "send_num\t"
		<< "send_num\t"
		<< "Rate(Gbps)\t"
		<< "recved_num\t"
		<< "recved_num\t"
		<< "Rate(Gbps)\t"
		<< endl;
	xout <<"\t" 
		<< "\t"
		<< "\t\t"
		<< "\t\t"
		<< "\t\t"
		<< "\t\t"
		<< "\t\t"
		<< "\t\t"
		<<"(clock cycles)\t"
		<<"(clock cycles)\t"
		<< endl;
}


void Tracker::printTileStatTitled(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	printTitleForTlStat(xout);
	printTileStat(tileId, xout);
}

void Tracker::printTileStat(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	xout << tileId << "\t"
		<< tileProbe->io_num << "\t"
		<< tileProbe->total_flit_in << "\t\t"
		<< tileProbe->total_pkt_in << "\t\t"
		<< tileProbe->avg_inputRate << "\t\t"
		
		<< tileProbe->total_flit_out << "\t\t"
		<< tileProbe->total_pkt_out << "\t\t"
		<< tileProbe->avg_outputRate << "\t\t"

		<< tileProbe->avg_flit_delay << "\t\t"
		<< tileProbe->avg_pkt_delay << "\t\t"
	
		<< tileProbe->icProbes[tileProbe->io_num-1].flit_in << "\t\t"
		<< tileProbe->icProbes[tileProbe->io_num-1].pkt_in << "\t\t"
		<< tileProbe->icProbes[tileProbe->io_num-1].avg_inputRate << "\t\t"
		
		<< tileProbe->ocProbes[tileProbe->io_num-1].flit_out << "\t\t"
		<< tileProbe->ocProbes[tileProbe->io_num-1].flit_out << "\t\t"
		<< tileProbe->ocProbes[tileProbe->io_num-1].avg_outputRate << "\t\t"
		<< endl;
}


void Tracker::printTileDataDis(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	xout << "Flit_&_packet_distribution" << endl
		<< "Tile\t"
		<< "flit_send\t"
		<< "packet_send\t"
		<< "flit_recved\t"
		<< "packet_recved\t" 
		<< "avg._recvd\t" 
		<< "avg._recvd\t"
		<< "avg._recvd\t" << endl;
	xout << "\t"
		<< "\t\t"
		<< "\t\t"
		<< "\t\t"
		<< "\t\t"
		<< "flit_hops\t"
		<< "flit_latency\t"
		<< "pkt_latency\t" << endl;
	xout << "SUM\t"
		<< tileProbe->icProbes[tileProbe->io_num-1].flit_in << "\t\t"
		<< tileProbe->icProbes[tileProbe->io_num-1].pkt_in << "\t\t"
		<< tileProbe->ocProbes[tileProbe->io_num-1].flit_out << "\t\t"
		<< tileProbe->ocProbes[tileProbe->io_num-1].flit_out << "\t\t"
		<< "-\t\t"
		<< "-\t\t"
		<< "-\t\t" << endl;
	for (int i=0; i< g_tileNum; i++)
	{
		xout << i << "\t" 
			<< tileProbe->flitTo[i] << "\t\t" 
			<< tileProbe->pktTo[i] << "\t\t" 
			<< tileProbe->flitFrom[i] << "\t\t" 
			<< tileProbe->pktFrom[i]<< "\t\t"
			<< tileProbe->avg_flit_hops[i] << "\t\t"
			<< tileProbe->avg_flit_latency[i] << "\t\t"
			<< tileProbe->avg_pkt_latency[i] << "\t\t" << endl;
	}
}

void Tracker::printTitleForOcStat(ostream & xout){
	xout<<"Tile\t"
		<<"Output\t"
		<<"Clock_cycle\t"
		<<"Clock_cycle\t"
		<<"Total_no.\t"
		<<"Total_no.\t"
		<<"Average\t\t"
		<<"Average\t\t"
		<<"Avg._delay\t"
		<<"Avg._delay\n";
	xout<<"ID\t"
		<<"channel\t"
		<<"first_arr\t"
		<<"last_leave\t"
		<<"of_flits\t"
		<<"of_packets\t"
		<<"outputRate\t"
		<<"outputRate2\t"
		<<"per_flit\t"
		<<"per_packet\n";
	xout<<"\t"
		<<"\t"
		<<"\t\t"
		<<"\t\t"
		<<"out\t\t"
		<<"out\t\t"
		<<"(Gbps)\t\t"
		<<"(Gbps)\t\t"
		<<"(clock_cycles)\t"
		<<"(clock_cycles)\n";
}

void Tracker::printOcStat(UI tileId, UI portId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	OcProbe* ocprobe = getOcProbe(tileId, portId);
	if (ocprobe == NULL)
		return ;
	ocprobe->updateStat();

	if (portId == tileProbe->io_num - 1)
		xout << tileId << "\t" <<portId << "(IP)\t";
	else
		xout << tileId << "\t "<<portId << "\t";
	xout<< ocprobe->beg_cycle <<"\t\t"
		<< ocprobe->end_cycle <<"\t\t"
		<< ocprobe->flit_out <<"\t\t"
		<< ocprobe->pkt_out <<"\t\t"
		<< ocprobe->avg_outputRate <<"\t\t"
		<< ocprobe->avg_outputRate2 <<"\t\t"
		<< ocprobe->avg_flit_delay <<"\t\t"
		<< ocprobe->avg_pkt_delay
		<<endl;
}

void Tracker::printOcStatTitled(UI tileId, UI portId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	OcProbe* ocprobe = getOcProbe(tileId, portId);
	if (ocprobe == NULL)
		return ;
	printTitleForOcStat(xout);
	printOcStat(tileId, portId, xout);
}

void Tracker::printOCsStat(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	UI io_num = tileProbe->io_num;
	for(int i=0; i< io_num; i++)
		printOcStat(tileId, tileProbe->ocProbes[i].portId , xout);
}

void Tracker::printOCsStatTitled(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	printTitleForOcStat(xout);
	printOCsStat(tileId, xout);
}

void Tracker::printTitleForIcStat(ostream & xout){
	// add here
	xout<<"Tile\t"
		<<"Input\t"
		<<"Clock_cycle\t"
		<<"Clock_cycle\t"
		<<"Total_no.\t"
		<<"Total_no.\t"
		<<"Average\t\t"
		<<"Average\n";
	xout<<"ID\t"
		<<"channel\t"
		<<"first_arr\t"
		<<"last_leave\t"
		<<"of_flits\t"
		<<"of_packets\t"
		<<"inputRate\t"
		<<"intputRate2\n";
	xout<<"\t"
		<<"\t"
		<<"\t\t"
		<<"\t\t"
		<<"in\t\t"
		<<"in\t\t"
		<<"(Gbps)\t\t"
		<<"(Gbps)\n";
}

void Tracker::printIcStat(UI tileId, UI portId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if ( tileProbe == NULL )
		return ;
	IcProbe* icprobe = getIcProbe(tileId, portId);
	if (icprobe == NULL)
		return ;
	icprobe->updateStat();

	if (portId == tileProbe->io_num - 1)
		xout << tileId << "\t" <<portId << "(IP)\t";
	else
		xout << tileId << "\t "<<portId << "\t";
	xout<< icprobe->beg_cycle <<"\t\t"
		<< icprobe->end_cycle <<"\t\t"
		<< icprobe->flit_in <<"\t\t"
		<< icprobe->pkt_in <<"\t\t"
		<< icprobe->avg_inputRate <<"\t\t"
		<< icprobe->avg_inputRate2 <<endl;
}

void Tracker::printIcStatTitled(UI tileId, UI portId, ostream & xout){
	printTitleForIcStat(xout);
	printIcStat(tileId, portId, xout);
}

void Tracker::printICsStat(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if (tileProbe == NULL)
		return ;
	UI io_num = tileProbe->io_num;
	for(int i=0; i< io_num; i++)
		printIcStat(tileId, tileProbe->icProbes[i].portId , xout);
}

void Tracker::printICsStatTitled(UI tileId, ostream & xout){
	printTitleForIcStat(xout);
	printICsStat(tileId, xout);
}

void Tracker::printRoutePath(UI src, UI dest, ostream & xout){
	UI tileId = src;
	UI nextTile;
	xout << src ;
	do{
		NwtileProbe* tileProbe = getNwtileProbe(tileId);
		if ( tileProbe == NULL )
			return ;
		map<int, int> * rTable = tileProbe->rtTable;
		nextTile = rTable->find(dest)->second;
		tileId = nextTile;
		xout << " -> " << tileId;
	}
	while(nextTile != dest);
	xout << endl;
}

void Tracker::printPortTable(UI tileId, ostream & xcout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if ( tileProbe == NULL )
		return ;
	xcout << "\tPort\t"<< "NextTileId" << endl;
	for (int i =0; i < tileProbe->portTable->size(); i++)
		xcout << "\t" << i << "\t" << tileProbe->portTable->at(i) << endl;
}

void Tracker::printRouteTable(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if ( tileProbe == NULL )
		return ;
	xout << "# Tile_" << tileId <<  endl;
	map<int, int>::iterator iter = tileProbe->rtTable->begin();
	xout << "# Dest\tNextTile" << endl;
	while(iter != tileProbe->rtTable->end()){
		xout << iter->first << "\t" << iter->second << endl;
		iter ++;
	}
}

void Tracker::printVaVcFree(UI tileId, ostream & xout){
	NwtileProbe* tileProbe = getNwtileProbe(tileId);
	if ( tileProbe == NULL )
		return ;
	xout << "Port\t\t";
	for (int i=0; i<NUM_VCS; i++)
		xout<< "VC_" << i << "\t";
	xout << endl;

	for (int i =0; i < tileProbe->VA_vcFree->size(); i++){
		xout <<  i ;
		if ( i < tileProbe->portTable->size())
			xout << " (" << tileProbe->portTable->at(i) << ")\t\t";
		else
			xout << " (IP)\t\t";
		for (int j=0; j<NUM_VCS; j++)
			if ( tileProbe->VA_vcFree->at(i).at(j) == true)
				xout << "free\t";
			else 
				xout << "busy\t";
		xout <<  endl;
	}
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
				return &(f->regs[j]);
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
//////////////////////////////////////////////////////////////////////////

NwtileProbe::NwtileProbe(){
	total_flit_in = 0;
	total_flit_out = 0;
	total_pkt_in = 0;
	total_pkt_out = 0;

	total_pkt_delay = 0;		
	total_flit_delay = 0;

	avg_inputRate = 0;
	avg_outputRate = 0;		
	avg_pkt_delay = 0;		
	avg_flit_delay = 0;		

	flitFrom.resize(g_tileNum);
	flitTo.resize(g_tileNum);
	pktFrom.resize(g_tileNum);
	pktTo.resize(g_tileNum);

	flit_latency.resize(g_tileNum);
	pkt_latency.resize(g_tileNum);
	flit_hops.resize(g_tileNum);

	avg_flit_latency.resize(g_tileNum);
	avg_pkt_latency.resize(g_tileNum);
	avg_flit_hops.resize(g_tileNum);

	total_flit_latency = 0;
	total_pkt_latency = 0;
	total_flit_hops = 0;

	{
		inst_total_flit_in = 0;
		inst_total_flit_out = 0;
		inst_total_pkt_in = 0;
		inst_total_pkt_out = 0;

		inst_total_pkt_delay = 0;			///< total latency
		inst_total_flit_delay = 0;

		inst_inputRate = 0;
		inst_outputRate = 0;
		inst_pkt_delay = 0;
		inst_flit_delay = 0;

		flit_latency_sum_inst = 0;
		pkt_latency_sum_inst = 0;
		flit_hops_sum_inst = 0;
	}
}

void NwtileProbe::updateStat(){
	for (int i=0; i< this->io_num; i++)
	{
		icProbes[i].updateStat();
		ocProbes[i].updateStat();
	}

	// delay
	for (int i=0; i< this->io_num; i++)
	{
		total_flit_in += icProbes[i].flit_in;
		total_pkt_in += icProbes[i].pkt_in;
		total_flit_out += ocProbes[i].flit_out;
		total_pkt_out += ocProbes[i].pkt_out;

		total_flit_delay += ocProbes[i].flit_delay_sum;
		total_pkt_delay += ocProbes[i].pkt_delay_sum;

		inst_total_flit_in += icProbes[i].flit_in_inst;
		inst_total_pkt_in += icProbes[i].pkt_in_inst;
		inst_total_flit_out += ocProbes[i].flit_out_inst;
		inst_total_pkt_out += ocProbes[i].pkt_out_inst;

		inst_total_flit_delay += ocProbes[i].flit_delay_sum_inst;
		inst_total_pkt_delay += ocProbes[i].pkt_delay_sum_inst;
	}
	if (total_flit_out != 0)
		avg_flit_delay = (float)total_flit_delay / total_flit_out;
	if (total_pkt_out != 0 )
		avg_pkt_delay = (float)total_pkt_delay / total_pkt_out;
	if (g_simCount>gc_statEd?gc_statEd:g_simCount - gc_statOp >0){
		ULL cy_dr = g_simCount>gc_statEd?gc_statEd:g_simCount - gc_statOp;
		avg_inputRate = (float)(total_flit_in * gc_FLITSIZE * 8) / (cy_dr * g_clkPeriod * io_num);
		avg_outputRate = (float)(total_flit_out * gc_FLITSIZE * 8) / (cy_dr * g_clkPeriod * io_num);
	}

	if (inst_total_flit_out != 0)
		inst_flit_delay = (float)inst_total_flit_delay / inst_total_flit_out;
	if (inst_total_pkt_out != 0)
		inst_pkt_delay = (float)inst_total_pkt_delay / inst_total_pkt_out;
	inst_inputRate = (float)(inst_total_flit_in * gc_FLITSIZE * 8) / (gc_statRes * g_clkPeriod * io_num);
	inst_outputRate = (float)(inst_total_flit_out * gc_FLITSIZE * 8) / (gc_statRes * g_clkPeriod * io_num);
	
	// IP data
	for (int i=0; i<g_tileNum; i++)
	{
		if (flitFrom[i] != 0)
		{
			avg_flit_latency[i] = flit_latency[i] / flitFrom[i];
			avg_flit_hops[i] = flit_hops[i] / flitFrom[i];
		}
		if (pktFrom[i] != 0)
			avg_pkt_latency[i] = pkt_latency[i] / pktFrom[i];
		

		total_flit_latency += flit_latency[i];
		total_pkt_latency += pkt_latency[i];
		total_flit_hops += flit_hops[i];
	}
}

void NwtileProbe::cleanStat(){
	for (int i=0; i< this->io_num; i++)
	{
		icProbes[i].cleanStat();
		ocProbes[i].cleanStat();
	}
	inst_total_flit_in = 0;
	inst_total_flit_out = 0;
	inst_total_pkt_in = 0;
	inst_total_pkt_out = 0;

	inst_total_pkt_delay = 0;			///< total latency
	inst_total_flit_delay = 0;

	inst_inputRate = 0;
	inst_outputRate = 0;
	inst_pkt_delay = 0;
	inst_flit_delay = 0;

	flit_latency_sum_inst = 0;
	pkt_latency_sum_inst = 0;
	flit_hops_sum_inst = 0;

	total_flit_in = 0;
	total_pkt_in = 0;
	total_flit_out = 0;
	total_pkt_out = 0;

	total_flit_delay = 0;
	total_pkt_delay = 0;

	total_flit_latency = 0;
	total_pkt_latency = 0;
	total_flit_hops = 0;
	

}

void IcProbe::updateStat(){
	if ( g_simCount>gc_statEd?gc_statEd:g_simCount - gc_statOp >0 ){
		ULL cy_dr = g_simCount>gc_statEd?gc_statEd:g_simCount - gc_statOp;
		avg_inputRate = (float)(flit_in * gc_FLITSIZE * 8) / (cy_dr * g_clkPeriod);
	}
	if (end_cycle - beg_cycle > 0)
		avg_inputRate2 = (float)(flit_in * gc_FLITSIZE * 8) / ((end_cycle - beg_cycle) * g_clkPeriod);
	
	inst_inputRate = (float)(flit_in_inst * gc_FLITSIZE * 8) / (gc_statRes * g_clkPeriod);		
}

void IcProbe::cleanStat(){
	flit_in_inst = 0;
	pkt_in_inst = 0;

	inst_inputRate = 0;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// OcProbe
OcProbe::OcProbe(){
	this->pkt_delay_sum = 0;
	this->flit_delay_sum = 0;

	this->flit_out = 0;
	this->pkt_out = 0;

	this->avg_pkt_delay = 0;
	this->avg_flit_delay = 0;

	this->avg_outputRate = 0;
	this->avg_outputRate2 = 0;

	this->beg_cycle = 0;
	this->end_cycle = 0;

	for (int i=0; i<NUM_VCS; i++){
		this->enterIc_time[i] = 0;
		this->enterNoc_time[i] =0;
	}
	{
		flit_out_inst = 0;
		pkt_out_inst = 0;
		pkt_delay_sum_inst = 0;
		flit_delay_sum_inst = 0;
		inst_pkt_delay = 0;
		inst_flit_delay = 0;
		inst_outputRate = 0;
	}
}

void OcProbe::updateStat(){
	if(pkt_out != 0)
		avg_pkt_delay = (float)pkt_delay_sum/pkt_out;
	if(flit_out != 0)
		avg_flit_delay = (float)flit_delay_sum/flit_out;
	if ( g_simCount>gc_statEd?gc_statEd:g_simCount - gc_statOp >0){
		ULL cy_dr = g_simCount>gc_statEd?gc_statEd:g_simCount - gc_statOp;
		avg_outputRate = (float)(flit_out * gc_FLITSIZE * 8) / (cy_dr * g_clkPeriod);
	}
	if (end_cycle - beg_cycle > 0)
		avg_outputRate2 = (float)(flit_out * gc_FLITSIZE * 8) / ((end_cycle - beg_cycle) * g_clkPeriod);	// Gbps
	
	//
	{
		if (pkt_out_inst != 0)
			inst_pkt_delay = (float)pkt_delay_sum_inst/pkt_out_inst;
		if (flit_out_inst != 0)
			inst_flit_delay = (float)flit_delay_sum_inst/flit_out_inst;
		inst_outputRate = (float)(flit_out_inst * gc_FLITSIZE * 8) / (gc_statRes * g_clkPeriod);
	}
}

void OcProbe::cleanStat(){
	flit_out_inst = 0;
	pkt_out_inst = 0;
	pkt_delay_sum_inst = 0;
	flit_delay_sum_inst = 0;

	inst_pkt_delay = 0;
	inst_flit_delay = 0;
	inst_outputRate = 0;
}
//////////////////////////////////////////////////////////////////////////
void Tracker::cleanStat(){
	for (int i=0; i< tileProbes.size(); i++)
		tileProbes[i].cleanStat();

	inst_total_flit_latency = 0;
	inst_total_pkt_latency = 0;
	inst_total_flit_hops = 0;

	inst_total_flit_send = 0;
	inst_total_pkt_send = 0;
	inst_total_flit_recved = 0;
	inst_total_pkt_recved = 0;

	inst_pkt_swDelay = 0;		// package switch Delay
	inst_flit_swDelay = 0;		// flit switch delay
	inst_flit_latency = 0;
	inst_pkt_latency = 0;
	inst_flit_hops = 0;
	inst_inputRate = 0;
	inst_outputRate = 0;
}

 void Tracker::updateStat(){
	// instance

	ULL total_flit_swDelay = 0,
		total_pkt_swDelay = 0,
		total_flits_swOut = 0,
		total_pkts_swOut = 0;
	ULL inst_total_flit_swDelay = 0,
		inst_total_pkt_swDelay = 0,
		inst_total_flits_swOut = 0,
		inst_total_pkts_swOut = 0;
	{
		total_flits_swOut = 0;
		total_pkts_swOut = 0;
		total_flit_swDelay = 0;
		total_pkt_swDelay = 0;

		total_flit_send = 0;
		total_pkt_send = 0;
		total_flit_recved = 0;
		total_pkt_recved = 0;

		total_flit_latency = 0;
		total_pkt_latency = 0;
		total_flit_hops = 0;

		// instance
		inst_total_flit_swDelay = 0;
		inst_total_pkt_swDelay = 0;
		inst_total_flits_swOut = 0;
		inst_total_pkts_swOut = 0;

		inst_total_flit_send = 0;
		inst_total_pkt_send = 0;
		inst_total_flit_recved = 0;
		inst_total_pkt_recved = 0;

		inst_total_flit_latency = 0;
		inst_total_pkt_latency = 0;
		inst_total_flit_hops = 0;
	}
	for (int i=0; i< tileProbes.size(); i++){
		tileProbes[i].updateStat();
		total_flits_swOut += tileProbes[i].total_flit_out;
		total_pkts_swOut += tileProbes[i].total_pkt_out;
		total_flit_swDelay += tileProbes[i].total_flit_delay;
		total_pkt_swDelay += tileProbes[i].total_pkt_delay;
		
		total_flit_send += tileProbes[i].icProbes[tileProbes[i].io_num-1].flit_in;
		total_pkt_send += tileProbes[i].icProbes[tileProbes[i].io_num-1].pkt_in;
		total_flit_recved += tileProbes[i].ocProbes[tileProbes[i].io_num-1].flit_out;
		total_pkt_recved += tileProbes[i].ocProbes[tileProbes[i].io_num-1].pkt_out;

		total_flit_latency += tileProbes[i].total_flit_latency;
		total_pkt_latency += tileProbes[i].total_pkt_latency;
		total_flit_hops += tileProbes[i].total_flit_hops;

		inst_total_flits_swOut += tileProbes[i].inst_total_flit_out;
		inst_total_pkts_swOut += tileProbes[i].inst_total_pkt_out;
		inst_total_flit_swDelay += tileProbes[i].inst_total_flit_delay;
		inst_total_pkt_swDelay += tileProbes[i].inst_total_pkt_delay;

		inst_total_flit_send += tileProbes[i].icProbes[tileProbes[i].io_num-1].flit_in_inst;
		inst_total_pkt_send += tileProbes[i].icProbes[tileProbes[i].io_num-1].pkt_in_inst;
		inst_total_flit_recved += tileProbes[i].ocProbes[tileProbes[i].io_num-1].flit_out_inst;
		inst_total_pkt_recved += tileProbes[i].ocProbes[tileProbes[i].io_num-1].pkt_out_inst;

		inst_total_flit_latency += tileProbes[i].flit_latency_sum_inst;
		tileProbes[i].flit_latency_sum_inst = 0;
		inst_total_pkt_latency += tileProbes[i].pkt_latency_sum_inst;
		tileProbes[i].pkt_latency_sum_inst = 0;
		inst_total_flit_hops += tileProbes[i].flit_hops_sum_inst;
		tileProbes[i].flit_hops_sum_inst = 0;
	}

	if (total_flits_swOut != 0)
		avg_flit_swDelay = (float)total_flit_swDelay / total_flits_swOut;
	if (total_pkts_swOut != 0)
		avg_pkt_swDelay = (float)total_pkt_swDelay / total_pkts_swOut;
	if (total_flit_recved != 0 ){
		avg_flit_latency = (float)total_flit_latency / total_flit_recved;
		avg_flit_hops = (float)total_flit_hops / total_flit_recved;
	}
	if (total_pkt_recved != 0)
		avg_pkt_latency = (float)total_pkt_latency / total_pkt_recved;

	if ( g_simCount>gc_statEd?gc_statEd:g_simCount - gc_statOp >0){
		ULL cy_dr = g_simCount>gc_statEd?gc_statEd:g_simCount - gc_statOp;
		avg_inputRate = (float)(total_flit_send * gc_FLITSIZE * 8) / (cy_dr * g_clkPeriod * g_tileNum);
		avg_outputRate = (float)(total_flit_recved * gc_FLITSIZE * 8) / (cy_dr * g_clkPeriod * g_tileNum);
	}
	// instance
	{
		if (inst_total_flits_swOut != 0)
			inst_flit_swDelay = (float)inst_total_flit_swDelay / inst_total_flits_swOut;
		if (inst_total_pkts_swOut != 0)
			inst_pkt_swDelay = (float)inst_total_pkt_swDelay / inst_total_pkts_swOut;
		if (inst_total_flit_recved != 0)
		{
			inst_flit_latency = (float)inst_total_flit_latency / inst_total_flit_recved;
			inst_flit_hops = (float)inst_total_flit_hops / inst_total_flit_recved;
		}
		if (inst_total_pkt_recved != 0)
			inst_pkt_latency = (float)inst_total_pkt_latency / inst_total_pkt_recved;
		
		inst_inputRate = (float)(inst_total_flit_send * gc_FLITSIZE * 8) / (gc_statRes * g_clkPeriod * g_tileNum);
		inst_outputRate = (float)(inst_total_flit_recved * gc_FLITSIZE * 8) / (gc_statRes * g_clkPeriod * g_tileNum);
	}
}

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

	sequence++;
	flitSeqOut	<< "SQ " << f.simdata.sequence
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tPK " << f.pktid
		<< "\tFL " << f.flitid
		<< "\tSR " << f.src 
		<< "\tDS " << f.dest << endl;
	
	// logger
	if(g_simCount >= gc_statOp && g_simCount <= gc_statEd)
	{
	//	flitPool.insert(sequence);
	}
}

void Tracker::enterInputChannel(UI tileId, UI portId, flit & f){
	trackerOut	<< "EtIC" 
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tSQ " << f.simdata.sequence
		<< "\tTL " << tileId 
		<< "\tPT " << portId 
		<< "\tVC " << f.vcid << endl;
	f.simdata.ICtimestamp = g_simCount;

	// logger
	if (g_simCount >= gc_statOp && g_simCount <= gc_statEd)
	{	
		IcProbe * icprobe = getIcProbe(tileId, portId);
		icprobe->flit_in++;
		icprobe->flit_in_inst++;
		if(f.type == TAIL || f.type == HDT){
			icprobe->pkt_in++;
			icprobe->pkt_in_inst++;
		}

		if(icprobe->beg_cycle == 0)
			icprobe->beg_cycle = g_simCount;

		if(portId == icprobe->icPtr->io_num-1){		// from IP
			f.simdata.itime = sc_time_stamp();
			f.simdata.itimestamp = g_simCount;

			NwtileProbe* tileProbe = getNwtileProbe(tileId);
			tileProbe->flitTo[f.dest]++;
			//tileProbe->flit_send++;

			if(f.type == TAIL || f.type == HDT){ 
				tileProbe->pktTo[f.dest]++;
				//tileProbe->pkt_send++;
			}
		}
	}
	
}

void Tracker::exitInputChannel(UI tileId, UI portId, flit & f){
	trackerOut	<< "ExIC" 
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tSQ " << f.simdata.sequence
		<< "\tTL " << tileId 
		<< "\tPT " << portId << endl;
	f.simdata.num_sw ++;
	f.simdata.ctime = sc_time_stamp();

	// logger
	if (g_simCount >= gc_statOp && g_simCount <= gc_statEd)
	{
		IcProbe * icprobe = getIcProbe(tileId, portId);
		icprobe->end_cycle = g_simCount;
	}
}

void Tracker::enterOutputChannel(UI tileId, UI portId, flit & f){
	OcProbe* ocprobe = getOcProbe(tileId, portId);
	trackerOut	<< "EtOC" 
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tSQ " << f.simdata.sequence
		<< "\tTL " << tileId 
		<< "\tPT " << portId << endl;

	// logger
	if (g_simCount >= gc_statOp && g_simCount <= gc_statEd)
	{
		if(ocprobe->beg_cycle == 0)
			ocprobe->beg_cycle = f.simdata.ICtimestamp;
	}
}
void Tracker::getVcAtOutputChannel(UI tileId, UI portId, flit & f){
	OcProbe* ocprobe = getOcProbe(tileId, portId);

	// logger
	if (g_simCount >= gc_statOp && g_simCount <= gc_statEd)
	{
		if(f.type == HDT || f.type == HEAD)
		{
			ocprobe->enterIc_time[f.vcid] = f.simdata.ICtimestamp;
			ocprobe->enterNoc_time[f.vcid] = f.simdata.itimestamp;
		}
	}
}

void Tracker::exitOutputChannel(UI tileId, UI portId, flit & f){
	trackerOut	<< "ExOC" 
		<< "\tTI "<< sc_time_stamp().value() 
		<< "\tSQ " << f.simdata.sequence
		<< "\tTL " << tileId 
		<< "\tPT " << portId << endl;
	f.simdata.ctime = sc_time_stamp();

	// logger
	if (g_simCount >= gc_statOp && g_simCount <= gc_statEd)
	{
		OcProbe* ocprobe = getOcProbe(tileId, portId);
		ocprobe->end_cycle = g_simCount;
		
		// for delay
		ocprobe->flit_out++;
		ocprobe->flit_delay_sum += g_simCount - f.simdata.ICtimestamp;
		ocprobe->flit_out_inst++;
		ocprobe->flit_delay_sum_inst += g_simCount - f.simdata.ICtimestamp;

		if(f.type == TAIL || f.type == HDT) 
		{
			ocprobe->pkt_out++;
			ocprobe->pkt_delay_sum += g_simCount - ocprobe->enterIc_time[f.vcid];

			ocprobe->pkt_out_inst++;
			ocprobe->pkt_delay_sum_inst += g_simCount - ocprobe->enterIc_time[f.vcid];
		}
		
		// for latency
		if(portId == ocprobe->ocPtr->io_num-1){		// to IP
			NwtileProbe* tileProbe = getNwtileProbe(tileId);
			//tileProbe->flit_recved++;
			tileProbe->flitFrom[f.src]++;
			tileProbe->flit_latency[f.src] += g_simCount - f.simdata.itimestamp;
			tileProbe->flit_hops[f.src] += f.simdata.num_sw; 

			tileProbe->flit_latency_sum_inst += g_simCount - f.simdata.itimestamp;
			tileProbe->flit_hops_sum_inst += f.simdata.num_sw;

			if(f.type == TAIL || f.type == HDT) 
			{
				tileProbe->pkt_latency[f.src] += g_simCount - ocprobe->enterNoc_time[f.vcid];
				tileProbe->pktFrom[f.src]++;

				tileProbe->pkt_latency_sum_inst += g_simCount - ocprobe->enterNoc_time[f.vcid];
				//tileProbe->pkt_recved++;
			}
		}
	}
}
