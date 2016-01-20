/*
// $Workfile: INSTICFG.H $
// $Revision: 1 $
// $Date: 3/03/99 6:06p $
//
// Copyright © 1995 MicroLogic Software, Inc.
// All rights reserved.
*/

/*
// Revision History:
//
// $Log: /PM8/App/INSTICFG.H $
// 
// 1     3/03/99 6:06p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:21:48   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:18   Fred
// Initial revision.
// 
//    Rev 1.0   07 Aug 1996 11:55:16   Fred
// Initial revision.
// 
//    Rev 1.1   15 Nov 1995 14:40:18   FRED
//  
// 
//    Rev 1.0   16 Oct 1995 15:06:10   FRED
// Initial revision.
*/

class InstallIdapiConfiguration;

#ifndef __INSTICFG_H__
#define __INSTICFG_H__

// Translated to C++ from L:\DATABASE\INSTIC1.PAS

class InstallIdapiConfiguration : public CObject
{
public:
	InstallIdapiConfiguration();
	virtual ~InstallIdapiConfiguration();

public:

	// 2's are markers.
	// 3's are everything else.

	enum tIRTypes
	{
		irTypeMarker2,
		irTypeAllOthers3
	};

	// 0's indicate the item is not displayed.
	// 1's indicate a displayable item.

	enum tIRSubTypes
	{
		irSubTypeHidden0,
		irSubTypeDisplay1
	};

	// 1's indicate end of record.
	// 4's indicate a second string argument.

	enum tIRArgType
	{
		irArgType1,
		irArgType4
	};

	class tIdapiRec : public CObject
	{
	public:
		tIdapiRec();
		virtual ~tIdapiRec();

	public:
		tIRTypes irType;
		tIRSubTypes irSubType;
		CString irpStr1;
		tIRArgType irArgType;
		CString irpStr2;

	public:
		BOOL IsRec(tIRTypes aType, tIRSubTypes aSubType, LPCSTR s);
	};

	class tIdapiRecArray : public CObArray
	{
	public:
		tIdapiRecArray();
		virtual ~tIdapiRecArray();

	public:
		void RemoveAll();
		tIdapiRec* GetAt(int nIndex) const
			{ return (tIdapiRec*)(CObArray::GetAt(nIndex)); }
		tIdapiRec*& ElementAt(int nIndex)
			{ return (tIdapiRec*&)(CObArray::ElementAt(nIndex)); }
		void RemoveAt(int nIndex, int nCount = 1);
		tIdapiRec* operator[](int nIndex) const
			{ return (tIdapiRec*)(CObArray::GetAt(nIndex)); }
		tIdapiRec*& operator[](int nIndex)
			{ return (tIdapiRec*&)(CObArray::ElementAt(nIndex)); }

	public:
		tIRTypes irType;
		tIRSubTypes irSubType;
		CString irpStr1;
		tIRArgType irArgType;
		CString irpStr2;

	public:
		BOOL IsRec(tIRTypes aType, tIRSubTypes aSubType, LPCSTR s);
	};

public:
	BOOL Hello(LPCSTR ApplicationVolume, LPSTR IdapiDirectory, LPSTR IdapiCfgFile, BOOL& MinimalCopy, LPSTR ErrorMessage);
	BOOL Goodbye(LPSTR ErrorMessage);

public:
	tIdapiRecArray* ReadIdapiCfg(LPCSTR cfgFileName);
	void WriteIdapiCfg(LPCSTR cfgFileName, tIdapiRecArray* iFile, BOOL CreateBackup);
	void PrepareForIdapi(void);
	void ConfigureIdapi(void);
	void ValidateKeys(BOOL& BothKeysChanged);
	tIdapiRec* LocateLastNetDirSection(tIdapiRecArray* iFile);
	void ValidateNetDir(tIdapiRec* ir, BOOL& Changes);
	CString FirstSpecFromKey(const CString& s);

public:
	tIdapiRecArray aList;
	CString KeyDllPath;
	CString KeyConfigFile01;
	BOOL DoMinimalCopy;
	CString DefaultVolume;
	BOOL CopyCfgFile;
	BOOL Initialized;
};

extern const LPCSTR IDAPIKEY_DLLPATH;
extern const LPCSTR IDAPIKEY_CONFIGFILE01;
extern const LPCSTR PXNETDIRECTORY;
extern const LPCSTR PXNETDIR;

extern InstallIdapiConfiguration ConfigureIdapi;

#endif
