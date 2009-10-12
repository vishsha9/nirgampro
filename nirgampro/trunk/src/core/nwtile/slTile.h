/*
* NWTile.h
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


#ifndef __SOCLIB_TILE__
#define __SOCLIB_TILE__

#include "../BaseNWTile.h"
#include "../ipcore.h"

#include "InputChannel.h"
#include "OutputChannel.h"
#include "VCAllocator.h"
#include "Controller.h"
#include "InnerSigs.h"
#include "../../app_inc.h"
#include "../selfCheck.h"

//////////////////////////////////////////////////////////////////////////
#include "slIpmap.h"

#include "slInitiatorInterface.h"
#include "slTargetInterface.h"

#include "../../mutekh/.config.h"

#include "mapping_table.h"
#include "loader.h"
#include "int_tab.h"
#include "mips32.h"
#include "gdbserver.h"
#include "vci_xcache_wrapper.h"
#include "vci_ram.h"
#include "vci_locks.h"
#include "vci_icu.h"
#include "vci_multi_tty.h"
#include "vci_signals.h"

#include "defines.h"
//////////////////////////////////////////////////////////////////////////
void loadSocLibConfig();

struct SlTile : public BaseNWTile{
	SC_HAS_PROCESS(SlTile);
	SlTile(sc_module_name slTile, UI tileID, UI nb_num);

	InputChannel **Ichannel;	///< Input channels
	OutputChannel **Ochannel;
	VCAllocator vcAlloc;
	Controller	ctr;
	ipcore *ip;

	UI nb_num;
	UI io_num;

	soclib::caba::VciSignals<soclib::caba::vci_param> vci_signals;

	Sigs_IcOc** sigs_IcOc;
	Sigs_IcIp sigs_IcIp;
	Sigs_IcVca* sigs_IcVca;
	Sigs_IcCtl* sigs_IcCtl;
	Sigs_OcIp sigs_OcIp;
	sc_signal<creditLine>	creditIC_CS[NUM_VCS];

	bool isCoreIO(UI i);
	void innerConnect();
	void setID(UI id);
};

#endif