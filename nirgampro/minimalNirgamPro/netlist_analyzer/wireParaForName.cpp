#include "netlist_analyzer.h"

#include "../nirgam pro/simpleWire.h"
#include "../nirgam pro/ptmWire.h"

bool wireParaForName(string name, WirePara* &wirepara){
	if( strcasecmp(name.c_str(), "ptmwire_top") == 0){
		wirepara = new ptm_para();
		wirepara->setFieldByName("layer", PTM_TOP);
		return true;
	}
	else if( strcasecmp(name.c_str(), "ptmwire_local") == 0){
		wirepara = new ptm_para();
		wirepara->setFieldByName("layer", PTM_LOCAL);
		return true;
	}
	else if( strcasecmp(name.c_str(), "simplewire") == 0){
		wirepara = new simpleWire();
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	/* template
	else if( strcasecmp(name.c_str(), "WireModelName") == 0){
	wirepara = new WireModelParaName();
	return true;
	}
	*/
	//////////////////////////////////////////////////////////////////////////
	return false;
}