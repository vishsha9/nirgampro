
/*
 * OutputChannel.h
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
/// \file OutputChannel.h
/// \brief Defines module OutputChannel
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_OP_CHANNEL_
#define	_OP_CHANNEL_

#include "systemc.h"
#include <string>
#include <fstream>
#include <iostream>

//#include "switch_reg.h"
#include "../flit.h"
#include "../credit.h"
#include "../../extern.h"
#include "InnerSigs.h"


using namespace std;

///////////////////////////////////////////////////////////////
/// \brief register data structure
///
/// used in OC to store a single flit 
///////////////////////////////////////////////////////////////
struct switch_reg {
	flit	val;	///< flit stored in register
	bool	free;	///< register status (0 - register is occupied, 1 - register is free)

	/// Constructor
	switch_reg() {
		free = true;
	}
};

//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent an Output Channel
///
/// This module defines an Output Channel in a network tile
//////////////////////////////////////////////////////////////////////////
struct OutputChannel : public sc_module {
	/// Constructor
	SC_HAS_PROCESS(OutputChannel);
	OutputChannel(sc_module_name OutputChannel, UI io_num);

	UI io_num;

	// PORTS ////////////////////////////////////////////////////////////////////////////////////
	sc_in<flit>* inport;		///< input data/flit ports (one for each IC)
	sc_out<bool>* inReady;		///< output port to send ready signal to IC
	sc_in<bool>  switch_cntrl;		///< clock input port
	sc_out<flit> outport;			///< output data/flit port
	sc_in<creditLine> credit_in[NUM_VCS];	///< input port to recieve credit info (buffer status) from ICs of neighbor tiles
	// PORTS END ////////////////////////////////////////////////////////////////////////////////////
	
	bool isCoreIO(UI i);
	// PROCESSES ///////////////////////////////////////////////////////////////////////////////////////////////
	void innerConnect(UI ioId,
					sc_clock & switch_cntrl,
					Sigs_IcOc & sigs_InOut,
					Sigs_OcIp & sigs_OcIp,
					sc_out<flit>& op_port
					);
	void entry();			///< reads and processes incoming flit
	void closeLogs();		///< closes logfiles at the end of simulation and computes performance stats
	/// sets tile ID and id corresponding to port directions
	void setTileID(UI tileID);
	// PROCESSES END //////////////////////////////////////////////////////////////////////////////////////////
	
	// VARIABLES //////////////////////////////////////////////////////////////////////////////////////////
	UI tileID;	///< unique tile ID
	UI cntrlID;	///< control ID to identify channel direction (N, S, E, W, C)
		
	//switch_reg*	r_in;	///< registers to store flit from inport, one for each inport
	vector<switch_reg> r_in;
	//switch_reg	r_vc[NUM_VCS];	///< registers, one for each next VCID
	vector<switch_reg> r_vc;

	ULL latency;			///< total latency
	ULL num_pkts;			///< total number of packets
	ULL num_flits;			///< total number of flits
	ULL input_time[NUM_VCS];			///< generation timestamp of head flit of a packet
	float avg_latency;		///< average latency (in clock cycles) per packet
	float avg_latency_flit;		///< average latency (in clock cycles) per flit
	float avg_throughput;		///< average throughput (in Gbps)
	
	int beg_cycle;			///< clock cycle in which first flit is recieved in the channel
	int end_cycle;			///< clock cycle in which last flit leaves the channel
	int total_cycles;		///< total number of clock cycles
	// VARIABLES END //////////////////////////////////////////////////////////////////////////////////////////
};
#endif
