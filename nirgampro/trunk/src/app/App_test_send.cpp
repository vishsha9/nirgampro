#include "App_test_send.h"


App_test_send::App_test_send(sc_module_name App_test_send):ipcore(App_test_send)
{
	SC_THREAD(send);
	sensitive_pos<<clock;
}

void App_test_send::send()
{
	int ctrl = cluster_ctrl(RT_ALGO, 0, 0, 0, 0);
	int i =0;
	while (i<6)
	{
		wait();

		i++;
		if(i == 1)
			write_reg_head(0, 0, 2, ctrl, 10, 10);
		else
			write_reg_data(0, 0, 2, ctrl, 10, 10);

		flit_reg.field4 = sc_time_stamp().value();
		flit_reg.src = tileID;
		if (credit_in[0].read().freeBuf){
			flit_outport.write(flit_reg);
		}
	}
}

void App_test_send::recv()
{

}

//extern "C"
//{
//	ipcore* maker()
//	{
//		return new App_test_send("App_test_send");
//	}
//}
