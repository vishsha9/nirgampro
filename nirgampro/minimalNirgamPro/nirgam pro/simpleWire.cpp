#include "simpleWire.h"

simpleWire::simpleWire(sc_module_name wires, UI length): wires(wires){
	this->length = length;
	this->delayTime = getDelayTime();
}

UI simpleWire::getDelayTime(){	
	return length;
}