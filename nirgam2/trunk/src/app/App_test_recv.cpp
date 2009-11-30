#include "App_test_recv.h"


App_test_recv::App_test_recv(sc_module_name App_test_recv): ipcore(App_test_recv)
{
	SC_THREAD(recv);
	sensitive << clock << flit_inport;
}

void App_test_recv::send()
{
}
void App_test_recv::recv()
{
	while (true)
	{
		wait();
		if (flit_inport.event())
		{
			flit flit_recvd = flit_inport.read();
			cout << "Time:" << sc_time_stamp().value() << " flit received. timestamp:" << flit_recvd.field4 << endl;
		}
	}
}


//extern "C"
//{
//	ipcore *maker()
//	{
//		return new App_test_recv("App_test_recv");
//	}
//}
