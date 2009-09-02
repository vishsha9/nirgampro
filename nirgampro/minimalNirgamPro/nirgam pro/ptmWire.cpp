#include "ptmWire.h"


//////////////////////////////////////////////////////////////////////////
// ptmPara
//////////////////////////////////////////////////////////////////////////
ptm_para* ptm_para::clone(){
	return new ptm_para(*this);
}

bool ptm_para::setFieldByName(string name, double val){
	if (strcasecmp(name.c_str(), "width") == 0){
		w = val;
		return true;
	}
	else if( strcasecmp(name.c_str(), "space") == 0){
		s = val;
		return true;
	}
	else if( strcasecmp(name.c_str(), "thick") == 0){
		t = val;
		return true;
	}
	else if( strcasecmp(name.c_str(), "height") == 0){
		h = val;
		return true;
	}
	else if( strcasecmp(name.c_str(), "K") == 0){
		K = val;
		return true;
	}
	else if( strcasecmp(name.c_str(), "rou") == 0){
		rou = val;
		return true;
	}
	else if( strcasecmp(name.c_str(), "length") == 0){
		length = val;
		return true;
	}
	else if( strcasecmp(name.c_str(), "layer") == 0){
		layer = (PTM_LAYER)(int)val;
		return true;
	}
	return false;
}

wire* ptm_para::getWire(string wirename){
	ptmWire * wire;
	if(layer == PTM_TOP)
		wire = new ptmTopWire(wirename.c_str(), this);
	else
		wire = new ptmLocalWire(wirename.c_str(), this);
	return wire;
}

//////////////////////////////////////////////////////////////////////////
// PTM wire model base
//////////////////////////////////////////////////////////////////////////
ptmWire::ptmWire(sc_module_name wires): wire(wires){

}

double ptmWire::getR(ptm_para *p){
	double r;
	double rou = p->rou, l = p->length, w = p->w, t = p->t;
	r = rou*l/(w*t);
	return r;
}

UI ptmWire::getDelayTime(){
	return 0.38*(Cc+Cg)*R/getCharValue('p');
}

//////////////////////////////////////////////////////////////////////////
// PTM Top Wire
//////////////////////////////////////////////////////////////////////////
ptmTopWire::ptmTopWire(sc_module_name wires, ptm_para* para): ptmWire(wires){
	this->para = para;
	this->Cc = getCc(para);
	this->Cg = getCg(para);
	this->R = getR(para);

	this->delayTime = getDelayTime();
}

double ptmTopWire::getCc(ptm_para* p){
	double cc;
	double k = p->K, t = p->t, h = p->h, s = p->s, w = p->w,  l = p->length;
	cc = k * ( 1.14 * t/s * pow( h/(h+2.06*s), 0.09) + 0.7 * pow( w/(w+1.59*s), 1.14) + 1.16 * pow( w/(w+1.87*s), 0.16) * pow( h/(h+0.98*s), 1.18) );
	return cc*l;
}

double ptmTopWire::getCg(ptm_para* p){
	double cg;
	double w = p->w, h = p->h, s = p->s, t = p->t, k = p->K, l = p->length;
	cg = k * ( w/h + 2.22 * pow( s/(s+0.7*h), 3.19) + 1.17 * pow( s/(s+1.51*h) , 0.76)  * pow( t/(t+4.53*h) , 0.12) );
	return cg*l;
}

//////////////////////////////////////////////////////////////////////////
// PTM local wire
//////////////////////////////////////////////////////////////////////////
ptmLocalWire::ptmLocalWire(sc_module_name wires, ptm_para* para):ptmWire(wires){
	this->para = para;
	this->Cc = getCc(para);
	this->Cg = getCg(para);
	this->R = getR(para);

	this->delayTime = getDelayTime();
}

double ptmLocalWire::getCc(ptm_para *p){
	double cc;
	double k = p->K, t = p->t, h = p->h, s = p->s, w = p->w, l = p->length;
	cc = k * ( 1.41 * t/s * 1/exp( 4*s/(s+8.01*h) ) + 2.37 * pow( w/(w+0.31*s), 0.28) * pow( h/(h+8.96*s), 0.76) * 1/exp(2*s/(s+6*h)) );
	return cc*l;
}

double ptmLocalWire::getCg(ptm_para *p){
	double cg;
	double w = p->w, h = p->h, s = p->s, t = p->t, k = p->K, l = p->length;
	cg = k * ( w/h + 2.04 * pow( s/(s+0.54*h) , 1.77)  * pow( t/(t+4.53*h) , 0.07) );
	return cg*l;
}
