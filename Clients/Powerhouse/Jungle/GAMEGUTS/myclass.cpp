#include "windows.h"
#include "sound.h"

//************************************************************************
// The SOUND class definition
//************************************************************************

typedef class cSOUND FAR * pcSOUND;

class cSOUND // : public BaseClass
{
// CONSTRUCTORS
	public:
		cSOUND(void);
		cSOUND(HWND hWnd);

// DESTRUCTOR
	public:
		~cSOUND(void);

// ATTRIBYUTES
	protected:
	private:
		// Static means one copy for all class objects
		static int Count;

		HWND myHWnd;
	public:
		PAINTSTRUCT myPS;

// FUNCTIONS
	public:
		// Static means it can be called without a class object
		int Open(HWND);
		inline void Delay( int iCount ) { while ( --iCount >= 0 ); }
		inline HWND GetHWnd( void ) { return( myHWnd ); }
	private:
		void Read();

// OPERATIONS
	public:
		BOOL operator==(SIZE size) const;
		BOOL operator!=(SIZE size) const;
		void operator+=(SIZE size);
		void operator-=(SIZE size);
};


//************************************************************************
// The DEFINITION and/or INITIALIZATION of static members
//************************************************************************

int cSOUND::Count = 12;


// The CONSTRUCTOR
//************************************************************************
cSOUND::cSOUND()
//************************************************************************
{
}

// The DESTRUCTOR
//************************************************************************
cSOUND::~cSOUND()
//************************************************************************
{
}

//************************************************************************
int cSOUND::Open(HWND hWnd)
//************************************************************************
{
return( 1 );
}

//************************************************************************
void cSOUND::Read()
//************************************************************************
{
}

//************************************************************************
void UseSound(void)
//************************************************************************
{
HWND hWnd;
cSOUND Sound1;
cSOUND Sound2( (HWND)0 );

Sound1.Delay(7);
hWnd = Sound2.GetHWnd();
}
