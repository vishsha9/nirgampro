
/*
 * ipcore.cpp
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
/// \file ipcore.cpp
/// \brief Implements module ipcore (base class for applications)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ipcore.h"



////////////////////////
/// Constructor
////////////////////////
ipcore::ipcore(sc_module_name ipcore): sc_module(ipcore) {

	sim_count = 0;
	ran_var = new RNG((RNG::RNGSources)2,1);

	// process sensitive to clock, sends out flit
//	SC_METHOD(transition);
//	dont_initialize();
//	sensitive << clock.pos();
	
	// datum will be changed @ falling edge, according to current state 
//	SC_METHOD(genMoore);
//	dont_initialize();
//	sensitive <<  clock.neg();
	
	// process sensitive to clock, to keep track of simulation count
	SC_THREAD(entry);
	dont_initialize();
	sensitive << clock.pos();
	


	// process sensitive to clock and inport event, recieves incoming flit
	//SC_METHOD(recv);
	//dont_initialize();
	//sensitive << flit_inport << clock;

}

///////////////////////////////////////////////////////////////////////////
/// Method to assign tile ID
///////////////////////////////////////////////////////////////////////////
void ipcore::setID(UI id){
	tileID = id;
}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to clock
/// Keeps track of clock cycles in the module
///////////////////////////////////////////////////////////////////////////
void ipcore::entry(void) {
	sim_count = 0;
	while(true) {
		wait();
		sim_count++;
	}
}

///////////////////////////////////////////////////////////////////////////
/// Method to return a random destination
///////////////////////////////////////////////////////////////////////////
int ipcore::get_random_dest() {
	int dest = ran_var->uniform(num_tiles);
	while(dest == tileID)
		dest = ran_var->uniform(num_tiles);
	return dest;
}

UI ipcore::cluster_ctrl(int alg, int cmd, int be, int contig, int error)
{
	UI ctrl = 0;
	
	ctrl |= (error & 0xFF) << 24;
	ctrl |= (be    & 0xF) << 20;
	ctrl |= (contig& 0xF) << 16;
	ctrl |= (cmd   & 0xFF) << 8;
	ctrl |= (alg   & 0xFF);
	
	return ctrl;
}

void ipcore::disperse_ctrl(UI ctrl, int &alg, int &cmd, int &be, int &contig, int &error)
{
	alg = ctrl 				& 0xFF;
	cmd = (ctrl >> 8) 		& 0xFF;
	contig = (ctrl >> 16)	& 0xF;
	be = (ctrl >> 20)		& 0xF;
	error = (ctrl >> 24)	& 0xFF;
}


///////////////////////////////////////////////////////////////////////////
/// Method to create a hdt flit
/// hdt flit represents a packet consisting of single flit.
/// Parameters:
/// - packet id
/// - flit id
/// - route 
///   - destination tileID for XY and OE routing
///   - route code fro source routing
/// .
///////////////////////////////////////////////////////////////////////////
void ipcore::write_reg_hdt(UI pkt_id, UI flit_id, UI route,
							UI ctrl, UI addr, UI data) 
{
	flit_reg.vcid = 0;
	flit_reg.src = tileID;
	flit_reg.pktid = pkt_id;
	flit_reg.flitid = flit_id;
	flit_reg.type = HDT;

	flit_reg.data1 = route;
	flit_reg.data2 = ctrl;
	flit_reg.data3 = addr;
	flit_reg.data4 = data;

	flit_reg.simdata.gtime = sc_time_stamp();
	flit_reg.simdata.ctime = sc_time_stamp();
	flit_reg.simdata.atime = SC_ZERO_TIME;
	flit_reg.simdata.atimestamp = 0;
	flit_reg.simdata.num_waits = 0;
	flit_reg.simdata.num_sw = 0;
	flit_reg.simdata.gtimestamp = sim_count;
}

///////////////////////////////////////////////////////////////////////////
/// Method to create a head flit
/// Parameters:
/// - packet id
/// - flit id
/// - route info
///   - destination tileID for XY and OE routing
///   - route code fro source routing
/// .
///////////////////////////////////////////////////////////////////////////
void ipcore::write_reg_head(UI pkt_id, UI flit_id, UI route,
							UI ctrl, UI addr, UI data) 
{
	flit_reg.vcid = 0;
	flit_reg.src = tileID;
	flit_reg.pktid = pkt_id;
	flit_reg.flitid = flit_id;
	flit_reg.type = HEAD;

	flit_reg.data1 = route;
	flit_reg.data2 = ctrl;
	flit_reg.data3 = addr;
	flit_reg.data4 = data;

	flit_reg.simdata.gtime = sc_time_stamp();
	flit_reg.simdata.ctime = sc_time_stamp();
	flit_reg.simdata.atime = SC_ZERO_TIME;
	flit_reg.simdata.atimestamp = 0;
	flit_reg.simdata.num_waits = 0;
	flit_reg.simdata.num_sw = 0;
	flit_reg.simdata.gtimestamp = sim_count;
}

///////////////////////////////////////////////////////////////////////////
/// Method to create a data flit
/// Parameters:
/// - packet id
/// - flit id
/// returns pointer to new flit
///////////////////////////////////////////////////////////////////////////

void ipcore::write_reg_data(UI pkt_id, UI flit_id, UI opt,
							UI ctrl, UI addr, UI data) 
{
	flit_reg.vcid = 0;
	flit_reg.src = tileID;
	flit_reg.pktid = pkt_id;
	flit_reg.flitid = flit_id;
	flit_reg.type = DATA;

	flit_reg.data1 = opt;
	flit_reg.data2 = ctrl;
	flit_reg.data3 = addr;
	flit_reg.data4 = data;

	flit_reg.simdata.gtime = sc_time_stamp();
	flit_reg.simdata.ctime = sc_time_stamp();
	flit_reg.simdata.atime = SC_ZERO_TIME;
	flit_reg.simdata.atimestamp = 0;
	flit_reg.simdata.num_waits = 0;
	flit_reg.simdata.num_sw = 0;
	flit_reg.simdata.gtimestamp = sim_count;
}

///////////////////////////////////////////////////////////////////////////
/// Method to create a tail flit
/// Parameters:
/// - packet id
/// - flit id
/// returns pointer to new flit
///////////////////////////////////////////////////////////////////////////

void ipcore::write_reg_tail(UI pkt_id, UI flit_id, UI opt,
							UI ctrl, UI addr, UI data) 
{
	flit_reg.vcid = 0;
	flit_reg.src = tileID;
	flit_reg.pktid = pkt_id;
	flit_reg.flitid = flit_id;
	flit_reg.type = TAIL;

	flit_reg.data1 = opt;
	flit_reg.data2 = ctrl;
	flit_reg.data3 = addr;
	flit_reg.data4 = data;

	flit_reg.simdata.gtime = sc_time_stamp();
	flit_reg.simdata.ctime = sc_time_stamp();
	flit_reg.simdata.atime = SC_ZERO_TIME;
	flit_reg.simdata.atimestamp = 0;
	flit_reg.simdata.num_waits = 0;
	flit_reg.simdata.num_sw = 0;
	flit_reg.simdata.gtimestamp = sim_count;
}

