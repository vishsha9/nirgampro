
/*
 * TG.cpp
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
/// \file TG.cpp
/// \brief Implements abstract traffic generator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TG.h"
#include "../extern.h"

////////////////////////////////////////////////
/// Constructor
////////////////////////////////////////////////
TrafficGenerator::TrafficGenerator(sc_module_name TrafficGenerator): ipcore(TrafficGenerator) {
	flit_interval = 1;	// inter flit interval
	cycles_per_flit = 2;	// clock cycles required for processing of a flit
}

////////////////////////////////////////////////
/// Thread sensitive to clock
/// - inherited from ipcore
/// - send flits as per traffic configuration
////////////////////////////////////////////////
void TrafficGenerator::send() {
	int num_pkts_gen = 0;	// initialize number of packets generated to zero
	wait(WARMUP);		// wait for WARMUP period
	
	string field;
	
	// open traffic log file
	char str_id[3];
	sprintf(str_id, "%d", tileID);
	string traffic_filename = string("log/traffic/tile-") + string(str_id);
	ifstream trafstream;
	trafstream.open(traffic_filename.c_str());

	int start_latency=0;
	int ctrl = cluster_ctrl(RT_ALGO, 0, 0, 0, 0);

	// read inter-flit interval
	trafstream >> field >> flit_interval;
	trafstream >> field >> start_latency;
		// format
		ULL flit_start_due; 
		
		if(start_latency > 0)
			wait(start_latency);
		// generate traffic until TG_NUM
		while(sim_count <= TG_NUM && !trafstream.eof()) {
			// format
			trafstream >> field >> flit_start_due;
			// read inter-pkt interval
			trafstream >> field >> next_pkt_time;
			// read no. of flits in packet
			trafstream >> field >> num_flits;
			// read destination or route code
			trafstream >> field >> route_info;

			//////////////////////////////////////////////////////////////////////////
			//status[tileID][route_info]++;
			//////////////////////////////////////////////////////////////////////////

			int num_flits_gen = 0;
			flit flit_out;
			
			// create hdt/head flit
			if(num_flits == 1){
				write_reg_hdt(num_pkts_gen, num_flits_gen, route_info, ctrl, 10, 10);
				flit_out = flit_reg;
			}
			
			else{
				write_reg_head(num_pkts_gen, num_flits_gen, route_info, ctrl, 10, 10);
				flit_out = flit_reg;
			}

			//format
			flit_out.simdata.gtimestamp = flit_start_due;
			if(flit_start_due < CAL_NUM){
				sent_flit[tileID].insert(flit_start_due);
				last_sent_flit[tileID] = flit_start_due;
			}
			else
				last_sent_flit[tileID] = CAL_NUM;

			if(LOG >= 1)
				eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<flit_out;
				
			// wait while buffer has space
			while(!credit_in[0].read().freeBuf) {
				//cout<<"Time: "<<sc_time_stamp()<<" ipcore: "<<tileID<<" No space in core buffer"<<endl;
				wait();
				next_pkt_time--;
			}
			
			// write flit to output port
			flit_outport.write(flit_out);

			if(LOG >= 1)
				eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Sending flit from core "<<flit_out;
			
			num_flits_gen = 1;
			// generate subsequent flits in packet
			while(num_flits_gen < num_flits) {
				wait(flit_interval);
				next_pkt_time -= flit_interval;
				
				// create flit
				if(num_flits_gen == num_flits-1)
					flit_out = *create_tail_flit(num_pkts_gen, num_flits_gen);
				else
					flit_out = *create_data_flit(num_pkts_gen, num_flits_gen);
				
				num_flits_gen++;

				// format
				flit_start_due += flit_interval;
				flit_out.simdata.gtimestamp = flit_start_due;
				if(flit_start_due < CAL_NUM)
					sent_flit[tileID].insert(flit_start_due);

				//trafstream<<sim_count<<endl;
				if(LOG >= 1)
					eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Generating flit at core "<<flit_out;
					
				// wait until space in buffer
				while(!credit_in[0].read().freeBuf) {
					//cout<<"Time: "<<sc_time_stamp()<<" ipcore: "<<tileID<<" No space in core buffer"<<endl;
					wait();
					next_pkt_time--;
				}
				
				// send flit
				flit_outport.write(flit_out);
				
				if(LOG >= 1)
					eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Sending flit from core "<<flit_out;
				
			}

			num_pkts_gen++;
			if(next_pkt_time > 0)
				wait(next_pkt_time);
			else wait(1);
		}
	trafstream.close();	// close traffic config file
}

////////////////////////////////////////////////
/// Thread sensitive to clock and inport event
/// - inherited from ipcore
/// - receive incoming flits
/// - assign arrival timestamps
////////////////////////////////////////////////
void TrafficGenerator::recv() {
	while(true) {
		wait();
		if(flit_inport.event()) {
			flit flit_recd = flit_inport.read();
			flit_recd.simdata.atimestamp = sim_count;
			flit_recd.simdata.atime = sc_time_stamp();
			if(LOG >= 1)
				eventlog<<"\ntime: "<<sc_time_stamp()<<" name: "<<this->name()<<" tile: "<<tileID<<" Received flit at core "<<flit_recd<<flit_recd.simdata;
			////////////////////////////////////
			// format 
			ULL flit_stime = flit_recd.simdata.gtimestamp;
			UL src_id = flit_recd.src;
			if(flit_stime < CAL_NUM){
				// the flit have be recved, remove it in the set
				if(sent_flit[src_id].count(flit_stime) >0)
					sent_flit[src_id].erase(flit_stime);
				
				ULL flit_l = flit_recd.simdata.atimestamp - flit_stime;
				flit_latency<<"src " << flit_recd.src <<", dest "<< tileID <<", latency "<< flit_l<<", sendt "<< flit_stime <<", recvt "<< flit_recd.simdata.atimestamp <<endl;
				flit_latency_total += flit_l;
				flit_num++;
			}
			////////////////////////////////////
		}
	}
}

////////////////////////////////////////////////
/// Method to convert time unit from string representation to systemC representation
////////////////////////////////////////////////
sc_time_unit TrafficGenerator::strToTime(string unit) {
	if(unit == "s")
		return SC_SEC;
	if(unit == "ms")
		return SC_MS;
	if(unit == "us")
		return SC_US;
	if(unit == "ns")
		return SC_NS;
	if(unit == "ps")
		return SC_PS;
	if(unit == "fs")
		return SC_FS;
}


