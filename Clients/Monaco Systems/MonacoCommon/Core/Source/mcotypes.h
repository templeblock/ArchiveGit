//jmm - rearranged for ifdef's
#ifndef MCO_TYPES
#define MCO_TYPES

typedef short int16;
typedef long int32;
typedef unsigned short u_int16;
typedef unsigned long  u_int32;

#if defined(_WIN32)
	// Put Windows-specific types here for portability
	#include <windows.h>
	#pragma warning (disable:4018)
	#pragma warning (disable:4101)
	#pragma warning (disable:4129)
	#pragma warning (disable:4244)
	#pragma warning (disable:4305) // truncation from 'const double' to 'float'
	typedef HANDLE Handle;
	typedef BOOLEAN Boolean;
	typedef RECT Rect;
	typedef	unsigned char Str63[64];
	typedef	unsigned char Str255[256];

	typedef struct Point {
		int h;
		int v;
	} Point;
	
	typedef struct FSSpec {
		char  name[256];			// pointer to name of the file 
		char  mode[8];				// open flag
	} FSSpec;

	typedef struct RGBColor {
		unsigned short red;			// magnitude of red component
		unsigned short green;		// magnitude of green component
		unsigned short blue;		// magnitude of blue component
	} RGBColor;

	typedef unsigned long Fixed;
	typedef unsigned long OSType;
	typedef unsigned long ScriptCode;

#else
	// Put Mac-specific types here for portability
	typedef struct {
		double 	top,bottom,left,right;
	} McoPatchLoc;
/*
	typedef struct {
		int16	num_patches;
		int16	num_duplicate;
		double	max_dif;
		McoPatchLoc	locs[300];
	} McoPatches;
*/
#endif

typedef Handle McoHandle;
typedef int16 McoBool;
#define McoTrue (1)
#define McoFalse (0)

#endif	//MCO_TYPES
