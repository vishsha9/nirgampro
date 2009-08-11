<<<<<<< .working
#include "netlist_analyzer.h"

string neta_version = "1.0";

enum Nest{MAIN, CONNECTION, WIRE};

void showError(ostream & out, int line, string msg){
	out << "- Error! Line " << line<< ": " << msg << endl;
	out << "> Exit analysis. Plz check the netlist file" << endl;
}
void showError(ostream & out, string msg){
	out << "- Error!: "<<msg << endl;
	out << "> Exit analysis. Plz check the netlist file" << endl;
}
void showWarning(ostream & out, int line, string msg){
	out << "- Warning, Line " << line<< ": " << msg << endl;
}
void showWarning(ostream & out, string msg){
	out << "- Warning: "<< msg << endl;
}
void showMsg(ostream & out, string msg){
	out << "> " << msg << endl;
}

bool checkWirePara(WirePara* wp, ostream & out, int line){
	bool ret = true;
	if(wp->name.size() == 0){
		showError(out, line, "WireType name must be defined, format: NAME [wirename]");
		ret = false;
	}
	if(wp->unit.size() == 0){
		showError(out, line, "Wire length unit name must be defined, format: UNIT [unit]");
		ret = false;
	}
	return ret;
}

/* 
* return NULL if error occured 
*
*/
AdjList* analyze(string filename, ostream & msg){
	ifstream input;
	AdjList* a;

	showMsg(msg, "Netlist Analyzer for Nirgam Pro, Version " + neta_version);
	showMsg(msg, "");

	input.open(filename.c_str());
	if(!input){
		showError(msg, "Can't find netlist file" + filename);
		return NULL;
	}
	showMsg(msg, "Input file: "+filename);

	string line, word;
	string fileversion;
	int lCount = 0;
	int nodeN = 0;
	
	int edgeId = 0;
	int max_n = 0;

	stack<Nest> st;
	map<string, WirePara*> wpm;
	map<int, Node*> nodem;

	WirePara * wp;

	showMsg(msg, "Start analysis ...");

	st.push(MAIN);

	getline(input, line);
	lCount++;
	istringstream stream(line);
	stream >> word >> fileversion;
	if(stream.fail()){
		showError(msg, lCount, "the first line of .nfn must contain analyzer version info: VERISON [version]");
		return NULL;
	}
	if(_stricmp(word.c_str(),"VERSION") != 0){
		showError(msg, lCount, "the first line of .nfn must contain analyzer version info: VERISON [version]");
		return NULL;
	}
	if(fileversion.compare(neta_version)!=0){
		showWarning(msg, lCount, "netlist analyzer version is not match the input file");
	}

	while(getline(input, line)){
		lCount++;
		istringstream stream(line);
		
		while(stream >> word){
			const char* cfd = word.c_str();
			// COMMENT
			if(cfd[0] == '#'){							
				break;
			}
			// TILENUM
			if(_stricmp(cfd,"TILENUM") == 0){
				int wordn;
				stream >> wordn;
				if(stream.fail()){
					showError(msg, lCount, "A number must be behind 'TILENUM'");
					return NULL;
				}
				a = new AdjList();
				a->nodeNum = wordn;
			}
			//
			else if(_stricmp(cfd,"NODE") == 0){
				int nodeid; 
				stream >> nodeid ;
				if(stream.fail()){
					showError(msg, lCount, "node definition format error: NODE [id] [name]");
					return NULL;
				}
				map<int, Node*>::iterator iter_n1 = nodem.find(nodeid);
				Node * node;
				if(iter_n1 == nodem.end()){
					node = new Node(nodeid);
					nodem.insert(pair<int, Node*>(nodeid, node));
					nodeN ++;
					if(nodeid > max_n)
						max_n = nodeid;
				}
				else{
					showError(msg, lCount, "node with this id has been defined");
					return NULL;
				}
			}
			// WIRE
			else if(_stricmp(cfd,"WIRE") == 0){
				if(st.top() != MAIN){
					showError(msg, lCount, "the block before was not closed");
					return NULL;
				}
				st.push(WIRE);
				wp = new WirePara();
			}
			// WIREEND
			else if(_stricmp(cfd,"WIREEND") == 0){
				if(st.top() != WIRE){
					showError(msg, lCount, "'WIREEND' occured without block start keyword 'WIRE'");
					return NULL;
				}
				if(checkWirePara(wp, msg, lCount)){
					wpm.insert(pair<string, WirePara *>(wp->name, wp));
					st.pop();
				}
				else
					return NULL;
			}
			// CONNECTION
			else if(_stricmp(cfd,"CONNECTION") == 0){
				if(st.top() != MAIN){
					showError(msg, lCount, "the block before was not closed");
					return NULL;
				}
				st.push(CONNECTION);
			}
			// CONNECTIONEND
			else if(_stricmp(cfd,"CONNECTIONEND") == 0){
				if(st.top() != CONNECTION){
					showError(msg, lCount, "'CONNECTIONEND' occured without block start keyword 'CONNECTION'");
					return NULL;
				}
				st.pop();
			}
			else if(_stricmp(cfd,"NAME") == 0){
				stream >> word;
				if(stream.fail()){
					showError(msg, lCount, "A string must be behind 'NAME'");
					return NULL;
				}
				if(st.top() == WIRE)
					wp->name = word;
			}
			else if(_stricmp(cfd,"LENGTHUNIT") == 0){
				stream >> word;
				if(stream.fail()){
					showError(msg, lCount, "A string must be behind 'LENGTHUNIT'");
					return NULL;
				}
				if(st.top() == WIRE)
					wp->unit = word;
			}
			else{
				if(st.top() == CONNECTION){
					map<string, WirePara*>::iterator iter = wpm.find(string(cfd));
					if(iter == wpm.end()){
						showError(msg, lCount, "No such WireType definition");
						return NULL;
					}
					wp = (*iter).second;

					int n1, n2, wlgth;
					stream >> n1 >> n2 >> wlgth;
					if(stream.fail()){
						showError(msg, lCount, "Nodes connect format error: [WireType][node1][node2][length]");
						return NULL;
					}
					Node * node1, * node2;
					Edge * edge;
					map<int, Node*>::iterator iter_n1 = nodem.find(n1);
					if(iter_n1 == nodem.end()){
						node1 = new Node(n1);
						nodem.insert(pair<int, Node*>(n1, node1));
						nodeN ++;
						if(n1 > max_n)
							max_n = n1;
					}
					else
						node1 = (*iter_n1).second;

					map<int, Node*>::iterator iter_n2 = nodem.find(n2);
					if(iter_n2 == nodem.end()){
						node2 = new Node(n2);
						nodem.insert(pair<int, Node*>(n2, node2));
						nodeN ++;
						if(n2 > max_n)
							max_n = n2;
					}
					else
						node2 = (*iter_n2).second;
					
					edge = new Edge();
					edge->wp = wp;
					edge->edgeId = edgeId;
					edgeId ++;
					edge->node1 = node1;
					edge->node2 = node2;
					edge->length = wlgth;

					node1->adjNum++;
					node1->adjs.push_back(node2);
					node1->edges.push_back(edge);

					node2->adjNum++;
					node2->adjs.push_back(node1);
					node2->edges.push_back(edge);

					a->edges.push_back(edge);
				}
				else{
					showError(msg, lCount, "Not defined KeyWord");
					return NULL;
				}
			}
		} //@ end of while(stream >> word)
	}//@ end of while(getline(input, line))
	a->edgeNum = a->edges.size();
	if(a->nodeNum != nodeN){
		a->nodeNum = nodeN;
		showWarning(msg, "TILENUM is not equal with the number of nodes appeared in CONNECT block");
	}
	for (int i=0; i<= max_n; i++)
	{
		map<int, Node*>::iterator iter = nodem.find(i);
		if(iter != nodem.end()){
			Node * node = (*iter).second;
			a->nodes.push_back(node);
		}
		else
			showError(msg, string("Maybe you forget node #")+string(1, i+'0'));
	}
	if(st.top() != MAIN){
		showWarning(msg, "the last block was not closed");
	}
	/*
	ok adjList nodes was empty, temperary stored at map<Node>. 
	ok adjList nodesNum was 0, should compare nodeNum in nfn file with the actual number, then warn if they are not equal.
	ok check node index from 0 to nodeNum to see whether nodes isolated exsit, that is a node with a given id is not in the list. this problem also cause the problem above.
	ok check the last block to comfirm whether it's closed.
	*/
	showMsg(msg, "Analysis complete\n");
	
	return a;
} 
=======
#include "netlist_analyzer.h"

string neta_version = "1.0";

enum Nest{MAIN, CONNECTION, WIRE};

void showError(ostream & out, int line, string msg){
	out << "- Error! Line " << line<< ": " << msg << endl;
	out << "> Exit analysis. Plz check the netlist file" << endl;
}
void showError(ostream & out, string msg){
	out << "- Error!: "<<msg << endl;
	out << "> Exit analysis. Plz check the netlist file" << endl;
}
void showWarning(ostream & out, int line, string msg){
	out << "- Warning, Line " << line<< ": " << msg << endl;
}
void showWarning(ostream & out, string msg){
	out << "- Warning: "<< msg << endl;
}
void showMsg(ostream & out, string msg){
	out << "> " << msg << endl;
}

bool checkWirePara(WirePara* wp, ostream & out, int line){
	bool ret = true;
	if(wp->name.size() == 0){
		showError(out, line, "WireType name must be defined, format: NAME [wirename]");
		ret = false;
	}
	if(wp->unit.size() == 0){
		showError(out, line, "Wire length unit name must be defined, format: UNIT [unit]");
		ret = false;
	}
	return ret;
}

/* 
* return NULL if error occured 
*
*/
AdjList* analyze(string filename, ostream & msg){
	ifstream input;
	AdjList* a;

	showMsg(msg, "Netlist Analyzer for Nirgam Pro, Version " + neta_version);
	showMsg(msg, "");

	input.open(filename.c_str());
	if(!input){
		showError(msg, "Can't find netlist file" + filename);
		return NULL;
	}
	showMsg(msg, "Input file: "+filename);

	string line, word;
	string fileversion;
	int lCount = 0;
	int nodeN = 0;
	
	int edgeId = 0;
	int max_n = 0;

	stack<Nest> st;
	map<string, WirePara*> wpm;
	map<int, Node*> nodem;

	WirePara * wp;
)()()((()))
	showMsg(msg, "Start analysis ...");

	st.push(MAIN);

	getline(input, line);
	lCount++;
	istringstream stream(line);
	stream >> word >> fileversion;
	if(stream.fail()){
		showError(msg, lCount, "the first line of .nfn must contain analyzer version info: VERISON [version]");
		return NULL;
	}
	if(_stricmp(word.c_str(),"VERSION") != 0){
		showError(msg, lCount, "the first line of .nfn must contain analyzer version info: VERISON [version]");
		return NULL;
	}
	if(fileversion.compare(neta_version)!=0){
		showWarning(msg, lCount, "netlist analyzer version is not match the input file");
	}

	while(getline(input, line)){
		lCount++;
		istringstream stream(line);
		
		while(stream >> word){
			const char* cfd = word.c_str();
			// COMMENT
			if(cfd[0] == '#'){							
				break;
			}
			// TILENUM
			if(_stricmp(cfd,"TILENUM") == 0){
				int wordn;
				stream >> wordn;
				if(stream.fail()){
					showError(msg, lCount, "A number must be behind 'TILENUM'");
					return NULL;
				}
				a = new AdjList();
				a->nodeNum = wordn;
			}
			//
			else if(_stricmp(cfd,"NODE") == 0){
				int nodeid; 
				stream >> nodeid ;
				if(stream.fail()){
					showError(msg, lCount, "node definition format error: NODE [id] [name]");
					return NULL;
				}
				map<int, Node*>::iterator iter_n1 = nodem.find(nodeid);
				Node * node;
				if(iter_n1 == nodem.end()){
					node = new Node(nodeid);
					nodem.insert(pair<int, Node*>(nodeid, node));
					nodeN ++;
					if(nodeid > max_n)
						max_n = nodeid;
				}
				else{
					showError(msg, lCount, "node with this id has been defined");
					return NULL;
				}
			}
			// WIRE
			else if(_stricmp(cfd,"WIRE") == 0){
				if(st.top() != MAIN){
					showError(msg, lCount, "the block before was not closed");
					return NULL;
				}
				st.push(WIRE);
				wp = new WirePara();
			}
			// WIREEND
			else if(_stricmp(cfd,"WIREEND") == 0){
				if(st.top() != WIRE){
					showError(msg, lCount, "'WIREEND' occured without block start keyword 'WIRE'");
					return NULL;
				}
				if(checkWirePara(wp, msg, lCount)){
					wpm.insert(pair<string, WirePara *>(wp->name, wp));
					st.pop();
				}
				else
					return NULL;
			}
			// CONNECTION
			else if(_stricmp(cfd,"CONNECTION") == 0){
				if(st.top() != MAIN){
					showError(msg, lCount, "the block before was not closed");
					return NULL;
				}
				st.push(CONNECTION);
			}
			// CONNECTIONEND
			else if(_stricmp(cfd,"CONNECTIONEND") == 0){
				if(st.top() != CONNECTION){
					showError(msg, lCount, "'CONNECTIONEND' occured without block start keyword 'CONNECTION'");
					return NULL;
				}
				st.pop();
			}
			else if(_stricmp(cfd,"NAME") == 0){
				stream >> word;
				if(stream.fail()){
					showError(msg, lCount, "A string must be behind 'NAME'");
					return NULL;
				}
				if(st.top() == WIRE)
					wp->name = word;
			}
			else if(_stricmp(cfd,"LENGTHUNIT") == 0){
				stream >> word;
				if(stream.fail()){
					showError(msg, lCount, "A string must be behind 'LENGTHUNIT'");
					return NULL;
				}
				if(st.top() == WIRE)
					wp->unit = word;
			}
			else{
				if(st.top() == CONNECTION){
					map<string, WirePara*>::iterator iter = wpm.find(string(cfd));
					if(iter == wpm.end()){
						showError(msg, lCount, "No such WireType definition");
						return NULL;
					}
					wp = (*iter).second;

					int n1, n2, wlgth;
					stream >> n1 >> n2 >> wlgth;
					if(stream.fail()){
						showError(msg, lCount, "Nodes connect format error: [WireType][node1][node2][length]");
						return NULL;
					}
					Node * node1, * node2;
					Edge * edge;
					map<int, Node*>::iterator iter_n1 = nodem.find(n1);
					if(iter_n1 == nodem.end()){
						node1 = new Node(n1);
						nodem.insert(pair<int, Node*>(n1, node1));
						nodeN ++;
						if(n1 > max_n)
							max_n = n1;
					}
					else
						node1 = (*iter_n1).second;

					map<int, Node*>::iterator iter_n2 = nodem.find(n2);
					if(iter_n2 == nodem.end()){
						node2 = new Node(n2);
						nodem.insert(pair<int, Node*>(n2, node2));
						nodeN ++;
						if(n2 > max_n)
							max_n = n2;
					}
					else
						node2 = (*iter_n2).second;
					
					edge = new Edge();
					edge->wp = wp;
					edge->edgeId = edgeId;
					edgeId ++;
					edge->node1 = node1;
					edge->node2 = node2;
					edge->length = wlgth;

					node1->adjNum++;
					node1->adjs.push_back(node2);
					node1->edges.push_back(edge);

					node2->adjNum++;
					node2->adjs.push_back(node1);
					node2->edges.push_back(edge);

					a->edges.push_back(edge);
				}
				else{
					showError(msg, lCount, "Not defined KeyWord");
					return NULL;
				}
			}
		} //@ end of while(stream >> word)
	}//@ end of while(getline(input, line))
	a->edgeNum = a->edges.size();
	if(a->nodeNum != nodeN){
		a->nodeNum = nodeN;
		showWarning(msg, "TILENUM is not equal with the number of nodes appeared in CONNECT block");
	}
	for (int i=0; i<= max_n; i++)
	{
		map<int, Node*>::iterator iter = nodem.find(i);
		if(iter != nodem.end()){
			Node * node = (*iter).second;
			a->nodes.push_back(node);
		}
		else
			showError(msg, string("Maybe you forget node #")+string(1, i+'0'));
	}
	if(st.top() != MAIN){
		showWarning(msg, "the last block was not closed");
	}
	/*
	ok adjList nodes was empty, temperary stored at map<Node>. 
	ok adjList nodesNum was 0, should compare nodeNum in nfn file with the actual number, then warn if they are not equal.
	ok check node index from 0 to nodeNum to see whether nodes isolated exsit, that is a node with a given id is not in the list. this problem also cause the problem above.
	ok check the last block to comfirm whether it's closed.
	*/
	showMsg(msg, "Analysis complete\n");
	
	return a;
} 
>>>>>>> .merge-right.r7
