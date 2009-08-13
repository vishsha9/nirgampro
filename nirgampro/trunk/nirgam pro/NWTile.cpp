#include "NWTile.h"

NWTile::NWTile(sc_module_name NWTile, UI tileID, UI nb_num) : BaseNWTile(NWTile, tileID, nb_num),
vcAlloc("VA", nb_num+1),
ctr("Controller", nb_num+1){
	this->tileID = tileID;
	this->nb_num = nb_num;
	this->io_num = nb_num + 1;

	Ichannel = new InputChannel*[nb_num+1];
	Ochannel = new OutputChannel*[nb_num+1];

	sigs_IcOc = new Sigs_IcOc*[nb_num+1];
	for (int i =0; i<nb_num+1; i++)
	{
		sigs_IcOc[i] = new Sigs_IcOc[nb_num+1];
	}
	sigs_IcVca = new Sigs_IcVca[nb_num+1];
	sigs_IcCtl = new Sigs_IcCtl[nb_num+1];

	for (int i=0; i<nb_num; i++)
	{
		char name[4];
		sprintf(name, "IC%d", i);
		Ichannel[i] = new InputChannel(name, nb_num);
		sprintf(name, "OC%d", i);
		Ochannel[i] = new OutputChannel(name, nb_num);
	}
	for(int i =0; i<io_num; i++){
		ctr.innerConnect(i, *nw_clock, sigs_IcCtl[i], creditIC_CS, credit_in);
	}
	
}

void NWTile::innerConnect(){

}