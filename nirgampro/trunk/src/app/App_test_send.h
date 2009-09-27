#ifndef _APP_TEST_SEND_H_
#define _APP_TEST_SEND_H_

#include "../core/ipcore.h"

#include <fstream>
#include <math.h>

using namespace std;

struct App_test_send: public ipcore
{
	SC_CTOR(App_test_send);

	void send();
	void recv();
};

#endif
