#pragma once

#include "sctypes.h"

class scClass;

class scClassInit
{
public:
	scClassInit ( scClass* );
};

class scObject;

typedef void (*scClassInitFunc ) ( void* );

class scClass
{
	friend			scClassInit;

public:
	static scObject*		MakeInstance( const char* name )
	{
		const scClass* cl = scClass::FindClass( name );
		return cl ? (scObject*)cl->MakeInstance() : 0;
	}

	void*				MakeInstance ( void ) const;
	
	const char* 	GetName ( void ) const		{ return fName; }
	size_t			GetSize ( void ) const		{ return fSize; }
	const scClass*	GetBase ( void ) const		{ return fBaseClass; }
	
	BOOL			IsAbstract ( void ) const	{ return fInitializer == NULL; }
	
	const scClass*	GetNext ( void ) const		{ return fNext; }

	static const scClass*	FindClass ( const char* );

// private:
	void			InitInstance ( void* ) const;

 // MEMBERS
	const char* 	fName;			// class name
	size_t			fSize;			// instance size
	scClassInitFunc fInitializer;	// instance initializer
	const scClass*	fBaseClass; 	// base class
	const scClass*	fNext;			// link in class list

	static const scClass*	sClasses;	// class list

};

#define scEmptyClassInitFunc	( (scClassInitFunc) -1 )

class scObject {
 // METHODS
protected:
						scObject(){}
private:
							scObject( const scObject & );	// no def
	const scObject& 		operator=( const scObject & );	// no def

public: 					
	virtual 			~scObject(){}

								// allocator support
//j	void*					operator new ( size_t size );
//j	void*					operator new ( size_t, void* p )	{ return p; }
//j	void					operator delete ( void* p );

	const char* 			GetClassname ( void ) const 		{ return GetClass().GetName(); }
	
								// rtti support
	virtual const scClass&	GetClass ( void ) const;
	BOOL					IsClass ( const scClass & c ) const;
	BOOL					IsClass ( const char* name ) const	{ return IsClass(*scClass::FindClass( name ) ); }

	virtual int 			IsEqual( const scObject& ) const;
	int 					operator==( const scObject& ) const;
	int 					operator!=( const scObject& ) const;

 // MEMBERS 
public:
	static scClass			sClass;
};

//  Runtime Type Macros

#define scDECLARE_RTTI											\
		 public:												\
			virtual const scClass & GetClass ( void ) const;	\
		 public:												\
			static scClass		sClass; 						\
		 private:												\
			static void 		InitInstance ( void* )

#define _scRTTI(className,baseClassName,initFunc) \
		scClass className::sClass = \
			{\
				#className, sizeof(className), initFunc, &baseClassName::sClass, 0 }; \
				static scClassInit className##InitClass ( &className::sClass ); \
				const scClass & className::GetClass ( void ) const { return sClass;\
			}

#define scDEFINE_ABSTRACT_RTTI(className,baseClassName) \
		_scRTTI(className,baseClassName,NULL)

#define scDEFINE_RTTI(className,baseClassName) \
		void className::InitInstance ( void* p ) { (void) new ( p ) className; } \
		_scRTTI(className,baseClassName,&className::InitInstance)
