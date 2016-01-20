/*
// $Workfile: photoprj.h $
// $Revision: 1 $
// $Date: 3/03/99 6:09p $
*/

/*
// Revision History:
//
// $Log: /PM8/App/photoprj.h $
// 
// 1     3/03/99 6:09p Gbeddow
// 
// 4     2/24/99 9:14p Psasse
// fixed a memory leak
// 
// 3     1/30/99 11:32p Psasse
// Updated and fairly stable EasyPrints code
// 
// 2     1/26/99 6:34p Psasse
// Added PhotoProjects Project Type (still in progress)
// 
// 1     1/13/99 1:41p Psasse
// initial add
// 
*/

#ifndef __PHOTOPRJ_H__
#define __PHOTOPRJ_H__

#include "labels.h"

class CPhotoLabelData : public CLabelData
{
public:
	CPhotoLabelData();
	virtual ~CPhotoLabelData();

		/*
	// Draw the page to a DC.
	*/
	// bPaintPage added for drawing stiker desktop
	void DrawPage(CDC* pDC, const CRect& crClient, BOOL fLandscape = FALSE, CRect* pDrawnRect = NULL, short drawmode = DRAW_PREVIEW, BOOL fEraseBackground = TRUE);

	void DrawPage3D(CDC* pDC, const CRect& crClient, int nGap, BOOL fLandscape = FALSE, CRect* pDrawnRect = NULL, CRect* pEraseRect = NULL, BOOL fEraseBackground = TRUE);


};

class CPhotoPrjData 
{
public:

	CPhotoPrjData();
	virtual ~CPhotoPrjData();
	int m_nType;
	CPhotoLabelData* m_pSubAreaData[3];  //CPaperInfo[3] !!!
};


class CPhotoPrjList : public CLabelList
{
public:

	virtual ~CPhotoPrjList();
	BOOL ReadList(LPCSTR FileName);
	CPhotoPrjData* FindPhotoProject(const CString& strPhotoProject);

	int PhotoProjects(void) const
		{ return m_cpaPhotoProjects.GetSize(); }
	CPhotoPrjData* PhotoProject(int nPhotoProject) const
		{ return (CPhotoPrjData*)m_cpaPhotoProjects.GetAt(nPhotoProject); }

protected:
	// array of CLabelData items
	CPtrArray m_cpaPhotoProjects;

};

#endif
