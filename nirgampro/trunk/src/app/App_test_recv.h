#ifndef _APP_TEST_RECV_H_
#define _APP_TEST_RECV_H_

#include "../core/ipcore.h"
#include "../extern.h"
#include <fstream>
#include <math.h>

using namespace std;

struct App_test_recv:public ipcore
{
	SC_CTOR(App_test_recv);

	void send();
	void recv();
};

#endif
