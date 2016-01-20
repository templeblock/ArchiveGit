/*=======================================================================*\

	ROOT.H - Base class overloads new and delete to get around MFC set_new_handler

\*=======================================================================*/

/*=======================================================================*\

	(c) Copyright 1994 MICROGRAFX, Inc., All Rights Reserved.
	This material is confidential and a trade secret.
	Permission to use this work for any purpose must be obtained
	in writing from: MICROGRAFX, 1303 E Arapaho, Richardson, TX  75081

\*=======================================================================*/
#ifndef __root_h__
#define __root_h__

class CPPRoot
{
public:
	void *operator new(size_t size);
	void operator delete(void *ptr);
};

typedef CPPRoot FAR *LPCPPRoot;

#endif /* __root_h__ */
