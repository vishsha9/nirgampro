#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__

#include "systemc.h"

/////////////////////////////////////////
/// types of request to controller
////////////////////////////////////////
enum request_type {
	NONE,
	ROUTE,
	UPDATE
};

////////////////////////////////////////////////
/// types of routing algorithms: SOURCE, XY, OE
////////////////////////////////////////////////
enum routing_type {
	SOURCE,
	XY,
	OE
};

typedef sc_uint<ADDR_SIZE> addr;
//typedef port_id sc_uint<3>;
typedef sc_uint<4> port_id;
#endif

