#ifndef __NOTATION_H__
#define  __NOTATION_H__

#include <iostream>
#include <string>

using namespace std;

enum DEC_NOTATION{
	T, G, M, K, m, u, n, p, f, NF
};

DEC_NOTATION charToNotation(char c);
char notationToChar(DEC_NOTATION notation);

double getNotationValue(DEC_NOTATION notation);
double getCharValue(char c);

double strToValue(string s);

#endif