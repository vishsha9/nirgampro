#ifndef __WIRES__
#define __WIRES__

#include "../constants.h"
#include "../extern.h"
#include "flit.h"
#include "baseWireModel.h"
#include "credit.h"
#include <queue>
#include <map>
//#include "systemc.h"


///////////////////////////////////////////////
/// \brief signals to connect neighboring tiles
///////////////////////////////////////////////
struct wireModule : public sc_module {
	sc_in<bool> clk;	///< Clock input port

	/// Constructor
	SC_HAS_PROCESS(wireModule);
	wireModule(sc_module_name wire, baseWireModel* para);	

	baseWireModel* para;

	void entry();
	
	//void delay();

	void doDelaySig1();
	void doDelaySig2();
	void doDelayCrd1();
	void doDelayCrd2();


	void setTileID(UI, UI);

	//virtual UI getDelayTime()=0;

	UI delayTime;
	// ps

	UI tileID_1;
	UI tileID_2;

	sc_signal<flit> sig_from1;
	map<int, flit> map_sig_1to2;
	sc_signal<flit> sig_to2;

	sc_signal<flit> sig_from2;
	map<int, flit> map_sig_2to1;
	sc_signal<flit> sig_to1;

	
	sc_signal<creditLine>	credit_from1[NUM_VCS];	///< input ports for credit line (buffer status)
	map<int, creditLine> map_credit_1to2[NUM_VCS];
	sc_signal<creditLine>	credit_to2[NUM_VCS];	///< output ports for credit line (buffer status)


	sc_signal<creditLine>	credit_from2[NUM_VCS];	///< input ports for credit line (buffer status)
	map<int, creditLine> map_credit_2to1[NUM_VCS];
	sc_signal<creditLine>	credit_to1[NUM_VCS];	///< output ports for credit line (buffer status)
	
	/////
	/*sc_event e_sig_1;
	sc_event e_sig_2;
	sc_event e_crd_1[NUM_VCS];
	sc_event e_crd_2[NUM_VCS];
	void notify();
	void delay_notf();
	flit b_s1to2;
	flit b_s2to1;
	creditLine b_c1to2[NUM_VCS];
	creditLine b_c2to1[NUM_VCS];*/
	/////

	//sc_signal<flit> sig_to2;			///< data line (flit line) from a tile to its South neighbor
	//sc_signal<flit> sig_from;			///< data line (flit line) to a tile from its South neighbor

	//sc_signal<creditLine> cr_sig_to[NUM_VCS];	///< credit line (transmits buffer status) per virtual channel from a tile to its South neighbor
	//sc_signal<creditLine> cr_sig_from[NUM_VCS];	///< credit line (transmits buffer status) per virtual channel to a tile from its South neighbor
};

#endif