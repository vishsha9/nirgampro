
/*
 * NWTile.h
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
/// \file NWTile.h
/// \brief Defines a network tile
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BASE_NW_TILE__
#define __BASE_NW_TILE__

#include "systemc.h"
//#include "../config/constants.h"
#include "flit.h"
#include "credit.h"
//#include "InputChannel.h"
//#include "OutputChannel.h"
//#include "VCAllocator.h"
//#include "ipcore.h"
//#include "Controller.h"
//#include "BaseNWTile.h"

//#define ptr (NWTile_for_test<NUM_NB, NUM_IC, NUM_OC> *)
//#define ptr_b (NWTile_for_test<NUM_NB_B, NUM_IC_B, NUM_OC_B> *)
//#define ptr_c (NWTile_for_test<NUM_NB_C, NUM_IC_C, NUM_OC_C> *)
//#define ptr_f (NWTile_for_test<NUM_NB_F, NUM_IC_F, NUM_OC_F> *) // format

//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent a tile in NoC
///
/// This module defines a network tile and submodules within it.
/// It is derived from abstract class BaseNWTile.
/// Template parameters:
/// - num_nb: Number of neighbors
/// - num_ic: Number of input channels
/// - num_oc: Number of output channels
//////////////////////////////////////////////////////////////////////////

//template <int num_nb = NUM_NB, int num_ic = NUM_IC, int num_oc = NUM_OC>
struct BaseNWTile: public sc_module {
	// PORTS ////////////////////////////////////////////////////////////////////////////////////
	sc_in<bool>	clk;		///< input clock port
	sc_in<flit>*	ip_port;	///< input data/flit ports
	sc_out<flit>*	op_port;	///< output data/flit ports

	UI tileID;

	UI nb_num;
	UI* nb_id;

	UI nb_initPtr;

	sc_in<creditLine> (*credit_in)[NUM_VCS];	///< input ports for credit line (buffer status)
	sc_out<creditLine> (*credit_out)[NUM_VCS];	///< output ports for credit line (buffer status)

	bool connect(UI nb_id, 
		sc_signal<flit>& sig_in, 
		sc_signal<flit>& sig_out, 
		sc_signal<creditLine> crd_in[NUM_VCS], 
		sc_signal<creditLine> crd_out[NUM_VCS]);

	// PORTS END ////////////////////////////////////////////////////////////////////////////////

	/// Constructor
	// Parameter - module name, tile id.
	SC_HAS_PROCESS(BaseNWTile);
	BaseNWTile(sc_module_name NWTile, UI tileID, UI nb_num);

	// PROCESSES //////////////////////////////////////////////////////////////////////////////////////////
	void entry();		///< Writes buffer utilization information at the tile, at each clock cycle
	void setID(UI);		///< sets unique tile id and associates ports with directions
	
	//////////////////////////////////////////////////////////////////////////
	// for test
	void read();
	void write();
	//////////////////////////////////////////////////////////////////////////
};

#endif
