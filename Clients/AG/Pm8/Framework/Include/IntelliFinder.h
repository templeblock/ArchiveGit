//****************************************************************************
//
// File Name:		IntelliFinder.h
//
// Project:			Renaissance Framework Component
//
// Author:			Mike Heydlauf
//
// Description:	Thesaurus interface to Inso libraries header.
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
//  $Logfile:: /PM8/Framework/Include/IntelliFinder.h                          $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
//*****************************************************************************

#ifndef		_INTELLIFINDER_H_
#define		_INTELLIFINDER_H_

#define SF_RE
extern "C"
{
	//
	// Intellifinder API includes
	#include "slfun.h"
	#include "sl.h"
	#include "slcalls.h"
	#include "slascii.h"
	#include "slchrmcs.h"
	#include "slmacros.h"
	#include "sllang.h"
	#include "sllibext.h"
	#include "slmemman.h"
	#include "slmemtyp.h"
	#include "slatt.h"
	#include "sffun.h"
	#include "Reerrors.h"
	#include "restatus.h"
	#include "reoutmac.h"
	#include "reapidef.h"
	#include "retpadef.h"
	#include "reapityp.h"
	#include "retpatyp.h"
	#include "reapi.h"
	#include "pdexthdr.h"

}

BOOLEAN	LocateFile( RMBCString& rFile, RMBCString& rPath );

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class FrameworkLinkage RIntelliFinder 
{
	// Construction & destruction
	public :
					RIntelliFinder( );
		virtual	~RIntelliFinder( );

		//
		// Initialization methods
		BOOLEAN	Initialize( );
		void		Terminate();

		//
		// Thesaurus methods
		BOOLEAN	IsInitialized( );
		BOOLEAN	LookupWord( RMBCString& rWord );
		BOOLEAN	GetNextSynonym( RMBCString& rSynonym );	
		BOOLEAN	GetNextDefinition( RMBCString& rDefinition );

		//
		// Protected methods
	protected:
		BOOLEAN	EndSession();


	// Member vars
	protected:
		RMBCString				m_rLookupWord;

		BOOLEAN					m_fInitialized; //Flag to see if class has been completely initialized
		BOOLEAN					m_fLookupOk;	 //Flag to see if structures are set to return syns/defs
		int						m_nNumSyns;
		int						m_nCurrentSyn;
		int						m_nNumDefs;
		int						m_nCurrentDef;
		//
		// Inso IO structures....
		PFRE_APP_IO				m_pAppIO;
		PFRE_DB_IO				m_pDbIO;
		PFRE_ENTLIST_IO		m_pEntListIO;
		PFRE_ETPAR_IO			m_pETParIO;
		PFRE_STRTAB_IO			m_pStrTabIO;
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif //_INTELLIFINDER_H_
