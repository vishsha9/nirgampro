
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
#include "BaseNWTile.h"

//#include "../config/extern.h"

/// array to store library name of application attached to ipcore on each tile

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


BaseNWTile::BaseNWTile(sc_module_name NWTile, UI id, UI nb): sc_module(NWTile){
	this->tileID = id;
	this->nb_num = nb;
	this->nb_initPtr = 0;

	ip_port = new sc_in<flit>[nb_num];
	op_port = new sc_out<flit>[nb_num];

	credit_in = new sc_in<creditLine>[nb_num][NUM_VCS];
	credit_out = new sc_out<creditLine>[nb_num][NUM_VCS];

	//nb_id = new UI[nb_num]; 


	//////////////////////////////////////////////////////////////////////////
	// sensitive
	//////////////////////////////////////////////////////////////////////////
	/*SC_THREAD(read);
	sensitive << clk ;
	for(int i=0; i<nb_num; i++){
		sensitive << ip_port[i];
		for(int j=0; j< NUM_VCS ; j++){
			sensitive << credit_in[i][j];
		}
	}

	SC_THREAD(write);
	sensitive_pos<<clk;*/
} // end constructor

bool BaseNWTile::connect(UI nb_id, sc_signal<flit>& sig_from, sc_signal<flit>& sig_to, sc_signal<creditLine> credit_from[NUM_VCS], sc_signal<creditLine> credit_to[NUM_VCS]){
	op_port[nb_initPtr](sig_from);
	ip_port[nb_initPtr](sig_to);
	for(int i=0; i<NUM_VCS; i++){
		credit_out[nb_initPtr][i](credit_from[i]);
		credit_in[nb_initPtr][i](credit_to[i]);
	}
	nb_initPtr++;

	this->nb_id.push_back(nb_id);
	return true;
}


//
//void BaseNWTile::read(){
//	while(true){
//		wait();
//		for(int i=0; i<nb_num; i++){
//			if (ip_port[i].event())
//			{
//			flit r = ip_port[i].read();
//			readlog << "CLK: " << sc_time_stamp() << "\ttile_" << this->tileID << " port_"<< i <<"\treadflit data:" << r.data4 << "\ttimestamp "<< r.simdata.gtime <<endl;
//
//			}
//
//			/*for(int j = 0; j< NUM_VCS; j++){
//				if(credit_in[i][j].event()){
//					creditLine c = credit_in[i][j].read();
//					readlog << "CLK: " << sc_time_stamp() << "\ttile_" << this->tileID << "\tVC_" << j <<"\treadCreditLine\ttimestamp "<< c.gtime <<endl;
//				}
//			}*/
//		}
//	}
//}
//
//
//void BaseNWTile::write(){
//	int count=0;
//	while(true){
//		wait();
//
//		if(count<nb_num){
//			flit w;
//			w.data4 = count+2;
//			w.simdata.gtime = sc_time_stamp();
//
//			op_port[(count+1)%nb_num].write(w);
//			writelog << "CLK: " << sc_time_stamp() << "\tPort "<< (count+1)%nb_num <<"\ttile_" << this->tileID << " to "<< nb_id[(count+1)%nb_num] <<"\twriteflit data:" << w.data4 << "\ttimestamp "<< w.simdata.gtime << endl;
//			count ++;
//
//		
//			/*for(int i = 0; i< NUM_VCS; i++){
//				creditLine c;
//				c.gtime = sc_time_stamp();
//				credit_out[(count+1)%nb_num][i].write(c);
//				writelog << "CLK: " << sc_time_stamp() << "\ttile_" << this->tileID <<" Port:"<< (count+1)%nb_num <<" VC_"<< i <<"\twriteCreditLine\ttimestamp "<< c.gtime << endl;
//			}
//			count ++;*/
//		}
//	}
//}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to clock.
/// Writes buffer utilization info at each clock
///////////////////////////////////////////////////////////////////////////
void BaseNWTile::entry() {
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
void BaseNWTile::setID(UI id) {

}