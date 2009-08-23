
/*
 * VCAllocator.cpp
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
/// \file VCAllocator.cpp
/// \brief Implements virtual channel allocator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VCAllocator.h"
#include "extern.h"
	
////////////////////////
/// Constructor
////////////////////////
VCAllocator::VCAllocator(sc_module_name VCAllocator, UI io_num): sc_module(VCAllocator) {
	this->io_num = io_num;
	vcRequest = new sc_in<bool>[io_num];
	opRequest = new sc_in<port_id>[io_num];
	nextVCID = new sc_out<sc_uint<VCS_BITSIZE+1> >[io_num];
	vcReady = new sc_out<bool>[io_num];
	credit_in = new sc_in<creditLine>[io_num][NUM_VCS];
	vcFree = new bool[io_num][NUM_VCS];

	// process sensitive to VC request, calls VC allocation
	SC_THREAD(allocate_VC);
	for(UI i = 0; i < io_num; i++)
		sensitive << vcRequest[i];
	
	// process sensitive to credit info, updates credit status
	SC_THREAD(update_credit);
	for(UI i = 0; i < io_num; i++){
		for(UI j = 0; j < NUM_VCS; j++) {
			sensitive << credit_in[i][j];
		}
	}
	
	// initialize credit status
	for(UI i = 0; i < io_num; i++){
		for(UI j = 0; j < NUM_VCS; j++) {
			vcFree[i][j] = true;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to incoming credit information.
/// updates credit info (buffer status) of neighbor tiles
///////////////////////////////////////////////////////////////////////////
void VCAllocator::update_credit() {
	while(true) {
		wait();	// wait until change in credit info
		for(UI i = 0; i < io_num; i++) {
			for(UI j = 0; j < NUM_VCS; j++) {
				if(credit_in[i][j].event()) {
					vcFree[i][j] = credit_in[i][j].read().freeVC;	// update credit
					if(LOG >= 4)
						eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Credit change in "<<i<<" dir, vc num: "<<j<<" status: "<<vcFree[i][j];
				}
			}//end for
		}
	}//end while
}//end entry

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to incoming request for virtual channel allocation
/// - reads request
/// - allocates virtual channel in the requested channel on neighbor tile
/// - returns allocated vcid and ready signal
///////////////////////////////////////////////////////////////////////////
void VCAllocator::allocate_VC() {
	while(true) {
		wait();		// wait until request from any IC
		for(UI i = 0; i < io_num; i++) {
			if(vcRequest[i].event() && vcRequest[i].read() == true) {
				// read output direction in which VC is requested
				sc_uint<2> dir = opRequest[i].read();
				sc_uint<VCS_BITSIZE+1> nextvc = NUM_VCS+1;
				// get next VC, parameters: o/p direction requested, i/p direction from which request recieved
				nextvc = getNextVCID(dir,i);
				
				if(LOG >= 4) 
					eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "<<this->name()<< " VA: got this next vc " << nextvc << endl;
				
				// send ready signal and nextVCID to IC
				vcReady[i].write(true);
				nextVCID[i].write(nextvc);
			}
			if(vcRequest[i].event() && vcRequest[i].read() == false) {
				vcReady[i].write(false);
			}
		}
	} //end while
} //end allocate_VC

///////////////////////////////////////////////////////////////////////////
/// Method that implements virtual channel allocation
/// Parameters:
/// - o/p direction (neighbor tile) in which virtual channel is requested
/// - i/p direction (IC) from which request is recieved
/// .
/// return allocated VC id
///////////////////////////////////////////////////////////////////////////
sc_uint<VCS_BITSIZE+1> VCAllocator::getNextVCID (int dir, int dir_from) {	
	for(int i = 0; i < NUM_VCS; i++) {
		if(vcFree[dir][i]) {
			vcFree[dir][i] = false;
			return i;
		}
	}
	if(LOG >= 4) 
		eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "<<this->name()<<" VA: did not find a free VC at "<<dir<<endl;
	return NUM_VCS+1;
}//end getnextVCID

///////////////////////////////////////////////////////////////////////////
/// Method to assign tile IDs and port IDs
///////////////////////////////////////////////////////////////////////////
void VCAllocator::setTileID(UI id){
	tileID = id;
}
