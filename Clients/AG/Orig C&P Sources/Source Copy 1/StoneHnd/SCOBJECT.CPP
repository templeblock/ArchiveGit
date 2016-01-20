/****************************************************************************

	Stonehand Base Object System Implementation

	$Header: /Projects/Toolbox/ct/SCOBJECT.CPP 2     5/30/97 8:45a Wmanis $

	Contains:

	Written by:	Adams

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.
	
********************************************************************/


#include "scmem.h"
#include "scobject.h"
#include "scexcept.h"

/*======================================================================*/

const scClass * scClass::sClasses	= NULL;

/*======================================================================*/

void * scClass::MakeInstance ( void ) const
{
	void *volatile	p = NULL;

	try {
#if defined( MEM_DEBUG )
		p = MEMAllocPtrDebug( fSize, __FILE__, __LINE__ );	
#else
		p = MEMAllocPtr( fSize );
#endif
		InitInstance ( (void *) p );
	} 
	catch( ... ) {
		MEMFreePtr( p ), p = NULL;
	} 
	return p;
}

/*======================================================================*/

void scClass::InitInstance ( void * p ) const
{
	if ( fInitializer && ( fInitializer != scEmptyClassInitFunc ) )
		( * fInitializer ) ( p );
	else
		raise( scERRexception );
}

/*======================================================================*/

const scClass * scClass::FindClass ( const char * className )
{
	if ( ! className )
		return NULL;
	for ( const scClass * pc = sClasses; pc; pc = pc->fNext )
		if ( strcmp ( className, pc->fName ) == 0 )
			return pc;
	return NULL;
}

/*======================================================================*/

scClassInit::scClassInit ( scClass * c )
{
	c->fNext    = c->sClasses;
	c->sClasses = c;
}

/*======================================================================*/

scClass scSimpleObject::sClass =
{
	"scSimpleObject",
	sizeof (scSimpleObject), 
	scEmptyClassInitFunc,
	NULL,
	1,
	NULL
};

static scClassInit scSimpleObjectInitClass ( & scSimpleObject::sClass );

/*======================================================================*/

const scClass & scSimpleObject::GetClass ( void ) const
{
	return sClass;
}

/*======================================================================*/

Bool scSimpleObject::IsClass ( const scClass & c ) const
{
	return &c == &sClass;
}

/*======================================================================*/

scClass scObject::sClass =
{
	"scObject",
    sizeof (scObject), 
    scEmptyClassInitFunc,
    NULL,
    0,
    NULL
};

static scClassInit scObjectInitClass( & scObject::sClass );

/*======================================================================*/

const scClass & scObject::GetClass( void ) const
{
	return sClass;
}

/*======================================================================*/

Bool scObject::IsClass( const scClass& c ) const
{
	if ( c.IsSimple() )
		return 0;
	for ( const scClass * pc = & GetClass(); pc; pc = pc->GetBase() )
		if ( & c == pc )
			return 1;
	return 0;
}

/*======================================================================*/

void* scObject::operator new( size_t size )
{
#if defined( MEM_DEBUG )
	return MEMAllocPtrDebug( size, __FILE__, __LINE__ );	
#else
	return MEMAllocPtr( size );
#endif
}

/*======================================================================*/

#if defined( MEM_DEBUG )
void *scObject::operator new( size_t		objSize,
							  const char*	file,
							  int			line )
{
	return MEMAllocPtrDebug( objSize, file, line );
}
#endif

/* ======================================================================== */

void scObject::operator delete( void* objStorage )
{
	MEMFreePtr( objStorage );
}

/* ======================================================================== */

int scObject::IsEqual( const scObject& ) const
{
		// if i am all the way down here what can i check, classnames seems 
		// a bit late for that
	return true;
}
	
/* ==================================================================== */

int scObject::operator==( const scObject& obj ) const
{
	return IsEqual( obj );
}

/* ==================================================================== */

int scObject::operator!=( const scObject& obj ) const
{
	return !IsEqual( obj );
}

/* ==================================================================== */





