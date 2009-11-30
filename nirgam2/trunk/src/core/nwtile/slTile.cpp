#include "slTile.h"

//////////////////////////////////////////////////////////////////////////
#include "address_decoding_table.h"
#include "mapping_table.h"
#include "loader.h"

#include "slSecmap.h"

using soclib::common::IntTab;
using soclib::common::Segment;

vector<ipmap_entry> ipmap;
vector<secmap_entry> secmap;
soclib::common::AddressDecodingTable<uint32_t, int> Addr2Id;
soclib::common::MappingTable *pMapTab;
soclib::common::Loader *pLoader;

void loadSocLibConfig(){
	// read ip location configuration (ipmap.config)
	if (!read_ipmap(gc_ipmapFile.c_str(), ipmap))	
	{
		cerr << "read_ipmap error" << endl;
		exit(-1);
	}
	cout << "ipmap: " << endl;
	print_ipmap(ipmap);

	// read section location configuration (secmap.config)
	if (!read_secmap(gc_secmapFile, secmap))	
	{
		cerr << "read_secmap error" << endl;
		exit(-1);
	}

	//debug
	cout << "secmap: " << endl;
	print_secmap(secmap);
	//
	pLoader = new soclib::common::Loader(ga_elfPath.c_str());	//argv[1] is now the program name

	//make address decoding table:

	pMapTab = new soclib::common::MappingTable(32, IntTab(8), IntTab(8), 0x00300000);

	for (int i = 0; i < secmap.size(); i++)
	{
		pMapTab->add(Segment(secmap[i].secname,
			secmap[i].base,
			secmap[i].size,
			IntTab(secmap[i].id),
			secmap[i].cacheability));
	}
	Addr2Id = pMapTab->getRoutingTable(soclib::common::IntTab());
}

//////////////////////////////////////////////////////////////////////////

static int proc_cnt = 0;

bool SlTile::isCoreIO(UI i){
	return i == io_num -1 ;
}

SlTile::SlTile(sc_module_name SlTile, UI tileID, UI nb_num) : BaseNWTile(SlTile, tileID, nb_num),
vcAlloc("VA", nb_num+1),
ctr("Controller", nb_num+1, &portTable){
	this->nb_num = nb_num;
	this->io_num = nb_num + 1;

	Ichannel = new InputChannel*[io_num];
	Ochannel = new OutputChannel*[io_num];

	sigs_IcOc = new Sigs_IcOc*[io_num];
	for (int i =0; i < io_num; i++)
	{
		sigs_IcOc[i] = new Sigs_IcOc[io_num];
	}
	sigs_IcVca = new Sigs_IcVca[io_num];
	sigs_IcCtl = new Sigs_IcCtl[io_num];

	for (int i=0; i < io_num; i++)
	{
		char name[4];
		sprintf(name, "IC%d", i);
		Ichannel[i] = new InputChannel(name, io_num);
		sprintf(name, "OC%d", i);
		Ochannel[i] = new OutputChannel(name, io_num);
	}

	//////////////////////////////////////////////////////////////////////////
	// not soclib
	/*
	if(g_appLibName[tileID] == "NULL")
	ip = NULL;
	else{
	string name = string(1, tileID+'0') + string(1, '_') + 
	g_appLibName[tileID].substr(0, g_appLibName[tileID].find_first_of('.'));
	if(g_appLibName[tileID].compare("App_test_recv.so") == 0)
	ip = new App_test_recv(name.c_str());
	else if(g_appLibName[tileID].compare("App_test_send.so") == 0)
	ip = new App_test_send(name.c_str());
	else if(g_appLibName[tileID].compare("selfCheck.so") == 0)
	ip = new selfCheck(name.c_str());
	else{
	cout<<"error: no such APP"<<endl;
	exit(-1);
	}
	ip->tileID = tileID;
	}
	*/
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// attach soclib components to NoC node:
	int idx;
	for (idx = 0; idx < ipmap.size(); idx++)
		if (ipmap[idx].id == tileID) 
			break;

	cout << "IPMAP size "<< ipmap.size() << endl;
	if (idx >= ipmap.size())	//this is a tile without ip
	{
		cout << tileID <<" null" << endl;
		ip = NULL;
	}
	else
	{
		char buf[128];

		if ("PROC" == ipmap[idx].type)
		{
			cout << tileID <<" PROC" << endl;
			typedef soclib::common::Mips32ElIss ProcessorIss;
			typedef soclib::common::GdbServer<ProcessorIss> Processor;

			soclib::caba::VciXcacheWrapper<soclib::caba::vci_param, Processor>* pCache = 
				new soclib::caba::VciXcacheWrapper<soclib::caba::vci_param, Processor>( 
				ipmap[idx].ipname.c_str(), proc_cnt, *pMapTab, 
				IntTab(ipmap[idx].id), 
				1, 8, 4, 1, 8, 4);
			//debug
			cout << "pcache = " << (int) pCache << endl;
			cout << "proc_cnt = " << proc_cnt << endl;
			//

			pCache->p_clk(*g_clock);
			pCache->p_resetn(g_resetN);
			pCache->p_irq[0](*(new sc_signal<bool>("signal_irq0")));
			pCache->p_irq[1](*(new sc_signal<bool>("signal_irq1")));
			pCache->p_irq[2](*(new sc_signal<bool>("signal_irq2")));
			pCache->p_irq[3](*(new sc_signal<bool>("signal_irq3")));
			pCache->p_irq[4](*(new sc_signal<bool>("signal_irq4")));
			pCache->p_irq[5](*(new sc_signal<bool>("signal_irq5")));

			pCache->p_vci(vci_signals);
			sprintf(buf, "InitiatorInterface_%i", tileID);
			ip = new InitiatorInterface(buf);
			static_cast<InitiatorInterface*>(ip)->vci_to_initiator(vci_signals);

			//debug
			sprintf(buf, "mips_%d_r_pc", proc_cnt);
			//sc_trace(tracefile, pCache->m_iss.r_pc, buf);
			//

			proc_cnt++;

		} else if ("RAM" == ipmap[idx].type)
		{
			cout << tileID <<" RAM" << endl;
			soclib::caba::VciRam<soclib::caba::vci_param> *pRam = 
				new soclib::caba::VciRam<soclib::caba::vci_param>(
				ipmap[idx].ipname.c_str(), IntTab(tileID), *pMapTab, *pLoader);

			pRam->p_clk(*g_clock);
			pRam->p_resetn(g_resetN);

			pRam->p_vci(vci_signals);
			sprintf(buf, "TargetInterface_%i", tileID);
			ip = new TargetInterface(buf);
			static_cast<TargetInterface*>(ip)->vci_to_target(vci_signals);

		} else if ("TTY" == ipmap[idx].type)
		{
			cout << tileID <<" TTY" << endl;
			soclib::caba::VciMultiTty<soclib::caba::vci_param> *pTty = 
				new soclib::caba::VciMultiTty<soclib::caba::vci_param>(
				ipmap[idx].ipname.c_str(), IntTab(tileID), *pMapTab,"vcitty", NULL);

			pTty->p_clk(*g_clock);
			pTty->p_resetn(g_resetN);
			pTty->p_irq[0](*(new sc_signal<bool>("tty_irq")));

			pTty->p_vci(vci_signals);
			sprintf(buf, "TargetInterface_%i", tileID);
			ip = new TargetInterface(buf);
			static_cast<TargetInterface*>(ip)->vci_to_target(vci_signals);

		} else if ("LOCK" == ipmap[idx].type)
		{
			cout << tileID <<" LOCK" << endl;
			soclib::caba::VciLocks<soclib::caba::vci_param> *pLock = 
				new soclib::caba::VciLocks<soclib::caba::vci_param>(
				ipmap[idx].ipname.c_str(), IntTab(tileID), *pMapTab);

			pLock->p_clk(*g_clock);
			pLock->p_resetn(g_resetN);

			pLock->p_vci(vci_signals);
			sprintf(buf, "TargetInterface_%i", tileID);
			ip = new TargetInterface(buf);
			static_cast<TargetInterface*>(ip)->vci_to_target(vci_signals);

		} else if ("ICU" == ipmap[idx].type)
		{
			cout << tileID <<" ICU" << endl;
			soclib::caba::VciIcu<soclib::caba::vci_param> *pIcu = 
				new soclib::caba::VciIcu<soclib::caba::vci_param>(
				ipmap[idx].ipname.c_str(), IntTab(tileID), *pMapTab, 1);

			pIcu->p_clk(*g_clock);
			pIcu->p_resetn(g_resetN);
			pIcu->p_irq_in[0](*(new sc_signal<bool>("tty_irq_in")));
			pIcu->p_irq(*(new sc_signal<bool>("irq_to_proc")));

			pIcu->p_vci(vci_signals);
			sprintf(buf, "TargetInterface_%i", tileID);
			ip = new TargetInterface(buf);
			static_cast<TargetInterface*>(ip)->vci_to_target(vci_signals);

		}
	}
	//////////////////////////////////////////////////////////////////////////
	setID(tileID);

	innerConnect();
}

void SlTile::innerConnect(){
	vcAlloc.switch_cntrl(*g_clock);
	for (UI i = 0; i < io_num; i++)
	{
		vcAlloc.vcRequest[i](sigs_IcVca[i].vcReq);
		vcAlloc.vcReady[i](sigs_IcVca[i].vcReady);
		vcAlloc.opRequest[i](sigs_IcVca[i].opReq);
		vcAlloc.nextVCID[i](sigs_IcVca[i].nextvc);
		for (UI j = 0; j < NUM_VCS; j++)
		{
			if(isCoreIO(i))
				vcAlloc.credit_in[i][j](creditIC_CS[j]);
			else
				vcAlloc.credit_in[i][j](credit_in[i][j]);
		}
	}

	ctr.switch_cntrl(*g_clock);
	for (UI i = 0; i < io_num; i++)
	{
		ctr.rtRequest[i](sigs_IcCtl[i].rtReq);
		ctr.destRequest[i](sigs_IcCtl[i].destReq);
		ctr.sourceAddress[i](sigs_IcCtl[i].srcAddr);
		ctr.rtReady[i](sigs_IcCtl[i].rtReady);
		ctr.nextRt[i](sigs_IcCtl[i].nextRt);
		for (UI j = 0; j < NUM_VCS; j++)
		{
			if (isCoreIO(i))
				ctr.credit_in[i][j](creditIC_CS[j]);
			else
				ctr.credit_in[i][j](credit_in[i][j]);
		}
	}

	for (UI i = 0; i < io_num; i++)
	{
		Ichannel[i]->cntrlID = i;
		Ichannel[i]->switch_cntrl(*g_clock);
		if(isCoreIO(i))
			Ichannel[i]->inport(sigs_IcIp.flit_CS_IC);
		else
			Ichannel[i]->inport(ip_port[i]);

		for (UI j = 0; j < io_num; j++)
		{
			Ichannel[i]->ports_oc.outport[j](sigs_IcOc[i][j].flit_sig);
			Ichannel[i]->ports_oc.outReady[j](sigs_IcOc[i][j].rdy);
		}

		for (UI j = 0; j < NUM_VCS; j++)
		{
			if(isCoreIO(i))
				Ichannel[i]->credit_out[j](creditIC_CS[j]);
			else
				Ichannel[i]->credit_out[j](credit_out[i][j]);
		}

		Ichannel[i]->ports_VCA.nextVCID(sigs_IcVca[i].nextvc);
		Ichannel[i]->ports_VCA.opRequest(sigs_IcVca[i].opReq);
		Ichannel[i]->ports_VCA.vcReady(sigs_IcVca[i].vcReady);
		Ichannel[i]->ports_VCA.vcRequest(sigs_IcVca[i].vcReq);

		Ichannel[i]->ports_ctr.destRequest(sigs_IcCtl[i].destReq);
		Ichannel[i]->ports_ctr.nextRt(sigs_IcCtl[i].nextRt);
		Ichannel[i]->ports_ctr.rtReady(sigs_IcCtl[i].rtReady);
		Ichannel[i]->ports_ctr.rtRequest(sigs_IcCtl[i].rtReq);
		Ichannel[i]->ports_ctr.sourceAddress(sigs_IcCtl[i].srcAddr);
	}

	for (UI i = 0; i < io_num; i++)
	{
		Ochannel[i]->cntrlID = i;
		Ochannel[i]->switch_cntrl(*g_clock);
		if (isCoreIO(i))
			Ochannel[i]->outport(sigs_OcIp.flit_OC_CR);
		else
			Ochannel[i]->outport(op_port[i]);

		for (UI j = 0; j < io_num; j++)
		{
			Ochannel[i]->inport[j](sigs_IcOc[j][i].flit_sig);
			Ochannel[i]->inReady[j](sigs_IcOc[j][i].rdy);
		}

		for (UI j = 0; j < NUM_VCS; j++)
		{
			if(isCoreIO(i))
				Ochannel[i]->credit_in[j](creditIC_CS[j]);
			else
				Ochannel[i]->credit_in[j](credit_in[i][j]);
		}
	}
	if(ip !=NULL ) {
		ip->resetn(g_resetN);
		ip->clock(*g_clock);
		ip->flit_inport(sigs_OcIp.flit_OC_CR);
		ip->flit_outport(sigs_IcIp.flit_CS_IC);
		for (UI i = 0; i < NUM_VCS; i++)
			ip->credit_in[i](creditIC_CS[i]);
	}
}


void SlTile::setID(UI id) {
	tileID = id;

	for(int i = 0; i < io_num; i++)
		Ichannel[i]->setTileID(id);

	for(int i = 0; i < io_num; i++)
		Ochannel[i]->setTileID(id);

	vcAlloc.setTileID(id);

	for (int i=0; i<ipmap.size(); i++)
		if (ipmap[i].id == id)
			ip->setID(id);
	//if(g_appLibName[id] != "NULL")
	//	ip->setID(id);

	ctr.setTileID(id);
}