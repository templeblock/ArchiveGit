/****************************************************************

	File:		pfileio.h

	$Header: /Projects/Toolbox/ct/SCFILEIO.H 2     5/30/97 8:45a Wmanis $

	Contains:	Independent byte order calls.

	Written by:	Coletti

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
****************************************************************/

#ifndef _H_PFILEIO
#define _H_PFILEIO

#include "sctypes.h" 


typedef enum eByteOrders {
	kNoOrder		= 0,
	kIntelOrder		= 1,
	kMotorolaOrder	= 2
} eByteOrder;


typedef uchar	REALBUF[12];
typedef uchar 	ByteOrderStr[8];


#define kShortBufSize	2
#define kLongBufSize	4


uchar*			BufSet_byteorder( uchar[] );
const uchar*	BufGet_byteorder( const uchar[], short* );


uchar*			BufSet_char( uchar*			dstbuf,
							 const uchar*	srcbuf,
							 size_t			bytes,
							 eByteOrder		desiredByteOrder );

const uchar*	BufGet_char( const uchar*	srcbuf,
							 uchar*			dstbuf,
							 size_t			bytes,
							 eByteOrder		byteOrder );
			
uchar*			BufSet_short( uchar			sbuf[2], 
							  ushort		s,
							  eByteOrder	desiredByteOrder  );

const uchar*	BufGet_short( const uchar	sbuf[2], 
							  ushort&		ps, 
							  eByteOrder	byteOrder );
								 
uchar*			BufSet_long( uchar			pbuf[4], 
							 ulong			l,
							 eByteOrder		desiredByteOrder  );

const uchar*	BufGet_long( const uchar	lbuf[4], 
							 ulong&			pl, 
							 eByteOrder		byteOrder );
								
uchar*			BufSet_REAL( uchar			rbuf[12], 
							 REAL			r,
							 eByteOrder		desiredByteOrder  );

const uchar*	BufGet_REAL( const uchar	rbuf[12], 
							 REAL&			pr, 
							 eByteOrder		byteOrder );


					// the follow are not good for
					// writing out alot of data, but for a long
					// here are there they are goo
void			ReadLong( long&,
						  APPCtxPtr,
						  IOFuncPtr,
						  eByteOrder );

					// a quick way of writing out a long
void			WriteLong( long,
						   APPCtxPtr,
						   IOFuncPtr,
						   eByteOrder );

void			ReadBytes( uchar*,
						   APPCtxPtr,
						   IOFuncPtr,
						   long );

void			WriteBytes( const uchar*,
							APPCtxPtr,
							IOFuncPtr,
							long );
#endif
