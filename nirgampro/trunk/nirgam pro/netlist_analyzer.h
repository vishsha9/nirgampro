#ifndef _NETLIST_ANALYZER_H_
#define _NETLIST_ANALYZER_H_

#define ADJ_MAX 6
#define WIREPARA_NMAX 8

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <stack>
#include <map>
#include <vector>

using namespace std;

class Edge;

class Node{
public:
	string name;
	int nodeId;
	int adjNum;
	vector<Node*> adjs;
	vector<Edge*> edges;
	Node();
	Node(int nodeId);
};


class WirePara{
public:
	string name;
	string unit;
	WirePara();
	WirePara(string name, string unit);
};

class Edge{
public:
	int edgeId;
	int length;
	WirePara * wp;
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

AdjList* analyze(string filename, ostream & msg);

extern string neta_version;

#endif