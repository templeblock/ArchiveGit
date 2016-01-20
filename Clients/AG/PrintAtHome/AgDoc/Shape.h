#pragma once

#define SYMBOL_COMMANDPOINT(x)	x>100000
#define SYMBOL_POLYGON 100001
#define SYMBOL_BEZIER  100002
#define SYMBOL_LINE    100003
#define SYMBOL_MOVE    100004

static LPCSTR m_szShapeNames[] = {
		"Rectangle",
		"Circle/Ellipse",
		"Line",
		"Star1",
		"Star2",
		"Star3",
		"Star4",
		"Shape1",
		"Shape2",
		"Circle Mask",
		"Circle Frame",
		"Square Frame",
		"House2",
		"Jewel",
		"Headstone",
		"Rounded Corner",
		"Stop Sign",
//		"Skull",
		"Cross1",
		"Cross2",
		"Cross3",
		"Cross4",
		"Cross5",
		"Crown",
		"Cog",
		"Cup",
		"Egg",
		"Flame",
		"G Clef",
		"Quarter Note",
		"Gear",
		"Horseshoe",
		"Hand",
		"Leaf",
		"Phone",
		"Plane",
		"Arrow1",
		"Arrow2",
		"Arrow3",
		"Arrow4",
		"Arrow5",
		"Arrow6",
		"Arrow7",
		"Arrow8",
		"Bowling Pin",
		"Heart",
		"Starburst1",
		"Starburst2",
		"Starburst3",
		"Triangle",
		"Rounded Triangle",
		"Rounded Rectangle",
		"House",
		"Flower",
		"Lightning Bolt",
		"Badge1",
		"Badge2",
		"Badge3",
		"Banner1",
		"Banner2"
	};

class CAGSymDrawing;

class CShape  
{
public:
	CShape();
	virtual ~CShape();

	static CAGSymDrawing* GenerateSymbol(LPCSTR strShapeName, RECT DestRect);
	static bool CShape::RegeneratePoints(CAGSymDrawing* pSymDrawing, bool fAspect);
	static inline int GetShapeCount()
	{
		return sizeof(m_szShapeNames)/sizeof(LPCSTR);
	}
	static LPCSTR GetShapeName(int iIndex)
	{
		if (iIndex < 0 || iIndex >= GetShapeCount())
			return "";
		return m_szShapeNames[iIndex];
	}
	static int GetShapeIndex(LPCSTR pShapeName)
	{
		if (!pShapeName)
			return -1;
		for (int i = 0; i < GetShapeCount(); i++)
		{
			if (!_stricmp(pShapeName, CShape::GetShapeName(i)))
				return i;
		}
		
		return -1;
	}
};
