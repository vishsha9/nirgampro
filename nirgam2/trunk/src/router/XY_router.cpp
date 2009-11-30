
/*
* XY_router.cpp
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
/// \file XY_router.cpp
/// \brief Implements XY routing algorithm
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "XY_router.h"
#include "../global.h"

////////////////////////////////////////////////
/// Method to set unique id
////////////////////////////////////////////////
void XY_router::setID(UI id_tile) {
	id = id_tile;
	initialize();
}

////////////////////////////////////////////////
/// Method that implements XY routing
/// inherited from base class router
/// Parameters:
/// - input direction from which flit entered the tile
/// - tileID of source tile
/// - tileID of destination tile
/// returns next hop direction
////////////////////////////////////////////////
UI XY_router::calc_next(UI ip_dir, ULL source_id, ULL dest_id) {
	int xco = id / 3;
	int yco = id % 3;
	int dest_xco = dest_id / 3;
	int dest_yco = dest_id % 3;
	int x_to = xco;
	int y_to = yco;
	if(dest_yco > yco)
		y_to++;
	else if(dest_yco < yco)
		y_to--;
	else if(dest_yco == yco) {
		if(dest_xco < xco)
			x_to--;
		else if(dest_xco > xco)
			x_to++;
		else if(dest_xco == xco)
			return portTable->size(); // self directory
	}
	UI rel;
	idToDir(x_to*3+y_to, &rel);
	return rel;
}

////////////////////////////////////////////////
/// Method containing any initializations
/// inherited from base class router
////////////////////////////////////////////////
// may be empty
// definition must be included even if empty, because this is a virtual function in base class
void XY_router::initialize() {

}

// for dynamic linking
//extern "C" {
//router *maker() {
//	return new XY_router;
//}
//}
