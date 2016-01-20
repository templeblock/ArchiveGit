/*
// $Header: /PM8/App/OLDFLOW.H 1     3/03/99 6:08p Gbeddow $
//
// Textflow context definitions.
//
// $Log: /PM8/App/OLDFLOW.H $
// 
// 1     3/03/99 6:08p Gbeddow
// 
//    Rev 1.0   14 Aug 1997 15:23:10   Fred
// Initial revision.
// 
//    Rev 1.0   14 Aug 1997 09:34:50   Fred
// Initial revision.
// 
//    Rev 1.1   26 Jun 1996 14:21:42   Jay
//  
// 
//    Rev 1.0   24 May 1996 10:14:54   Jay
// Initial revision.
// 
//    Rev 1.0   14 Mar 1996 13:42:50   Jay
// Initial revision.
// 
//    Rev 1.0   16 Feb 1996 12:06:14   FRED
// Initial revision.
// 
//    Rev 2.0   07 Feb 1995 15:49:04   JAY
// Initial revision.
// 
//    Rev 1.0   16 Dec 1993 16:26:06   JAY
//  
*/

#ifndef __OLDFLOW_H__
#define __OLDFLOW_H__

#if 0
FARCLASS TextFlow
{
private:
	PMGDatabasePtr database;

/*
// Refresh management.
*/

	VOID near add_width_refresh_extent(FramePtr frame, LINE_PTR lp,
			PCOORD xmin, PCOORD xmax, REFRESH_TYPE refresh_type);

	VOID near add_words_refresh_extent(FramePtr frame, LINE_PTR lp,
			TEXT_WORD_PTR first_word, TEXT_WORD_PTR last_word,
			REFRESH_TYPE refresh_type);

	BOOL want_refresh;

	FrameObjectPtr frame_object;
public:

	TextFlow(PMGDatabasePtr d, FrameObjectPtr obj, BOOL wr = TRUE)
			: database(d), want_refresh(wr), frame_object(obj) {}

	static WORD_TYPE character_type(CHARACTER c);

	VOID add_line_refresh_extent(FramePtr frame, LINE_PTR lp);

	ERRORCODE adjust_words(DB_RECORD_NUMBER p_record,
					DB_RECORD_NUMBER f_record,
					CHARACTER_DELTA cdelta, WORD_DELTA_PTR wdelta);

	ERRORCODE rebuild_words(DB_RECORD_NUMBER p_record,
					DB_RECORD_NUMBER f_record,
					CHARACTER_RANGE crange,
					WORD_DELTA_PTR wdelta, WORD_RANGE_PTR wrange);

	ERRORCODE size_words(DB_RECORD_NUMBER p_record, WORD_RANGE wrange, BOOL force);

	ERRORCODE adjust_lines(DB_RECORD_NUMBER f_record, WORD_DELTA wdelta);

	VOID position_lines(FramePtr frame, VERT_ALIGN_TYPE align_type);

	ERRORCODE rebuild_lines(FrameObjectPtr object, WORD_RANGE wrange);

	ERRORCODE refresh_frame(DB_RECORD_NUMBER f_record, CHARACTER_RANGE crange);

};
#endif

#endif					// #ifndef __OLDFLOW_H__
