#include "Winmisc.h"

/////////////////////////////////////////////////////////////////////////////
BOOL SetDefaultDir(const char *dirname)
{
	unsigned long size;
	char appname[256];
	char pathname[256];

	size = ::GetModuleFileName(NULL, appname, 256);
	while(size > 0){
		if(appname[size] == '\\'){
			appname[size+1] = '\0';
			break;
		}
		size--;
	}
	
	if(!size)
		return FALSE;

	strcpy(pathname, appname);
	strcat(pathname, dirname);

	if(::SetCurrentDirectory(pathname) )	return TRUE;

	BOOL exist = ::CreateDirectory(pathname, NULL);
	return ( ::SetCurrentDirectory(pathname)? TRUE : ::SetCurrentDirectory(appname) ) ;
}

/////////////////////////////////////////////////////////////////////////////
BOOL SetDirectory(const char *dirname)
{
	if(::SetCurrentDirectory(dirname) )	return TRUE;

	BOOL exist = ::CreateDirectory(dirname, NULL);
	return ( ::SetCurrentDirectory(dirname)? TRUE : FALSE);
}


//get the file under the current directory
BOOL GetDefaultFile(char *filename)
{
	char pathname[256];

	if(!::GetCurrentDirectory(256, pathname)) return FALSE;

	strcat(pathname, filename);
	strcpy(filename, pathname);
	return TRUE;
}
