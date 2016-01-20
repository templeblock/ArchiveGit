/*	$Header: /PM8/App/NumberDialog.cpp 1     3/03/99 6:08p Gbeddow $
//
//	Definition of the RNumberDialog class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a number compound graphic.
//	
// Author:     Mike Heydlauf
//
// Portability: Windows Specific
//
// Developed by:	Turning Point Software
//						One Gateway Center, Suite 800
//						Newton, Ma 02158
//						(617)332-0202
//
// Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
// $Log: /PM8/App/NumberDialog.cpp $
// 
// 1     3/03/99 6:08p Gbeddow
// 
// 2     2/11/99 3:01p Rgrenfel
// Forced reconstruction of the graphics collection searcher when
// necessary.
// 
// 1     2/08/99 3:10p Rgrenfel
// Implementation of the smart number grouped object dialog.
*/

#include "StdAfx.h"

ASSERTNAME

#include "NumberDialog.h"
#include "ComponentTypes.h"
#include "ComponentView.h"
#include "ComponentDocument.h"
#include "ResourceManager.h"

const int kMaxNumNumbers	= 3;
const int kMaxNumber			= 999;

//
// To be safe for all languages we define suffixes for every number up to
// 20, then just define suffixes for the second digit for numbers greater
// than that.
const int kNumberOfSuffixesDefinedFromZero = 20;

// ****************************************************************************
//
//  Function Name:	RNumberDialog::RNumberDialog
//
//  Description:		Ctor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RNumberDialog::RNumberDialog( RStandaloneDocument* pParentDocument, RComponentDocument* pEditedComponent, int nResourceId )
	: RInitCapDialog(pParentDocument,pEditedComponent,nResourceId),
	m_pGraphicCollectionSearcher( NULL ),
	m_fSmartNumber( FALSE )
{
}

// ****************************************************************************
//
//  Function Name:	RNumberDialog::~RNumberDialog
//
//  Description:		Dtor
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
RNumberDialog::~RNumberDialog()
{
	delete m_pGraphicCollectionSearcher;
}


// ****************************************************************************
//
//  Function Name:	RNumberDialog::DoDataExchange
//
//  Description:		dialog data exchange
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RNumberDialog::DoDataExchange(CDataExchange* pDX)
{
	RInitCapDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RNumberDialog)
	BOOL fSmartNum = static_cast<BOOL>( m_fSmartNumber );
	DDX_Check( pDX, CONTROL_CHKBOX_NUMBER_SMARTNUMBER, fSmartNum );
	m_fSmartNumber = static_cast<BOOLEAN>( fSmartNum );
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(RNumberDialog, RInitCapDialog)
	//{{AFX_MSG_MAP(RNumberDialog)
	ON_BN_CLICKED(CONTROL_CHKBOX_NUMBER_SMARTNUMBER, OnButtonNumberSmartNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// RNumberDialog message handlers


// ****************************************************************************
//
//  Function Name:	RNumberDialog::OnInitDialog
//
//  Description:		dialog Numberization.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
BOOL RNumberDialog::OnInitDialog() 
{
	m_rHeadlineText =  ::GetResourceManager().GetResourceString( IDS_DEFAULT_NUMBER );

	if( !RInitCapDialog::OnInitDialog() ) return FALSE;

	m_cHeadlineEditCtrl.SetLimitText( kMaxNumNumbers );

	// Display the number
	DisplayNumber();
	ApplyHeadlineEffect();

	m_fInitializing = FALSE;
	UpdatePreview();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// ****************************************************************************
//
//  Function Name:	RNumberDialog::CreateGraphicsList
//
//  Description:		Create '|' delimited string of graphics names
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RNumberDialog::CreateGraphicsList( RMBCString& rList )
{
	//
	// Only allocate memory for the collection searcher once..
	if( m_pGraphicCollectionSearcher == NULL || m_fRebuiltGraphicArrays )
	{
		// We rebuilt the graphic arrays, so we have to recreate the searcher.
		if (m_pGraphicCollectionSearcher != NULL)
		{
			delete m_pGraphicCollectionSearcher;
		}
		m_pGraphicCollectionSearcher = new 	RCollectionSearcher<RNumberGraphicSearchCollection>(m_rCollectionArray);
	}
	// Find all the graphics
	RSearchResultArray* pResultArray = m_pGraphicCollectionSearcher->SearchCollections();

	// Now iterate through all the graphics and insert them into
	// the string
	RSearchResultIterator resultIter( pResultArray->Start() );
	RSearchResultIterator resultIterEnd( pResultArray->End() );

	// The index has to start at 1 because the None selection is at zero
	for (int nIdx = 1; resultIter != resultIterEnd; resultIter++, nIdx++ )
	{
		rList += RMBCString( (LPSTR)(*resultIter).namePtr );
		rList += RMBCString( kListFieldSeperator );

		//
		// Keep track of graphics and their indexes...
		m_cGraphicMap.SetAt( (*resultIter).id, (*resultIter) );
		m_cGraphicArray.SetAtGrow( nIdx, (*resultIter) );
   }
}
// ****************************************************************************
//
//  Function Name:	RNumberDialog::OnEditChangeHeadlineText
//
//  Description:		Number text is changing, reset timer.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RNumberDialog::OnEditChangeHeadlineText()
{
	DisplayNumber();
}

// ****************************************************************************
//
//  Function Name:	RNumberDialog::OnButtonNumberSmartNumber
//
//  Description:		User clicksd smart number checkbox.
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RNumberDialog::OnButtonNumberSmartNumber()
{
	m_fSmartNumber = !m_fSmartNumber;
	DisplayNumber();
}

// ****************************************************************************
//
//  Function Name:	RNumberDialog::DisplayNumber
//
//  Description:		Show user entered number in preview respecting smart number
//
//  Returns:			void
//
//  Exceptions:		None
//
// ****************************************************************************
void RNumberDialog::DisplayNumber()
{
	CString cNumber;
	m_cHeadlineEditCtrl.GetWindowText(cNumber);
	m_rHeadlineText = RMBCString(cNumber);
	//
	// Just return if the user has deleted all text
	if( !m_rHeadlineText.IsEmpty() ) 
	{
		int nRangeCheck = atoi( (LPCSZ)m_rHeadlineText );
		TpsAssert( nRangeCheck <= kMaxNumber , "Number contains too many digits" );
		if( m_fSmartNumber )
		{
			m_rHeadlineText += GetSmartNumberSuffix( m_rHeadlineText );
		}
	}
	UpdatePreview();
}

// ****************************************************************************
//
//  Function Name:	RNumberDialog::GetSmartNumberSuffix
//
//  Description:		Return internationally correct suffix for given string 
//							embedded number
//
//  Returns:			suffix
//
//  Exceptions:		None
//
// ****************************************************************************
RMBCString RNumberDialog::GetSmartNumberSuffix( const RMBCString& rNumber )
{
	int nNumber = atoi( (LPCSZ)rNumber );
	TpsAssert( nNumber >= 0 , "Number string invalid. Negative numbers not supported." );
	TpsAssert( nNumber <= kMaxNumber, "Number string invalid. Too large.");
	TpsAssert( rNumber.GetStringLength() <= kMaxNumNumbers, "Number string too long" );
	if( nNumber == 0 && rNumber.GetStringLength() > 1 )
	{
		TpsAssertAlways( "Invalid number string." );
	}

	//
	// Only interested in the last two digits of number (ie. Suffix for 12 is the
	// same as suffix for 112)
	int nMeaningfulDigits = atoi( (LPCSZ)rNumber );
	if( rNumber.GetStringLength() > 2 )
	{
		// If the number is three digits or greater, just get the last two digits..
		char sDigitConvert[3];
		//strncpy( sDigitConvert, (LPCSZ)rNumber[ rNumber.GetStringLength() - 2 ], 2 );
		sDigitConvert[ 0 ] = rNumber[ rNumber.GetStringLength() - 2 ];
		sDigitConvert[ 1 ] = rNumber[ rNumber.GetStringLength() - 1 ];
		sDigitConvert[ 2 ] = 0;
		nMeaningfulDigits = atoi( sDigitConvert );
	}
	//
	// To be safe for all languages we define suffixes for every number up to
	// 20, then just define suffixes for the second digit for numbers greater
	// than that.
	if( nMeaningfulDigits < kNumberOfSuffixesDefinedFromZero )
	{
		/*!!!!!!!!!!!!       NOTE        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		// Smart number ids from 0-19 MUST be sequential or the proper suffix will
		// NOT be used!!!!!! -MWH
		int nFirstSuffixInResource = IDS_SMARTNUM_SUFFIX_0;
		return ::GetResourceManager().GetResourceString( nFirstSuffixInResource + nMeaningfulDigits );
	}
	else 
	{
		//
		// Number is greater than 20, need to get last digit suffix
		RMBCString rLastDigit("");
		rLastDigit += rNumber[rNumber.GetStringLength()-1];
		int nLastDigit = atoi( (LPCSZ)rLastDigit );
		/*!!!!!!!!!!!!       NOTE        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
		// Smart number two digit ids MUST be sequential or the proper suffix will
		// NOT be used!!!!!! -MWH
		int nFirst2DigitSuffixInResource = IDS_SMARTNUM_2DIGIT_SUFFIX_0;
		return ::GetResourceManager().GetResourceString( nFirst2DigitSuffixInResource + nLastDigit );
	}
}

// ****************************************************************************
//
//  Function Name:	RNumberDialog::GetComponentType
//
//  Description:		Gets the type of component that this dialog creates and
//							edits
//
//  Returns:			Component type
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YComponentType& RNumberDialog::GetComponentType( ) const
	{
	static YComponentType componentType = kSmartNumberComponent;

	return componentType;
	}

// ****************************************************************************
//
//  Function Name:	RNumberDialog::WriteUIContextData
//
//  Description:		Writes this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RNumberDialog::WriteUIContextData( RUIContextData& contextData ) const
{
	//
	// REVIEW MWH -Can no longer call InitCap's ReadUIContextData (base method)
	// because of UI persistance changes and backward compatiability.
	// Write common data..
	RCommonGroupedObjectDialog::WriteUIContextData( contextData );

	// REVIEW MWH -this call is also on InitCapDialog...
	// Write out the solid color effects
	contextData << m_rSolidColorEffect;

	// Write checkstate of smart number
	contextData << m_fSmartNumber;

	// REVIEW MWH -this would go in the base method, but it can't
	// because then we wouldn't be backwards compatiable
	// Write out the selected graphics id
	contextData << m_uSelectedGraphicId;

	//	Write extra data at the current end of the context data.
	RCommonGroupedObjectDialog::WriteVersion2UIContextData( contextData );
}

// ****************************************************************************
//
//  Function Name:	RNumberDialog::ReadUIContextData
//
//  Description:		Read this dialogs UI context data
//
//  Returns:			pointer to ContextData
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData* RNumberDialog::ReadUIContextData( ) 
{
	RUIContextData* pContextData = NULL;
	//
	// REVIEW MWH -Can no longer call InitCap's ReadUIContextData (base method)
	// because of UI persistance changes and backward compatiability.
	// Write common data..
	pContextData = RCommonGroupedObjectDialog::ReadUIContextData();
	if ( pContextData )
	{
		try
		{
			// REVIEW MWH -this call is also on InitCapDialog...
			// Read in the color effects
			(*pContextData) >> m_rSolidColorEffect;

			//
			// Strip any possible suffixes from the headline text..
			StripSmartNumber( m_rHeadlineText );

			// Get checkstate of smart number
			(*pContextData) >> m_fSmartNumber;

			// REVIEW MWH -this would go in the base method, but it can't
			// because then we wouldn't be backwards compatiable.
			// Read in the selected graphics id
			(*pContextData) >> m_uSelectedGraphicId;

			// Read common data..
			if ( pContextData )
				pContextData 
				 = RCommonGroupedObjectDialog::ReadVersion2UIContextData( *pContextData );
		}
		catch( ... )
		{
			// Catch exceptions thrown be earlier versions that don't support
			// all current UI info.
			m_rHeadlineText = "1";
		}

		UpdateData( FALSE );
	}
	return pContextData;
}

// ****************************************************************************
//
//  Function Name:	RNumberDialog::StripSmartNumber
//
//  Description:		Strip the smart number suffix if there is one.
//
//  Returns:			pointer to ContextData
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RNumberDialog::StripSmartNumber( RMBCString& rNumber )
{
	char* pStrippedNumber = new char[ rNumber.GetStringLength() + 1 ];
	try
	{
		strcpy( pStrippedNumber, (LPCSZ)rNumber );
		for( int nIdx = 0; nIdx < rNumber.GetStringLength(); nIdx++ )
		{
			if( !isdigit( pStrippedNumber[nIdx] ) )
			{
				pStrippedNumber[nIdx] = 0;	
				break;
			}
		}
	}
	catch( ... )
	{
		delete [] pStrippedNumber;
		throw;
	}
	rNumber = RMBCString( pStrippedNumber );
	delete [] pStrippedNumber;
}