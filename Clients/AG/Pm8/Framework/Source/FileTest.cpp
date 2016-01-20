// ****************************************************************************
//
//  File Name:			FileTest.cpp
//
//  Project:			FileTest
//
//  Author:				E. VanHelene
//
//  Description:		Declaration of main program to test streams
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
//  $Logfile:: /PM8/Framework/Source/FileTest.cpp           $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:17p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

//	Implementation Notes
//		Test the stream classes by filling blocks of memory with their offset in the
//		stream. So if the stream offset size is 4 bytes then a sample value would
//		would be '00040008000C...'. We will then know what every byte in the file
//		should be so we can verify all file accesses in place.

#include "stdafx.h"
#include "afxcmn.h"
#include "FrameworkIncludes.h"

#define kReReadChunkTest
//#undef kReReadChunkTest

ASSERTNAME

#include <time.h>
#include "FileTest.h"
#include "FileStream.h"
#include "MemoryStream.h"
#include "ChunkyStorage.h"
#include "ChunkyBackstore.h"
#include "RChunkDisplay.h"
#include "script.h"
#include "streamscript.h"
//#include "color.h"

//	local fcns 
void WriteFileEntry( RChunkStorage* pParent, RMBCString& rName );
void WriteDirectoryEntry( RChunkStorage* pParent, RMBCString& rName );
void TestSpecialChunkTree( RChunkStorage& rClipTest );

//	The following stub definitions allow us to not include large sections of the framework:
RColor::~RColor(){}
RColor::RColor(const RSolidColor&){}
RSolidColor::RSolidColor(enum EColors){}

// ****************************************************************************
//
// The following fcns support file testing. Test data is per the implementation 
//		notes above
//
// ****************************************************************************
//
// ****************************************************************************
//
// Function Name:		GetYBuffSize( YStreamLength yBuffSize )
//
// Description:		get size of buffer to use when writing test data 
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
YStreamLength GetYBuffSize( YStreamLength yBuffSize )
	{
	YStreamLength ysBase = yBuffSize/sizeof(YStreamPosition);
	YStreamLength ysExtra = (yBuffSize%sizeof(YStreamPosition))?1:0;
	return ysBase + ysExtra;
	}

// ****************************************************************************
//
// Function Name:		GetYBuffStart( YStreamLength yBuffSize )
//
// Description:		 
//							figure out the start of the data to write (in the buffer which has been
//							expanded to an integral number of YStreamPosition values)
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
YStreamPosition GetYBuffStart( YStreamPosition yBuffOffset )
	{
	return yBuffOffset%sizeof(YStreamPosition);
	}


// ****************************************************************************
//
// Function Name:		FileVerify( RStream& rfTest )
//
// Description:		scan test file; verify that it contains the correct contents
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void FileVerify( RStream& rfTest )
	{
	YStreamPosition yPosOld = rfTest.GetPosition();
	YStreamPosition yPos;
	YStreamPosition yData;
	YStreamLength yDataSize = GetYBuffSize(rfTest.GetSize()) * sizeof(yData);
	YStreamLength yReadSize = sizeof(yData);

	try
		{
#ifndef TESTMEMORYMAPPED
		rfTest.Flush();
#endif
		//	scan entire file; verify that it contains the correct contents
		rfTest.SeekAbsolute( 0 );
		for( yPos = 0; yPos < (yDataSize-sizeof(yData)); yPos += sizeof(yData))
			{
			yData = ~yData;	//	clear buffer from last read if any
			rfTest.Read( yReadSize, (LPUBYTE)&yData );
			TpsAssert( yData == yPos, "file verify failed" );
			TpsAssert( !rfTest.AtEnd(), " atend failed" );
			}
		
			//check the left over bytes one by one in case there are not sizeof YStreamPosition of them
		if (0 < (rfTest.GetSize() - yPos))
			{
			DWORD dwError = GetLastError( );
			uBYTE ubData;
			uBYTE ubPos;
			YStreamPosition i;
			yReadSize = rfTest.GetSize() - yPos;
			rfTest.Read( yReadSize, (LPUBYTE)&yData );
			for( i=0; i < yReadSize; i+=sizeof(uBYTE))
				{
				ubData = static_cast<uBYTE>(yData<<i);
				ubPos = static_cast<uBYTE>(yPos<<i);
				TpsAssert( ubPos == ubData, "file verify failed" );
				}
			}
		TpsAssert( rfTest.AtEnd(), " atend failed" );							//	always at the end of an empty file

		//	restore file pos
		rfTest.SeekAbsolute( yPosOld );
	}
catch(...)
	{
	TpsAssert( FALSE, "unhandled exception" );
	SetLastError(NO_ERROR);
	}
}


// ****************************************************************************
//
// Function Name:		WriteTest( RStream& rfTest, const YStreamPosition yPlaceToWrite, const YStreamLength yBytesToWrite, BOOLEAN fTestWrite )
//
// Description:		write a buffer to the file; fill it with its offsets
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void WriteTest( RStream& rfTest, const YStreamPosition yPlaceToWrite, const YStreamLength yBytesToWrite, BOOLEAN fTestWrite )
	{
	YStreamLength yBuffSize = GetYBuffSize(yBytesToWrite);
	YStreamPosition* pyBuff = new YStreamPosition[yBuffSize];
	YStreamPosition yPos;
	YStreamPosition yPlaceToWriteSave = yPlaceToWrite;

	//	fill buffer with the offsets where these values will live in the file
	for(yPos = 0; yPos < yBuffSize; yPos++)
		{
		pyBuff[yPos] = (yPlaceToWrite + yPos) * sizeof(YStreamPosition);
		}
	
	//	place the data in the stream
	rfTest.SeekAbsolute( yPlaceToWrite );
	rfTest.Write( yBytesToWrite, (LPUBYTE)(pyBuff + GetYBuffStart(yPlaceToWrite)) );
	
	//	restore stream pointer
	rfTest.SeekAbsolute( yPlaceToWriteSave );

	//	make sure the file was written correctly
	//	cant do this if were in write only mode; I assume
	//	we will verify later
	if ( fTestWrite )
		FileVerify(rfTest);

	delete[] pyBuff;
	}



// ****************************************************************************
//
// Function Name:		TestEmpty ( RFileStream& rfTest, EAccessMode eOpenMode )
//
// Description:		perform empty stream tests
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void TestEmpty ( RFileStream& rfTest, EAccessMode eOpenMode )
	{
			//	reopen existing empty file new access mode
	switch( eOpenMode )
		{
		case kRead: rfTest.Open( "e:\\Temp\\test3.bin", kRead );						//	try new empty file 
			break;
		case kWrite: rfTest.Open( "e:\\Temp\\test3.bin", kWrite );						//	try new empty file 
			break;
		case kReadWrite: rfTest.Open( "e:\\Temp\\test3.bin", kReadWrite );						//	try new empty file 
			break;
		default:
			TpsAssert( FALSE, "invalid access mode" );
		}

	rfTest.Flush();																	//	flush empty file
	TpsAssert( rfTest.AtEnd(), " atend failed" );							//	always at the end of an empty file
	rfTest.SeekRelative(0);															//	SeekRelative empty file
	rfTest.SeekAbsolute(0);															//	SeekAbsolute empty file
	TpsAssert( 0 == rfTest.GetPosition(), "getposition failed" );		//	getposition empty file
	rfTest.Close();																	//	close empty file
	}


// ****************************************************************************
//
// Function Name:		TestSeekError ( RStream& rfTest, EAccessMode eOpenMode )
//
// Description:		verify that seek errors occur for the following operations
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void TestSeekError ( RStream& rfTest, EAccessMode eOpenMode )
	{
	switch( eOpenMode )
		{
		case kRead: rfTest.Open( kRead );				
			break;
		case kWrite: rfTest.Open( kWrite );				
			break;
		case kReadWrite: rfTest.Open( kReadWrite );	
			break;
		default:
			TpsAssert( FALSE, "invalid access mode" );
		}

	try
		{
		rfTest.SeekAbsolute(0);
		rfTest.SeekRelative(-1);													//	seek relative to before beginning of file
		TpsAssert( FALSE, "Exception not thrown on error" );
		}
	catch( YException eTest )
		{
		TpsAssert( kBadSeek == eTest, "unhandled exception" );
		SetLastError(NO_ERROR);
		}

	try
		{
		rfTest.SeekAbsolute(0);
		rfTest.SeekRelative(0xFFFFFF);											//	seek relative to after end of file
		TpsAssert( FALSE, "Exception not thrown on error" );
		}
	catch( YException eTest )
		{
		TpsAssert( kBadSeek == eTest, "unhandled exception" );
		SetLastError(NO_ERROR);
		}
	
	try
		{
		rfTest.SeekAbsolute(0xFFFFFF);											//	seek absolute after eof
		TpsAssert( FALSE, "Exception not thrown on error" );
		}
	catch( YException eTest )
		{
		TpsAssert( kBadSeek == eTest, "unhandled exception" );
		SetLastError(NO_ERROR);
		}

	rfTest.Close();																	//	close empty file
	}



// ****************************************************************************
//
// The following fcns support chunk testing. Test data is a directory tree 
//		for specified volumes. Testing proceeds by recursively writing chunks
//		for files and directories on specified volume
//
// ****************************************************************************
//
// ****************************************************************************
//
// Function Name:		WriteDirectoryEntry( RChunkStorage* pParent, RMBCString& rName )
//
// Description:		recursively write a chunk for the given directory and its children
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void WriteDirectoryEntry( RChunkStorage* pParent, RMBCString& rName )
	{
	TRACE( "D: %s\n", rName );
	RChunkStorage* pCurSelection = pParent->GetSelectedChunk();

	//	note that exceptions thrown during testing cause leaks!
	//	this is testing code not production code

	//	write our header and add ourselves to the index in our parent's chunk
	YChunkId yDirID = pParent->AddSubChunk( (EChunkType)'DIRE' );	

	//	write our children
	//	for each entry in this directory 
	//	write a new chunk for each file and directory encountered
	WIN32_FIND_DATA searchData;
	HANDLE hFile = NULL;
	BOOLEAN fFound = TRUE;
	RMBCString rChild;
	RMBCString rChildUse;

	//	loop thru files at this level
	hFile = ::FindFirstFile( rName, &searchData );
	
	//	get the dir name if the dir exists
	if (INVALID_HANDLE_VALUE != hFile)
		{
		char dBuff[250];

		::GetCurrentDirectory( 250, dBuff );
		::SetLastError( NO_ERROR );				//	we dont care about an error here
		rName = dBuff;
		}

	for(	;
		(INVALID_HANDLE_VALUE != hFile) && fFound;
		fFound = static_cast<BOOLEAN>(::FindNextFile( hFile, &searchData )))
		{
		rChild = searchData.cFileName;

		if ( !rChild.Compare(".") || !rChild.Compare("..") )
			continue;	//	skip the directory references

		if (searchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			rChildUse = "a:";
			rChildUse += rChild;

			fFound = static_cast<BOOLEAN>(::SetCurrentDirectory( rChildUse ));	//	step down to child
			if ( !fFound )
				ThrowFileExceptionIfError();

			rChildUse = "*.*";
			WriteDirectoryEntry( pParent, rChildUse );	//	recur on child
			
			fFound = static_cast<BOOLEAN>(::SetCurrentDirectory( __TEXT("..") ));	//	return to parent
			if ( !fFound )
				ThrowFileExceptionIfError();
			}
		else
			WriteFileEntry( pParent, rChild );
		}
	::SetLastError( NO_ERROR );				//	we dont care about an error here

	//	write our data
	//	write the dir name into the dir 
	pParent->WriteAuxData( rName.GetDataLength(), (LPCUBYTE)rName );
	YChunkId yDirNameID = pParent->AddChunk( (EChunkType)'NAME' );
	rName += '\0';
	pParent->Write( rName.GetDataLength(), (LPCUBYTE)rName );
	
	if ( ::GetLastError( ) == ERROR_NO_MORE_FILES )
		::SetLastError( NO_ERROR );

	if (INVALID_HANDLE_VALUE != hFile)
		{
		if ( !::FindClose( hFile ) )
			ThrowFileExceptionIfError();
		}

	pParent->SetSelectedChunk( pCurSelection );	//	restore selected chunk
	}

// ****************************************************************************
//
// Function Name:		WriteFileEntry( RChunkStorage* pParent, RMBCString& rName )
//
// Description:		write a chunk for the given file
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void WriteFileEntry( RChunkStorage* pParent, RMBCString& rName )
	{
	RChunkStorage* pCurSelection = pParent->GetSelectedChunk();

	TRACE( "F: %s\n", rName );

	//	write our header and add ourselves to the index in this chunk
	YChunkId yFileID = pParent->AddChunk( (EChunkType)'FILE' );	

	//	write any additional data
	//	write our name
	rName += '\0';
	pParent->WriteAuxData( rName.GetDataLength(), (LPCUBYTE)rName );
	YChunkId yFileNameID = pParent->AddChunk( (EChunkType)'NAME' );
	pParent->Write( rName.GetDataLength(), (LPCUBYTE)rName );

	pParent->SetSelectedChunk( pCurSelection );	//	restore selected chunk
	}


// ****************************************************************************
//
// Function Name:		TestSpecialChunkTree( RChunkStorage& rClipTest )
//
// Description:		verify that the given chunk tree is in the format of 
//							the chunk storage kTEST_24_NAME created below.
//
// Returns:				nothing
//
// Exceptions:			file
//
// ****************************************************************************
//
void TestSpecialChunkTree( RChunkStorage& rClipTest )
	{
		//	try creating a chunk storage from a given memory buffer (used to support clipboard transfers)
		YChunkStorageId pId = rClipTest.GetSelectedChunk();	//	remember the root
		YStreamLength yBufferSize;
		PUBYTE pubSrcBuffer;
		uBYTE pubBuffer[60];

		//	Try creating a chunk storage from a retrieved buffer.
		pubSrcBuffer = rClipTest.GetCurrentChunkBuffer( yBufferSize );
		RChunkStorage rClipboardTest1( yBufferSize, pubSrcBuffer );
		rClipTest.ReleaseBuffer( pubSrcBuffer );
		RChunkStorage::RChunkSearcher rSrch;
		rSrch = rClipboardTest1.Start('hith', !kRecursive );
		rClipboardTest1.SeekAbsolute( 0 );
		rClipboardTest1.Read( 60, pubBuffer );
		TpsAssert( !memcmp( pubBuffer, (PUBYTE)"a123456789b123456789c123456789d123456789e123456789f123456789", 60 ), "clipboard support test failed" );

		//	try again with an internal node
		//	select a file chunk inside given doc
		rSrch = rClipTest.Start('hit2', kRecursive );
//		rSrch = rClipTest.Start('hith', !kRecursive );
//		rSrch = rClipTest.Start('hit2', !kRecursive );
		
		//	Try getting the data buffer.
		pubSrcBuffer = rClipTest.GetCurrentChunkDataBuffer( yBufferSize );
		TpsAssert( 30 == yBufferSize, "returned buffer wrong size" );
		TpsAssert( !memcmp( pubSrcBuffer, (PUBYTE)"g123456789h123456789i123456789", 30 ), "clipboard support test failed" );
		rClipTest.ReleaseBuffer( pubSrcBuffer );

		//	Test the searching fcns.
		//	Start each search at the root.

		//	Change the versions of some chunks
		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit4', kRecursive );
		rClipTest.GetCurrentChunkMinDataVersion() = (YVersion)2;

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit6', kRecursive );
		rClipTest.GetCurrentChunkMinDataVersion() = (YVersion)2;

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit7', kRecursive );
		rClipTest.GetCurrentChunkMinDataVersion() = (YVersion)2;

		//	Try recursive searches		
		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit2', kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit3', kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		RChunkStorage::RChunkSearchParams rParams( kRecursive, kSearchTag, 'hit4', !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit5', kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rParams = RChunkStorage::RChunkSearchParams( kRecursive, kSearchTag, 'hit6', !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rParams = RChunkStorage::RChunkSearchParams( kRecursive, kSearchTag, 'hit7', !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Recursive search failed" );

		//	Try non-recursive searches		
		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit2', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit3', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );
		rParams = RChunkStorage::RChunkSearchParams( !kRecursive, kSearchTag, 'hit4', !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit3', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit5', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit3', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );
		rParams = RChunkStorage::RChunkSearchParams( !kRecursive, kSearchTag, 'hit6', !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit3', !kRecursive );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );
		rParams = RChunkStorage::RChunkSearchParams( !kRecursive, kSearchTag, 'hit7', !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetSelectedChunk() != NULL, "Non-Recursive search failed" );

		//	Try another test looking for each of the version 2 chunks in order.
		rClipTest.SetSelectedChunk( pId );
		rParams = RChunkStorage::RChunkSearchParams( kRecursive, !kSearchTag, kTagDontCare, !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetCurrentChunkMinDataVersion() == 2
			, "recursive versioned chunk failed" );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit6'
			, "recursive versioned chunk failed" );

		++rSrch;
		TpsAssert( rClipTest.GetCurrentChunkMinDataVersion() == 2
			, "recursive versioned chunk failed" );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit7'
			, "recursive versioned chunk failed" );

		++rSrch;
		TpsAssert( rClipTest.GetCurrentChunkMinDataVersion() == 2
			, "recursive versioned chunk failed" );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit4'
			, "recursive versioned chunk failed" );
	}

//	main test routine
#undef TESTERRORS
#define kTEST1SIZE 100001
#define kTEST_OUT_NAME RMBCString( "e:\\Temp\\FileTest\\testout.txt" )
#define kTEST_1_NAME RMBCString( "e:\\Temp\\FileTest\\test1.bin" )
#define kTEST_6_NAME RMBCString( "e:\\Temp\\FileTest\\test6.bin" )
#define kTEST_7_NAME RMBCString( "e:\\Temp\\FileTest\\test7.bin" )
#define kTEST_8_NAME RMBCString( "e:\\Temp\\FileTest\\test8.bin" )
#define kTEST_9_NAME RMBCString( "e:\\Temp\\FileTest\\testDoc9.bin" )
#define kTEST_10_NAME RMBCString( "e:\\Temp\\FileTest\\testDoc10.bin" )
#define kTEST_11_NAME RMBCString( "e:\\Temp\\FileTest\\testDoc11.bin" )
#define kTEST_12_NAME RMBCString( "e:\\Temp\\FileTest\\testDoc12.bin" )
#define kTEST_13_NAME RMBCString( "e:\\Temp\\FileTest\\testDoc13.bin" )
#define kTEST_14_NAME RMBCString( "e:\\Temp\\FileTest\\testDoc14.bin" )
#define kTEST_15_NAME RMBCString( "e:\\Temp\\FileTest\\testDoc15.bin" )
#define kTEST_16_NAME RMBCString( "e:\\Temp\\FileTest\\testDoc16.bin" )
#define kTEST_17_NAME RMBCString("e:\\Temp\\FileTest\\testDoc17.bin" )
#define kTEST_18_NAME RMBCString("e:\\Temp\\FileTest\\testDoc18.bin" )
#define kTEST_19_NAME RMBCString("e:\\Temp\\FileTest\\testDoc19.bin" )
#define kTEST_20_NAME RMBCString("e:\\Temp\\FileTest\\testDoc20.bin" )
#define kTEST_21_NAME RMBCString("e:\\Temp\\FileTest\\testDoc21.bin" )
#define kTEST_22_NAME RMBCString("e:\\Temp\\FileTest\\testDoc22.bin" )
#define kTEST_23_NAME RMBCString("e:\\Temp\\FileTest\\testDoc23.bin" )
#define kTEST_24_NAME RMBCString("e:\\Temp\\FileTest\\testDoc24.bin" )
#define kTEST_25_NAME RMBCString("e:\\Temp\\FileTest\\testDoc25.bin" )
#define kTEST_26_NAME RMBCString("e:\\Temp\\FileTest\\testDoc26.bin" )
#define kTEST_27_NAME RMBCString("e:\\Temp\\FileTest\\testDoc27.bin" )
#define kTEST_28_NAME RMBCString("e:\\Temp\\FileTest\\testDoc28.bin" )
#define kTEST_29_NAME RMBCString("e:\\Temp\\FileTest\\testDoc29.bin" )
#define kTEST_30_NAME RMBCString("e:\\Temp\\FileTest\\testDoc30.bin" )
#define kTEST_1_JUNK_NAME RMBCString("e:\\sdfsdf\\asdfasdf\\sdfsadfsdfas.bin" )



// ****************************************************************************
//
// The following code drives the testing process 
//		The preceding fcns are called to exercise the stream code.
//
// ****************************************************************************
//
// ****************************************************************************
//
// Function Name:		MainTest()
//
// Description:		test the file routines
//
// Returns:				nothing
//
// Exceptions:			none
//
// ****************************************************************************
//
void MainTest()
{
	//	clean up from prior test
	SetFileAttributes( kTEST_OUT_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_OUT_NAME );
	SetFileAttributes( kTEST_1_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_1_NAME );
	SetFileAttributes( kTEST_6_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_6_NAME );
	SetFileAttributes( kTEST_7_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_7_NAME );
	SetFileAttributes( kTEST_8_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_8_NAME );
#ifdef kReReadChunkTest
	SetFileAttributes( kTEST_9_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_9_NAME );
#endif
	SetFileAttributes( kTEST_10_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_10_NAME );
	SetFileAttributes( kTEST_11_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_11_NAME );
	SetFileAttributes( kTEST_12_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_12_NAME );
	SetFileAttributes( kTEST_13_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_13_NAME );
	SetFileAttributes( kTEST_14_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_14_NAME );
	SetFileAttributes( kTEST_15_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_15_NAME );
	SetFileAttributes( kTEST_16_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_16_NAME );
	SetFileAttributes( kTEST_17_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_17_NAME );
	SetFileAttributes( kTEST_18_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_18_NAME );
	SetFileAttributes( kTEST_19_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_19_NAME );
	SetFileAttributes( kTEST_20_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_20_NAME );
	SetFileAttributes( kTEST_21_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_21_NAME );
	SetFileAttributes( kTEST_22_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_22_NAME );
	SetFileAttributes( kTEST_23_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_23_NAME );
	SetFileAttributes( kTEST_24_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_24_NAME );
	SetFileAttributes( kTEST_25_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_25_NAME );
	SetFileAttributes( kTEST_26_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_26_NAME );
	SetFileAttributes( kTEST_27_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_27_NAME );
	SetFileAttributes( kTEST_28_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_28_NAME );
	SetFileAttributes( kTEST_29_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_29_NAME );
	SetFileAttributes( kTEST_30_NAME, !FILE_ATTRIBUTE_READONLY );
	DeleteFile( kTEST_30_NAME );

	try
		{
//		RFileStream rfReport( kTEST_OUT_NAME );
//		rfReport.Open(kReadWrite);

//		char s[255];
#ifdef TESTMEMORYMAPPED
//		wsprintf(s, "TEST MEMORY MAPPED\n");
//		rfReport.Write( strlen(s), (LPUBYTE)s );
#else
//		wsprintf(s, "TEST CSTDIO\n");
//		rfReport.Write( strlen(s), (LPUBYTE)s );
#endif
//		wsprintf(s, "Start test at %lu\n", clock());
//		rfReport.Write( strlen(s), (LPUBYTE)s );

		//	tests																			//	functionality tested

{

//	This code tests a problem reported by Michael Kelson at Broderbund.
//	It represents part of a proof of concept and not complete enough to be a diagnostic aid.
#if 0
        try
        {

enum ECollectionChunkType
{ 
          kCollectionHeader     ='CLHD'
        , kContentHeader        ='CONT'
        , kIndexHeader          ='LIST'
        , kGraphicHeader        ='GRHD'
        , kGraphicItem          ='GRAF'
        , kQVHeader             ='QVHD' // Quotes and Verses
        , kCategoryTable        ='CATG'
        , kNameTable            ='NAMS'
        , kKeywordTable         ='GKWC'
        , kGraphicListAll       ='IGAL'
        , kNameListAll          ='INAL'
        , kNameListSquare       ='INSQ'
        , kNameListRow          ='INRW'
        , kNameListColumn       ='INCO'
        , kNameListBitmap       ='INBM'
        , kNameListQV           ='INQV'
        , kCategoryListAll      ='ICAL'
        , kCategoryListSquare   ='ICSQ'
        , kCategoryListRow      ='ICRW'
        , kCategoryListColumn   ='ICCO'
        , kCategoryListBitmap   ='ICBM'
        , kGraphicListBackdrop  ='IGBK'
        , kCategoryListBackdrop ='ICBK'
        , kCategoryListBorder   ='ICBR'
        , kCategoryListLine     ='ICLN'
        , kCategoryListQV       ='ICQV'
        , kRelatedKeywordList   ='IRKW'
        , kRelatedKeywordOffsets ='RKWO'
};
#pragma pack(push, 1)
struct OFFSETS
{
   LONG  namsOFFSET;
   LONG  gkwcOFFSET;
   LONG  grafOFFSET;
};
#pragma pack(pop)
#pragma pack(push, 1)
struct CLHD
{
   LONG     nID;  
   LONG     nLocale;    
   short    sCurVer;
   short    sMinVer;
   short    sEncoding;
   short    sType;
   LONG     nCount;
   LONG     nUnused;
};
#pragma pack(pop)


class INAL 
   { 
   public: 
        unsigned short nCount; //       ???
        OFFSETS offset1;        //      ???
        OFFSETS offset2;        //      ???
        unsigned long ulPadding; //     ???
   };

TRACE("CHUNKY STORAGE %s\n", "g:\\two04091.grc");
		RMBCString rFilename("g:\\two04091.grc" );
      RChunkStorage * pChunkStorage = new RChunkStorage( rFilename, kRead );
                
      RChunkStorage::RChunkSearcher rSrch;
      PUBYTE           lpData;
      YStreamLength    yBufferSize;
                
      rSrch  = pChunkStorage->Start( 'INDX', !kRecursive );
      rSrch  = pChunkStorage->Start( 'INAL', !kRecursive );
		uBYTE pubBuffer[99];
		pChunkStorage->Read( 30, pubBuffer );

		YChunkId chunkID = pChunkStorage->GetCurrentChunkId();
/*
      pChunkStorage->SelectRootChunk();
      rSrch   = pChunkStorage->Start('CONT', !kRecursive);
      YChunkId chunkID = pChunkStorage->GetCurrentChunkId();

      rSrch = pChunkStorage->Start( kNameTable, chunkID, !kRecursive );
*/

      // Cache the INAL chunk into memory
      //
		pChunkStorage->SelectRootChunk();
		pChunkStorage->Start('INDX', !kRecursive);
		chunkID = pChunkStorage->GetCurrentChunkId();

		rSrch = pChunkStorage->Start( kNameListAll, chunkID, !kRecursive); 
//		rSrch = pChunkStorage->Start( kNameListAll, !kRecursive);

		if (!rSrch.End())
		{
			// lpData = pChunkStorage->GetCurrentChunkDataBuffer( yBufferSize ); 
			// lpINAL = new BYTE[yBufferSize];
			// memcpy( lpINAL, lpData, yBufferSize );

			yBufferSize = pChunkStorage->GetCurrentChunkDataSize(); 
			PUBYTE lpINAL = new   uBYTE[yBufferSize];
			pChunkStorage->Read( yBufferSize, lpINAL ); 
		}
		else
		{
			TRACE("INAL CHUNK NOT FOUND.\n");
		}


      pChunkStorage->SelectRootChunk();
      rSrch  = pChunkStorage->Start( kCollectionHeader, !kRecursive );

      int   nCount = 0;    // Count of collection entries
      
      if (!rSrch.End())
      {
         lpData = pChunkStorage->GetCurrentChunkDataBuffer( yBufferSize );
         CLHD * pHeader = (CLHD *) lpData;
         nCount = pHeader->nCount;
      }
      else
      {
         TRACE("CLHD CHUNK NOT FOUND.\n");
      }

      INAL* lpINAL = NULL;
      PUBYTE lpNAMS = NULL;

      // Cache the INAL chunk into memory
      //
      pChunkStorage->SelectRootChunk();
      pChunkStorage->Start('INDX', !kRecursive);
      chunkID = pChunkStorage->GetCurrentChunkId();

      rSrch = pChunkStorage->Start( kNameListAll, chunkID, !kRecursive );

      if (!rSrch.End())
      {
         lpData = pChunkStorage->GetCurrentChunkDataBuffer( yBufferSize );
         lpINAL = (INAL*)new uBYTE[yBufferSize];
         memcpy( lpINAL, lpData, yBufferSize );
      }
      else
      {
         TRACE("INAL CHUNK NOT FOUND.\n");
      }

      // Cache the NAMS chunk into memory
      //
      pChunkStorage->SelectRootChunk();
      rSrch   = pChunkStorage->Start('CONT', !kRecursive);
      chunkID = pChunkStorage->GetCurrentChunkId();

      rSrch = pChunkStorage->Start( kNameTable, chunkID, !kRecursive );

      if (!rSrch.End())
      {
         lpData = pChunkStorage->GetCurrentChunkDataBuffer( yBufferSize );
         lpNAMS = new uBYTE[yBufferSize];
         memcpy( lpNAMS, lpData, yBufferSize );
      }
      else
      {
         TRACE("NAMS CHUNK NOT FOUND.\n");
      }

      // Now iterate through the INAL and NAMS chunks
      //
      if (lpINAL && lpNAMS)
      {
         INAL* pInal = lpINAL;

         short    sCount = *((short *) pInal);
         pInal += sizeof(short);

         TRACE("INAL COUNT: %d\n", sCount);

// sCount = 2 ; 2 bytes
//	next find 2 blocks of 3 4 byte offsets         
for (int i=0; i < sCount; i++)
         {
            OFFSETS  * pOffsets;
            pOffsets = (OFFSETS *) pInal;
            
            TRACE("Graphic %d\n", i);
            TRACE("   NAMS offset: %x\n", pOffsets->namsOFFSET);
            TRACE("   GKWC offset: %x\n", pOffsets->gkwcOFFSET);
            TRACE("   GRAF offset: %x\n", pOffsets->grafOFFSET);

            pInal += sizeof(OFFSETS);
         }
      }

      delete [] lpINAL;
      delete [] lpNAMS;
      delete    pChunkStorage;

/*
                rSrch = pGCFile->m_pRCollectionStorage->Start( 
kGraphicHeader, !kRecursive );
                
                for ( rSrch = pGCFile->m_pRCollectionStorage->Start( 
kGraphicItem, !kRecursive );
                                !rSrch.End(); rSrch ++ )
                {
                        (*rSrch).SeekAbsolute( 0 );

                        pGCItem = new RGraphicCollectionItem;
                        if ( pGCItem )
                        {
                                pGCItem->yCollectionID = 
aCurrentCollectionID;
                                pGCItem->yOffset = 
(*rSrch).GetAbsolutePositionInStream( );

                                m_GCItemList.InsertAtEnd( pGCItem );
                        }
                }
*/

        }

        catch( CMemoryException )
        {
                ::MessageBox( NULL, "Out of memory", "Error", MB_OK );
        }

        catch( char* s )
        {
                TpsAssert( NULL, "Who threw this???" );
        }

        catch( CFileException )
        {
                ::MessageBox( NULL, "Error reading file.", "Error", MB_OK 
);
        }
#endif
}		
		
		
		//	Test RFileStream
		YStreamLength ysl;
	{
	{
		//	perform basic open/write/read test
		{
		RFileStream ftTest( kTEST_1_NAME );										//	ctor(name exists/doesnt)
		ftTest.Open(kWrite);												//	open(write)
		WriteTest( ftTest, 0, kTEST1SIZE, FALSE );							//	write(write) large odd size
		ftTest.Flush();																//	flush(write)
		ysl = ftTest.GetSize();
		TpsAssert( kTEST1SIZE == ysl, "test file wrong size" );
		
		//	test implicit close when ftTest goes out of scope
		//ftTest.Close();																//	implicit dtor, close
		}

		//	re-verify test file
		RFileStream ftTest2;															//	base ctor
		//	reopen existing non-empty file new access mode
		ftTest2.Open( kTEST_1_NAME, kRead );						//	open( name, read ) nonzero size
		TpsAssert( kTEST1SIZE == ftTest2.GetSize(), "test file wrong size" );
		FileVerify(ftTest2);
		ftTest2.Close();																//	close(read) nonzero size
		
		//	make sure file size does not change due to non-growing reads and writes
		ftTest2.Open( kTEST_1_NAME, kReadWrite );					//	open( name, readWrite ) nonzero size
		uBYTE ub = 0;
		ftTest2.Write(1, &ub);
		TpsAssert( kTEST1SIZE == ftTest2.GetSize(), "test file wrong size" );
		ftTest2.Read(1, &ub);
		TpsAssert( kTEST1SIZE == ftTest2.GetSize(), "test file wrong size" );
		ftTest2.Close();																//	close(read) nonzero size

		RFileStream rfTest10;
		SetFileAttributes( kTEST_1_NAME, FILE_ATTRIBUTE_READONLY );

		//	re-verify read only test file
		rfTest10.Open( kTEST_1_NAME, kRead );			//	open( name, read ) nonzero size
		TpsAssert( kTEST1SIZE == rfTest10.GetSize(), "test file wrong size" );
		FileVerify(rfTest10);
		rfTest10.Close();						


		//	try copy
		RFileStream rfTest8;
		RFileStream rfTest9;
		SetFileAttributes( kTEST_1_NAME, !FILE_ATTRIBUTE_READONLY );
		rfTest8.Open( kTEST_1_NAME, kReadWrite );	
		rfTest9.Open( kTEST_8_NAME, kReadWrite );	
		rfTest9.Copy( &rfTest8 );													//	copy entire stream from beginning to empty destination
		TpsAssert( kTEST1SIZE == rfTest8.GetSize(), "copied stream wrong size" );
		TpsAssert( kTEST1SIZE == rfTest9.GetSize(), "copied stream wrong size" );
		TpsAssert( rfTest8.AtEnd(), " stream position wrong" );
		TpsAssert( rfTest9.AtEnd(), " stream position wrong" );
		FileVerify(rfTest8);
		FileVerify(rfTest9);
		
		//	try an internal stream to stream copy of a specified size
		rfTest8.SeekAbsolute(1280);
		rfTest9.SeekAbsolute(1280);
		rfTest9.Copy( &rfTest8, 666 );
		FileVerify(rfTest8);
		FileVerify(rfTest9);
		
		//	try an internal stream to stream copy; no specified size
		rfTest8.SeekAbsolute(666);
		rfTest9.SeekAbsolute(666);
		rfTest9.Copy( &rfTest8 );
		FileVerify(rfTest8);
		FileVerify(rfTest9);
		TpsAssert( kTEST1SIZE == rfTest8.GetSize(), "copied stream wrong size" );
		TpsAssert( kTEST1SIZE == rfTest9.GetSize(), "copied stream wrong size" );
		rfTest8.Close();
		rfTest9.Close();

		//	test routines to implement safe save
		//		i.e. save data from an existing file (the original doc pre user changes)
		//		into a temp file (the new user doc to save)
		//		then replace the original with the temp file
		RFileStream rfTest11;
		RFileStream rfTest12;
		XPlatformFileID xpID;
		rfTest11.Open( kTEST_1_NAME, kRead );	//	open original user doc to save
		rfTest11.GetFileIdentifier( xpID );					//	get the location of the file to save
		rfTest12.CreateTempFile( xpID );						//	make a temp file in the same directory as the source file
		rfTest12.Open( kWrite );
		rfTest12.Copy( &rfTest11 );							//	fill new user doc to save with old data
		rfTest12.Close();	
		rfTest11.Delete();										//	get rid of original
		rfTest12.Rename( xpID );								//	move new copy into original
		rfTest12.Open( kRead );						//	make sure file object is correctly configured
		TpsAssert( kTEST1SIZE == rfTest12.GetSize(), "test file wrong size" );
		FileVerify(rfTest12);
		rfTest12.Close();	
	}
		
	}

		//	Test RMemoryStream
		//	perform basic open/write/read test
	{
		{
		RMemoryStream ftMTest;														//	ctor(name exists/doesnt)
		ftMTest.Open(kReadWrite);											//	open(write)
		WriteTest( ftMTest, 0, kTEST1SIZE );							//	write(write) large odd size
		ftMTest.Flush();																//	flush(write)
		TpsAssert( kTEST1SIZE == ftMTest.GetSize(), "test file wrong size" );
		
		//	test implicit close when ftTest goes out of scope
		//ftMTest.Close();															//	implicit dtor, close
		}

		//	try copy
		RMemoryStream rfMTest8;
		RMemoryStream rfMTest9;
		rfMTest8.Open( kReadWrite );	
		WriteTest( rfMTest8, 0, kTEST1SIZE );							//	write(write) large odd size
		rfMTest9.Open( kReadWrite );	
		rfMTest9.Copy( &rfMTest8 );													//	copy entire stream from beginning to empty destination
		TpsAssert( kTEST1SIZE == rfMTest8.GetSize(), "copied stream wrong size" );
		TpsAssert( kTEST1SIZE == rfMTest9.GetSize(), "copied stream wrong size" );
		TpsAssert( rfMTest8.AtEnd(), " stream position wrong" );
		TpsAssert( rfMTest9.AtEnd(), " stream position wrong" );
		FileVerify(rfMTest8);
		FileVerify(rfMTest9);
		
		//	try an internal stream to stream copy of a specified size
		rfMTest8.SeekAbsolute(1280);
		rfMTest9.SeekAbsolute(1280);
		rfMTest9.Copy( &rfMTest8, 666 );
		FileVerify(rfMTest8);
		FileVerify(rfMTest9);
		
		//	try an internal stream to stream copy; no specified size
		rfMTest8.SeekAbsolute(666);
		rfMTest9.SeekAbsolute(666);
		rfMTest9.Copy( &rfMTest8 );
		FileVerify(rfMTest8);
		FileVerify(rfMTest9);
		TpsAssert( kTEST1SIZE == rfMTest8.GetSize(), "copied stream wrong size" );
		TpsAssert( kTEST1SIZE == rfMTest9.GetSize(), "copied stream wrong size" );
		rfMTest8.Close();
		rfMTest9.Close();

		//	try copy to file stream
		RMemoryStream rfMTest18;
		RFileStream rfTest19;
		rfMTest18.Open( kReadWrite );	
		WriteTest( rfMTest18, 0, kTEST1SIZE );							//	write(write) large odd size
		rfTest19.CreateTempFile();
		rfTest19.Open( kReadWrite );	
		rfTest19.Copy( &rfMTest18 );													//	copy entire stream from beginning to empty destination
		TpsAssert( kTEST1SIZE == rfMTest18.GetSize(), "copied stream wrong size" );
		TpsAssert( kTEST1SIZE == rfTest19.GetSize(), "copied stream wrong size" );
		TpsAssert( rfMTest18.AtEnd(), " stream position wrong" );
		TpsAssert( rfTest19.AtEnd(), " stream position wrong" );
		FileVerify(rfMTest18);
		FileVerify(rfTest19);
		rfMTest18.Close();
		rfTest19.Close();
	
		//	replace existing file test
		{
		RFileStream ftTest( kTEST_1_NAME );										//	ctor(name exists)
		ftTest.Open(kWriteReplaceExisting);												//	open(write) replace
		ysl = ftTest.GetSize();
		TpsAssert( 0 == ysl, "test file wrong size" );
		
		//	test implicit close when ftTest goes out of scope
		//ftTest.Close();																//	implicit dtor, close
		}

		}

		{		
#ifdef TESTERRORS
		{
		//	perform remaining coverage testing

		RFileStream ftTest3;

		//	test empty file
		TestEmpty( ftTest3, kReadWrite);
		TestEmpty( ftTest3, kWrite);

		//	verify error conditions

		RFileStream ftTest4( kTEST_1_NAME );							//	ctor(name exists)
		TestSeekError( ftTest4, kRead);
		TestSeekError( ftTest4, kReadWrite);
		TestSeekError( ftTest4, kWrite);
		}

		//	read from write only file
		{
		RFileStream ftTest5( kTEST_1_NAME );								
		try
			{
			uBYTE ub;
			ftTest5.Open(kWrite);
			ftTest5.Read( 1, &ub );
			TpsAssert( FALSE, "Exception not thrown on error" );
			}
		catch( YException eTest )
			{
			TpsAssert( kAccessDenied == eTest, "unhandled exception" );
			if ( kAccessDenied != eTest )
				ftTest5.ThrowFileException( eTest );

			SetLastError(NO_ERROR);
			}
		ftTest5.Close();

		//	write to read only file
		try
			{
			uBYTE ub;
			ftTest5.Open(kRead);
			ftTest5.Write( 1, &ub );
			TpsAssert( FALSE, "Exception not thrown on error" );
			}
		catch( YException eTest )
			{
			TpsAssert( kAccessDenied == eTest, "unhandled exception" );
			if ( kAccessDenied != eTest )
				ftTest5.ThrowFileException( eTest );

			SetLastError(NO_ERROR);
			}
		ftTest5.Close();
		}

		//	open non-existant read only file
		{
		RFileStream ftTest6( kTEST_6_NAME );								
		try
			{
			ftTest6.Open(kRead);
			TpsAssert( FALSE, "Exception not thrown on error" );
			}
		catch( YException eTest )
			{
			TpsAssert( kFileNotFound == eTest, "unhandled exception" );
			if ( kFileNotFound != eTest )
				ftTest6.ThrowFileException( eTest );

			SetLastError(NO_ERROR);
			}
		ftTest6.Close();
		}

		//	open read only file for writing
		{
		RFileStream ftTest7( kTEST_7_NAME );								
		try
			{
			//	create read only file
			ftTest7.Open(kWrite);
			ftTest7.Close();
			SetFileAttributes( kTEST_7_NAME, FILE_ATTRIBUTE_READONLY );

			ftTest7.Open(kWrite);
			TpsAssert( FALSE, "Exception not thrown on error" );
			}
		catch( YException eTest )
			{
			TpsAssert( kAccessDenied == eTest, "unhandled exception" );
			if ( kAccessDenied != eTest )
				ftTest7.ThrowFileException( eTest );

			SetLastError(NO_ERROR);
			}

		try
			{
			ftTest7.Open(kReadWrite);
			TpsAssert( FALSE, "Exception not thrown on error" );
			}
		catch( YException eTest )
			{
			TpsAssert( kAccessDenied == eTest, "unhandled exception" );
			if ( kAccessDenied != eTest )
				ftTest7.ThrowFileException( eTest );

			SetLastError(NO_ERROR);
			}

		ftTest7.Close();
		}
#endif	// TESTERRORS
		}

//	Test ChunkStorage
	{
#if 0
	//	Find a document in a collection file and get its position and size.
		{	
		RChunkStorage rCollection( "g:\\Psd\\Development\\Renaissance\\Bins\\Collections\\Mar25 Test1 0409.lyc", kRead );
		RChunkStorage::RChunkSearcher rSrch;

		rSrch = rCollection.Start('CONT', !kRecursive );
		rSrch = rCollection.Start('LYHD', !kRecursive );
		rSrch = rCollection.Start('LYPF', !kRecursive );
		rSrch = rCollection.Start('PROJ', !kRecursive );
		YStreamLength yPos = rCollection.GetAbsolutePositionInStream( );
		YStreamLength ySize = rCollection.GetCurrentChunkSize( );
		YStreamLength yDataSize = rCollection.GetCurrentChunkDataSize( );
		}
#endif

		{
		//	Test stream access of chunk contents.

		//	Create the storage backing the chunkstorage seperately
		//	so that we may access its contents directly.
		RStorage rBackingStorage( kTEST_27_NAME, kReadWrite );
		
		//	Create a test chunkstorage for testing
		RChunkStorage rcDocTest( &rBackingStorage );
		rcDocTest.AddChunk( 'base' );
		rcDocTest.Write(8, (LPUBYTE)"a123456789");
		YChunkStorageId yStartPos = rcDocTest.GetSelectedChunk( );
		YStreamLength yAPos = rcDocTest.GetAbsolutePositionInStream( );

		rcDocTest.AddChunk( 'tag1' );
		rcDocTest.Write(9, (LPUBYTE)"b123456789");
		YStreamLength yBPos = rcDocTest.GetAbsolutePositionInStream( );

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag2' );
		rcDocTest.Write(10, (LPUBYTE)"c123456789");
		YStreamLength yCPos = rcDocTest.GetAbsolutePositionInStream( );

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag1' );
		rcDocTest.Write(10, (LPUBYTE)"d123456789");
		YStreamLength yDPos = rcDocTest.GetAbsolutePositionInStream( );

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag2' );
		rcDocTest.Write(10, (LPUBYTE)"e123456789");
		YStreamLength yEPos = rcDocTest.GetAbsolutePositionInStream( );

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag1' );
		rcDocTest.Write(10, (LPUBYTE)"f123456789");
		YStreamLength yFPos = rcDocTest.GetAbsolutePositionInStream( );
		
		rcDocTest.AddChunk( 'tag1' );
		rcDocTest.Write(10, (LPUBYTE)"g123456789");
		YStreamLength yGPos = rcDocTest.GetAbsolutePositionInStream( );

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag2' );
		rcDocTest.Write(10, (LPUBYTE)"h123456789");
		YStreamLength yHPos = rcDocTest.GetAbsolutePositionInStream( );

		//	Make sure the absolute positions recorded above actually 
		//	point to the correct data in the chunkstorage.
		uBYTE pubBuffer[22];
		rBackingStorage.SeekAbsolute( yAPos );
		rBackingStorage.Read( 8, pubBuffer );
		TpsAssert( !memcmp( "a123456789", pubBuffer, 8 ) , "Absolute positioning test failed" );
		
		rBackingStorage.SeekAbsolute( yBPos );
		rBackingStorage.Read( 9, pubBuffer );
		TpsAssert( !memcmp( "b123456789", pubBuffer, 9 ) , "Absolute positioning test failed" );
		
		rBackingStorage.SeekAbsolute( yCPos );
		rBackingStorage.Read( 10, pubBuffer );
		TpsAssert( !memcmp( "c123456789", pubBuffer, 10 ) , "Absolute positioning test failed" );
		
		rBackingStorage.SeekAbsolute( yDPos );
		rBackingStorage.Read( 10, pubBuffer );
		TpsAssert( !memcmp( "d123456789", pubBuffer, 10 ) , "Absolute positioning test failed" );
		
		rBackingStorage.SeekAbsolute( yEPos );
		rBackingStorage.Read( 10, pubBuffer );
		TpsAssert( !memcmp( "e123456789", pubBuffer, 10 ) , "Absolute positioning test failed" );
		
		rBackingStorage.SeekAbsolute( yFPos );
		rBackingStorage.Read( 10, pubBuffer );
		TpsAssert( !memcmp( "f123456789", pubBuffer, 10 ) , "Absolute positioning test failed" );
		
		rBackingStorage.SeekAbsolute( yGPos );
		rBackingStorage.Read( 10, pubBuffer );
		TpsAssert( !memcmp( "g123456789", pubBuffer, 10 ) , "Absolute positioning test failed" );
		
		rBackingStorage.SeekAbsolute( yHPos );
		rBackingStorage.Read( 10, pubBuffer );
		TpsAssert( !memcmp( "h123456789", pubBuffer, 10 ) , "Absolute positioning test failed" );
		
		}

		//	test clipboard support
		{
		//	specify and open an archive
		RChunkStorage rClipTest( kTEST_24_NAME, kReadWrite );
		YChunkStorageId pId = rClipTest.GetSelectedChunk();	//	remember the root
		
		//	Create a chunk tree in the form:
		//	   	hith
		//	    /   |  \  
		//	hit2  hit3   hit4
		//	     / |  \ 
		//	hit5 hit6 hit7
		rClipTest.AddChunk('hith');								//	add the chunk we will write to
		rClipTest.Write( 10, (PUBYTE)"a123456789" );
		rClipTest.Write( 10, (PUBYTE)"b123456789" );
		rClipTest.Write( 10, (PUBYTE)"c123456789" );
		rClipTest.Write( 10, (PUBYTE)"d123456789" );
		rClipTest.Write( 10, (PUBYTE)"e123456789" );			//	with current value for  the next write will cause the backstore chunk to grow
		rClipTest.Write( 10, (PUBYTE)"f123456789" );
		rClipTest.SeekAbsolute( 0 );
		YChunkStorageId yHithId = rClipTest.GetSelectedChunk();	//	Remember this chunk's id so we can search for it later.
		
		//	Test read the chunk we just created.
		uBYTE pubBuffer[60];
		rClipTest.Read( 60, pubBuffer );
		TpsAssert( !memcmp( pubBuffer, (PUBYTE)"a123456789b123456789c123456789d123456789e123456789f123456789", 60 ), "read test failed" );

		rClipTest.AddChunk('hit2');				
		rClipTest.Write( 10, (PUBYTE)"g123456789" );
		rClipTest.Write( 10, (PUBYTE)"h123456789" );
		rClipTest.Write( 10, (PUBYTE)"i123456789" );
		YChunkStorageId yHit2Id = rClipTest.GetSelectedChunk();	//	Remember this chunk's id so we can search for it later.

		rClipTest.SelectParentChunk( );

		rClipTest.AddChunk('hit3');							
		rClipTest.Write( 10, (PUBYTE)"j123456789" );
		rClipTest.Write( 10, (PUBYTE)"k123456789" );
		rClipTest.Write( 10, (PUBYTE)"l123456789" );
		YChunkStorageId yHit3Id = rClipTest.GetSelectedChunk();	//	Remember this chunk's id so we can search for it later.

		rClipTest.AddChunk('hit5');							
		rClipTest.Write( 10, (PUBYTE)"m123456789" );
		rClipTest.Write( 10, (PUBYTE)"n123456789" );
		rClipTest.Write( 10, (PUBYTE)"o123456789" );
		YChunkStorageId yHit5Id = rClipTest.GetSelectedChunk();	//	Remember this chunk's id so we can search for it later.

		rClipTest.SelectParentChunk( );

		rClipTest.AddChunk('hit6');							
		rClipTest.Write( 10, (PUBYTE)"p123456789" );
		rClipTest.Write( 10, (PUBYTE)"q123456789" );
		rClipTest.Write( 10, (PUBYTE)"r123456789" );
		YChunkStorageId yHit6Id = rClipTest.GetSelectedChunk();	//	Remember this chunk's id so we can search for it later.

		rClipTest.SelectParentChunk( );

		rClipTest.AddChunk('hit7');						
		rClipTest.Write( 10, (PUBYTE)"s123456789" );
		rClipTest.Write( 10, (PUBYTE)"t123456789" );
		rClipTest.Write( 10, (PUBYTE)"u123456789" );
		YChunkStorageId yHit7Id = rClipTest.GetSelectedChunk();	//	Remember this chunk's id so we can search for it later.

		rClipTest.SelectParentChunk( );

		rClipTest.SelectParentChunk( );

		rClipTest.AddChunk('hit4');						
		rClipTest.Write( 10, (PUBYTE)"v123456789" );
		rClipTest.Write( 10, (PUBYTE)"w123456789" );
		rClipTest.Write( 10, (PUBYTE)"x123456789" );
		YChunkStorageId yHit4Id = rClipTest.GetSelectedChunk();	//	Remember this chunk's id so we can search for it later.

		rClipTest.SetSelectedChunk( pId );
		
		//	Run the tests for this chunk tree.
		TestSpecialChunkTree( rClipTest );
		}

		//	test ready made collection support.
		{	
		//	Create a collection file.
		RFileStream* pCollection = new RFileStream;
		pCollection->Open( kTEST_30_NAME, kReadWrite );
		RFileStream rTestReadyMade;
		rTestReadyMade.Open( kTEST_24_NAME, kRead );
		
		//	Catenate three chunk storage trees into our collection file test.
		//	Record their positions in the collection file.
		YStreamPosition yReadyMadeOnePos = pCollection->GetSize();
		YStreamLength yReadyMadeOneSize = rTestReadyMade.GetSize();
		YChunkTag yReadyMadeOneTag = 'hith';
		pCollection->Copy( &rTestReadyMade );

		YStreamPosition yReadyMadeTwoPos = pCollection->GetSize();
		YStreamLength yReadyMadeTwoSize = rTestReadyMade.GetSize();
		YChunkTag yReadyMadeTwoTag = 'hith';
		rTestReadyMade.SeekAbsolute( 0 );
		pCollection->Copy( &rTestReadyMade );
		
		YStreamPosition yReadyMadeThreePos = pCollection->GetSize();
		YStreamLength yReadyMadeThreeSize = rTestReadyMade.GetSize();
		YChunkTag yReadyMadeThreeTag = 'hith';
		rTestReadyMade.SeekAbsolute( 0 );
		pCollection->Copy( &rTestReadyMade );

		//	Close the created collection file so we may open it in read only
		//	mode and load the individual chunk storages in it.
		delete pCollection;
		RStorage* pCollectionStorage = new RStorage( kTEST_30_NAME, kRead );
		RChunkStorage* pCurrentReadyMadeChunkStorage;

		//	Load the first storage and test it.
		pCurrentReadyMadeChunkStorage = new RChunkStorage( pCollectionStorage, yReadyMadeOnePos, yReadyMadeOneSize );
		pCurrentReadyMadeChunkStorage->CreateSubtreeFromStorage( );
		TestSpecialChunkTree( *pCurrentReadyMadeChunkStorage );
		delete pCurrentReadyMadeChunkStorage;

		//	Load the second storage and test it.
		pCurrentReadyMadeChunkStorage = new RChunkStorage( pCollectionStorage, yReadyMadeTwoPos, yReadyMadeTwoSize );
		pCurrentReadyMadeChunkStorage->CreateSubtreeFromStorage( );
		TestSpecialChunkTree( *pCurrentReadyMadeChunkStorage );
		delete pCurrentReadyMadeChunkStorage;

		//	Load the third storage and test it.
		pCurrentReadyMadeChunkStorage = new RChunkStorage( pCollectionStorage, yReadyMadeThreePos, yReadyMadeThreeSize );
		pCurrentReadyMadeChunkStorage->CreateSubtreeFromStorage( );
		TestSpecialChunkTree( *pCurrentReadyMadeChunkStorage );
		delete pCurrentReadyMadeChunkStorage;

		//	Try the header size fcn.
		//	Try a mac and a windows chunk storage
//		RStorage rCollection( "g:\\Psd\\Development\\Renaissance\\Bins\\Collections\\Mar25 Test1 0409.lyc", kRead );
//		rCollection.SeekAbsolute( 0 );
//		TpsAssert( sizeof( YChunkHeader ) == RChunkStorage::GetHeaderSizeInStream( &rCollection ), "Failed to get the size of a chunk header" );

		pCollectionStorage->SeekAbsolute( yReadyMadeTwoPos );
		TpsAssert( sizeof( YChunkHeader ) == RChunkStorage::GetHeaderSizeInStream( pCollectionStorage ), "Failed to get the size of a chunk header" );

		//	Clean up.
		delete pCollectionStorage;
		}

		{
		//	Try recursive searches after re-opening the file.
		RChunkStorage rClipTest( kTEST_24_NAME, kRead );
		YChunkStorageId pId = rClipTest.GetSelectedChunk();	//	remember the root
		RChunkStorage::RChunkSearcher rSrch;

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit4', kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit4', "Recursive search failed" );

//		rClipTest.SetSelectedChunk( pId );
//		RChunkStorage::RChunkSearchParams rParams( kRecursive, kSearchTag, 'hit4', !kSearchId, kIdDontCare, kSearchVersion, 2 );
//		rSrch = rClipTest.Start( rParams );
//		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit4', "Recursive search failed" );
		
		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit2', kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit2', "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hith', "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit3', kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit3', "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit5', kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit5', "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit6', kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit6', "Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hit7', kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit7', "Recursive search failed" );

//		rClipTest.SetSelectedChunk( pId );
//		rParams = RChunkStorage::RChunkSearchParams( kRecursive, kSearchTag, 'hit6', !kSearchId, kIdDontCare, kSearchVersion, 2 );
//		rSrch = rClipTest.Start( rParams );
//		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit6', "Recursive search failed" );

//		rClipTest.SetSelectedChunk( pId );
//		rParams = RChunkStorage::RChunkSearchParams( kRecursive, kSearchTag, 'hit7', !kSearchId, kIdDontCare, kSearchVersion, 2 );
//		rSrch = rClipTest.Start( rParams );
//		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit7', "Recursive search failed" );

		//	Try non-recursive searches		
		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hith', "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hith', "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit2', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit2', "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hith', "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit3', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit3', "Non-Recursive search failed" );

//		rClipTest.SetSelectedChunk( pId );
//		rSrch = rClipTest.Start('hith', !kRecursive );
//		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hith', "Non-Recursive search failed" );
//		rParams = RChunkStorage::RChunkSearchParams( !kRecursive, kSearchTag, 'hit4', !kSearchId, kIdDontCare, kSearchVersion, 2 );
//		rSrch = rClipTest.Start( rParams );
//		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit4', "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hith', "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit3', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit3', "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit5', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit5', "Non-Recursive search failed" );
/*
		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hith', "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit3', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit3', "Non-Recursive search failed" );
		rParams = RChunkStorage::RChunkSearchParams( !kRecursive, kSearchTag, 'hit6', !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit6', "Non-Recursive search failed" );

		rClipTest.SetSelectedChunk( pId );
		rSrch = rClipTest.Start('hith', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hith', "Non-Recursive search failed" );
		rSrch = rClipTest.Start('hit3', !kRecursive );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit3', "Non-Recursive search failed" );
		rParams = RChunkStorage::RChunkSearchParams( !kRecursive, kSearchTag, 'hit7', !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit7', "Non-Recursive search failed" );

		//	Try another test looking for each of the version 2 chunks in order.
		rClipTest.SetSelectedChunk( pId );
		rParams = RChunkStorage::RChunkSearchParams( kRecursive, !kSearchTag, kTagDontCare, !kSearchId, kIdDontCare, kSearchVersion, 2 );
		rSrch = rClipTest.Start( rParams );
		TpsAssert( rClipTest.GetCurrentChunkMinDataVersion() == 2
			, "recursive versioned chunk failed" );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit6'
			, "recursive versioned chunk failed" );

		++rSrch;
		TpsAssert( rClipTest.GetCurrentChunkMinDataVersion() == 2
			, "recursive versioned chunk failed" );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit7'
			, "recursive versioned chunk failed" );

		++rSrch;
		TpsAssert( rClipTest.GetCurrentChunkMinDataVersion() == 2
			, "recursive versioned chunk failed" );
		TpsAssert( rClipTest.GetCurrentChunkTag() == 'hit4'
			, "recursive versioned chunk failed" );
*/
}
		{
		//	Test tag search.
		RChunkStorage rcDocTest( kTEST_25_NAME, kReadWrite );
		rcDocTest.AddChunk( 'base' );
		rcDocTest.Write(8, (LPUBYTE)"a123456789");
		YChunkStorageId yStartPos = rcDocTest.GetSelectedChunk( );

		rcDocTest.AddChunk( 'tag1' );
		rcDocTest.Write(9, (LPUBYTE)"b123456789");

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag2' );
		rcDocTest.Write(10, (LPUBYTE)"c123456789");

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag1' );
		rcDocTest.Write(10, (LPUBYTE)"d123456789");

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag2' );
		rcDocTest.Write(10, (LPUBYTE)"e123456789");

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag1' );
		rcDocTest.Write(10, (LPUBYTE)"f123456789");
		
		rcDocTest.AddChunk( 'tag1' );
		rcDocTest.Write(10, (LPUBYTE)"g123456789");

		rcDocTest.SelectParentChunk();
		rcDocTest.AddChunk( 'tag2' );
		rcDocTest.Write(10, (LPUBYTE)"h123456789");

		RChunkStorage::RChunkSearcher rSrch;
		uBYTE pubBuffer[22];
		rcDocTest.SetSelectedChunk( yStartPos );
		rSrch = rcDocTest.Start('tag1', !kRecursive );	//	find b123456789
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );

		++rSrch;	//	find d123456789
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );
		
		++rSrch;	//	find f123456789
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );
		
		++rSrch;	//	find end
		TpsAssert( rSrch.End(), "Chunk searcher found wrong end point." );

		//	try recursive search
		rcDocTest.SetSelectedChunk( yStartPos );
		rSrch = rcDocTest.Start('tag1', kRecursive );	//	find b123456789
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );
		
		++rSrch;	//	find d123456789
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );

		++rSrch;	//	find f123456789; last one in a non recursuve search
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );

		++rSrch;	//	find g123456789; no more in recursive search
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );

		++rSrch;	//	find end
		TpsAssert( rSrch.End(), "Chunk searcher found wrong end point." );

		//	Test id search.
		rcDocTest.SetSelectedChunk( yStartPos );
		rSrch = rcDocTest.Start('tag1', 0, !kRecursive );	//	find b123456789
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );
		
		++rSrch;	//	find end
		TpsAssert( rSrch.End(), "Chunk searcher found wrong end point." );

		rcDocTest.SetSelectedChunk( yStartPos );
		rSrch = rcDocTest.Start('tag1', 0, kRecursive );	//	find b123456789
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );

		++rSrch;	//	find g123456789; no more in recursive search
		(*rSrch).SeekAbsolute( 0 );
		(*rSrch).Read( 9, pubBuffer );

		++rSrch;	//	find end
		TpsAssert( rSrch.End(), "Chunk searcher found wrong end point." );
		}

		//	test direct writing to the backstore
		{
		RChunkBackstore* rBackstoreTest  = RChunkStorage::GetRootBackstore();
		rBackstoreTest->AddChunk('hith');								//	add the chunk we will write to
		rBackstoreTest->Write( 10, (PUBYTE)"a123456789" );
		RChunkStorage* pId = rBackstoreTest->GetSelectedChunk();	//	remember it
		rBackstoreTest->SelectParentChunk();							//	close our chunk we will write to
		RChunkStorage* pOld = rBackstoreTest->GetSelectedChunk();
		rBackstoreTest->SetSelectedChunk( pId );						//	reselect and write to our closed chunk
		rBackstoreTest->Write( 10, (PUBYTE)"b123456789" );
		rBackstoreTest->Write( 10, (PUBYTE)"c123456789" );
		rBackstoreTest->Write( 10, (PUBYTE)"d123456789" );
		rBackstoreTest->Write( 10, (PUBYTE)"e123456789" );			//	with current value for  the next write will cause the backstore chunk to grow
		rBackstoreTest->Write( 10, (PUBYTE)"f123456789" );
		rBackstoreTest->Write( 10, (PUBYTE)"g123456789" );
		rBackstoreTest->Write( 10, (PUBYTE)"h123456789" );
		rBackstoreTest->Write( 10, (PUBYTE)"i123456789" );
		rBackstoreTest->SeekAbsolute( 0 );
		
		uBYTE pubBuffer[90];
		rBackstoreTest->Read( 90, pubBuffer );
		TpsAssert( !memcmp( pubBuffer, (PUBYTE)"a123456789b123456789c123456789d123456789e123456789f123456789g123456789h123456789i123456789", 90 ), "read test failed" );

		//	try again and make sure that we reuse one of the deleted chunks created above
		rBackstoreTest->SelectParentChunk();							//	add the new test chunk below the root level
		rBackstoreTest->AddChunk('hite');								//	add the chunk we will write to
		pId = rBackstoreTest->GetSelectedChunk();						//	remember it
		rBackstoreTest->SelectParentChunk();							//	close our chunk we will write to
		rBackstoreTest->SetSelectedChunk( pId );						//	reselect and write to our closed chunk
		rBackstoreTest->Write( 10, (PUBYTE)"j123456789" );
		rBackstoreTest->SeekAbsolute( 0 );
		rBackstoreTest->Read( 10, pubBuffer );
		TpsAssert( !memcmp( pubBuffer, (PUBYTE)"j123456789", 10 ), "read test failed" );

		}

		{
		//	[ data chunk chunk ]
		RChunkStorage rcDocEmpty( kTEST_10_NAME, kReadWrite );
		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(8, (LPUBYTE)"a123456789");

		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(9, (LPUBYTE)"b123456789");

		rcDocEmpty.SelectParentChunk();
		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(10, (LPUBYTE)"c123456789");
		
		//	try the save function
		RChunkStorage rcDocOut( kTEST_18_NAME, kReadWrite );
		rcDocEmpty.Save( rcDocOut );
			}
			{
		//	[ chunk chunk data ]
		RChunkStorage rcDocEmpty( kTEST_11_NAME, kReadWrite );
		rcDocEmpty.AddChunk( kAuxData );

		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(8, (LPUBYTE)"a123456789");

		rcDocEmpty.SelectParentChunk();
		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(9, (LPUBYTE)"b123456789");

		rcDocEmpty.SelectParentChunk();
		rcDocEmpty.Write(10, (LPUBYTE)"c123456789");
		
		//	try the save function
		RChunkStorage rcDocOut( kTEST_17_NAME, kReadWrite );
		rcDocEmpty.Save( rcDocOut );
			}
			{
		//	[ chunk chunk data ^chunk ]		//	this should not assert
		RChunkStorage rcDocEmpty( kTEST_14_NAME, kReadWrite );
		rcDocEmpty.AddChunk( kAuxData );

		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(7, (LPUBYTE)"a123456789");

		rcDocEmpty.SelectParentChunk();
		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(8, (LPUBYTE)"b123456789");

		rcDocEmpty.SelectParentChunk();
		rcDocEmpty.Write(9, (LPUBYTE)"c123456789");

		rcDocEmpty.SelectParentChunk();
		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(10, (LPUBYTE)"d123456789");
			}
			{
		//	[ chunk data chunk ]			
		RChunkStorage rcDocEmpty( kTEST_12_NAME, kReadWrite );
		rcDocEmpty.AddChunk( kAuxData );

		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(8, (LPUBYTE)"a123456789");

		rcDocEmpty.SelectParentChunk();
		rcDocEmpty.Write(9, (LPUBYTE)"b123456789");

		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(10, (LPUBYTE)"c123456789");
			}
			{
		//	test backstore code	
		RChunkStorage rcDocEmpty( kTEST_15_NAME, kReadWrite );
		rcDocEmpty.AddChunk( 'root' );
		rcDocEmpty.Write(10, (LPUBYTE)"c123456789");	

		rcDocEmpty.AddChunk( 'tes1' );
		rcDocEmpty.Write(8, (LPUBYTE)"a123456789");

		rcDocEmpty.AddChunk( 'tes2' );
		rcDocEmpty.SelectParentChunk();					//	close tes2
		rcDocEmpty.SelectParentChunk();					//	close tes1
		rcDocEmpty.Start( 'tes1', !kRecursive );		//	select tes1
		rcDocEmpty.Write(9, (LPUBYTE)"b123456789");	//	write to backstore
		
		//	try the save function
		RChunkStorage rcDocOut( kTEST_20_NAME, kReadWrite );
		rcDocEmpty.Save( rcDocOut );

//		rcDocEmpty.AddChunk( 'tes3' );
//		rcDocEmpty.Write(10, (LPUBYTE)"c123456789");
			}
			{
		//	show what the chunkstorage would look like for the above backstore
		//	test without the backstore testing entries
		RChunkStorage rcDocEmpty( kTEST_16_NAME, kReadWrite );
		rcDocEmpty.AddChunk( 'root' );
		rcDocEmpty.Write(10, (LPUBYTE)"c123456789");	

		rcDocEmpty.AddChunk( 'tes1' );
		rcDocEmpty.Write(8, (LPUBYTE)"a123456789");

		rcDocEmpty.AddChunk( 'tes2' );
//		rcDocEmpty.SelectParentChunk();					//	close tes2
//		rcDocEmpty.SelectParentChunk();					//	close tes1
//		rcDocEmpty.Start( 'tes1', !kRecursive );		//	select tes1
//		rcDocEmpty.Write(9, (LPUBYTE)"b123456789");	//	write to backstore

//		rcDocEmpty.AddChunk( 'tes3' );
//		rcDocEmpty.Write(10, (LPUBYTE)"c123456789");
			}
#ifdef kReReadChunkTest
		{
		//	try saving a 'document'
		//	our data will be the names of files and directories
		//	on some of our disks

		//	specify and open an archive
		RChunkStorage rcDocOut( kTEST_9_NAME, kReadWrite );

		//	write our document(the file and dir structure); it will write each of its components (files and directories)
		::SetCurrentDirectory( __TEXT("a:\\") );
		RMBCString rRoot("*.*");
		WriteDirectoryEntry( &rcDocOut, rRoot );
//		::SetCurrentDirectory( __TEXT("c:\\") );
//		WriteDirectoryEntry( &rcDocOut, rRoot );
//		::SetCurrentDirectory( __TEXT("d:\\") );
//		WriteDirectoryEntry( &rcDocOut, rRoot );
		
		//	try the save function
		RChunkStorage rcDocOutTest( kTEST_19_NAME, kReadWrite );
		rcDocOut.Save( rcDocOutTest );
		::SetCurrentDirectory( __TEXT("e:") );
		}
		{
		//	try writing a lot of data to our backstore by creating a chunk, closing it so that
		//	any additional data written to it must go to the backstore then
		//	filling the chunk with a file tree
		RChunkStorage rcDocEmpty( kTEST_21_NAME, kReadWrite );

		rcDocEmpty.AddChunk( 'root' );
		rcDocEmpty.Write(10, (LPUBYTE)"c123456789");	

		rcDocEmpty.AddChunk( 'tes1' );
		rcDocEmpty.Write(8, (LPUBYTE)"a123456789");

		rcDocEmpty.AddChunk( 'tes2' );
		rcDocEmpty.SelectParentChunk();					//	close tes2
		rcDocEmpty.SelectParentChunk();					//	close tes1
		rcDocEmpty.Start( 'tes1', !kRecursive );		//	select tes1

		//	write our document(the file and dir structure); it will write each of its components (files and directories)
		::SetCurrentDirectory( __TEXT("a:\\") );
		RMBCString rRoot("*.*");
		WriteDirectoryEntry( &rcDocEmpty, rRoot );
		
		//	try the save function
		RChunkStorage rcDocOutTest( kTEST_22_NAME, kReadWrite );
		rcDocEmpty.Save( rcDocOutTest );
		::SetCurrentDirectory( __TEXT("e:") );
		}
#endif

		//	try just a few chunks
		{	
		//	make sure we can write data in the following order:
#ifdef NOT_TIRED_OF_ASSERTING_EVERY_TIME_THE_VIEWER_IS_RUN
			{
		//	[ chunk data chunk data ]	//	this should assert when writing second top level data area
		RChunkStorage rcDocEmpty( kTEST_13_NAME, kReadWrite );
		rcDocEmpty.AddChunk( kAuxData );

		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(7, (LPUBYTE)"a123456789");

		rcDocEmpty.SelectParentChunk();
		rcDocEmpty.Write(8, (LPUBYTE)"b123456789");

		rcDocEmpty.AddChunk( kAuxData );
		rcDocEmpty.Write(9, (LPUBYTE)"c123456789");

		rcDocEmpty.SelectParentChunk();
		rcDocEmpty.Write(10, (LPUBYTE)"d123456789");
			}
#endif
		}
/*
		//	try an empty document
		{
		RChunkStorage rcDocEmpty2( kTEST_10_NAME, kReadWrite );
		}
		{	
		//	specify and open an archive
		RChunkStorage rcDocEmpty( kTEST_10_NAME, kRead );
		}
*/
#ifdef NOT_TIRED_OF_ASSERTING_EVERY_TIME_THE_VIEWER_IS_RUN
		//	try a junk name; we should get an exception
		{	
		try
			{
			RChunkStorage rcDocOut( kTEST_1_JUNK_NAME, kReadWrite );
			}
		catch(...)
			{
			}
		}
#endif
		{
		uBYTE ctest[10] = "123456789";
		uBYTE ctestRead[10] = "         ";

		//	create a stream script
			{
		RStreamScript rScript1( kTEST_23_NAME, kReadWrite );
		rScript1.Write( 10, (PUBYTE)&ctest );
			}

		
		//	create it again and make sure it deletes the first one
			{
		RStreamScript rScript2( kTEST_23_NAME, kReadWrite );
		rScript2.Write( 5, (PUBYTE)&ctest );
			}
		
		//	open it for reading and verify that it worked
			{
		RStreamScript rScript3( kTEST_23_NAME, kRead );
		rScript3.Read( 5, (PUBYTE)&ctestRead );
			}
		}

		//	test clipboard support
		{	
		//	specify and open an archive
		RChunkStorage rDocIn( kTEST_9_NAME, kRead );

		//	try creating a chunk storage from a given memory buffer (used to support clipboard transfers)
		YStreamLength yBufferSize;
		PUBYTE pubSrcBuffer = rDocIn.GetCurrentChunkBuffer( yBufferSize );
		RChunkStorage rClipboardTest1( yBufferSize, pubSrcBuffer );
		rDocIn.ReleaseBuffer( pubSrcBuffer );
		
		//	try again with an internal node
		//	select a file chunk inside given doc
//		RChunkStorage::RChunkSearcher rSrch = rDocIn.Start('FILE', kRecursive );
//		pubSrcBuffer = rDocIn.GetCurrentChunkBuffer( yBufferSize );
//		RChunkStorage rClipboardTest2( yBufferSize, pubSrcBuffer );
//		rDocIn.ReleaseBuffer( pubSrcBuffer );
		}
}

#ifdef NOT_TIRED_OF_ASSERTING_EVERY_TIME_THE_VIEWER_IS_RUN
	//	Try the IsChunkStorage api	  
	{
	  //	This approach should work fine.
//	RStorage* anRStorage = new RStorage( kTEST_1_NAME, kRead );
//	if (!RChunkStorage::IsChunkStorage( anRStorage ))
//		{
//		delete anRStorage;
//		return;
//		}
//	RChunkStorage* pRChunkStorage = new RChunkStorage( anRStorage );
//	delete pRChunkStorage;
//	delete anRStorage;

	//	Try a file which is a chunk storage
	//	This approach is easier.
	RChunkStorage* pRChunkStorage = NULL;
	pRChunkStorage = new RChunkStorage( kTEST_24_NAME, kRead );
	delete pRChunkStorage;


	//	Try a file which is not a chunk storage
	//	Create a sample non-chunk storage data file.
	RFileStream* pFile = new RFileStream;
	pFile->Open( kTEST_29_NAME, kWrite );
	pFile->Write( 60, (LPUBYTE)"a123456789b123456789c123456789d123456789e123456789f123456789" );
	delete pFile;

	//	Try opening a non-chunk storage.
	pRChunkStorage = NULL;
	try
		{
		pRChunkStorage = new RChunkStorage( kTEST_29_NAME, kReadWrite );
		TpsAssertAlways( "Opening a non chunk storage threw no exception" );
		}
	catch( YException exception )
		{
		if ( kDataFormatInvalid != exception )
			{
			TpsAssertAlways( "Opening a non chunk storage threw the wrong exception" );
			throw;
			}
		}
	delete pRChunkStorage;
	}

	{
	//	Try double opening a chunk storage.
	RChunkStorage* pRChunkStorage1 = NULL;
	RChunkStorage* pRChunkStorage2 = NULL;
	try
		{
		pRChunkStorage1 = new RChunkStorage( kTEST_24_NAME, kReadWrite );
		pRChunkStorage2 = new RChunkStorage( kTEST_24_NAME, kReadWrite );
		TpsAssertAlways( "no exception thrown when a file was opened twice" );
		}
	catch( YException exception )
		{
		TpsAssert ( kShareViolation == exception, "wrong exception thrown when a file was opened twice" )
		}
	catch( ... )
		{
		TpsAssertAlways( "wrong exception thrown when a file was opened twice" );
		}

	delete pRChunkStorage1;
	delete pRChunkStorage2;
	}
#endif

//		wsprintf(s, "End test at %lu\n", clock());
//		rfReport.Write( strlen(s), (LPUBYTE)s );
//		rfReport.Close();																	//	try close(write)
		}
	catch(...)
		{
		TpsAssert( FALSE, "unhandled exception" );
		SetLastError(NO_ERROR);
		}
	}