//*
//* STUBS.HPP
//*
//* Contains:
//*     Inline code for ReportLinkStatus stubs
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*
//*

#ifndef STUBS_HPP
#define STUBS_HPP

#include "cppcomp.h"
#include "compplib.hpp"

void GFLineStatus::ReportLineStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

void GFDataFormat::ReportDataStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

#if !defined( GCPP_WINDOWS ) && !defined( GCPP_OS2 ) && !defined( GCPP_X32 )

void GFI8250::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

void GFSmartArnet::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

void GFBIOS::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

void GFExtendedBIOS::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

void GFSmartDigiBoard::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

void GFSmartStarGate::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

void GFNasi::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

#endif

#if defined( GCPP_X32 )

void GFI8250::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

#endif
#if defined( GCPP_OS2 )

void GFComOS2::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

#endif

#if defined( GCPP_WINDOWS ) && !defined( GCPP_WIN32 ) && !defined( GCPP_X32 )

void GFComWin3::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

#endif

#if defined( GCPP_WINDOWS ) && defined( GCPP_WIN32 ) && !defined( GCPP_X32 )

void GFComWin32::ReportLinkStatus( void (*ViewerOutput)(char *data) )
{
  GCPP_UNUSED_PARAMETER( ViewerOutput );
}

#endif
#endif
