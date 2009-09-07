
/*
 * InputChannel.h
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * Author: Lavina Jain
 *
 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file InputChannel.h
/// \brief Defines module InputChannel (reads and processes incoming flits)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_IP_CHANNEL_
#define	_IP_CHANNEL_

#include "systemc.h"
#include <string>
#include <iostream>
#include <fstream>

#include "fifo.h"
#include "../../constants.h"
#include "../credit.h"
#include "../../typedef.h"
#include "../flit.h"
#include "../../extern.h"
#include "InnerSigs.h"

/// required for stl
using namespace std;

///////////////////////////////////////////////
/// \brief Virtual channel (VC) data structure
///////////////////////////////////////////////
struct  VC {
	sc_uint<VCS_BITSIZE>	id;		///< unique identifier
	sc_uint<VCS_BITSIZE+1>	vc_next_id;	///< virtual channel id on next tile to which flit is supposed to go
	sc_uint<BUF_BITSIZE>	num_buf;	///< number of buffers in this VC
	//sc_uint<3>		vc_route;	
	port_id			vc_route;	///< routing decision (next hop) for the flits stored in this VC
	fifo			vcQ;		///< buffer (fifo queue)
};

//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent an Input Channel
///
/// This module defines an Input Channel in a network tile
//////////////////////////////////////////////////////////////////////////

struct ports_IC_VCA{
	sc_out<bool> vcRequest;		///< output port for sending request to VCA
	sc_in<bool> vcReady;		///< input port for ready signal from VCA
	sc_in<sc_uint<VCS_BITSIZE+1> >	nextVCID;	///< input port to recieve next VCID from VCA
	sc_out<port_id>  opRequest;	///< output port for sending OC requested to VCA
};

struct ports_IC_CTR{
	sc_out<request_type> rtRequest;		///< output port to send request to Controller
	sc_in<bool> rtReady;				///< input port to recieve ready signal from Controller
	sc_out<addr> destRequest;	///< output port to send destination address to Controller
	sc_out<addr> sourceAddress;	///< output port to send source address to Controller
	sc_in<port_id> nextRt;		///< input port to recieve routing decision (next hop) from Controller
};

struct ports_IC_OC{
	sc_out<flit>* outport;		///< ouput data/flit ports (one for each output channel)
	sc_in<bool>* outReady;		///< input ports for ready signal from OCs
};

struct InputChannel : public sc_module {
	/// Constructor
	SC_HAS_PROCESS(InputChannel);
	InputChannel(sc_module_name InputChannel, UI io_num);

	UI io_num;

	// PORTS //////////////////////////////////////////////////////////////////////////////////
	sc_in<bool> switch_cntrl;	///< input clock port

	sc_in<flit> inport;			///< input data/flit port
	
	// format 2->3
	ports_IC_OC ports_oc;
	ports_IC_VCA ports_VCA;
	ports_IC_CTR ports_ctr;

	sc_out<creditLine> credit_out[NUM_VCS];		///< output ports to send credit info (buffer status) to OC, VCA and Ctr
	
	// PORTS END //////////////////////////////////////////////////////////////////////////////
	
	
	bool isCoreIO(UI i);
	// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	void read_flit();			///< reads flit from i/p port and calls function to store it in buffer
	void store_flit_VC(flit*);	///< stores flit in buffer
	void route_flit();			///< routes the flit at the front of fifo buffer
	void routing_src(flit*);	///< routing function for algorithms containing entire path in header (source routing)
	void routing_dst(flit*);	///< routing function for algorithms containing destination address in header

	void transmit_flit();		///< transmits flit at the front of fifo to output port
	/// sets tile ID and id corresponding to port directions
	void setTileID(UI tileID);
	void resetCounts();			///< resets buffer counts to zero
	void closeLogs();			///< closes logfiles

	int reverse_route(int);		///< reverses route (to be used in future)
	// FUNCTIONS END /////////////////////////////////////////////////////////////////////////
	
	// VARIABLES /////////////////////////////////////////////////////////////////////////////
	VC	vc[NUM_VCS];	///< Virtual channels
	
	int	cntrlID;		///< Control ID to identify channel direction
	// if cntrlId == num_op then this InputChannel connect to IP

	UI	tileID;			///< Tile ID
	
	UI	numBufReads;	///< number of buffer reads in the channel
	UI	numBufWrites;	///< number of buffer writes in the channel
	UI	numBufsOcc;		///< number of occupied buffers
	UI	numVCOcc;		///< number of occupied virtual channels
	ULL sim_count;		///< keeps track of number of clock cycles
	// VARIABLES END /////////////////////////////////////////////////////////////////////////
};

#endif

