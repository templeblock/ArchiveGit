/****************************************************************************
 *
 *	Header Name:	tpserr.h
 *	Description:	TPS error definitions header.
 *	Version:			v 1.00
 *	Author:			Michael C. Robert
 *	Date:				7/11/1994
 *
 *	Copyright (C) 1994-1995 by Turning Point Software, Inc. All Rights Reserved.
 *
 ****************************************************************************
 */

#ifndef  _TPSERR_H_
#define  _TPSERR_H_

//----------------------------------------
// SYMBOLIC CONSTANTS
//----------------------------------------

#define	TERR_NO_SECTION	0
#define	TERR_SUCCESS		0

//----------------------------------------
// TYPEDEFS
//----------------------------------------

typedef uLONG TERRCODE;

//----------------------------------------
// MACROS
//----------------------------------------

#define  terrComputeErrorCode( uwSection, uwError )\
				( (uLONG) (((uLONG)uwSection) << 16) | (uwError) )

#define  terrGetSection( ec )\
				( (uWORD) ((uLONG)ec >> 16)) )

#define  terrGetValue( ec )\
				( (uWORD) (((uLONG)ec & 0x0000FFFF)) )

#define	terrGetErrorCode( ec )\
				( (ec) )

#endif	/* _TPSERR_H_ */
