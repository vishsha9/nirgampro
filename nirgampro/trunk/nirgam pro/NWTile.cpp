
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
#include "NWTile.h"
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


//template <int num_nb, int num_ic, int num_oc>
NWTile::NWTile(sc_module_name NWTile, UI id, UI nb): sc_module(NWTile){
	this->tileID = id;
	this->nb_num = nb;
	this->nb_initPtr = 0;

	ip_port = new sc_in<flit>[nb_num];
	op_port = new sc_out<flit>[nb_num];

	credit_in = new sc_in<creditLine>[nb_num][NUM_VCS];
	credit_out = new sc_out<creditLine>[nb_num][NUM_VCS];

	nb_id = new UI[nb_num];

	for (int i=0; i<nb_num; i++)
	{
		nb_id[i] = -1;
	}


	//////////////////////////////////////////////////////////////////////////
	// sensitive
	//////////////////////////////////////////////////////////////////////////
	SC_THREAD(read);
	sensitive << clk ;
	for(int i=0; i<nb_num; i++){
		sensitive << ip_port[i];
		for(int j=0; j< NUM_VCS ; j++){
			sensitive << credit_in[i][j];
		}
	}

	SC_THREAD(write);
	sensitive_pos<<clk;
} // end constructor

bool NWTile::connect(UI nb_id, sc_signal<flit>* sig_from, sc_signal<flit>* sig_to, sc_signal<creditLine> credit_from[NUM_VCS], sc_signal<creditLine> credit_to[NUM_VCS]){
	op_port[nb_initPtr](*sig_from);
	ip_port[nb_initPtr](*sig_to);
	for(int i=0; i<NUM_VCS; i++){
		credit_out[nb_initPtr][i](credit_from[i]);
		credit_in[nb_initPtr][i](credit_to[i]);
	}
	this->nb_id[nb_initPtr] = nb_id;
	nb_initPtr++;
	return true;
}


//template <int num_nb, int num_ic, int num_oc>
void NWTile::read(){
	while(true){
		wait();
		for(int i=0; i<nb_num; i++){
			if (ip_port[i].event())
			{
				flit r = ip_port[0].read();
				readlog << "CLK: " << sc_time_stamp() << "\ttile_" << this->tileID << " port_"<< i <<"\treadflit " << r.data4 << "\ttimestamp "<< r.simdata.gtime <<endl;

			}
		}
		/*for(int i = 0; i< NUM_VCS; i++){
		if(credit_in[0][i].event()){
		creditLine c = credit_in[0][i].read();
		readlog << "CLK: " << sc_time_stamp() << "\ttile_" << this->tileID << "\tVC_" << i <<"\treadCreditLine\ttimestamp "<< c.gtime <<endl;
		}
		}*/
	}
}

//template <int num_nb, int num_ic, int num_oc>
void NWTile::write(){
	int count=0;
	while(true){
		wait();

		flit w;
		w.data4 = count;
		w.simdata.gtime = sc_time_stamp();

		op_port[(count+1)%nb_num].write(w);
		writelog << "CLK: " << sc_time_stamp() << "\tPort "<< (count+1)%nb_num <<"\ttile_" << this->tileID << " to "<< nb_id[(count+1)%nb_num] <<"\twriteflit " << w.data4 << "\ttimestamp "<< w.simdata.gtime << endl;
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
//template <int num_nb, int num_ic, int num_oc>
void NWTile::entry() {
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
//template <int num_nb, int num_ic, int num_oc>
void NWTile::setID(UI id) {

}




//template struct NWTile_for_test<NUM_NB, NUM_IC, NUM_OC>;
//template struct NWTile_for_test<NUM_NB_B, NUM_IC_B, NUM_OC_B>;
//template struct NWTile_for_test<NUM_NB_C, NUM_IC_C, NUM_OC_C>;
