
/*
 * VCAllocator.h
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
/// \file VCAllocator.h
/// \brief Defines virtual channel allocator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef	_VC_ALLOC_
#define	_VC_ALLOC_

#include "systemc.h"
#include <string>
#include <fstream>
#include <iostream>

#include "../credit.h"
#include "../../constants.h"
#include "../flit.h"
#include "../../typedef.h"
#include "../../extern.h"
#include "InnerSigs.h"
/// required for stl
using namespace std;


/////////////////////////////////////////////////////////////////////////////
/// \brief Module to represent a Virtual channel allocator
///
/// This module defines a Virtual channel allocator (VCA) in a network tile
/////////////////////////////////////////////////////////////////////////////
struct VCAllocator : public sc_module {
	/// Constructor
	SC_HAS_PROCESS(VCAllocator);
	VCAllocator(sc_module_name VCAllocator, UI io_num);

	UI io_num;
	// PORTS //////////////////////////////////////////////////////////////////////////////////	
	sc_in<bool> switch_cntrl;				///< input clock port
	sc_in<bool>* vcRequest;				///< input ports for request signals from ICs
	sc_in<port_id>* opRequest;			///< input ports to recieve output channel requested from ICs
	sc_out<sc_uint<VCS_BITSIZE+1> >* nextVCID;	///< output ports to send next VCID to ICs
	sc_out<bool>* vcReady;				///< output ports to send ready signal to ICs
	
	sc_in_creditLine_Array *credit_in;
	//sc_in<creditLine> (*credit_in)[NUM_VCS];	///< input ports to recieve credit info (buffer status) from ICs
	// PORTS END //////////////////////////////////////////////////////////////////////////////////	

	// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	void allocate_VC();	///< process sensitive to VC request
	void update_credit();	///< process sensitive to credit input,  updates credit (buffer status)
	/// sets tile ID and id corresponding to port directions
	void setTileID(UI tileID);
	sc_uint<VCS_BITSIZE+1> getNextVCID(int,int);	///< allocates vcid in the requested channel
	// FUNCTIONS END /////////////////////////////////////////////////////////////////////////////
	
	// VARIABLES /////////////////////////////////////////////////////////////////////////////
	bool (*vcFree)[NUM_VCS];	///< status registers to store credit info (buffer status)
	UI tileID;	///< TileID
	// VARIABLES END /////////////////////////////////////////////////////////////////////////////
};

#endif
