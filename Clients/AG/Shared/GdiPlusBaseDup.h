// Create by Jim McCurdy
// Don't include this file directly; include Gdip.h
// This is a clone of <GdiPlusBase.h> except:
// o There is a conflict between the GdiplusBase new and delete functions and atldbgmem.h
// o Provide GDI+ with overloads for new and delete operators that accepts and discards the additional parameters

/**************************************************************************\
*
* Copyright (c) 1998-2001, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusBase.h
*
* Abstract:
*
*   GDI+ base memory allocation class
*
\**************************************************************************/

#ifndef _GDIPLUSBASE_H
#define _GDIPLUSBASE_H

class GdiplusBase
{
public:
	// The following code does not like the 'new' debug macro, so save it undefine it, and restore it
	#pragma push_macro("new")
	#undef new

    void (operator delete)(void* in_pVoid)
    {
       DllExports::GdipFree(in_pVoid);
    }
    void* (operator new)(size_t in_size)
    {
       return DllExports::GdipAlloc(in_size);
    }
    void (operator delete[])(void* in_pVoid)
    {
       DllExports::GdipFree(in_pVoid);
    }
    void* (operator new[])(size_t in_size)
    {
       return DllExports::GdipAlloc(in_size);
    }
	void* (operator new)(size_t nSize, LPCSTR lpszFileName, int nLine)
	{
		return DllExports::GdipAlloc(nSize);
	}
	void operator delete(void* p, LPCSTR lpszFileName, int nLine)
	{
		DllExports::GdipFree(p);
	}

	#pragma pop_macro("new")
};

#endif 
