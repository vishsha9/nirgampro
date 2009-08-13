#ifndef _CONSTANTS_H
#define _CONSTANTS_H


#include <string>
using namespace std;

#define UI  unsigned int
#define UL  unsigned long
#define ULL unsigned long long

enum flit_type{
	HEAD,
	DATA,
	TAIL,
	HDT
};

/// number of virtual channels
#define NUM_VCS 1
// parameters for bitwidth
#define ADDR_SIZE 32
#define VCS_BITSIZE 2


//// parameters for NWTile
///// number of neighbors of a general tile in mesh/torus
//#define NUM_NB 1
///// number of input channels in a general tile in mesh/torus
//#define NUM_IC 1
///// number of output channels in a general tile in mesh/torus
//#define NUM_OC 1
//
//// parameters for mesh (non-toroidal) topology
///// number of neighbors of a corner tile in mesh
//#define NUM_NB_C 3
///// number of input channels in a corner tile in mesh
//#define NUM_IC_C 4
///// number of output channels in a corner tile in mesh
//#define NUM_OC_C 4
//
//// parameters for mesh (non-toroidal) topology
///// number of neighbors of a border tile in mesh
//#define NUM_NB_B 4
///// number of input channels in a border tile in mesh
//#define NUM_IC_B 5
///// number of output channels in a border tile in mesh
//#define NUM_OC_B 5
//
//#define NUM_NB_F 5
//#define NUM_IC_F 6
//#define NUM_OC_F 6
//
//// parameters for topology
///// maximum number of rows
//#define MAX_NUM_ROWS 3
///// maximum number of columns
//#define MAX_NUM_COLS 3
///// maximum number of vertical
//#define MAX_NUM_VERT 3

extern ofstream readlog;
extern ofstream writelog;
extern ofstream delaylogin;
extern ofstream delaylogout;

#endif