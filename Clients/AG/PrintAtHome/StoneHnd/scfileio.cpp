/****************************************************************
Contains:	Implementation of intdependent byte order code.
****************************************************************/

#include "scfileio.h"
#include "scexcept.h"
#include "scmem.h"

// the string that goes into the header
static const char *byteOrderStr[] = {
	"",
	"Intel86",
	"Motor68",
	NULL
};

// Intel to Motorola
#define SC_I2M_MKWORD(p)	(((ushort) ((p)[1]) << 8 ) | ( (p)[0] ))

// Motorola to Intel
#define SC_M2I_MKWORD(p)	(((ushort) ((p)[0]) << 8 ) | ( (p)[1] ))

// Intel to Motorola
#define SC_I2M_MKLONG(p)	\
	((long)(ulong)SC_I2M_MKWORD(p) | (((long)(ulong)SC_I2M_MKWORD((p)+2)) << 16))

// Motorola to Intel
#define SC_M2I_MKLONG(p)	\
	((long)(ulong)SC_M2I_MKWORD((p)+2) | (((long)(ulong)SC_M2I_MKWORD(p)) << 16))				

static const int localByteOrder = kIntelOrder;
#define SCPIO_MKWORD	SC_M2I_MKWORD
#define SCPIO_MKLONG	SC_M2I_MKLONG

// code for creating the header
uchar*			BufSet_byteorder( uchar pbuf[] )
{
	memset( pbuf, 0, sizeof( ByteOrderStr ) );

	strcpy( (char*)pbuf, byteOrderStr[localByteOrder] );

	return pbuf + sizeof( ByteOrderStr );
}

// code for extracting the header
const uchar*	BufGet_byteorder( const uchar	pbuf[],
								  short*		byteOrder )
{
	if ( !strcmp( (char *)pbuf, byteOrderStr[kMotorolaOrder] ) )
		*byteOrder = kMotorolaOrder;
	else if ( !strcmp( (char *)pbuf, byteOrderStr[kIntelOrder] ) )
		*byteOrder = kIntelOrder;
	else
		*byteOrder = kNoOrder;
	return pbuf + sizeof( ByteOrderStr );
}

/* ------------------------------------------------------------ */
uchar*			BufSet_char( uchar* 		dstbuf,
							 const uchar*	srcbuf,
							 size_t 		bytes,
							 eByteOrder  )
{
	memmove( dstbuf, srcbuf, bytes );
	return dstbuf + bytes;
}

/* ------------------------------------------------------------ */
const uchar*	BufGet_char( const uchar*	srcbuf,
							 uchar* 		dstbuf,
							 size_t 		bytes,
							 eByteOrder  )							 
{
	memmove( dstbuf, srcbuf, bytes );
	return srcbuf + bytes;
}

/* ------------------------------------------------------------ */
// write out a short to a byte buffer
uchar*			BufSet_short( uchar 		pbuf[2],
							  ushort		s,
							  eByteOrder	desiredByteOrder  )
{
	if ( desiredByteOrder != localByteOrder ) {
		
		switch ( desiredByteOrder ) {
			
			case kMotorolaOrder:
				*pbuf = (uchar)SC_I2M_MKWORD((uchar*)&s);
				break;
				
			case kIntelOrder:
				*pbuf = (uchar)SC_M2I_MKWORD((uchar*)&s);
				break;
				
			default:
				*(ushort*)pbuf = s;
		}
		
	}
	else
		*(ushort*)pbuf = s;

	return pbuf + sizeof( ushort );
}

/* ------------------------------------------------------------ */
// read out a short from a byte buffer
const uchar*	BufGet_short( const uchar	abuf[2],
							  ushort&		s,
							  eByteOrder	byteOrder )
{
	if ( localByteOrder != byteOrder )
		s = (ushort)SCPIO_MKWORD(abuf);
	else
		s = *(ushort*)abuf;

	return abuf+2;
}

/* ------------------------------------------------------------ */
// write out a long to a byte buffer
uchar*			BufSet_long( uchar		pbuf[4],
							 ulong		l,
							 eByteOrder desiredByteOrder  )
{
	if ( desiredByteOrder != localByteOrder ) {
		
		switch ( desiredByteOrder ) {
			
			case kMotorolaOrder:
				*(ulong*)pbuf = SC_I2M_MKLONG((uchar*)&l);
				break;
				
			case kIntelOrder:
				*(ulong*)pbuf = SC_M2I_MKLONG((uchar*)&l);
				break;
				
			default:
				*((ulong*)pbuf) = l;
		}
	}
	else
		*((ulong*)pbuf) = l;

	return pbuf + sizeof( ulong );
}

/* ------------------------------------------------------------ */
/* read out a long from a byte buffer */
const uchar*	BufGet_long( const uchar	abuf[4],
							 ulong& 		l,
							 eByteOrder 	byteOrder )
{
	if ( localByteOrder != byteOrder )
		l = SCPIO_MKLONG(abuf);
	else
		l = *(ulong*)abuf;

	return abuf+4;
}

/* ------------------------------------------------------------ */
static const uchar* BytesToIntelDouble( const REALBUF	rbuf,
										REAL&			r )
{
	uchar	*ptr = (uchar *)&r;

	switch( sizeof( REAL ) ) {
		case 10:
			ptr[9]	= rbuf[2];
			ptr[8]	= rbuf[3];
			break;
		case 8:
			break;
		default:
			scAssert( 0 );
			break;
	}

	ptr[7]	= rbuf[4];
	ptr[6]	= rbuf[5];
	ptr[5]	= rbuf[6];
	ptr[4]	= rbuf[7];
	ptr[3]	= rbuf[8];
	ptr[2]	= rbuf[9];
	ptr[1]	= rbuf[10];
	ptr[0]	= rbuf[11];

	return rbuf + 12;
}

/* ------------------------------------------------------------ */
static uchar*	IntelDoubleToBytes( REALBUF 	rbuf,
									REAL		r )
{
	uchar	*ptr = (uchar *)&r;

	switch( sizeof( REAL ) ) {
		case 10:
			rbuf[0] = ptr[9];
			rbuf[1] = ptr[8];
			rbuf[2] = ptr[9];
			rbuf[3] = ptr[8];
			break;

		case 8:
			rbuf[0] = ptr[7];
			rbuf[1] = ptr[6];
			rbuf[2] = ptr[7];
			rbuf[3] = ptr[6];
			break;

		default:
			scAssert( 0 );
			break;
	}

	rbuf[4] = ptr[7];
	rbuf[5] = ptr[6];
	rbuf[6] = ptr[5];
	rbuf[7] = ptr[4];
	rbuf[8] = ptr[3];
	rbuf[9] = ptr[2];
	rbuf[10] = ptr[1];
	rbuf[11] = ptr[0];

	return rbuf + 12;
}

/* ------------------------------------------------------------ */
static const uchar* BytesToMotorolaDouble( const REALBUF	rbuf,
											REAL&			r )
{
	uchar	*ptr = (uchar *)&r;

	switch( sizeof( REAL ) ) {
		case 12:
			memcpy( ptr, rbuf, sizeof( REAL ) );
			break;
		case 10:
			ptr[0] = rbuf[0];
			ptr[1] = rbuf[1];
			ptr[2] = rbuf[4];
			ptr[3] = rbuf[5];
			ptr[4] = rbuf[6];
			ptr[5] = rbuf[7];
			ptr[6] = rbuf[8];
			ptr[7] = rbuf[9];
			ptr[8] = rbuf[10];
			ptr[9] = rbuf[11];
			break;
		case 8:
#ifdef NOTUSED //j
			// Convert extended representation to 64 bit IEEE format
			{
				long extendedRep[3];
				memcpy( extendedRep, rbuf, 12 );
				CvtFloat96To64( r, extendedRep );
			}
#else
			raise( scERRnotImplemented );
#endif NOTUSED
			break;
		default:
			scAssert( 0 );
			break;
	}

	return rbuf + 12;
}

/* ------------------------------------------------------------ */
static uchar*	MotorolaDoubleToBytes( REALBUF	rbuf,
										REAL 	r )
{
	uchar	*ptr = (uchar *)&r;

	switch( sizeof( REAL ) ) {
		case 12:
			memcpy( rbuf, ptr, sizeof( REAL ) );
			break;
		case 10:
			rbuf[0] = ptr[0];
			rbuf[1] = ptr[1];
			rbuf[2] = ptr[0];
			rbuf[3] = ptr[1];
			rbuf[4] = ptr[2];
			rbuf[5] = ptr[3];
			rbuf[6] = ptr[4];
			rbuf[7] = ptr[5];
			rbuf[8] = ptr[6];
			rbuf[9] = ptr[7];
			rbuf[10] = ptr[8];
			rbuf[11] = ptr[9];
			break;
		case 8:
#ifdef NOTUSED //j
			// Convert 64 bit representation to extended
			{
				long extendedRep[3];
				CvtFloat64To96( extendedRep, &r );
				memcpy( rbuf, extendedRep, 12 );
			}
#else
			raise( scERRnotImplemented );
#endif NOTUSED
			break;
			
		default:
			scAssert( 0 );
			break;
	}

	return rbuf + 12;
}

/* ------------------------------------------------------------ */
/* write out a REAL to a byte buffer */
uchar*		BufSet_REAL( uchar		rbuf[12],
						 REAL		d,
						 eByteOrder  )
{
	switch ( localByteOrder ) {
		
		case kIntelOrder:
				// convert intel long double to bytes
			IntelDoubleToBytes( rbuf, d );
			break;
			
		case kMotorolaOrder:
				// convert motorla long double to bytes
			MotorolaDoubleToBytes( rbuf, d );
			break;
			
		default:
			scAssert( 0 );
			break;
	}

	return rbuf + 12;
}

/* ------------------------------------------------------------ */
/* read in a REAL from a byte buffer */
const uchar*	BufGet_REAL( const uchar	rbuf[12],
							 REAL&			r,
							 eByteOrder )
{
	switch ( localByteOrder ) {
		case kMotorolaOrder:
			BytesToMotorolaDouble( (uchar *)rbuf, r );
			break;
			
		case kIntelOrder:
			BytesToIntelDouble( (uchar *)rbuf, r );
			break;
		default:
			break;
	}
	return rbuf + 12;
}

/* ------------------------------------------------------------ */
// write a quick long
void ReadLong( long&		val,
			   APPCtxPtr	ctxPtr,
			   IOFuncPtr	readFunc,
			   eByteOrder	byteOrder )
{
	uchar buf[4];
	raise_if( (*readFunc)( ctxPtr, buf, 4 ) != 4, scERRfile );
	ulong uval; 
	BufGet_long( buf, uval, byteOrder );
	val = (long)uval;
}


/* ------------------------------------------------------------ */
// write a quick long
void WriteLong( long			val,
				APPCtxPtr		ctxPtr,
				IOFuncPtr		writeFunc,
				eByteOrder		byteOrder )
{
	uchar buf[4];
	BufSet_long( buf, val, byteOrder );
	raise_if( (*writeFunc)( ctxPtr, buf, 4 ) != 4, scERRfile );
}


/* ------------------------------------------------------------ */
void ReadBytes( uchar*			buf,
				APPCtxPtr		ctxPtr,
				IOFuncPtr		readFunc,
				long			numbytes )
{
	raise_if( (*readFunc)( ctxPtr, buf, numbytes ) != numbytes, scERRfile ); 
}

/* ------------------------------------------------------------ */
void WriteBytes( const uchar*	buf,
				 APPCtxPtr		ctxPtr,
				 IOFuncPtr		writeFunc,
				 long			numbytes )
{
	raise_if( (*writeFunc)( ctxPtr, (void*)buf, numbytes ) != numbytes, scERRfile );	
}
