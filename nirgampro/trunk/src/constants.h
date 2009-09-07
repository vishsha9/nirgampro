#ifndef _CONSTANTS_H
#define _CONSTANTS_H


#include <string>
using namespace std;


enum flit_type{
	HEAD,
	DATA,
	TAIL,
	HDT
};

// buffer parameters
#define	BUF_BITSIZE	4
/// maximum buffer depth
#define	MAX_NUM_BUFS	16

/// number of virtual channels
#define NUM_VCS 1
#define VCS_BITSIZE 3 // 0~7

// parameters for bitwidth
#define ADDR_SIZE 32

#endif