#ifndef _WIRES_
#define _WIRES_



#include "constants.h"
#include "flit.h"
#include "credit.h"
#include <queue>
#include <map>
//#include "systemc.h"

///////////////////////////////////////////////
/// \brief signals to connect neighboring tiles
///////////////////////////////////////////////
struct wires : public sc_module {
	/// Constructor
	sc_in<bool> clk;	///< Clock input port

	//wires(){	}	///< default constructor

	SC_HAS_PROCESS(wires);
	wires(sc_module_name wires, UI length);	

	void entry();
	void delay_sig();
	void delay_credit();
	void doDelay();


	UI getDelay(UI length);


	UI delay_time;
	// unit ps

	UI length;

	UI tileID_1;
	UI tileID_2;

	sc_signal<flit> sig_from1;
	map<int, flit> map_sig_1to2;
	sc_signal<flit> sig_to2;

	sc_signal<flit> sig_from2;
	map<int, flit> map_sig_2to1;
	sc_signal<flit> sig_to1;

	//sc_signal<flit> sig_to2;			///< data line (flit line) from a tile to its South neighbor
	//sc_signal<flit> sig_from;			///< data line (flit line) to a tile from its South neighbor
	
	sc_signal<creditLine>	credit_from1[NUM_VCS];	///< input ports for credit line (buffer status)
	map<int, creditLine> map_credit_1to2[NUM_VCS];
	sc_signal<creditLine>	credit_to2[NUM_VCS];	///< output ports for credit line (buffer status)


	sc_signal<creditLine>	credit_from2[NUM_VCS];	///< input ports for credit line (buffer status)
	map<int, creditLine> map_credit_2to1[NUM_VCS];
	sc_signal<creditLine>	credit_to1[NUM_VCS];	///< output ports for credit line (buffer status)
	
	

	//sc_signal<creditLine> cr_sig_to[NUM_VCS];	///< credit line (transmits buffer status) per virtual channel from a tile to its South neighbor
	//sc_signal<creditLine> cr_sig_from[NUM_VCS];	///< credit line (transmits buffer status) per virtual channel to a tile from its South neighbor
};

#endif