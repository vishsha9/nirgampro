#include "wires.h"

wires::wires(sc_module_name wires, UI length): sc_module(wires){
	this->length = length;
	this->delay_time = getDelay(this->length);


	/*SC_THREAD(delay_sig);
	for(int i=0; i< NUM_VCS ; i++){
	sensitive << credit_from1[0] << credit_from2[0];
	}
	sensitive<< clk << sig_from1<< sig_from2;*/

	SC_THREAD(doDelay);
	sensitive<< sig_from1;


}


void wires::doDelay(){
	flit f_1to2;
	sc_time cur_time;
	while(true){
		wait();
		if(sig_from1.event()){
			f_1to2 = sig_from1.read();
			map_sig_1to2.insert(pair<int, flit>(cur_time.value(), f_1to2));
			delaylogin << "CLK: " << sc_time_stamp() << "\twire readflit from 1 " << f_1to2.data4<< "\ttimestamp "<< f_1to2.simdata.gtime  << endl;
		}
		wait(1333, SC_PS);
		cur_time = sc_time_stamp();
		sig_to2.write(f_1to2);
		delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire writeflit from 1 " << f_1to2.data4<< "\ttimestamp "<< f_1to2.simdata.gtime  << endl;

	}
}


UI wires::getDelay(UI length){
	// add your code here to descript wire delay behavior
	return length;
}

void wires::delay_credit(){
	creditLine c_1to2[NUM_VCS], c_2to1[NUM_VCS];
}

void wires::delay_sig(){

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
			delaylogin << "CLK: " << sc_time_stamp() << "\twire readflit from 1 " << f_1to2.data4<< "\ttimestamp "<< f_1to2.simdata.gtime  << endl;
		}
		//////////////////////////////////////////////////////////////////////////
		// flit from 2 to 1 event
		//////////////////////////////////////////////////////////////////////////
		if(sig_from2.event()){
			f_2to1 = sig_from2.read();
			map_sig_2to1.insert(pair<int, flit>(cur_time.value(), f_2to1));
			delaylogin << "CLK: " << sc_time_stamp() << "\twire readflit from 2 " << f_2to1.data4<< "\ttimestamp "<< f_2to1.simdata.gtime  << endl;
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
					delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire writeflit from 1 " << (*iter).second.data4<< "\ttimestamp "<< (*iter).second.simdata.gtime  << endl;
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
					delaylogout << "write\tCLK: " << sc_time_stamp() << "\twire writeflit from 2 " << (*iter).second.data4<< "\ttimestamp "<< (*iter).second.simdata.gtime  << endl;
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
