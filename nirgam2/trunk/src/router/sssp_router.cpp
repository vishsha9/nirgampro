#include "sssp_router.h"

#include "../global.h"
///////////////////////////////////////////////////////////////////////////
//  Dijkstra Algorithm for Single Source Shortest Path
//////////////////////////////////////////////////////////////////////////

int extract_min(map<int, double>& Q, set<int> & sQ){
	set<int>::const_iterator map_it = sQ.begin();
	int index = -1;
	double dis= -1;
	while (map_it != sQ.end()){
		map<int, double>::iterator mapQ = Q.find(*map_it);
		if (mapQ->second != -1 && (mapQ->second < dis || dis == -1 ) ){
			dis = mapQ->second;
			index = *map_it;
		}
		++map_it;
	}
	sQ.erase(index);
	return index;
}

void relax(int root, int nodeId, AdjList *a, map<int, double>& Q, map<int, int>* dest){
	Node * src = a->nodes[nodeId];
	double curDis= Q.find(nodeId)->second;
	for (int i=0; i<src->adjNum; i++)
	{
		int dir = dest->find(nodeId)->second;
		map<int, double>::iterator iter = Q.find(src->adjs[i]->nodeId);
		map<int, int>::iterator diter = dest->find(src->adjs[i]->nodeId);
		double tmpDis = curDis + src->edges[i]->wp->getDelayTime();
		if(iter->second == -1 || iter->second > tmpDis){
			iter->second = tmpDis;
			if (nodeId == root)
				diter->second = src->adjs[i]->nodeId;
			else
				diter->second = dir;
		}
	}
}

map<int, int>* dijkstra(int root, AdjList * a){
	vector<int> S;
	map<int, double> Q;
	set<int> sQ;
	map<int, int>* dest=new map<int, int>();
	for (int i=0; i<a->nodeNum; i++){
		sQ.insert(i);

		if (a->nodes[i]->nodeId == root){
			Q.insert(pair<int, double>(i, 0));
			dest->insert(pair<int, int>(i, root));
		}
		else{
			Q.insert(pair<int, double>(i, -1));
			dest->insert(pair<int, int>(i, -1));
		}
	}
	while (sQ.size() != 0){
		int u = extract_min(Q, sQ);
		relax(root, u, a, Q, dest);
		//printf("%d\n", u);
	}
	return dest;
}


sssp_router::sssp_router(){
	//
}

UI sssp_router::calc_next(UI ip_dir, ULL source_id, ULL dest_id){
	UI nextTile = opTable->find(dest_id)->second;
	if (nextTile == id)
		return portTable->size();
	UI dir = -1;
	idToDir(nextTile, &dir);
	return dir;
}

void sssp_router::initialize(){
	
}

void sssp_router::setID(UI tileid){
	this->id  = tileid;

	// setId() is after construction of each components
	// dijkstra need id, so it can't be placed in construction function
	// initialize() above is not be called
	// format :: all constructions completed in NWTile, then set ID. It's weird. if some work in initialization need id, it could be placed in construction function. The function more than its name will bewilder successive coders.
	this->opTable = dijkstra(this->id, g_a);
}