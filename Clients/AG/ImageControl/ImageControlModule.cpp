#include "stdafx.h"
#include "resource.h"

// The module attribute causes DllMain, DllRegisterServer and DllUnregisterServer to be automatically implemented for you
[
	module(dll,
		uuid = "{093C0E16-C4B2-40CF-B29D-6B4A66557604}", 
		name = "ImageControlModule", 
		helpstring = "ImageControl 1.0 Type Library",
		resource_name = "IDR_IMAGECONTROL")
]
class CImageControlModule
{
// Override CAtlDllModuleT members
};
