#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
//	|t|    K absolute dielectric constant
//         |
//	 /^^^^^^^^\  	-
//	/ /^^^^^^\ \ 	|
//	| | rou  | | 	|
//	\ \______/ / 	|
//	|\________/| 	|
//	|          | 	L
//	|          | 	|
//	|          | 	|
//	\          / 	|
//	 \________/  	_
//
//  |--- D ----|
//
//////////////////////////////////////////////////////////////////////////
#define PI 3.1415926

struct TSV_para{
	double D;
	double t;
	double L;
	double rou;
	double K;
};

double calc_Cg_tsv(TSV_para p){
	double D = p.D, t = p.t, L = p.L, K = p.K;
	return 2 * PI * K * L / log(D/(D-2*t));
}

double calc_R_tsv(TSV_para p){
	double rou = p.rou, D = p.D, L = p.L, t = p.t;
	return rou * 4 * L / ( PI * pow( D-2*t, 2) );
}

void main(){
	TSV_para p;
	p.D = 5000; 
	p.L = 2000; 
	p.t = 100;
	p.K = 3.8;

	printf("C: %f\n", calc_Cg_tsv(p));
	printf("%f", log((double)50/49));
}

