// ****************************************************************************
//
//  File Name:			MergeData.cpp
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RMergeData and supporting classes
//
//  Portability:		Platform independent
//
//  Developed by:		Turning Point Software.
//							One Gateway Center, Suite 800
//							Newton, MA 02158
//							(617) 332-0202
//
//  Client:				Broderbund Software, Inc.         
//
//  Copyright (C) 1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Source/MergeData.cpp                            $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#include "FrameworkIncludes.h"

ASSERTNAME

#include "MergeData.h"

#undef	FrameworkLinkage
#define	FrameworkLinkage

#include "Storage.h"
#include "ApplicationGlobals.h"
#include "FrameworkResourceIds.h"

// ****************************************************************************
//
//  Function Name:	RMergeEntry::RMergeEntry( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMergeEntry::RMergeEntry( )
	: m_fSelected( FALSE )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RMergeEntry::RMergeEntry( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMergeEntry::RMergeEntry( const RMergeEntry& rhs )
	{
	if ( &rhs != this )
		*this = rhs;
	}

// ****************************************************************************
//
//  Function Name:	RMergeEntry::~RMergeEntry( )
//
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMergeEntry::~RMergeEntry( )
	{
	NULL;
	}


// ****************************************************************************
//
//  Function Name:	RMergeEntry::operator=( )
//
//  Description:		assign the values in the rhs to this object
//
//  Returns:			this
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMergeEntry& RMergeEntry::operator=( const RMergeEntry& rhs )
{
	m_fSelected	= rhs.m_fSelected;
	return *this;
}
// ****************************************************************************
//
//  Function Name:	RMergeEntry::IsSelected( )
//
//  Description:		Return TRUE if this entry is selected for printing.
//
//  Returns:			m_fSelected
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN	RMergeEntry::IsSelected( ) const
	{
	return m_fSelected;
	}

// ****************************************************************************
//
//  Function Name:	RMergeEntry::SetSelected( )
//
//  Description:		Set the selected flag for printing.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RMergeEntry::SetSelected( )
	{
	m_fSelected = TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RMergeEntry::ClearSelected( )
//
//  Description:		Clear the selected flag for printing.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RMergeEntry::ClearSelected( )
	{
	m_fSelected = FALSE;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RMergeEntry::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMergeEntry::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RMergeEntry, this );
	}

#endif	// TPSDEBUG


// ****************************************************************************
//
//  Function Name:	RMergeData::RMergeData( )
//
//  Description:		Constructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMergeData::RMergeData( )
	{
	NULL;
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::~RMergeData( )
//
//  Description:		Destructor.
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMergeData::~RMergeData( )
	{
	//	Free up all of the objects in the MergeList
	DeleteAllItems( m_MergeData );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::GetMergeListCount( )
//
//  Description:		Return the number of items in the Merge List.
//
//  Returns:			m_MergeData.Count( );
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter	RMergeData::GetMergeListCount( ) const
	{
	return m_MergeData.Count( );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::GetMergeListEnd( )
//
//  Description:		Return the End of the merge list.
//
//  Returns:			m_MergeData.End( );
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMergeIterator	RMergeData::GetMergeListEnd( ) const
	{
	return m_MergeData.End( );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::GetMergeListStart( )
//
//  Description:		Return the start of the merge list.
//
//  Returns:			m_MergeData.Start( );
//
//  Exceptions:		None
//
// ****************************************************************************
//
RMergeIterator	RMergeData::GetMergeListStart( ) const
	{
	return m_MergeData.Start( );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::InsertMergeEntry( )
//
//  Description:		Merge this entry into the m_MergeData class (and sort)
//
//  Returns:			Nothing
//
//  Exceptions:		kMemory
//
// ****************************************************************************
//
YCounter RMergeData::InsertMergeEntry( RMergeEntry* pEntry )
	{
	m_MergeData.InsertAtEnd( pEntry );
	SortList( );

	YCounter				index			= 0;
	RMergeIterator		iterator		= GetMergeListStart( );
	RMergeIterator		iteratorEnd	= GetMergeListEnd( );
	for ( ; iterator != iteratorEnd; ++iterator, ++index )
		if ( *iterator == pEntry )
			break;

	TpsAssert( index <= GetMergeListCount( ), "Index of inserted item is after list" );
	return index;
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::DeleteMergeEntry( )
//
//  Description:		Delete the given entry from the list
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void	RMergeData::DeleteMergeEntry( YCounter entryNumber )
	{
	TpsAssert( entryNumber <= m_MergeData.Count( ), "Deleting entry number that is larger than list size" );

	RMergeIterator	iterator	= GetMergeListStart( ) + entryNumber;
	m_MergeData.RemoveAt( iterator );
	//
	//	Clear up the item
	delete (*iterator);
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::DeleteMergeEntry( )
//
//  Description:		Delete the given entry from the list
//
//  Returns:			Nothing
//
//  Exceptions:		Nothing
//
// ****************************************************************************
//
void	RMergeData::DeleteMergeEntry( RMergeEntry* pEntry )
	{
	m_MergeData.Remove( pEntry );
	//
	//	Delete the entry
	delete pEntry;
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::SelectAllEntries( )
//
//  Description:		Select all of the entries in the list (for printing)
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RMergeData::SelectAllEntries( )
	{
	RMergeIterator	iterator	= GetMergeListStart( );
	RMergeIterator	iterEnd	= GetMergeListEnd( );
	for ( ; iterator != iterEnd; ++iterator )
		(*iterator)->SetSelected( );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::SelectEntry( )
//
//  Description:		Select the given entry (for printing)
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RMergeData::SelectEntry( YCounter entryNumber )
	{
	TpsAssert( entryNumber <= m_MergeData.Count( ), "Selecting entry number that is larger than list size" );

	RMergeIterator	iterator	= GetMergeListStart( ) + entryNumber;
	(*iterator)->SetSelected( );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::ClearEntry( )
//
//  Description:		Clear the given entry (for printing)
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RMergeData::ClearEntry( YCounter entryNumber )
	{
	TpsAssert( entryNumber <= m_MergeData.Count( ), "Clearing entry number that is larger than list size" );

	RMergeIterator	iterator	= GetMergeListStart( ) + entryNumber;
	(*iterator)->ClearSelected( );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::ClearAllEntries( )
//
//  Description:		Clears all of the entries in the list (for printing)
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void	RMergeData::ClearAllEntries( )
	{
	RMergeIterator	iterator	= GetMergeListStart( );
	RMergeIterator	iterEnd	= GetMergeListEnd( );
	for ( ; iterator != iterEnd; ++iterator )
		(*iterator)->ClearSelected( );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::ClearAllEntries( )
//
//  Description:		Filters out entries in the list that do not contain the specified field
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMergeData::FilterEntries(const YMergeId nId)
{
	for (RMergeIterator itr = GetMergeListStart(); itr != GetMergeListEnd(); ++itr)
	{
		if ((*itr)->GetFieldEntry(nId).IsEmpty()) DeleteMergeEntry(*itr--);				
	}
}

// ****************************************************************************
//
//  Function Name:	RMergeData::ClearAllEntries( )
//
//  Description:		Clears all of the entries in the list (for printing)
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
YCounter	RMergeData::CountSelected( ) const
	{
	YCounter			counter	= 0;
	RMergeIterator	iterator	= GetMergeListStart( );
	RMergeIterator	iterEnd	= GetMergeListEnd( );
	for ( ; iterator != iterEnd; ++iterator )
		{
		if ( (*iterator)->IsSelected( ) )
			++counter;
		}

	return counter;
	}

// ****************************************************************************
//		
//  Function Name:	MergeCompareProc( )
//
//  Description:		Compare two Merge entries for equality
//
//  Returns:			standard ansi compare values
//
//  Exceptions:		None
//		
// ****************************************************************************
//
	inline int MergeCompareProc( RMergeEntry* merge1, RMergeEntry* merge2 )
		{
		RMBCString&		merge1Key	= merge1->GetFieldKey( );
		RMBCString&		merge2Key	= merge2->GetFieldKey( );
		return merge1Key.Compare( merge2Key );
		}

// ****************************************************************************
//
//  Function Name:	RMergeData::SortList( )
//
//  Description:		Sort the List to arrange all entries
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMergeData::SortList( )
	{
	::Sort( GetMergeListStart( ), GetMergeListEnd(), MergeCompareProc );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::GetFileName( )
//
//  Description:		Return the File name of this MergeData entry
//
//  Returns:			m_FileName
//
//  Exceptions:		None
//
// ****************************************************************************
//
const RMBCString& RMergeData::GetFileName( ) const
	{
	return m_FileName;
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::NewFile( )
//
//  Description:		Make this MergeData think that it is a new MergeData
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMergeData::NewFile( )
	{
	::DeleteAllItems( m_MergeData );
	m_MergeData.Empty();

	m_FileName = "";	//	Empty string to remove the name
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::OpenFile( )
//
//  Description:		Make this MergeData takes its values from a choosen file
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMergeData::OpenFile( BOOL& bCancelled )
	{
	RMBCString	fileName;
	bCancelled = FALSE;
	BOOL bCompatibleMode = FALSE;

#ifdef	_WINDOWS
	RMBCString	fileExtension;
	RMBCString	fileFilter;
	ReturnFileExtension( fileExtension, FALSE );
	ReturnFileFilter( fileFilter, FALSE );
	CFileDialog	dialog( TRUE, (LPCTSTR)fileExtension, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
								(LPCTSTR)fileFilter, NULL );
	if ( dialog.DoModal( ) == IDCANCEL )
	{
		bCancelled = TRUE;
		return FALSE;
	}

	fileName	= dialog.GetPathName( );

	// did the user pick a PSD4 file?
	ReturnFileExtension( fileExtension, TRUE );

	// we have to add a '.' to the start of what GetFileExt()
	// returns so we can do the comparision.
	CString	szExtension('.' + dialog.GetFileExt() );
	
	if ( szExtension.CompareNoCase( fileExtension ) == 0 )
		bCompatibleMode = TRUE;

#else		//	MAC

#endif	//	_WINDOWS

	//	Validate we can read the file
	try
	{
		RStorage	storage( fileName, kRead );
	}
	catch( YException exception )
	{
		if ( exception != kEndOfFile )
			return FALSE;
	}
	catch( ... )
	{
		return FALSE;
	}

	return OpenFile( fileName, bCompatibleMode );
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::OpenFile( )
//
//  Description:		Make this MergeData takes its values from the given file
//							if bCompatibleMode == TRUE the file is a PSD4 file
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMergeData::OpenFile( const RMBCString& fileName, BOOL bCompatibleMode )
	{
	uBYTE*			pBuffer			= ::GetGlobalBuffer( );
	BOOLEAN			fReturnValue	= FALSE;
	try
		{
		RStorage	storage( fileName, kRead );

		//	Quick validate a that we are possible using a text file.
		YStreamLength	current		= storage.GetPosition( );
		YStreamLength	readAmt		= ::GetGlobalBufferSize( );
		try
		{
			storage.Read( readAmt, pBuffer );
		}
		catch( YException exception )
		{
			if ( exception != kEndOfFile )
				throw;
			readAmt	= storage.GetPosition( ) - current;
		}
		for (YStreamLength iCnt = 0; iCnt < readAmt; ++iCnt )
			if ( pBuffer[iCnt] == '\0' )	//	If null, this can't be a text file...
				throw kDataFormatInvalid;
		//
		//		Reset to beginning of file.
		storage.SeekAbsolute( current );
		//	delete the entries, and empty the list
		::DeleteAllItems( m_MergeData );
		m_MergeData.Empty( );

		//	Now, merge the new list if there is actually data
		if ( readAmt > 0 )
		{
			if ( bCompatibleMode == FALSE )
			{
				MergeStorage( storage );
		
				//	Remember the new file name
				m_FileName		= fileName;
			}
			else
				MergeCompatibleStorage( storage );
		}

		//	We succeeded
		fReturnValue	= TRUE;
		}
	catch( YException exception )
		{
		if ( exception == kEndOfFile )
			{
			::DeleteAllItems( m_MergeData );
			m_MergeData.Empty( );
			m_FileName		= fileName;
			fReturnValue	= TRUE;
			}
		else
			::ReportException( exception );
		}
	catch( ... )
		{
		::ReportException( kUnknownError );
		}

	::ReleaseGlobalBuffer( pBuffer );
	return fReturnValue;
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::InsertFile( )
//
//  Description:		Merge this MergeData with the choosen file
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMergeData::InsertFile( )
	{
	UntestedCode();

	BOOLEAN	fSuccess	= FALSE;
	try
		{
		RMBCString	fileName;

#ifdef	_WINDOWS
		CFileDialog	dialog( FALSE );
		if ( dialog.DoModal( ) == IDCANCEL )
			return FALSE;
		fileName	= dialog.GetPathName( );
#else		//	MAC

#endif	//	_WINDOWS

		RStorage	storage( fileName, kRead );
		MergeStorage( storage );
		fSuccess = TRUE;
		}
	catch( YException exception )
		{
		::ReportException( exception );
		}
	catch( ... )
		{
		::ReportException( kUnknownError );
		}

	return fSuccess;
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::SaveFile( )
//
//  Description:		Save the data for this MergeData to the saved file
//
//  Returns:			Nothing
//
//  Exceptions:		None.
//
// ****************************************************************************
//
BOOLEAN RMergeData::SaveFile( )
	{
	//	If no file name choosen, call SaveAsFile
	if ( m_FileName.GetStringLength() == 0 )
		return SaveAsFile( );
	else
		{
		try
			{
			RStorage	storage( m_FileName, kReadWriteReplaceExisting );
			RMergeIterator	iterator		= GetMergeListStart( );
			RMergeIterator	iteratorEnd	= GetMergeListEnd( );

			//	Loop through and ask each entry to save itself
			for ( ; iterator != iteratorEnd; ++iterator )
				(*iterator)->Write( storage );
			}
		catch( YException exception )
			{
			::ReportException( exception );
			return FALSE;
			}
		catch( ... )
			{
			::ReportException( kUnknownError );
			return FALSE;
			}
		}

	return TRUE;
	}
	
// ****************************************************************************
//
//  Function Name:	RMergeData::SaveAsFile( )
//
//  Description:		Make this MergeData takes its values from a choosen file
//							
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
BOOLEAN RMergeData::SaveAsFile( )
	{

	try
	{
		RMBCString	fileName;

	#ifdef	_WINDOWS
		RMBCString	fileExtension;
		RMBCString	fileFilter;
		ReturnFileExtension( fileExtension, TRUE );
		ReturnFileFilter( fileFilter, TRUE );
		CFileDialog	dialog( FALSE, (LPCTSTR)fileExtension, NULL, OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
									(LPCTSTR)fileFilter, NULL );
		if ( dialog.DoModal( ) == IDCANCEL )
			return FALSE;
		fileName	= dialog.GetPathName( );
	#else	//	MAC
	#endif	//	_WINDOWS

		if ( fileName.GetStringLength() > 0 )
			{
			RMBCString	oldFileName	= m_FileName;
			m_FileName = fileName;
			if ( !SaveFile( ) )
				{
				m_FileName = oldFileName;
				return FALSE;
				}
			}
	}
	catch( YException exception )
		{
		::ReportException( exception );
		return FALSE;
		}
	catch( ... )
		{
		::ReportException( kUnknownError );
		return FALSE;
		}

	return TRUE;
	}

// ****************************************************************************
//
//  Function Name:	RMergeData::ReadTabDelimitedString( )
//
//  Description:		Read a string that is tab delimited from the storage.
//							Eat the delimeter character also (eof, cr/lf, tab).
//
//  Returns:			Nothing
//
//  Exceptions:		Any File Exception, will not send through on kEndOfFile
//
// ****************************************************************************
//
RMergeData::EDataDelimiter RMergeData::ReadTabDelimitedString( RStorage& storage, RMBCString& string )
	{
	EDataDelimiter	delimiter	= kTab;

	try
		{
		uBYTE			ubChar( 0 );
		RCharacter	character;

		//	First, make sure the string is empty
		string.Empty( );

		//	Loop until I hit a EOF, TAB, CR, or LF
		while ( 1 )
			{
			storage >> ubChar;
			if ( (ubChar == '\t') || (ubChar == '\r') || (ubChar == '\n') )
				break;

			//	Check for multi-Byte
			character = ubChar;

			if ( character.IsLead( ) )
				{
				UntestedCode( );
				uBYTE	lead	= ubChar;
				uBYTE	trail;
				storage >> trail;
				character = RCharacter( lead, trail );
				}

			string += character;
			}

			//	If I found a CR or a LF, make sure the other is not also present.
			if ( (ubChar == '\r') || (ubChar == '\n') )
				{
				storage >> ubChar;
				//	If character is NOT a CR or LF, rewind one character
				if ( !((ubChar == '\r') || (ubChar == '\n')) )
					storage.SeekRelative( -1 );
				delimiter = kEOL;
				}
		}
	catch( YException exception )
		{
		if (exception != kEndOfFile)
			throw;
		delimiter	= kEOF;
		}

	return delimiter;
	}

#ifdef	TPSDEBUG

// ****************************************************************************
//
//  Function Name:	RMergeData::Validate( )
//
//  Description:		Validates the object
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RMergeData::Validate( ) const
	{
	RObject::Validate( );
	TpsAssertIsObject( RMergeData, this );
	}

#endif	// TPSDEBUG
