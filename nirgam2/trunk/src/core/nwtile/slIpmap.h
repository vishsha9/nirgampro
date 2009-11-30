//#include "../config/constants.h"
#ifndef _SLIPMAP_H_
#define _SLIPMAP_H_

#include <string>
#include <vector>

#define UI unsigned int

struct ipmap_entry
{
	std::string		ipname;//ipment name
	std::string 	type;
	UI				id;		//location id in NoC
};

// return true if store all ipment maping info in ipmap array
bool read_ipmap(std::string filename, std::vector<ipmap_entry> &ipmap);
void print_ipmap(std::vector<ipmap_entry> &ipmap);

#endif // _IPMAP_H_
