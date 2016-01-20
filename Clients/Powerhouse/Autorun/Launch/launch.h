
#ifndef _LAUNCH_H_
#define _LAUNCH_H_

#include "proto.h"
#include "launchid.h"
#include "scene.h"

#ifdef MAIN
	#define DECLARE
#else
	#define DECLARE extern
#endif

class CLaunchApp : public CPHApp
{
public:

	// Constructor
	CLaunchApp();
	// Destructor
	~CLaunchApp();

	// Implementation
	// hooks for your initialization code
	BOOL InitInstance();
};

typedef CLaunchApp FAR *LPLAUNCHAPP;
DECLARE CLaunchApp App;

#endif // _LAUNCH_H_  
                          