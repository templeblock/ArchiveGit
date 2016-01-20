// ****************************************************************************
//
//  File Name:			lzCompression.h
//
//  Project:			Renaissance Framework
//
//  Author:				B. Rosenblum
//
//  Description:		lz compression and decompression
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
//  $Logfile:: /PM8/Framework/Include/lzCompression.h                         $
//   $Author:: Gbeddow                                                        $
//     $Date:: 3/03/99 6:15p                                                  $
// $Revision:: 1                                                              $
//
// ****************************************************************************

sLONG FrameworkLinkage LZunpack( const uBYTE* TheInputData, PUBYTE TheOutputData,
											sLONG OutputDataSize, sWORD TheLengthBits);

sLONG FrameworkLinkage PackLZ (PUBYTE Dest, const uBYTE* Source, sLONG InputSize,
										 sWORD TheLengthBits, BOOLEAN PackFlag);

sWORD FrameworkLinkage FindBestLZPackKey ( const uBYTE* puSource, sLONG slInputSize, PSLONG pslCompressedSize);
