
/*
 * Controller.h
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
/// \file Controller.h
/// \brief Defines Controller module that implements routing
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _CONTROLLER_
#define _CONTROLLER_

#include "systemc.h"
#include "credit.h"
#include "constants.h"
#include "router.h"
#include <string>
#include <fstream>
#include <iostream>
#include "typedef.h"

#include "InnerSigs.h"
//#include <dlfcn.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent a Controller
///
/// This module defines a Controller (implements router)
//////////////////////////////////////////////////////////////////////////
struct Controller : public sc_module {
	/// Constructor
	SC_HAS_PROCESS(Controller);
	Controller(sc_module_name Controller, UI io_num);

	UI io_num;
	// PORTS ////////////////////////////////////////////////////////////////////////////////
	sc_in<bool> switch_cntrl;		///< input clock port
	sc_in<request_type>* rtRequest;	///< input ports to recieve route request from ICs
	sc_in<sc_uint<ADDR_SIZE> > * destRequest;	///< input ports to recieve destination address
	sc_in<sc_uint<ADDR_SIZE> > * sourceAddress; ///< input ports to recieve source address
	sc_in<creditLine> (*Icredit)[NUM_VCS];	///< input ports to recieve credit info (buffer info) from ICs
	sc_out<bool>* rtReady;		///< output ports to send ready signal to ICs
	sc_out<port_id> * nextRt;	///< output ports to send routing decision to ICs
	// PORTS END /////////////////////////////////////////////////////////////////////////////
	

	bool isCoreIO(UI i);
	// PROCESSES /////////////////////////////////////////////////////////////////////////////
	/// sets tile ID and id corresponding to port directions
	void innerConnect(UI icId,
					sc_clock & switch_cntrl,
					Sigs_IcCtl & sigs_IcCtl,
					sc_sigs_creditLine &creditIC_CS,
					sc_in<creditLine> (*credit_in)[NUM_VCS]);
	void setTileID(UI tileID, UI portN, UI portS, UI portE, UI portW);
	void allocate_route();	///< does routing
	UI idToDir(UI);		///< returns port id for a given direction (N, S, E, W)
	UI dirToId(UI);		///< returns direction (N, S, E, W) corresponding to a given port id
	// PROCESSES END /////////////////////////////////////////////////////////////////////////
	
	// VARIABLES /////////////////////////////////////////////////////////////////////////////
	UI tileID;	///< Unique tile identifier
	
	router *rtable;	///< router (plug-in point for routing algorithm)
	// VARIABLES END /////////////////////////////////////////////////////////////////////////
};

#endif
 
