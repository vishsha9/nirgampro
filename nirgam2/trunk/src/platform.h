#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#ifdef WIN32
#define strcasecmp(a, b) _stricmp(a, b)
#else
#endif

#ifdef WIN32
#define stringPath(a) convertPathToWin32(a)
#else
#define stringPath(a) a
#endif

#endif 

