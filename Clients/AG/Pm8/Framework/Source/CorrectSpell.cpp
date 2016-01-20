//****************************************************************************
//
// File Name:		CorrectSpell.cpp
//
// Project:			Renaissance Framework Component
//
// Author:			Mike Heydlauf
//
// Description:	Spell checker interface to Inso libraries
//
// Portability:	Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
//  Client:			Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/CorrectSpell.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:16p                                                  $
// $Revision:: 1                                                              $
//
//*****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "CorrectSpell.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#include "FrameworkResourceIDs.h"
#include "StandaloneApplication.h"
#include "Configuration.h"

///	REVIEW MDH - This is a HACK, but it is the only way to get access to the given
///			code unless we modify the INSO code.
extern "C"
{
	#include "SLCnv.c"
}

char	szEnglishSpellName[]		= "iens9522.dat";
char	szEnglishHyphenName[]	= "henis321.dat";
char	szEnglishPersonalName[]	= "userdict";
RMBCString	dbPath;
RMBCString	dbSpellName;// STRING_NAME_CS_SPELLDB
RMBCString	dbHyphenName;// STRING_NAME_CS_HYPHENDB
RMBCString	dbPersonalName;// STRING_NAME_CS_PERSONALDB
RMBCString	rPDSaveErr; // string to hold shutdown error
RMBCString	rPDCloseErr; // string to hold shutdown error
RMBCString	rICCloseErr; // string to hold shutdown error

const long kCacheSize = 16384;
const char kSoftHyphen = '-';

//*****************************************************************************************************
// Function Name:	RCorrectSpell::RCorrectSpell
//
// Description:	Constructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RCorrectSpell::RCorrectSpell() 
	: m_fInitialized( FALSE ),
	  m_fHyphenation( FALSE ),
	  m_nAlternativeCount( 0 ),
	  m_fICDBOpen( FALSE ),
	  m_fPDDBOpen( FALSE ),
	  m_fSpellChecking( FALSE ),
	  m_fAttemptedToInit( FALSE ),
	  m_pPDBuffReturns( NULL ),
	  m_rCurrentWord( "" )
{
	memset( &m_ICBuff, '\0', sizeof( m_ICBuff) );
	memset( &m_PDBuff, '\0', sizeof( m_PDBuff) );
	//
	// Need to dynamically allocate memory for  the pReturns array member of m_PDBuff
	m_pPDBuffReturns =  new ICPD_RET[ICMAXALT];
	m_PDBuff.pReturns = &m_pPDBuffReturns;

	m_sTempPath[0] = '\0';
	m_eAltAvailable = EAlternatives(0);
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::~RCorrectSpell
//
// Description:	Destructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RCorrectSpell::~RCorrectSpell() 
{
	delete[] m_pPDBuffReturns;
	m_PDBuff.pReturns = NULL;
}

//************************************************************************************
//************************************************************************************
//************************************************************************************
//****************************                               *************************
//****************************    Initialization Functions   *************************
//****************************                               *************************
//************************************************************************************
//************************************************************************************
//************************************************************************************

//*****************************************************************************************************
// Function Name:	RCorrectSpell::Initialize
//
// Description:	Initialize CorrectSpell libraries
//
// Returns:			TRUE if successful, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RCorrectSpell::Initialize( BOOLEAN fInitICDatabase, BOOLEAN fInitPersonalDict )
{
	m_fAttemptedToInit = TRUE;
	TpsAssert( !m_fInitialized, "Initialized already called once..." );
	if ( fInitICDatabase )
	{
		if ( InitICDatabase() != OKRET )
		{
			ProcessICInitFailure();
			return FALSE;
		}
		m_fICDBOpen		= TRUE;
		m_fHyphenation	= static_cast<BOOLEAN>( (m_ICBuff.init_flags & ICHYPAVAIL)? TRUE : FALSE );
	}

	if ( fInitPersonalDict )
	{
		if ( InitPersonalDictionary() != OKRET )
		{
			ProcessPDInitFailure();
			return FALSE;
		}
	}

	//
	// Load shutdown error strings now because at the time I'd need them its too late
	// to load them from the string table (ie. ResourceManager is already gone).
	rPDSaveErr = GetResourceManager().GetResourceString( STRING_ERROR_CS_PDSAVEERR );
	rPDCloseErr = GetResourceManager().GetResourceString( STRING_ERROR_CS_PDCLOSEERR );
	rICCloseErr = GetResourceManager().GetResourceString( STRING_ERROR_CS_ICCLOSEERR );

	m_fInitialized = TRUE;
	return TRUE;
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::InitICDatabase
//
// Description:	Initialize CorrectSpell database
//
// Returns:			OKRET if successful, error value otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
int RCorrectSpell::InitICDatabase()
{
	//
	// Call calloc to clear and initialize m_ICBuff
	memset( &m_ICBuff, '\0', sizeof( m_ICBuff) );

	//	Get dialect string
	RMBCString	szDialect	= GetResourceManager( ).GetResourceString( STRING_CS_DIALECT_FLAG );
	uWORD			uwDialect	= SLDEFAULT;
	if ( !szDialect.IsEmpty( ) )
	{
		uwDialect	= atoi( szDialect );

#ifdef	TPSDEBUG
		//	Make sure only one dialect is selected...
		uWORD	uwTmpDialect	= uwDialect & 0x00F0;
		TpsAssert( ((uwTmpDialect==0x80)||(uwTmpDialect==0x40)||
						 (uwTmpDialect==0x20)||(uwTmpDialect==0x10)), "Not a valid dialect selected" );
#endif	//	TPSDEBUG
	}
	//
	// Set up ICBUFF struct
	m_ICBuff.task					= ICINITIALIZE;
	m_ICBuff.lang					= SLUNIVERSAL;
	m_ICBuff.lang_dialect		= uwDialect;
	m_ICBuff.lang_mode			= SLDEFAULT;

	// Get the application path, and add the spell check path to it
///	RMBCString path = ::GetApplication( ).GetApplicationPath( );
	// REVIEW: BKN, MH dbPath will go away??
///	path += dbPath;
	
	RMBCString path;

	// search for SpellCheck Database 
	if ( dbSpellName.IsEmpty( ) )
	{
		dbSpellName	= GetResourceManager( ).GetResourceString( STRING_NAME_CS_SPELLDB );
		if ( dbSpellName.IsEmpty( ) )
			dbSpellName = szEnglishSpellName;
	}
	if (!( ::LocateFile( dbSpellName, path )) )
		return ERRET;  // return error code

	//	Path and File name for the SpellCheck Database
	strncpy( (char*)m_ICBuff.mstr_path.path, path, SLPATHMAX );
	strncpy( m_ICBuff.mstr_fn, dbSpellName, SLPATHMAX );

	// search for Hyphenation Database 
	if ( dbHyphenName.IsEmpty( ) )
	{
		dbHyphenName	= GetResourceManager( ).GetResourceString( STRING_NAME_CS_HYPHENDB );
		if ( dbHyphenName.IsEmpty( ) )
			dbHyphenName = szEnglishHyphenName;
	}
	if (!( ::LocateFile( dbHyphenName, path )) )
		return ERRET;  // return error code

	//	Path and File name for the Hyphenation Database
	strncpy( (char*)m_ICBuff.hyp_path.path, path, SLPATHMAX );
	strncpy( m_ICBuff.hyp_fn, dbHyphenName, SLPATHMAX );

	//
	//	Force all strings to be null terminated (just in case...)
	m_ICBuff.mstr_path.path[ SLPATHMAX ]	= 0;
	m_ICBuff.hyp_path.path[ SLPATHMAX ]		= 0;
	m_ICBuff.mstr_fn[ SLPATHMAX ]				= 0;
	m_ICBuff.hyp_fn[ SLPATHMAX ]				= 0;

	m_ICBuff.hyp_flag				= TRUE;
	m_ICBuff.ret_every_step		= FALSE;//TRUE;
	m_ICBuff.all_steps			= TRUE;
	m_ICBuff.parse_flag			= TRUE;
	m_ICBuff.soft_hyp_char		= SL_SOFT_HYPHEN;
	m_ICBuff.cache_siz			= kCacheSize;
	//
	// Set the ICspl() parameters:

	//
	// Call the ICspl function
	return ICspl( (PUCHAR) NULLSTR, &m_ICBuff );

}
//*****************************************************************************************************
// Function Name:	RCorrectSpell::GetICBuff
//
// Description:	Convenient way to access appropriate pointer to ICBuff regardless of whether its
//						been initialized or not.  Usefull only for Personal Dict operations because I have
//						to pass in ICBUFF struct if its been initialized and NULL if it hasn't.  This method
//						saves the if( m_fICDBOpen ) ... all over the place
//
// Returns:			pointer to ICBUFF struct, NULL if IC database hasn't been opened
//
// Exceptions:		None
//
//*****************************************************************************************************
ICBUFF* RCorrectSpell::GetICBuff()
{
	if( m_fICDBOpen )
	{
		return &m_ICBuff;
	}
	return NULL;

}
//*****************************************************************************************************
// Function Name:	RCorrectSpell::InitPersonalDictionary
//
// Description:	Initialize Personal Dictionary
//
// Returns:			OKRET if successful, error value otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
int RCorrectSpell::InitPersonalDictionary()
{
	int	nRetVal = OKRET;

	if ( !m_fPDDBOpen )
	{
		nRetVal = OpenPersonalDictionary( );
	}

	return nRetVal;
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::OpenPersonalDictionary
//
// Description:	Open Personal Dictionary
//
// Returns:			OKRET if successful, error value otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
int RCorrectSpell::OpenPersonalDictionary()
{
	int nRetVal = OKRET;

	if ( !m_fPDDBOpen )
	{
		//
		// Call calloc to clear and initialize m_ICBuff
		memset( &m_PDBuff, '\0', sizeof( m_PDBuff) );

		//
		// Set the ICpd() parameters:
		//
		// Set up PD_IC_IO struct
		m_PDBuff.task					= PDIC_INIT_PD;
		nRetVal = ICpd( (PFICBUFF)NULL, &m_PDBuff );

		if ( nRetVal != OKRET )
			return FALSE;	//	Not opened successfully

		//
		// Try to open existing DB if it exists...
		m_PDBuff.task						=	PDIC_START_DICT;

		// Get the application path, and add the spell check path to it
		//		RMBCString path = ::GetApplication( ).GetApplicationPath( );
		//		path += dbPath;

		// search for existing personal dictionary
		RMBCString path;
		if ( dbPersonalName.IsEmpty( ) )
		{
			dbPersonalName	= GetResourceManager( ).GetResourceString( STRING_NAME_CS_PERSONALDB );
			if ( dbPersonalName.IsEmpty( ) )
				dbPersonalName = szEnglishPersonalName;
		}
		::LocateFile(dbPersonalName, path);

		strncpy( (char*)m_PDBuff.szOpenFilename, (LPCSZ)dbPersonalName, PD_MAXFILENAME );
		m_PDBuff.szOpenFilename[ PD_MAXFILENAME ] = 0;

		strncpy( (char*)m_PDBuff.OpenPath.path, (LPCSZ)path, SLPATHMAX );
		m_PDBuff.OpenPath.path[ SLPATHMAX ] = 0;

		m_PDBuff.byOtype					=	PD_RAW_FILE;
		//
		// REVIEW -MDH (MWH) -what should I do about the following win specific code?
		#ifdef _WINDOWS
		::GetTempPath( MAX_PATH, m_sTempPath );
		#else
		strcpy( m_sTempPath, ".\\" );
		#endif
		strncpy( (char*)m_PDBuff.tmpPath.path, m_sTempPath, SLPATHMAX );
		m_PDBuff.tmpPath.path[ SLPATHMAX ] = 0;

		m_PDBuff.wDictSize				=	PD_AUTOEXPAND_INCREMENT_SIZE;
		m_PDBuff.byDictlang				=	SLUNIVERSAL;
		m_PDBuff.bySoftHypChar			=	kSoftHyphen;

		// REVIEW TPS - we are getting the correct path from the registry, but
		// the userdict file is NOT being written there. userdict does exist at
		// the path indicated in the registry, so it should not create userdict
		// in the application directory. but it does. This call to ICpd appears
		// to succeed, not sure why does not work correctly. PressWriter contains
		// code that correctly sets the path for userdict. Recommend looking at 
		// it for a working example

		nRetVal = ICpd( GetICBuff(), &m_PDBuff );
		if ( nRetVal != OKRET )
		{
			//
			// No DB previously exists, so create one...
			m_PDBuff.byOtype				=	PD_NEW_FILE;
			nRetVal = ICpd( GetICBuff(), &m_PDBuff );
		}

		//	If all is OK, We have opened the Personal DB File
		if ( nRetVal == OKRET )
		{
			m_fPDDBOpen	= TRUE;
		}
	}

	return nRetVal;
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::ProcessICInitFailure
//
// Description:	Handle initialization errors
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RCorrectSpell::ProcessICInitFailure()
{
 	RAlert rAlert;
	switch( m_ICBuff.init_flags )
	{
	case ICALLOCERR:
		rAlert.AlertUser( STRING_ERROR_CS_ICALLOCERR );
		break;

	case ICHEADERR:
		rAlert.AlertUser( STRING_ERROR_CS_ICHEADERR );
		break;

	case ICOPENERR:
		rAlert.AlertUser( STRING_ERROR_CS_ICOPENERR );
		break;

	case ICREADERR:
		rAlert.AlertUser( STRING_ERROR_CS_ICREADERR );
		break;

	case ICSEEKERR:
		rAlert.AlertUser( STRING_ERROR_CS_ICSEEKERR );
		break;

	default:
		{
			TpsAssert( FALSE, "CorrectSpell: Unknown error condition" );
			rAlert.AlertUser( STRING_ERROR_CS_UNKNOWNERR );
		}
		break;

	}
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::ProcessICInitFailure
//
// Description:	Handle initialization errors
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RCorrectSpell::ProcessPDInitFailure()
{
 	RAlert rAlert;
	switch( m_PDBuff.xStatus.nRetCode )
	{
	case PD_SDINIT_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_PD_SDINIT_ERR );
		break;
	case SLALLOC_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_SLALLOC_ERR );
		break;
	case SLOPEN_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_SLOPEN_ERR );
		break;
	case SLREAD_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_SLREAD_ERR );
		break;
	case SLWRITE_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_SLWRITE_ERR );
		break;
	case PD_FULL_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_PD_FULL_ERR );
		break;
	case PD_MAXDID_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_PD_MAXDID_ERR );
		break;
	default:
		{
			TpsAssert( FALSE, "CorrectSpell: Unknown error condition" );
			rAlert.AlertUser( STRING_ERROR_CS_UNKNOWNERR );
		}
		break;
	}
}
//************************************************************************************
//************************************************************************************
//************************************************************************************
//****************************                               *************************
//****************************    Terminate    Functions     *************************
//****************************                               *************************
//************************************************************************************
//************************************************************************************
//************************************************************************************

//*****************************************************************************************************
// Function Name:	RCorrectSpell::Terminate
//
// Description:	Close CorrectSpell libraries
//
// Returns:			TRUE if successful, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RCorrectSpell::Terminate()
{
	if ( m_fICDBOpen )
	{
		if ( CloseICDatabase() != OKRET )
		{
			ProcessICCloseFailure();
			return FALSE;
		}
		m_fICDBOpen	= FALSE;
	}

	if ( m_fPDDBOpen )
	{
		if ( ClosePersonalDictionary() != OKRET )
		{
			ProcessPDCloseFailure();
			return FALSE;
		}
	}

	return TRUE;
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::CloseICDatabase
//
// Description:	Close CorrectSpell database
//
// Returns:			OKRET if successful, error value otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
int RCorrectSpell::CloseICDatabase()
{

	//
	// Set up ICBUFF struct
	m_ICBuff.task					= ICTERMINATE;

	//
	// Set the ICspl() parameters:

	//
	// Call the ICspl function
	return ICspl( (PUCHAR) NULLSTR, &m_ICBuff );

}
//*****************************************************************************************************
// Function Name:	RCorrectSpell::ClosePersonalDictionary
//
// Description:	Initialize Personal Dictionary
//
// Returns:			OKRET if successful, error value otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
int RCorrectSpell::ClosePersonalDictionary()
{
	int nRetVal = ERRET;

	if ( m_fPDDBOpen )
	{
		//
		// Need to save then close personal db then terminate..
		if ( SavePersonalDictionary() != OKRET ) return nRetVal;

		//
		// Set up PD_IC_IO struct for closing
		m_PDBuff.task					= PDIC_STOP_DICT;
		if ( ICpd( (PFICBUFF)NULL, &m_PDBuff ) != OKRET ) return nRetVal;

		//
		// Set up PD_IC_IO struct for termination
		m_PDBuff.task					= PDIC_TERM_PD;

		//
		// Call the ICpd function
		nRetVal = ICpd( (PFICBUFF)NULL, &m_PDBuff );

		//
		//	We are no longer open
		m_fPDDBOpen = FALSE;
	}
	return nRetVal;
}
//*****************************************************************************************************
// Function Name:	RCorrectSpell::SavePersonalDictionary
//
// Description:	Save personal dictionary file
//
// Returns:			True if successful, error otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
int RCorrectSpell::SavePersonalDictionary()
{
	//
	// Set up PD_IC_IO struct for saving
	m_PDBuff.task					= PDIC_SAVE;

	// Get the application path, and add the spell check path to it
	//RMBCString path = ::GetApplication( ).GetApplicationPath( );
	//path += dbPath;

	// search for existing personal dictionary
	RMBCString path;
	::LocateFile(dbPersonalName, path);

	strncpy( (char*)m_PDBuff.szCloseFilename, (LPCSZ)dbPersonalName, PD_MAXFILENAME );
	m_PDBuff.szCloseFilename[ PD_MAXFILENAME ] = 0;

	strncpy( (char*)m_PDBuff.ClosePath.path, (LPCSZ)path, SLPATHMAX );
	m_PDBuff.ClosePath.path[ SLPATHMAX ] = 0;

	m_PDBuff.byCtype = PD_RAW_FILE;

	int nRetVal = ICpd( &m_ICBuff, &m_PDBuff );
	if( nRetVal != OKRET )
	{
		RAlert rAlert;
		rAlert.InformUser( rPDSaveErr );//"CorrectSpell: Error saving personal dictionary." );
	}
	return nRetVal;
}


//*****************************************************************************************************
// Function Name:	RCorrectSpell::ProcessICCloseFailure
//
// Description:	Handle initialization errors
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RCorrectSpell::ProcessICCloseFailure()
{
	RAlert rAlert;
	rAlert.AlertUser( rICCloseErr );
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::ProcessPDCloseFailure
//
// Description:	Handle initialization errors
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RCorrectSpell::ProcessPDCloseFailure()
{
	RAlert rAlert;
	rAlert.AlertUser( rPDCloseErr );
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::IsInitialzied
//
// Description:	Return TRUE if the Correct Spell class has been initialized
//
// Returns:			m_fInitialized
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RCorrectSpell::IsInitialized( )
{
	if( !m_fAttemptedToInit )
	{
		Initialize();
	}
	return m_fInitialized;
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::IsHyphenation
//
// Description:	Return TRUE if the Correct Spell class has been initialized
//
// Returns:			m_fInitialized
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RCorrectSpell::IsHyphenation( ) const
{
	return m_fHyphenation;
}

//************************************************************************************
//************************************************************************************
//************************************************************************************
//****************************                               *************************
//****************************    Spell Check  Functions     *************************
//****************************                               *************************
//************************************************************************************
//************************************************************************************
//************************************************************************************

//*****************************************************************************************************
// Function Name:	RCorrectSpell::CheckSpelling
//
// Description:	Check spelling of given word
//
// Returns:			TRUE if word spelled correctly, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RCorrectSpell::CheckSpelling( const RMBCString& rWord )
{
	TpsAssert( m_fInitialized, "Correct Spell has not been initialized" );
	//
	// If empty word is passed in, just return;
	if( rWord.IsEmpty() ) return TRUE;

	m_rCurrentWord			= rWord;
	m_nAlternativeCount	= 0;
	m_eAltAvailable		= kAltAvail;

	BOOLEAN fWordOk = FALSE;

 	RAlert rAlert;
	//
	// Set ICspl() parameters... 
	//
	// Setup m_ICBuff for word verification...
	m_ICBuff.task = ICVERIFY;
	_TUCHAR* pFormattedWord = new _TUCHAR[rWord.GetStringLength() + 1];
	try
	{
		strcpy( (_TCHAR*)pFormattedWord, (LPCSZ)rWord );
		FormatWord( pFormattedWord, m_ICBuff.lang );
		switch( ICspl( pFormattedWord, &m_ICBuff ) ) 
		{
		case ICRETPREPROC:
			{
				fWordOk = TRUE;
			}
			break;
		case ICRETFOUND:
			{
				fWordOk = TRUE;
			}
			break;

		case ICRETNOTFOUND:
			{
				fWordOk = FALSE;
				GetAltSpellings(pFormattedWord);
				//
				// Check error flags to find out why word failed..
			}
			break;

		case ICRETALT:
			{
				fWordOk = FALSE;
			}
			break;

		case ICRETINVALID:
			{
				fWordOk = FALSE;
				GetAltSpellings(pFormattedWord);
				//
				// Display message to user describing error condition; 
				// then allow the user to enter a corrected word.
			}
			break;

		case ERRET:
			{
				fWordOk = FALSE;
				GetAltSpellings(pFormattedWord);
			}
			break;

		default:
			{
				TpsAssert( FALSE, "CorrectSpell: Unknown error condition" );
				fWordOk = FALSE;
				m_eAltAvailable = kNoAlternatives;
			}
			break;
		}

		m_fSpellChecking = TRUE;
	}
	catch( ... )
	{
		delete[] pFormattedWord;
		throw;
	}
	delete[] pFormattedWord;
	return fWordOk;
}
//*****************************************************************************************************
// Function Name:	static RCorrectSpell::FormatWord
//
// Description:	Format a given word for correct spell spell checking
//
// Returns:			TRUE if successfully converted, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
//static
BOOLEAN RCorrectSpell::FormatWord( _TUCHAR* pFormattedWord, _TUCHAR uLang )
{
	return static_cast<BOOLEAN>( SLfchcnv( pFormattedWord, static_cast<short>(strlen((char*)pFormattedWord)),pFormattedWord, 
   													NAT_TO_DEF, uLang ) == OKRET );
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::PrepareAltSpellings
//
// Description:	Get list of alternative spellings for given word
//
// Returns:			TRUE if another alternative exists, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
void RCorrectSpell::GetAltSpellings(_TUCHAR* pWord)
{
	TpsAssert( m_fInitialized, "Correct Spell has not been initialized" );

	m_ICBuff.task = ICCORRECT;
	switch( ICspl( pWord, &m_ICBuff ) )
	{
	case ICRETCONT:
		{
			m_eAltAvailable = kMoreAltAvail;
		}
		break;
	case ICRETDONE:
		{
			m_eAltAvailable = kAltAvail;
		}
		break;
	case ICRETFULL:
		{
			m_eAltAvailable = kAltAvail;
		}
		break;
	case ERRET:
		{
			m_eAltAvailable = kNoAlternatives;
		}
		break;
	default:
		{
			TpsAssert( FALSE, "Correct Spell: Unknown error" );
			m_eAltAvailable = kNoAlternatives;
		}
		break;

	}
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::GetNextSuggestedSpelling
//
// Description:	Get another alternative spelling to given word
//
// Returns:			TRUE if another alternative exists, FALSE otherwise
//
// Exceptions:		Memory
//
//*****************************************************************************************************
BOOLEAN	RCorrectSpell::GetNextSuggestedSpelling( RMBCString& rSuggestion )
{
	TpsAssert( m_fInitialized, "Correct Spell has not been initialized" );

	if ( m_eAltAvailable == kNoAlternatives )
		return FALSE;

	BOOLEAN fRetVal = FALSE;
	if ( m_nAlternativeCount < m_ICBuff.num_alt )
	{
		RMBCString rPreStrip		= RMBCString((char*)m_ICBuff.prestrip);
		RMBCString rWord			= RMBCString((char*)m_ICBuff.list + m_ICBuff.corptr[m_nAlternativeCount-1]);
		RMBCString rPostStrip	= RMBCString((char*)m_ICBuff.poststrip);
		//
		// Reassemble word with appropriate surrounding punctuation.
		//rSuggestion					+= rPreStrip += rWord += rPostStrip;
		rSuggestion					= rWord;
		m_nAlternativeCount++;
		fRetVal = TRUE;
	}
	else
	{
		if (	m_eAltAvailable == kMoreAltAvail )
		{
			//
			// Set ICspl() parameters... 
			//
			// Setup m_ICBuff for word verification...
			m_ICBuff.task = ICCORMORE;
			_TUCHAR* pFormattedWord = new _TUCHAR[m_rCurrentWord.GetStringLength() + 1];
			try
			{
				strcpy( (_TCHAR*)pFormattedWord, (LPCSZ)m_rCurrentWord );
				FormatWord( pFormattedWord, m_ICBuff.lang );
				ICspl( pFormattedWord, &m_ICBuff ); 
			}
			catch( ... )
			{
				delete[] pFormattedWord;
				throw;
			}
			delete[] pFormattedWord;
		}
		else
		{
			m_nAlternativeCount = 0;
			fRetVal = FALSE;
		}
		m_eAltAvailable = kAltAvail;
	}
	return fRetVal;
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::GetHyphenation
//
// Description:	Get the Hyphenation map of the given word.
//						This REQUIRES that the ulMAP is at least an array of 2 longs
//
// Returns:			TRUE if successful, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RCorrectSpell::GetHyphenation( const RMBCString& rWord, uLONG ulMap[] )
{
	//
	// Mike, I took out the hyphenation optimization to get Alternatives to work -MWH
	//	m_ICBuff.ret_every_step		= TRUE;
	TpsAssert( m_fInitialized, "Correct Spell has not been initialized" );

	unsigned char strStdLib[128];

	m_fSpellChecking = FALSE;

	// put into standard form using ICS conversion
	uWORD		wordLen	= static_cast<uWORD>( rWord.GetStringLength( ) );
	//RETCODE rcode = SLfchcnv((unsigned char*)rWord, wordLen, (unsigned char*)strStdLib, NAT_TO_DEF, m_ICBuff.lang);
	strncpy( (char*)strStdLib, (LPCSZ)rWord, sizeof(strStdLib) );

  	m_ICBuff.task = ICHYPHENATE;
	ICspl(strStdLib, &m_ICBuff);

	if (m_ICBuff.retcode == ICRETFOUND)
	{
		// return bitmap encoded hyphenation
		ulMap[0] = m_ICBuff.hypmap[0];
		ulMap[1] = m_ICBuff.hypmap[1];
		return TRUE;
	}
	return FALSE;
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::AddWordToPersonalDict
//
// Description:	Adds given word to user's personal dictionary
//
// Returns:			TRUE if successful, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
void RCorrectSpell::AddWordToPersonalDict( const RMBCString& rWord )
{
	TpsAssert( m_fPDDBOpen, "Call to AddWordToPersonalDict without first opening Personal DB" );
	TpsAssert( !rWord.IsEmpty(), "Call to AddWordToPersonalDict with empty word" );

	_TUCHAR* pFormattedWord = new _TUCHAR[rWord.GetStringLength() + 1];
	try
	{
		strcpy( (_TCHAR*)pFormattedWord, (LPCSZ)rWord );
		FormatWord( pFormattedWord, m_ICBuff.lang );
		m_PDBuff.task					= PDIC_ADD;
		m_PDBuff.bNegWordFlag		= FALSE;

		//
		// When hyphenated words are looked up, the hyphen is stripped and
		// each individual word is verifyed, therefore we must strip and
		// write each word into the personal dict seperately.
		char* pWordToken = NULL;
		pWordToken = strtok( (_TCHAR*)pFormattedWord, "-" );
		int nAddResult = OKRET;
		if( pWordToken )
		{
			while( pWordToken && nAddResult == OKRET )
			{
				strncpy( (char*)m_PDBuff.szWord, (LPCSZ)pWordToken, PDWRDMAX );
				m_PDBuff.szWord[PDWRDMAX]	= 0;
				if( !CheckSpelling( RMBCString( pWordToken ) ) )
				{
					nAddResult = ICpd( GetICBuff(), &m_PDBuff ); 
				}
				pWordToken = strtok( NULL, "-" );
			}

		}
		else
		{
			strncpy( (char*)m_PDBuff.szWord, (LPCSZ)pFormattedWord, PDWRDMAX );
			m_PDBuff.szWord[PDWRDMAX]	= 0;
			nAddResult = ICpd( GetICBuff(), &m_PDBuff );
		}

		ShowAddToDictResults( nAddResult );
		SavePersonalDictionary();
	}
	catch( ... )
	{
		delete[] pFormattedWord;
		throw;
	}
	delete[] pFormattedWord;
}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::ShowAddToDictResults
//
// Description:	Shows user results of attempting to add to their personal dictionary
//
// Returns:			VOID
//
// Exceptions:		None
//
//*****************************************************************************************************
void RCorrectSpell::ShowAddToDictResults( int nResult )
{
	RAlert rAlert;
	switch( m_PDBuff.xStatus.nRetCode )
	{
	case PD_AUTOEXP_WRN:
		rAlert.AlertUser( STRING_ERROR_CS_PD_AUTOEXP_WRN );
		break;

	case PD_AUTODEL_WRN:
		rAlert.AlertUser( STRING_ERROR_CS_PD_AUTODEL_WRN );
		break;

	case PD_FULL_WRN:
		rAlert.AlertUser( STRING_ERROR_CS_PD_FULL_WRN );
		break;

	case PD_NOALT_WRN:
		rAlert.AlertUser( STRING_ERROR_CS_PD_NOALT_WRN );
		break;

	case PD_DUP_WRN:
		rAlert.AlertUser( STRING_ERROR_CS_PD_DUP_WRN );
		break;

	case SLALLOC_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_SLALLOC_ERR );
		break;

	case SLLOCK_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_SLLOCK_ERR );
		break;

	case SLTMPFNAME_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_SLTMPFNAME_ERR );
		break;

	case PD_FULL_ERR:
		rAlert.AlertUser( STRING_ERROR_CS_PD_FULL_ERR );
		break;

	default:
		{
			if( nResult == OKRET )
			{
				rAlert.InformUser( STRING_ERROR_CS_PD_OKRET );
			}
			else
			{
				TpsAssert( FALSE, "CorrectSpell: Unknown error encountered while trying to add to dictionary." );
				rAlert.AlertUser( STRING_ERROR_CS_PD_ERRET );
			}
		}
		break;

	}

}

//*****************************************************************************************************
// Function Name:	RCorrectSpell::LocateFile
//
// Description:	Helper function searches for a file in a set of directories
//						The paths in order of searching for the file are:
//						The application directory
//						The  Print Shop Family shared content directory
//
// Returns:			TRUE if the file is located
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN LocateFile( RMBCString& rFile, RMBCString& rPath )
{
	rPath.Empty( );
	RMBCString rFullPath;

	// Get the application path, and add the spell check path to it
	RMBCString path = ::GetApplication( ).GetApplicationPath( );
	rFullPath = path;
	rFullPath += rFile;

#ifdef _WINDOWS
	WIN32_FIND_DATA findFileData;
	HANDLE findFileHandle = ::FindFirstFile( (const char*)rFullPath, &findFileData );
	if (findFileHandle == INVALID_HANDLE_VALUE)
	{
		// Get registry entry for path to dictionaries
		RMBCString szResultsKey;
		RRenaissanceConfiguration config;
		if( config.GetStringValue( RRenaissanceConfiguration::kDictionariesLocation, szResultsKey ) )
		{
			// complete the path to dictionaries
			path = szResultsKey + "\\";
			rFullPath = path;
			rFullPath += rFile;
			findFileHandle = ::FindFirstFile( (const char*)rFullPath, &findFileData );
			rPath = path;
			if (findFileHandle != INVALID_HANDLE_VALUE)
			{
				rPath = path;
				::FindClose(findFileHandle);
				return TRUE;
			}
			else
				return FALSE;
		}

#ifdef OLDSTUFFTHATISWRONG
		RMBCString rDictionariesLocation;
		RToolConfiguration rConfig;
		if (rConfig.GetStringValue(RToolConfiguration::kDictionariesLocation, rDictionariesLocation))
		{
			rDictionariesLocation += "\\";
			rFullPath = rDictionariesLocation;
			rFullPath += rFile;
			
			findFileHandle = ::FindFirstFile( (const char*)rFullPath, &findFileData );
			rPath = path;
			if (findFileHandle != INVALID_HANDLE_VALUE)
			{
				rPath = path;
				::FindClose(findFileHandle);
				return TRUE;
			}
			else
				return FALSE;
		}
#endif

	}
	else
		::FindClose(findFileHandle);
#endif

	// rPath becomes the 'TRUE' path
	rPath = path;

	return TRUE;
}

