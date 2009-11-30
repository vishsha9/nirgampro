#include "selfCheck.h"

selfCheck::selfCheck(sc_module_name App_test_recv): ipcore(App_test_recv)
{
	/**/

	SC_THREAD(recv);
	sensitive << clock << flit_inport;

	SC_THREAD(send);
	sensitive_pos << clock;

	/*for(int i=0; i< num_tiles* num_tiles; i++)
		ckTable[i] = false;*/
}

void selfCheck::send()
{
	int dest = tileID;
	int ctrl = cluster_ctrl(0, 0, 0, 0, 0);
	while (true)
	{	
		wait();
		if((g_simCount-tileID)%g_tileNum == 0){
			if (g_ckTable[tileID*g_tileNum + dest] == -1)
			{
				//cout << "sim_count "<< sim_count << " "  << tileID << " send to "<< dest << endl;
				write_reg_hdt(0, 0, dest, ctrl, 10, 10);
				flit_reg.field4 = sc_time_stamp().value();
				flit_reg.src = tileID;
				if (credit_in[0].read().freeBuf){
					flit_outport.write(flit_reg);
				}
				dest++;
				dest = dest % g_tileNum;
			}
		}
	}
}

void selfCheck::recv()
{
	while (true)
	{
		wait();
		if (flit_inport.event())
		{
			flit flit_recvd = flit_inport.read();
			UI src = flit_recvd.src;
			//cout << "sim_count "<< sim_count << " "  << tileID << " recv from "<< src << endl;
			g_ckTable[src*g_tileNum + tileID] = sc_time_stamp().value() - flit_recvd.field4;
		}
	}
}