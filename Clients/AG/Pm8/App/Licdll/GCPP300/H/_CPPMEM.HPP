//*
//* _CPPMEM.HPP
//*
//* Contains:
//*              class GFMem
//*
//*
//* The Greenleaf Comm++ Library
//*
//* Copyright (C) 1989-95 Greenleaf Software Inc.  All Rights Reserved.
//*
//* MODIFICATIONS
//*
//*

#ifndef _CPPMEM_HPP
#define _CPPMEM_HPP

#include "cppcomp.h"
#include "compplib.hpp"
#include "comdef.hpp"

class GCPP_CLASS_TYPE GFMem
{
  public:
    static void * GCPP_PROTO GFMem::AllocateMemory( unsigned int bytes );
    static void GCPP_PROTO GFMem::DeallocateMemory( void *p );
};

#if defined( GCPP_X32 )
class GCPP_CLASS_TYPE GFMemDPMI : public GFMem
{
	public:
		static GCPP_FARPTR32 GCPP_PROTO GFMemDPMI::AllocateDOSMemory( int size );
		static void GCPP_PROTO GFMemDPMI::DeallocateDOSMemory( GCPP_FARPTR32 );
};
#endif
#endif
