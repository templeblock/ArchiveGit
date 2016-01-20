//****************************************************************************
//
// File Name:		IntelliFinder.cpp
//
// Project:			Renaissance Framework Component
//
// Author:			Mike Heydlauf
//
// Description:	Thesaurus interface to Inso libraries
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
//  $Logfile:: /PM8/Framework/Source/IntelliFinder.cpp                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
//*****************************************************************************

#include	"FrameworkIncludes.h"

ASSERTNAME

#include "IntelliFinder.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#include "FrameworkResourceIDs.h"
#include "CorrectSpell.h"
#include "StandaloneApplication.h"

//*****************************************************************************************************
// Function Name:	RIntelliFinder::RIntelliFinder
//
// Description:	Constructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RIntelliFinder::RIntelliFinder() 
	: m_rLookupWord( "" ),
	  m_fInitialized( FALSE ),
	  m_fLookupOk( FALSE ),
	  m_nNumSyns( 0 ),
	  m_nCurrentSyn( 0 ),
	  m_nNumDefs( 0 ),
	  m_nCurrentDef( 0 ),
	  m_pAppIO( NULL ),
	  m_pDbIO( NULL ),
	  m_pEntListIO( NULL ),
	  m_pStrTabIO( NULL ),
	  m_pETParIO( NULL )
{

}

//*****************************************************************************************************
// Function Name:	RIntelliFinder::~RIntelliFinder
//
// Description:	Destructor
//
// Returns:			Nothing
//
// Exceptions:		None
//
//*****************************************************************************************************
RIntelliFinder::~RIntelliFinder() 
{
	Terminate();
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
// Function Name:	RIntelliFinder::Initialize
//
// Description:	Initialize IntelliFinder libraries
//
// Returns:			TRUE if successful, FALSE otherwise
//
// Exceptions:		Memory
//
//*****************************************************************************************************
BOOLEAN RIntelliFinder::Initialize( )
{
	//
	// If already initialized, return..
	if( m_fInitialized ) return TRUE;
	BOOLEAN fRetVal = FALSE;

	//
	// Allocate memory for the app structure
	if( REmemory( (void**)&m_pAppIO, NULLNP, RE_APP_ST, RE_START, RE_DEFAULT_SIZ) == OKRET )
	{
		if( REapp( m_pAppIO, RE_NULL_OP, RE_START ) == ERRET )
		{
			return FALSE;
		}
	}
	else
	{
		m_pAppIO = NULL;
		return FALSE;
	}

	//
	// Allocate memory for the db structure
	if( REmemory( (void**)&m_pDbIO, m_pAppIO, RE_DB_ST, RE_START, RE_DEFAULT_SIZ) == OKRET )
	{
		try
		{
			m_pDbIO->pFileSpec = new RE_FILENAMES;

			RMBCString rDBFile;
			rDBFile = ::GetResourceManager( ).GetResourceString( STRING_NAME_IF_THESAURUSDB );

			m_pDbIO->pFileSpec->pDBFile = new char[rDBFile.GetStringLength() + 1];
			strcpy( (char*)m_pDbIO->pFileSpec->pDBFile, (LPCSZ)rDBFile );

			m_pDbIO->pFileSpec->pPath	= new SLPATH;

			// Get the application path
			//RMBCString path = ::GetApplication( ).GetApplicationPath( );
		
			// search for Thesaurus Database
			// REVIEW: TPS please review this to see if setting fRetVal = FALSE
			// is the right thing to do here

			RMBCString path;
			if ( ::LocateFile( rDBFile, path ) )
			{
				strcpy( (char*)m_pDbIO->pFileSpec->pPath->path, (LPCSZ)path );

				//	Get dialect string
				RMBCString	szDialect	= GetResourceManager( ).GetResourceString( STRING_IF_DIALECT_FLAG );
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
				m_pDbIO->mDialect			= uwDialect; //Default dialect
				m_pDbIO->fnAbort			= NULL;		 //No abort proc
				
				if( REdb( m_pDbIO, RE_NULL_OP, RE_START ) == OKRET )
				{
					if( REmemory( (void**)&m_pEntListIO, m_pDbIO, RE_ENTLIST_ST, RE_START, RE_DEFAULT_SIZ) == OKRET &&
						 REmemory( (void**)&m_pETParIO,	 m_pDbIO, RE_ETPAR_ST,   RE_START, RE_DEFAULT_SIZ) == OKRET )
					{
						fRetVal = TRUE;
					}
				}
			}

			delete m_pDbIO->pFileSpec->pDBFile; m_pDbIO->pFileSpec->pDBFile = NULL;
			delete m_pDbIO->pFileSpec->pPath;	m_pDbIO->pFileSpec->pPath	 = NULL;
			delete m_pDbIO->pFileSpec;				m_pDbIO->pFileSpec			 = NULL;
		}
		catch( ... )
		{
			if( m_pDbIO->pFileSpec )
			{
				delete m_pDbIO->pFileSpec->pDBFile;
				delete m_pDbIO->pFileSpec->pPath;
				delete m_pDbIO->pFileSpec;
			}
			::ReportException( kMemory );
			m_fInitialized	= FALSE;
			return FALSE;
		}
	}
	else
	{
		m_pDbIO = NULL;
	}

	//
	//REVIEW - MWH Too early in app(ie.app still initializing) to display error messages so just return FALSE..
	m_fInitialized = fRetVal;
	return fRetVal;
}

//*****************************************************************************************************
// Function Name:	RIntelliFinder::IsInitialzied
//
// Description:	Return TRUE if the IntelliFinder class has been initialized
//
// Returns:			m_fInitialized
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RIntelliFinder::IsInitialized( )
{
	return m_fInitialized;
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
// Function Name:	RIntelliFinder::Terminate
//
// Description:	Free memory for all Inso IO structs
//
// Returns:			void
//
// Exceptions:		None
//
//*****************************************************************************************************
void RIntelliFinder::Terminate()
{
	EndSession();
	//
	// Free all assigned memory...
	if( m_pETParIO )
	{
		//
		// Free allocated memory for etpar struct
		REmemory( (void**)&m_pETParIO, m_pDbIO, RE_ETPAR_ST, RE_STOP, NULLS );
	}
	if( m_pEntListIO )
	{
		//
		// Free allocated memory for entry list
		REmemory( (void**)&m_pEntListIO, m_pDbIO, RE_ENTLIST_ST, RE_STOP, NULLS );
	}
	if( m_pDbIO )
	{		
		//
		// Free allocated memory for db
		if ( m_pDbIO->pFileSpec )
		{
			delete m_pDbIO->pFileSpec->pPath;		m_pDbIO->pFileSpec->pPath		= NULL;
			delete[] m_pDbIO->pFileSpec->pDBFile;	m_pDbIO->pFileSpec->pDBFile	= NULL;
			delete m_pDbIO->pFileSpec;					m_pDbIO->pFileSpec				= NULL;
		}
		REmemory( (void**)&m_pDbIO, NULLNP, RE_DB_ST, RE_STOP, NULLS );
	}
	if( m_pAppIO )
	{
		//
		// Free allocated memory for app
		REmemory( (void**)&m_pAppIO, NULLNP, RE_APP_ST, RE_STOP, NULLS );
	}
	m_pETParIO		= NULL;
	m_pEntListIO	= NULL;
	m_pDbIO			= NULL;
	m_pAppIO			= NULL;

	m_fInitialized = FALSE;

}

//*****************************************************************************************************
// Function Name:	RIntelliFinder::EndSession
//
// Description:	Return TRUE if the IntelliFinder sessions have been successfully terminated
//
// Returns:			TRUE if successful, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RIntelliFinder::EndSession( )
{
	BOOLEAN fClosedDb  = FALSE;
	BOOLEAN fClosedApp = FALSE;

	if( REdb( m_pDbIO, RE_NULL_OP, RE_STOP ) == OKRET )
	{
		fClosedDb = TRUE;
	}

	if( REapp( m_pAppIO, RE_NULL_OP, RE_STOP ) == OKRET )
	{
		fClosedApp = TRUE;
	}
	return static_cast<BOOLEAN>(fClosedDb && fClosedApp);
}




//************************************************************************************
//************************************************************************************
//************************************************************************************
//****************************                               *************************
//****************************    Thesaurus    Functions     *************************
//****************************                               *************************
//************************************************************************************
//************************************************************************************
//************************************************************************************

//*****************************************************************************************************
// Function Name:	RIntelliFinder::GetNextSynonym
//
// Description:	Get next synonym for rWord and stick it in rSynonym
//
// Returns:			TRUE if successful, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RIntelliFinder::GetNextSynonym( RMBCString& rSynonym )
{	
	TpsAssert( m_rLookupWord.GetStringLength(), "Called GetNextSynonym with empty word(call LookupWord first)" );
	if( !m_fLookupOk ) return FALSE;

	if( m_nCurrentSyn < (*m_pETParIO->pSynData->pMngArray)->nSyns )
	{
		//
		// Load next synonym...
		rSynonym = (LPSZ)(*(*m_pETParIO->pSynData->pMngArray)->pSynArray)->pPrimDispStr;
		(*m_pETParIO->pSynData->pMngArray)->pSynArray++;
		m_nCurrentSyn++;
		return TRUE;
	}
	//
	// Out of synonyms, reset..
	m_nCurrentSyn = 0;
	return FALSE;

}
//*****************************************************************************************************
// Function Name:	RIntelliFinder::LookupWord
//
// Description:	Loads list of synonyms and definitions (if they exist) for given word in member var.
//
// Returns:			TRUE if lists found, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RIntelliFinder::LookupWord( RMBCString& rWord )
{
	TpsAssert( rWord.GetStringLength(), "Called LookupWord with empty word" );
	TpsAssert( m_fInitialized, "Called LookupWord without successfully initializing Intellifinder" );
	if( !m_fInitialized )
	{
		RAlert rAlert;
		rAlert.AlertUser( STRING_ERROR_IF_INITERR );//"The thesaurus database was not successfully initialized.  The file may be corrupt or missing.  Please reinstall the application and restart." ); 
		return FALSE;
	}
	m_fLookupOk = FALSE;

	m_rLookupWord = rWord;
	BOOLEAN fRetVal = FALSE;

	_TUCHAR* pFormattedWord	= NULL;
	try
	{
		m_pEntListIO->pInWord	= NULL;
		//
		// Reset RE_ENTLIST_IO structure...
		if( m_pEntListIO )
		{
			//
			// Free allocated memory for entry list
			REmemory( (void**)&m_pEntListIO, m_pDbIO, RE_ENTLIST_ST, RE_STOP, NULLS );
		}
		REmemory( (void**)&m_pEntListIO, m_pDbIO, RE_ENTLIST_ST, RE_START, RE_DEFAULT_SIZ );
		pFormattedWord				= new _TUCHAR[rWord.GetStringLength() + 1];
		m_pEntListIO->pInWord	= new _TUCHAR[rWord.GetStringLength() + 1];

		strcpy( (_TCHAR*)pFormattedWord, (LPCSZ)rWord );
		RCorrectSpell::FormatWord( pFormattedWord, (_TUCHAR)m_pDbIO->nLanguage );
		//
		// Prepare RE_ENTLIST_IO structure for word verification...
		strcpy((char*)m_pEntListIO->pInWord, (const char*)pFormattedWord );
		m_pEntListIO->uCap			= RE_NOT_SIG_CAP;
		m_pEntListIO->nDBID			= m_pDbIO->nDBID;
		m_pEntListIO->pTocFilter	= (RE_toc_filter*)RE_DEF_FILTER;

		//
		// Verify the input word ( and see if synonyms were found).  If synonyms were found, 
		// 
		if( ( REentlst( m_pEntListIO, RE_VERIFY, RE_START ) == OKRET) )
		{
			if( m_pETParIO )
			{
				//
				// Free allocated memory for etpar struct
				REmemory( (void**)&m_pETParIO, m_pDbIO, RE_ETPAR_ST, RE_STOP, NULLS );
			}
			if( m_pStrTabIO )
			{
				//
				// Free allocated memory for etpar struct
				REmemory( (void**)&m_pStrTabIO, m_pDbIO, RE_STRTAB_ST, RE_STOP, NULLS );
			}
			if( m_pEntListIO->nReturns )
			{
				//
				if( REmemory( (void**)&m_pETParIO, m_pDbIO, RE_ETPAR_ST, RE_START, RE_DEFAULT_SIZ )		== OKRET &&
					 REmemory( (void**)&m_pStrTabIO, m_pDbIO, RE_STRTAB_ST, RE_START, RE_DEFAULT_SIZ )	== OKRET )
				{
					//
					// Synonyms were found...
					//
					// Prepare RE_ETPAR_IO structure for definiton retrieval..
					m_pETParIO->pDBIO			= m_pDbIO;
					m_pETParIO->pBaseform	= m_pEntListIO->pBaseform;
					m_pETParIO->pEntList		= m_pEntListIO->pEntList;
					m_pETParIO->nReturns		= m_pEntListIO->nReturns;
					m_pETParIO->nEntOffset	= 0;
					m_pETParIO->bCrossRef	= FALSE;
					if( REetpar( m_pETParIO, RE_FULL_PARSE, RE_START ) == OKRET )
					{
						//
						// Definition was found... now all the IO structures have the data necessary to return syns/defs..
						fRetVal = TRUE;		
						m_fLookupOk = TRUE;
					}
					else
					{
						RAlert rAlert;
						rAlert.WarnUser( STRING_ERROR_IF_LOOKUPERR );
					}
				}
				else
				{
					//
					// Failed to reinitialize memory for ETParIO or StrTabIO structs..
		 			::ReportException( kMemory );
				}
			}
		}
	}
	catch( ... )
	{
		delete[] pFormattedWord;
		delete[] m_pEntListIO->pInWord;
		m_fInitialized	= FALSE;
		throw;
	}
	delete[] pFormattedWord;
	delete[] m_pEntListIO->pInWord;
	m_pEntListIO->pInWord	= NULL;

	return fRetVal;
}
//*****************************************************************************************************
// Function Name:	RIntelliFinder::GetNextDefinition
//
// Description:	Get definition of given word
//
// Returns:			TRUE if successful, FALSE otherwise
//
// Exceptions:		None
//
//*****************************************************************************************************
BOOLEAN RIntelliFinder::GetNextDefinition( RMBCString& rDefinition )
{
	const RMBCString kLeftParanString = "(";
	const RMBCString kRightParanString = ") ";

	TpsAssert( m_rLookupWord.GetStringLength(), "Called GetNextDefinition with empty word(call LookupWord first)" );
	if( !m_fLookupOk ) return FALSE;

	if( m_nCurrentDef < m_pEntListIO->nReturns )
	{
		//
		// Load next definition...
		rDefinition  = kLeftParanString;
		rDefinition += (LPSZ)(*m_pETParIO->pSynData->pMngArray)->pScndDispStr;
		rDefinition += kRightParanString;
		rDefinition += (LPSZ)(*m_pETParIO->pSynData->pMngArray)->pPrimDispStr;
		m_pETParIO->pSynData->pMngArray++;
		m_nCurrentDef++;
		return TRUE;
	}
	//
	// Out of definitions, reset..
	m_nCurrentDef = 0;
	return FALSE;
}