#include "scappint.h"
#include "scpubobj.h"
#include "scstream.h"
#include "scselect.h"
#include "univstr.h"

class stContUnitIterImp : public stContUnitIter {
public:
	stContUnitIterImp( scStream* str, scContUnit* cu );
	stContUnitIterImp( scSelection* sel );
    virtual void    release();
    virtual void    reset();
	virtual int		gettokeniter( stTokenIter*& );
    virtual int     next();
	virtual void	range( scStreamLocation&, scStreamLocation& );

private:
	scStream*	stream_;
	scContUnit*	cu_;
	scSelection range_;
};


class stTokenIterImp : public stTokenIter {
public:
	stTokenIterImp( scContUnit*, int32, int32, scSelection& );

    virtual void    release();
    virtual void    reset();
    virtual int     paraselection( scSelection* );
	virtual int     setselection( scSelection* );
 	virtual int		gettoken( stUnivString& );
	virtual int	    replacetoken( stUnivString& );
    virtual int     next();
	
private:
	scSelection& range_;	
	scContUnit*	cu_;
	scSelection select_;

	int32		start_;
	int32		end_;
};


/* ==================================================================== */

stTokenIterImp::stTokenIterImp( scContUnit* cu, int32 start, int32 end, scSelection& range ) :
   range_( range ),
   cu_( cu ),
   start_( start ),
   end_( end )
{
	reset();
}

/* ==================================================================== */
	
void stTokenIterImp::release()
{
	delete this;
}

/* ==================================================================== */

void stTokenIterImp::reset()
{
	select_.SetParaSelection( cu_, start_, start_ );
	select_.fMark.SelectStartSpellWord( true );
	select_.fPoint = select_.fMark;
	select_.NextSpellWord();
}

/* ==================================================================== */

int stTokenIterImp::paraselection( scSelection* sel )
{
	if ( cu_ )
		sel->SetParaSelection( cu_, 0, cu_->GetContentSize() );
	return cu_ != 0;
}

/* ==================================================================== */

int stTokenIterImp::setselection( scSelection* sel )
{
	*sel = select_;	
	return 1;
}

/* ==================================================================== */

int stTokenIterImp::next()
{
	if ( select_.fMark.SelectStartSpellWord( true ) ) {
		if ( select_.fMark > select_.fPoint )
			select_.fPoint = select_.fMark;
		if ( select_.NextSpellWord( scSelection::inContUnit ) )
			return select_.fPoint.fOffset <= end_;
	}
	return 0;
}

/* ==================================================================== */

int stTokenIterImp::gettoken( stUnivString& ustr )
{
	if ( !select_.IsSliverCursor() ) {
		ulong tokenSize = select_.ContentSize();
		if ( ustr.len < tokenSize )
			return -select_.ContentSize();

		return cu_->GetToken( ustr, select_.fMark.fOffset, select_.fPoint.fOffset );		
	}
	return 0;
}

/* ==================================================================== */

int stTokenIterImp::replacetoken( stUnivString& ustr )
{
	int diff = select_.fPoint.fOffset;
	int ret = cu_->ReplaceToken( ustr, 
								 select_.fMark.fOffset, 
								 select_.fPoint.fOffset );

	diff = select_.fPoint.fOffset - diff;
	if ( ret && diff && end_ < LONG_MAX )
		end_ += diff;

	if ( cu_ == range_.fPoint.fPara )
		range_.fPoint.fOffset = end_;		// extend the parent selection
	
	return ret;
}

/* ==================================================================== */

stContUnitIterImp::stContUnitIterImp( scStream* str, scContUnit* cu ) :
	stream_( str ),
	cu_( cu )
{
	range_.AllSelect();
}

/* ==================================================================== */

stContUnitIterImp::stContUnitIterImp( scSelection* sel ) :
	stream_( sel->GetStream() ),
	cu_( 0 )
{
	range_ = *sel;
	range_.Sort();
	reset();
}

/* ==================================================================== */
	
void stContUnitIterImp::release()
{
	delete this;
}

/* ==================================================================== */

void stContUnitIterImp::reset()
{
	cu_ = range_.fMark.fPara;
}

/* ==================================================================== */

int stContUnitIterImp::gettokeniter( stTokenIter*& tokenIter )
{
	if ( cu_ ) {
		int32 start;
		int32 end;

		if ( cu_ == range_.fMark.fPara )
			start = range_.fMark.fOffset;
		else
			start = 0;

		if ( cu_ == range_.fPoint.fPara )
			end  = range_.fPoint.fOffset;
		else
			end = LONG_MAX;
		tokenIter = new stTokenIterImp( cu_, start, end, range_ );
	}
	else
		tokenIter = 0;
	return cu_ != 0;
}

/* ==================================================================== */

int stContUnitIterImp::next()
{
	cu_ = cu_->GetNext();

	if ( range_.fPoint.fPara ) {
		if ( cu_ == range_.fPoint.fPara->GetNext() )
			cu_ = 0;
	}
	return cu_ ? cu_->GetCount() : -1;
}

/* ==================================================================== */

void stContUnitIterImp::range( scStreamLocation& mark, scStreamLocation& point )
{
	range_.Decompose( mark, point );
}

/* ==================================================================== */

status SCSTR_GetContUnitIter( scStream*			str,
							  stContUnitIter*&	iter )
{
	status stat = scSuccess;
	
	try {
		iter = new stContUnitIterImp( str, str->First() );
	} 
	IGNORE_RERAISE;

	return stat;
}

/* ==================================================================== */

status SCSEL_GetContUnitIter( scSelection* sel, stContUnitIter*& iter )
{
	status stat = scSuccess;
	
	try {
		iter = new stContUnitIterImp( sel );
	} 
	IGNORE_RERAISE;

	return stat;
}

/* ==================================================================== */

class stFindIterImp : public stFindIter {
public:
	stFindIterImp();
	stFindIterImp( stUnivString& ustr, const SearchState&, scStream* str );
	stFindIterImp( stUnivString& ustr, const SearchState&, scSelection* sel );
	
    virtual void    release();
    virtual void    reset();
	virtual int		setselection( scSelection* );
	virtual int		next();
	virtual int		replacetoken( stUnivString& );
	virtual void	range( scStreamLocation&, scStreamLocation& );
	
	int		forwards();
	int		backwards();
	
private:
	UniversalString	ustr_;
	scStream*		str_;
	scSelection		range_;
	scSelection		select_;
	SearchState		state_;
	int32			cuOffset_;
};


/* ==================================================================== */

stFindIterImp::stFindIterImp() :
	cuOffset_( 0 )
{
}

/* ==================================================================== */
	
stFindIterImp::stFindIterImp( stUnivString& ustr,
							  const SearchState& state,
							  scStream* str ) :
									ustr_( ustr ),
									str_( str ),
									state_( state ), 
									cuOffset_( 0 )
{
	scContUnit* mark	= str->First();
	scContUnit* point	= str->Last();

	range_.SetMark( TextMarker( mark, mark->GetCount(), 0 ) );
	range_.SetPoint( TextMarker( point, point->GetCount(), point->GetContentSize() ) );
	select_.SetMark( range_.GetMark() );
	select_.SetPoint( range_.GetMark() );

	select_.Sort();
	range_.Sort();

	reset();
}

/* ==================================================================== */
	
stFindIterImp::stFindIterImp( stUnivString&	ustr,
							  const SearchState& state,
							  scSelection*	sel ) :
								ustr_( ustr ),
								str_( sel->GetStream() ),
								range_( *sel ),
								state_( state ),
								cuOffset_( 0 )
{
	select_.SetMark( range_.GetMark() );
	select_.SetPoint( range_.GetMark() );
	
	select_.Sort();
	range_.Sort();

	reset();
}

/* ==================================================================== */

void stFindIterImp::release()
{
	delete this;
}

/* ==================================================================== */

void stFindIterImp::reset()
{
	if ( !state_.reverse() ) {
		select_.SetMark( range_.GetMark() );
		select_.SetPoint( range_.GetMark() );
	}
	else {
		select_.SetMark( range_.GetPoint() );
		select_.SetPoint( range_.GetPoint() );
	}
}

/* ==================================================================== */

int stFindIterImp::setselection( scSelection* sel )
{
	*sel = select_;
	return 1;
}

/* ==================================================================== */

int stFindIterImp::next()
{
	if ( !state_.reverse() )
		return forwards();
	else
		return backwards();
}

/* ==================================================================== */

int stFindIterImp::forwards()
{
	scContUnit* firstcu = select_.GetMark().fPara;
	scContUnit* lastcu  = range_.GetPoint().fPara;
	
	int32	startoffset;
	int32	endoffset;

	while ( firstcu != lastcu->GetNext() ) {
		if ( firstcu == select_.GetPoint().fPara )
			startoffset = select_.GetPoint().fOffset;
		else
			startoffset = 0;

		if ( firstcu == range_.GetPoint().fPara )
			endoffset   = range_.GetPoint().fOffset;
		else
			endoffset = LONG_MAX;

		int32	offset;
		if ( firstcu->FindString( ustr_,
								  state_,
								  startoffset,
								  endoffset,
								  offset ) ) {
			select_.SetMark( TextMarker( firstcu, firstcu->GetCount(), offset ) );
			select_.SetPoint( TextMarker( firstcu, firstcu->GetCount(), offset + ustr_.len ) );
			return 1;
		}
		firstcu = firstcu->GetNext();
	}
	return 0;
}

/* ==================================================================== */

int stFindIterImp::backwards()
{
	scContUnit* firstcu = range_.GetMark().fPara;
	scContUnit* lastcu  = select_.GetPoint().fPara;
	
	int32	startoffset;
	int32	endoffset;
	
	while ( firstcu != lastcu->GetPrev() ) {
		if ( firstcu == select_.GetMark().fPara )
			endoffset   = select_.GetMark().fOffset;
		else
			endoffset = LONG_MAX;

		if ( firstcu == range_.GetMark().fPara )
			startoffset = range_.GetMark().fOffset;
		else
			startoffset = 0;

		int32	offset;
		if ( firstcu->FindString( ustr_,
								  state_,
								  startoffset,
								  endoffset,
								  offset ) ) {
			select_.SetMark( TextMarker( firstcu, firstcu->GetCount(), offset ) );
			select_.SetPoint( TextMarker( firstcu, firstcu->GetCount(), offset + ustr_.len ) );
			return 1;
		}
	}
	return 0;
}

/* ==================================================================== */

int stFindIterImp::replacetoken( stUnivString& ustr )
{
	int diff = select_.fPoint.fOffset;
	int ret = select_.fMark.fPara->ReplaceToken( ustr, 
												 select_.fMark.fOffset, 
												 select_.fPoint.fOffset );

	diff = select_.fPoint.fOffset - diff;

	if ( select_.fPoint.fPara == range_.fPoint.fPara )
		range_.fPoint.fOffset += diff;		// extend the parent selection
	
	return ret;
}

/* ==================================================================== */

void stFindIterImp::range( scStreamLocation& mark, scStreamLocation& point )
{
	range_.Decompose( mark, point );
}

/* ==================================================================== */

status SCSTR_GetFindIter( scStream*				str, 
						  stUnivString&			ustr, 
						  const SearchState&		flags,
						  stFindIter*&	friter )
{
	status stat = scSuccess;
	
	try {
		friter = new stFindIterImp( ustr, flags, str );
	} 
	IGNORE_RERAISE;

	return stat;
}

/* ==================================================================== */

status SCSEL_GetFindIter( scSelection*			sel, 
						  stUnivString&			ustr, 
						  const SearchState&		flags,
						  stFindIter*&	friter )
{
	status stat = scSuccess;
	
	try {
		friter = new stFindIterImp( ustr, flags, sel );
	} 
	IGNORE_RERAISE;

	return stat;
}

/* ==================================================================== */
