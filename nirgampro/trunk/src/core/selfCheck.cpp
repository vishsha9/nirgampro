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
	int ctrl = cluster_ctrl(RT_ALGO, 0, 0, 0, 0);
	while (true)
	{	
		wait();
		if((sim_count-tileID)%num_tiles == 0){
			if (g_ckTable[tileID*num_tiles + dest] == -1)
			{
				//cout << "sim_count "<< sim_count << " "  << tileID << " send to "<< dest << endl;
				write_reg_hdt(0, 0, dest, ctrl, 10, 10);
				flit_reg.data4 = sc_time_stamp().value();
				flit_reg.src = tileID;
				if (credit_in[0].read().freeBuf){
					flit_outport.write(flit_reg);
				}
				dest++;
				dest = dest % num_tiles;
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
			g_ckTable[src*num_tiles + tileID] = sc_time_stamp().value() - flit_recvd.data4;
		}
	}
}