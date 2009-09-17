
/*
 * NoC.h
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 * Author: Lavina Jain
 *
 */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \file NoC.h
/// \brief Defines network topology
///
/// This file defines:
/// - module NoC, a 2-dimentional topology of network tiles.
/// - module signals, a set of signals to connect tiles in the network.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __NOC__
#define __NOC__

#include "BaseNWTile.h"
#include "nwtile/NWTile.h"
#include "wireModule.h"
#include "topoAnalyzer.h"
#include "../extern.h"
//#include "../config/extern.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief Represents the entire Network-on-Chip
///
/// This module represents the NoC. It implements an array of network tiles and connectivity among them.
////////////////////////////////////////////////////////////////////////////////////////////////////////
struct NoC : public sc_module {
	/// Constructor
	SC_HAS_PROCESS(NoC);
	NoC(sc_module_name noc_name, AdjList* a);  // format

	~NoC();


	sc_in<bool> switch_cntrl;	///< Clock input port
	
	//int rows;	///< number of rows in topology
	//int cols;	///< number of columns in topology
	//int vert;   /// format number of vertical in topology

	UI tileNum;

	vector<BaseNWTile *> nwtile;	///< A 2-d array of network tiles
	vector<wireModule *>sigs;	///< Signals to interconnect network tiles


	wireModule * connect(sc_module_name wire_name, baseWireModel* para, BaseNWTile * tile1, BaseNWTile * tile2);
		
	void closeLogs();

	void entry();	///< Keeps count of number of simulation cycles
};

#endif
