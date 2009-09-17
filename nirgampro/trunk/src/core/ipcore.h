
/*
 * ipcore.h
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
/// \file ipcore.h
/// \brief Defines abstract ipcore module
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _IPCORE_
#define _IPCORE_

#include "systemc.h"
#include "../constants.h"
#include "flit.h"
#include "credit.h"
#include "rng.h"
#include "../extern.h"
#include "tracker/tracker.h"

#include <fstream>
#include <string>
#include <math.h>
//#include <dlfcn.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
/// \brief Module to represent an ipcore
///
/// This module defines an ipcore
//////////////////////////////////////////////////////////////////////////
struct ipcore : public sc_module {

	// PORTS ////////////////////////////////////////////////////
	sc_in_clk clock;			///< input clock port
	//sc_in<bool> resetn;			// global reset signal
	sc_in<flit> flit_inport;		///< input data/flit port
	sc_out<flit> flit_outport;		///< ouput data/flit port
	sc_in<creditLine> credit_in[NUM_VCS];	///<ports to recieve credit info (buffer status)
	// PORTS END /////////////////////////////////////////////////
	
	/// Constructor
	SC_CTOR(ipcore);


	// FUNCTIONS ///////////////////////////////////////////////////
	void entry(void);			///< process to keep track of simulation count in the module, sensitive to clock
	virtual void recv(void) = 0;	///< abstract process to recieve flits, sensitive to clock and input flit port
//	virtual void transition(void) = 0;	///< do data exchange and state transition
//	virtual void genMoore(void) = 0;	///< change data acording to state
	
	void setID(UI tileID);		///< sets tileID
	
	//write corresponding information to flit_reg register
	// it is connected to the output port.
	
	void write_reg_hdt(UI pkt_id, UI flit_id, UI route, 
						UI ctrl, UI addr, UI data);
	void write_reg_head(UI pkt_id, UI flit_id, UI route, 
						UI ctrl, UI addr, UI data);
	void write_reg_data(UI pkt_id, UI flit_id, UI opt, 
						UI ctrl, UI addr, UI data);
	void write_reg_tail(UI pkt_id, UI flit_id, UI opt, 
						UI ctrl, UI addr, UI data);

	//asseble several control information in a 32-width int
	UI cluster_ctrl(int alg, int cmd, int be, int contig, int error);
	void disperse_ctrl(UI ctrl, int &alg, int &cmd, int &be, int &contig, int &error);

	/// return a randomly chosen destination
	int get_random_dest();
	// FUNCTIONS END ///////////////////////////////////////////////////////
	
	// VARIABLES ////////////////////////////////////////////////
	UI tileID;	///< unique tile id
	RNG *ran_var;	///< random variable

	flit flit_reg;	//data write here is sent to flit_outport.
	// VARIABLES END ///////////////////////////////////////////
};

#endif
