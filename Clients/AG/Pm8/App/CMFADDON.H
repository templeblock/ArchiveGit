/*
// CMFADDON.H
//
//		Copyright (C) 1993 MicroLogic Software, Inc.
//
//	C++ interface to MFADDON.DLL
//
// $Header: /PM8/App/CMFADDON.H 1     3/03/99 6:04p Gbeddow $
//
// $Log: /PM8/App/CMFADDON.H $
// 
// 1     3/03/99 6:04p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:19:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:33:04   Fred
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:26:20   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:33:20   FRED
// Initial revision.
// 
//    Rev 2.1   08 Feb 1995 13:39:10   JAY
// Reverted. New series.
// 
//    Rev 1.2   04 Aug 1994 08:30:18   JAY
// Large model and 32-bit changes
// 
//    Rev 1.1   30 Jan 1994 12:27:46   JAY
//  
// 
//    Rev 1.6   29 Jan 1994 11:43:38   FRED
// Fixed syntax error.
// 
// 
//    Rev 1.5   29 Jan 1994 11:38:46   FRED
// Added Status field and flags for always/never deleting OTL fonts.
// 
// 
//    Rev 1.4   08 Jan 1994 10:00:20   FRED
// Fixed comment.
// 
// 
//    Rev 1.3   06 Jan 1994 16:52:16   FRED
// Fixed documentation error.
// 
// 
//    Rev 1.2   05 Jan 1994 15:43:56   MACDUFF
// Fixed syntax of previous change.
// 
//    Rev 1.1   05 Jan 1994 10:12:08   FRED
// Replaced SearchForSource flag with a flags long word.
// Defined flags.
// Added comments for rAddonProcessFontsData structure.
// Added ApplicationDirectory field to rAddonProcessFontsData.
// 
// 
//    Rev 1.0   29 Dec 1993 18:01:48   MACDUFF
// Initial revision.
*/

#ifndef _CMFADDON_H_
#define _CMFADDON_H_

#ifndef STRICT
#ifndef NOSTRICT
#define STRICT
#endif
#endif

#include <windows.h>


typedef char tPathStr[129];
typedef char tDirStr[117];
typedef char tNameStr[9];
typedef char tExtStr[5];
typedef char tNameExtStr[13];

typedef enum
{
	AddonAskAlways = 0,
	AddonAskIfNotValid,
	AddonAskNever
} tAddonAskType;

typedef enum
{
	AddonTrueTypeConversion = 0,
	AddonPostScriptConversion
} tAddonConversionType;

typedef enum
{
	AddonStatusOk = 0,
	AddonStatusError,
	AddonStatusUserAbort
} tAddonStatusType;

typedef unsigned long tAddonFlags;

#define	AddonFlagSearchForSource			(1L << 0)
#define	AddonFlagInstallAll					(1L << 1)
#define	AddonFlagIncludeRegistration		(1L << 2)
#define	AddonFlagOnlyRegistration			(1L << 3)
#define	AddonFlagAlwaysDeleteOtlFonts		(1L << 4)
#define	AddonFlagNeverDeleteOtlFonts		(1L << 5)

/*
	This record contains the configuration data for the
	addon process:

		RecordLength					The length of the data record.

		AskForSource					If this field is Always, then the user is
											prompted for a source directory and the
											Source field is used for the default.
											If this field is Never, then the value of
											the Source field is used as the source
											for add-on fonts. If this field is set
											to IfNotValid then the value of Source will
											be used unless it is not a valid source in
											which case the user will be prompted for a
											new source directory with the value of source
											being used as the default.

		Flags								Flags affecting the add-on installation:

											AddonFlagSearchForSource

												If this flag is nonzero, then MFADDON will
												check likely places (the floppy drives) for
												add-on fonts. If fonts are found, then their
												location will be used for the default source
												directory.

											AddonFlagInstallAll

												Automatically install all the available
												add-on objects without allowing the
												user to individually select them.

											AddonFlagIncludeRegistration

												Allow the user to select add-on objects
												that belong to the group named
												"RegistrationBonusGroup".

											AddonFlagOnlyRegistration

												Allow the user to select only add-on
												objects that belong to the group named
												"RegistrationBonusGroup".

											AddonFlagAlwaysDeleteOtlFonts

												Normally, the OTL files (and only those files)
												that are created by the this object are
												deleted. This flag forces all converted OTL
												files to be deleted.

											AddonFlagNeverDeleteOtlFonts

												Normally, the OTL files (and only those files)
												that are created by the this object are
												deleted. This flag forces all converted OTL
												files to not be deleted.

		TrueTypeDll						Full path name of DLL to convert OTL fonts
											to TrueType. If this is an empty string, then
											TrueType conversion is not allowed.

		PostScriptDll					Full path name of DLL to convert OTL fonts
											to PostScript. If this is an empty string, then
											PostScript conversion is not allowed.

		SymbolOtl						Full path name of SYMBOL.OTL file to use.

		SymbolSet						Full path name of SYMBOL.SET file to use.

		TempDirectory					Directory path to use for creation of
											temporary files.

		ConversionType					The default conversion type. This must
											be either AddonTrueTypeConversion or
											AddonPostScriptConversion. The final value
											selected by the user will be returned in
											this field.

		Source							The source path for the add-on font files.
											On return, this field contains the directory
											chosen by the user. Typically, this value
											will be written out to a configuration file
											and saved as the default add-on source
											directory.

		ApplicationDirectory			The full path name of the application
											directory. Mandatory files are placed
											in this directory unless otherwise
											specified (e.g., the directory for
											SYMBOL.OTL is specified by the
											SymbolOtl field of this structure.)

		TTFDirectory					The default TTF file directory. The final
											value selected by the user will be returned
											in this field.

		PFBDirectory					The default PFB file directory. The final
											value selected by the user will be returned
											in this field.

		PFMDirectory					The default PFM file directory. The final
											value selected by the user will be returned
											in this field.

		TrueTypeFontsInstalled		The number of TrueType fonts successfully
											installed into Windows.

		PostScriptFontsInstalled	The number of PostScript fonts successfully
											installed into Windows. The user must typically
											restart Windows in order to make these fonts
											available in their applications.

		Status							A status describing the result of the installation.
*/

#pragma pack(1)
typedef struct
{
	// Fields set by caller, not changed by MFADDON

	WORD						RecordLength;
	tAddonAskType			AskForSource;
	tAddonFlags				Flags;
	tPathStr					TrueTypeDll;		// Set to empty if TrueType not allowed
	tPathStr					PostScriptDll;		// Set to empty if PostScript not allowed
	tPathStr					SymbolOtl;
	tPathStr					SymbolSet;
	tPathStr					TempDirectory;

	// Fields initialized by caller, possibly modified by MFADDON

	WORD						ConversionType;
	tPathStr					Source;
	tPathStr					ApplicationDirectory;
	tPathStr					TTFDirectory;
	tPathStr					PFBDirectory;
	tPathStr					PFMDirectory;

	// Fields returned by MFADDON

	WORD						TrueTypeFontsInstalled;
	WORD						PostScriptFontsInstalled;
	tAddonStatusType		Status;
} rAddonProcessFontsData;
#pragma pack()

typedef LPVOID FAR PASCAL MFADDON_INIT_FN();
typedef BOOL FAR PASCAL MFADDON_DONE_FN(LPVOID);
typedef BOOL FAR PASCAL MFADDON_PROCESSFONTS_FN(
	LPVOID lpData,
	HWND hWindow,
	rAddonProcessFontsData FAR *lpProcessFontsData);


//
// CMFAddonDll
//
//		Object to encapsulate invocation of MFADDON.DLL
//

class CMFAddonDll
{
	tPathStr m_szDllPath;
	HINSTANCE m_hLibrary;
	LPVOID m_lpContext;

	MFADDON_INIT_FN FAR *m_lpMFAddonInit;
	MFADDON_DONE_FN FAR *m_lpMFAddonDone;
	MFADDON_PROCESSFONTS_FN FAR *m_lpMFAddonProcessFonts;

public:

	CMFAddonDll(LPCSTR lpszDllPath);
	~CMFAddonDll();

	BOOL Valid() { return (m_lpContext != NULL); }

	BOOL ProcessFonts(
		HWND hWindow,
		rAddonProcessFontsData FAR *lpProcessFontsData);
};



#endif	// _CMFADDON_H_


