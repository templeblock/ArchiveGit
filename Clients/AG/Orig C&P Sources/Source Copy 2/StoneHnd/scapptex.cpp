/***************************************************************************

	File:		SCAPPTEX.C


	$Header: /Projects/Toolbox/ct/SCAPPTEX.CPP 2	 5/30/97 8:45a Wmanis $
	
	Contains:
		This module takes a handle to APPTextRun structure and manages the
		memory (specs & chars) associated with it. Locking and unlocking. It
		also maintains some internal structures that used for reading the text
		between the lock and unlock calls. Refer to the 
		APPTextRun in SCTextExch.h.
	
	Written by: Manis

	Copyright (c) 1989-94 Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

*****************************************************************************/

#include "scapptex.h"
#include "scparagr.h"

/* ==================================================================== */

stPara::stPara( ) :
	paraspec_( 0 )
{
	reset();
}

/* ==================================================================== */

stPara::stPara( TypeSpec& pspec ) :
	paraspec_( pspec )
{
	if ( pspec.ptr() )
		specs_.AppendSpec( pspec, 0 );
	reset();
}

/* ==================================================================== */

stPara::~stPara()
{
}

/* ==================================================================== */

void stPara::append( TypeSpec& ts )
{
	if ( ts != specs_.SpecAtOffset( ch_.NumItems() ) )
		 specs_.AppendSpec( ts, ch_.NumItems() );
}

/* ==================================================================== */

void stPara::append( const uchar* ch, int len )
{
	for ( int i = 0; i < len; i++ )
		ch_.Append( (UCS2)ch[i] );
}

/* ==================================================================== */

void stPara::append( stUnivString& ustr )
{
	for ( unsigned i = 0; i < ustr.len; i++ )
		ch_.Append( (UCS2)ustr.ptr[i] );
}

/* ==================================================================== */

void stPara::append( UCS2 ch )
{
	ch_.Append( ch );
}

/* ==================================================================== */

stPara& stPara::operator=( const stPara&  p )
{
	ch_.RemoveAll();
	for ( int i = 0; i < p.ch_.NumItems(); i++ )
		ch_.Append( p.ch_[i] );

	choffset_ = p.choffset_;

	specs_.RemoveAll();
	for ( i = 0; i < p.specs_.NumItems(); i++ )
		specs_.Append( p.specs_[i] );

	paraspec_ = p.paraspec_;
	return *this;
}

/* ==================================================================== */

int stPara::get( UCS2& ch, TypeSpec& spec )
{
	if ( choffset_ < ch_.NumItems() ) {
		spec = specs_.SpecAtOffset( choffset_ );
		ch = ch_[choffset_++];
		return choffset_;
	}
	return 0;
}

/* ==================================================================== */	

int stPara::validate() const
{
	scAssert( paraspec_.ptr() );
	specs_.DebugRun( "stPara::validate" );
	return 1;
}

/* ==================================================================== */

void stPara::setparaspec( TypeSpec& ts )
{
	paraspec_ = ts;
	scAssert( ch_.NumItems() == 0 );
	specs_.AppendSpec( ts, 0 );
}

/* ==================================================================== */

int stPara::complete()
{
	if ( specs_.NumItems() == 1 )
		specs_.AppendSpec( paraspec_, 0 );
	
	return validate();
}

/* ==================================================================== */

class stTIEImp : public stTextImportExport {
public:
					stTIEImp();
					~stTIEImp();

	void			release();
	
		// writing
	virtual void	StartPara( TypeSpec& );
	virtual void	SetParaSpec( TypeSpec& );
	virtual void	PutString( const uchar*, int, TypeSpec& );
	virtual void	PutString( stUnivString&, TypeSpec& );
	virtual void	PutChar( UCS2, TypeSpec& ); 
	
		// reading
	virtual int 	NextPara( TypeSpec& );
	virtual int 	GetChar( UCS2&, TypeSpec& );		
	virtual void	reset();
	virtual void	resetpara();

protected:
	stPara& 		currentPara();
	
	int32						pindex_;
	scSizeableArrayD<stPara>	paras_;
};


/* ==================================================================== */

stTextImportExport& stTextImportExport::MakeTextImportExport( int encoding )
{
	stTIEImp* stimp = new stTIEImp();
	return *stimp;
}

/* ==================================================================== */

stTIEImp::stTIEImp()
{
	reset();
}

/* ==================================================================== */

stTIEImp::~stTIEImp()
{
}

/* ==================================================================== */

void stTIEImp::release()
{
	delete this;
}

/* ==================================================================== */

stPara& stTIEImp::currentPara()
{
	return paras_[pindex_];
}

/* ==================================================================== */
// importing

void stTIEImp::StartPara( TypeSpec& ts )
{
	if ( paras_.NumItems() > 0 )
		paras_[pindex_].complete();
	
	stPara newPara( ts );
	paras_.Append( newPara );
	pindex_ = paras_.NumItems() - 1;
}

/* ==================================================================== */

void stTIEImp::SetParaSpec( TypeSpec& ts )
{
	stPara& p = currentPara();
	p.setparaspec( ts );
}

/* ==================================================================== */

void stTIEImp::PutString( const uchar* str, int len, TypeSpec& ts )
{
	stPara& p = currentPara();
	p.append( ts );
	p.append( str, len );
}

/* ==================================================================== */

void stTIEImp::PutString( stUnivString& ustr, TypeSpec& ts )
{
	stPara& p = currentPara();
	p.append( ts );
	p.append( ustr );
}

/* ==================================================================== */

void stTIEImp::PutChar( UCS2 ch, TypeSpec& ts )
{
	stPara& p = currentPara();
	p.append( ts );
	p.append( ch ); 
}

/* ==================================================================== */
// reading

int stTIEImp::NextPara( TypeSpec& ts )
{
	pindex_++;
	if ( pindex_ < paras_.NumItems() ) {
		ts = paras_[pindex_].paraspec();
		paras_[pindex_].validate();
		return pindex_;
	}
	return -1;
}

/* ==================================================================== */

int stTIEImp::GetChar( UCS2& ch, TypeSpec& ts )
{
	stPara& p = currentPara();
	return p.get( ch, ts );
}

/* ==================================================================== */

void stTIEImp::reset()
{
	pindex_ = -1;
}

/* ==================================================================== */

void stTIEImp::resetpara()
{
	stPara& p = currentPara();
	p.reset();
}

/* ==================================================================== */
