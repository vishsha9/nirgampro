#ifndef __SSSP_ROUTER__
#define __SSSP_ROUTER__

#include <vector>
#include <map>
#include <set>

#include "../extern.h"
#include "../core/router.h"
#include "../core/topoAnalyzer.h"

//////////////////////////////////////////////////////////////////////
/// \brief Class to implement XY routing algorithm
///
/// inherited from from class router
//////////////////////////////////////////////////////////////////////
class sssp_router : public router {
public:
	/// Constructor
	sssp_router();

	/// returns next hop for a given source and destination. ip_dir is the incoming direction
	UI calc_next(UI ip_dir, ULL source_id, ULL dest_id);

	void initialize();	///< any initializations to be done
	void setID(UI tileid);	///< set unique id

protected:
	map<int, int> * opTable;
};

#endif
