
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
#include "../config/extern.h"

////////////////////////
/// Constructor
////////////////////////
template<UI num_ip>
OutputChannel<num_ip>::OutputChannel(sc_module_name OutputChannel): sc_module(OutputChannel) {

	// process to read and process incoming flits
	SC_THREAD(entry);
	for(UI i = 0; i < num_ip; i++)
		sensitive<<inport[i];
	sensitive_pos << switch_cntrl;

	// initialize ready signal to true
	for(UI i = 0; i < num_ip; i++)
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
template<UI num_ip>
void OutputChannel<num_ip>::entry() {
	
	ULL sim_count = 0;	// keep track of clock cycles
	while(sim_count < SIM_NUM) 
	{
		wait();
		if(switch_cntrl.event()) 	// clock event
		{
			sim_count++;

			if(!r_vc[(sim_count+NUM_VCS)%NUM_VCS].free) {	// flit in register r_vc
				
				// local channel, send flit from r_vc to outport, no need to check credit info
				if(cntrlID == C)
				{
					
					r_vc[(sim_count+NUM_VCS)%NUM_VCS].val.simdata.ctime = sc_time_stamp();
					outport.write(r_vc[(sim_count+NUM_VCS)%NUM_VCS].val);
					r_vc[(sim_count+NUM_VCS)%NUM_VCS].free = true;
					
					if(r_vc[(sim_count+NUM_VCS)%NUM_VCS].val.type == TAIL 
						|| r_vc[(sim_count+NUM_VCS)%NUM_VCS].val.type == HDT) 
					{
						latency += sim_count -1 - input_time[(sim_count+NUM_VCS)%NUM_VCS];
						num_pkts++;
						end_cycle = sim_count - 1;
					}
					num_flits++;
					
					if(LOG >= 2)
						eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "
						<<this->name()<<" tile: "<<tileID<<" cntrlID: "<<cntrlID
						<<" Sending out flit from OC "<<r_vc[(sim_count+NUM_VCS)%NUM_VCS].val;
				}
				else // send flit to outport basis of credit info, if free space in buf at IC of next tile
				{	
					if(credit_in[r_vc[(sim_count+NUM_VCS)%NUM_VCS].val.vcid].read().freeBuf) 
					{
						r_vc[(sim_count+NUM_VCS)%NUM_VCS].val.simdata.ctime = sc_time_stamp();
						outport.write(r_vc[(sim_count+NUM_VCS)%NUM_VCS].val);
						
						if(r_vc[(sim_count+NUM_VCS)%NUM_VCS].val.type == TAIL 
							|| r_vc[(sim_count+NUM_VCS)%NUM_VCS].val.type == HDT) 
						{
							latency += sim_count -1 - input_time[(sim_count+NUM_VCS)%NUM_VCS];
							num_pkts++;
							end_cycle = sim_count - 1;
						}
						num_flits++;
							
						if(LOG >= 2)
							eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "
									<<this->name()<<" tile: "<<tileID
									<<" cntrlID: "<<cntrlID
									<<" Buf at next tile is free, Sending out flit from OC "
									<<r_vc[(sim_count+NUM_VCS)%NUM_VCS].val;
						
						r_vc[(sim_count+NUM_VCS)%NUM_VCS].free = true;
					}
					else 
					{
						if(LOG >= 4)
							eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "
									<<this->name()<<" tile: "<<tileID
									<<" cntrlID: "<<cntrlID
									<<" Buf at next tile is not free for VC "
									<<(sim_count+NUM_VCS)%NUM_VCS<<endl;
					}
				}
			}

			// if any r_vc got free, move any waiting flits from r_in to r_vc
			for(UI i = 0; i <= num_ip; i++) 
			{
				if(!r_in[i].free) 
				{
					if(r_vc[r_in[i].val.vcid].free) 
					{
						r_vc[r_in[i].val.vcid].val = r_in[i].val;
						if(r_in[i].val.type == HDT || r_in[i].val.type == HEAD)
							input_time[r_in[i].val.vcid] = r_in[i].val.simdata.ICtimestamp;
					
						r_vc[r_in[i].val.vcid].free = false;
						//int vc_id = r_in[i].val.vcid;
						r_in[i].free = true;
						inReady[i].write(true);
					}
				}
			}
		} //end if switch_cntrl

		// inport event, store the flit in corresponding register
		for(UI i = 0; i < num_ip; i++) 
		{
			if(inport[i].event()) 
			{
				r_in[i].val = inport[i].read();
				r_in[i].free = false;
				//if(r_in[i].val.pkthdr.nochdr.flittype == HDT || r_in[i].val.pkthdr.nochdr.flittype == HEAD)
				//	input_time = r_in[i].val.simdata.ICtimestamp;
				if(beg_cycle == 0)
					//beg_cycle = input_time;
					beg_cycle = r_in[i].val.simdata.ICtimestamp;
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
					if(r_in[i].val.type == HDT || r_in[i].val.type == HEAD)
						input_time[r_in[i].val.vcid] = r_in[i].val.simdata.ICtimestamp;
					
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
template<UI num_ip>
void OutputChannel<num_ip>::setTileID(UI id, UI port_N, UI port_S, UI port_E, UI port_W){
	tileID = id;
	portN = port_N;
	portS = port_S;
	portE = port_E;
	portW = port_W;
}

///////////////////////////////////////////////////////////////////////////
/// - close logfiles
/// - compute preformance stats (latency and throughput)
/// - dump results
///////////////////////////////////////////////////////////////////////////
template<UI num_ip>
void OutputChannel<num_ip>::closeLogs(){
	if(num_pkts != 0)
		avg_latency = (float)latency/num_pkts;
	if(num_flits != 0)
		avg_latency_flit = (float)latency/num_flits;
	total_cycles = end_cycle - beg_cycle;
	if(total_cycles != 0)
		avg_throughput = (float)(num_flits * FLITSIZE * 8) / (total_cycles * CLK_PERIOD);	// Gbps
	//eventlog<<"\nTime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" cntrlID: "<<cntrlID<<" latency: "<<latency<<" num_pkts: "<<num_pkts<<" num_flits: "<<num_flits<<" avg per pkt: "<<avg_latency<<" avg per flit: "<<avg_latency_flit<<" cycles: "<<total_cycles;
	if(cntrlID != C) {
		results_log<<tileID<<"\t";
		switch(cntrlID) {
			case N: results_log<<"North\t\t"; break;
			case S: results_log<<"South\t\t"; break;
			case E: results_log<<"East\t\t"; break;
			case W: results_log<<"West\t\t"; break;
		}
		results_log<<num_pkts<<"\t\t"<<num_flits<<"\t\t"<<avg_latency<<"\t\t"<<avg_latency_flit<<"\t\t"<<avg_throughput<<endl;
	}
}

template struct OutputChannel<NUM_IC>;
template struct OutputChannel<NUM_IC_B>;
template struct OutputChannel<NUM_IC_C>;
