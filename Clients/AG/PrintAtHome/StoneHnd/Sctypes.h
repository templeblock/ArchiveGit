#pragma once

#ifndef SCDEBUG 		// SCDEBUG needs to be defined!!!
	#ifdef _DEBUG
		#define SCDEBUG 	1
	#else
		#define SCDEBUG 	0
	#endif
#endif

#ifndef SCDEBUG
	#error "SCDEBUG has not been defined"
#endif

#include <windows.h>

#ifdef _DEBUG
//j	#include "atldbgmem.h"
#endif _DEBUG

#include <limits.h>
#include <stdlib.h>
#include <string.h>

#define scChar			TCHAR
#define scString		TEXT

#include "scapptyp.h"			// application types 

/* ================== TOOLBOX BEHAVIOR ================================ */

#define scColumnShape			// can containers be more than a rectangle
#define scFlowJustify			// allow vj
#undef	scLayoutVerify			// verify columns ids - debugging
#undef	scGermanHyph			/* allows char replacement when hyphenating */

/*						MEMORY STUFF									*/

typedef void** scMemHandle;
typedef scMemHandle 		scVertHandle;
typedef scMemHandle 		HRgnHandle;	// a device independent region.

/*						FORWARD REFERENCES								*/

// @class scColumn | An internal Composition Toolbox class, it is exposed to
// the client only as a pointer.
// @class scSelection | An internal Composition Toolbox class, it is exposed to
// the client only as a pointer.
// @class scStream | An internal Composition Toolbox class, it is exposed to
// the client only as a pointer.
	
class						scColumn;			
class						scStream;
class						scSelection;
class						scContUnit;
class						scTextline;
class						scTBObj;

typedef class scTBObj*		scScrapPtr; 		/* clipboard stuff */

/* ================== TOOLBOX TYPES =================================== */

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned long	ulong;

typedef uchar		uint8;
typedef char		int8;
typedef ushort		uint16;
typedef short		int16;
typedef ulong		uint32;
typedef long		int32;
typedef int16		RLU;				// typographic relative unit
typedef int16		TenThousandth;		// used to express +/-100% in one hundreth
										// of one percent, yielding a accuracy
										// of +/-1/10000
										//

#define kInvalMP	(LONG_MIN + 1)		// an invalid micropoint, I hope
										// we do not hit this, if we do
										// hit this we are perilously close
										// to the edge of our coordinate space

typedef int32		GlyphSize;			// @type GlyphSize | A measurement of a glyph,
										// this should be used only to store small
										// values that will not cause overflow.


typedef int32		MicroPoint; 		// @type MicroPoint | A 32 bit coordinate.

typedef int16		scAngle;			// in one hundreths of a degree 180+/- 


			// if scGlyphSize is a 16 bit quantity and there is a possibility there could be an overflow 
			// you may want to define the following
#ifndef SC_GLYPHSIZE_COORDINATE_OVERFLOW
	inline GlyphSize	MPtoGS( MicroPoint mp ) 	{ return (GlyphSize)mp; }
#else			
	inline GlyphSize	MPtoGS( MicroPoint mp );	{ return mp > 0x7fff ? raise( scERRcoordOverflow ) : (GlyphSize)mp; }
#endif	

typedef uint32		scTicks;			// used for timers

typedef WCHAR		UCS2;				// @type UCS2 | A 16 bit Unicode character.

typedef ushort		GlyphID;

#if defined(powerc) || defined(__powerc) || defined(_WINNT_)
	typedef double REAL;		// on Power Mac use 64 bit doubles instead of 128 bit ones
#else
	typedef long double REAL;	
#endif

inline MicroPoint	scRoundMP( REAL mp )
					{
						return (MicroPoint)( mp>=0 ? mp+0.5 : mp-0.5 );
					}
inline GlyphSize	scRoundGS( REAL mp )
					{
						return (GlyphSize)( mp>=0 ? mp+0.5 : mp-0.5 );	
					}

inline REAL 		AngleToRadians( scAngle ang )	{ return (REAL)ang * 3.14159 / 18000; }

	// units fields
typedef enum eUnitTypes {
	eNoUnit = 0,
	eRluUnit,
	eAbsUnit,
	eAngleUnit,
	ePercentUnit,
	eCountUnit,
	eBoolUnit,
	eEnumUnit,
	eBitUnit,
	eChar16Unit,
	eFontUnit,
	eRenderUnit,
	eColorUnit,
	eLanguageUnit
} eUnitType;

/* ==================== TIMERS	======================================= */

#define SCTickCount()		((scTicks)GetTickCount())
#define SCSysBeep(x)		MessageBeep(-1)

/* ============== UNITS for OBJECT to DEVICE CONVERSION =============== */

// this will use twips * 16
// we will use a corrupted point 1/72 inch
#define one_point				(20)
#define one_inch				(one_point*72)
#define one_millimeter			(one_inch/25.4) 					// this value is not great for this resolution
inline MicroPoint TwipsToMP( int twips )
{
	return (MicroPoint)twips;
}
inline GlyphSize TwipsToGE( int twips )
{
	return (GlyphSize)twips;
}

inline int MPToTwips( MicroPoint mp )
{
	return (int)mp;
}

#define nm_one_inch 			25400000L	// one inch in nanometers
#define nm_to_mPoints			5.38296 	// nano-meters to one_point */

#define one_q					(one_millimeter/4)
#define one_half_point			(one_point / 2)
#define one_pica				(one_point * 12)
#define one_cicero				(one_didot * 12)
#define one_meter				(one_millimeter * 1000)
#define mpInfinity				(one_inch * 160) // this is about the limit of the unit system


inline MicroPoint scPOINTS( int n ) 	{ return one_point * n; }
inline MicroPoint scPICAS( int n )		{ return one_pica * n; }

inline REAL muInches( MicroPoint mp )	{ return (REAL)mp / one_inch; }
inline long muPoints( MicroPoint mp )	{ return mp / one_point; }
inline long muQs( MicroPoint mp )		{ return scRoundMP( (REAL)mp / one_millimeter / 4 ); }

inline MicroPoint scSliverSize( void )	{ return one_point * 3; }


/*					ASSERTION CODE										*/
	
void SCAssertFailed(const scChar* exp, const scChar* file, int line);
void SCDebugTrace(int, const scChar* fmt, ...);
void SCDebugBreak(void);

inline void scFuncAssert(int val, const scChar* exp, const scChar* file, int line)
{
	if (!val)
		SCAssertFailed(exp, file, line);
}

		// these assertions will be turned off when debugging is turned off
#if SCDEBUG > 1
	#define scDebugAssert(exp)	scAssert(exp)
#else
	#define scDebugAssert(exp)
#endif

		// these assertions will remain in the code
#define scAssert(exp) 	scFuncAssert((int)(exp), scString(#exp), scString(__FILE__), __LINE__)

#ifndef MAX
	#define MAX(a, b)		((a) > (b) ? (a) : (b))
	#define MIN(a, b)		((a) > (b) ? (b) : (a))
#endif

#ifndef ABS
	#define ABS(a)			((a) > 0 ? (a) : -(a))
#endif
		   
// @enum status | This enumeration lists errors returned or thrown.

typedef enum eStatusTypes { 
	scERRcoordOverflow	= -13,	// @emem	Coordinate overflow.
	scERRexception		= -12,	// @emem	An exception.
	scERRmemReformat	= -11,	// @emem	Memory failure during reformatting.
	scERRassertFailed	= -10,	// @emem	Assertion failure.
	scERRnotImplemented = -9,	// @emem	Not implemented.
	scERRbounds 		= -8,	// @emem	Input is out of bounds.
	scERRinput			= -7,	// @emem	Invalid input.
	scERRlogical		= -6,	// @emem	A logical error has occured.
	scERRstructure		= -5,	// @emem	Application may have a model inconsistency
	scERRidentification = -4,	// @emem	Never heard of this object/pointer.
	scERRmem			= -3,	// @emem	Memory failure.
	scERRfile			= -2,	// @emem	File I/O error.
	scERRgeneral		= -1,	// @emem	General failure.
	scSuccess			= 0,	// @emem	Successful completion.
	scNoAction			= 1,	// @emem	Not an error, but nothing was done.
	scUserAbort 		= 2 	// @emem	User aborted the action.
} status;


// @enum eSpecTask | These are tasks that are need to be done when a
// <t TypeSpec> has changed. Use in <t SCENG_ChangedTS>.

typedef enum eRecomposeTasks {
	eSCRetabulate	= 0x01, 	// @emem	Recompute kerning, widths, etc.
	eSCRebreak		= 0x02, 	// @emem	Rebreak because rag, leading has changed.
	eSCRepaint		= 0x04, 	// @emem	Repaint because color has changed.
	eSCDoAll		= 0x07, 	// @emem	Do all the recomposition tasks.
	eSCDelete		= 0x08		// @emem	Delete any reference to this spec in any internal cache.
								// The client should have already removed any text with this spec. 
} eSpecTask;

// the RLU system that is used by the Toolbox - all calls returning 
// an RLU will be expected to use this system, it may be changed if
// necessary
#define scBaseRLUsystem 	2048

typedef enum eColShapeTypes {
	eNoShape		= 0x00,
	eVertShape		= 0x01,
	eRgnShape		= 0x02,
	eIrregShape 	= 0x03, 	/* all three vert, strip, and region bits */
	eVertFlex		= 0x10,
	eHorzFlex		= 0x20,
	eFlexShape		= 0x30
} eColShapeType;

/* direction of text flow */
typedef enum eTextDirections {
	eLeftToRight = 1,
	eRightToLeft,
	eTopToBottom,
	eBottomToTop,
	eInvalidFlow	= 0xee
} eTextDirection;

// @enum	eCommonFlow | These are common text flows.

typedef enum eCommonFlows {
	eNoFlow,
	eRomanFlow, 			// @emem	Char flow is left to right and line flow is top to bottom.
	eVertJapanFlow, 		// @emem	Char flow is top to bottom and line flow is right to left.
	eVertChineseFlow,		// @emem	Char flow is top to bottom and line flow is left to right.
	eBidiFlow				// @emem	Char flow is right to left ( most of the time ) and line flow is top to bottom.
} eCommonFlow;

// @class This class defines text flow in a text container. ( e.g. Horizontal
// flow is used in Roman text, Vertical is used in Kanji. )

class scFlowDir {
public:
	scFlowDir() :
			linedir_( eInvalidFlow ),
			glyphdir_( eInvalidFlow )
			{
			}
	scFlowDir( eTextDirection linedir, eTextDirection glyphdir ) :
			linedir_( linedir ),
			glyphdir_( glyphdir_ )
			{
			}
	scFlowDir( eCommonFlow flow )
			{
				SetFlow( flow );
			}
	scFlowDir( const scFlowDir& fd ) :
			linedir_( fd.linedir_ ),
			glyphdir_( fd.glyphdir_ )
			{
			}


	void		SetFlow( eCommonFlow );
	eCommonFlow GetFlow() const;
	
	BOOL		IsVertical( void ) const
					{
						return linedir_ == eRightToLeft;
					}
	BOOL		IsHorizontal( void ) const
					{
						return glyphdir_ == eLeftToRight;
					}
	
	void		Invalidate( void )
					{
						linedir_ = eInvalidFlow, glyphdir_ = eInvalidFlow;
					}
	BOOL		IsValid( void ) const
					{
						return linedir_ != eInvalidFlow && glyphdir_ != eInvalidFlow;
					}
	
		// return a crossflow structure
	void		CounterFlow( const scFlowDir& fd )
					{
						if ( fd.IsVertical() )
							SetFlow( eRomanFlow );
						else
							SetFlow( eVertJapanFlow );
					}

	scFlowDir&	operator=( const scFlowDir& fd )
					{
						linedir_ = fd.linedir_;
						glyphdir_ = fd.glyphdir_;
						return *this;
					}

	int 		operator==( const scFlowDir& fd ) const
					{
						return linedir_ == fd.linedir_ && glyphdir_ == fd.glyphdir_;
					}
	
	int 		operator!=( const scFlowDir& fd ) const
					{
						return linedir_ != fd.linedir_ || glyphdir_ != fd.glyphdir_;
					}

	void		SetLineDir( eTextDirection linedir )
					{
						linedir_ = linedir;
					}
	void		SetCharDir( eTextDirection chardir )
					{
						glyphdir_ = chardir;
					}

	eTextDirection	GetLineDir( void ) const
					{
						return linedir_;
					}
	eTextDirection	GetCharDir( void ) const
					{
						return glyphdir_;
					}	
	
private:
	eTextDirection	linedir_;		// @member linedir_ | <t eTextDirection> Line flow within container.
	eTextDirection	glyphdir_;		// @member glyphdir_ | <t eTextDirection> Char flow within line.
};



/*======================================================================*/
// CHARACTER TRANSFORMATIONS

typedef enum eCharTrans {

	eNormalTran,			// clear alt code
	eLowerTran, 		// transform all alphabetic characters to lowercase
	eUpperTran, 		// transform all alphabetic characters to uppercase
	eCapitalizeTran,		// transform all alphabetic characters to lowercase 
						// except initial characters of words, which are
						// transformed to caps
	eNoBreakTran,		// convert string to nobreak
	eAllowBreakTran,	// convert string to nominal brakable characters
	eChangeCaseTran,	// toggle case
	eRenMojiTran,		// apply ren-moji to indicated characters
	eRubiTran,			// apply rubi to indicated characters
	eWarichuTran,
	eRemoveJapTran		// remove the japanese transformations
} eChTranType;


/* this is used to in the ConToSys and SysToCon calls */

class SystemMemoryObject {
public:
	SystemMemoryObject();
	~SystemMemoryObject();

	long			HandleSize( void );
	status			SetHandleSize( long );
	void*			LockHandle( void );
	void			UnlockHandle( void );

	void			ReleaseMem( void ); 	// the object no longer owns the memory
private:
	HANDLE			fSYSHandle;
};


/*					BASIC GEOMETRY STUFF								*/

typedef enum eCoordinateSystems {
	eNoQuad,
	eFirstQuad,
	eSecondQuad,
	eThirdQuad,
	eFourthQuad
} eCoordSystem;

// @class This class is used to represent a x/y coordinate.
class scMuPoint {
public:
			scMuPoint() 								{ Invalidate(); }
			scMuPoint( MicroPoint xx, MicroPoint yy )	{ x = xx, y = yy; }
			scMuPoint( const scMuPoint& mp )			{ x = mp.x, y = mp.y; }

				// @meth void | Translate | Move the object by this much.
				// @parm MicroPoint | dx | Horizontal shift.
				// @parm MicroPoint | dy | Vertical shift.		
	void	Translate( MicroPoint xx, MicroPoint yy )	{ x += xx, y += yy; }

				// @meth void | Translate | Move the object by this much.
				// @parm const scMuPoint& | delta_point | Point to shift by.
	void	Translate( const scMuPoint& mp )			{ x += mp.x, y += mp.y; }

	void	Set( MicroPoint xx, MicroPoint yy ) 		{ x = xx, y = yy; }
	void	Invalidate( void )							{ Set( kInvalMP, kInvalMP ); }

	void	FourthToThird( MicroPoint );
	void	ThirdToFourth( MicroPoint );



	scMuPoint&	operator=( const scMuPoint& pt )
						{ x = pt.x, y = pt.y; return *this; }
	
	int 		operator==( const scMuPoint& pt ) const
						{ return x == pt.x && y == pt.y; }
	int 		operator!=( const scMuPoint& pt ) const
						{ return x != pt.x || y != pt.y; }
	
				// @meth scMuPoint& | operator+ | Add two scMuPoints.
				// @rdesc	An reference to a new scMuPoint with the sum of the object and the args coordinates.
	scMuPoint	operator+( const scMuPoint& mp ) const
								{ return scMuPoint( x + mp.x, y + mp.y ); }
	scMuPoint	operator-( const scMuPoint& mp ) const
								{ return scMuPoint( x - mp.x, y - mp.y ); }
	
	scMuPoint&	operator+=( const scMuPoint& mp )
								  { Translate( mp ); return *this; }
	scMuPoint&	operator-=( const scMuPoint& mp )
								  { Translate( -mp.x, -mp.y ); return *this; }

	MicroPoint	x;
	MicroPoint	y;
};


// @class This class is for representing width and depth ( size ) in
// graphical objects as opposed to position ( x, y ).
class scSize {
public:
	scSize( MicroPoint width = 0, MicroPoint depth = 0 )
					{ w = width, d = depth; }
	scSize( const scSize& sz )
					{ w = sz.w, d = sz.d; }

	MicroPoint	Width( void ) const { return w; }
	MicroPoint	Depth( void ) const { return d; }	

	void		SetWidth( MicroPoint width )	{ w = width; }
	void		SetDepth( MicroPoint depth )	{ d = depth; }

	void		Set( MicroPoint width = 0, MicroPoint depth = 0 )
					{ w = width, d = depth; }

	void		Grow( const scSize& sz )
					{ w += sz.Width(), d += sz.Depth(); }
	void		Grow( MicroPoint width, MicroPoint depth )
					{ w += width, d += depth; }
	
	scSize& 	operator=( const scSize& sz )
					{ w = sz.Width(), d = sz.Depth(); return *this; }
	
	int 		operator==( const scSize& sz ) const
					{ return w == sz.Width() && d == sz.Depth(); }
	int 		operator!=( const scSize& sz ) const
					{ return w != sz.Width() || d != sz.Depth(); }
	

	scSize operator+( const scSize& sz )
					{ return scSize( w + sz.Width(), d + sz.Depth() ); }
	scSize operator-( const scSize& sz )
					{ return scSize( w - sz.Width(), d - sz.Depth() ); }

	
	scSize& operator+=( const scSize& sz )
					{ Grow( sz ); return *this; }
	scSize& operator-=( const scSize& sz )
					{ Grow( -sz.Width(), -sz.Depth() ); return *this; }
	
private:
	MicroPoint w;
	MicroPoint d;
};

// @class This class is for representing a rectangle.
class scXRect {
public:
				scXRect() { x1 = y1 = LONG_MAX; x2 = y2 = LONG_MIN; }
				
				scXRect( MicroPoint xx1, MicroPoint yy1,
						 MicroPoint xx2, MicroPoint yy2 )
							{
								x1 = xx1;
								y1 = yy1;
								x2 = xx2;
								y2 = yy2;
							}

				scXRect( const scXRect& xr ){ x1 = xr.x1, y1 = xr.y1, x2 = xr.x2, y2 = xr.y2; }

				
#if SCDEBUG > 1 
	scChar* 	DebugStr( scChar* buf, int factor = 1 ) const;
#endif

	MicroPoint	Width( void ) const { return x2 - x1; }
	MicroPoint	Depth( void ) const { return y2 - y1; }

	void		Set( MicroPoint xx1, MicroPoint yy1, MicroPoint xx2, MicroPoint yy2 )
						{ x1 = xx1, y1 = yy1, x2 = xx2, y2 = yy2; }

	void		Set( scMuPoint& upperLeft, scMuPoint& lowerRight )
						{
							x1 = upperLeft.x;
							y1 = upperLeft.y;
							x2 = lowerRight.x;
							y2 = lowerRight.y;
						}

	void		Translate( const scMuPoint& pt )
						{
							scDebugAssert( Valid( eFourthQuad ) );

							x1 += pt.x;
							x2 += pt.x;
							y1 += pt.y;
							y2 += pt.y;
						}
	void		Translate( MicroPoint x, MicroPoint y )
						{
							scDebugAssert( Valid( eFourthQuad ) );

							x1 += x;
							x2 += x;
							y1 += y;
							y2 += y;
						}

	void		MoveTo( MicroPoint x, MicroPoint y )
						{
							scDebugAssert( Valid( eFourthQuad ) );
							
							x2 = x + Width();
							y2 = y + Depth();
							x1 = x;
							y1 = y;
						}

	void		MoveTo( const scMuPoint& pt )
						{
							scDebugAssert( Valid( eFourthQuad ) );
							
							x2 = pt.x + Width();
							y2 = pt.y + Depth();
							x1 = pt.x;
							y1 = pt.y;
						}

	void		Scale( TenThousandth );
	void		Scale( REAL );
	
	void		Inset( MicroPoint x, MicroPoint y )
						{
							scDebugAssert( Valid( eFourthQuad ) );

							x1 += x;
							x2 -= x;
							y1 += y;
							y2 -= y;

							scDebugAssert( Valid( eFourthQuad ) );
						}

	const		scMuPoint& UpperLeft( void ) const
					{ return *(const scMuPoint*)this; }
					
	const		scMuPoint& LowerRight( void ) const
					{ return *((const scMuPoint *)this + 1); }

	void		Union(	const scXRect &exrect )
					{
						if ( Valid( eFourthQuad )  && exrect.Valid( eFourthQuad ) ) {
							x1 = MIN( exrect.x1, x1 );
							y1 = MIN( exrect.y1, y1 );
							x2 = MAX( exrect.x2, x2 );
							y2 = MAX( exrect.y2, y2 );
						}
						else if ( exrect.Valid( eFourthQuad ) )
							operator=( exrect );
					}

	BOOL		Intersect( const scXRect& exrect )
					{
						if ( !Valid( eFourthQuad ) )
							return false;
		
						if ( !exrect.Valid( eFourthQuad ) )
							return false;

						x1 = MAX( exrect.x1, x1 );
						y1 = MAX( exrect.y1, y1 );
						x2 = MIN( exrect.x2, x2 );
						y2 = MIN( exrect.y2, y2 );
						return Valid( eFourthQuad );
					}
	
	BOOL		Contains( const scXRect& exrect ) const
					{
						return PinRect( exrect.UpperLeft() )	&&
							   PinRect( exrect.LowerRight() );
					}



				// force pt to be in rect
	void		Clamp( scMuPoint& pt ) const
					{
						scDebugAssert( Valid( eFourthQuad ) );		

						pt.x = MAX( pt.x, x1 );
						pt.y = MAX( pt.y, y1 );

						pt.x = MIN( pt.x, x2 );
						pt.y = MIN( pt.y, y2 );
					}

	BOOL		PinRect( const scMuPoint& pt ) const
					{
						scDebugAssert( Valid( eFourthQuad ) );				
						return pt.x >= x1 && pt.x <= x2 && pt.y >= y1 && pt.y <= y2;
					}


	void		Invalidate( void )
					{
						x1 = LONG_MAX;
						y1 = LONG_MAX;
						x2 = LONG_MIN;
						y2 = LONG_MIN;
					}

		
	BOOL		Valid( eCoordSystem coordSys = eFourthQuad ) const;

		// our coordinate system is typically fourth quadrant
		// but occasionally we need to switch it and here
		// are some routines
	void		FourthToThird( MicroPoint );
	void		ThirdToFourth( MicroPoint );
	
	void		FourthToFirst( MicroPoint );
	void		FirstToFourth( MicroPoint );
	
	scXRect&	operator=( const scXRect& xrect )
						 { 
							x1 = xrect.x1;
							y1 = xrect.y1;
							x2 = xrect.x2;
							y2 = xrect.y2;
							return *this;
						 }



	int 		operator==( const scXRect& xrect ) const
						{	return	   x1 == xrect.x1
									&& y1 == xrect.y1
									&& x2 == xrect.x2
									&& y2 == xrect.y2;
						}
						
	int 		operator!=( const scXRect& xrect ) const
						{
							return	   x1 != xrect.x1
									|| y1 != xrect.y1
									|| x2 != xrect.x2
									|| y2 != xrect.y2;
						}
	MicroPoint	x1; 	// @member x1 | <t MicroPoint> left.
	MicroPoint	y1; 	// @member y1 | <t MicroPoint> top.
	MicroPoint	x2; 	// @member x2 | <t MicroPoint> right.
	MicroPoint	y2; 	// @member y2 | <t MicroPoint> bottom.
	
private:
	scMuPoint&	AlterUpperLeft( void )
						{ return *(scMuPoint *)this; }	
	scMuPoint&	AlterLowerRight( void )
						{ return *((scMuPoint *)this + 1); }
};


// @class This class is used for representing a rectangle using RLU's
// which are 16 bit quantities.
class scRLURect  {
public:
		// rectangle for character extents expressed in relative units
	RLU 	rluLeft;
	RLU 	rluTop;
	RLU 	rluRight;
	RLU 	rluBottom;			

			scRLURect();
			scRLURect( const scRLURect& );
			scRLURect( RLU, RLU, RLU, RLU );

	BOOL	Valid( eCoordSystem coordSystem = eFourthQuad ) const;
	void	Set( RLU, RLU, RLU, RLU );	
//	void	RLUSetRect( Rect& );

	void	FirstToFourth( RLU );
	void	FourthToFirst( RLU );
	
//	void	Translate( Point& );
	void	Translate( RLU, RLU );

	void	Scale( TenThousandth );
	void	Scale( REAL );

	void	RLURomanBaseLineToCenter( void );
	void	RLURomanBaseLineToLeft( void );
	void	RLURomanBaseLineToRight( void );
	void	RLURomanBaseLineToMiddle( void );
	void	RLURomanBaseLineToBottom( void );
	void	RLURomanBaseLineToTop( void );

	void	Invalidate( void );
};
		  
#if scBaseRLUsystem == 1000
	#define RLU_BASEfmBottom	170
#elif scBaseRLUsystem == 2048
	#define RLU_BASEfmBottom	348
#else 
	#define RLU_BASEfmBottom	(scBaseRLUsystem*17/100)
#endif

#define RLU_BASEfmTop		(scBaseRLUsystem - RLU_BASEfmBottom)


/*							VERTEX										*/	
// @enum ePointType | Vertex definitions for polygons.
	
typedef enum ePointTypes {
	eFinalPoint = -1,		// @emem Final point of the polygons.
	eNoPoint,				// @emem Invalid point.
	eStartPoint = 1,		// @emem Start point of a polygon.	
	eStopPoint, 			// @emem Stop point of a polygon.
	eCornerPoint,			// @emem Corner point.
	eTangentPoint,			// @emem Tangent.
	eBezControlPoint		// @emem Bezier control point.
} ePointType;

// @class scVertex | This class is used to represent a vertex in a polygon.
// Vertices are sets of closed polygons. StartPoint starts a polygon and 
// stopPoint closes a polygon and finalPoint ends the vertex list.
// Coordinates are absolute, no deltas.<nl>
// BEZIERs are in the form<nl>
// [corner or tangent point], [bezControlPoint], [bezControlPoint], [corner or
// tangent point]
// @ex To describe one square inside another; |
//	
//	10, 10, startPoint			; square one
//	20, 10, cornerPoint
//	20, 20, cornerPoint
//	10, 20	cornerPoint
//	10, 10	stopPoint
//	0,	0,	startPoint			; square two
//	30, 0,	cornerPoint
//	30, 30, cornerPoint
//	0,	30, cornerPoint
//	0,	0,	finalPoint

class scVertex : public scMuPoint {
public:
				scVertex()							{ fPointType = eNoPoint; }
				scVertex( MicroPoint xx, MicroPoint yy, ePointType pt ) :
						scMuPoint( xx, yy ) 		{ fPointType = pt; }
				scVertex( const scVertex& vt )		{ x = vt.x, y = vt.y, fPointType = vt.fPointType; }
	
	ePointType	fPointType; 	// @member fPointType |  <t ePointType>.
	
	int operator==( const scVertex& v ) const
			{ return x == v.x	&& y == v.y && fPointType == v.fPointType; }
	
	int operator!=( const scVertex& v ) const
			{	return x != v.x || y != v.y || fPointType != v.fPointType; }
};




	
/*						SELECTION STUFF 								*/

// SELECTION TYPES, these apply to strings of characters that make up the 
// following selection types
typedef enum eCursContentMovements {
	eCursBackward,
	eCursNoMovement,
	eCursForward
} eContentMovement;


// @enum eSelectModifier | Selection modifiers.
typedef enum eSelectModifiers {
	eCharSelect,		// @emem Characer selection.
	eWordSelect,		// @emem String selection of all chars in the hit word.
	eParaSelect,		// @emem String selection of all chars in the hit paragraph.
	eLineSelect,		// @emem String selection of all chars in the hit line.
	eColumnSelect,		// @emem String selection of all chars in the hit column.
	eAllSelect			// @emem Select the entire stream.
} eSelectModifier;


// @enum eSelectMove| Selection movement.
typedef enum eSelectionMovements {
	ePrevChar,			/* find the prev character */
	eNextChar,			/* find the next character */
	ePrevCharInPara,	/* find the prev character and do not leave the paragraph */
	eNextCharInPara,	/* find the next character and do not leave the paragraph */
	ePrevWord,			/* find the prev token */
	eNextWord,			/* find the next token */
	ePrevSpellWord, 	/* find the prev alphabetic token */
	eNextSpellWord, 	/* find the next alphabetic token */
	eStartWord, 		/* go to start of word */
	eEndWord,			/* go to end of word */
	ePrevLine,			/* go to prev line */
	eNextLine,			/* go to next line */
	ePrevEntireLine,	/* select the entire prev line */
	eNextEntireLine,	/* select the entire next line */
	eStartLine, 		/* go to start of line */
	eEndLine,			/* go to end of line */
	ePrevPara,			/* select entire prev para */
	eNextPara,			/* select entire next para */
	eBeginPara, 		/* go to beginning of para */
	eEndPara,			/* go to end of para */
	eFirstPara, 		/* select the first para in stream */
	eLastPara,			/* select the last para in stream */
	ePrevEntireColumn,	/* select entire prev column */
	eNextEntireColumn,	/* select entire next column */
	eBeginColumn,		/* go to beginning of column */
	eEndColumn, 		/* go to end of column */
	eStartStream,		/* go to the start of the stream */
	eEndStream			/* go to the end of the stream */
} eSelectMove;

	
// @struct scStreamLocation | This structure contains alot
// of information about the text at its location, this may be used
// store state or to display status.
	
class scStreamLocation {
public:
	scStream*	fStream;		// @field <c scStream> containing this selection.
	APPColumn	fAPPColumn; 	// @field <t APPColumn> containing the selection OR 
								// the last container containing the stream
								// if the text flows out of the container
	long		fParaNum;		// @field The para #  containing this selection
	long		fParaOffset;	// @field Offset into paragraph of character
	BOOL		fEndOfLine; 	// @field True if cursor is at end of line
	UCS2		fTheCh; 		// @field The character that is at this point.
	ulong		fFlags; 		// @field The flags on the character
	eUnitType	fUnitType;
	MicroPoint	fTheChWidth;	// @field The character escapement
	TypeSpec	fChSpec;		// @field Spec at this point
	TypeSpec	fParaSpec;
	MicroPoint	fPosOnLine; 	// @field Start position of character
	MicroPoint	fSelMaxX;		// @field Max x pos for up or down arrow
	APPFont 	fFont;			// @field Font at location
	MicroPoint	fPointSize; 	// @field Pointsize at location
	MicroPoint	fBaseline;		// @field Baseline at location
	MicroPoint	fMeasure;		// @field Line measure
	MicroPoint	fLetterSpace;	// @field Letter space
	MicroPoint	fWordSpace; 	// @field Word space

						scStreamLocation();
						scStreamLocation( const scStreamLocation& );
						
	scStreamLocation&	operator=( const scStreamLocation& );
	
	void		Init( void );

		// this tests the equality of the offsets within a stream
	BOOL		OffsetsEqual( const scStreamLocation& pl ) const
							{ return fStream		== pl.fStream &&
									 fParaNum		== pl.fParaNum &&
									 fParaOffset	== pl.fParaOffset; }
};




/* ===================	 KEYSTROKE RECORD	=========================== */
// @class scKeyRecord | This contains the data necessary for inserting
// keystrokes and performing undos.
class scKeyRecord {
public:
	enum Type {
		insert,
		overstrike
	};

	scKeyRecord();
	scKeyRecord( const scKeyRecord& );				
	~scKeyRecord(); 			

						// inverts the keystrokes in preparation 
						// for an undo
	void		Invert( void );
	
	UCS2		keycode() const
				{
					return fKeyCode;
				}
	UCS2&		keycode()
				{
					return fKeyCode;
				}

	Type		type() const
				{
					return type_;
				}
	Type&		type()
				{
					return type_;
				}

	UCS2		replacedchar() const
				{
					return fReplacedChar;
				}
	UCS2&		replacedchar()
				{
					return fReplacedChar;
				}

	uint8		field() const
				{
					return field_;
				}
	uint8&		field()
				{
					return field_;
				}

	uint8		replacedfield() const
				{
					return replacedfield_;
				}
	uint8&		replacedfield()
				{
					return replacedfield_;
				}

	GlyphSize	escapement() const
				{
					return fEscapement;
				}
	GlyphSize&	escapement()
				{
					return fEscapement;
				}

	const TypeSpec& spec() const 
				{
					return fSpec;
				}
	TypeSpec&	spec()
				{
					return fSpec;
				}

	BOOL		noop() const
				{
					return fNoOp;
				}
	BOOL&		noop()
				{
					return fNoOp;
				}

	BOOL		restoreselect() const
				{
					return fRestoreSelect;
				}
	BOOL&		restoreselect()
				{
					return fRestoreSelect;
				}

	const scStreamLocation& mark() const
				{
					return fMark;
				}
	scStreamLocation& mark()
				{
					return fMark;
				}

	scKeyRecord& operator=( const scKeyRecord& );

private:
	Type		type_;			// insert or overstrike 				  
	UCS2		fKeyCode;		// @cmember The keycode.
	UCS2		fReplacedChar;	// @cmember Replaced char in case of backspace.   
	uint8		field_;
	uint8		replacedfield_;
	GlyphSize	fEscapement;	// @cmember 0 in most cases - val for fixed/rel spaces.
	TypeSpec	fSpec;			// @cmember The spec to associate with that character, 
								// NULL means use spec of previous character.
	BOOL		fNoOp;			// @cmember True if character has no effect 
								// (such as arrow beyond stream).

	BOOL		fRestoreSelect; // @cmember On initial insertion, this is set to false.
								// In some cases, the toolbox will save the
								// selection point in the mark field and set
								// restoreSelection to true. This should
								// signal the caller to save the contents of
								// the mark field for undo. During undo, if
								// restoreSelection is true, the toolbox will
								// reset the selection point to the value
								// stored in mark.
	scStreamLocation	fMark;	// @cmember Should initially be set to NULL. 
								// In some cases, the toolbox will save
								// the selection point in this field, and set
								// restoreSelection to true to indicate it
								// is doing so. The caller should store
								// this value and pass it back to the toolbox
								// (with restoreSelection == true) for undo.
};

	
/*					FUNCTION POINTERS									*/
/* for spell checking */
/* 
	Substitution function
	inWord and outWord are null terminated strings
	outWord is allocated and freed by application

the substitution function returns	
	- successful	if the word has been changed
	- noAction		if no change is necessary
	- other error	to be propogated back to app
*/

typedef  status (*SubstituteFunc)( UCS2 **outWord, UCS2 *inWord, scSelection* );

/*====================================================================*/
// @type HiliteFuncPtr | The hilite function is a call back from the toolbox
// requesting the client to xor an area. The area is in object coordinates.
// The <t APPDrwCtx> has been determined by <f APPDrawContext> - this will
// have the container in it allowing the client to set up the drawing context
// and the transform from object to device coordinates.

typedef void (*HiliteFuncPtr)( const scXRect&, APPDrwCtx, BOOL sliverCursor );

/*====================================================================*/
// @type IOFuncPtr | The I/O function pointer points to function that takes
// the following arguments, think of it as an abstract definition
// for standard library "read" or "write".<nl>
//	long (*IOFuncPtr)( <t APPCtxPtr> contextPtr, void* data, long inBytes );<nl>
//	return value is a long that specifies :<nl>
//		number of bytes read or written,<nl>
//		in the case of an error a negative value is returned,<nl>
//		end of file on a read is detected by zero bytes being read
//			(e.g. return value of 0L)
//


typedef long (*IOFuncPtr)( APPCtxPtr, void*, long inBytes );

/* ================= STYLE RELATED DATA =============================== */
// linebreaker to use
typedef enum eTSCompLangs {
	eCompRoman		= 0,
	eCompJapanese		// for JIS4051 compostion
} eTSCompLang;


typedef enum eEmphasisTypes {
	eNormal 	= 0x0000,
	eItalic 	= 0x0001,
	eBold		= 0x0002,
	eUnderline	= 0x0004,
	eStrikeThru = 0x0008
} eEmphasisType;

/* Track Kerning types */
typedef enum TSTrackings {
	kTRnoTrack,
	kTRvLoose,
	kTRloose,
	kTRnormal,
	kTRtight,
	kTRvTight
} TSTracking;

// @enum eTSJust | Justification bits
typedef enum eTSJustTypes {
	eNoRag			= -1,
	eRagCentered	= 0x0000,			// @emem Rag centered. 
	eRagRight		= 0x0001,			// @emem Rag right.
	eRagLeft		= 0x0002,			// @emem Rag left.
	eRagJustified	= 0x0003,			// @emem Justified.
	eFlushLeft		= 0x0001,			// @emem Same as rag right.
	eFlushRight 	= 0x0002,			// @emem Same as rag left
	eRagFlag		= 0x000f,			// @emem Boolean op flag.
	eLastLineJust	= 0x0010,			// @emem Justify the last line of a para.
	eHangPuncLeft	= 0x0100,			// @emem Hang left.
	eHangPuncRight	= 0x0200,			// @emem Hang right.
	eHangFlag		= 0x0f00			// @emem Boolean op flag for hanging.
} eTSJust;


// @enum eVertJust | Vertical justification bits
typedef enum TSVertJustifications {
	eVertCentered	= 1,	// @emem Center the lines.
	eVertTop,				// @emem Lines are aligned to the top.
	eVertBottom,			// @emem Lines are aligned to the bottom.
	eVertJustified, 		// @emem Lines are justified, if the coolumn
							// in not the last column in a flow set.  
	eVertForceJustify		// @emem Lines are justified, even if the
							// column is the last column in a flow set.  
} eVertJust;


// @enum eTabAlign | Tab alignments.
typedef enum eTabAlignments {
	eTBNoAlign,
	eTBLeftAlign,						// @emem Left alignment
	eTBRightAlign,						// @emem Right alignment.
	eTBCenterAlign, 					// @emem Center alignment.
	eTBDecimalAlign 					// @emem Decimal alignment.
} eTabAlign;

// @enum eFCAlignment | Fill character alignment.
typedef enum FillCharAlignments {
	eFCNormalAlign, 		// @emem Normal - align on integral width boundary.
	eFCAltAlign,			// @emem Alternate from one line to next by
							// half fill char width.
	eFCLeftAlign,			// @emem Left align.
	eFCRightAlign,			// @emem Right align.
	eFCJustAlign			// @emem Justify alignment glyph.
} eFCAlignment;

struct scLeaderChar {
	UCS2	one;
	UCS2	two;
};		/* up to two characters */


// @struct scTabInfo | Tab position information

class scTabInfo {
public:
	scTabInfo( MicroPoint mp = 0, eTabAlign ta = eTBNoAlign ) :
			xPosition( mp ),
			tabAlign( ta ),
			fillCharAlign( eFCNormalAlign )
			{
				fillChar = 0;
			}
							
	void			init()
						{
							xPosition		= 0;
							tabAlign		= eTBNoAlign;
							fillChar		= 0;
							fillCharAlign	= eFCNormalAlign;
						}
	
	scTabInfo&		operator=( const scTabInfo& ti )
							 {
								xPosition		= ti.xPosition;
								fillChar		= ti.fillChar;
								tabAlign		= ti.tabAlign;
								fillCharAlign	= ti.fillCharAlign;
								return *this;
							 }
	
	int 			operator==( const scTabInfo& ti ) const
							 {
								if ( tabAlign == eTBNoAlign &&
									 ti.tabAlign == eTBNoAlign )
									return 1;

								return	xPosition		== ti.xPosition &&
										fillChar		== ti.fillChar	&&
										tabAlign		== ti.tabAlign	&&
										fillCharAlign	== ti.fillCharAlign;
							 }
	
	int 			operator!=( const scTabInfo& ti ) const
							{
								return !operator==( ti );
							}

	MicroPoint		xPosition;		// @field Where to position the tab.
	UCS2			fillChar;		// @field Character to use as a fill character.
	// scLeaderChar fillChar;
	eTabAlign		tabAlign;		// @field <t eTabAlign>
	eFCAlignment	fillCharAlign;	// @field Alignment for fill character.

	
};

// baseline positioning of characters
typedef enum eFntBaselines {
	kNoBaseline,		// this will generate an exception if
						// metric info is requested and no baseline 
						// is set
		// horizontal flow
	kTopBaseline,
	kMiddleBaseline,
	kBottomBaseline,
	kRomanBaseline,
		// vertical flow
	kLeftBaseline,
	kCenterBaseline,
	kRightBaseline
} eFntBaseline;

// position of characters within em square
typedef enum eCharPositions {
	eLeftPosition,
	eCenterPosition,
	eRightPosition
} eCharPosition;


// @struct scParaColBreak | Used for specifying widow/orphan control.
class scParaColBreak {
public:
	scParaColBreak() :
		fLinesBefore( 0 ),
		fLinesAfter( 0 ),
		fNoBreak( false ),
		fKeepWithNext( false ){}

	scParaColBreak( short lb, short la, BOOL nb, BOOL kwn ) :
		fLinesBefore( lb ),
		fLinesAfter( la ),
		fNoBreak( nb ),
		fKeepWithNext( kwn ){}				

	void	Set( short lb, short la, BOOL nb, BOOL kwn )
					{
						fLinesBefore	= lb;
						fLinesAfter 	= la;
						fNoBreak		= nb;
						fKeepWithNext	= kwn;			
					}

	short	LinesBefore() const
				{
					return fLinesBefore;
				}
	
	short	LinesAfter() const
				{
					return fLinesAfter;
				}
	
	BOOL	NoBreak() const
				{
					return fNoBreak;
				}
	
	BOOL	KeepWithNext() const
				{
					return fKeepWithNext;
				}
	
private:
	short	fLinesBefore;		// @field Min lines before a column break.
	short	fLinesAfter;		// @field Min lines after a column break.
	BOOL	fNoBreak;			// @field Do not break this paragraph
								// across a column.
	BOOL	fKeepWithNext;		// @field Keep this with the next paragraph.
};

/* ================== LINEINFO ======================================== */
struct scLineInfo {
	const scTextline*	fLineID;
	scMuPoint			fOrg;
	scXRect 			fInkExtents;
	scXRect 			fLogicalExtents;	// interpret this knowing the flow direction
	MicroPoint			fAscenderHite;
	MicroPoint			fXHite;  
	MicroPoint			fCapHite;
	MicroPoint			fDescenderHite;
};


/* ================= CHARACTER DRAWING CALL BACK DATA ================= */
// extra space in character escapement
typedef enum eSpacePositions {
	eNoSpace,
	eTrailingSpace,
	eLeadingSpace
} eSpacePos;

// @struct scGlyphArray | Glyph id and escapements.
struct scGlyphArray {
	GlyphID 	fGlyphID;	// @field Character code.
	GlyphSize	hEscapement;// @field Horz width to escape after drawing glyph.
	GlyphSize	vEscapement;// @field Vert width to escape after drawing glyph.
	
#ifdef scJIS4051	
	eSpacePos	pos_;		// flags for positioning glyphs
#endif
};

// @struct scGlyphInfo | Structure containing information to draw glyphs.

class scGlyphInfo {
public:
	APPFont 		fontname;	// @field Font for this run of glyphs.
	eEmphasisType	emphasis_;
	APPRender		fontRender; // @field Additional info on font rendering.
	TypeSpec		typespec;	// @field The <t TypeSpec> associated with these glyphs,
								// there is only one per run.
	APPColor		color;		// @field <t APPColor> pass thru color for run of glyphs.
	GlyphSize		pointsize;	// @field Vertical size for these glyphs. 
	GlyphSize		setsize;	// @field Horizontal size for these chars.
	scAngle 		oblique;	// @field Pseudo italicization.
	scAngle 		rotation;	// @field Glyph rotation - not container rotation!.
	eFntBaseline	fBaseline;	// @field Baseline for drawing.
	scFlowDir		flowDir;	// @field <c scFlowDir> flowing direction of this run.
};

struct USTR
{
	const WCHAR* ptr;
	unsigned long len;
};
