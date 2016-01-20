/*=================================================================
 
	File:		scpubobj

	$Header: /Projects/Toolbox/ct/SCPUBOBJ.H 2     5/30/97 8:45a Wmanis $

	Contains:	This contains some of the more specialized public
				data types that are NOT in sctypes.h.

	Written by:	Manis

	Copyright:	(c) 1988-94 by Stonehand Inc., of Cambridge, MA.
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

	@doc
=================================================================*/

#ifndef _H_SCPUBOBJ
#define _H_SCPUBOBJ

#include "scmemarr.h"
#include "scarray.h"

/* ==================================================================== */
// ERROR HANDLING
/* ==================================================================== */

#ifdef API_RERAISE

	#define IGNORE_RERAISE		catch ( status stat_ ) {\
									throw( stat_ );\
								}\
								catch ( ... ) {\
									throw( scERRgeneral );\
								}

#else

	#define IGNORE_RERAISE		catch ( status stat_ ) {\
									stat = stat_;\
								}\
								catch ( ... ) {\
									stat = scERRgeneral;\
								}

#endif

/* ==================================================================== */
/* ==================================================================== */
/* ==================================================================== */
/* =====================   SCTYPESPECLIST   =========================== */
/* ==================================================================== */
/* ==================================================================== */
// @class The TypeSpec List used for retreiving lists of type specs.
// @xref <c scSpecLocList>

class scTypeSpecList : public scSizeableArrayD< TypeSpec > {
PUBLISHED:
	TypeSpec	CL_GetNth( int );
	
public:
	void	Insert( TypeSpec& );
};

/* ==================================================================== */
/* ==================================================================== */
/* ====================   scSpecLocList   ============================ */
/* ==================================================================== */
/* ==================================================================== */
// The Char Spec list - used to retreive and set the spec associated
// with a particular character(s)

// @struct scParaOffset | is used to specify a postion in a stream.
class scParaOffset {
public:
			scParaOffset() :
					fParaOffset( -1 ),
					fCharOffset( -1 ){}					
					
	int32	fParaOffset;			// @field Paragraph offset within stream.
	int32	fCharOffset;			// @field Character offset within paragraph.
};

// @struct scSpecLocation | is used to specify a position in the stream
// at which to start the application of a <t TypeSpec>.
// @xref <c scSpecLocList>
class scSpecLocation {
public:
					scSpecLocation(){}
					scSpecLocation( int32 poffset, int32 offset )
					{
						offset_.fParaOffset = poffset;
						offset_.fCharOffset = offset;
					}
					scSpecLocation( int32     poffset,
								   int32     offset,
								   TypeSpec& spec )
					{
						offset_.fParaOffset = poffset;
						offset_.fCharOffset = offset;
						spec_ = spec;
					}
	scParaOffset&	offset()
					{
						return offset_;
					}
	const scParaOffset&	offset() const
					{
						return offset_;
					}

	TypeSpec&		spec()
					{
						return spec_;
					}

	const TypeSpec&	spec() const
					{
						return spec_;
					}
	
private:
	scParaOffset	offset_;	// @field The <t scParaOffset> is
									// the stream location.
	TypeSpec		spec_;			// @field The <t TypeSpec> to be applied at
									// this stream location.
};


// @class The scSpecLocList is an array of <t scSpecLocation> that contain
// the stream position and the spec associated with each spec transition.
// @xref <c scTypeSpecList>
//

class scSpecLocList : public scSizeableArrayD< scSpecLocation > {
PUBLISHED:
	TypeSpec	GetFirstValidSpec( void ) const;
	TypeSpec	GetNthValidSpec( int ) const;
	TypeSpec	GetLastValidSpec( void ) const;

	void		SetStream( scStream* stream )
					{
						fStream = stream;
					}
	scStream*	GetStream( void ) const
					{
						return fStream;
					}
	
public:
				scSpecLocList( scStream* stream ) :
						fStream( stream ){}


			// append a paragraph terminator		
	void		TermParagraph( int32 paraoffset, int32 offset )
				{
					scSpecLocation chsploc;
					scParaOffset& poffset	= chsploc.offset();
					poffset.fParaOffset		= paraoffset;
					poffset.fCharOffset		= offset;
				
					Append( chsploc );
				}

	
#if SCDEBUG > 1
	void		DbgPrint( void ) const;
#endif

private:
	scStream*	fStream;		// @member fStream | Stream to which this list belongs.
						
};


/* ==================================================================== */
/* ================= LINE INFO ======================================== */
/* ==================================================================== */

class scLineInfoList : public scMemArray {
public:
			scLineInfoList() :
					scMemArray( sizeof( scLineInfo ) ) {}
};


/* ==================================================================== */

/* ==================================================================== */
/* ==============   COLUMN REDISPLAY/DAMAGE REPORT  =================== */
/* ==================================================================== */

/* a column id struct that Stonehand returns on many calls, used by APP to
 * determine damage extents for redisplay, the redisplay requires two areas
 * for correct redisplay, the damaged area - is the area that needs to 
 * be repainted to patch the screen, the repaint area is the area that needs 
 * to be repainted to update the screen, in most instances the union of these 
 * two areas would be sufficient but if the user's interaction, reorigined 
 * the column - an action that Stonehand
 * has no way of knowing, the control program may repair the screen with the
 * damaged area and then use the repaint area to update the screen.
 * all of the areas/rectangles are expressed in high precision rectangles and
 * are relative to the columns origin, the extents and repaint area 
 * are in relation to the current origin, the damaged area is in relation
 * to the previous origin, no account has been made for any matrix 
 * transformations.
 * The flagLineHandle is a handle to an array containing information about 
 * lines that exceed nominal values (e.g. overset, word-space values 
 * exceeded, etc ), it may be used to flag the lines visually for the user
 */

/*================== COLUMN LEVEL MESSAGES ==================*/
/* a note on functions returning a pointer to ColRect's,
   many of these operations may have impact on multiple containers, 
   therefore a pointer to a ColRect is a null terminated chain
   of ColRect's, with each ColRect containing the scColumn* & appName,
   its new set of extents and its damaged area, a typical thing to do 
   when one receives one of these might be something like this:

    for ( ; colrect->columnID != NULL; colrect++ ) {
        ColumnLocalToGlobal( colrect->appName, &colrect->exRect );
        UpdateColumnExtents( colrect->appName, &colrect->exRect );
        ColumnLocalToGlobal( colrect->appName, &colrect->damageRect );
        InvalHRect( &colrect->damageRect );
    }

 */

 
/* FLAG LINE */
typedef enum eFlagTypes{
    okSet,
    overSet,            /* the line is overset */
    minExSet,           /* the minimum wordspace value is less than desired */
    maxExSet            /* the maximum wordspace value is exceeded */
} eFlagType;

struct scFlaggedLine {
    scXRect		lineExtents;            /* extents of line to flag */
    /* the following represent character characteristics of the 
     * first character on the line
     */
    MicroPoint  xHite;                  
    MicroPoint  capHite;
    MicroPoint  ascHite;
    MicroPoint  desHite;
    MicroPoint  baseline;
    eFlagType   flag;                   /* type of flag */
};                             /* used to flag lines */

// @class scImmediateRedisp is for immediate redisplay of text that has
// been altered in editting. This is useful for updateing a minimal number of
// lines during editting and letting the rest of the text get repaired
// on an update event. It is basically redisplaying only those lines
// which the cursor has been on, hitting a carriage return should force two
// lines to be redisplayed immediately, same with a backspace at the
// begginning of a line, normally only one line will be redisplayed
// IF THE OPERATION CROSSES COLUMNS WE WILL ONLY UPDATE THE COLUMN 
// IN WHICH THE CURSOR ENDS UP IN.
// @xref <f SCCOL_UpdateLine>
	
class scImmediateRedisp {
public:
                scImmediateRedisp() :
					fStartLine( 0 ),
					fStopLine( 0 ) { }

	short		fStartLine;			// @cmember Start drawing this line.
	short		fStopLine;			// @cmember End drawing this line.
	scXRect		fImmediateRect;		// @cmember Erase this area.
};

// @class This contains the information needed to correctly redisplay
// a column after reformatting.

class scColRedisplay {
public:
					scColRedisplay(){}
					scColRedisplay( scColumn* col, APPColumn appcol ) :
						fColumnID( col ),
						fAPPName( appcol ),
						fWidth( 0 ),
						fDepth( 0 ),
						fAdditionalText( false ),
						fHasDamage( false ),
						fHasRepaint( false ),
						fImmediateRedisplay( false ) {}

PUBLISHED:						
    scColumn*         	fColumnID;			// @member fColumnID | Stonehand name.
    APPColumn       	fAPPName;			// @member fAPPName | An <t APPColumn>
											// holding the client's name.
	
		// the recorded width and depth in the container
    MicroPoint      	fWidth;				// @member fWidth | Current container width.
    MicroPoint      	fDepth;				// @member fDepth | Current container depth.
    scXRect           	fExRect;            // @member fExRect | An <c scXRect> that contains
											// the ink extents of the container.
    Bool            	fAdditionalText;    // @member fAdditionalText | True
											// is text flows out the bottom of
											// this container.
	
	Bool				fHasDamage;
    scXRect          	fDamageRect;         /* damaged area of column */

	Bool				fHasRepaint;
    scXRect         	fRepaintRect;        /* area to repaint */
	
	Bool				fImmediateRedisplay;	// @member fImmediateRedisplay |
												// True if there is a need
												// immediate redisplay info.
	scImmediateRedisp	fImmediateArea;			// @member fImmediateArea | Area
												// to redisplay immediately.
};


/* ==================================================================== */

class scCOLRefData;

// @class This class contains an array of structures that hold information
// about redisplay. For each effected column there will be a corresponding
// class <c scColRedisplay> with information about what areas ( in local coordinates ) need
// to be redispalyed.

class scRedispList : public scMemArray {
PUBLISHED:
	status			CL_GetColumnData( APPColumn, scColRedisplay& ) const;
	
public:
					scRedispList( ) :
						scMemArray( sizeof( scColRedisplay ) ){}
						
	void			ReInit( void );
	void			AddColumn( const scCOLRefData& colRefData );
 	void			AddColumn( scColumn*, scXRect& );

	void			SetImmediateRect( scColumn*, const scImmediateRedisp& );

protected:
	scColRedisplay*	FindCell( const scColumn* ) const;
	void			AddCell( scColumn* );	
};


/* ==================================================================== */
/* ==================================================================== */
/* ==================================================================== */


#endif
