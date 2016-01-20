/****************************************************************************
;
;	PROGRAM:    WINTFC.C - Include file for the import file filters.
;
;
;	DATE:	   	3/3/1992	
;
;	USAGE:		Include this file in your application to call image library 
;              functions.   
;
;
;			Copyright 1992, Accusoft Corporation.
;
;****************************************************************************/


#if defined(__unix)
#define __stdcall
#define cdecl

#define IMGPORT	unsigned int					
#define FAR								
#define HWND  		int					
#define HPALETTE 	int					
#define PASCAL		__stdcall			
#define WINAPI		__stdcall		
#define DDB			int
#define LPBITMAPINFOHEADER  LPDIB_HEADER
#define BITMAPINFOHEADER  	 DIB_HEADER

#ifndef WORD
#define WORD		signed short		
#endif

#ifndef UWORD
#define UWORD		unsigned short		
#endif

#endif

#if defined(WIN32)

#define IMGPORT	unsigned int					
#define FAR								
#define HWND  		int					
#define HPALETTE 	int					
#define PASCAL		__stdcall			
#define WINAPI		__stdcall		
#define DDB			int
#define LPBITMAPINFOHEADER  LPDIB_HEADER
#define BITMAPINFOHEADER  	 DIB_HEADER

#ifndef WORD
#define WORD		signed short		
#endif

#ifndef UWORD
#define UWORD		unsigned short		
#endif

#endif 


#if defined(OS2)

#define IMGPORT		HPS					
#define FAR								
#define HWND  			int					
#define HPALETTE int					
#define PASCAL			__stdcall			
#define WINAPI			__stdcall		
#define DDB						int
#define LPBITMAPINFOHEADER  LPDIB_HEADER
#define BITMAPINFOHEADER  	 DIB_HEADER

#ifndef WORD
#define WORD		signed short		
#endif

#ifndef UWORD
#define UWORD		unsigned short		
#endif

#endif 


