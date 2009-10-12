#ifndef _SLTARGET_INTERFACE_H_
#define _SLTARGET_INTERFACE_H_

#include <systemc.h>
#include <list>
#include <cassert>
#include "fifo.h"
#include "../ipcore.h"

#include "defines.h"
#include "vci_initiator.h"

struct TargetInterface : public ipcore
{
	SC_CTOR(TargetInterface);

	void transition(void);
	void recv(void);
	void genMoore(void);

	
	bool ready; //indicate RSPACK, means interface is ready to receive response 
	bool accepted; //indicate T has accept cur request, thus go to next request

	//for sending use
	UI pktid;
	UI flitid;

	//for receiving use
	bool partly;
	UI	cur_src;
	UI cur_pktid;

	soclib::caba::VciInitiator<soclib::caba::vci_param>  vci_to_target;

	std::list<flit> recvbuf;
	fifo feedbuf;
};

#endif
