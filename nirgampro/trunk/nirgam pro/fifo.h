
/*
 * fifo.h
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
/// \file fifo.h
/// \brief Defines first-in-first-out buffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _FIFO_
#define _FIFO_
#include "flit.h"
#include "extern.h"

/////////////////////////////////////////
/// \brief fifo data structure
////////////////////////////////////////
struct fifo {
	sc_uint<BUF_BITSIZE> num_bufs;	///< buffer depth (number of buffers in fifo)
	flit regs[MAX_NUM_BUFS];	///< register array to store flits
	bool full;			///< full status of buffer
	bool empty;			///< empty status of buffer
	sc_uint<BUF_BITSIZE> pntr;	///< number of registers occupied (index of last flit inserted)
	
	/// Constructor
	fifo() {
		full = false;	// initialize full status to false
		empty = true;	// initialize empty status to true
		pntr = 0;	// no registers are occupied
		num_bufs = NUM_BUFS;	// initialize depth of buffer as read from user
    	};

	// FUNCTIONS /////////////////////////////////////////////////////////////////////////////
	void flit_in(const flit& data_flit);	///< insert flit in fifo queue
	flit flit_out();			///< read and remove flit from fifo queue
	void flit_push(flit pack);		///< push back flit in queue at original position
	flit flit_read();			///< read flit from fifo without removing it
	// FUNCTIONS END /////////////////////////////////////////////////////////////////////////////
};

#endif
