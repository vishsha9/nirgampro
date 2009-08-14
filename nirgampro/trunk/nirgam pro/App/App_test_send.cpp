#include "App_test_send.h"
#include "../extern.h"

App_test_send::App_test_send(sc_module_name App_test_send):ipcore(App_test_send)
{
	SC_THREAD(send);
	sensitive_pos<<clock;
}

void App_test_send::send()
{
	int ctrl = cluster_ctrl(RT_ALGO, 0, 0, 0, 0);
	write_reg_hdt(0, 0, 5, ctrl, 10, 10);
	flit_reg.data4 = sc_time_stamp().value();
	if (credit_in[0].read().freeBuf){
		cout << "Send flit" << endl;
		flit_outport.write(flit_reg);
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
