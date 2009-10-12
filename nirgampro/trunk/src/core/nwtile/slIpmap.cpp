#include "slIpmap.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

bool read_ipmap(string filename, vector<ipmap_entry> &ipmap)
{
	ifstream mapfile(filename.c_str());
	
	if (!mapfile)
	{
		cerr << "file " << filename << " not exist" << endl;
		return false;
	}

	string line;
	ipmap_entry e;

	while (getline(mapfile, line))
	{
		if (line.find_first_of("#") != line.npos)	//comment line
			continue;


		istringstream formatter(line);
		
		formatter >> e.ipname;
		formatter >> e.type;
		formatter >> dec;
		formatter >> e.id;

		ipmap.push_back(e);
	}

	mapfile.close();

	return true;
}


void print_ipmap(vector<ipmap_entry> &ipmap)
{
	for (int i = 0; i < ipmap.size(); i++)
		cout << ipmap[i].ipname << "\t"
			 << ipmap[i].type << "\t"
			 << ipmap[i].id << "\t"
			 << endl;
	
}
