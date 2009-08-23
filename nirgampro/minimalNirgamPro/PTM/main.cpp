#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
// PTM interconnect
// For top global layer (Coupling lines above one metal ground),
//    the above Cground is not existed.
// The graphics blow present local and intermediate layers model 
//    (Coupling lines bewteen two metal ground planes)
// _______________________________________________
//
//                    <--w-->     dielectric c: K	
//        _______   _ _______     _______
//        |ooooo|   | |ooooo|     |ooooo|
//        |ooooo|   t |ooooo|--s--|ooooo|
//        |ooooo|   | |ooooo|     |ooooo|
//        -------   - -------     -------
//                       |
//                       h
// ______________________|________________________
//
// _______________________________________________
//                       | Cground
//                       = 	
//        ____Ccouple  __|___  Ccouple __
//        |ooo        |ooooo|         oo|
//        |ooooo|-||--|ooooo|--||-|ooooo|
//        |ooooo|     |ooooo|     |ooooo|
//        -------     -------     -------
//                       |
//                       = Cground
// ______________________|________________________
//
//
// Top Global Layer
//   R:
//   Ctotal = Cground + 2*Ccouple
//     Cground = 
//     Ccouple = 
//
// Local Layer
//   R:
//   Ctotal = Cground + 2*Ccouple
//     Cground = 
//     Ccouple = 
//
//////////////////////////////////////////////////////////////////////////


struct ptm_para{
	double w;
	double s;
	double length;
	double t;
	double h;
	double K;
	double rou;
};

//////////////////////////////////////////////////////////////////////////
//  Cg = 
//////////////////////////////////////////////////////////////////////////
double calc_Cg_topGlobalLayer(ptm_para p){
	double cg;
	double w = p.w, h = p.h, s = p.s, t = p.t, k = p.K, l = p.length;
	cg = k * ( w/h + 2.22 * pow( s/(s+0.7*h), 3.19) + 1.17 * pow( s/(s+1.51*h) , 0.76)  * pow( t/(t+4.53*h) , 0.12) );
	return cg*l;
}

double calc_Cc_topGlobalLayer(ptm_para p){
	double cc;
	double k = p.K, t = p.t, h = p.h, s = p.s, w = p.w,  l = p.length;
	cc = k * ( 1.14 * t/s * pow( h/(h+2.06*s), 0.09) + 0.7 * pow( w/(w+1.59*s), 1.14) + 1.16 * pow( w/(w+1.87*s), 0.16) * pow( h/(h+0.98*s), 1.18) );
	return cc*l;
}
double calc_R_topGlobalLayer(ptm_para p){
	double r;
	double rou = p.rou, l = p.length, w = p.w, t = p.t;
	r = rou*l/(w*t);
	return r;
}

double calc_Cg_localLayer(ptm_para p){
	double cg;
	double w = p.w, h = p.h, s = p.s, t = p.t, k = p.K, l = p.length;
	cg = k * ( w/h + 2.04 * pow( s/(s+0.54*h) , 1.77)  * pow( t/(t+4.53*h) , 0.07) );
	return cg*l;
}

double calc_Cc_localLayer(ptm_para p){
	double cc;
	double k = p.K, t = p.t, h = p.h, s = p.s, w = p.w, l = p.length;
	cc = k * ( 1.41 * t/s * 1/exp( 4*s/(s+8.01*h) ) + 2.37 * pow( w/(w+0.31*s), 0.28) * pow( h/(h+8.96*s), 0.76) * 1/exp(2*s/(s+6*h)) );
	return cc*l;
}

double calc_R_localLayer(ptm_para p){
	double r;
	double rou = p.rou, l = p.length, w = p.w, t = p.t;
	r = rou*l/(w*t);
	return r;
}

void main(){
	ptm_para p;
	
	p.w = 0.3;
	p.s = 0.1;
	p.length = 100;
	p.t = 0.2;
	p.h = 0.2;
	p.K = 2.2;
	p.rou = 2.2;

	printf("Local Cg, %f\n", calc_Cg_localLayer(p));
	printf("Local Cc, %f\n", calc_Cc_localLayer(p));
	printf("Local R, %f\n", calc_R_localLayer(p));
}
