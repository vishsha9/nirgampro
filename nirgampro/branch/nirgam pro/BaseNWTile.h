
/*
 * BaseNWTile.h
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
/// \file BaseNWTile.h
/// \brief Defines abstract module for network tile
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BASE_NWTILE_
#define _BASE_NWTILE_

#include "constants.h"
#include "systemc.h"
#include "flit.h"
#include "credit.h"

///////////////////////////////////////////////////////////////////
/// \brief Abstract class to represent network tile.
//////////////////////////////////////////////////////////////////
struct BaseNWTile : public sc_module {

	UI tileID;	///< unique tile id
	sc_in<bool>	clk;		///< input clock port

	BaseNWTile() {	}	///< default constructor


	virtual bool connect(UI nb_id, 
		sc_signal<flit>& sig_in, 
		sc_signal<flit>& sig_out, 
		sc_signal<creditLine> crd_in[NUM_VCS], 
		sc_signal<creditLine> crd_out[NUM_VCS]);

	/// systemC constructor
	/// parameters - module name, tile id.
	SC_HAS_PROCESS(BaseNWTile);
	BaseNWTile(sc_module_name BaseNWTile, UI id) : sc_module(BaseNWTile) {
	}
};

#endif
