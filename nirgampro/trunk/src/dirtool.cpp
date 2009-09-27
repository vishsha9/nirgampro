#include "dirtool.h"

#include <iostream>
#include <sys/stat.h>
#include <dirent.h>

string rwPathInWin32Styl(string path){
	int pos;
	while ((pos = path.find_first_of('/')) != string::npos)
	{
		path.replace(pos, 1, "\\");
	}
	return path;
}


/*
change path to linux format, using '/' standing for directory.
add '/' in the end of path
a\b\c --> a/b/c/
*/
string rwDirInLinuxStyl(string path){
	if(path.size() == 0)
		return path;
	path = rwPathInLinuxStyl(path);
	if (path.at(path.size()-1) == '/' || path.at(path.size()-1) == '\\')
		return path;
	else
		return path + "/";
}


string rwPathInLinuxStyl(string path){
	int pos;
	while ((pos = path.find_first_of('\\')) != string::npos)
	{
		path.replace(pos, 1, "/");
	}
	return path;
}
/*
try to open path. if no such dir, make it
support path like a/b/c/, /a/b/c/, ./a/b/c/
make a, a/b, a/b/c separately
no error return true, error occured return false;
*/
bool tryDir(string path){
	path = rwDirInLinuxStyl(path);

	int lastend = 0;

	// 755 MODE drwxr-xr-x
	mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR |
		S_IRGRP | S_IXGRP |
		S_IROTH | S_IXOTH;
	
	while (lastend != string::npos)
	{
		lastend = path.find_first_of("/", path.find_first_not_of("./", lastend));
		if (lastend != string::npos){
			string pathseg = path.substr(0, lastend+1);
			if(opendir(pathseg.c_str()) == NULL){
				if(mkdir(pathseg.c_str(), mode) == 0)
					cout << "Auto make dir: " << pathseg << endl;
				else{
					cerr << "Can't not mkdir: " << pathseg << endl;
					return false;
				}
			}
			else;
				//cout << "dir exist: " << pathseg << endl;
		}
	}
	return true;
}

string getDirOfFile(string path)
{
	int index = path.find_last_of("/");
	string ret = path.substr(0, index+1);
	ret = rwDirInLinuxStyl(ret);
	return ret;
}

string getPathOffset(string home, string path){
	if (path.size() > 0){
		if ( path[0] == '/')
			return path;
		else
			return home + path;
	}
	else
		return path;
}