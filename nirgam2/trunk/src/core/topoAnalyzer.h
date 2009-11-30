#ifndef _TOPO_ANALYZER_H_
#define _TOPO_ANALYZER_H_

#define ADJ_MAX 6
#define WIREPARA_NMAX 8

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <stack>
#include <map>
#include <vector>

#include "../platform.h"
#include "notation.h"
#include "wireModule.h"


using namespace std;


class Edge;

class Node{
public:
	int nodeId;
	int adjNum;
	vector<Node*> adjs;
	vector<Edge*> edges;
	Node();
	Node(int nodeId);
};


class Edge{
public:
	int edgeId;
	baseWireModel * wp;
	Node * node1;
	Node * node2;
};

class AdjList{
public:
	vector<Node*> nodes;
	vector<Edge*> edges;
	int nodeNum;
	int edgeNum;

	AdjList();
	AdjList(int nodeNum);
	~AdjList();
};


bool wireModelForName(string name, baseWireModel* &wirepara);

AdjList* analyze(string filename, ostream & msg);

extern string neta_version;

#endif