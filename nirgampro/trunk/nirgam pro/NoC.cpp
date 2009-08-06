
/*
 * NoC.cpp
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
/// \file NoC.cpp
/// \brief Creates network topology
///
/// This file creates a 2-dimensional topology of network tiles and interconnects them.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NoC.h"


ULL SIM_NUM = 30;	

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor to generate topology.
/// It creates 2-d toroidal or non- toroidal mesh of m x n network tiles depending on input from user.
/// Parameters:
/// - number of rows in topology
/// - number of columns in topology
/// .
/// The constructor implements the following:
/// - interconnect network tiles
/// - assign tile IDs
/// - connect clock signal to clock input port of tiles.
////////////////////////////////////////////////////////////////////////////////////////////////////
NoC::NoC(sc_module_name NoC, int num_rows, int num_cols, int num_vert): sc_module(NoC) {

	//////////////////////////////////////////////////////////////////////////
	// create tiles
	nwtile[0][0][0] = new NWTile_for_test<NUM_NB, NUM_IC, NUM_OC>("tile0", 0);
	(ptr nwtile[0][0][0])->clk(switch_cntrl);
	nwtile[0][0][1] = new NWTile_for_test<NUM_NB, NUM_IC, NUM_OC>("tile1", 1);
	(ptr nwtile[0][0][1])->clk(switch_cntrl);
	//////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////////////
	// connect tiles
	sigs[0][0][0] = new wires("wires0", 2500);
	wires* w_0to1 = sigs[0][0][0] ;
	w_0to1->clk(switch_cntrl);
	(ptr nwtile[0][0][0])->op_port[0](w_0to1->sig_from1);
	(ptr nwtile[0][0][0])->ip_port[0](w_0to1->sig_to1);
	for(int i=0; i<NUM_VCS; i++){
		(ptr nwtile[0][0][0])->credit_out[0][i](w_0to1->credit_from1[i]);
		(ptr nwtile[0][0][0])->credit_in[0][i](w_0to1->credit_to1[i]);
	}

	(ptr nwtile[0][0][1])->op_port[0](w_0to1->sig_from2);
	(ptr nwtile[0][0][1])->ip_port[0](w_0to1->sig_to2);
	for(int i=0; i<NUM_VCS; i++){
		(ptr nwtile[0][0][1])->credit_out[0][i](w_0to1->credit_from2[i]);
		(ptr nwtile[0][0][1])->credit_in[0][i](w_0to1->credit_to2[i]);
	}
	//////////////////////////////////////////////////////////////////////////
	

	SC_THREAD(entry);	// Thread entry() sensitive to clock
	sensitive_pos << switch_cntrl;
}

wires* NoC::connect(BaseNWTile* tile1, BaseNWTile * tile2){
	wires* w_0to1 = new wires("wires0", 2500); 
	w_0to1->clk(switch_cntrl);
	
	
	return NULL;
}

///////////////////////////////////////////////////////////
/// This thread keeps track of global simulation count.
/// It also closes logfiles upon completion of simulation.
////////////////////////////////////////////////////////////
void NoC::entry() {
	while(true) {
		ULL sim_count = 0;
		while(sim_count < SIM_NUM) {
			wait();
			sim_count++;
		}

		sc_stop();
	}//end while
}//end entry

