#include "router.h"

bool router::setIndexTable(vector<UI> * indexTable){
	this->indexTable = indexTable;
	return true;
}

int router::dirToId(UI dir, UI* id){
	if(dir < indexTable->size()){
		*id = indexTable->at(dir);
		return 0;
	}
	else
		return -1;
}

int router::idToDir(UI id, UI* dir){
	for (int i=0; i<indexTable->size(); i++)
	{
		if(indexTable->at(i) == id){
			*dir = i;
			return 0;
		}
	}
	return -1;
}



