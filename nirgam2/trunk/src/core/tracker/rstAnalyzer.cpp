
#include "rstAnalyzer.h"
#include "../../global.h"

resultAnalyzer::resultAnalyzer(){
	string outname = gc_resultHome + "trackerOut";
	trackerOut.open(outname.c_str(), istream::in);
	if(!trackerOut.is_open()){
		cout << "Can't open trackOut file" << endl;
	}

	outname = gc_resultHome + "flitSeqOut";
	flitSeqOut.open(outname.c_str(), istream::in);
	if(!flitSeqOut.is_open()){
		cout << "Can't open flitSeqOut file" << endl;
	}
}