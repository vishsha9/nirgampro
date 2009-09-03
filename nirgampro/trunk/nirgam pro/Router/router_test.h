#ifndef __ROUTER_TEST__
#define __ROUTER_TEST__

#include "../router.h"


//////////////////////////////////////////////////////////////////////
/// \brief Class to implement XY routing algorithm
///
/// inherited from from class router
//////////////////////////////////////////////////////////////////////
class router_test : public router {
public:
	/// Constructor
	router_test(vector<UI> * nb_id);

	vector<UI> * nb_id;

	/// returns next hop for a given source and destination. ip_dir is the incoming direction
	UI calc_next(UI ip_dir, ULL source_id, ULL dest_id);

	void initialize();	///< any initializations to be done
	void setID(UI tileid);	///< set unique id
};

#endif
