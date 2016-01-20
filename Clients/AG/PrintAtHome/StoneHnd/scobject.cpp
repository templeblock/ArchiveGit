#include "scmem.h"
#include "scobject.h"
#include "scexcept.h"

const scClass* scClass::sClasses	= NULL;

/*======================================================================*/
void * scClass::MakeInstance ( void ) const
{
	void *volatile	p = NULL;

	try {
		p = MEMAllocPtr( fSize );
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
scClassInit::scClassInit ( scClass* c )
{
	c->fNext	= c->sClasses;
	c->sClasses = c;
}

/*======================================================================*/
scClass scObject::sClass =
{
	"scObject",
	sizeof (scObject), 
	scEmptyClassInitFunc,
	NULL,
	NULL
};

/*======================================================================*/
const scClass & scObject::GetClass( void ) const
{
	return sClass;
}

/*======================================================================*/
BOOL scObject::IsClass( const scClass& c ) const
{
	for ( const scClass * pc = & GetClass(); pc; pc = pc->GetBase() )
		if ( & c == pc )
			return 1;
	return 0;
}

#ifdef NOTUSED //j
/*======================================================================*/
void* scObject::operator new( size_t size )
{
	return MEMAllocPtr( size );
}

/*======================================================================*/
void scObject::operator delete( void* objStorage )
{
	MEMFreePtr( objStorage );
}
#endif NOTUSED //j

/*======================================================================*/
int scObject::IsEqual( const scObject& ) const
{
		// if i am all the way down here what can i check, classnames seems 
		// a bit late for that
	return true;
}
	
/*======================================================================*/
int scObject::operator==( const scObject& obj ) const
{
	return IsEqual( obj );
}

/*======================================================================*/
int scObject::operator!=( const scObject& obj ) const
{
	return !IsEqual( obj );
}
