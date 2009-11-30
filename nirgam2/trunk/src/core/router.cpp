#include "router.h"

bool router::setPortTable(vector<UI> * portTable){
	this->portTable = portTable;
	return true;
}

int router::dirToId(UI dir, UI* id){
	if(dir < portTable->size()){
		*id = portTable->at(dir);
		return 0;
	}
	else
		return -1;
}

int router::idToDir(UI id, UI* dir){
	for (int i=0; i<portTable->size(); i++)
	{
		if(portTable->at(i) == id){
			*dir = i;
			return 0;
		}
	}
	return -1;
}



