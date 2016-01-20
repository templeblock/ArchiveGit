/****************************************************************************

	Stonehand Base Object Classes

	$Header: /Projects/Toolbox/ct/SCOBJECT.H 2     5/30/97 8:45a Wmanis $

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


#ifndef _H_SCOBJECT
#define _H_SCOBJECT


#include "sctypes.h"

///////////////////////////////////////////////////////////////////////////////
//                               scClassInit                                 //
///////////////////////////////////////////////////////////////////////////////

class scClass;

class scClassInit {
 public:
  				scClassInit ( scClass * );
};

///////////////////////////////////////////////////////////////////////////////
//                                 scClass                                   //
///////////////////////////////////////////////////////////////////////////////

class scObject;
class scSimpleObject;


typedef void ( * scClassInitFunc ) ( void * );

class scClass {

 // FRIENDS
	friend			scClassInit;

 // METHODS
public:
	static scSimpleObject*	MakeSimpleInstance( const char* name )
						{
							const scClass* cl = scClass::FindClass( name );
							return cl ? (scSimpleObject*)cl->MakeInstance() : 0;
						}
	
	static scObject*		MakeInstance( const char* name )
						{
							const scClass* cl = scClass::FindClass( name );
							return cl ? (scObject*)cl->MakeInstance() : 0;
						}

	
	void*				MakeInstance ( void ) const;
	
	const char*		GetName ( void ) const		{ return fName; }
	size_t			GetSize ( void ) const		{ return fSize; }
	const scClass*	GetBase ( void ) const		{ return fBaseClass; }
	
	Bool			IsAbstract ( void ) const	{ return fInitializer == NULL; }
	Bool			IsSimple ( void ) const		{ return fSimple; }
	
	const scClass*	GetNext ( void ) const		{ return fNext; }

	static const scClass*	GetClasses ( void )	{ return sClasses; }
	static const scClass*	FindClass ( const char * );

// private:
	void			InitInstance ( void * ) const;

 // MEMBERS
	const char*		fName;			// class name
	size_t			fSize;			// instance size
	scClassInitFunc	fInitializer;	// instance initializer
	const scClass*	fBaseClass;		// base class
	Bool			fSimple;		// true => non-virtual
	const scClass*	fNext;			// link in class list

	static const scClass*	sClasses;	// class list

};

#define scEmptyClassInitFunc	( (scClassInitFunc) -1 )

///////////////////////////////////////////////////////////////////////////////
//                              sbSimpleObject                               //
///////////////////////////////////////////////////////////////////////////////

class scSimpleObject {
	// METHODS
public:
  				// allocator support
	void*			operator new( size_t s )			{ return ::operator new ( s ); }
	void*			operator new ( size_t, void* p )	{ return p; }
	void			operator delete ( void* p )			{ ::operator delete( p ); }

  				// rtti support
  const scClass&	GetClass ( void ) const;
  Bool				IsClass ( const scClass & c ) const;

  // MEMBERS
public:
	static scClass	sClass;
};

///////////////////////////////////////////////////////////////////////////////
//                                 scObject                                  //
///////////////////////////////////////////////////////////////////////////////

class scObject {
 // METHODS
protected:
						scObject(){}
private:
							scObject( const scObject & );	// no def
	const scObject&			operator=( const scObject & );	// no def

public:						
	virtual				~scObject(){}


	
								// allocator support
	void*					operator new ( size_t size );
#if defined( MEM_DEBUG )
	void*					operator new ( size_t size, const char*, int );
#endif	
	void*					operator new ( size_t, void* p )	{ return p; }
	void					operator delete ( void *p );

	const char*				GetClassname ( void ) const			{ return GetClass().GetName(); }
	
								// rtti support
	virtual const scClass&	GetClass ( void ) const;
	Bool					IsClass ( const scClass & c ) const;
	Bool					IsClass ( const char* name ) const	{ return IsClass( *scClass::FindClass( name ) ); }

	virtual int				IsEqual( const scObject& ) const;
	int						operator==( const scObject& ) const;
	int						operator!=( const scObject& ) const;

 // MEMBERS	
public:
	static scClass			sClass;
	
#if SCDEBUG	> 1
	virtual void		DebugPrint( const char * ) const { };
#endif
};


///////////////////////////////////////////////////////////////////////////////
//                            Runtime Type Macros                            //
///////////////////////////////////////////////////////////////////////////////

#define scRTTI(className)	(className::sClass)

#define scDECLARE_RTTI											\
		 public:												\
			virtual const scClass &	GetClass ( void ) const;	\
		 public:												\
			static scClass		sClass;							\
		 private:												\
			static void			InitInstance ( void * )

#define _scRTTI(className,baseClassName,initFunc,simple) \
		scClass className::sClass = \
			{\
				#className, sizeof(className), initFunc, &scRTTI(baseClassName), simple, 0 }; \
				static scClassInit className##InitClass ( &scRTTI(className) ); \
				const scClass & className::GetClass ( void ) const { return sClass;\
			}

#define scDEFINE_ABSTRACT_RTTI(className,baseClassName) \
		_scRTTI(className,baseClassName,NULL,false)

#define scDEFINE_RTTI(className,baseClassName) \
		void className::InitInstance ( void * p ) { (void) new ( p ) className; } \
		_scRTTI(className,baseClassName,&className::InitInstance,false)

#define scDECLARE_SIMPLE_RTTI \
		public: \
			const scClass &		GetClass ( void ) const; \
			Bool				IsClass ( const scClass & ) const; \
		public: \
			static scClass		sClass; \
		private: \
			static void			InitInstance ( void * );

#define scDEFINE_SIMPLE_RTTI(className,baseClassName) \
		void className::InitInstance ( void * p ) { (void) new ( p ) className; } \
		Bool className::IsClass ( const scClass & c ) const { return &c == &sClass; } \
		_scRTTI(className,baseClassName,&className::InitInstance,true)



#if defined( MEM_DEBUG )
	#define SCNEW	new( __FILE__, __LINE__ )
#else
	#define SCNEW	new
#endif


#endif /* _H_SBBASE_ */
