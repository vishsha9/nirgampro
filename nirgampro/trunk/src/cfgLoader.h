#ifndef __CFG_LOADER_H__
#define __CFG_LOADER_H__

#include <string>
#include <iostream>


using namespace std;

bool loadConfig(string fileName);

void printConfig(ostream & xout);

bool checkConfig();

#endif