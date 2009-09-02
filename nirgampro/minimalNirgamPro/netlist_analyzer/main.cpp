#include "netlist_analyzer.h"

ofstream readlog;
ofstream writelog;
ofstream delaylogin;
ofstream delaylogout;


void main(){
	AdjList* a;
	a = analyze("a.nfn", cout);
	printf("aaa");
}