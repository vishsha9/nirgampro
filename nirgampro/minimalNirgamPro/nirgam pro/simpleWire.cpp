#include "simpleWire.h"

bool simpleWire::setFieldByName(string name, double val){
	return true;
}

simpleWire* simpleWire::clone(){
	return new simpleWire(*this);
}

UI simpleWire::getDelayTime(){
	return 33;
}