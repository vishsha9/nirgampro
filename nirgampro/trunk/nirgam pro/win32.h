#ifndef _win32_h
#define _win32_h

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include <iostream>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

using namespace std;

struct timezone 
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};


extern int gettimeofday(struct timeval *tv, struct timezone *tz);
extern LPCWSTR stringToLPCWSTR(string orig);

#endif /* _win32_h */
