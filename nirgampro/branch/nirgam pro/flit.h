
/*
 * flit.h
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
/// \file flit.h
/// \brief Defines flow control unit(flit) for NoC
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _FLIT_INC_
#define _FLIT_INC_

#include "systemc.h"
#include "constants.h"
#include <sys/stat.h>
#include <sys/types.h>

/// required for stl




////////////////////////////////////////////////
/// \brief header to record simulation data
////////////////////////////////////////////////
struct sim_hdr {     
	sc_time gtime;		///< flit generation time (in time units)
	sc_time atime;		///< flit arrival time (in time units)
	sc_time ctime;		///< instantaneous time (in time units)
	ULL	gtimestamp;	///< flit generation timestamp (in clock cycle)
	ULL	atimestamp;	///< flit arrival timestamp at reciever (in clock cycle)
	ULL	ICtimestamp;	///< input channel time stamp (in clock cycles)
	ULL	num_waits;	///< no. of clock cycles spent waiting in buffer
	ULL	num_sw;		///< no. of switches traversed
};

////////////////////////////////////////////////
/// \brief flit data structure
///
/// This is the structure that represents flow control unit(flit) in NoC
////////////////////////////////////////////////
struct flit 
{
	int	 vcid;		///< virtual channel id
	UI	 src;		///< source tileID
	UI	pktid;		///< packet id
	UI	flitid;		///<flit id
	flit_type type; ///< HDT, HEAD, DATA or TAIL, defined in constants.h

	//128 bit wire

	UI	data1;		// for HDT/HEAD flit, it is route information
	UI	data2;		// control information
	UI	data3;		// ADDRESS
	UI	data4;		// WDATA or RDATA
	
	///<simulation data
	
	sim_hdr	simdata;

	// overloading equality operator
	inline bool operator == (const flit& temp) const
	{
		if(temp.simdata.gtime != simdata.gtime || temp.simdata.atime != simdata.atime || temp.simdata.ctime != simdata.ctime || temp.src != src)
			return false;
		
		if(temp.pktid != pktid || temp.type != type || temp.flitid != flitid)
			return false;
	
		return true;
	}
};

// overloading extraction operator for flit
inline ostream&
operator << ( ostream& os, const flit& temp ) 
{
	switch(temp.type) 
	{
		case HEAD: os<<"HEAD flit, "; break;
		case DATA: os<<"DATA flit, "; break;
		case TAIL: os<<"TAIL flit, "; break;
		case HDT: os<<"HDT flit, "; break;
	}
	os<<"src: "<<temp.src<<" pktid: "<<temp.pktid<<" flitid: "<<temp.flitid;
	os << temp.data1 << "\t" << temp.data2 << "\t" 
		<< temp.data3 << "\t" << temp.data4;
	os<<endl;
	return os;
}

// overloading extraction operator for simulation header
inline ostream&
operator << ( ostream& os, const sim_hdr& temp ) {
	os<<"gtimestamp: "<<temp.gtimestamp<<" gtime: "<<temp.gtime;
	os<<"\natimestamp: "<<temp.atimestamp<<" atime: "<<temp.atime<<endl;
	return os;
}

// overloading sc_trace for flit
inline void sc_trace( sc_trace_file*& tf, const flit& a, const std::string& name) {
	//sc_trace( tf, a.pkttype, name+".pkttype");
	sc_trace(tf, a.src, name+".src");
	sc_trace(tf, a.pktid, name+".pktid");
	sc_trace(tf, a.flitid, name+".flitid");
	//sc_trace( tf, a.freeBuf, name+".freeBuf");
}

#endif
