#include "router_test.h"

router_test::router_test(vector<UI> * nb_id){
	this->nb_id = nb_id;
}

UI router_test::calc_next(UI ip_dir, ULL source_id, ULL dest_id){
	if(id == 0)
	{
		for (int i =0 ;i< nb_id->size(); i++)
			if((*nb_id)[i] == 1)
				return i;
	}
	else
		for (int i =0 ;i< nb_id->size(); i++)
			if((*nb_id)[i] == 2)
				return i;
}

void router_test::initialize(){

}

void router_test::setID(UI tileid){
	this->id  = tileid;
}