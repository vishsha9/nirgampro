#ifndef __RST_ANALYZER_H__
#define __RST_ANALYZER_H__

#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

class resultAnalyzer{
public:
	resultAnalyzer();
protected:
	ifstream trackerOut;
	ifstream flitSeqOut;
};

#endif