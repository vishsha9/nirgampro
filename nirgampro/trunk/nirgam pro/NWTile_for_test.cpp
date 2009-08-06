
/*
* NWTile.cpp
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
/// \file NWTile.cpp
/// \brief Creates a network tile.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "systemc.h"
#include "NWTile_for_test.h"
#include <string>
#include <fstream>
//#include "../config/extern.h"

/// array to store library name of application attached to ipcore on each tile
//extern string app_libname[MAX_NUM_TILES];

//////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor to create a network tile.
/// Creates and connects the following submodules:
/// - Input Channels
/// - Output Channels
/// - Virtual Channel Allocator
/// - IP Core
/// - Controller
/// .
/// Template parameters:
/// - num_nb: Number of neighbors
/// - num_ic: Number of input channels
/// - num_oc: Number of output channels
/// Parameters to constructor:
/// - module name
/// - unique tile ID
/////////////////////////////////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
NWTile_for_test<num_nb, num_ic, num_oc>::NWTile_for_test(sc_module_name NWTile, UI id): BaseNWTile(NWTile, id) {
	this->tileID = id;

	SC_THREAD(read);
	sensitive << clk << ip_port[0];
	for(int i=0; i< NUM_VCS ; i++){
		sensitive << credit_in[0][i];
	}
	
	SC_THREAD(write);
	sensitive_pos<<clk;
} // end constructor


template <int num_nb, int num_ic, int num_oc>
void NWTile_for_test<num_nb, num_ic, num_oc>::read(){
	while(true){
		wait();
		if (ip_port[0].event())
		{
			flit r = ip_port[0].read();
			readlog << "CLK: " << sc_time_stamp() << "\ttile_" << this->tileID <<"\treadflit " << r.data4 << "\ttimestamp "<< r.simdata.gtime <<endl;

		}
		/*for(int i = 0; i< NUM_VCS; i++){
			if(credit_in[0][i].event()){
				creditLine c = credit_in[0][i].read();
				readlog << "CLK: " << sc_time_stamp() << "\ttile_" << this->tileID << "\tVC_" << i <<"\treadCreditLine\ttimestamp "<< c.gtime <<endl;
			}
		}*/
	}
}

template <int num_nb, int num_ic, int num_oc>
void NWTile_for_test<num_nb, num_ic, num_oc>::write(){
	static int count=0;
	while(true){
		wait();

		flit w;
		w.data4 = count;
		w.simdata.gtime = sc_time_stamp();

		op_port[0].write(w);
		writelog << "CLK: " << sc_time_stamp() << "\ttile_" << this->tileID <<"\twriteflit " << w.data4 << "\ttimestamp "<< w.simdata.gtime << endl;
		count ++;

		/*for(int i = 0; i< NUM_VCS; i++){
			creditLine c;
			c.gtime = sc_time_stamp();
			credit_out[0][i].write(c);
			writelog << "CLK: " << sc_time_stamp() << "\ttile_" << this->tileID <<"\twriteCreditLine\ttimestamp "<< c.gtime << endl;
		}*/

	}
}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to clock.
/// Writes buffer utilization info at each clock
///////////////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
void NWTile_for_test<num_nb, num_ic, num_oc>::entry() {
	while(true) {
		wait();
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
	}
}


///////////////////////////////////////////////////////////////////////////
/// - set unique tile ID
/// - map port number to port direction
//////////////////////////////////////////////////////////////////////////
template <int num_nb, int num_ic, int num_oc>
void NWTile_for_test<num_nb, num_ic, num_oc>::setID(UI id) {

}




template struct NWTile_for_test<NUM_NB, NUM_IC, NUM_OC>;
template struct NWTile_for_test<NUM_NB_B, NUM_IC_B, NUM_OC_B>;
template struct NWTile_for_test<NUM_NB_C, NUM_IC_C, NUM_OC_C>;
