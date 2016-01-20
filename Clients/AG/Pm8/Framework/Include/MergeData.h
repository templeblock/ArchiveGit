// ****************************************************************************
//
//  File Name:			MergeData.h
//
//  Project:			Renaissance Framework
//
//  Author:				M. Houle
//
//  Description:		Declaration of the RMergeData class
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
//  Copyright (C) 1995-1996 Turning Point Software. All Rights Reserved.
//
//  $Logfile:: /PM8/Framework/Include/MergeData.h                             $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

#ifndef		_MERGEDATA_H_
#define		_MERGEDATA_H_

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

//	Object.h is in PCH


// ****************************************************************************
//
//  Notes:
//
//			Valid MergeId Ranges are between 1 and 249.
//			The range of 250-255 is reserved delimiters needed for building
//			token arrays for merge entries that require more than one field
//
// ****************************************************************************
//
const YMergeId		kMergeFieldReturn	= 250;
const YMergeId		kMergeFieldSpace	= 251;
const YMergeId		kMergeFieldTab		= 252;
const YMergeId		kMergeFieldComma	= 253;

//	Keywords, Typedefs, Structures, & prototypes
class RStorage;

typedef	uLONG		YMergeTypeId;

const YMergeTypeId	kNoMergeTypeDefined	= 0;

// ****************************************************************************
//
//  Class Name:	RMergeEntry
//
//  Description:	This class will maintain one Merge Entry for use by the 
//						MergeData class.
//
// ****************************************************************************
//
class FrameworkLinkage RMergeEntry : public RObject
	{
	//	Construction & Destruction
	public :
													RMergeEntry( );
													RMergeEntry( const RMergeEntry& rhs );
		virtual									~RMergeEntry( );

		RMergeEntry&							operator=( const RMergeEntry& rhs );
	//	Retrieval mechinism
	public :
		virtual RMBCString&					GetFieldEntry( YMergeId mergeId ) = 0;
		virtual RMBCString&					GetFieldKey( ) = 0;

	//	Loading and saving
	public :
		virtual void							Read( RStorage& storage ) = 0;
		virtual void							Write( RStorage& storage ) const = 0;

	//	Get/Set for printing
	public :
		BOOLEAN									IsSelected( ) const;
		void										SetSelected( );
		void										ClearSelected( );

	//		Member data.
	private :
		BOOLEAN									m_fSelected;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void							Validate( ) const = 0;
#endif
	} ;


//	Keywords, Typedefs, Structures, & prototypes
typedef	RList< RMergeEntry* >				RMergeEntryContainer;
typedef	RMergeEntryContainer::YIterator	RMergeIterator;

// ****************************************************************************
//
//  Class Name:	RMergeData
//
//  Description:	This class will maintain any Merge data in use by a 
//						Standalone document.
//
// ****************************************************************************
//
class FrameworkLinkage RMergeData : public RObject
	{
	//	Enums
	protected :
		enum	EDataDelimiter	{	kTab = 0, kEOL, kEOF };

	//	Construction & Destruction
	public :
													RMergeData( );
		virtual									~RMergeData( );


	//	Retrieval of information
	public :
		virtual RMBCString&					GetFieldDescriptor( YMergeId mergeId ) = 0;
		virtual BOOLEAN						SelectField( RMergeFieldContainer& ) const = 0;
		virtual YMergeTypeId					GetMergeDataType( ) const = 0;

		YCounter									GetMergeListCount( ) const;
		RMergeIterator							GetMergeListEnd( ) const;
		RMergeIterator							GetMergeListStart( ) const;

		void										SelectAllEntries( );
		void										SelectEntry( YCounter entryNumber );
		YCounter									CountSelected( ) const;
		
		void										ClearEntry( YCounter entryNumber );
		void										ClearAllEntries( );		
		void										FilterEntries(const YMergeId nId);

		//	Calls for the EditList Dialog
		const RMBCString&						GetFileName( ) const;
		void										NewFile( );
		BOOLEAN									OpenFile( BOOL& bCancelled );
		BOOLEAN									OpenFile( const RMBCString& fileName, BOOL bCompatibleMode = FALSE );
		BOOLEAN									InsertFile( );
		BOOLEAN									SaveFile( );
		BOOLEAN									SaveAsFile( );
		YCounter									InsertMergeEntry( RMergeEntry* pEntry );
		void										DeleteMergeEntry( YCounter entryNumber );
		void										DeleteMergeEntry( RMergeEntry* pEntry );

		virtual RMergeEntry*					CloneEntry( RMergeEntry* pEntry ) const = 0;

	//	Implementation
	protected :
		virtual void							MergeStorage( RStorage& storage ) = 0;
		virtual void							MergeCompatibleStorage( RStorage& storage ) = 0;
		virtual void							ReturnFileExtension( RMBCString& extension, BOOL bCompatible = FALSE ) = 0;
		virtual void							ReturnFileFilter( RMBCString& filter, BOOL bCompatible = FALSE  ) = 0;
		EDataDelimiter							ReadTabDelimitedString( RStorage& storage, RMBCString& string );
		void										SortList( );

	private :
		RMergeEntryContainer					m_MergeData;
		RMBCString								m_FileName;

#ifdef	TPSDEBUG
	// Debugging stuff
	public :
		virtual void							Validate( ) const = 0;
#endif
	} ;

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif

#endif	//	_MERGEDATA_H_
