#ifndef __SIMPLE_WIRE_H__
#define __SIMPLE_WIRE_H__

#include "wires.h"

struct simpleWire:public WirePara{
	bool setFieldByName(string name, double val);
	simpleWire* clone();
	UI getDelayTime();
};

#endif 

