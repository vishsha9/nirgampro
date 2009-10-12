#include "slTargetInterface.h"

using soclib::caba::vci_param;
using namespace std;

TargetInterface::TargetInterface(sc_module_name name) : ipcore(name) 
{
	// process sensitive to clock, sends out flit
	SC_METHOD(transition);
	dont_initialize();
	sensitive << clock.pos();

	// datum will be changed @ falling edge, according to current state 
	SC_METHOD(genMoore);
	dont_initialize();
	sensitive <<  clock.neg();

	SC_METHOD(recv);
	dont_initialize();
	sensitive << flit_inport << clock;

	/*
	char buf[256];

	sprintf(buf, "%s.cmdval", (const char*) name);
	sc_trace(tracefile, vci_to_target.cmdval, buf);

	sprintf(buf, "%s.cmdack", (const char*) name);
	sc_trace(tracefile, vci_to_target.cmdack, buf);

	sprintf(buf, "%s.rspval", (const char*) name);
	sc_trace(tracefile, vci_to_target.rspval, buf);

	sprintf(buf, "%s.rspack", (const char*) name);
	sc_trace(tracefile, vci_to_target.rspack, buf);

	sprintf(buf, "%s.cmd", (const char*) name);
	sc_trace(tracefile, vci_to_target.cmd, buf);

	sprintf(buf, "%s.be", (const char*) name);
	sc_trace(tracefile, vci_to_target.be, buf);

	sprintf(buf, "%s.address", (const char*) name);
	sc_trace(tracefile, vci_to_target.address, buf);

	sprintf(buf, "%s.plen", (const char*) name);
	sc_trace(tracefile, vci_to_target.plen, buf);

	sprintf(buf, "%s.contig", (const char*) name);
	sc_trace(tracefile, vci_to_target.contig, buf);

	sprintf(buf, "%s.wdata", (const char*) name);
	sc_trace(tracefile, vci_to_target.wdata, buf);

	sprintf(buf, "%s.rdata", (const char*) name);
	sc_trace(tracefile, vci_to_target.rdata, buf);

	sprintf(buf, "%s.rsrcid", (const char*) name);
	sc_trace(tracefile, vci_to_target.rsrcid, buf);

	sprintf(buf, "%s.rpktid", (const char*) name);
	sc_trace(tracefile, vci_to_target.rpktid, buf);

	sprintf(buf, "%s.eop", (const char*) name);
	sc_trace(tracefile, vci_to_target.eop, buf);

	sprintf(buf, "%s.reop", (const char*) name);
	sc_trace(tracefile, vci_to_target.reop, buf);

	sprintf(buf, "%s.accepted", (const char*) name);
	sc_trace(tracefile, accepted, buf);

	sprintf(buf, "%s.feedbuf.empty", (const char*) name);
	sc_trace(tracefile, feedbuf.empty, buf);

	sprintf(buf, "%s.partly", (const char*) name);
	sc_trace(tracefile, partly, buf);

	sprintf(buf, "%s.cur_src", (const char*) name);
	sc_trace(tracefile, cur_src, buf);

	sprintf(buf, "%s.cur_pktid", (const char*) name);
	sc_trace(tracefile, cur_pktid, buf);
	*/
}

void TargetInterface::transition(void)
{
		if (!resetn)	//do initialization
		{
			ready = true;
			accepted = false;
			pktid = 0;
			flitid = 0;

			partly = false;
			cur_src = 0;
			cur_pktid = 0;
	
			vci_to_target.cmdval = false;
			vci_to_target.rspack = false;

			return;
		}


		if (ready)
		{
			if(vci_to_target.rspval.read())
			{
				int alg = 0;
				//int alg = static_cast<int> (RT_ALGO);
				int cmd, be, contig;
				int rerror = static_cast<int>(vci_to_target.rerror.read());
	
				UI dest = static_cast<UI>(vci_to_target.rsrcid.read());
	
				UI route;
				//if ((SOURCE == RT_ALGO) || (XY == RT_ALGO))
					route = dest;
				UI ctrl = cluster_ctrl(alg, cmd, be, contig, rerror); 
				UI addr;
				UI data = static_cast<UI>(vci_to_target.rdata.read());
				
/*
				//debug:
				cout << "Target interface are to send data" <<endl;
				cout << dest << hex << data << endl;
				//
*/
				pktid = static_cast<UI>(vci_to_target.rpktid.read());
				
				if (0 == flitid)	//start of a packet
				{
					if (vci_to_target.reop.read())
						write_reg_hdt(pktid, flitid, route, ctrl, addr, data);
					else
					{
						write_reg_head(pktid, flitid, route, ctrl, addr, data);
						flitid++;
					}
				} else				// middle of a packet
				{
					if (vci_to_target.reop.read())
					{
						write_reg_tail(pktid, flitid, route, ctrl, addr, data);
						flitid = 0;
					} else
					{
						write_reg_data(pktid, flitid, route, ctrl, addr, data);
						flitid++;
					}
				}
					
				if (credit_in[0].read().freeBuf)
					flit_outport.write(flit_reg);	
				else
					ready = false;	//don't make ACK at next cycle
			}
		} else	//there is pending flit 
		{
			if (credit_in[0].read().freeBuf)
			{
				flit_outport.write(flit_reg);
				ready = true;
			} else
				ready = false;
		}
	
		// state transition for request information
		accepted = false;
		if (vci_to_target.cmdval.read() && vci_to_target.cmdack.read())
			accepted = true;	//the initiator has accepted the data

}

void TargetInterface::recv(void)
{
		if (flit_inport.event())
		{
			flit f = flit_inport.read();
			if (partly)	//waiting for remaining flits of a packet
			{
				if ((cur_src == f.src) && (cur_pktid == f.pktid))
				{
					feedbuf.flit_in(f);
					if (TAIL == f.type)
						partly = false;
				} else //not the wanted flit arrived, temporarily put it in recvbuf
				{
					list<flit>::iterator iter = recvbuf.begin();
					while ((iter != recvbuf.end()) 
							&& (iter->src != f.src) && (iter->pktid != f.pktid))
						iter++;

					while ((iter != recvbuf.end()) 
							&& (iter->src == f.src) && (iter->pktid == f.pktid))
						iter++;
					recvbuf.insert(iter, f);
				}
			} else	//waiting for a new packet
			{
				if (recvbuf.empty()) //under this satuation, f must be hdt or head
				{
					if (HDT == f.type)
					{
						cur_src = f.src;
						cur_pktid = f.pktid;
						
						feedbuf.flit_in(f);
					} else if (HEAD == f.type)
					{
						partly = true;
						cur_src = f.src;
						cur_pktid = f.pktid;	

						feedbuf.flit_in(f);
					} else
						assert(false);
				} else	//just insert in recvbuf, let head of recvbuf first
				{
					list<flit>::iterator iter = recvbuf.begin();
					while ((iter != recvbuf.end()) 
							&& (iter->src != f.src) && (iter->pktid != f.pktid))
						iter++;

					while ((iter != recvbuf.end()) 
							&& (iter->src == f.src) && (iter->pktid == f.pktid))
						iter++;
					recvbuf.insert(iter, f);
				}
			}
		}
}

void TargetInterface::genMoore(void)
{
		//for sending response
		if (ready)
			vci_to_target.rspack = true;
		else
			vci_to_target.rspack = false;
		
		//for received requests
		if (accepted)
			feedbuf.flit_out();
			
		if (!feedbuf.empty)
		{
			flit req = feedbuf.flit_read();

			UI ctrl = req.field2;
			UI addr = req.field3;
			UI data = req.field4;

			int cmd, be, contig, plen, algo;
			disperse_ctrl(ctrl, algo, cmd, be, contig, plen);
			
			if ((HDT == req.type) || (TAIL == req.type))
				vci_to_target.eop = true;
			else
				vci_to_target.eop = false;

			vci_to_target.cmd = static_cast<vci_param::cmd_t>(cmd);
			vci_to_target.be = static_cast<vci_param::be_t>(be);
			vci_to_target.address = static_cast<vci_param::addr_t>(addr);
			vci_to_target.plen = static_cast<vci_param::plen_t>(plen);
			vci_to_target.contig = static_cast<vci_param::contig_t>(contig);
			vci_to_target.wdata = static_cast<vci_param::data_t>(data);
			vci_to_target.srcid = static_cast<vci_param::srcid_t>(req.src);
			vci_to_target.pktid = static_cast<vci_param::pktid_t>(req.pktid);

			vci_to_target.cmdval = true;

		} else	//no pending flits in feed buf
		{
			if ((false == recvbuf.empty()) && (false == partly))
			{
				cur_src = recvbuf.front().src;
				cur_pktid = recvbuf.front().pktid;

				while (!recvbuf.empty())
				{
					if ((recvbuf.front().src == cur_src)
						&& (recvbuf.front().pktid == cur_pktid))
					{
						feedbuf.flit_in(recvbuf.front());
						
						if ((HDT == recvbuf.front().type) || TAIL == recvbuf.front().type)
							partly = false;
						else	//HEAD or DATA
							partly = true;

						recvbuf.pop_front();
					} else	//a new packet reached
						break;
				}
			}

			vci_to_target.cmdval = false;
			vci_to_target.eop = false;
			vci_to_target.cmd = static_cast<vci_param::cmd_t>(0);
			vci_to_target.be = static_cast<vci_param::be_t>(0);
			vci_to_target.address = static_cast<vci_param::addr_t>(0);
			vci_to_target.wdata = static_cast<vci_param::data_t>(0);
			vci_to_target.plen = static_cast<vci_param::plen_t>(0);
			vci_to_target.contig = static_cast<vci_param::contig_t>(0);
			vci_to_target.srcid = static_cast<vci_param::srcid_t>(0);
			vci_to_target.pktid = static_cast<vci_param::pktid_t>(0);
		}
}
