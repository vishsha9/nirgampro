#include "slSecmap.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

bool read_secmap(string filename, vector<secmap_entry> &secmap)
{
	ifstream mapfile(filename.c_str());
	
	if (!mapfile)
	{
		cerr << "file " << filename << " not exist" << endl;
		return false;
	}

	string line;
	secmap_entry e;

	while (getline(mapfile, line))
	{
		if (line.find_first_of("#") != line.npos)	//comment line
			continue;


		istringstream formatter(line);
		
		formatter >> e.secname;

		formatter >> showbase >> hex;
		formatter >> e.base;
		formatter >> e.size;
		
		formatter >> dec;
		formatter >> e.id;

		formatter >> boolalpha;
		formatter >> e.cacheability;
		
		secmap.push_back(e);
	}

	mapfile.close();

	return true;
}


void print_secmap(vector<secmap_entry> &secmap)
{
	for (int i = 0; i < secmap.size(); i++)
		cout << secmap[i].secname << "\t"
			 << secmap[i].base << "\t"
			 << secmap[i].size << "\t"
			 << secmap[i].id << "\t"
			 << secmap[i].cacheability << "\t"
			 << endl;
	
}
