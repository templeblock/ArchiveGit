#pragma once

struct PaperTemplate
{
	LPCSTR pProduct;
	LPCSTR pName;
	double fCustomWidth;
	double fCustomHeight;
	int nCols;
	int nRows;
	double fLeft;
	double fTop;
	double fWidth;
	double fHeight;
	double fWidthWithGap;
	double fHeightWithGap;
	bool bReverseOrientation;
};

class CPaperTemplates
{
private:
	static bool SizeMatch(const SIZE& Size, SIZE& DeltaSize, const PaperTemplate* pTemplate);
public:
	static int FindSizeMatch(int iAGDocType, SIZE& Size);
	static bool AdjustSize(int iAGDocType, SIZE& Size);
	static void StuffCombo(int iAGDocType, int iTemplate, const PaperTemplate** pRetTemplate, HWND hCombo, int iWidth, int iHeight, bool bIgnoreCompetitors = false);
	static int GetTemplate(int iAGDocType, int iTemplate, const PaperTemplate** pRetTemplate);
};
