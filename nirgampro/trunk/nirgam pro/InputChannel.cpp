
/*
* InputChannel.cpp
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
/// \file InputChannel.cpp
/// \brief Implements module InputChannel (reads and processes incoming flits)
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputChannel.h"

////////////////////////
/// Constructor
////////////////////////
InputChannel::InputChannel(sc_module_name InputChannel, UI io_num): sc_module(InputChannel) {
	this->io_num = io_num;
	ports_oc.outport = new sc_out<flit>[io_num];
	ports_oc.outReady = new sc_in<bool>[io_num];

	// process sensitive to inport event, reads in flit and stores in buffer
	SC_THREAD(read_flit);
	sensitive << inport;

	// transmit flit at the front of fifo to output port at each clock cycle
	SC_THREAD(transmit_flit);
	sensitive << ports_VCA.vcReady;
	sensitive_pos << switch_cntrl;

	// route flit at the front of fifo if required
	SC_THREAD(route_flit);
	sensitive << ports_ctr.rtReady;
	sensitive_pos << switch_cntrl;

	// initialize VC request to false
	ports_VCA.vcRequest.initialize(false);

	// initialize route request to NONE
	ports_ctr.rtRequest.initialize(NONE);

	// initialize virtual channels and buffers
	for(UI i=0; i < NUM_VCS ; i++) {
		vc[i].vcQ.num_bufs = NUM_BUFS;
		vc[i].vcQ.pntr = 0;
		vc[i].vcQ.full = false;
		vc[i].vcQ.empty = true;
		vc[i].vc_route = 5;
		vc[i].vc_next_id = NUM_VCS + 1;
	}

	// reset buffer counts to zero
	resetCounts();

	// Send initial credit info (buffer status)
	for(int i=0; i < NUM_VCS ; i++){
		creditLine t; t.freeVC = true; t.freeBuf = true;
		credit_out[i].initialize(t);
	}
}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to inport event
/// Reads flit from input port and calls function to store in buffer
///////////////////////////////////////////////////////////////////////////
void InputChannel :: read_flit() 
{
	//flit that is read into the input channel
	flit flit_in;
	while(true) 
	{
		wait(); // waiting for next flit

		if(inport.event()) 
		{
			flit_in = inport.read();	// read flit

			// set input timestamp (required for per channel latency stats)
			flit_in.simdata.ICtimestamp = sim_count - 1;	

			if(LOG >= 2)
				eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
				<<this->name()<<" tile: "<<tileID
				<<" cntrl: "<<cntrlID <<" Inport event!"<<" flitID: "<<flit_in;

			store_flit_VC(&flit_in);	// store flit in buffer

			// find buffers and VCs occupied
			numBufsOcc = 0; numVCOcc = 0;
			for(int i = 0; i < NUM_VCS; i++) 
			{
				numBufsOcc += vc[i].vcQ.pntr;
				if(vc[i].vc_next_id != NUM_VCS+1) numVCOcc++;
			}
		} // end if inport
	} // end while
} //end entry()

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to clock
/// Calls routing functions if head/hdt flit at the front of fifo
///////////////////////////////////////////////////////////////////////////
void InputChannel :: route_flit() 
{
	sim_count = 0;
	while(sim_count < SIM_NUM) 
	{
		wait();		// wait for next clock cycle
		flit flit_out;
		if(switch_cntrl.event()) 
		{
			sim_count++;

			ULL vc_to_serve;
			if(cntrlID == this->io_num)	// assuming only 1 VC at IchannelC
				vc_to_serve = 0;
			else
				vc_to_serve = (sim_count-1) % NUM_VCS;	// serving VCs in round robin manner

			if(vc[vc_to_serve].vc_route == 5) // route not set, require routing
			{	
				if(vc[vc_to_serve].vcQ.empty == false) 
				{	
					// read flit at front of fifo
					//flit_out = vc[vc_to_serve].vcQ.flit_out();
					flit_out = vc[vc_to_serve].vcQ.flit_read();
					//numBufReads++;

					// call routing function depending on type of routing algorithm
					if(flit_out.type == HEAD || flit_out.type == HDT) 
					{
						routing_type rt = static_cast<routing_type> (flit_out.data2 & 0xFF);

						if(rt == SOURCE) 
							routing_src(&flit_out);
						else
							routing_dst(&flit_out);
					}

					// push back flit in fifo
					//vc[vc_to_serve].vcQ.flit_push(flit_out);

					if(LOG >= 2)
						eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
						<<this->name()<<" tile: "<<tileID
						<<" cntrl: "<<cntrlID<<" Routing flit: "<<flit_out;
				}
			}
		}
	}
}
bool InputChannel::isCoreIO(UI i){
	return i == io_num -1 ;
}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to clock
/// Transmits flit at front of fifo to OC
/// - If head/hdt flit, send VC request
/// - write flit to output port if ready signal from OC
///////////////////////////////////////////////////////////////////////////
void InputChannel :: transmit_flit() {
	ULL sim_count = 0;
	while(sim_count < SIM_NUM) 
	{
		wait();		// wait for next clock cycle
		flit flit_out;
		if(switch_cntrl.event()) {	// clock event
			sim_count++;

			// find buffers and VCs occupied
			/*			numBufsOcc = 0; numVCOcc = 0;
			for(int i = 0; i < NUM_VCS; i++) {
			numBufsOcc += vc[i].vcQ.pntr;
			if(vc[i].vc_next_id != NUM_VCS+1) numVCOcc++;
			}
			*/
			// serve a VC in round robin manner
			ULL vc_to_serve;
			if( isCoreIO(cntrlID) )	// assuming only 1 VC at IchannelC
				vc_to_serve = 0;
			else
				vc_to_serve = (sim_count-1) % NUM_VCS;

			if(vc[vc_to_serve].vc_route == 5)	// routing decision pending, before transmission
				continue;

			// Routing decision has been made, proceed to transmission
			int oldvcid;

			UI i;
			i = vc[vc_to_serve].vc_route;
			/*switch(TOPO) {
			case TORUS:
				//#ifdef TORUS
				i = vc[vc_to_serve].vc_route;
				//#endif
				break;
			case MESH:
				//#ifdef MESH
				UI dir = vc[vc_to_serve].vc_route;
				switch(dir) {
			case N: i = portN;
				break;
			case S: i = portS;
				break;
			case E: i = portE;
				break;
			case W: i = portW;
				break;
			case C: i = num_op - 1;
				break;
				}
				//#endif
				break;
			}*/

			if(vc[vc_to_serve].vcQ.empty == false) {	// fifo not empty
				if(ports_oc.outReady[i].read() == true) {	// OC ready to recieve flit
					//flit_out = vc[vc_to_serve].vcQ.flit_out();	// read flit from fifo
					flit_out = vc[vc_to_serve].vcQ.flit_read();	// read flit from fifo
					numBufReads++;		// increase buffer read count

					oldvcid = flit_out.vcid;

					if(LOG >= 4)
						eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
						<<this->name()<<" IC: Attempting to forward  flit: "
						<<flit_out;

					if(  !isCoreIO(i) ) 	// not to be done for core OC
					{	
						if( vc[vc_to_serve].vc_next_id == NUM_VCS+1 &&  !isCoreIO(cntrlID)) 
						{
							if(flit_out.type == DATA || flit_out.type == TAIL) 
							{
								//should have been a head, need to clean out the fifo Q
								if(LOG >= 4)
									eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
									<<this->name()<<" IC: flit is not a head..Error"<<endl;
								vc[vc_to_serve].vcQ.pntr = 0;
								vc[vc_to_serve].vcQ.empty = true;
								vc[vc_to_serve].vcQ.full = false;
								continue;
							}
						}

						if(flit_out.type == HEAD || flit_out.type == HDT) 
						{

							// VC request
							if(LOG >= 4)
								eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()
								<<" tile: "<<tileID<<" cntrlID: "<<cntrlID<<" vcRequest: "<<i;

							ports_VCA.vcRequest.write(true);
							ports_VCA.opRequest.write(i);

							wait();	// wait for ready event from VC
							if(ports_VCA.vcReady.event()) 
							{
								if(LOG >= 4)
									eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
									<<this->name()<<" IC: vcReady event..."<<endl;
							}
							else if(switch_cntrl.event()) 
							{
								if(LOG >= 4)
									eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
									<<this->name()<<" IC: unknown clock event..."<<endl;
							}

							// read next VCid sent by VC
							vc[vc_to_serve].vc_next_id = ports_VCA.nextVCID.read();

							if(vc[vc_to_serve].vc_next_id == NUM_VCS+1) {	// VC not granted
								if(LOG >= 4) 
									eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
									<<this->name()
									<<" IC: No free next vc, pushing flit in Q" <<endl;
								// push flit back in fifo
								//flit_out.simdata.num_waits++;
								//vc[vc_to_serve].vcQ.flit_push(flit_out);
								ports_VCA.vcRequest.write(false);
								continue;
							}
						}

						oldvcid = flit_out.vcid;

					} // end if, this block not executed for core OC

					flit_out = vc[vc_to_serve].vcQ.flit_out();	// read flit from fifo
					if( !isCoreIO(i) ) {	// not to be done for core OC
						flit_out.vcid = vc[vc_to_serve].vc_next_id;
					}
					// write flit to output port
					flit_out.simdata.num_sw++;
					flit_out.simdata.ctime = sc_time_stamp();
					ports_oc.outport[i].write(flit_out);

					if(LOG >= 2)
						eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
						<<this->name()<<" tile: "<<tileID<<" cntrl: "<<cntrlID
						<<" Transmitting flit to output port: "<<i<<flit_out;

					//if hdt/tail flit, put freeVC signal on creditLine
					//if head/data flit, if fifo buf is free, then put freeBuf signal creditLine
					// Update credit info
					if(LOG >= 4)
						eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<
						this->name()<<" IC: Updating freeVC status for vc:"<<vc_to_serve<<endl;

					// if hdt/tail flit, free VC
					// else free buffer
					if(flit_out.type == TAIL || flit_out.type == HDT)
					{
						vc[oldvcid].vc_next_id = NUM_VCS+1;
						creditLine t; t.freeVC = true; t.freeBuf = true;
						credit_out[vc_to_serve].write(t);
						//if(cntrlID == C)
						vc[vc_to_serve].vc_route = 5;
					}
					else {
						creditLine t; t.freeVC = false; t.freeBuf = true;
						credit_out[vc_to_serve].write(t);
					}
					ports_VCA.vcRequest.write(false);
				} // end outReady == true
				else {
					if(LOG >= 4)
						eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
						<<this->name()<<" IC: OC cannot accept flit!"<<endl;
				}
			} // end serving VC
		} //end if switch_cntrl
	} // end while
} //end transmit_flit()

///////////////////////////////////////////////////////////////////////////
/// Method to assign tile IDs and port IDs
///////////////////////////////////////////////////////////////////////////
void InputChannel::setTileID(UI id, UI port_N, UI port_S, UI port_E, UI port_W, UI portU, UI portD) {
	tileID = id;
	/*portN = port_N;
	portS = port_S;
	portE = port_E;
	portW = port_W;*/
	resetCounts();
}

///////////////////////////////////////////////////////////////////////////
/// Method to resut buffer stats to zero
///////////////////////////////////////////////////////////////////////////
void InputChannel::resetCounts(){
	numBufReads = numBufWrites = 0;
	numBufsOcc = 0;
	numVCOcc = 0;
}

///////////////////////////////////////////////////////////////////////////
/// Method to close logfiles
///////////////////////////////////////////////////////////////////////////
void InputChannel::closeLogs(){
	//	logcout.close();
}

///////////////////////////////////////////////////////////////////////////
/// Method to store flit in fifo buffer
///////////////////////////////////////////////////////////////////////////
void InputChannel::store_flit_VC(flit *flit_in) 
{
	int vc_id = flit_in->vcid;
	if(LOG >= 4)
		eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
		<<this->name()<<" Buffer: "<<vc[vc_id].vcQ.pntr;
	if(vc[vc_id].vcQ.full == true)
	{
		if(LOG >= 4)
			eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
			<<this->name()<<" Error: DATA has arrived. vcQ is full!"<<endl;
	}
	else 
	{
		vc[vc_id].vcQ.flit_in(*flit_in);
		numBufWrites++;
		creditLine t;
		t.freeVC = false; t.freeBuf = !vc[vc_id].vcQ.full;
		credit_out[vc_id].write(t);
	}
}

///////////////////////////////////////////////////////////////////////////
/// Method to call Controller for source routing
///////////////////////////////////////////////////////////////////////////
void InputChannel::routing_src(flit *flit_in) {
	int vc_id = flit_in->vcid;
	ports_ctr.rtRequest.write(ROUTE);
	ports_ctr.sourceAddress.write(flit_in->src);
	ports_ctr.destRequest.write(flit_in->data1);
	flit_in->data1 = flit_in->data1 >> 3; //Right shift
	if(LOG >= 4)
		eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
		<<this->name()<<" IC: rtRequest sent!"<<endl;
	wait();
	if(ports_ctr.rtReady.event()) {
		if(LOG >= 4)
			eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
			<<this->name()<<" IC: rtReady event..."<<endl;
	}
	else if(switch_cntrl.event()) {
		if(LOG >= 4)
			eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
			<<this->name()<<" IC: unknown clock event..."<<endl;
	}
	else if(LOG >= 4)
		eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
		<<this->name()<<" IC: Unknown Event!"<<endl;

	vc[vc_id].vc_route = ports_ctr.nextRt.read();
	ports_ctr.rtRequest.write(NONE);
}

///////////////////////////////////////////////////////////////////////////////////////
/// Method to call controller for routing algorithms that require destination address
//////////////////////////////////////////////////////////////////////////////////////
void InputChannel::routing_dst(flit *flit_in) {
	int vc_id = flit_in->vcid;
	ports_ctr.rtRequest.write(ROUTE);
	ports_ctr.sourceAddress.write(flit_in->src);
	ports_ctr.destRequest.write(flit_in->data1);
	if(LOG >= 4)
		eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
		<<this->name()<<" IC: rtRequest sent!"<<endl;
	wait();
	if(ports_ctr.rtReady.event()) {
		if(LOG >= 4)
			eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
			<<this->name()<<" IC: rtReady event..."<<endl;
	}
	else if(switch_cntrl.event()) {
		if(LOG >= 4)
			eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "
			<<this->name()<<" IC: unknown clock event..."<<endl;
	}
	vc[vc_id].vc_route = ports_ctr.nextRt.read();
	ports_ctr.rtRequest.write(NONE);
}

//////////////////////////////////////////////////
// Ant routines

// Route table lookup on basis of interconnects
int InputChannel::reverse_route(int rt_code) {
	int rt_rev;
	switch(rt_code) {
		case 0: rt_rev = 1; break;
		case 1: rt_rev = 0; break;
		case 2: rt_rev = 3; break;
		case 3: rt_rev = 2; break;
	};
	return rt_rev;
}