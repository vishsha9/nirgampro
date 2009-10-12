//#include "../config/constants.h"
#ifndef _SLSECMAP_H_
#define _SLSECMAP_H_

#include <string>
#include <vector>

#define UI unsigned int

struct secmap_entry
{
	std::string		secname;//secment name
	UI				base;	//secment base of this ip
	UI				size;	//secment size of the ip
	UI				id;		//location id in NoC
	bool			cacheability;	//wheather this secment is cacheable
};

// return true if store all secment maping info in secmap array
bool read_secmap(std::string filename, std::vector<secmap_entry> &secmap);
void print_secmap(std::vector<secmap_entry> &secmap);

#endif // _SECMAP_H_
