#pragma once

#include "AGLayer.h"

#define MAX_AGLAYER 2

class CAGPage  
{
public:
	CAGPage(int cx = 0, int cy = 0);
	~CAGPage();

	void AddLayer(CAGLayer* pLayer);
	void Draw(CAGDC& dc, DWORD dwFlags) const;
	void GetFonts(LOGFONTLIST& lfList);
	CAGPage* CAGPage::Duplicate(bool bCopySymbols, bool bFullPage, int iNewPageNumber, const CString& strNewPageName);
	CAGLayer* GetLayer(int nLayer) const;
	CAGLayer* GetActiveLayer() const;
	CAGLayer* GetMasterLayer() const;
	CAGLayer* GetUserLayer() const;
	CAGLayer* SetActiveLayer(int nActiveLayer);
	int GetNumLayers() const
		{ return m_nLayers; }
	void GetPageName(CString& strPageName) const
		{ strPageName = m_szPageName; }
	void SetPageName(const CString& strPageName)
		{ m_szPageName = strPageName; }
	void GetPageSize(SIZE& PageSize) const
		{ PageSize = m_PageSize; }
	void SetPageSize(const SIZE& PageSize)
		{ m_PageSize = PageSize; }
	bool IsPortrait() const;
	bool IsEmpty() const;
	bool Read(CAGDocIO* pInput);
	bool Write(CAGDocIO* pOutput) const;
	void CreateThumb(const SIZE& MaxSize, LPCSTR pszFileName, LPCSTR pszType = "DIB", HBITMAP* phBitmap = NULL);

protected:
	SIZE m_PageSize;
	CString m_szPageName;
	int m_nLayers;
	int m_nActiveLayer;
	CAGLayer* m_pLayers[MAX_AGLAYER];
};
