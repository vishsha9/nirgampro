#include "notation.h"
const char * decemalNum="0123456789";
const char * decemalChar=".0123456789e+-";
const char * notationChar="TGMKmunpf";

DEC_NOTATION charToNotation(char c){
	switch (c)
	{
	case 'T':
		return T;
		break;
	case 'G':
		return G;
		break;
	case 'M':
		return M;
		break;
	case 'K':
		return K;
		break;
	case 'm':
		return m;
		break;
	case 'u':
		return u;
		break;
	case 'n':
		return n;
		break;
	case 'p':
		return p;
		break;
	case 'f':
		return f;
		break;
	default:
		cout << "WARNING: decimal notation error, no such prefix "<< c << endl;
		return NF;
		break;
	}
}

char notationToChar(DEC_NOTATION notation){
	switch (notation)
	{
	case T:
		return 'T';
	case G:
		return 'G';
	case M:
		return 'M';
	case K:
		return 'K';
	case m:
		return 'm';
	case u:
		return 'u';
	case n:
		return 'n';
	case p:
		return 'p';
	case f:
		return 'f';
	default:
		cout << "WARNING: decimal notation error, no such prefix "<< (int)n << endl;
		return '0';
	}
}

double getNotationValue(DEC_NOTATION notation){
	switch (notation)
	{
	case T:
		return 1e12;
	case G:
		return 1e9;
	case M:
		return 1e6;
	case K:
		return 1e3;
	case m:
		return 1e-3;
	case u:
		return 1e-6;
	case n:
		return 1e-9;
	case p:
		return 1e-12;
	case f:
		return 1e-15;
	default:
		cout << "WARNING: decimal notation error, no such prefix "<< (int)n << endl;
		return '0';
	}
}

double getCharValue(char c){
	DEC_NOTATION nota = charToNotation(c);
	return getNotationValue(nota);
}

double strToValue(string s){
	int loc = s.find_first_not_of(decemalChar);
	string subs = s.substr(0, loc);
	double ret = atof(subs.c_str());
	if(loc != string::npos){
		char nota = s.at(loc);
		double suffix = getCharValue(nota);
		ret = ret * suffix;
	}
	return ret;
}