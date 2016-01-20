/****************************************************************************
Contains:	Flags for the glyph processing.
****************************************************************************/

#pragma once

#define SIZE_OF_MACHINE 	256

/* character definitions */
#define MIN_CHARACTER		START_STREAM
#define MAX_CHARACTER		(SIZE_OF_MACHINE - 1)
	
struct scCharFlags1 {
	unsigned	fFauxChar	: 16;			// for alignment purposes
	unsigned	fDiscHyph	: 1;
	unsigned	fNoBreak	: 1;
	unsigned	fHyphLevel	: 3;
	unsigned	fAutoKern	: 1;
	unsigned	fLineBreak	: 1;		// why do i need this
	
	unsigned	fField		: 8;
};

struct scCharFlags2 {
	unsigned	fauxchar_	: 16;			// for alignment purposes
	unsigned	dischyph_	: 1;
	unsigned	nobreak_	: 1;
	unsigned	hyphlevel_	: 3;
	unsigned	autokern_	: 1;
	unsigned	linebreak_	: 1;		// why do i need this
	
	unsigned	spacepos_	: 2;		// position of space leading or trailing or none in escapement
	unsigned	warichu_	: 2;		// if non-zero represent # lines	
	unsigned	rubi_		: 1;		// annotated character(s)
	unsigned	renmoji_	: 3;		// max target of 7 characters
};

class scCharFlags {
	friend class CharRecord;
	
public:

	void		ClrCJKVarious( void )
				{
					ClrVarious();
					f2_.spacepos_ = 0;
				}
	
	void		ClrVarious( void )
				{
					f1_.fLineBreak = 0;
					f1_.fHyphLevel = 0;
				}

	
	
	int 		operator==( const scCharFlags& flags ) const
				{
					return f__ == flags.f__;
				}
	
//	scCharFlags&	operator=( const scCharFlags& flags )
//				{
//					f__ = flags.f__;
//					return *this;
//				}
	
	void		SetLineBreak(void)
				{
					f1_.fLineBreak = 1;
				}
	void		ClrLineBreak(void)
				{
					f1_.fLineBreak = 0;
				}
	BOOL		IsLineBreak(void) const
				{
					return f1_.fLineBreak;
				}

	void		SetKernBits( void )
				{
					f1_.fAutoKern = 1;
				}
	void		ClrKernBits( void )
				{
					f1_.fAutoKern = 0;
				}
	BOOL		IsKernPresent( void ) const
				{
					return f1_.fAutoKern;
				}

	void		SetAutoHyphen( unsigned val )
				{
					f1_.fHyphLevel = val;
				}
	void		ClrAutoHyphen( void )
				{
					f1_.fHyphLevel = 0;
				}
	unsigned	GetHyphLevel( void ) const
				{
					return f1_.fHyphLevel;
				}

	void		SetDiscHyphen( void )
				{
					f1_.fDiscHyph = 1;
				}
	void		ClrDiscHyphen( void )
				{
					f1_.fDiscHyph = 0;
				}
	BOOL		IsDiscHyphen( void ) const
				{
					return f1_.fDiscHyph;
				}

	void		SetNoBreak( void )
				{
					f1_.fNoBreak = 1;
				}
	void		ClrNoBreak( void )
				{
					f1_.fNoBreak = 0;
				}
	BOOL		IsNoBreak( void ) const
				{
					return f1_.fNoBreak;
				}

	BOOL		IsHyphPresent( void ) const
				{
					return GetHyphLevel()||IsDiscHyphen();
				}

	void		ClrAutoBits( void )
				{
					ClrAutoHyphen();
					ClrKernBits();
				}		

	void		SetRubi( void )
				{
					f2_.rubi_ = 1;
				}
	void		ClrRubi( void )
				{
					f2_.rubi_ = 0;
				}
	BOOL		IsRubi( void ) const
				{
					return f2_.rubi_;
				}
					

	void		SetRenMoji( unsigned val )
				{
					f2_.renmoji_ = val;
				}
	void		ClrRenMoji( void )
				{
					f2_.renmoji_ = 0;
				}
	unsigned	GetRenMoji( void ) const
				{
					return f2_.renmoji_;
				}
	
	void		SetWarichu( unsigned val )
				{
					f2_.warichu_ = val;
				}
	void		ClrWarichu( void )
				{
					f2_.warichu_ = 0;
				}
	unsigned	GetWarichu( void ) const
				{
					return f2_.warichu_;
				}
			
		
	BOOL		IsSpecialNihon( void ) const
				{
					return f2_.renmoji_ || f2_.rubi_ || f2_.warichu_;
				}
	
	void		ClrSpecialNihon( void )
				{
					ClrRubi();
					ClrRenMoji();
					ClrWarichu();
				}
	
	void		SetSpacePosition( unsigned val )
				{
					f2_.spacepos_ = val;
				}
	void		ClrSpacePosition( void )
				{
					f2_.spacepos_ = 0;
				}
	unsigned	GetSpacePosition( void ) const
				{
					return f2_.spacepos_;
				}

	void		SetField( uint8 field )
				{
					f1_.fField = field;
				}	
	uint8		GetField( ) const
				{
					return (uint8)f1_.fField;
				}

	
	BOOL		IsBreakable( void ) const
				{
					return !( f2_.renmoji_ || f2_.rubi_ || f2_.warichu_ || f1_.fNoBreak );
				}
	
private:
	void		ClearMinFlags( void )
				{
					f1_.fDiscHyph	= 0;
					f1_.fNoBreak		= 0;
					f1_.fHyphLevel	= 0;
					f1_.fAutoKern	= 0;
					f1_.fLineBreak	= 0;
				}
	void		ClearAllFlags( void )
				{
					f2_.dischyph_	= 0;
					f2_.nobreak_	= 0;
					f2_.hyphlevel_	= 0;
					f2_.autokern_	= 0;
					f2_.linebreak_	= 0;
	
					f2_.spacepos_	= 0;
					f2_.warichu_	= 0;	
					f2_.rubi_		= 0;
					f2_.renmoji_	= 0;
				}

	union {
		scCharFlags1	f1_;
		scCharFlags2	f2_;
		uint32			f__;
	};
};
