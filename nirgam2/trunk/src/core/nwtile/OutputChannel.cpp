
/*
 * OutputChannel.cpp
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
/// \file OutputChannel.cpp
/// \brief Implements module OutputChannel
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OutputChannel.h"

#include "../tracker/tracker.h"

////////////////////////
/// Constructor
////////////////////////
OutputChannel ::OutputChannel(sc_module_name OutputChannel, UI io_num): sc_module(OutputChannel) {
	this->io_num = io_num;
	inport = new sc_in<flit>[io_num];
	inReady = new sc_out<bool>[io_num];
	//r_in = new switch_reg[io_num];

	r_in.resize(io_num);
	r_vc.resize(NUM_VCS);

	// process to read and process incoming flits
	SC_THREAD(entry);
	for(UI i = 0; i < io_num; i++)
		sensitive<<inport[i];
	sensitive_pos << switch_cntrl;

	// initialize ready signal to true
	for(UI i = 0; i < io_num; i++)
		inReady[i].initialize(true);

	// initialize performance stats to zero
	latency = 0;
	num_pkts = 0;
	num_flits = 0;
	avg_latency = 0.0;
	avg_latency_flit = 0.0;
	avg_throughput = 0.0;

	beg_cycle = 0;
	end_cycle = 0;
	total_cycles = 0;
}

bool OutputChannel::isCoreIO(UI i){
	return i == io_num -1 ;
}

///////////////////////////////////////////////////////////////////////////
/// Process sensitive to inport event and clock event
/// - clock event:
///   - send flit from register r_vc to output port
///   - move any waiting flits from register r_in to r_vc
///   .
/// - inport event:
///   - read flit from inport and store in register r_in
///   .
/// .
///////////////////////////////////////////////////////////////////////////
void OutputChannel::entry() {
	//ULL sim_count = 0;	// keep track of clock cycles
	while(true) 
	{
		wait();
		if(switch_cntrl.event()) 	// clock event
		{
			//sim_count++;

			UI vc_to_serve = (g_simCount+NUM_VCS)%NUM_VCS;

			if(!r_vc[vc_to_serve].free) {	// flit in register r_vc

				// local channel, send flit from r_vc to outport, no need to check credit info
				if(isCoreIO(cntrlID))
				{

					// r_vc[vc_to_serve].val.simdata.ctime = sc_time_stamp(); moved to exitOutputChannel
					g_tracker->exitOutputChannel(tileID, cntrlID, r_vc[vc_to_serve].val);
					outport.write(r_vc[vc_to_serve].val);
					r_vc[vc_to_serve].free = true;

					

					// moved to exitOutputChannel
					/*if(r_vc[vc_to_serve].val.type == TAIL 
						|| r_vc[vc_to_serve].val.type == HDT) 
					{
						latency += sim_count -1 - input_time[vc_to_serve];
						num_pkts++;
						end_cycle = sim_count - 1;
					}
					num_flits++;*/

					if(LOG >= 2)
						eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "
						<<this->name()<<" tile: "<<tileID<<" cntrlID: "<<cntrlID
						<<" Sending out flit from OC "<<r_vc[vc_to_serve].val;
				}
				else // send flit to outport basis of credit info, if free space in buf at IC of next tile
				{	
					if(credit_in[r_vc[vc_to_serve].val.vcid].read().freeBuf) 
					{
						//r_vc[vc_to_serve].val.simdata.ctime = sc_time_stamp(); moved to exitOutputChannel
						g_tracker->exitOutputChannel(tileID, cntrlID, r_vc[vc_to_serve].val);
						outport.write(r_vc[vc_to_serve].val);
			
						

						// moved to exit OutputChannel
						/*if(r_vc[vc_to_serve].val.type == TAIL 
							|| r_vc[vc_to_serve].val.type == HDT) 
						{
							latency += sim_count -1 - input_time[vc_to_serve];
							num_pkts++;
							end_cycle = sim_count - 1;
						}
						num_flits++;*/

						if(LOG >= 2)
							eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "
							<<this->name()<<" tile: "<<tileID
							<<" cntrlID: "<<cntrlID
							<<" Buf at next tile is free, Sending out flit from OC "
							<<r_vc[vc_to_serve].val;

						r_vc[vc_to_serve].free = true;
					}
					else 
					{
						if(LOG >= 4)
							eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "
							<<this->name()<<" tile: "<<tileID
							<<" cntrlID: "<<cntrlID
							<<" Buf at next tile is not free for VC "
							<<vc_to_serve<<endl;
					}
				}
			}

			// if any r_vc got free, move any waiting flits from r_in to r_vc
			for(UI i = 0; i < io_num; i++) 
			{
				if(!r_in[i].free) 
				{
					if(r_vc[r_in[i].val.vcid].free) 
					{
						r_vc[r_in[i].val.vcid].val = r_in[i].val;

						g_tracker->getVcAtOutputChannel(tileID, cntrlID, r_in[i].val);
						/*if(r_in[i].val.type == HDT || r_in[i].val.type == HEAD)
							input_time[r_in[i].val.vcid] = r_in[i].val.simdata.ICtimestamp;*/

						r_vc[r_in[i].val.vcid].free = false;
						//int vc_id = r_in[i].val.vcid;
						r_in[i].free = true;
						inReady[i].write(true);
					}
				}
			}
		} //end if switch_cntrl

		// inport event, store the flit in corresponding register
		for(UI i = 0; i < io_num; i++) 
		{
			if(inport[i].event()) 
			{
				r_in[i].val = inport[i].read();
				r_in[i].free = false;

				g_tracker->enterOutputChannel(tileID, cntrlID, r_in[i].val);

				////if(r_in[i].val.pkthdr.nochdr.flittype == HDT || r_in[i].val.pkthdr.nochdr.flittype == HEAD)
				////	input_time = r_in[i].val.simdata.ICtimestamp;
				//if(beg_cycle == 0)
				//	//beg_cycle = input_time;
				//	beg_cycle = r_in[i].val.simdata.ICtimestamp;

				if(LOG >= 4)
					eventlog<<"\nTime: "<<sc_time_stamp()
					<<" name: "<<this->name()<<" tile: "<<tileID
					<<" cntrlID: "<<cntrlID<<" Recvd flit at port "<<i<<": "<<r_in[i].val;

				// if r_vc is not full, write r_in into it, else set ready signal to false
				if(!r_vc[r_in[i].val.vcid].free)
					inReady[i].write(false);
				else 
				{
					r_vc[r_in[i].val.vcid].val = r_in[i].val;
					g_tracker->getVcAtOutputChannel(tileID, cntrlID, r_in[i].val);
					/*if(r_in[i].val.type == HDT || r_in[i].val.type == HEAD)
						input_time[r_in[i].val.vcid] = r_in[i].val.simdata.ICtimestamp;*/

					r_vc[r_in[i].val.vcid].free = false;
					r_in[i].free = true;
					int vc_id = r_in[i].val.vcid;
					if(LOG >= 4)
						eventlog<<"\nTime: "<<sc_time_stamp()
						<<" name: "<<this->name()<<" tile: "<<tileID
						<<" cntrlID: "<<cntrlID<<" VC "<<vc_id
						<<" is free, putting in flit"<<endl;
					inReady[i].write(true);
				}
			} //end inport event
		}
	} //end while
} //end entry

///////////////////////////////////////////////////////////////////////////
/// Method to assign tile IDs and port IDs
///////////////////////////////////////////////////////////////////////////
void OutputChannel ::setTileID(UI id){
	tileID = id;
}

///////////////////////////////////////////////////////////////////////////
/// - close logfiles
/// - compute preformance stats (latency and throughput)
/// - dump results
///////////////////////////////////////////////////////////////////////////
//void OutputChannel ::closeLogs(){
//	if(num_pkts != 0)
//		avg_latency = (float)latency/num_pkts;
//	if(num_flits != 0)
//		avg_latency_flit = (float)latency/num_flits;
//	total_cycles = end_cycle - beg_cycle;
//	if(total_cycles != 0)
//		avg_throughput = (float)(num_flits * FLITSIZE * 8) / (total_cycles * g_clkPeriod);	// Gbps
//	//eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" cntrlID: "<<cntrlID<<" latency: "<<latency<<" num_pkts: "<<num_pkts<<" num_flits: "<<num_flits<<" avg per pkt: "<<avg_latency<<" avg per flit: "<<avg_latency_flit<<" cycles: "<<total_cycles;
//	if (isCoreIO(cntrlID))
//	{
//		g_resultsLog<<tileID<<"\ttoIP\t\t";
//		g_resultsLog<<num_pkts<<"\t\t"<<num_flits<<"\t\t"<<avg_latency<<"\t\t"<<avg_latency_flit<<"\t\t"<<avg_throughput<<endl;
//	}
//	else{
//		g_resultsLog<<tileID<<"\t"<<cntrlID<<"\t\t";
//		g_resultsLog<<num_pkts<<"\t\t"<<num_flits<<"\t\t"<<avg_latency<<"\t\t"<<avg_latency_flit<<"\t\t"<<avg_throughput<<endl;
//	}
//}