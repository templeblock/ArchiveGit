// ****************************************************************************
//
//  File Name:			SearchCollection.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of abstract base class that performs a
//							search on a collection file. Concrete subclasses are
//							suitable as the parameterized type of an RCollectionSearcher
//
//  Portability:		Mostly cross platform (some Win32 dependencies)
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef _SearchCollection_H_
#define _SearchCollection_H_


#include <stdio.h>
#include "ChunkyStorage.h"			// RChunkStorage class
#include "SearchResult.h"
#include "CollectionManager.h"


// set to 0 to disable ClickSearch search engine, 1 to enable
// (used in graphic keyword browser and quotes and verses keyword browser)
#define ENABLE_CLICKSEARCH 0

enum RKeywordSearchOption
{
	kSynonymSearch = 0,
	kCLangStringSearch,
	kAsmLangStringSearch
};

extern RKeywordSearchOption gKeywordSearchOption;


class RCollectionDate : public RObject
{
public:
				RCollectionDate()
				// default constructor for an RCollectionDate
				:	theDate( 0 )
				{}

				RCollectionDate( char collectionDateBuf[12] )
				// construct RCollectionDate from buffer coming from collection file
				// buffer format: mmddyyyyhhmm (month, day, year, 24 hour, minute)
				// valid range: 1/1/0 to 12/31/9999 (caution: minimal error checking)
				: theDate( 0 )
				{
					SetDate( collectionDateBuf );
				}

	virtual	~RCollectionDate() {}

	void		SetDate( char collectionDateBuf[12] )
				// set date from buffer coming from collection file
				// buffer format: mmddyyyyhhmm (month, day, year, 24 hour, minute)
				// valid range: 1/1/0 to 12/31/9999 (caution: minimal error checking)
				{
					if (collectionDateBuf[0])
					{
						int month, day, year;
						int sscanfRetVal = ::sscanf( collectionDateBuf, "%2d%2d%4d", &month, &day, &year );
						TpsAssert( sscanfRetVal == 3 &&
							1 <= month && month <= 12 &&
							1 <= day && day <= 31 &&
							0 <= year && year <= 9999, "Invalid collection date" );
						theDate = year << 16;
						theDate |= month << 8;
						theDate |= day;
					}
				}

	int		Compare( RCollectionDate& date2 )
				// compare this date with date2
				// return a value:
				//		<0 if this date < date2
				//		=0 if this date == date2
				//		>0 if this date > date2
				{
					return theDate - date2.theDate;
				}

	void		GetCurrentDate( char collectionDateBuf[12] )
				// return current date into collectionDateBuf
				// buffer format: mmddyyyyhhmm (month, day, year, 24 hour, minute)
				// relies on CTime class which returns year in the range 1970 to 2038
				{
					CTime t = CTime::GetCurrentTime();
					sprintf( collectionDateBuf, "%#02i%#02i%#04i%#02i%#02i",
						t.GetMonth(), t.GetDay(), t.GetYear(), t.GetHour(), t.GetMinute() );
				}

	virtual void	Validate() const {}

private:
	long		theDate;
};


class RSearchCriteria : public RObject
{
public:
				RSearchCriteria() { }
	virtual ~RSearchCriteria() { }

	virtual void	Validate() const {}
};


class RSearchCollection : public RObject
{
public:
						RSearchCollection( RChunkStorage *aChunkStorage )
						// construct an RSearchCollection from aChunkStorage
							:	theChunkStorage( aChunkStorage ),
								theCurrentEntry( 0 ),
								theTotalEntries( 0 ),
								theEntrySize( 0 ),
								theNameOffset( 0 ),
								theDataOffset( 0 ),
								theNAMS( 0 ),
								theAtEndFlag( FALSE ),
								theID( 0 ),
								theDate(),
								theShowNewestAtTopFlag( FALSE )
						{ TpsAssert( aChunkStorage, "NULL ChunkStorage." ); }

	virtual			~RSearchCollection()
						{ delete[] theNAMS; }

	virtual void	SetSearchCriteria( const RSearchCriteria* aSearchCriteria )=0;
						// Initializes the RSearchCriteria object. This object is
						// used by GetNumItems() and MatchSearchCriteria() to perform
						// the appropriate operations.

	virtual uLONG	GetNumItems()=0;
						// gets the number of items to be searched in the collection

	virtual void	InitSearch()
						// seek to CLHD chunk and read collection date into theDate
						// seek to NAMS chunk and cache into theNAMS
						// seek to index chunk and read theTotalEntries,
						// leave aChunkStorage positioned to the first index entry
						// throws exception if NAMS or INDX chunk can't be found in
						// theChunkStorage or on a read error
						//
						// subclasses should call inherited member function then
						// do whatever additional steps are needed
						{
							TpsAssert( theChunkStorage, "NULL ChunkStorage." );
							GetCollectionDate();
							CacheNamesChunk();
							SeekToFirstIndexEntry();
							theAtEndFlag = (theTotalEntries > 0 ? FALSE : TRUE);
						}

	virtual void	SeekToFirstIndexEntry()
						// seek to beginning of index chunk
						//
						// subclasses should read theTotalEntries (see example below)
						// and leave theChunkStorage positioned for
						// reading the first index entry (via ReadNextIndexEntry)
						{
							TpsAssert( theChunkStorage, "NULL ChunkStorage." );
							theChunkStorage->SelectRootChunk();
							RChunkStorage::RChunkSearcher yIter = theChunkStorage->Start( 'INDX', FALSE );
							if (yIter.End())
								throw kBadSeek;
							else
							{
								theCurrentEntry = 0;
								// subclasses should call inherited member function then
								// seek to the appropriate index chunk and read
								// theTotalEntries something like this:
								//
								// RSearchCollection::SeekToFirstIndexEntry();
								// RChunkStorage::RChunkSearcher yIter =
								//		theChunkStorage->Start( 'INAL', FALSE );
								// if (yIter.End())
								// 	throw kBadSeek;
								// else
								// 	*theChunkStorage >> theTotalEntries;
							}
						}

	virtual BOOLEAN ReadNextIndexEntry()
						// read the next index entry from theChunkStorage
						// (assumes that theChunkStorage is already positioned to the next entry)
						// return TRUE for success, else FALSE
						//
						// subclasses should follow example below
						{
							if (theCurrentEntry >= theTotalEntries)
							{
								theAtEndFlag = TRUE;		// note: one record read-ahead
								return FALSE;
							}
							else
							{
								++theCurrentEntry;

								// subclasses should call inherited member function then
								// read the next index entry something like this:
								// 
								// RSearchCollection::ReadNextIndexEntry();
								// *theChunkStorage >> theNameOffset;
								// *theChunkStorage >> theDataOffset;
							}
							return TRUE;
						}

	virtual BOOLEAN MatchSearchCriteria()=0;
						// Uses the current RSearchCriteria object to determine if
						// the current item is a "hit". MatchSearchCriteria() is called
						// by the RCollectionSearcher object.

	BOOLEAN			AtEnd()
						// return TRUE if ReadNextEntry() was called when all entries
						// had already been read (one record read-ahead), else FALSE.
						{ return theAtEndFlag; }

	RChunkStorage*	ChunkStorage()
						// return pointer to the associated RChunkStorage
						{ return theChunkStorage; }

	BYTE*				NAMS()
						// return pointer to beginning of NAMS chunk in memory
						{ return theNAMS; }

	YStreamLength	NameOffset()
						// return relative offset from beginning of NAMS chunk in
						// collection file of the graphic name associated with
						// current index entry
						{ return theNameOffset; }

	YStreamLength	DataOffset()
						// return absolute offset into collection file of the data
						// (typically a graphic) associated with current index entry
						{ return theDataOffset; }

	uLONG				ID()
						// return graphic or quote & verse id associated with the
						// current index entry
						{ return theID; }

	void				SetShowNewestAtTop( BOOLEAN showNewestAtTop )
						// sets flag indicating whether the "show newest at top"
						// option is in effect
						{ theShowNewestAtTopFlag = showNewestAtTop; }

	BOOLEAN			ShowNewestAtTop()
						// return flag indicating whether the "show newest at top"
						// option is in effect
						{ return theShowNewestAtTopFlag; }

	RCollectionDate& Date()
						// return reference to the collection date associated with this
						// RSearchCollection
						{ return theDate; }

	static int		CompareItems( RSearchCollection* index1, RSearchCollection* index2 )
						// compare the current index entry of index1
						// with the current index entry of index2
						// return a value:
						//		<0 if index1 < index2
						//		=0 if index1 == index2
						//		>0 if index1 > index2
						//
						// Used by an RCollectionSearcher object for accessing index
						// entries from all online collection files in graphic name
						// order or, if "show newest at top" is in effect,
						// in date,graphic name order
						{
							if (!index1->AtEnd() && !index2->AtEnd())
							{
								int dateCompare;
								if (index1->ShowNewestAtTop())
									dateCompare = index1->Date().Compare( index2->Date() );
								else
									dateCompare = 0;
								if (dateCompare != 0)
									return -dateCompare;
								else
								{
#ifdef WIN
// GCB 4/25/97 - perform language-specific, case-insensitive name string comparison
									int nameCompare = SearchResult::CompareString(
										::GetUserDefaultLCID(),
										NORM_IGNORECASE + SORT_STRINGSORT,
										(const char *)index1->NAMS() + index1->NameOffset(),
										-1,
										(const char *)index2->NAMS() + index2->NameOffset(),
										-1 );
									TpsAssert( nameCompare > 0, "Error in CompareString" );
									return nameCompare - 2;
#else
// GCB 4/25/97 - strcmp doesn't work as well, especially for non-English languages
									return ::strcmp(
										(const char *)index1->NAMS() + index1->NameOffset(),
										(const char *)index2->NAMS() + index2->NameOffset() );
#endif
								}
							}
							else if (index1->AtEnd())
								return 1;
							else if (index2->AtEnd())
								return -1;
							else
								return 0;
						}

	virtual void	Validate() const {}

protected:
	RChunkStorage*		theChunkStorage;
	uLONG					theCurrentEntry;
	uLONG					theTotalEntries;
	YStreamOffset		theEntrySize;
	YStreamLength		theNameOffset;
	YStreamLength		theDataOffset;
	BYTE*					theNAMS;
	BOOLEAN				theAtEndFlag;
	uLONG					theID;
	RCollectionDate	theDate;
	BOOLEAN				theShowNewestAtTopFlag;

private:
	void				GetCollectionDate()
						// seek to CLHD chunk and read collection date into theDate
						// throws exception if CLHD chunk can't be found or on a read error.
						{
							theChunkStorage->SelectRootChunk();
							RChunkStorage::RChunkSearcher yIter = theChunkStorage->Start( 'CLHD', FALSE );
							if (yIter.End())
								throw kBadSeek;
							else
							{
								struct CollectionHeader header;
								theChunkStorage->SeekAbsolute( 0 );
								YStreamLength yChunkSize = theChunkStorage->GetCurrentChunkSize();
								if (yChunkSize >= sizeof(CollectionHeader))
								{
									theChunkStorage->Read( sizeof(CollectionHeader), (PUBYTE)&header );
									// if the collection file's "first use" date is
									// uninitialized, set it to today's date, making it
									// appear at the top in the browser when set to
									// Show Newest At Top
									if (header.yFirstUseDateTimeStamp[0] == NULL)
									{
										theDate.GetCurrentDate( header.yFirstUseDateTimeStamp );
										theChunkStorage->SeekAbsolute( 0 );
										try
										{
#if 1 // REVIEW GCB 3/27/98 - enable when Eric has RChunkStorage support for kReadWrite
											theChunkStorage->PreserveFileDate( FALSE );
											theChunkStorage->Write( sizeof(CollectionHeader), (PUBYTE)&header );
#endif
										}
										catch (...)
										{
											TpsAssertAlways( "Couldn't write current date to collection file with uninitialized date - locked file or volume?!" );
										}
									}
									theDate.SetDate( header.yFirstUseDateTimeStamp );
								}
							}
						}

	void				CacheNamesChunk()
						// if NAMS chunk has not already been cached,
						// seek to beginning of NAMS chunk and cache into memory in theNAMS
						// throws exception if NAMS chunk can't be found or on a read error.
						{
							if (!theNAMS)
							{
								theChunkStorage->SelectRootChunk();
								RChunkStorage::RChunkSearcher yIter = theChunkStorage->Start( 'CONT', FALSE );
								if (yIter.End())
									throw kBadSeek;
								else
								{
									yIter = theChunkStorage->Start( 'NAMS', FALSE );
									if (yIter.End())
										throw kBadSeek;
									else
									{
										YStreamLength yBufferSize = theChunkStorage->GetCurrentChunkSize();
										theNAMS = new BYTE[yBufferSize];
										theChunkStorage->SeekAbsolute( 0 );
										theChunkStorage->Read( yBufferSize, theNAMS );
									}
								}
							}
						}
};

#endif	// _SearchCollection_H_
