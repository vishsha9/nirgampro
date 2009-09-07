#include "topoAnalyzer.h"

#include "../wireModel/simpleWire.h"
#include "../wireModel/ptmModel.h"

bool wireModelForName(string name, baseWireModel* &wirepara){
	if( strcasecmp(name.c_str(), "ptmwire_top") == 0){
		wirepara = new ptmModel();
		wirepara->setFieldByName("layer", PTM_TOP);
		return true;
	}
	else if( strcasecmp(name.c_str(), "ptmwire_local") == 0){
		wirepara = new ptmModel();
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