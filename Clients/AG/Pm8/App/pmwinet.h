/*
	JRO
	This will conditionally include afxinet.h. 
	A pragma in that file will cause the wininet static library 
	to be linked in. This, in turn, will cause the wininet DLL(s)
	to be required.
*/
#ifndef	ARTSTORE
	#include <afxinet.h>
#else
	#pragma message("NOT including afxinet.h\n")
#endif
