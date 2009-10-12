#ifndef _SLINITIATOR_INTERFACE_H_
#define _SLINITIATOR_INTERFACE_H_

#include <systemc.h>
#include <list>
#include <string>
#include <cassert>
#include "fifo.h"
#include "../ipcore.h"

#include "defines.h"
#include "vci_target.h"

struct InitiatorInterface : public ipcore
{
	SC_CTOR(InitiatorInterface);

	void transition(void);
	void recv(void);
	void genMoore(void);
	
	
	bool ready;	//change same time with CMDACK, means interface ready for request
	bool accepted;	//indicate I has accept cur response, thus go to next response

	//debug
	bool recv_flag;
	//debug

	//for sending use
	UI pktid;
	UI flitid;
	UI route;
	UI dest;

	//for receiving use
	bool partly;
	UI	recv_src;
	UI recv_pktid;

	soclib::caba::VciTarget<soclib::caba::vci_param>  vci_to_initiator;

	std::list<flit> recvbuf;
	fifo feedbuf;

	std::string name;
};

#endif
