#ifndef __INNER_SIGS__
#define __INNER_SIGS__

#include "../../typedef.h"
#include "../../constants.h"
#include "../../core/flit.h"
#include "../../core/credit.h"

struct Sigs_IcCtl{
	sc_signal<request_type> rtReq;
	sc_signal<addr> destReq;
	sc_signal<addr> srcAddr;
	sc_signal<bool> rtReady;
	sc_signal<port_id> nextRt;
};

struct Sigs_IcVca{
	sc_signal<bool> vcReq;
	sc_signal<port_id> opReq;
	sc_signal<bool> vcReady;
	sc_signal<sc_uint<VCS_BITSIZE+1> >	nextvc;
};

struct Sigs_IcOc{
	sc_signal<flit> flit_sig;
	sc_signal<bool>	rdy;
};

struct Sigs_IcIp{
	sc_signal<flit>	flit_CS_IC;
};

struct Sigs_OcIp{
	sc_signal<flit> flit_OC_CR;
};

typedef sc_signal<creditLine> sc_sigs_creditLine[NUM_VCS];
typedef sc_in<creditLine> sc_in_creditLine[NUM_VCS];
#endif