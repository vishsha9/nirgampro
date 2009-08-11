
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
#include "../config/constants.h"
#include "router.h"
#include <string>
#include <fstream>
#include <iostream>
#include <dlfcn.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent a Controller
///
/// This module defines a Controller (implements router)
//////////////////////////////////////////////////////////////////////////
template<UI num_ip = NUM_IC>
struct Controller : public sc_module {

	// PORTS ////////////////////////////////////////////////////////////////////////////////
	sc_in<bool> switch_cntrl;		///< input clock port
	sc_in<request_type> rtRequest[num_ip];	///< input ports to recieve route request from ICs
	sc_in<sc_uint<ADDR_SIZE> > destRequest[num_ip];	///< input ports to recieve destination address
	sc_in<sc_uint<ADDR_SIZE> > sourceAddress[num_ip]; ///< input ports to recieve source address
	sc_in<creditLine> Icredit[num_ip][NUM_VCS];	///< input ports to recieve credit info (buffer info) from ICs
	sc_out<bool> rtReady[num_ip];		///< output ports to send ready signal to ICs
	sc_out<sc_uint<3> > nextRt[num_ip];	///< output ports to send routing decision to ICs
	// PORTS END /////////////////////////////////////////////////////////////////////////////
	
	/// Constructor
	SC_CTOR(Controller);
	
	// PROCESSES /////////////////////////////////////////////////////////////////////////////
	/// sets tile ID and id corresponding to port directions
	void setTileID(UI tileID, UI portN, UI portS, UI portE, UI portW);
	void allocate_route();	///< does routing
	UI idToDir(UI);		///< returns port id for a given direction (N, S, E, W)
	UI dirToId(UI);		///< returns direction (N, S, E, W) corresponding to a given port id
	// PROCESSES END /////////////////////////////////////////////////////////////////////////
	
	// VARIABLES /////////////////////////////////////////////////////////////////////////////
	UI tileID;	///< Unique tile identifier
	UI portN;	///< port number representing North direction
	UI portS;	///< port number representing South direction
	UI portE;	///< port number representing North direction
	UI portW;	///< port number representing North direction
	
	router *rtable;	///< router (plug-in point for routing algorithm)
	// VARIABLES END /////////////////////////////////////////////////////////////////////////
};

#endif
 
