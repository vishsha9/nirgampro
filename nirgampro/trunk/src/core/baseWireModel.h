#ifndef _BASE_WIRE_MODEL_H_
#define _BASE_WIRE_MODEL_H_

#include <string>
#include "../typedef.h"

class baseWireModel{
public:
	virtual bool setFieldByName(string name, double val)=0;
	//virtual wire* getWire(string wirename)=0;
	virtual baseWireModel* clone()=0;
	virtual UI getDelayTime()=0;
};


#endif

