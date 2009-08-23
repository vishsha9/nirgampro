#include <iostream>

using namespace std;

enum DEC_NOTATION{
	T, G, M, K, m, u, n, p, f, NF
};

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

double notationToValue(DEC_NOTATION notation){
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

double charToValue(char c){
	DEC_NOTATION not = charToNotation(c);
	return notationToValue(not);
}

void main(){
	char c = 'p';
	cout << charToValue(c) << endl;
}