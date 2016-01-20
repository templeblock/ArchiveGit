#pragma once

typedef enum 
{
	TOOL_OFF,
	TOOL_SOLIDFILL,
	TOOL_SWEEPDOWN,
	TOOL_SWEEPRIGHT,
	TOOL_SWEEPCENTER,
	TOOL_REDEYE
} SelectedTool;

class CToolSettings
{
public:
	CToolSettings()
	{
		m_SelectedTool = TOOL_OFF;
		m_PrimaryColor = RGB(255,255,255);
		m_SecondaryColor = RGB(255,255,255);
		m_nTolerance = 0;
	}

	SelectedTool m_SelectedTool;
	COLORREF m_PrimaryColor;
	COLORREF m_SecondaryColor;
	int m_nTolerance;
};
