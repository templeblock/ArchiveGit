#ifndef _FILETEST_H_
#define _FILETEST_H_

#include "FrameworkIncludes.h"

#include "Stream.h"
#include "FileStream.h"

void TestOpen( RStream& rfTest );
void MainTest();
void WriteTest( RStream& rfTest, const YStreamPosition yPlaceToWrite, const YStreamLength yBytesToWrite, BOOLEAN fTestWrite=TRUE );
YStreamLength GetYBuffSize( YStreamLength yBuffSize );
YStreamLength GetYBuffStart( YStreamPosition yBuffOffset );
void FileVerify( RStream& rfTest );
void TestEmpty( RFileStream& rfTest, EAccessMode eOpenMode );
void TestSeekError( RStream& rfTest, EAccessMode eOpenMode );

#if 0
#include "CStdioFileStream.h"

class RFileTest: public RCStdioFileStream	//RFileStream
	{
	public:
		RFileTest( const char* szFilename );	
		RFileTest( ):RCStdioFileStream(){}	//RFileStream(){}	
		void TestOpen();
		void WriteTest( const YStreamPosition yPlaceToWrite, const YStreamLength yBytesToWrite );
		YStreamLength GetYBuffSize( YStreamLength yBuffSize ) const;
		YStreamLength GetYBuffStart( YStreamPosition yBuffOffset ) const;
		void FileVerify();
	};
#endif

//	INLINES

#endif	//	_FILETEST_H_