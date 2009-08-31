#ifndef __SIMPLE_WIRE_H__
#define __SIMPLE_WIRE_H__
#include "wires.h"

struct simpleWire: public wires{
	SC_HAS_PROCESS(simpleWire);
	simpleWire(sc_module_name wires, UI length);

	double length;
	/// DelayTime' unit is ps
	UI getDelayTime();
}; 

#endif 

