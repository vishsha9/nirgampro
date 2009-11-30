#include "slInitiatorInterface.h"
#include "address_decoding_table.h"

extern soclib::common::AddressDecodingTable<uint32_t, int> Addr2Id;

using soclib::caba::vci_param;

using namespace std;

InitiatorInterface::InitiatorInterface(sc_module_name name) : ipcore(name)
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

	this->name = string((const char*) name);
/*
	char buf[256];

	sprintf(buf, "%s.clock", (const char*) name);
	sc_trace(tracefile, clock, buf);

	sprintf(buf, "%s.resetn", (const char*) name);
	sc_trace(tracefile, resetn, buf);

	sprintf(buf, "%s.ready", (const char*) name);
	sc_trace(tracefile, ready, buf);

	sprintf(buf, "%s.pktid", (const char*) name);
	sc_trace(tracefile, pktid, buf);

	sprintf(buf, "%s.CMDVAL", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.cmdval, buf);

	sprintf(buf, "%s.CMDACK", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.cmdack, buf);

	sprintf(buf, "%s.RSPACK", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.rspack, buf);

	sprintf(buf, "%s.RSPVAL", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.rspval, buf);

	sprintf(buf, "%s.REOP", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.reop, buf);

	sprintf(buf, "%s.EOP", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.eop, buf);

	sprintf(buf, "%s.ADDRESS", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.address, buf);

	sprintf(buf, "%s.PLEN", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.plen, buf);

	sprintf(buf, "%s.CONTIG", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.contig, buf);

	sprintf(buf, "%s.CMD", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.cmd, buf);

	sprintf(buf, "%s.BE", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.be, buf);

	sprintf(buf, "%s.WDATA", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.wdata, buf);

	sprintf(buf, "%s.RDATA", (const char*) name);
	sc_trace(tracefile, vci_to_initiator.rdata, buf);

	sprintf(buf, "%s.dest", (const char*) name);
	sc_trace(tracefile, dest, buf);

	sprintf(buf, "%s.recv_src", (const char*) name);
	sc_trace(tracefile, recv_src, buf);

	sprintf(buf, "%s.recv_pktid", (const char*) name);
	sc_trace(tracefile, recv_pktid, buf);

	sprintf(buf, "%s.partly", (const char*) name);
	sc_trace(tracefile, partly, buf);

	sprintf(buf, "%s.recv_flag", (const char*) name);
	sc_trace(tracefile, recv_flag, buf);*/

}

void InitiatorInterface::transition(void)
{
		if (resetn == false)	//do initialization
		{
			cout << "reset once" << endl;

			ready = true;
			accepted = false;
			pktid = 0;
			flitid = 0;

			partly = false;
			recv_src = 0;
			recv_pktid = 0;
			pktid = 0;

			recv_flag = false;

			vci_to_initiator.cmdack = false;	//turn to true @genMoore with ready 
			vci_to_initiator.rspval = false;	

			return;
		}

		if (ready)
		{
			if(vci_to_initiator.cmdval.read())
			{
				int alg = 0;
				//int alg = static_cast<int> (RT_ALGO);
				int cmd = static_cast<int> (vci_to_initiator.cmd.read());
				int contig = static_cast<int> (vci_to_initiator.contig.read());
				int be = static_cast<int> (vci_to_initiator.be.read());
				int plen = static_cast<int>(vci_to_initiator.plen.read());
				UI addr = static_cast<UI>(vci_to_initiator.address.read());

				dest = Addr2Id[addr];

				//if ((SOURCE == RT_ALGO) || (XY == RT_ALGO))
					route = dest;
				UI ctrl = cluster_ctrl(alg, cmd, be, contig, plen); 
				UI data = static_cast<UI>(vci_to_initiator.wdata.read());
			
				if (0 == flitid)	//start of a packet
				{
					if (vci_to_initiator.eop.read())
					{
						write_reg_hdt(pktid, flitid, route, ctrl, addr, data);
						pktid++;
					} else
					{
						write_reg_head(pktid, flitid, route, ctrl, addr, data);
						flitid++;
					}
				} else				// middle of a packet
				{
					if (vci_to_initiator.eop.read())
					{
						write_reg_tail(pktid, flitid, route, ctrl, addr, data);
						pktid++;
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
					ready = false;	//don't accept request at next cycle
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

		// state transition for response information
		accepted = false;
		if (vci_to_initiator.rspval.read() && vci_to_initiator.rspack.read())
			accepted = true;	//the initiator has accepted the data

}

void InitiatorInterface::recv(void)
{
		if (flit_inport.event())
		{
			recv_flag = !recv_flag;

			flit f = flit_inport.read();

/*
				//debug:
				cout << "Initiator interface got data" <<endl;
				cout << f.data1 <<"\t" <<  hex << f.data2 << "\t"  << f.data4 << endl;
				cout << f.type << endl;
				//

*/
			feedbuf.flit_in(f);
		}

}

void InitiatorInterface::genMoore(void)
{
		//for sending request
		if (ready)
			vci_to_initiator.cmdack = true;
		else
			vci_to_initiator.cmdack = false;
		
		//for received response
		if (accepted)
		{
			flit out = feedbuf.flit_out();
/*
			//debug
			cout << "out @" << out.data1 << endl;
			cout << "rdata = " << hex << out.data4 << "\t" << "rtype =" << out.type << endl;
			//
*/
		}
			
		if (!feedbuf.empty)
		{
			flit rsp = feedbuf.flit_read();

			UI ctrl = rsp.field2;
			UI data = rsp.field4;

			int cmd, be, contig, rerror, algo;
			disperse_ctrl(ctrl, algo, cmd, be, contig, rerror);
			
			if ((HDT == rsp.type) || (TAIL == rsp.type))
				vci_to_initiator.reop = true;
			else
				vci_to_initiator.reop = false;
			
			vci_to_initiator.rerror = static_cast<vci_param::rerror_t>(rerror);
			vci_to_initiator.rdata = static_cast<vci_param::data_t>(data);
			vci_to_initiator.rsrcid = static_cast<vci_param::srcid_t>(rsp.src);
			vci_to_initiator.rpktid = static_cast<vci_param::pktid_t>(rsp.pktid);

			vci_to_initiator.rspval = true;

/*
			//debug
			cout << "response received @" << rsp.data1 << endl;
			cout << "rdata = " << hex << data << "\t" << "rtype =" << rsp.type << endl;
			//
*/
		} else	//no pending flits in feed buf
		{
		 	vci_to_initiator.rspval = false;
			vci_to_initiator.reop = false;
			vci_to_initiator.rerror = static_cast<vci_param::rerror_t>(0);
			vci_to_initiator.rdata = static_cast<vci_param::data_t>(0);
			vci_to_initiator.rsrcid = static_cast<vci_param::srcid_t>(0);
			vci_to_initiator.rpktid = static_cast<vci_param::pktid_t>(0);

	 	}
}
