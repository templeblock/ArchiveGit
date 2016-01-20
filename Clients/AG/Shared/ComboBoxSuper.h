#pragma once

//#pragma warning ( disable : 4786 )

// The following code does not like the 'new' debug macro, so save it undefine it, and restore it
#pragma push_macro("new")
#undef new
	#include <map>
	#include <vector>
#pragma pop_macro("new")

using namespace WTL;
//////////////////////////////////////////////////////////////////////////////////////////////////////////
//	class CComboBoxSuper
//
// Implements Combo-box with a colored items, icons, bold items, and multiple columns.
//
//	Usage:
//	To use in a dialog - add a normal combo to the dialog, and replace the class from CComboBox to CComboBoxSuper.
//
//	Make sure to change the combo style to Owner Drawn Fixed and Has String in the resource editor, or by code.
//
//	To create on the fly - call create of the CComboBoxSuper object, and don't forget the styles.
//
//	Note: I always used the combo as a drop-list, in dropdown you get the first column's text only for editing.
 
class CComboBoxSuper : public CWindowImpl<CComboBoxSuper, CComboBox>
{
// Construction
public:
	CComboBoxSuper();
	virtual ~CComboBoxSuper(){};

	CComboBoxSuper& operator =(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}
	struct ItemData
	{
		ItemData() : crTextColor(RGB(0,0,0)),nImageIndex(-1),bBold(FALSE){}
		COLORREF crTextColor;
		int nImageIndex;
		std::map<int,CString> mapStrings;
		BOOL bBold;
		DWORD dwItemData;
	};

	std::vector<int> m_vecColumnWidth;

	CImageList* m_pImageList;
	BOOL m_bUseImage;

	static const int DEFAULT_COLUMN_COUNT;
	static const int DEFAULT_COLUMN_WIDTH;


	// Operations
public:

                             
    DECLARE_WND_SUPERCLASS(_T("SUPERCOMBOBOX"), TEXT("COMBOBOX"))

	BEGIN_MSG_MAP(CComboBoxSuper)
		MESSAGE_HANDLER(WM_DELETEITEM, OnDeleteItem)
    END_MSG_MAP()

	LRESULT OnDeleteItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled);

// Implementation
public:

	/** Sets to TRUE for using the image list, of FALSE to disable the use of the image-list. */
	void SetUseImage(BOOL bUseImage=TRUE) { m_bUseImage=bUseImage; Invalidate(); }

	/** Sets the number of columns to use - new columns are inserted at default width. */
	void SetColumnCount(int nColumnCount);

	/** Sets the width of a specific column. */
	void SetColumnWidth(int nColumnIndex, int nWidth);

	/** Sets the image list to use - will be show only if SetUseImage is called. */
	void SetImageList(CImageList* pImageList) { m_pImageList = pImageList; }

	/** Set a specific row to bold. */
	void SetItemBold(int nItemIndex, BOOL bBold = TRUE);

	/** Sets a row's image index. */
	void SetItemImage(int nItemIndex, int nImageIndex);

	/** Sets a row's color. */
	void SetItemColor(int nItemIndex, COLORREF rcTextColor);

	/** Sets an item or sub-item text. */
	void SetItemText(int nItemIndex, int nColumn, CString str);
	
	/** Sets item data (override the default function) */
	void SetItemData(int nItemIndex, DWORD dwData);

	/** Get item data(override the default function) */
	DWORD GetItemData(int nItemIndex);

	/** Gets item or sub-item text. */
	CString GetItemText(int nItemIndex,int nColumn);


	// Overridables
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

protected:

	ItemData* GetOrCreateItemData(int nItemIndex);
	void MeasureItem(LPMEASUREITEMSTRUCT) {}
};
