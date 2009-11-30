#include "wireModule.h"

wireModule::wireModule(sc_module_name wires, baseWireModel* para): sc_module(wires){
	this->para = para;
	this->delayTime = para->getDelayTime();

	SC_THREAD(doDelaySig1);
	sensitive<< sig_from1;

	SC_THREAD(doDelaySig2);
	sensitive<< sig_from2;

	SC_THREAD(doDelayCrd1);
	for(int i=0; i< NUM_VCS ; i++){
		sensitive << credit_from1[i];
	}

	SC_THREAD(doDelayCrd2);
	for(int i=0; i< NUM_VCS ; i++){
		sensitive << credit_from2[i];
	}

	//this->length = length;
	/*SC_THREAD(delay_sig);
	for(int i=0; i< NUM_VCS ; i++){
	sensitive << credit_from1[0] << credit_from2[0];
	}
	sensitive<< clk << sig_from1<< sig_from2;*/

	/*SC_THREAD(notify);
	for(int i=0; i< NUM_VCS ; i++){
	sensitive << credit_from1[i] << credit_from2[i];
	}
	sensitive << sig_from1<< sig_from2;

	SC_THREAD(delay_notf);
	sensitive << e_sig_1 << e_sig_2;
	for(int i=0; i< NUM_VCS; i++)
	{
	sensitive << e_crd_1[i] << e_crd_2[i];
	}*/
}

void wireModule::setTileID(UI tile1, UI tile2){
	this->tileID_1 = tile1;
	this->tileID_2 = tile2;
}

void wireModule::doDelaySig1(){
	flit f_1to2;
	
	for(int i=0; i < NUM_VCS ; i++){
		creditLine t; t.freeVC = true; t.freeBuf = true;
		credit_to2[i].write(t);
	}

	while(true){
		wait();
		if(sig_from1.event()){
			f_1to2 = sig_from1.read();
			//delaylogin << "CLK: " << sc_time_stamp() << "\twire readflit from 1 data:" << f_1to2.field4<< "\ttimestamp "<< f_1to2.simdata.gtime  << endl;
		}
		wait(delayTime, SC_PS);
		sig_to2.write(f_1to2);
		//delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire writeflit from 1 data:" << f_1to2.field4<< "\ttimestamp "<< f_1to2.simdata.gtime  << endl;
	}
}

void wireModule::doDelaySig2(){
	flit f_2to1;

	for(int i=0; i < NUM_VCS ; i++){
		creditLine t; t.freeVC = true; t.freeBuf = true;
		credit_to1[i].write(t);
	}

	while(true){
		wait();
		if(sig_from2.event()){
			f_2to1 = sig_from2.read();
			//delaylogin << "CLK: " << sc_time_stamp() << "\twire readflit from 2 data:" << f_2to1.field4<< "\ttimestamp "<< f_2to1.simdata.gtime  << endl;
		}
		wait(delayTime, SC_PS);
		sig_to1.write(f_2to1);
		//delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire writeflit from 2 data:" << f_2to1.field4<< "\ttimestamp "<< f_2to1.simdata.gtime  << endl;
	}
}

void wireModule::doDelayCrd1(){
	creditLine c_1to2[NUM_VCS];
	bool inflag[NUM_VCS];
	while(true){
		wait();
		for(int i=0; i<NUM_VCS; i++){
			if(credit_from1[i].event()){
				c_1to2[i] = credit_from1[i].read();
				inflag[i] = true;
				//delaylogin << "CLK: " << sc_time_stamp() << "\t credit from 1\ttimestamp "<< c_1to2[i].gtime  << endl;
			}
			else
				inflag[i] = false;
		}
		wait(delayTime, SC_PS);
		for(int i=0; i<NUM_VCS; i++){
			if(inflag[i]){
				credit_to2[i].write(c_1to2[i]);
				//delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire credit from 1\tVC_"<< i <<"\ttimestamp "<< c_1to2[i].gtime  << endl;
			}
		}
	}
}

void wireModule::doDelayCrd2(){
	creditLine c_2to1[NUM_VCS];
	bool inflag[NUM_VCS];
	while(true){
		wait();
		for(int i=0; i<NUM_VCS; i++){
			if(credit_from2[i].event()){
				c_2to1[i] = credit_from2[i].read();
				inflag[i] = true;
				//delaylogin << "CLK: " << sc_time_stamp() << "\t credit from 1\ttimestamp "<< c_2to1[i].gtime  << endl;
			}
			else
				inflag[i] = false;
		}
		wait(delayTime, SC_PS);
		for(int i=0; i<NUM_VCS; i++){
			if(inflag[i]){
				credit_to1[i].write(c_2to1[i]);
				//delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire credit from 1\tVC_"<< i <<"\ttimestamp "<< c_2to1[i].gtime  << endl;
			}
		}
	}
}


/*
void wires::delay(){

	//map<int, flit> map1to2;
	sc_time cur_time;
	creditLine c_1to2[NUM_VCS], c_2to1[NUM_VCS];
	flit f_1to2, f_2to1;

	while (true)
	{
		wait();

		cur_time = sc_time_stamp();
		//////////////////////////////////////////////////////////////////////////
		// flit from 1 to 2 event
		//////////////////////////////////////////////////////////////////////////
		if(sig_from1.event()){
			f_1to2 = sig_from1.read();
			map_sig_1to2.insert(pair<int, flit>(cur_time.value(), f_1to2));
			delaylogin << "CLK: " << sc_time_stamp() << "\twire readflit from 1 " << f_1to2.field4<< "\ttimestamp "<< f_1to2.simdata.gtime  << endl;
		}
		//////////////////////////////////////////////////////////////////////////
		// flit from 2 to 1 event
		//////////////////////////////////////////////////////////////////////////
		if(sig_from2.event()){
			f_2to1 = sig_from2.read();
			map_sig_2to1.insert(pair<int, flit>(cur_time.value(), f_2to1));
			delaylogin << "CLK: " << sc_time_stamp() << "\twire readflit from 2 " << f_2to1.field4<< "\ttimestamp "<< f_2to1.simdata.gtime  << endl;
		}

		for(int i= 0; i < NUM_VCS; i++){
			//////////////////////////////////////////////////////////////////////////
			// credit from 1 to 2 event
			//////////////////////////////////////////////////////////////////////////
			if(credit_from1[i].event()){
				c_1to2[i] = credit_from1[i].read();
				map_credit_1to2[i].insert(pair<int, creditLine>(cur_time.value(), c_1to2[i]));
				delaylogin << "CLK: " << sc_time_stamp() << "\t credit from 1\ttimestamp "<< c_1to2[i].gtime  << endl;
			}
			//////////////////////////////////////////////////////////////////////////
			// credit from 2 to 1 event
			//////////////////////////////////////////////////////////////////////////
			if(credit_from2[i].event()){
				c_2to1[i] = credit_from2[i].read();
				map_credit_2to1[i].insert(pair<int, creditLine>(cur_time.value(), c_2to1[i]));
				delaylogin << "CLK: " << sc_time_stamp() << "\t credit from 2\ttimestamp "<< c_2to1[i].gtime  << endl;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		// clock event, pos_edge and neg_edge
		//////////////////////////////////////////////////////////////////////////
		if(clk.event()){
			map<int,  flit>::iterator iter, iend;
			int del; 

			iend = map_sig_1to2.end();
			del = -1;
			for(iter = map_sig_1to2.begin(); iter!=iend; iter++){
				if( cur_time.value() - (*iter).first >= delay_time){
					sig_to2.write((*iter).second);
					delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire writeflit from 1 " << (*iter).second.field4<< "\ttimestamp "<< (*iter).second.simdata.gtime  << endl;
					del = (*iter).first;
					break;
				}
			}
			if( del != -1)
				map_sig_1to2.erase(del);

			iend = map_sig_2to1.end();
			del = -1;
			for(iter = map_sig_2to1.begin(); iter!=iend; iter++){
				if( cur_time.value() - (*iter).first >= delay_time){
					sig_to1.write((*iter).second);
					delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire writeflit from 2 " << (*iter).second.field4<< "\ttimestamp "<< (*iter).second.simdata.gtime  << endl;
					del = (*iter).first;
					break;
				}
			}
			if( del != -1)
				map_sig_2to1.erase(del);

			map<int,  creditLine>::iterator iter2, iend2;
			for(int i= 0; i < NUM_VCS; i++){
				iend2 = map_credit_1to2[i].end();
				del = -1;
				for(iter2 = map_credit_1to2[i].begin(); iter2!=iend2; iter2++){
					if( cur_time.value() - (*iter2).first >= delay_time){
						credit_to2[i].write((*iter2).second);
						delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire credit from 1\tVC_"<< i <<"\ttimestamp "<< (*iter2).second.gtime  << endl;
						del = (*iter2).first;
						break;
					}
				}
				if( del != -1)
					map_credit_1to2[i].erase(del);

				iend2 = map_credit_2to1[i].end();
				del = -1;
				for(iter2 = map_credit_2to1[i].begin(); iter2!=iend2; iter2++){
					if( cur_time.value() - (*iter2).first >= delay_time){
						credit_to1[i].write((*iter2).second);
						delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire credit from 2\tVC_"<< i <<"\ttimestamp "<< (*iter2).second.gtime  << endl;
						del = (*iter2).first;
						break;
					}
				}
				if( del != -1)
					map_credit_2to1[i].erase(del);
			}
		}
	}
}
*/


/*
void wires::notify(){
while(true)
{
wait();
if(sig_from1.event()){
b_s1to2 = sig_from1.read();
e_sig_1.notify(delay_time, SC_PS);
delaylogin << "CLK: " << sc_time_stamp() << "\twire readflit from 1 " << b_s1to2.field4<< "\ttimestamp "<< b_s1to2.simdata.gtime  << endl;
}
if(sig_from2.event()){
b_s2to1 = sig_from2.read();
e_sig_2.notify(delay_time, SC_PS);
delaylogin << "CLK: " << sc_time_stamp() << "\twire readflit from 2 " << b_s2to1.field4<< "\ttimestamp "<< b_s2to1.simdata.gtime  << endl;
}
for(int i= 0; i < NUM_VCS; i++){
if(credit_from1[i].event()){
b_c1to2[i] = credit_from1[i].read();
e_crd_1[i].notify(delay_time, SC_PS);
}
if(credit_from2[i].event()){
b_c2to1[i] = credit_from2[i].read();
e_crd_2[i].notify(delay_time, SC_PS);
}
}
}
}

void wires::delay_notf(){
while(true)
{
wait();
if(e_sig_1){
delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire writeflit from 1 " << b_s2to1.field4<< "\ttimestamp "<< b_s2to1.simdata.gtime  << endl;
sig_to2.write(b_s1to2);
}
if(e_sig_2){
delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire writeflit from 2 " << b_s2to1.field4<< "\ttimestamp "<< b_s2to1.simdata.gtime  << endl;
sig_to1.write(b_s2to1);
b_s2to1 = sig_from2.read();
}
for(int i= 0; i < NUM_VCS; i++){
if(e_crd_1[i]){
credit_to2[i].write(b_c1to2[i]);
}
if(e_crd_2[i]){
credit_to1[i].write(b_c2to1[i]);
}
}
}
}*/
//////