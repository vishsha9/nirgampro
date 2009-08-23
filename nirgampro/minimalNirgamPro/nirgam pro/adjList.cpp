#include "netlist_analyzer.h"


//////////////////////////////////////////////////////////////////////////
// class Node
Node::Node(int nodeId){
	this->nodeId = nodeId;
	this->adjNum = 0;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// WirePara
WirePara::WirePara(){
}

WirePara::WirePara(string name, string unit){
	this->name = name;
	this->unit = unit;
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// AdjList
AdjList::AdjList(){
}

AdjList::AdjList(int nodeNum){
	this->nodeNum = nodeNum;
}
AdjList::~AdjList(){
	for (int i=0; i<this->nodes.size(); i++)
	{
		delete nodes[i];
	}
	for (int i=0; i<this->edges.size(); i++)
	{
		delete edges[i];
	}
}
//////////////////////////////////////////////////////////////////////////
