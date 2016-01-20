/*
// $Workfile: MACROSTD.H $
// $Revision: 1 $
// $Date: 3/03/99 6:07p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/MACROSTD.H $
// 
// 1     3/03/99 6:07p Gbeddow
// 
// 4     3/18/98 11:14a Jayn
// Page numbers
// 
// 3     1/28/98 1:04p Johno
// 
// 2     1/27/98 11:54a Johno
// For a version that will compile
// 
//    Rev 1.0   14 Aug 1997 15:22:24   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:34   Fred
// Initial revision.
// 
//    Rev 1.2   04 Aug 1997 10:19:40   Fred
// Fixed crash from bad address book data.
// 
//    Rev 1.1   16 Jul 1996 08:50:56   Fred
// Hooked up address/sender book
// 
//    Rev 1.0   14 Mar 1996 13:38:28   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:34:46   FRED
// Initial revision.
// 
//    Rev 1.12   27 Aug 1995 12:34:24   FRED
// New fill-in-fields stuff
// 
//    Rev 1.11   11 Aug 1995 10:58:30   FRED
// Phil Frank page; macro size limits; move project album text
// 
//    Rev 1.10   07 Aug 1995 07:48:38   FRED
// Standard fill-in fields.
// 
//    Rev 1.9   06 Aug 1995 09:46:04   FRED
// More fill-in fields stuff.
// 
//    Rev 1.8   04 Aug 1995 10:47:04   FRED
// Sender fields.
// 
//    Rev 1.7   03 Aug 1995 14:33:12   FRED
//  
// 
//    Rev 1.6   03 Aug 1995 08:00:50   FRED
// Fill in stuff.
// 
//    Rev 1.5   02 Aug 1995 14:56:28   JAY
//  
// 
//    Rev 1.4   02 Aug 1995 12:58:14   FRED
// Some more fill-in fields stuff.
// 
//    Rev 1.3   02 Aug 1995 08:07:10   FRED
// Fill-in fields stuff.
// 
//    Rev 1.2   13 Jun 1995 17:30:38   JAY
// Caches field names to handle the common case.
// 
//    Rev 1.1   02 May 1995 15:39:32   JAY
// More smart fields stuff.
// 
//    Rev 1.0   01 May 1995 15:25:08   JAY
// Initial revision.
*/

class CStdMacroServer;

#ifndef __MACROSTD_H__
#define __MACROSTD_H__

#include "macrosrv.h"

class MacroListRecord;
class NameListRecord;
class CFlatFileDatabaseCursor;
class CMacroList;
class CMacro;
class CPmwDoc;

/*
// Standard base class which supports macros ("placeholders" / "smart fields" / "Fill-in Fields").
*/

class CStdMacroServer : public CMacroServer
{
public:

	CStdMacroServer();
	virtual ~CStdMacroServer();
 
	virtual CHARACTER* GetMacroText(CHARACTER c, DB_RECORD_NUMBER lPage = 0);
	virtual LPCSTR GetMacroFieldText(CHARACTER c);

	virtual CHARACTER* GetMacroText(CMacro* pMacro);
	virtual CString GetMacroTextString(CMacro* pMacro);

/*
// Are we showing values?
*/

	BOOL ShowMacroValues(void) const;
	void ShowMacroValues(BOOL fShowValues);

/*
// Macro list support.
*/

	void SetMacroList(MacroListRecord* pMacroList);
	virtual MacroListRecord* GetMacroList(void)
		{ return m_pMacroList; }

/* Macro access. */
	virtual CMacroList& MacroList(void);
	virtual int Macros(void);
	virtual CHARACTER GetMacroCharacter(int nMacro);
	void ModifiedMacros(void);

/*
// Name list support.
*/

	void SetNameList(NameListRecord* pNameList);
	NameListRecord* GetNameList(void) const
		{ return m_pNameList; }

	void AddFieldMacrosToList(void);
	void AddGlobalMacrosToList(void);

	void AfterAddressBookRun(CWnd* pParent, CPmwDoc* pDoc);

/*
// Database field support.
*/

	virtual int ReceiverFields(void);
	virtual const CString& ReceiverFieldName(int nField);
	virtual const CString& ReceiverFieldClass(int nField);
	virtual void ReceiverFieldValue(const CString& csFieldName, const CString& csFieldValue);
	virtual CString ReceiverFieldValue(const CString& csFieldValue);
	virtual CString ReceiverFieldValue(int nField);
	virtual void ReceiverFieldValue(int nField, const CString& csValue);
	virtual void ClearReceiverFieldValues(void);

	virtual int SenderFields(void);
	virtual const CString& SenderFieldName(int nField);
	virtual const CString& SenderFieldClass(int nField);
	virtual void SenderFieldValue(const CString& csFieldName, const CString& csFieldValue);
	virtual CString SenderFieldValue(const CString& csFieldValue);
	virtual CString SenderFieldValue(int nField);
	virtual void SenderFieldValue(int nField, const CString& csValue);
	virtual void ClearSenderFieldValues(void);

	virtual const CString& GlobalFieldName(int nField);
	virtual BOOL ReadGlobalFieldNames(LPCSTR pszFileName);
	
	virtual void ClearFieldValues(void);
	virtual void CompositeName(LPCSTR pFieldName, LPCSTR pClassName, CString& csName);
	
	virtual void DatabaseFieldNameToMacroName(LPCSTR pszField, char* pszMacro, int nBufferSize);
	virtual void MacroNameToDatabaseFieldName(LPCSTR pszMacro, char* pszField, int nBufferSize);

// Read and cache the generic receiver fields.

	BOOL ReadReceiverFieldNames(LPCSTR pszDatabaseName);
	
// Bind the receiver fields to a particular record.

	BOOL BindToRecord(CFlatFileDatabaseCursor* pCursor, DWORD dwKey);

// Read and cache the generic sender fields.

	BOOL ReadSenderFieldNames(LPCSTR pszDatabaseName);
	
// Bind the sender fields to the current sender.

	BOOL BindSender(void);

// Check if bound to a receiver.
	
	BOOL ReceiverIsBound(void)
		{ return m_fReceiverIsBound; }

// Check if bound to a sender.

	BOOL SenderIsBound(void)
		{ return m_fSenderIsBound; }
		
//
// Document access (needed for page macro resolution).
//
	void SetDocument(CPmwDoc* pDoc)
		{ m_pDoc = pDoc; }
	CPmwDoc* GetDocument(void) const
		{ return m_pDoc; }

protected:

/* The record containing the current existing macros. */
	MacroListRecord* m_pMacroList;

/* The record containing the current name list. */
	NameListRecord* m_pNameList;

/* An array of receiver database field names. These are indexed as the database is. */
	CStringArray m_csaReceiverFieldNames;

/* An array of sender database field names. These are indexed as the database is. */
	CStringArray m_csaSenderFieldNames;

/* A map for matching receiver field names with data from records. */
	CMapStringToString m_mapReceiverFieldValues;

/* A map for matching sender field names with data from sender record for receiver address book. */
	CMapStringToString m_mapSenderFieldValues;

/* Is there actually a receiver record bound to this? */

	BOOL m_fReceiverIsBound;

/* Is there actually a receiver record bound to this? */

	BOOL m_fSenderIsBound;
	
// Buffer used to return macro value.

	CHARACTER* m_pMacroText;
	int m_nMacroTextLength;
	CHARACTER* CharactersFromString(const CString& csValue);

/*
// Caching to help speed things up.
*/

	static CStringArray m_csaLastReceiverFieldNames;
	static CString m_csLastReceiverDatabaseName;
	static CString m_csReceiverFieldClass;
	static CStringArray m_csaLastSenderFieldNames;
	static CString m_csLastSenderDatabaseName;
	static CString m_csSenderFieldClass;
	static CStringArray m_csaGlobalFieldNames;
	static CString m_csGlobalFieldClass;

//
// The current document (may be NULL).
// Needed for page number resolution.
//
	CPmwDoc* m_pDoc;
};

#endif				// #ifndef __MACROSTD_H__
