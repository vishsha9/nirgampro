
/*
 * NoC.cpp
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
/// \file NoC.cpp
/// \brief Creates network topology
///
/// This file creates a 2-dimensional topology of network tiles and interconnects them.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "noc.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor to generate topology.
/// It creates 2-d toroidal or non- toroidal mesh of m x n network tiles depending on input from user.
/// Parameters:
/// - number of rows in topology
/// - number of columns in topology
/// .
/// The constructor implements the following:
/// - interconnect network tiles
/// - assign tile IDs
/// - connect clock signal to clock input port of tiles.
////////////////////////////////////////////////////////////////////////////////////////////////////
NoC::NoC(sc_module_name name, AdjList* a): sc_module(name) {

	/*//////////////////////////////////////////////////////////////////////////
	// create tiles
	nwtile = new NWTile*[3];

	nwtile[0]= new NWTile("tile0", 0, 2);
	nwtile[0]->clk(switch_cntrl);
	nwtile[1] = new NWTile("tile1", 1, 2);
	nwtile[1]->clk(switch_cntrl);
	nwtile[2] = new NWTile("tile2", 2, 2);
	nwtile[2]->clk(switch_cntrl);
	//////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////////////
	// connect tiles
	sigs = new wires*[3];
	sigs[0] = connect("wire0", 444, nwtile[0], nwtile[1]);
	sigs[1] = connect("wire1", 444, nwtile[1], nwtile[2]);
	sigs[2] = connect("wire2", 444
		, nwtile[2], nwtile[0]);
	//wires* w_0to1 = sigs[0] ;
	//w_0to1->clk(switch_cntrl);

	*//////////////////////////////////////////////////////////////////////////
	
	for (int i=0; i<a->nodeNum; i++){
		Node * node = a->nodes[i];
		char name[50];
		sprintf(name, "tile%d", node->nodeId);
#ifdef SL_TILE
		BaseNWTile * tile = new SlTile(name, node->nodeId, node->adjNum);
#else
		BaseNWTile * tile = new NWTile(name, node->nodeId, node->adjNum);
#endif
		tile->clk(switch_cntrl);
		nwtile.push_back(tile);
	}


	for (int i=0; i<a->edgeNum; i++)
	{
		Edge * edge = a->edges[i];
		char name[50];
		sprintf(name, "wire%d", edge->edgeId);
		sigs.push_back(connect(name, edge->wp, nwtile[edge->node1->nodeId], nwtile[edge->node2->nodeId]));
	}
	

	SC_THREAD(entry);	// Thread entry() sensitive to clock
	sensitive_pos << switch_cntrl;
}

NoC::~NoC(){
	
}

wireModule* NoC::connect(sc_module_name wire_name, baseWireModel * para, BaseNWTile* tile1, BaseNWTile * tile2){
	wireModule* w_1to2 = new wireModule(wire_name, para); 
	w_1to2->clk(switch_cntrl);
	w_1to2->setTileID(tile1->tileID, tile2->tileID);

	tile1->connect(tile2->tileID, 
					w_1to2->sig_from2,
					w_1to2->sig_to2,
					w_1to2->credit_from2,
					w_1to2->credit_to2);
	tile2->connect(tile1->tileID,
					w_1to2->sig_from1,
					w_1to2->sig_to1,
					w_1to2->credit_from1,
					w_1to2->credit_to1);
	//sc_signal<flit> aa;
	//tile1->connect(&aa);

	/*tile1->op_port[tile1->nb_initPtr](w_1to2->sig_from1);
	tile1->ip_port[tile1->nb_initPtr](w_1to2->sig_to1);
	for(int i=0; i<NUM_VCS; i++){
		tile1->credit_out[tile1->nb_initPtr][i](w_1to2->credit_from1[i]);
		tile1->credit_in[tile1->nb_initPtr][i](w_1to2->credit_to1[i]);
	}
	tile1->nb_id[tile1->nb_initPtr] = tile2->tileID;
	tile1->nb_initPtr++;

	tile2->op_port[tile2->nb_initPtr](w_1to2->sig_from2);
	tile2->ip_port[tile2->nb_initPtr](w_1to2->sig_to2);
	for(int i=0; i<NUM_VCS; i++){
		tile2->credit_out[tile2->nb_initPtr][i](w_1to2->credit_from2[i]);
		tile2->credit_in[tile2->nb_initPtr][i](w_1to2->credit_to2[i]);
	}
	tile2->nb_id[tile2->nb_initPtr] = tile1->tileID;
	tile2->nb_initPtr++;*/

	return w_1to2;
}


///////////////////////////////////////////////////////////
/// This thread keeps track of global simulation count.
/// It also closes logfiles upon completion of simulation.
////////////////////////////////////////////////////////////
void NoC::entry() {
	g_simCount = 0;
	while(true) {
		wait();
		g_simCount++;
		g_tracker->run();
		
	}//end while
}//end entry
