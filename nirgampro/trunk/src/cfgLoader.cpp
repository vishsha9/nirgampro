#include <unistd.h>

#include "dirtool.h"
#include "config.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stdlib.h>

using namespace std;


bool loadConfig(string fileName){
	cout << "LOAD MAIN CONFIG : " << fileName << " ..."<< endl;
	ifstream config;
	string configHome = getDirOfFile(fileName);
	config.open(fileName.c_str());
	if (!config.is_open())
	{
		cerr <<"cannot open config file: "<< fileName << endl;
		return false;
	}	
	string sline, field;
	int lcount = 0;
	while (getline(config, sline))
	{
		lcount++;
		istringstream line(sline);
		line >> field;
		if(line.fail()){
			continue;
		}
		if (field[0] == '#')
			continue;
		//////////////////////////////////////////////////////////////////////////
		if (field == "WARMUP"){
			ULL value; line >> value; gc_simWarm = value;
		}
		else if (field == "TG_NUM"){
			ULL value; line >> value; gc_simTg = value; 
		}
		else if (field == "SIM_NUM"){
			ULL value; line >> value; gc_simNum = value;
		}
		//////////////////////////////////////////////////////////////////////////
		else if (field == "STAT_OP"){
			ULL value; line >> value; gc_statOp = value;
		}
		else if (field == "STAT_ED"){
			ULL value; line >> value; gc_statEd = value;
		}
		else if (field == "STAT_RES"){
			ULL value; line >> value; gc_statRes = value;
		}
		//////////////////////////////////////////////////////////////////////////
		else if (field == "CLK_FREQ"){
			double value; line >> value; gc_simFreq = value;
		}
		//////////////////////////////////////////////////////////////////////////
		else if (field == "SIM_MODE"){
			UI value; line >> value; gc_simMode = value;
		}
		else if (field == "TILE_TYPE"){
			UI value; line >> value; gc_tileType = value;
		}
		//////////////////////////////////////////////////////////////////////////
		else if (field == "VC_BUFS"){
			UI value; line >> value; gc_VC_BUFS = value;
		}
		else if (field == "FLITSIZE"){
			UI value; line >> value; gc_FLITSIZE = value;
		}
		else if (field == "HEAD_PAYLOAD"){
			UI value; line >> value; gc_HEAD_PAYLOAD = value;
		}
		else if (field == "DATA_PAYLOAD"){
			UI value; line >> value; gc_DATA_PAYLOAD = value;
		}
		//////////////////////////////////////////////////////////////////////////
		else if (field == "TOPO_FILE"){
			string value; line >> value; value = rwPathInLinuxStyl(value);
			gc_topoFile = getPathOffset(configHome, value);
		}
		else if (field == "APP_FILE"){
			string value; line >> value; value = rwPathInLinuxStyl(value);
			gc_appFile = getPathOffset(configHome, value);
		}
		else if (field == "IPMAP_FILE"){
			string value; line >> value; value = rwPathInLinuxStyl(value);
			gc_ipmapFile = getPathOffset(configHome, value);
		}
		else if (field == "SECMAP_FILE"){
			string value; line >> value; value = rwPathInLinuxStyl(value);
			gc_secmapFile = getPathOffset(configHome, value);
		}
		else if (field == "RESULT_HOME"){
			string value; line >> value; value = rwPathInLinuxStyl(value);
			value = getPathOffset(configHome, value);
			gc_resultHome = rwDirInLinuxStyl(value);
		}
		//////////////////////////////////////////////////////////////////////////
		else
			cerr << "Warning. Unrecognized field " << field << endl;
		if(line.fail()){
			cerr << "Error. Line " << lcount << " @ " << fileName << endl;
			return false;
		}
	}

	return true;
}

void printConfig(ostream & xout){
	xout 	<< "PRINT MAIN CONFIG:" << endl
			<< "-------------------------------------------------------" <<endl;
	xout	<< "Nirgam Home Dir:\t" << g_nirgamHome << endl
		<< "Main Config File:\t" << ga_configPath << endl
			<< "Simulation Mode:\t" << gc_simMode;
				if(gc_simMode == 1) xout << " (Standard Mode)" << endl;
				else if (gc_simMode == 2) xout << " (Debug Mode)" << endl;
				else if (gc_simMode == 3) xout << " (Selfcheck Mode)" << endl;
	xout	<< "Using Tile Type:\t" << gc_tileType;
				if(gc_tileType == 1) xout << " (Generic Tile)" << endl;
				else if (gc_tileType == 2) xout << " (Soclib Tile)" << endl;		
	xout	<< "Topo Config File:\t" << gc_topoFile << endl;
if(gc_tileType == 1)
	xout	<< "App Config File:\t" << gc_appFile << endl;
else if (gc_tileType == 2) {
	xout	<< "IpMap Config File:\t" <<gc_ipmapFile << endl
			<< "SecMap Config File:\t" << gc_secmapFile << endl;
}
	xout	<< endl
			<< "Clock Frequency: " << gc_simFreq << " GHz" << endl
			<< "0\tsimWarm\t\t\tsimTg\tsimNum" << endl
			<< "|-------|-----------------------|-------|" <<  endl
			<< "0(cyc)\t" <<  gc_simWarm << "\t\t\t" << gc_simTg << "\t" << gc_simNum << endl
			<< endl
			<< "\tstatOP\tstatRes\t\tstatEd" <<  endl
			<< "\t|-------|---|-----------|" <<  endl
			<< "\t" <<  gc_statOp << "\t" << gc_statRes << "\t\t" << gc_statEd << endl
			<< endl
			<< "VC buffer size:\t\t" << gc_VC_BUFS << " flits" << endl
			<< "flit fize\t\t" << gc_FLITSIZE << " bytes" <<endl
			<< "Head payload\t\t" << gc_HEAD_PAYLOAD << " bytes" << endl
			<< "Data payload\t\t" << gc_DATA_PAYLOAD << " bytes" << endl
			<< endl
			<< "Result Dir:\t\t" << gc_resultHome <<  endl;
	xout	<< "-------------------------------------------------------" <<endl;
}


bool checkConfig(){
	cout << "CHECK MAIN CONFIG : " << ga_configPath << " ..."<< endl;
	bool ret = true;
		if( gc_tileType < 1 || gc_tileType > 2 ){
		cerr << "Tile Type error: "<< gc_tileType <<" { 1 ->Generic Tile; 2 ->Soclib Tile }" << endl;
		ret = false;
		return ret;
	}
	if ( gc_simMode < 1 || gc_simMode > 3 ){
		cerr << "Simulation Mode error : "<< gc_simMode <<" { 1 ->Standard; 2 ->Debug; 3 ->SelfCheck }" << endl;
		ret = false;
		return ret;
	}
	
	ifstream file;
	file.open(gc_topoFile.c_str());
	if(!file.is_open()){
		cerr << "Topo file: Can't open " << gc_topoFile << endl;
		ret = false;
	}
	file.close();
	
	/*
	if tileType == 1, nirgam use appFile to map generic IP.
	else if tileType == 2, nirgam use ipmapFile and secmapFile to map soclib IP
	*/
	if(gc_tileType == 1){
		file.open(gc_appFile.c_str());
		if(!file.is_open()){
			cerr << "App file: Can't open " << gc_appFile << endl;
			ret = false;
		}
		file.close();
	}
	else if(gc_tileType == 2){
		file.open(gc_ipmapFile.c_str());
		if(!file.is_open()){
			cerr << "IP map file: Can't open " << gc_ipmapFile << endl;
			ret = false;
		}
		file.close();
		
		file.open(gc_secmapFile.c_str());
		if(!file.is_open()){
			cerr << "Sec map file: Can't open " << gc_secmapFile << endl;
			ret = false;
		}
		file.close();
	}
	return ret;
}