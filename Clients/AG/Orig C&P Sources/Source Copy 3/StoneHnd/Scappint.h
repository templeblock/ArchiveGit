//<html><pre>
/****************************************************************************

	File:		SCAPPINT.H

	$Header: /Projects/Toolbox/ct/SCAPPINT.H 2	   5/30/97 8:45a Wmanis $
	
	Contains:	The portable c application interface prototypes

					SCENG_xxxxx 	- messages to the text engine
					
					SCFS_xxxxxx 	- messages to flowsets
					
					SCCOL_xxxxx 	- messages to columns
					
					SCSEL_xxxxx 	- messages to a selection
					
					SCSTR_xxxxx 	- messages to streams

					SCSCR_xxxxx 	- messages to the scrap

					SCHRGN_xxxx 	- messages to regions

					SCCHTS_xxxx 	- messages to scCharSpecList

					SCTSL_xxxxx 	- messages to scTypeSpecList

					SCAPPTXT_xx 	- messages to scAPPText

					SCRDL_xxxxx 	- messages to scRedisplayList

	Written by: Manis

	Copyright (c) 1989-1994 Stonehand Inc., of Cambridge, MA.  
	All rights reserved.

	This notice is intended as a precaution against inadvertent publication
	and does not constitute an admission or acknowledgment that publication
	has occurred or constitute a waiver of confidentiality.

	Composition Toolbox software is the proprietary
	and confidential property of Stonehand Inc.

@doc

****************************************************************************/

#ifndef _H_SCAPPINT
#define _H_SCAPPINT

#ifdef SCMACINTOSH
#pragma once
#endif

#include "sctypes.h"
#include "sccharex.h"

// [doc] [test]

class stTextImportExport;		// in scapptex.h  ( scTextExchange )

class scTypeSpecList;	// in scpubobj.h
class scLineInfoList;	// in scpubobj.h
class scRedispList; 	// in scpubobj.h
class scSpecLocList;	// in scpubobj.h

class scSet;			// see scset.h

class scImmediateRedisp;// in scpubobj.h

class clField;		// in sccallbk.h

// TERMINOLOGY
/* =======================
FLOW SET		a set of linked columns which contain a single stream
COLUMN			an area to flow text into, text MAY extend outside the
				column depending upon constraints, another name for a column
				is a TEXT CONTAINER
STREAM			a set of content units/paragraphs
CONTENT UNIT	a paragraph that contains characters and specs associated
				with the characters
SELECTION		a range of text, each flow set may only contain one selection

 * ======================= */


/*======================= SYSTEM LEVEL MESSAGES ================*/
// @func status | SCENG_Init | This must be called before any other
// calls are made into the Toolbox; it initializes and sets
// toolobx behavior. The base error is the number added to
// <t status> errors when exceptions are re-raised across the api.
//
// @parm int | baseError | Value to add to <t status> values
// if we are re-raising exceptions across the API.
//
status scIMPL_EXPORT	SCENG_Init( int baseError = 0 );



// @func Closes the Composition Toolbox; this releases all memory
// that the Toolbox has allocated. All references into the
// Toolbox become invalid.
//
status scIMPL_EXPORT	SCENG_Fini( void );



// The following three calls are optional. Their use will guarantee that
// the Toolbox can always recover from an out of memory condition,
// given that the application can get it back to a previous state.
//

// Holds memory to guarantee that if recomposition fails, we will
// be able to revert to previous state.
//
// [ ] [ ]
status scIMPL_EXPORT	SCENG_RetainMemory( void );




// Informs Toolbox to use retained memory - 
// only for use in a recovery operation.
//
// [ ] [ ]
status scIMPL_EXPORT	SCENG_UseRetainedMemory( void );




// Releases retained memory; recomposition has been successfully completed.
//
// [ ] [ ]
status scIMPL_EXPORT	SCENG_ReleaseMemory( void );


/* ==================================================================== */
/* ============= OBJECT ALLOCATION ==================================== */
/* ==================================================================== */

// If the application wants to allocate its objects it can do
// so bearing in mind that it must have set up an exception
// handler, otherwise the client may use the following to
// allocate and free the objects. Accessing the data within
// these objects should not present exception problems if the
// correct accessor methods are used.



/* ==================================================================== */
/* ====================  SCTYPESPECLISTT   ============================ */
/* ==================================================================== */

// @func Safely allocate.
status scIMPL_EXPORT	SCTSL_Alloc(
							scTypeSpecList*&  tsl ); // @parm <t scTypeSpecList>
// @func Safely delete.
status scIMPL_EXPORT	SCTSL_Delete(
							scTypeSpecList*&  tsl ); // @parm <t scTypeSpecList>


/* ==================================================================== */
/* =======================	SCREDISPLIST   ============================ */
/* ==================================================================== */

// @func Safely allocate.
status scIMPL_EXPORT	SCRDL_Alloc(
							scRedispList*& rdl );	// @parm <c scRedispList>


// @func Safely delete.
status scIMPL_EXPORT	SCRDL_Delete(
							scRedispList*& rdl );	 // @parm <c scRedispList>


/* ==================================================================== */
/* ==========================  SCAPPTEXT   ============================ */
/* ==================================================================== */


// @func Safely allocate.
status scIMPL_EXPORT	SCAPPTXT_Alloc(
						   stTextImportExport*& atxt ); 	// @parm <c scAPPText>

// @func Safely delete.
status scIMPL_EXPORT	SCAPPTXT_Delete(
						   stTextImportExport* atxt );		// @parm <c scAPPText>										



/* ==================================================================== */
/* ======================  SCCHARSPECLIST	=========================== */
/* ==================================================================== */

// @func Safely allocate.
status scIMPL_EXPORT	SCCHTS_Alloc(
							scSpecLocList*& cslist, 		// @parm <c scCharSpecList>
							scStream*			stream );		// @parm <c scStream> to associate with.

// @func Safely delete.
status scIMPL_EXPORT	SCCHTS_Delete(
							scSpecLocList*& cslist );			// @parm <c scCharSpecList>



/* ==================================================================== */
/* ========================== CONTAINER MESSAGES ====================== */
/* ==================================================================== */

// @func Creates a new column/container within the Composition Toolbox universe.
// The appName is simply a notational convenience for the client. It is 
// presumed that the client is maintaining some sort of container structure
// and the appName is typically pointing to the clients structure. In all
// conversations with the client we use this name. If it is 0 we will simply 
// fill in our name for it.
//
status scIMPL_EXPORT	SCCOL_New(
							APPColumn	appName,	// @parm <t APPColumn>, the name used by client.
							scColumn*&	newID,		// @parm <c scColumn> name of object allocated by toolbox.
							MicroPoint	width,		// @parm <t MicroPoint> width of new text container.
							MicroPoint	depth );	// @parm <t MicroPoint> depth of new text container.


// @func Deletes a container, removes itself from the flowset with
// the text in this container simply spilling over into 
// others in the flow set. 
// If it is the only column associated in the flow set 
// then the stream is also deleted.
// To delete the whole flow set start deleting from the last column
// so that the first column (and the stream) will be deleted last.
status scIMPL_EXPORT	SCCOL_Delete(
							scColumn*		col,		// @parm Name of <c scColumn> to delete.
							scRedispList*	rInfo );	// @parm <c scRedispList>
														// Redisplay info, arg may be zero.

// @func Links the two columns together using the following logic:
// COL2 must represent the first column in a flow set.
// COL1 may be anywhere within a (distinct) flow set,
// COL2 will be chained in after COL1. The ordering/reordering
// of the streams is as follows: <nl>
// 1. if COL1 has text and COL2 has no text, the text flows from COL1 to COL2
// <nl>
// 2. if COL2 has text and COL1 has no text, the text flows from COL1 to COL2
// <nl>
// 3. if both COL1 and COL2 have text, the text from COL2 is appended
// to the text in COL1 - this may may create some confusion
// on the user's part, so use with care. <nl>
// NOTE!!!!!! ANY SELECTION IN EITHER FLOW SET WILL NO LONGER BE VALID!!!!!
// @xref <f SCCOL_Unlink>

status scIMPL_EXPORT	SCCOL_Link(
							scColumn*		col1,		// @parm Name of first <c scColumn>.
							scColumn*		col2,		// @parm Name of second <c scColumn>.
							scRedispList*	rInfo );	// @parm Redisplay info, arg may be zero.



// @func Unlinks a column from its chain. The column passed in becomes
// an empty container, and the stream remains intact. If the column
// is the only column in a chain, it is a no-op.
// <nl>NOTE!!!!!! ANY SELECTION IN THE FLOW SET
// COLUMN WILL NO LONGER BE VALID!!!!!
// @xref <f SCCOL_Link>

status scIMPL_EXPORT	SCCOL_Unlink(
							scColumn*		col,		// @parm Name of <c scColumn> to unlink.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.


// @func Severs the link between the two columns. The stream is left in
// the first logical container set. The text is left in a
// uncomposed state.<NL>
// NOTE!!!!!! ANY SELECTION IN THE FLOWSET
// COLUMN WILL NO LONGER BE VALID!!!!!
//
status scIMPL_EXPORT	SCFS_Split(
							scColumn*	col1,		// @parm <c scColumn> prior to split.
							scColumn*	col2 ); 	// @parm <c scColumn> after split.


// @func Resizes a column. If the column has an irregular shape
// (such as a polygon), the width and depth values of the container
// are updated, but no reflowing occurs. The width and depth are independent
// of text flow. Width is always the horizontal dimenision and depth
// the vertical dimension.
//
status scIMPL_EXPORT	SCCOL_SetSize(
							scColumn*		col,		// @parm <c scColumn> to resize.
							MicroPoint		width,		// @parm New <t MicroPoint> width.
							MicroPoint		depth,		// @parm New <t MicroPoint> depth.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.

// tests to see if there is more text than is in this column
// this would set the flag to true if:
//			there is text in subsequent linked columns
//			there is unformatted text that will not fit in this column

status scIMPL_EXPORT	SCCOL_MoreText(
							scColumn*	col,
							Bool&		flag ); 	   
									   

/*======================= STREAM OPERATIONS ====================*/

// @func Returns an id to the stream a column/flow set contains.
//
status scIMPL_EXPORT	SCCOL_GetStream(
							scColumn*	column, 	// @parm <c scColumn> containing stream.
							scStream*&	stream );	// @parm <c scStream> pointer to be filled.

// @func Writes stream to file using the call back write routine.
// @xref <f SCCOL_GetStream>

status scIMPL_EXPORT	SCSTR_Write(
							scStream*	stream, 	// @parm <c scStream> to write.
							APPCtxPtr	ioctxptr,	// @parm <t APPCtxPtr> Abstract file i/o type.
							IOFuncPtr	ioFuncPtr );// @parm <t IOFuncPtr> write function pointer.



// @func Reads stream from file using the call back read routine.
// Use the appropriate calls for file i/o
// <f SCSET_InitRead>, <f SCOBJ_PtrRestore>, <f SCSET_FiniRead>, <f SCCOL_GetStream>
//
status scIMPL_EXPORT	SCSTR_Read(
							scStream*&	stream, 	// @parm Pointer to <c scStream> to be
													// filled in by Composition Toolbox.
							scSet*		enumTable,	// @parm Pointer enumeration table.
							APPCtxPtr	ioctxptr,	// @parm <t APPCtxPtr> Abstract file i/o type.
							IOFuncPtr	ioFuncPtr );// @parm <t IOFuncPtr> write function pointer.




// @func Deletes a stream. If it is associated with a set of linked columns,
// they become a linked set of empty containers. If no columns are
// attached or no redraw is necessary, the scRedispList will be NULL.
// @xref <f SCCOL_GetStream>
//
status scIMPL_EXPORT	SCSTR_Clear(
							scStream*		stream, 	// @parm <c scStream> to delete.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.




// @func Cuts a stream from its associated containers.
// @xref <f SCCOL_GetStream>
//
status scIMPL_EXPORT	SCSTR_Cut(
							scStream*		stream, 	// @parm Stream to cut.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.




// @func Copies a stream, returning a unique id in the second argument.
// The copy will not be associated with any containers.
// @xref <f SCCOL_GetStream>
//
status scIMPL_EXPORT	SCSTR_Copy(
							const scStream* srcStream,	// @parm Stream to copy.
							scStream*&		theCopy );	// @parm The new copy.




// @func Pastes a stream into a container. If the container already has a stream,
// the pasted stream is appended to the existing one. To conserve on 
// resources, the streamID is not duplicated. Thus, for multiple 
// pastes of one stream, a new copy of the stream must be made for each paste.
//
status scIMPL_EXPORT	SCFS_PasteStream(
							scColumn*		col,		 // @parm <c scColumn> containing stream
														 // which theStream will be appended.  
							scStream*		theStream,	 // @parm The stream to be appended.
							scRedispList*	rInfo );	 // @parm <c scRedispList> 
														 // Redisplay info, arg may be zero.


// Extracts a scContUnit from a scStreamLocation for use with SCSTR_Split
status scIMPL_EXPORT	SCSEL_GetContUnit(
							 scContUnit*& mark,
							 scContUnit*& point,
							 const scSelection* );


// This call is used to undo a link. To set this up,
// before linking two columns col1 and col2, save references
// to their streams, stream1 and stream2, respectively.
// When unlinking col1 and col2, call scStreamSplit( stream1, stream2 )
// to split the stream into its original two pieces. This call should be
// followed with a call to SCPasteStream( col2, stream2, scRedispList *)
// to reset the unlinked column's stream to its original value.
// NOTE: both stream1 and stream2 are assumed to be valid (non-NULL).
// This call should only be made with reformatting turned off.
//
// [ ] [ ]
status scIMPL_EXPORT	SCSTR_Split(
							scStream*,
							scContUnit*,
							scStream*& );


// @func Compare streams for equality, this tests content and specs
// scSuccess == equality
// @xref <f SCCOL_GetStream>
//
status scIMPL_EXPORT	SCSTR_Compare(
							const scStream* str1,		// @parm <c scStream>
							const scStream* str2 ); 	// @parm <c scStream>


/*==== COMPOSITION OPERATIONS & COMPOSITION ERROR RECOVERY =====*/
// @func Sets recomposition off or on in the flow set of the indicated column
//
status scIMPL_EXPORT	SCFS_SetRecompose(
							scColumn*	col,		// @parm Flow set to turn composition off or on in.
							Bool		onOff );	// @parm true == on, false = off

// @func Gets the current recomposition state of the flow set.
//
status scIMPL_EXPORT	SCFS_GetRecompose(
							scColumn*	col,		// @parm Flow set to query.
							Bool&		onOff );	// @parm Composition flag.

// @func Recomposes the flowset.
//
status scIMPL_EXPORT	SCFS_Recompose(
							scColumn*		col,	// @parm Flow set to recompose.
							scRedispList*	rInfo );// @parm <c scRedispList> 
													// Redisplay info, arg may be zero.

// NOT IMPLEMENTED
// Recompose a portion of the stream in the flowset. Process the flowset
// for the number of ticks indicated. We will process paragraphs
// until time exceeds the ticks
//
// [ ] [ ]
status scIMPL_EXPORT	SCFS_Recompose( scColumn*,
										long ticks,
										scRedispList* );


// Rebreaks all the lines in a column, with extreme prejudice;
// it ignores the flowset RecomposeHold() setting and processes
// only the indicated column, it will return an error if a prior
// column is uncomposed
//
// [ ] [ ]
status scIMPL_EXPORT	SCCOL_Recompose( scColumn*,
										 scRedispList* );


// @func Applies the specs in the CharSpecListHandle to the text
// at the locations indicated therein.
// @xref <f SCCOL_GetStream>, <f SCSTR_CHTSList>
status scIMPL_EXPORT	SCSTR_CHTSListSet(
							scStream*		stream, // @parm <c scStream> to apply scCharSpecList to. 
							const scSpecLocList& cslist,	// @parm <c scCharSpecList> list of specs and locations.
							scRedispList*	rInfo );// @parm <c scRedispList> 
													// Redisplay info, arg may be zero.

status scIMPL_EXPORT	SCSTR_PARATSListSet( scStream*				stream, 
											 const scSpecLocList&	cslist,
											 scRedispList*			rInfo );

/*============== CONTAINER CONSTRAINT OPERATIONS ===============*/


// These routines set containers to be flexible in the horizontal or
// vertical dimensions. A flexible container varies in size with its contents.
// A vertically flexible container varies with the number of lines;
// it grows until it reaches either the last character in the stream or
// a column break. A horizontally flexible container varies with the width
// of its widest line; it grows until the end of the paragraph or a hard
// return.
//
// NOTE: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Container FLEXIBILITY and IRREGULAR shapes are MUTUALLY EXCLUSIVE!!!!!!
// Setting a container to be flexible automatically clears any
// irregular shape associated with the column.
//


// @func Turns on vertical flexibility.
// @xref <f SCCOL_GetVertFlex>, <f SCCOL_ClearVertFlex>

status scIMPL_EXPORT	SCCOL_SetVertFlex(
							scColumn*		col,	// @parm <c scColumn> to set flex.
							scRedispList*	rInfo );// @parm <c scRedispList> 
													// Redisplay info, arg may be zero.


// @func Turns off vertical flexibility.
// @xref <f SCCOL_SetVertFlex>, <f SCCOL_GetVertFlex>

status scIMPL_EXPORT	SCCOL_ClearVertFlex(
							scColumn*		col,	// @parm <c scColumn> to clear flex.
							scRedispList*	rInfo );// @parm <c scRedispList> 
													// Redisplay info, arg may be zero.


// @func Gets the vert flex attribute of the column.
// @xref <f SCCOL_SetVertFlex>, <f SCCOL_ClearVertFlex>

status scIMPL_EXPORT	SCCOL_GetVertFlex(
							scColumn*	col,		// @parm <c scColumn> to get attribute from.
							Bool&		onOff );	// @parm Vertical flex attribute, true
													// equals on, false off.




// @func Turns on horizontal flexibility.
// @xref <f SCCOL_GetHorzFlex>, <f SCCOL_ClearHorzFlex>

status scIMPL_EXPORT	SCCOL_SetHorzFlex(
							scColumn*	col,	// @parm <c scColumn> to set flex.
							scRedispList*	rInfo );// @parm <c scRedispList> 
													// Redisplay info, arg may be zero.



// @func Turns off horizontal flexibility.
// @xref <f SCCOL_SetHorzFlex>, <f SCCOL_GetHorzFlex>

status scIMPL_EXPORT	SCCOL_ClearHorzFlex(
							scColumn*		col,	// @parm <c scColumn> to clear flex.
							scRedispList*	rInfo );// @parm <c scRedispList> 
													// Redisplay info, arg may be zero.




// @func Gets the horzontal flex attribute of the column.
// @xref <f SCCOL_SetHorzFlex>, <f SCCOL_ClearHorzFlex>

status scIMPL_EXPORT	SCCOL_GetHorzFlex(
							scColumn*	col,		// @parm <c scColumn> to get attribute from.
							Bool&		onOff );	// @parm Horizontal flex attribute, true
													// equals on, false off.


// @func Gets the direction of lines in the container,
// and of characters in the lines.
// @xref <f SCCOL_SetFlowDirection>

status scIMPL_EXPORT	SCCOL_GetFlowDirection(
							scColumn*	col,		// @parm <c scColumn> to query.
							scFlowDir&	fd );		// @parm <c scFlowDir> of column.

// @func Sets the direction of lines in the container,
// and of characters in the lines.
// @xref <f SCCOL_GetFlowDirection>

status scIMPL_EXPORT	SCCOL_SetFlowDirection(
							scColumn*			col,	// @parm <c scColumn> to set.
							const scFlowDir&	fd );	// @parm <c scFlowDir> value to
														// set of column.

/*============= POLYGON CONTAINER SHAPE OPERATIONS =============*/

#if defined( scColumnShape )

// The application may create polygons in any way that it sees fit.
// They are then passed into the Toolbox and text flows into them
// using an even-odd area fill algorithm.
//


// Pastes in a set of vertices to be added to the column's current 
// vertex list.
//
// [ ] [ ]
status scIMPL_EXPORT	SCCOL_PastePoly( scColumn*,
										 const scVertex*,
										 scRedispList* );


// Extracts a copy of the polygon applied to this column,
// causing no recomposition. Useful for editing the polygon.
//
// [ ] [ ]
status scIMPL_EXPORT	SCCOL_CopyPoly( scColumn*,
										scVertex*& );

// Clears the polygon applied to this column.
//
// [ ] [ ]
status scIMPL_EXPORT	SCCOL_ClearPoly( scColumn*,
										 scRedispList* );



/*================ REGION RUN-AROUND OPERATIONS ================*/

// Regions are high precision descriptions of arbitrary shapes.
// They are useful for representing irregularly shaped containers,
// or for enabling text to run around objects intersecting a given container.
// The application may use them to represent containers
// that have been modified by intersection with other page objects.
//

// @func Applies a region as the shape of this column.
// The region is assumed to be in local coordinates.
//
status scIMPL_EXPORT	SCCOL_PasteRgn(
							scColumn*			col,	// @parm <c scColumn> to apply region to.
							const HRgnHandle	rgn,	// @parm Region to apply.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.


// @func Extracts a copy of the region applied to this column,
// causing no recomposition.
//
status scIMPL_EXPORT	SCCOL_CopyRgn(
							scColumn*	col,	// @parm <c scColumn> with region
							HRgnHandle& rgn );	// @parm <t HRgnHandle> the region copy.




// @func Clears the region belonging to this column.
//
status scIMPL_EXPORT	SCCOL_ClearRgn(
							scColumn*		col,		// @parm <c scColumn> to clear region.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.



// The following functions operate on regions.

// @func Create a new region.
//

status scIMPL_EXPORT	SCHRGN_New(
							HRgnHandle& newRgn, 		// @parm <t HRgnHandle>
							MicroPoint	sliverSize);	// @parm Sliver size.

// @func What sliver size is a region using.

status scIMPL_EXPORT	SCHRGN_SliverSize(
							const HRgnHandle rgn,			// @parm <t HRgnHandle>
							MicroPoint& 	 sliverSize);	// @parm Size of sliver.

// @func Dispose a region.

status scIMPL_EXPORT	SCHRGN_Dispose(
							HRgnHandle disRgn );			// @parm <t HRgnHandle> to dispose.

// @func Make a region empty, remove all slivers.

status scIMPL_EXPORT	SCHRGN_SetEmpty( 
							HRgnHandle emptyRgn );			// @parm <t HRgnHandle> to empty.

// @func Is a region empty.
// @rdesc scSuccess == empty region

status scIMPL_EXPORT	SCHRGN_Empty(
							const HRgnHandle emptyRgn );	// @parm <t HRgnHandle> to test.

// @func Compare to regions for equality.
// @rdesc scSuccess == equality

status scIMPL_EXPORT	SCHRGN_Equal(
							const HRgnHandle rgn1,		// @parm <t HRgnHandle>.
							const HRgnHandle rgn2 );	// @parm <t HRgnHandle>.

// @func Determine if point is in region.
// @rdesc scSuccess == equality
//
status scIMPL_EXPORT	SCHRGN_PtIn(
							const HRgnHandle	rgn,		// @parm <t HRgnHandle> to test.
							const scMuPoint&	pt );		// @parm <c scMuPoint>
							
// @func Make a region rectangular.
//
status scIMPL_EXPORT	SCHRGN_Rect(
							HRgnHandle		rng,	// @parm Region to apply rect.
							const scXRect&	rect ); // @parm <c scXRect>

// @func Make a region from a set of verticies ( closed polygons(s) ).
// The polygon must have both a horizontal and vertical dimension
// (e.g. it must have interior space)

status scIMPL_EXPORT	SCHRGN_Poly(
							HRgnHandle		rng,	// @parm Region to apply polygon.
							const scVertex* polys );// @parm <c scVertex> Polygon(s) description.

// @func Copy a region.

status scIMPL_EXPORT	SCHRGN_Copy(
							HRgnHandle		 dstRgn,	// @parm The copy.
							const HRgnHandle srcRgn );	// @parm To copy.

// @func Translate a region.

status scIMPL_EXPORT	SCHRGN_Translate( 
							HRgnHandle	rgn,	// @parm Region to inset.
							MicroPoint	x,		// @parm Horizontal translation.
							MicroPoint	y );	// @parm Vertical translation.


// @func Inset a region.

status scIMPL_EXPORT	SCHRGN_Inset(
							HRgnHandle	rgn,	// @parm Region to inset.
							MicroPoint	h,		// @parm Horizontal size change.
							MicroPoint	v );	// @parm Vertical size change.


// @func Is this rect contained within the region.

status scIMPL_EXPORT	SCHRGN_RectIn(
							const HRgnHandle rgn,		// @parm Region to test.
							const scXRect&	 rect );	// @parm <c scXRect>


// Perform Boolean operations on regions, the 3rd arg may be one of the
// 2 original regions, in that case it will replace the contents of after the
// operation is complete.

// @func Performs an intersection of two regions, placing the intersection
// in a third region.

status scIMPL_EXPORT	SCHRGN_Sect(
							const HRgnHandle r1,			// @parm <t HRgnHandle>
							const HRgnHandle r2,			// @parm <t HRgnHandle>
							HRgnHandle		 intersection );// @parm <t HRgnHandle>,
															// the intersection of r1 & r2.

// @func Performs the union of two regions, placing the union in a third region.

status scIMPL_EXPORT	SCHRGN_Union(
							const HRgnHandle r1,			// @parm <t HRgnHandle>
							const HRgnHandle r2,			// @parm <t HRgnHandle>
							HRgnHandle		 rUnion );		// @parm <t HRgnHandle>,
															// the union of r1 & r2.


// @func Performs a difference of two regions, placing the diff
// in a third region.

status scIMPL_EXPORT	SCHRGN_Diff(
							const HRgnHandle r1,			// @parm <t HRgnHandle>
							const HRgnHandle r2,			// @parm <t HRgnHandle>
							HRgnHandle		 difference );	// @parm <t HRgnHandle>,
															// the difference of r1 & r2.



// @func Performs an xor of two regions, placing the result in a third region.

status scIMPL_EXPORT	SCHRGN_Xor(
							const HRgnHandle r1,			// @parm <t HRgnHandle>
							const HRgnHandle r2,			// @parm <t HRgnHandle>
							HRgnHandle		 xor ); 		// @parm <t HRgnHandle>,
															// the xor of r1 & r2.


#endif


/*========================= RENDERING ==========================*/


// @func Renders/draws that part of the column lying within the
// given rect. The scXRect is in local coordinates. The Toolbox
// then calls back to the client using <f APPDrawStartLine>,
// <f APPDrawString>, & <f APPDrawEndLine>, passing the <t APPDrwCtx>
// through. This call and <f SCCOL_UpdateLine> are the only two calls
// that cause glyphs to be drawn. ALL DRAWING OF TOOLBOX CONTAINERS
// HAPPENS AT THE  BEHEST OF THE CLIENT.
// @xref <k APPDrawStartLine>, <k APPDrawString>, & <k APPDrawEndLine>

status scIMPL_EXPORT	SCCOL_Update(
							scColumn*		col,		// @parm <c scColumn> to draw
							const scXRect&	clipRect,	// @parm Clip rect.
							APPDrwCtx		dc	);		// @parm Drawing context.




/*================== CONTAINER & LINE EXTENTS ==================*/


// @func Queries ink extents of the column. The extents returned are
// the maximum bounding box of the maximum character extents expressed
// in the column's coordinate system.
//
status scIMPL_EXPORT	SCCOL_QueryInkExtents(
							scColumn*	col,			// @parm <c scColumn> to query.
							scXRect&	inkExtents );	// @parm <t scXRect>


// @func Queries margins of the column. Returns the actual size of the
// container, rather than the extents. Only meaningful for rectangular
// containers. Bear in mind that the ink of text may extend outside of the
// container.
//
status scIMPL_EXPORT	SCCOL_QueryMargins(
							scColumn*	col,		// @parm <c scColumn> to query.
							scXRect&	margins );	// @parm <t scXRect>


// Queries positions of the column's lines. For each line, it queries
// the baseline, the extents, and the character characteristics at
// the start (x height, ascender height, descender height, cap height, etc. ).
// The last two parameters indicate the number of lines and whether text
// overflows the column.
// Typically used for alignment purposes. Positions are in the container's
// local coordinates.
//
// [ ] [ ]
status scIMPL_EXPORT	SCCOL_LinePositions( scColumn*,
											 scLineInfoList*,
											 long&,
											 Bool& );


// @func Queries the depth of the column. Useful for determining the depth
// of irregularly shaped columns.
//
status scIMPL_EXPORT	SCCOL_Size(
							scColumn*	col,	// @parm <c scColumn>
							scSize& 	size ); // @parm <c scSize>



/*===================== SCRAP CONVERSION  ======================*/


// These routines provide a means of converting between the Toolbox
// world and the outside world, typically a conversion into the
// lowest common denominator -- text.
//


// @func Converts Toolbox scrap in the scScrapPtr to global scrap and
// stores it in the given Handle, which should be passed in as a 
// valid handle. The Handle then contains
// a "C" string. This is one case where the Toolbox will not free
// the new structure it creates.
//
status scIMPL_EXPORT	SCSCR_ConToSys(
							scScrapPtr			scrap,		// @parm <c scScrapPtr>
							SystemMemoryObject& memobj );	// @parm <c SystemMemoryObject>




// @func Converts global scrap in the Handle to Toolbox scrap
// and places it in the given scScrapPtr, putting it on
// the internal Toolbox clipboard. The handle should contain
// a "C" string, so that when 0 is encountered, we stop reading.
//
status scIMPL_EXPORT	SCSCR_SysToCon(
							scScrapPtr& scrap,		// @parm <c scScrapPtr> 								   
							const scChar*	stringscrap,// @parm Null terminated "C" string
													// from system scrap.
							TypeSpec	ts );		// @parm <t TypeSpec> to apply to string.




// @func Frees the scScrapPtr.
//
status scIMPL_EXPORT	SCSCR_Free(
							scScrapPtr scrap ); 	// @parm <c scScrapPtr>




// @func Returns the list of specs referenced by the contents of the scScrapPtr.
//
status scIMPL_EXPORT	SCSCR_TSList(
							scScrapPtr		scrap,		// @parm <c scScrapPtr>
							scTypeSpecList& tslist );	// @parm <c scTypeSpecList>




// @func Writes the contents of the scScrapPtr to disk.

status scIMPL_EXPORT	SCSCR_Write(
							scScrapPtr	scrapPtr,		// @parm <c scScrapPtr>
							APPCtxPtr	ctxPtr, 		// @parm <t APPCtxPtr>		 
							IOFuncPtr	iofuncp );		// @parm <t IOFuncPtr>




// @func Reads from disk into the scScrapPtr.
// Scrap to be read must be a column.
// @xref <f SCSET_InitRead>
status scIMPL_EXPORT	SCSCR_ReadCol(
							scScrapPtr& 	scrap,		// @parm <c scScrapPtr>
							scSet*			enumtable,	// @parm <c scSet>
							APPCtxPtr		ctxPtr, 	// @parm <t APPCtxPtr>			   
							IOFuncPtr		readFunc ); // @parm <t IOFuncPtr>




// @func Reads from disk into the scScrapPtr, using the call back
// read routine, which should conform to the same header as above.
// Scrap to be read must be a stream.
//
status scIMPL_EXPORT	SCSCR_ReadStream(
							scScrapPtr& 	scrap,		// @parm <c scScrapPtr>
							scSet*			enumtable,	// @parm <c scSet>
							APPCtxPtr		ctxPtr, 	// @parm <t APPCtxPtr>			   
							IOFuncPtr		readFunc ); // @parm <t IOFuncPtr>




/*===================== TYPE SPECIFICATION =====================*/


// @func Informs the Toolbox that the TypeSpec has been changed and 
// tells it what action needs to be taken to respond accordingly. 
// The Toolbox will recompose, rebreak, or repaint (as instructed 
// by SpecTask) and report back on damage. SpecTasks may be ORed 
// together to indicate multiple tasks. The host application 
// can derive the tasks by calling the function SpecTaskCalculate, 
// located in the source module (delivered with the Toolbox) 
// sc_spchg.cpp.


status scIMPL_EXPORT	SCENG_ChangedTS(
							TypeSpec		ts, 		// @parm <t TypeSpec> has changed and text needs to be
														// reformatted to reflect the change.
							eSpecTask		specTask,	// @parm <t eSpecTask> tells the toolbox
														// what action to take to repair the change.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero. 									




// @func Gets a list of TypeSpecs in a column.
// @xref <f SCSTR_TSList>, <f SCSEL_TSList>

status scIMPL_EXPORT	SCCOL_TSList(
							scColumn*		col,		// @parm <c scColumn> to query.
							scTypeSpecList& tslist );	// @parm <c scTypeSpecList> contains
														// a list of specs used.


// @func Gets a list of TypeSpecs in a stream. When working with linked columns,
// this is more efficient than iteratively calling SCColTSList.
// @xref <f SCCOL_TSList>, <f SCSEL_TSList>

status scIMPL_EXPORT	SCSTR_TSList(
							scStream*		col,		// @parm <c scStream> to query.
							scTypeSpecList& tslist );	// @parm <c scTypeSpecList> contains
														// a list of specs used.

// @func Gets a list of ParaTypeSpecs in a stream.
// @xref <f SCCOL_TSList>, <f SCSEL_TSList>

status scIMPL_EXPORT	SCSTR_ParaTSList(
							scStream*		col,		// @parm <c scStream> to query.
							scTypeSpecList& tslist );	// @parm <c scTypeSpecList> contains
														// a list of specs used.

// @func Gets a list of TypeSpecs in a selection.
// @xref <f SCCOL_TSList>, <f SCSTR_TSList>

status scIMPL_EXPORT	SCSEL_TSList(
							scSelection*	sel,		// @parm <c scSelection>.
							scTypeSpecList& tslist );	// @parm <c scTypeSpecList> contains
														// a list of specs used.
status scIMPL_EXPORT	SCSEL_PARATSList(
							scSelection*	sel,		// @parm <c scSelection>.
							scTypeSpecList& tslist );	// @parm <c scTypeSpecList> contains
														// a list of specs used.


// @func Gets a list of the (TypeSpec, character location) pairs
// representing the TypeSpec runs of the stream.

status scIMPL_EXPORT	SCSTR_CHTSList(
							scStream*		 stream,	// @parm <c scStream> to query.
							scSpecLocList&	cslist );	// @parm <c scCharSpecList> has
														// positions of all specs contained.

status scIMPL_EXPORT	SCSTR_PARATSList( scStream* 		stream,
										  scSpecLocList&	cslist );	


// @func Gets a list of the (TypeSpec, character location) pairs
// representing the TypeSpec runs of the selection.
// CAUTION: the ends of paragraphs are marked by NULL specs,
// so depending on how many paragraphs the selection traverses,
// there may be multiple NULL specs contained in the list.
// These NULL specs are not terminators of the list;
// rely upon the CharSpecListHandle's count field for the
// accurate number of structures in the list.
//

status scIMPL_EXPORT	SCSEL_CHTSList(
							scSelection*	 sel,		// @parm <c scSelection> to query.
							scSpecLocList&	cslist );	// @parm <c scCharSpecList> has
														// positions of all specs contained.

status scIMPL_EXPORT	SCSEL_PARATSList( scSelection*		sel,
										  scSpecLocList&   cslist );	



// @func Counts the characters in a stream. This
// does not repesent an exact count for file i/o
// of characters written out.
//

status scIMPL_EXPORT	SCSTR_ChCount(
							scStream*	str,		// @parm <c scStream> to query.
							long&		chCount );	// @parm Characters in stream.




#ifdef scFlowJustify

// @func Sets the vertical justification attributes for the column to be
// flush top (no justification), flush bottom, centered, justified,
// or force justified. 
//
status scIMPL_EXPORT	SCCOL_FlowJustify(
							scColumn*	col,		// @parm <c scColumn>
							eVertJust	vj );		// @parm <t eVertJust>




// @func Sets the depth of a vertically flexible column and
// vertically justifies it. This is the only way to vertically
// justify a vert flex column. It should not be used with columns
// that are not vert flex.
// @xref <f SCCOL_SetVertFlex>

status scIMPL_EXPORT	SCCOL_SetDepthNVJ(
							scColumn*		col,		// @parm <c scColumn>
							MicroPoint		depth,		// @parm Depth to VJ to.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.




#endif /* scFlowJustify */


/*============== CONTENT CUT, COPY, PASTE & CLEAR ==============*/


// These routines move text in and out of the Toolbox,
// with filters converting text as necessary.
//

// @func Appends the text contained in the APPText to the end
// of the stream associated with the scColumn* (formerly SCReadAPPText).
// @xref <f SCSTR_GetAPPText>

status scIMPL_EXPORT	SCFS_PasteAPPText(
							scColumn*		col,		// @parm <c scColumn> in flow set to paste
														// the text.  
							stTextImportExport& 	appText,	// @parm <c scAPPText> contains "marked up" text.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero. 									  


status scIMPL_EXPORT	SCSEL_PasteAPPText(
							scSelection*		sel,		// @parm <c scColumn> in flow set to paste
															// the text.  
							stTextImportExport& 	appText,// @parm <c scAPPText> contains "marked up" text.
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero. 									  


// @func Returns a copy of the given stream in APPText&.
// @xref <f SCFS_PasteAPPText>

status scIMPL_EXPORT	SCSTR_GetAPPText(
							scStream*		str,		// @parm <c scStream> to get "marked up" text from.
							stTextImportExport& 	appText );	// @parm <c scAPPText> contains "marked up" text.										  

status scIMPL_EXPORT	SCSEL_GetAPPText(
							scSelection*			str,		// @parm <c scStream> to get "marked up" text from.
							stTextImportExport& 	appText );	// @parm <c scAPPText> contains "marked up" text.										  



/*======================== CONTENT I/O =========================*/


// These routines read and write ASCII text files with mark-up. */

// @func Imports Latin-1 text -- adds the contents of the text file
// to the column using the TypeSpec as the default text specification.
// The call back IO function should conform to the header:

status scIMPL_EXPORT	SCFS_ReadTextFile(
							scColumn*		col,	// @parm <c scColumn>
							TypeSpec		spec,	// @parm <t TypeSpec>
							APPCtxPtr		ctxp,	// @parm <t APPCtxPtr>				   
							IOFuncPtr		read,	// @parm <t IOFuncPtr>
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.
	



// @func Exports text -- writes the stream to the text file.
//
status scIMPL_EXPORT	SCSTR_WriteTextFile(
							scStream*	stream, // @parm <c scStream>
							APPCtxPtr	ctxp,	// @parm <t APPCtxPtr>				   
							IOFuncPtr	write );// @parm <t IOFuncPtr>



// @func Reads an Latin-1 text file and returns a scrap handle to it.
// This is useful for importing text and pasting it into a stream
// at an insertion point. The call back IO function should
// conform to the same header as above.
//
status scIMPL_EXPORT	SCSCR_TextFile(
							scScrapPtr& 	scrapP, // @parm <t scScrapPtr>
							APPCtxPtr		ctxp,	// @parm <t APPCtxPtr>				   
							IOFuncPtr		read ); // @parm <t IOFuncPtr>





/*=========================  FILE I/O ==========================*/


// These routines read and write the Toolbox structures to disk. */

// @func Tells the Toolbox that the application is about to
// commence writing structures out. This zeros the enumeration
// count of all objects in the Toolbox within this context
//
status scIMPL_EXPORT	SCTB_ZeroEnumeration( void );



// @func Tells the object to enumerate itself. 
//
status scIMPL_EXPORT	SCOBJ_Enumerate(
							scTBObj*	obj,
							long&		ecount );


// @func Stores the structures for this object. Streams and linked columns
// are by default written out with the first column; writes to columns
// that are not the first column of a stream are no-ops.
//
status scIMPL_EXPORT	SCCOL_Write(
							scColumn*	col,	// @parm <c scColumn>
							APPCtxPtr	ctxp,	// @parm <t APPCtxPtr>				   
							IOFuncPtr	write );// @parm <t IOFuncPtr>



// @func Tells the Toolbox that the application is about to
// commence reading structures in. This allocates an enumeration
// structure of class scSet, when the file was written out the
// client probably should have noted the enumeration count, by doing this
// we may allocate enough members of the enumeration structure
// to at least guarantee that there will be no failure in inserting
// members into the enumeration structure
//
status scIMPL_EXPORT	SCSET_InitRead(
							scSet*& enumerationTable,		// @parm Pointer to enum table
															// that Composition Toolbox will
															// allocate - pre allocating the
															// number of slots indicated to
															// minimize memory failures on 
															// file i/o.
							long	preAllocationCount );	// @parm Preallocate this many slots
															// in the enum table.


// @func Lets the Toolbox know the application is finished reading
// so it can free structures for restoring pointers and will
// recompose everything that needs recompostion.
//
status scIMPL_EXPORT	SCSET_FiniRead(
							scSet*			enumTable,	// @parm This table will be freed.
							scRedispList*	rInfo );	// @parm <c scRedispList>
														// Redisplay info, arg may be zero.




// @func Read this column.
//
status scIMPL_EXPORT	SCCOL_Read(
							APPColumn	appcol, 	// @parm Client's <t APPColumn> to
													// associate with this column.
							scColumn*&	col,		// @parm <c scColumn>
							scSet*		enumTable,	// @parm <c scSet>
							APPCtxPtr	ioctxptr,	// @parm <t APPCtxPtr> Abstract file i/o type.
							IOFuncPtr	ioFuncPtr );// @parm <t IOFuncPtr> write function pointer.


// @func Tells the object to restore its pointers. This
// function relies upon <f APPDiskIDToPointer>.
//
status scIMPL_EXPORT	SCOBJ_PtrRestore(
							scTBObj*	obj,			// @parm Restore this objects pointers.
							scSet*		enumTable );	// @parm Use this enumtable.


// @func Prior to calling SCOBJ_PtrRestore the client may want to abort
// the action for some reason. In that case call the following
// and all objects that have been read in, but have not had
// their pointers restored will be deleted, including the
// the enumeration table (scSet)
//
status scIMPL_EXPORT	SCSET_Abort(
							scSet*& enumTable );	// @parm <c scSet>


// @func Gives the size of a Toolbox object that will be written to disk.
// The first column in a flow set will contain the content/sctream.
//
status scIMPL_EXPORT	SCExternalSize(
							scColumn*	col,		// @parm <c scColumn>
							long&		bytes );	// @parm Disk bytes.


/*===================== SELECTION MESSAGES =====================*/


// @func Forces the initial selection within an empty container by creating
// an initial stream. Two conditions present interesting error conditions
// with this call.
// <nl>1. If the formatted text cannot fit into the container a
// scERRstructure is returned,
// <nl>2. If the container is not the first in a flow set then
// scERRlogical is returned.		
// <nl>The client must perform the first highlighting of the cursor by
// following this call with a call to <f SCHiLite>. 

status scIMPL_EXPORT	SCCOL_InitialSelect(
							scColumn*		col,		// @parm <c scColumn>
							TypeSpec&		spec,		// @parm <t TypeSpec>
							scSelection*&	select );	// @parm <c scSelection>




// @func Provides information on how good the hit is,
// to be used for selection evaluation.
// the REAL num is the distance squared in micropoints from the
// hitpoint to the nearest charcter and its baseline
//
status scIMPL_EXPORT	SCCOL_ClickEvaluate(
							scColumn*		 col,	// @parm <c scColumn>.
							const scMuPoint& evalpt,// @parm Point to evaluate.
							REAL&			 dist );// @parm Squared dist in <t MicroPoints>.


// @func The mouse down click should force a call of SCCOL_StartSelect and
// mouse moves should get <f SCCOL_ExtendSelect>. Effectively the StartSelect
// message forces a flow set to get the focus. Is is an error to call
// ExtendSelect with a column from a different flow set than was called
// from the original StartSelect. Also the client may want to coerce points
// to lie within the column's extents. If the container is rotated the coercion
// should happen in the containers coordinate space to insure correct
// interpretation of the coercion.
// After the first StartSelect message the Selection is accurate so if
// auto scrolling is necessary it may be done, provided the clip region
// is set up correctly.
// <nl>
// <nl>NOTE: The caller should filter out redundant mouse hits, (i.e if the 
// current mouse hit is the same as the last mouse hit don't call 
// SCCOL_ExtendSelect
//
status scIMPL_EXPORT	SCCOL_StartSelect(
							scColumn*			col,	// @parm <c scColumn>
							const scMuPoint&	hitpt,	// @parm The hit point in
														// object coordinates. 
							HiliteFuncPtr		hlfunc, // @parm <t HiliteFuncPtr>
							APPDrwCtx			drwctx, // @parm <t APPDrwCtx>
							scSelection*&		sel );	// @parm <c scSelection> to be filled in
														// by the Composition Toolbox.



// @func This extends the selection from the scStreamLocation and then may
// be followed with <f SCCOL_ExtendSelect>.
// @xref <f SCCOL_StartSelect>
status scIMPL_EXPORT	SCCOL_StartSelect(
							scColumn*			col,	// @parm <c scColumn>
							scStreamLocation&	stloc,	// @parm <c scStreamLocation>
							const scMuPoint&	hitpt,	// @parm The hit point in
														// object coordinates. 
							HiliteFuncPtr		hlfunc, // @parm <t HiliteFuncPtr>
							APPDrwCtx			drwctx, // @parm <t APPDrwCtx>
							scSelection*&		sel );	// @parm <c scSelection>
										  


// @func Extends a selection derived from <f SCCOL_ExtendSelect>. Can
// be used over multiple columns in a flowset.

status scIMPL_EXPORT	SCCOL_ExtendSelect(
							scColumn*			col,	// @parm <c scColumn>
							const scMuPoint&	hitpt,	// @parm The hit point in
														// object coordinates. 
							HiliteFuncPtr		hlfunc, // @parm <t HiliteFuncPtr>
							APPDrwCtx			drwctx, // @parm <t APPDrwCtx>
							scSelection*		sel );	// @parm <c scSelection>



// @func Converts a <c scSelection> into a mark and point.
// The mark is guaranteed to logically precede the point.
// This call is typically used in conjunction with <f SCSEL_Restore> and
// to determine information at the selection point.
// @xref <f SCCOL_StartSelect>, <f SCCOL_ExtendSelect>, <f SCCOL_InitialSelect>

status scIMPL_EXPORT	SCSEL_Decompose(
							scSelection*		sel,	// @parm <c scSelection>
							scStreamLocation&	mark,	// @parm <c scStreamLocation> 
							scStreamLocation&	point );// @parm <c scStreamLocation>

// same as above except that the selection is not sorted
status scIMPL_EXPORT	SCSEL_Decompose2(
							scSelection*		sel,	// @parm <c scSelection>
							scStreamLocation&	mark,	// @parm <c scStreamLocation> 
							scStreamLocation&	point );// @parm <c scStreamLocation>


// @func Invalidates the selection in the toolbox, changes the selection
// to null and invalidates the selection in the toolbox

status scIMPL_EXPORT	SCSEL_Invalidate(
							scSelection*& sel );	// @parm <c scSelection>

/*======================================================================*/
// @func Sets up a text selection, using the given
// mark and point. Useful for restoring the selection
// when re-activating a document, and for undo and redo,
// especially for undoing arrow and backspace keystrokes.
// In this call the <c scStreamLocation> need only have the following
// member variables filled in:
// <nl> fParaNum	
// <nl> fParaOffset
// <nl>All the rest are unneeded. After this call subsequent calls
// to <f SCSEL_Decompose> will fill in the scStreamLocation values
// correctly.

status scIMPL_EXPORT	SCSEL_Restore(
							const scStream* 		stream, // @parm <c scStream>
							const scStreamLocation& mark,	// @parm <c scStreamLocation> 
							const scStreamLocation& point,	// @parm <c scStreamLocation>
							scSelection*&			sel,	// @parm <c scSelection>
							Bool					geometryChange );	// has the layout changed



/*======================================================================*/
// @func Using a hitpt and a modifier (such as selection of a word or paragraph),
// returns a selection.
//
status scIMPL_EXPORT	SCCOL_SelectSpecial(
							scColumn*			col,	// @parm <c scColumn>
							const scMuPoint&	hitPt,	// @parm Hit point.
							eSelectModifier 	mod,	// @parm <t eSelectModifier>
							scSelection*&		sel );	// @parm <c scSelection>



/*======================================================================*/
// This grows the selection according to the enum eSelectMove found in 
// sctypes.h
//

status scIMPL_EXPORT	SCSEL_Move( scSelection*,
									eSelectMove );


/*======================================================================*/
// This alters the selection point according to the enum eSelectMove found in 
// sctypes.h
//

status scIMPL_EXPORT	SCSEL_Extend( scSelection*, eSelectMove );


/*======================================================================*/
// select the nth pagraph of a stream - if you go off the end 
// status returns noAction
//
// [ ] [ ]
status scIMPL_EXPORT	SCSTR_NthParaSelect( scStream*		streamID, 
											 long			nthPara, 
											 scSelection*	select );




/*======================================================================*/
// @func Highlights the current selection, using the function pointer passed
// in, the coordinates that will be contained in the call back are in
// object coordinates and MUST be transformed to device coordinates.
//
status scIMPL_EXPORT	SCSEL_Hilite(
							scSelection*	sel,			// @parm <c scSelection>
							HiliteFuncPtr	appDrawRect );	// @parm <t HiliteFuncPtr>


/*====================== EDITING MESSAGES ======================*/

// @func Inserts keystrokes into the given stream. Place one or more keystrokes
// into the array. NULL values in the key record array will simply be ignored.
// scKeyRecord* points to an array of keystrokes; numKeys is the number
// of elements in the array.
// Stores information for undo in the key records. Sample code will illustrate
// how to take the inverse values in scKeyRecord and use them to undo the 
// insertion of keystrokes.
//
status scIMPL_EXPORT	SCSEL_InsertKeyRecords(
							scSelection*	sel,		// @parm <c scSelection>
							short			numRecs,	// @parm Number of key recs
														// to follow.
							scKeyRecord*	keyRecs,	// @parm <c scKeyRecord>
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.

// @func For immediate redisplay of text that has been altered in editing.
// It redisplays only those lines which the cursor has been on.
// Typing a carriage return should force two lines to be redisplayed 
// immediately; likewise for a backspace at the beginning of a line.
// Normally only one line needs to be redisplayed.
// IF THE OPERATION CROSSES COLUMNS, ONLY THE COLUMN
// IN WHICH THE CURSOR ENDS UP IS UPDATED.
//
status scIMPL_EXPORT	SCCOL_UpdateLine(
							scColumn*			col,		// @parm <c scSelection>
							scImmediateRedisp&	immred, 	// @parm <c scImmediateRedisp>
							APPDrwCtx			drwctx );	// @parm <t APPDrwCtx>




// @func Applies a <t TypeSpec> to a <c scSelection>. The TypeSpec replaces
// all specs contained in the selection.
//
status scIMPL_EXPORT	SCSEL_SetTextStyle(
							scSelection*	sel,		// @parm <c scSelection>
							TypeSpec		ts, 		// @parm <t TypeSpec>
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.







// Applies a character transformation (to all caps, for example)
// to the selection.
//
// [ ] [ ]
status scIMPL_EXPORT	SCSEL_TextTrans( scSelection*,
										 eChTranType,
										 int,
										 scRedispList* );




/*============ TEXT LEVEL CUT, COPY, PASTE & CLEAR =============*/
// These routines cut and paste paragraphs and text characters.
// All relevant paragraph attributes and character attributes are maintained.
// The scrap is maintained in internal Toolbox formats.
//

// @func Cuts a selection of text, returning it in the scScrapPtr.
//
status scIMPL_EXPORT	SCSEL_CutText(
							scSelection*	sel,		// @parm <c scSelection>
							scScrapPtr& 	scrapPtr,	// @parm <c scScrapPtr>
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.




// @func Deletes a selection of text.
//
status scIMPL_EXPORT	SCSEL_ClearText(
							scSelection*	sel,		// @parm <c scSelection>
							scRedispList*	rInfo );	// @parm <c scRedispList> 
														// Redisplay info, arg may be zero.




// @func Copies a selection of text, returning the copy in the scScrapPtr.
// <a name="SCSEL_CopyText">-</a>
status scIMPL_EXPORT	SCSEL_CopyText(
							scSelection*	sel,		// @parm <c scSelection>
							scScrapPtr& 	scrap );	// @parm <c scScrapPtr>




// @func Pastes a selection of text contained in scScrapPtr into a stream
// at the insertion point marked by scSelection. If scSelection is
// a selection of one or more characters, the selected characters
// are deleted from the stream before the scScrapPtr text is pasted in.
// This operation copies the text as it pastes it. Therefore,
// multiple pastes can be made with the same scScrapPtr
// without making any explicit copies.
// @xref <f SCSEL_CutText>, <f SCSEL_CopyText>
status scIMPL_EXPORT	SCSEL_PasteText(
							scSelection*	sel,	// @parm <c scSelection>
							scScrapPtr		scrap,	// @parm <c scScrapPtr>
							TypeSpec		ts, 	// @parm If NULL uses prev spec.
							scRedispList*	rInfo );// @parm <c scRedispList> 
													// Redisplay info, arg may be zero.



/*======================================================================*/
// @func The following is a useful call to get a stream from a selection and the
// the first typespec in the selection
//
status scIMPL_EXPORT	SCSEL_GetStream(
							const scSelection*	sel,	// @parm <c scSelection>
							scStream*&			str,	// @parm <c scStream>	
							TypeSpec&			ts );	// @parm The first <t TypeSpec>.




/*======================================================================*/
// see doc in html file 

status SCSEL_InsertField( scSelection*, 
						  const clField&, 
						  TypeSpec&,
						  scRedispList* damage );


/*======================================================================*/
// The following are for spell checkings */

// actual definition in SCTYPES.H
//	Substitution function
//	inWord and outWord are null terminated strings
//	outWord is allocated and freed by application
//
// the substitution should function returns 	
//	- successful	if the word has been changed
//	- noAction		if no change is necessary
//	- other error	to be propogated back to app
//	
//	typedef status (*SubstituteFunc)( UCS2**outWord, UCS2*inWord );
//	
//

/*================================================*/
// DEPRECATED													
// check spelling in selection,
// this is best used in conjunction with SCMoveSelect 
//
// [ ] [ ]
status scIMPL_EXPORT	SCSEL_Iter( scSelection*		selectID,
									SubstituteFunc	func,
									scRedispList*	damage );


/*================================================*/
// DEPRECATED
// check spelling in the stream 
//
// [ ] [ ]
status scIMPL_EXPORT	SCSTR_Iter( scStream*		streamID,
									SubstituteFunc	func,
									scRedispList*	damage );


/*================================================*/
// DEPRECATED
// [ ] [ ]
status scIMPL_EXPORT	SCSTR_Search( scStream* 	streamID,
									  const UCS2*		findString,
									  SubstituteFunc	func,
									  scRedispList* damage );

// @func Search for the string from the current selection. When
// the string is found move the selection to it.
status scIMPL_EXPORT	SCSEL_FindString(
							scSelection*	select, 	// @parm <c scSelection>
							const UCS2* findString );	// @parm <t UCS2> string to find.

/*================================================*/
// this returns whether or not the column potentially has text, if
// because of reformatting nothing lands in here we will still return
// true successful == has text
//


// [ ] [ ]
status scIMPL_EXPORT	SCCOL_HasText( scColumn* colID );

/*================================================*/
// at start up this will have the first token selected

class stTokenIter {
public:
	virtual void	release() = 0;
	virtual void	reset() = 0;

				// sets a selection of the iterators para
	virtual int 	paraselection( scSelection* ) = 0;

				// sets selection for the token iterator
	virtual int 	setselection( scSelection* ) = 0;

				// retrieves the next token,
				// the string should have its size set in the
				// len field, if the token will not fit
				// in the string, a negative number is returned
				// that specifies the size, try again with a sufficient
				// size string
	virtual int 	gettoken( stUnivString& ) = 0;

				// replaces the current token
	virtual int 	replacetoken( stUnivString& ) = 0;

				// moves to next token
	virtual int 	next() = 0;
};

class stContUnitIter {
public:
	virtual void	release() = 0;
	virtual void	reset() = 0;
	virtual int 	gettokeniter( stTokenIter*& ) = 0;
	virtual int 	next() = 0;
	virtual void	range( scStreamLocation&, scStreamLocation& ) = 0;
};


status SCSTR_GetContUnitIter( scStream*, stContUnitIter*& );
status SCSEL_GetContUnitIter( scSelection*, stContUnitIter*& );


/*================================================*/

class stFindIter {
public:
	virtual void	release() = 0;
	virtual void	reset() = 0;
	virtual int 	setselection( scSelection* ) = 0;
	virtual int 	next() = 0;
	virtual int 	replacetoken( stUnivString& ) = 0;
	virtual void	range( scStreamLocation&, scStreamLocation& ) = 0;	
};


status SCSTR_GetFindIter( scStream*, 
						  stUnivString&, 
						  const SearchState&,
						  stFindIter*& );
status SCSEL_GetFindIter( scSelection*, 
						  stUnivString&, 
						  const SearchState&,
						  stFindIter*& );

/*================================================*/


#ifdef scRubiSupprt
class scAnnotation;

// take the select id and find the nth annotation within the selection,
// if no annotation is found - noAction is returned
// [ ] [ ]
status scIMPL_EXPORT	SCSEL_GetAnnotation( scSelection*,
											 int nth,
											 scAnnotation& );

// apply the annotation to the indicated selectid id, if there is an
// existing annotation it will be deleted
// [ ] [ ]
status scIMPL_EXPORT	SCSEL_ApplyAnnotation( scSelection*,
											   const scAnnotation&,
											   scRedispList* );

#endif

// ==================================================================== */

void scIMPL_EXPORT		SCCOL_InvertExtents( scColumn*,
											 HiliteFuncPtr,
											 APPDrwCtx );
#if SCDEBUG > 1

// these return the size of the memory image of these structures
long scIMPL_EXPORT		SCCOL_DebugSize( scColumn* );
long scIMPL_EXPORT		SCSTR_DebugSize( scStream* );



void scIMPL_EXPORT		SCCOL_InvertRegion( scColumn*,
											HiliteFuncPtr,
											APPDrwCtx );

void scIMPL_EXPORT		SCDebugColumnList( void );
void scIMPL_EXPORT		SCDebugColumn( scColumn*,
									   int	contentLevel );

void scIMPL_EXPORT		SCDebugParaSpecs( scSelection* );

void scIMPL_EXPORT		SCSTR_Debug( scStream* );

#endif /* DEBUG */



#endif /* _H_SCAPPINT */
//</pre></html>
