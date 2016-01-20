// ****************************************************************************
//
//  File Name:			CollectionSearcher.h
//
//  Project:			Renaissance
//
//  Author:				Greg Beddow
//
//  Description:		Declaration of RCollectionSearcher template class
//							that searches across all collection files in an
//							RCollectionArray returned from an RCollectionManager
//
//  Portability:		Cross platform
//
//  Developed by:		Broderbund Software, Inc.         
//
//  Copyright (C) 1997 Broderbund Software, Inc. All Rights Reserved.
//
// ****************************************************************************

#ifndef _CollectionSearcher_H_
#define _CollectionSearcher_H_


#include "ChunkyStorage.h"			// RChunkStorage class
#include "Array.h"					// RArray class
#include "CollectionManager.h"	// RCollectionManager & related
#include "Resource.h"				// alert string defines
#include "SearchResult.h"

class RSearchCriteria;

// maximum expected entries produced by RCollectionSearcher::SearchCollections()
// adjust as necessary for best memory/performance tradeoff
// (does not imply a size that can't be exceeded, just the allocation size)
const	uLONG		kMaxExpectedHits	= 9000;

#ifdef WIN
	// Custom message sent from RCollectionSearcher to
	// CDialog object with seach context information to update
	// the browser user interface
	//
	#define WM_UPDATESEARCHUI	(WM_USER + 100)
#endif // WIN


template <class T> class RCollectionSearcher : public RObject
{
public:
						RCollectionSearcher( RCollectionArray& collectionArray )
							:	theCollectionArray( collectionArray ),
								cancelSearch( FALSE ),
#ifdef WIN
								m_pUIWnd( NULL ),
#endif
								theSearchResultArray( sizeof(SearchResult) * kMaxExpectedHits )
						{
							// initialize each element of theIndexArray to reference
							// a collection file in theCollectionArray
							RCollectionArrayIterator collectionIter( theCollectionArray.Start() );
							RCollectionArrayIterator collectionIterEnd( theCollectionArray.End() );
							try
							{
								for (; collectionIter != collectionIterEnd; collectionIter++)
									theIndexArray.InsertAtEnd( new T( *collectionIter ) );
							}
							catch (...)
							{
								RAlert alert;
								alert.AlertUser( STRING_ERROR_CORRUPT_COLLECTION_FILE );
							}
						}

	virtual			~RCollectionSearcher()
						{
							cancelSearch = TRUE; // in case search is in progress

							RIndexArrayIterator indexIter( theIndexArray.Start() );
							RIndexArrayIterator indexIterEnd( theIndexArray.End() );
							for (; indexIter != indexIterEnd; indexIter++)
								delete *indexIter;

                     theSearchResultArray.Empty();
						}

#ifdef WIN
	void				SetCWndForUI( CWnd * pWnd )
						{
							if (pWnd && IsWindow( pWnd->m_hWnd ))
								m_pUIWnd = pWnd;
							else
								m_pUIWnd = NULL;
						}
#endif	

	void				SetSearchCriteria( const RSearchCriteria* aSearchCriteria )
						{
							RIndexArrayIterator indexIter( theIndexArray.Start() );
							RIndexArrayIterator indexIterEnd( theIndexArray.End() );
							for (; indexIter != indexIterEnd; indexIter++)
								(*indexIter)->SetSearchCriteria( aSearchCriteria );
						}

	uLONG				GetTotalNumItems()
						// Returns the total number of items across all collections
						// matching aSearchCriteria set by SetSearchCriteria()
						{
							uLONG nTotal = 0;
							RIndexArrayIterator indexIter( theIndexArray.Start() );
							RIndexArrayIterator indexIterEnd( theIndexArray.End() );
							for (; indexIter != indexIterEnd; indexIter++)
							{
								try
								{
									nTotal += (*indexIter)->GetNumItems();
								}
								catch (...)
								{
									RAlert alert;
									alert.AlertUser( STRING_ERROR_CORRUPT_COLLECTION_FILE );
								}
							}
							return nTotal;
						}

	void				SetShowNewestAtTop( BOOLEAN showNewestAtTop )
						{
							RIndexArrayIterator indexIter( theIndexArray.Start() );
							RIndexArrayIterator indexIterEnd( theIndexArray.End() );
							for (; indexIter != indexIterEnd; indexIter++)
								(*indexIter)->SetShowNewestAtTop( showNewestAtTop );
						}

	RSearchResultArray* SearchCollections( BOOL async = FALSE )
						// search all collection files referenced by this
						// RCollectionSearcher for index entries that match
						// aSearchCriteria set by SetSearchCriteria();
						// return all matching entries
						// if async==TRUE, this will eventually do an asynchronous search
						// but for now it returns RSearchResultArray* without searching
						{
							theSearchResultArray.Empty();
							if (!async && theIndexArray.Count() > 0)
							{
								BOOL bMatch;
								uLONG	nNumHits			= 0;
								uLONG	nNumConsidered = 0;
								T* index;
								SearchResult searchResult;

								try
								{
									// for each index entry in each element of theIndexArray...
									for (BOOLEAN success = InitSearch( index );
										success;
										success = ReadNextIndexEntry( index ))
									{
										nNumConsidered++;

										// if current index entry matches searchCriteria...
										bMatch = index->MatchSearchCriteria();
										if (bMatch)
										{
											// convert current index entry to searchResult
											// and add to theSearchResultArray
											searchResult.collectionStorage = index->ChunkStorage();
											searchResult.dataOffset = index->DataOffset();
											searchResult.namePtr = index->NAMS() + index->NameOffset();
											searchResult.id = index->ID();
											theSearchResultArray.InsertAtEnd( searchResult );
											nNumHits++;
										}
										#ifdef WIN
										if (m_pUIWnd && (bMatch || (nNumConsidered%50==0)))
											m_pUIWnd->SendMessage( WM_UPDATESEARCHUI, nNumHits, nNumConsidered );
										#endif
									}
								}

								catch( YException exception )
								{
									::ReportException( exception );
								}
							}
							return &theSearchResultArray;
						}

	void				CancelSearch()
						// cancel the asynchronous search that was begun earlier by
						// calling SearchCollections( TRUE )
						{ cancelSearch = TRUE; }

	virtual void	Validate() const {}

private:
	BOOLEAN			InitSearch( T*& index )
						// initialize each element in theIndexArray for searching
						// and read the first index entry into each (one record read-ahead)
						// return in index the element from theIndexArry that contains
						// the first index entry to consider
						// returns FALSE if all collection files are empty or if a read
						// error occurred, else returns TRUE
						{
							cancelSearch = FALSE;
							if (theIndexArray.Count() == 0)
								return FALSE;
							RIndexArrayIterator indexIter( theIndexArray.Start() );
							RIndexArrayIterator indexIterEnd( theIndexArray.End() );
							for (; indexIter != indexIterEnd; indexIter++)
							{
								try
								{
									(*indexIter)->InitSearch();
									(*indexIter)->ReadNextIndexEntry();
								}
								catch (...)
								{
									RAlert alert;
									alert.AlertUser( STRING_ERROR_CORRUPT_COLLECTION_FILE );
								}
							}
							SortIndexArray();
							index = theIndexArray[0];
							return static_cast<BOOLEAN>(!index->AtEnd());
						}


	BOOLEAN			ReadNextIndexEntry( T*& index )
						// return in index the element from theIndexArry that contains
						// the next index entry to consider, reading the next index entry
						// into one or more elements of theIndexArray as necessary
						// returns TRUE if there are more index entries to be returned
						// returns FALSE when all index entries of all elements of
						// theIndexArray have been read and returned or if a read
						// error occurred
						// the ReadNextIndexEntry member function of class T should
						// throw an exception if a read error occured
						{
							if (cancelSearch || theIndexArray.Count() == 0)
								return FALSE;
							index = theIndexArray[0];
							BOOLEAN success = TRUE;
							try
							{
								index->ReadNextIndexEntry();
							}
							catch (...)
							{
								RAlert alert;
								alert.AlertUser( STRING_ERROR_CORRUPT_COLLECTION_FILE );
								success = FALSE;
							}
							if (success)
							{
								SortIndexArray();
								index = theIndexArray[0];
								if (index->AtEnd())
									success = FALSE;
							}
							return success;
						}

	void				SortIndexArray()
						{
							::Sort( theIndexArray.Start(), theIndexArray.End(), T::CompareItems );
						}

	RCollectionArray&		theCollectionArray;
	RSearchResultArray	theSearchResultArray;

	typedef	RArray<T*>						RIndexArray;
	typedef	RArray<T*>::YIterator		RIndexArrayIterator;

	RIndexArray				theIndexArray;

	BOOL						cancelSearch;

#ifdef WIN
	CWnd*						m_pUIWnd;	// CWnd object to receive WM_UPDATESEARCHUI message
#endif	
};


#endif	// _CollectionSearcher_H_
