#ifndef __SIMPLE_WIRE_H__
#define __SIMPLE_WIRE_H__

#include "../core/baseWireModel.h"

struct simpleWire:public baseWireModel{
	bool setFieldByName(string name, double val);
	simpleWire* clone();
	UI getDelayTime();
};

#endif 

