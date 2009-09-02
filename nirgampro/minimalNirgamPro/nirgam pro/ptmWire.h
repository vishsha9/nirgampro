#ifndef __PTM_WIRE__
#define  __PTM_WIRE__

#include "wires.h"
#include <math.h>
#include <string>

#include "../netlist_analyzer/netlist_analyzer.h"
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


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
enum PTM_LAYER{PTM_TOP, PTM_LOCAL};

struct ptmWire;

struct ptm_para: public WirePara{
	PTM_LAYER layer;
	double w;//
	double s;//
	double length;
	double t;//
	double h;//
	double K;//
	double rou;
	bool setFieldByName(string name, double val);
	ptm_para* clone();
	wire* getWire(string wirename);
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
struct ptmWire: public wire{
	SC_HAS_PROCESS(ptmWire);
	ptmWire(sc_module_name wires);

protected:
	double Cg;
	double Cc;
	double R;
	virtual double getCg(ptm_para* para)=0;
	virtual double getCc(ptm_para* para)=0;
	double getR(ptm_para *para);
	UI getDelayTime();
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
struct ptmTopWire: public ptmWire{
	SC_HAS_PROCESS(ptmTopWire);
	ptmTopWire(sc_module_name wires, ptm_para* para);
protected:
	double getCg(ptm_para *para);
	double getCc(ptm_para *para);
};

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
struct ptmLocalWire: public ptmWire{
	SC_HAS_PROCESS(ptmLocalWire);
	ptmLocalWire(sc_module_name wires, ptm_para* para);
protected:
	double getCg(ptm_para *para);
	double getCc(ptm_para *para);
};


#endif


