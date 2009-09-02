#include "netlist_analyzer.h"


string neta_version = "1.5";

enum Nest{MAIN, CONNECTION, WIRE};


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


void showError(ostream & out, int line, string msg){
	out << "  - Error! Line " << line<< ": " << msg << endl;
	out << "> Exit analysis. Plz check the netlist file" << endl;
}
void showError(ostream & out, string msg){
	out << "  - Error!: "<<msg << endl;
	out << "> Exit analysis. Plz check the netlist file" << endl;
}
void showWarning(ostream & out, int line, string msg){
	out << "  - Warning, Line " << line<< ": " << msg << endl;
}
void showWarning(ostream & out, string msg){
	out << "  - Warning: "<< msg << endl;
}
void showMsg(ostream & out, string msg){
	out << "> " << msg << endl;
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
		showError(msg, "Can't find netlist file " + filename);
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
	map<string, vector<string>*> wpvalm;
	map<int, Node*> nodem;

	WirePara * wp;
	string wpname;
	vector<string> * wpval;

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
	if(strcasecmp(word.c_str(),"VERSION") != 0){
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
			//const char* cfd = word.c_str();
			// COMMENT
			if(word.c_str()[0] == '#'){							
				break;
			}

			if (st.top() == MAIN)
			{
				// TILENUM
				if(strcasecmp(word.c_str(),"TILENUM") == 0){
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
				else if(strcasecmp(word.c_str(),"NODE") == 0){
					int nodeid; 
					stream >> nodeid ;
					if(stream.fail()){
						showError(msg, lCount, "node definition format error: NODE [id]");
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
				else if(strcasecmp( word.c_str(), "WIRE") == 0){
					st.push(WIRE);
					string type;
					stream >> type;
					if(stream.fail()){
						showError(msg, lCount, "wirePara definition format error: WIRE [typeName]");
						return NULL;
					}

					if (wireParaForName(type, wp) == false){
						showError(msg, lCount, "Wire Type: "+ type + " was not defined");
						return NULL;
					}
					wpval = new vector<string>();
					//wp = new WirePara();
				}

				// CONNECTION
				else if(strcasecmp( word.c_str(), "CONNECTION") == 0){
					if(st.top() != MAIN){
						showError(msg, lCount, "the block before was not closed");
						return NULL;
					}
					st.push(CONNECTION);
				}

				else{
					showError(msg, lCount, "Syntax Error");
					return NULL;
				}
			}

			else if (st.top() == WIRE){
				if(strcasecmp( word.c_str(), "NAME") == 0){
					stream >> word;
					if(stream.fail()){
						showError(msg, lCount, "value of 'NAME' is not specified");
						return NULL;
					}
					wpname = word;
				}
				// WIREEND
				else if(strcasecmp( word.c_str(), "WIREEND") == 0){
					if(st.top() != WIRE){
						showError(msg, lCount, "'WIREEND' occurred without block start keyword 'WIRE'");
						return NULL;
					}
					if(wpname.size() != 0){
						wpm.insert(pair<string, WirePara *>(wpname, wp));
						st.pop();
					}
					else{
						showError(msg, lCount, "WireType name must be defined, format: NAME [wirename]");
						return NULL;
					}
					wpvalm.insert(pair<string, vector<string>*>(wpname, wpval));
					wpname = "";
				}
				else{
					string valstr;
					stream >> valstr;
					if(stream.fail()){
						showError(msg, lCount, "value of '"+ word + "' is not specified");
						return NULL;
					}
					if (strcasecmp(valstr.c_str(), "var") == 0){
						wpval->push_back(word);
					}
					else{
						double val = strToValue(valstr);
						if (wp->setFieldByName(word, val) == false){
							showError(msg, lCount, "Error occurred in parameter setting. Parameter is not existed or value is not correct");
							return NULL;
						}
					}
				}
			}

			else if(st.top() == CONNECTION){
				// CONNECTIONEND
				if(strcasecmp( word.c_str(), "CONNECTIONEND") == 0){
					if(st.top() != CONNECTION){
						showError(msg, lCount, "'CONNECTIONEND' occurred without block start keyword 'CONNECTION'");
						return NULL;
					}
					st.pop();
				}
				else{
					map<string, WirePara*>::iterator iter = wpm.find(word);
					if(iter == wpm.end()){
						showError(msg, lCount, "No such WireType definition");
						return NULL;
					}
					wp = (*iter).second->clone();

					int n1, n2;
					stream >> n1 >> n2;
					if(stream.fail()){
						showError(msg, lCount, "Nodes connect format error: [WireType][node1][node2][length]");
						return NULL;
					}

					//
					map<string, vector<string>*>::iterator varIter = wpvalm.find(word);
					if(varIter == wpvalm.end()){
						showError(msg, lCount, "This WireType definition has no variable parameter");
						return NULL;
					}
					vector<string>* vars = varIter->second;
					string valstr;
					for(int i=0; i<vars->size(); i++){
						string field = (*vars)[i];
						stream >> valstr;
						if(stream.fail()){
							showError(msg, lCount, "value of '"+ field + "' is not specified");
							return NULL;
						}
						double val = strToValue(valstr);
						if (wp->setFieldByName(field, val) == false){
							showError(msg, lCount, "Error occurred in parameter setting. Parameter is not existed or value is not correct");
							return NULL;
						}
					}
					
					//
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

					node1->adjNum++;
					node1->adjs.push_back(node2);
					node1->edges.push_back(edge);

					node2->adjNum++;
					node2->adjs.push_back(node1);
					node2->edges.push_back(edge);

					a->edges.push_back(edge);
				}
			}
			else{
				showError(msg, lCount, "Syntax Error");
				return NULL;
			}
		} //@ end of while(stream >> word)
	}//@ end of while(getline(input, line))
	a->edgeNum = a->edges.size();
	if(a->nodeNum != nodeN){
		a->nodeNum = nodeN;
		showWarning(msg, "TILENUM is not equal with the number of nodes you defined");
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
	
	map<string, vector<string>*>::reverse_iterator riter = wpvalm.rbegin();
	while(riter!=wpvalm.rend()){
		delete riter->second;
		riter++;
	}

	return a;
} 
