#ifndef _ADDPAGEPGPREV_H_
#define _ADDPAGEPGPREV_H_

#pragma pack(push, BYTE_ALIGNMENT)

// ****************************************************************************
//
//  Class Name:	RPagePreview
//
//  Description:	A pure virtual class for page preview
//
// ****************************************************************************
//
class FrameworkLinkage RPagePreview
{
	public:
		RPagePreview();		//ctor
		~RPagePreview();		//dtor

		virtual CSize GetPagePreviewSize() = 0;
		
		virtual CString GetPageName() = 0;
		
		virtual void PaintPagePreview(CWnd *pWnd, CPoint ptPosition) = 0;

};


// ****************************************************************************
//
//  Class Name:	RDocPagePreviews
//
//  Description:	A pure virtual class for a list of page previews
//
// ****************************************************************************
//
class FrameworkLinkage RDocPagePreviews
{
	public:
		RDocPagePreviews();		//ctor
		~RDocPagePreviews();		//dtor

		virtual CPtrList &GetPagePreviewList() = 0;

	// members
	public:
		CPtrList	m_pagePreviewList;
};


// ****************************************************************************
//
//  Class Name:	RAppQuickStartPages
//
//  Description:	QSL stuff
//
// ****************************************************************************
//
class FrameworkLinkage RAppQuickStartPages
{ 
	public:
		RAppQuickStartPages();		//ctor
		~RAppQuickStartPages();		//dtor
};

#pragma pack(pop)

#endif