#ifndef __PTM_WIRE__
#define  __PTM_WIRE__

#include "wires.h"
#include <math.h>
#include "../Notation/notation.h"

 

//////////////////////////////////////////////////////////////////////////
// PTM interconnect
// For top global layer (Coupling lines above one metal ground),
//    the above Cground is not existed.
// The graphics blow present local and intermediate layers model 
//    (Coupling lines bewteen two metal ground planes)
// _______________________________________________
//
//                    <--w-->     dielectric c: K	
//        _______   _ _______     _______
//        |ooooo|   | |ooooo|     |ooooo|
//        |ooooo|   t |ooooo|--s--|ooooo|
//        |ooooo|   | |ooooo|     |ooooo|
//        -------   - -------     -------
//                       |
//                       h
// ______________________|________________________
//
// _______________________________________________
//                       | Cground
//                       = 	
//        ____Ccouple  __|___  Ccouple __
//        |ooo        |ooooo|         oo|
//        |ooooo|-||--|ooooo|--||-|ooooo|
//        |ooooo|     |ooooo|     |ooooo|
//        -------     -------     -------
//                       |
//                       = Cground
// ______________________|________________________
//
//
// Top Global Layer
//   R:
//   Ctotal = Cground + 2*Ccouple
//     Cground = 
//     Ccouple = 
//
// Local Layer
//   R:
//   Ctotal = Cground + 2*Ccouple
//     Cground = 
//     Ccouple = 
//
//////////////////////////////////////////////////////////////////////////

typedef struct ptm_para{
	double w;
	double s;
	double length;
	double t;
	double h;
	double K;
	double rou;
}ptm_para;


struct ptmWire: public wires{
	SC_HAS_PROCESS(ptmWire);
	ptmWire(sc_module_name wires);

	ptm_para para;
protected:
	double Cg;
	double Cc;
	double R;
	virtual double getCg(ptm_para para)=0;
	virtual double getCc(ptm_para para)=0;
	double getR(ptm_para para);
	UI getDelayTime();
};

struct ptmTopWire: public ptmWire{
	SC_HAS_PROCESS(ptmTopWire);
	ptmTopWire(sc_module_name wires, ptm_para para);
protected:
	double getCg(ptm_para para);
	double getCc(ptm_para para);
};

struct ptmLocalWire: public ptmWire{
	SC_HAS_PROCESS(ptmLocalWire);
	ptmLocalWire(sc_module_name wires, ptm_para para);
protected:
	double getCg(ptm_para para);
	double getCc(ptm_para para);
};


#endif


