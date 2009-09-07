#ifndef __SELF_CHECK_H__
#define __SELF_CHECK_H__

#include "ipcore.h"

#include <fstream>
#include <math.h>

using namespace std;

class selfCheck:public ipcore
{
public:
	SC_CTOR(selfCheck);

	void send();
	void recv();
};

#endif
