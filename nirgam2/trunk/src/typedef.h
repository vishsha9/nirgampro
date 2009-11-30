#ifndef __TYPE_DEF_H__
#define __TYPE_DEF_H__

#include "systemc.h"
#include "core/credit.h"
#include "constants.h"
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

enum flit_type{
	HEAD,
	DATA,
	TAIL,
	HDT
};

#define UI  unsigned int
#define UL  unsigned long
#define ULL unsigned long long


typedef sc_uint<ADDR_SIZE> addr;
//typedef port_id sc_uint<3>;
typedef sc_uint<4> port_id;


typedef  sc_in<creditLine> sc_in_creditLine_Array[NUM_VCS];
typedef  sc_out<creditLine> sc_out_creditLine_Array[NUM_VCS];

#endif

