
/*
* Controller.cpp
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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// \file Controller.cpp
/// \brief Implements routing
//////////////////////////////////////////////////////////////////////////////////////////////////

#include "Controller.h"

////////////////////////
/// Constructor
////////////////////////
Controller::Controller(sc_module_name Controller, UI io_num, vector<UI> * indexTable): sc_module(Controller) {
	this->io_num = io_num;
	rtRequest = new sc_in<request_type>[io_num];
	destRequest = new sc_in<sc_uint<ADDR_SIZE> >[io_num];
	sourceAddress = new sc_in<sc_uint<ADDR_SIZE> >[io_num];
	credit_in = new sc_in<creditLine>[io_num][NUM_VCS];
	rtReady = new sc_out<bool>[io_num];
	nextRt = new sc_out<port_id>[io_num];

	void *hndl;
	void *mkr;

	string libname = string("./router/lib/");

	rtable = new sssp_router();
	rtable->setIndexTable(indexTable);
	/*switch(RT_ALGO) {
	case OE: //tg = new BurstyTraffic("TG");
	libname = libname + string("OE_router.so");
	break;
	case XY:
	libname = libname + string("XY_router.so");
	break;
	case SOURCE:
	libname = libname + string("source_router.so");
	break;
	}

	hndl = dlopen(libname.c_str(), RTLD_NOW);
	if(hndl == NULL) {
	cerr << dlerror() << endl;
	exit(-1);
	}
	mkr = dlsym(hndl, "maker");
	rtable = ((router*(*)())(mkr))();
	*/
	SC_THREAD(allocate_route);
	for(UI i = 0; i < io_num; i++)
		sensitive << rtRequest[i];
}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to route request
/// Calls routing algorithm
///////////////////////////////////////////////////////////////////////////
void Controller::allocate_route() {
	while(true) {
		wait();
		for(UI i = 0; i < io_num; i++) {
			if(rtRequest[i].event() && rtRequest[i].read() == ROUTE) {
				sc_uint<ADDR_SIZE> src = sourceAddress[i].read();
				sc_uint<ADDR_SIZE> dest = destRequest[i].read();
				UI ip_dir = idToDir(i);
				UI op_dir = rtable->calc_next(ip_dir, src, dest);
				//sc_uint<3> next_hop = dirToId(op_dir);
				//sc_uint<3> next_hop = rtable.calc_next(i, dest);
				rtReady[i].write(true);
				nextRt[i].write(op_dir);
			}
			// request from IC to update //////////////////////////
			if(rtRequest[i].event() && rtRequest[i].read() == UPDATE) {
				sc_uint<ADDR_SIZE> src = sourceAddress[i].read();
				sc_uint<ADDR_SIZE> dest = destRequest[i].read();
				//rtable.update(dest, i);
				rtReady[i].write(true);
			}
			if(rtRequest[i].event() && rtRequest[i].read() == NONE) {
				rtReady[i].write(false);
			}
		}
	}// end while
}// end allocate_route

///////////////////////////////////////////////////////////////////////////
/// Method to assign tile IDs and port IDs
///////////////////////////////////////////////////////////////////////////
void Controller::setTileID(UI id) {
	tileID = id;
	rtable->setID(id);
}

/////////////////////////////////////////////////////////////////////
/// Returns direction (N, S, E, W) corresponding to a given port id
////////////////////////////////////////////////////////////////////
UI Controller::idToDir(UI port_id) {
	/*if(port_id == portN)
	return N;
	else if(port_id == portS)
	return S;
	else if(port_id == portE)
	return E;
	else if(port_id == portW)
	return W;
	return C;*/
	return 0;
}

/////////////////////////////////////////////////////////////////////
/// Returns id corresponding to a given port direction (N, S, E, W)
////////////////////////////////////////////////////////////////////
UI Controller::dirToId(UI port_dir) {
	/*switch(port_dir) {
	case N: return portN;
	break;
	case S: return portS;
	break;
	case E: return portE;
	break;
	case W: return portW;
	break;
	case C: return num_ip - 1;
	break;
	}*/
	return io_num - 1;
}
