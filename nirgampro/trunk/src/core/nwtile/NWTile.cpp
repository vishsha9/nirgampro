#include "NWTile.h"


bool NWTile::isCoreIO(UI i){
	return i == io_num -1 ;
}

NWTile::NWTile(sc_module_name NWTile, UI tileID, UI nb_num) : BaseNWTile(NWTile, tileID, nb_num),
vcAlloc("VA", nb_num+1),
ctr("Controller", nb_num+1, &indexTable){
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
		else if(g_appLibName[tileID].compare("CBR.so") == 0)
			ip = new CBRTraffic(name.c_str());
		else{
			cout<<"error: no such APP"<<endl;
			exit(-1);
		}
		ip->tileID = tileID;
	}
	setID(tileID);

	innerConnect();
}

void NWTile::innerConnect(){
	vcAlloc.switch_cntrl(*nw_clock);
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

	ctr.switch_cntrl(*nw_clock);
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
		Ichannel[i]->switch_cntrl(*nw_clock);
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
		Ochannel[i]->switch_cntrl(*nw_clock);
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
	if(g_appLibName[tileID] != "NULL") {
		ip->clock(*nw_clock);
		ip->flit_inport(sigs_OcIp.flit_OC_CR);
		ip->flit_outport(sigs_IcIp.flit_CS_IC);
		for (UI i = 0; i < NUM_VCS; i++)
			ip->credit_in[i](creditIC_CS[i]);
	}
}


void NWTile::setID(UI id) {
	tileID = id;

	for(int i = 0; i < io_num; i++)
		Ichannel[i]->setTileID(id);

	for(int i = 0; i < io_num; i++)
		Ochannel[i]->setTileID(id);

	vcAlloc.setTileID(id);

	if(g_appLibName[id] != "NULL")
		ip->setID(id);

	ctr.setTileID(id);
}

void NWTile::closeLogs() {
	/*cout << "Tile_" << tileID << " adjNodeNUm:" << nb_num << endl;
	for (int i=0; i< nb_id.size(); i++)
	{
		cout << "\tport_" << i << " to " << nb_id.at(i) << endl;
	}
	cout << "tileId: " << tileID << " ionum: " << io_num << endl;*/
	for(int i = 0; i < io_num; i++)
		Ochannel[i]->closeLogs();
}