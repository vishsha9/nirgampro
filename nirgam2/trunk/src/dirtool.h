#ifndef __DIRTOOL_H__
#define __DIRTOOL_H__

#include <string>
using namespace std;


/*
change path to linux format, using '/' standing for directory.
add '/' in the end of path
a\b\c --> a/b/c/
*/
string rwDirInLinuxStyl(string path);

/*
change path to linux format, using '/' standing for directory.
a\b\c --> a/b/c
*/
string rwPathInLinuxStyl(string path);

/*
change path to win32 format, using '/' standing for directory.
a/b/c --> a\b\c
*/
string rwPathInWin32Styl(string path);

/*
try to open path. if no such dir, make it
support path like a/b/c/, /a/b/c/, ./a/b/c/
make a, a/b, a/b/c separately
no error return true, error occured return false;
*/
bool tryDir(string path);

/*
get directory path of a file, return linux style
a/b/c (file): ret a/b/
a/b/c/ (dir): ret a/b/c/
*/
string getDirOfFile(string path);

/*
argument:
home: current dir path.
path: path.
if path is a absolute path. return path with nothing to do.
if path is a relative path. return the absolute path of the path (home + path);
*/
string getPathOffset(string home, string path);

#endif