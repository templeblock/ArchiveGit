//////////////////////////////////////////////////////////////////////
// TutInfo.h: interface for the CTutInfo class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __TUTINFO_H__
#define __TUTINFO_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

struct TUTINFO
{
	CString csFilename;	// File name of DLL (*.tut)
	CString csInfo;		// Information string from DLL (id|caption|type|title)
};

class CTutInfo : public CObject  
{
   DECLARE_SERIAL(CTutInfo)

public:
	CTutInfo();
	virtual ~CTutInfo();

   BOOL BuildList();
   void Write(CStdioFile& ar);

   CPtrArray   m_TutList;  // array of TUTINFO structs
};

#endif // ifndef __TUTINFO_H__
