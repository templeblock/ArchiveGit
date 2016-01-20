#include "stdafx.h"
ASSERTNAME

#include "pmw.h"
#include "projtext.h"
#include "sortarry.h"
#include "pmwdoc.h"
#include "textrec.h"
#include "macrostd.h"
#include "macrorec.h"
#include "util.h"
#include "frameobj.h"
#include "compfn.h"	// For CompositeFileName
#include "fontrec.h" // for FontDataRecord
#include "stylerec.h" // for TextStyleRecord
#include "pagerec.h" // for PageRecord
#include "layrec.h" // for ObjectLayoutRecord
#include "pathrec.h" // for OutlinePathRecord
#include "warpfld.h" // for WarpFieldRecord
#include "framerec.h" // for FrameRecord
#include "grpobj.h" // for GroupObject
#include "calobj.h" // for CalendarObject
#include "lineobj.h" // for LineObject
#include "patobj.h" // for PatternObject
#include "grafobj.h" // for GraphicObject
#include "backobj.h" // for BackgroundObject
#include "rectobj.h" // for RectangleObject
#include "brdrobj.h" // for BorderObject
#include "compobj.h" // for ComponentObject
#include "dateobj.h" // for CalendarDateObject
#include "tblobj.h" // for CTableObject
#include "oleobj.h" // for OleObject
#include "ComponentView.h" // for RComponentView
#include "ImageInterface.h" // for RImageInterface
#include "BitmapImage.h" // for RBitmapImage
#include "pmgobj.h" // for CObjectProperties
#include "carddoc.h" // for CCardDoc
#include "comprec.h" // for ComponentRecord
#include "ComponentDocument.h" // for ComponentDocument
#include "PathInterface.h" // for IPathInterface
#include "Typeface.h" // for FaceEntry
#include "PmgFont.h" // for PMGFontServer
#include "Path.h" // for RPath
#include "Paper.h" // for CPaperInfo

// Helper functions.

/////////////////////////////////////////////////////////////////////////////
static bool ExtractEmbedded(GraphicObject* pObj, const CString& strProjectPath, CString& strFile)
{
	if (!pObj)
		return false;

	GraphicRecord* pGraphicRecord = pObj->LockGraphicRecord();
	if (!pGraphicRecord)
		return false;

	ReadOnlyFile File;
	ERRORCODE e = pGraphicRecord->prep_storage_file(&File);
	if (e != ERRORCODE_None)
	{
		pGraphicRecord->release();
		return false;
	}

	CompositeFileName cfn(pGraphicRecord->m_csFileName);	// Handles goofy characters maybe used in name
	pGraphicRecord->release();
	pGraphicRecord = NULL;

	// Build a destination file path
	CString strFileName = strFile;
	strFile = CString("embedded\\");
	if (!Util::MakeDirectory(strProjectPath + strFile))
		int i = 0;

	int nDigits = 0;
	for (int i=0; i<strFileName.GetLength(); i++)
	{
		if (isdigit(strFileName.GetAt(i)))
			nDigits++;
	}

	if (nDigits >= 3) // It is likely to be a unique name (unlike CLIP.BMP or CLIP.WMF)
		strFile += strFileName;
	else
	{
		CString strGrafName = cfn.get_dynamic_name(TRUE);
		int iStart = strGrafName.ReverseFind('\\');
		if (iStart > 0)
			iStart = strGrafName.Left(iStart).ReverseFind('\\');
		strGrafName = strGrafName.Mid(iStart+1);
		strGrafName.Replace('\\', '_');
		strGrafName.Replace(":", "");
		strGrafName.Replace("[", "");
		strGrafName.Replace("]", "");
		strFile += strGrafName;
	}

	CString strExportFileName = strProjectPath + strFile;
	StorageFile	FileOut(strExportFileName);
	return (copy_file(&File, &FileOut) == ERRORCODE_None);
}

/////////////////////////////////////////////////////////////////////////////
static bool ExtractEmbedded(ComponentObject* pObj, const CString& strProjectPath, CString& strFile)
{
	if (!pObj)
		return false;

	// Build a destination file path
	CString strFileName = strFile;
	strFile = CString("embedded\\");
	if (!Util::MakeDirectory(strProjectPath + strFile))
		int i = 0;

	int nDigits = 0;
	for (int i=0; i<strFileName.GetLength(); i++)
	{
		if (isdigit(strFileName.GetAt(i)))
			nDigits++;
	}

	if (nDigits >= 3) // It is likely to be a unique name (unlike CLIP.BMP or CLIP.WMF)
		strFile += strFileName;
	else
	{
		const CString* pstrPath = pObj->GetOriginalDataPath();
		CString strGrafName = (pstrPath ? *pstrPath : "~Missing");
		int iStart = strGrafName.ReverseFind('\\');
		if (iStart > 0)
			iStart = strGrafName.Left(iStart).ReverseFind('\\');
		strGrafName = strGrafName.Mid(iStart+1);
		strGrafName.Replace('\\', '_');
		strGrafName.Replace(":", "");
		strGrafName.Replace("[", "");
		strGrafName.Replace("]", "");
		strFile += strGrafName;
	}

	CString strExportFileName = strProjectPath + strFile;
	return (pObj->ExportRawData(&strExportFileName));
}

/////////////////////////////////////////////////////////////////////////////
static bool FindImage(const CString& strProjectPath, CString& strFile)
{
	CString strFileName = strFile.Mid(strFile.ReverseFind('\\')+1);
	CString strFileExtension =  strFile.Mid(strFile.ReverseFind('.')+1);
	strFileExtension.MakeLower();

	if (strFileExtension == "jpg")
		strFile = CString("..\\jpg\\") + strFileName;
	else
	if (strFileExtension == "png")
		strFile = CString("..\\png\\") + strFileName;
	else
	if (strFileExtension == "bmp")
		strFile = CString("..\\bmp\\") + strFileName;
	else
	if (strFileExtension == "wmf")
		strFile = CString("..\\wmf\\") + strFileName;
	else
	if (strFileExtension == "gif")
		strFile = CString("..\\gif\\") + strFileName;
	else
		strFile = CString("..\\zzz\\") + strFileName;

	CString strFileSaved;
	for (int i = 0; i <= 5; i++)
	{
		CString strFilePath = strProjectPath + strFile;

		CFileStatus tmpStatus;
		bool bFileFound = !!CFile::GetStatus(strFilePath, tmpStatus);
		if (bFileFound)
			return true;

		// Lookup failed; dig around for the file
		if (i == 0)
		{
			strFileSaved = strFile;
			strFileName = strFileName.Left(strFileName.ReverseFind('.')+1) + CString("jpg");
			strFile = CString("..\\jpg\\") + strFileName;
		}
		else
		if (i == 1)
		{
			strFileName = strFileName.Left(strFileName.ReverseFind('.')+1) + CString("png");
			strFile = CString("..\\png\\") + strFileName;
		}
		else
		if (i == 2)
		{
			strFileName = strFileName.Left(strFileName.ReverseFind('.')+1) + CString("bmp");
			strFile = CString("..\\bmp\\") + strFileName;
		}
		else
		if (i == 3)
		{
			strFileName = strFileName.Left(strFileName.ReverseFind('.')+1) + CString("wmf");
			strFile = CString("..\\wmf\\") + strFileName;
		}
		else
		if (i == 4)
		{
			strFileName = strFileName.Left(strFileName.ReverseFind('.')+1) + CString("gif");
			strFile = CString("..\\gif\\") + strFileName;
		}
	}

	strFile = strFileSaved;
	return false;
}

/////////////////////////////////////////////////////////////////////////////
static bool FindImageMisc(CString& strFile)
{
	CString strFilePath = CString("D:\\Misc\\") + strFile.Mid(strFile.ReverseFind('\\')+1);

	CFileStatus tmpStatus;
	bool bFileFound = !!CFile::GetStatus(strFilePath, tmpStatus);
	if (bFileFound)
	{
		strFile = strFilePath;
		return true;
	}

	CString strMessage;
	strMessage.Format("Can't find MISC image '%s' - One more try...", strFile);
	AfxMessageBox(strMessage, MB_OK);

	bFileFound = !!CFile::GetStatus(strFilePath, tmpStatus);
	if (bFileFound)
	{
		strFile = strFilePath;
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
static CString Fixed2Str(CFixed& fixValue)
{
	// Truncate to 3 decimal places
	double fTemp = MakeDouble(fixValue) * 1000.0;
	fTemp = (fTemp >= 0 ? fTemp + 0.5 : fTemp - 0.5);
	int iValue = (int)fTemp;
	double fValue = (double)iValue / 1000.0;
	CString strOutput;
	strOutput.Format("%.10g", fValue);
	return strOutput;
}

/////////////////////////////////////////////////////////////////////////////
static CString Coord2Str(int iValue)
{
	// Divide by the conversion factor (1800), and truncate to 3 decimal places
//j	double fValue = (double)::MulDiv(iValue, 100, 18) / 1000.0;
	double fTemp = (double)iValue / 1.8; //j 1.7999;
	fTemp = (fTemp >= 0 ? fTemp + 0.5 : fTemp - 0.5);
	iValue = (int)fTemp;
	double fValue = (double)iValue / 1000.0;
	CString strOutput;
	strOutput.Format("%.10g", fValue);
	return strOutput;
}

/////////////////////////////////////////////////////////////////////////////
static CString Color2Str(COLOR Color)
{
	int black = (int)BLACK_COMPONENT_OF(Color);
	int base = 255 - black;
	int r = base - (int)CYAN_COMPONENT_OF(Color);
	int g = base - (int)MAGENTA_COMPONENT_OF(Color);
	int b = base - (int)YELLOW_COMPONENT_OF(Color);
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;

	CString strOutput;
	strOutput.Format("%d,%d,%d", r, g, b);
	return strOutput;
}

/////////////////////////////////////////////////////////////////////////////
static void AddChar(CHARACTER c, CString& strText)
{
	if (c == '\t' || c == TAB_CHARACTER)
		strText += "      ";
	else
	if (c == '\n' || c == BREAK_CHARACTER)
		strText += "\\n";
	else
	if (c == '\r')
		strText += "\\r";
	else
	if (c == '&')
		strText += "&amp;";
	else
	if (c == '<')
		strText += "&lt;";
	else
	if (c == '>')
		strText += "&gt;";
	else
	if (c == '"')
		strText += "&quot;";
	else
		strText += (char)c;
}

/////////////////////////////////////////////////////////////////////////////
class CAttributes {
public:
	ALIGN_TYPE nAlignment;
	CString strFace;
	int nSize;
	COLOR ColorText;
	bool bBold;
	bool bItalic;
	bool bUnderline;
};

/////////////////////////////////////////////////////////////////////////////
CExtractProjectText::CExtractProjectText(BOOL bAll /* = TRUE*/)
{
}

/////////////////////////////////////////////////////////////////////////////
CExtractProjectText::~CExtractProjectText()
{
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::WritePathProperties(CStdioFile& cfOutput, RPath* pPath, int x, int y, int dx, int dy)
{
	// Common code to walk the path
	RIntRect rect = pPath->GetBoundingRect();
	if (!rect.Width() || !rect.Height())
		return;

	YScaleFactor sx = (double)dx / rect.Width();
	YScaleFactor sy = (double)dy / rect.Height();

	R2dTransform transform;
	transform.PreTranslate(x - rect.m_Left, y - rect.m_Top);
	transform.PreScale(sx, sy);

	RIntPoint* pPoints = NULL;
	YPointCountArray countArray;
	pPath->CalcPath(transform, pPoints, countArray);
	
	if (pPoints)
	{
		// iterate through the collection of points
		RIntPoint* p = pPoints;

		YPointCountArray::YIterator iterator = countArray.Start();
		for ( ; iterator != countArray.End(); ++iterator)
		{	
			int lastx = 0;
			int lasty = 0;
			// Add the points
			for(int i = 0; i < *iterator; ++i)
			{
				// convert from component units to app units, could do in a transform
				int newx = p->m_x;
				int newy = p->m_y;

				CString strOutput;
				if (!i || !(newx == lastx && newy == lasty)) // Eliminate duplicate points
				{
					strOutput.Format("\t\t\t<pt x='%s' y='%s' />\n", Coord2Str(newx), Coord2Str(newy));
					cfOutput.WriteString(strOutput);

					lastx = newx;
					lasty = newy;
				}

				p++;
			}
		}

		// Clean up
		::ReleaseGlobalBuffer(reinterpret_cast<uBYTE*>(pPoints));
	}
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::WriteTextProperties(CStdioFile& cfOutput, CFrameObject* pObj)
{
	if (!pObj)
		return;

	PMGDatabase* pDatabase = pObj->get_database();
	if (!pDatabase)
		return;
	PMGFontServer* pFontServer = (PMGFontServer*)(pDatabase->get_font_server());
	if (!pFontServer)
		return;

	ERRORCODE errorcode;
	CFrameRecord* pFrameRecord = pObj->LockFrameRecord(&errorcode);
	if (!pFrameRecord || errorcode != ERRORCODE_None)
		return;

	// Write out the vertical alignment
	int VAlign = pFrameRecord->VerticalAlignment();
	if (VAlign == ALIGN_top)
		cfOutput.WriteString(" valign='Top'");
	else
	if (VAlign == ALIGN_middle)
		cfOutput.WriteString(" valign='Middle'");
	else
	if (VAlign == ALIGN_bottom)
		cfOutput.WriteString(" valign='Bottom'");
	cfOutput.WriteString(">\n");

	// Check for an empty frame
	CHARACTER_COUNT lCount = pFrameRecord->NumberOfCharacters();
	if (lCount <= 0)
	{
		pFrameRecord->release();
		return;
	}

	CTextRecord* pTextRecord = pObj->LockTextRecord(&errorcode);
	if (!pTextRecord || errorcode != ERRORCODE_None)
	{
		pFrameRecord->release();
		return;
	}

	// Initialize the Last set of attributes
	CAttributes Last;
	Last.nAlignment	 = (ALIGN_TYPE)-1; // Invalid alignment
	Last.strFace	 = "";
	Last.nSize		 = 0;
	Last.ColorText	 = UNDEFINED_COLOR;
	Last.bBold		 = false;
	Last.bItalic	 = false;
	Last.bUnderline	 = false;

	// Initialize the output buffer
	CString strOutput;
	bool bOutputOpen = false;

	// Loop through style changes and generate the output
	CTextIterator textIterator;
	textIterator.SetRecord(pTextRecord);
	textIterator.Style().Frame(pObj);

	CHARACTER_INDEX lFirstChar = pFrameRecord->FirstCharacter();
	CHARACTER_INDEX lLastChar = lFirstChar + lCount - 1;
	CHARACTER_INDEX lChar = lFirstChar;

	while (lChar >= 0 && lChar <= lLastChar)
	{
		textIterator.SetPosition(lChar);
		CTextStyle textStyle = textIterator.Style();

		// Get the location of the next style change
		CHARACTER_INDEX lCharStyleEnd = textIterator.NextChangeStart() - 1;
		if (lCharStyleEnd < 0)
			lCharStyleEnd = lLastChar;
		if (lCharStyleEnd > lLastChar)
			lCharStyleEnd = lLastChar;

		// Get all of the characters with this style
		CHARACTER_COUNT lCharactersAvail = 0;
		CHARACTER* pCharacter = NULL;

		// Loop through all the characters up to the next style change
		CString strText;
		while (lChar <= lCharStyleEnd)
		{
			textIterator.SetPosition(lChar);
			if (!lCharactersAvail)
				pCharacter = pTextRecord->GetCharacters(lChar, &lCharactersAvail);

			lChar++;

			if (!pCharacter || lCharactersAvail <= 0)
				break;

			CHARACTER c = *pCharacter++;
			lCharactersAvail--;

			if (c < MACRO_CHARACTER)
				AddChar(c, strText);
			else
			{ // Have macro server handy if there are fill-in fields in character stream
				CMacroServer *pMacroServer = pDatabase->GetMacroServer();
				if (pMacroServer)
				{
					// Get the macro text.
					CHARACTER* pMacroText = pMacroServer->GetMacroText(c);
					if (pMacroText)
					{
						for (CHARACTER* p = pMacroText; *p != 0; p++)
							AddChar(*p, strText);
					}
				}
			}
		}

		// Get the text style's attributes
		CAttributes Current;

		// Get Alignment
		Current.nAlignment = (ALIGN_TYPE)textStyle.m_Paragraph.m_nAlignment;

		// Get Face
		Current.strFace.Empty();
		int nFace = pFontServer->font_record_to_face(textStyle.Font());
		if (nFace != -1)
		{
			FaceEntry* pEntry = typeface_server.get_face(nFace);
			if (pEntry)
				Current.strFace = pEntry->get_name();
		}

		// Get Size
		Current.nSize = FixedInteger(textStyle.Size());
		if (textStyle.Size() == UNDEFINED_CFIXED)
			Current.nSize = 12;

		// Get Color
		Current.ColorText = textStyle.Fill().m_ForegroundColor;
		if (Current.ColorText == UNDEFINED_COLOR)
			Current.ColorText = COLOR_WHITE;

		// Get Bold
		Current.bBold = !!textStyle.Bold();

		// Get Italic
		Current.bItalic = !!textStyle.Italic();

		// Get Underline
		Current.bUnderline = !!textStyle.Underline();

		// Collect any changed attributes in a string
		CString strChanges;
		CString strAttribute;

		// Collect any Alignment change
		if (Current.nAlignment != Last.nAlignment)
		{
			switch(Current.nAlignment)
			{
				case ALIGN_left:
				default:
					strChanges += " align='Left'";
					break;
				case ALIGN_center:
					strChanges += " align='Center'";
					break;
				case ALIGN_right:
					strChanges += " align='Right'";
					break;
				case ALIGN_justify_left:
					strChanges += " align='Justify'";
					break;
			}
		}

		// Collect any Font changes
		if (Current.strFace != Last.strFace)
		{
			strAttribute.Format(" face='%s'", Current.strFace);
			strChanges += strAttribute;
		}
	  
		// Collect any Size changes
		if (Current.nSize != Last.nSize)
		{
			strAttribute.Format(" size='%d'", Current.nSize);
			strChanges += strAttribute;
		}

		// Collect any Color changes
		if (Current.ColorText != Last.ColorText)
		{
			strAttribute.Format(" color='%s'", Color2Str(Current.ColorText));
			strChanges += strAttribute;
		}

		// Collect any Bold changes
		if (Current.bBold != Last.bBold)
		{
			strAttribute.Format(" b='%s'", (Current.bBold ? "on" : "off"));
			strChanges += strAttribute;
		}

		// Collect any Italic changes
		if (Current.bItalic != Last.bItalic)
		{
			strAttribute.Format(" i='%s'", (Current.bItalic ? "on" : "off"));
			strChanges += strAttribute;
		}

		// Collect any Underline changes
		if (Current.bUnderline != Last.bUnderline)
		{
			strAttribute.Format(" u='%s'", (Current.bUnderline ? "on" : "off"));
			strChanges += strAttribute;
		}

		// Update the Last set of attributes
		Last.nAlignment	 = Current.nAlignment;
		Last.strFace	 = Current.strFace;
		Last.nSize		 = Current.nSize;
		Last.ColorText	 = Current.ColorText;
		Last.bBold		 = Current.bBold;
		Last.bItalic	 = Current.bItalic;
		Last.bUnderline	 = Current.bUnderline;

		// If any attributes have changed, output the previous style tag, and start a new one
		if (!strChanges.IsEmpty())
		{
			if (bOutputOpen)
			{
				strOutput += "\" />\n";
				cfOutput.WriteString(strOutput);
				bOutputOpen = false;
			}

			strOutput = "\t\t\t<style";
			strOutput += strChanges;
			strOutput += " t=\"";
			bOutputOpen = true;
		}

		strOutput += strText;
	}

	if (bOutputOpen)
	{
		strOutput += "\" />\n";
		cfOutput.WriteString(strOutput);
	}

	pFrameRecord->release();
	pTextRecord->release();
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::WriteObjectProperties(CStdioFile& cfOutput, LPCSTR strType, int x, int y, int dx, int dy, float fAngle)
{
	CString strAttribute;
	strAttribute.Format("\t\t<%s x='%s' y='%s' dx='%s' dy='%s'",
		strType, Coord2Str(x), Coord2Str(y), Coord2Str(dx), Coord2Str(dy));
	cfOutput.WriteString(strAttribute);

	if (fAngle)
	{
		strAttribute.Format(" angle='%.1f'", fAngle);
		cfOutput.WriteString(strAttribute);
	}

	cfOutput.Flush();
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::WriteDrawObjectProperties(DrawingObject* pObject, bool bFill, CStdioFile& cfOutput)
{
	FillFormatV2 fill = pObject->GetFill();
	OutlineFormat line = pObject->GetOutline();
	ShadowFormat shadow = pObject->GetShadow();

	DoWriteDrawProperties(fill, line, shadow, bFill, cfOutput);
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::WriteDrawProperties(CObjectProperties& Properties, bool bFill, CStdioFile& cfOutput)
{
	FillFormatV2 fill = Properties.m_Fill;
	OutlineFormat line = Properties.m_Outline;
	ShadowFormat shadow = Properties.m_Shadow;

	// This is a hack to find out if the fill type is undefined
	short nFillType = 0;
	if (!Properties.ApplyFillType(nFillType))
		fill.m_nFillType = FillFormatV2::FillSolid;

	DoWriteDrawProperties(fill, line, shadow, bFill, cfOutput);
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::DoWriteDrawProperties(FillFormatV2& fill, OutlineFormat& line, ShadowFormat& shadow, bool bFill, CStdioFile& cfOutput)
{
	if (!fill.IsVisible())
		fill.m_nFillType = FillFormatV2::FillNone;
	if (!line.IsVisible())
		line.m_nStyle = OutlineFormat::None;
	if (!shadow.IsVisible())
		shadow.m_nStyle = ShadowFormat::None;

	CString strProperties;

	// Handle Outline Properties
	if (line.m_nStyle != OutlineFormat::None)
	{
		CString strLineType;
		switch (line.m_nStyle)
		{
			case OutlineFormat::Light:	strLineType = "Hairline"; break;
			default:					strLineType = "Normal"; break;
		}

		strProperties.Format(" linetype='%s'", strLineType);
		cfOutput.WriteString(strProperties);

		strProperties.Format(" linecolor='%s'", Color2Str(line.m_ForegroundColor));
		cfOutput.WriteString(strProperties);

		if (line.m_nStyle != OutlineFormat::Light) // if not hairline...
		{
			strProperties.Format(" linewidth='%s'", Fixed2Str(line.m_lWidth)); // Percent or points
			cfOutput.WriteString(strProperties);
		}
	}

	// Handle Fill Properties
	if (bFill && fill.m_nFillType != FillFormatV2::FillNone)
	{
		CString strFillType;
		switch (fill.m_nFillType)
		{
			case FillFormatV2::FillSolid:			strFillType = "Solid"; break;
			case FillFormatV2::FillSweepRight:		strFillType = "SweepRight"; break;
			case FillFormatV2::FillSweepDown:		strFillType = "SweepDown"; break;
			case FillFormatV2::FillRadialCenter:	strFillType = "RadialCenter"; break;
			case FillFormatV2::FillRadialCorner:	strFillType = "RadialCorner"; break;
			default:								strFillType = "Solid"; break;
		}

		strProperties.Format(" filltype='%s'", strFillType);
		cfOutput.WriteString(strProperties);

		strProperties.Format(" fillcolor='%s'", Color2Str(fill.m_ForegroundColor));
		cfOutput.WriteString(strProperties);

		if (fill.m_nFillType != FillFormatV2::FillSolid)
		{
			strProperties.Format(" fillblendcolor='%s'", Color2Str(fill.m_GradientBlendColor));
			cfOutput.WriteString(strProperties);
		}
	}

	// Handle Shadow properties
	if (shadow.m_nStyle != ShadowFormat::None)
	{
		CString strShadowType;
		switch (shadow.m_nStyle)
		{
			case ShadowFormat::Drop:		strShadowType = "Drop"; break;
			case ShadowFormat::Extruded:	strShadowType = "Extrude"; break;
			default:						strShadowType = "Drop"; break;
		}

		strProperties.Format(" shadtype='%s' shadcolor='%s' shadx='%s' shady='%s'",
			strShadowType,
			Color2Str(shadow.m_ForegroundColor),
			Fixed2Str(shadow.m_lXOffset), // Points
			Fixed2Str(shadow.m_lYOffset)); // Points
		cfOutput.WriteString(strProperties);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::ExamineObject(const CString& strProjectPath, PMGPageObject* pObject, POINT ptOffset, CStdioFile& cfOutput)
{
	if (!pObject)
		return;

	DB_OBJECT_TYPE iType = pObject->type();

	double fAngle = pObject->get_rotation() * RAD2DEGC;
	FLAGS flags = pObject->get_flags();
	bool bFlipX = !!(flags & OBJECT_FLAG_xflipped);
	bool bFlipY = !!(flags & OBJECT_FLAG_yflipped);

	PBOX Bound = ((RectPageObject*)pObject)->get_unrotated_bound();
	int dx = (int)(Bound.x1 - Bound.x0);
	int dy = (int)(Bound.y1 - Bound.y0);
	int x = (int)(Bound.x0 - ptOffset.x);
	int y = (int)(Bound.y0 - ptOffset.y);

	PMGDatabase* pDatabase = pObject->get_database();
	switch (iType)
	{
		case OBJECT_TYPE_Frame: // is a RectPageObject
		{
			CFrameObject* pObj = (CFrameObject*)pObject;
			WriteObjectProperties(cfOutput, "text", x, y, dx, dy, fAngle);
//j			cfOutput.WriteString(">\n");

			WriteTextProperties(cfOutput, pObj);
			cfOutput.WriteString("\t\t</text>\n");
			break;
		}
		case OBJECT_TYPE_CalendarDate: // is a CFrameObject/RectPageObject
		{
			CalendarDateObject* pObj = (CalendarDateObject*)pObject;
			WriteObjectProperties(cfOutput, "date", x, y, dx, dy, fAngle);
//j			cfOutput.WriteString(">\n");

			WriteTextProperties(cfOutput, pObj);
			cfOutput.WriteString("\t\t</date>\n");
			break;
		}
		case OBJECT_TYPE_Graphic: // is a RectPageObject
		{
			GraphicObject* pObj = (GraphicObject*)pObject;
			WriteObjectProperties(cfOutput, "image", x, y, dx, dy, fAngle);

			// May be a cropped image
			CDoubleRect& rect = pObj->GetVisibleRect();
			double x = rect.Width();
			double y = rect.Height();
			if (rect.left != 0.0 || rect.top != 0.0 || rect.right != 1.0 || rect.bottom != 1.0)
				int k = 0;

			const CString* pstrPath = pObj->GetOriginalDataPath();
			CString strPath = *pstrPath;
			strPath.TrimLeft();
			strPath.TrimRight();
			bool bFound = FindImage(strProjectPath, strPath);
			if (!bFound)
			{
				CString strFileName = strPath.Mid(strPath.ReverseFind('\\')+1);
				if (ExtractEmbedded(pObj, strProjectPath, strFileName))
				{
					strPath = strFileName;
					bFound = true;
				}

				if (!bFound && !FindImageMisc(strPath))
				{
					CString strMessage;
					strMessage.Format("Can't find image '%s'", strPath);
					AfxMessageBox(strMessage, MB_OK);
					strPath += "~Missing";
				}
			}

			cfOutput.WriteString(" file=\"" + strPath + "\" />\n");
			break;
		}
		case OBJECT_TYPE_Line: // is a DrawingObject/RectPageObject
		{
			LineObject* pObj = (LineObject*)pObject;
			WriteObjectProperties(cfOutput, "line", x, y, dx, dy, fAngle);
			WriteDrawObjectProperties(pObj, false/*bFill*/, cfOutput);

			CString strStart = "UpperLeft";
			if (bFlipX && bFlipY)
				strStart = "LowerRight";
			else
			if (bFlipX)
				strStart = "UpperRight";
			else
			if (bFlipY)
				strStart = "LowerLeft";

			CString strOutput;
			strOutput.Format(" start='%s'", strStart);
			cfOutput.WriteString(strOutput);
			cfOutput.WriteString(" />\n");
			break;
		}
		case OBJECT_TYPE_Ellipse: // is a DrawingObject/RectPageObject
		{
			EllipseObject* pObj = (EllipseObject*)pObject;
			WriteObjectProperties(cfOutput, "ellipse", x, y, dx, dy, fAngle);
			WriteDrawObjectProperties((DrawingObject*)pObj, true/*bFill*/, cfOutput);
			cfOutput.WriteString(" />\n");
			break;
		}
		case OBJECT_TYPE_Rectangle: // is a DrawingObject/RectPageObject
		{
			RectangleObject* pObj = (RectangleObject*)pObject;
			WriteObjectProperties(cfOutput, "rectangle", x, y, dx, dy, fAngle);
			WriteDrawObjectProperties(pObj, true/*bFill*/, cfOutput);
			cfOutput.WriteString(" />\n");
			break;
		}
		case OBJECT_TYPE_Pattern: // is a RectPageObject
		{
			PatternObject* pObj = (PatternObject*)pObject;
			WriteObjectProperties(cfOutput, "rectangle", x, y, dx, dy, fAngle);
			CString strOutput;
			strOutput.Format(" filltype='Solid' fillcolor='%s'", Color2Str(pObj->get_bcolor()));
			// For now, ignore the pObj->get_pattern() and pObj->get_color()
			cfOutput.WriteString(strOutput);
			cfOutput.WriteString(" />\n");
			break;
		}
		case OBJECT_TYPE_Hyperlink: // is a DrawingObject/RectPageObject
		{
			// Same as a RectangleObject
			RectangleObject* pObj = (RectangleObject*)pObject;
			WriteObjectProperties(cfOutput, "hyperlink", x, y, dx, dy, fAngle);
			WriteDrawObjectProperties(pObj, true/*bFill*/, cfOutput);
			cfOutput.WriteString(" />\n");
			break;
		}
		case OBJECT_TYPE_Border: // is a RectPageObject
		{
			BorderObject* pObj = (BorderObject*)pObject;
			WriteObjectProperties(cfOutput, "border", x, y, dx, dy, fAngle);
			CObjectProperties Properties;
			ZeroMemory(&Properties.m_Fill, sizeof(Properties.m_Fill));
			ZeroMemory(&Properties.m_Outline, sizeof(Properties.m_Outline));
			ZeroMemory(&Properties.m_Shadow, sizeof(Properties.m_Shadow));
			pObj->GetObjectProperties(Properties);
			WriteDrawProperties(Properties, true/*bFill*/, cfOutput);
			cfOutput.WriteString(" />\n");
			break;
		}
		case OBJECT_TYPE_Table: // is a GroupObject/RectPageObject
		{
			CTableObject* pObj1 = (CTableObject*)pObject;
			WriteObjectProperties(cfOutput, "table", x, y, dx, dy, fAngle);
			cfOutput.WriteString(">\n");

			// Same as a Group
			GroupObject* pGroupObject = (GroupObject*)pObject;
			ObjectList* pObjectList = pGroupObject->object_list();

			// Run through each object.
			PMGPageObject* pObj = (PMGPageObject*)pObjectList->first_object();
			while (pObj)
			{
				ExamineObject(strProjectPath, pObj, ptOffset, cfOutput);
				pObj = (PMGPageObject*)pObj->next_object();
			}

			cfOutput.WriteString("\t\t</table>\n");
			break;
		}
		case OBJECT_TYPE_Cell: // is a GroupObject/RectPageObject
		{
			CCellObject* pObj1 = (CCellObject*)pObject;
			WriteObjectProperties(cfOutput, "cell", x, y, dx, dy, fAngle);
			cfOutput.WriteString(">\n");

			// Same as a Group
			GroupObject* pGroupObject = (GroupObject*)pObject;
			ObjectList* pObjectList = pGroupObject->object_list();

			// Run through each object.
			PMGPageObject* pObj = (PMGPageObject*)pObjectList->first_object();
			while (pObj)
			{
				ExamineObject(strProjectPath, pObj, ptOffset, cfOutput);
				pObj = (PMGPageObject*)pObj->next_object();
			}

			cfOutput.WriteString("\t\t</cell>\n");
			break;
		}
		case OBJECT_TYPE_DrawingObject: // is a RectPageObject
		{
			DrawingObject* pObj = (DrawingObject*)pObject;

			COutlinePath Path;
			CRect crObject( 0, 0, kSystemDPI * 2, kSystemDPI * 2 );
			if (!pObj->GetPath(Path, crObject))
				break;

			RPath TheRPath;
			R2dTransform TheTransform;
			bool bOK = !!Path.GetRPath(TheRPath, TheTransform);
			if (!bOK)
				break;
			RPath* pPath = &TheRPath;

			WriteObjectProperties(cfOutput, "drawing", x, y, dx, dy, fAngle);
			WriteDrawObjectProperties(pObj, true/*bFill*/, cfOutput);
			cfOutput.WriteString(">\n");

			WritePathProperties(cfOutput, pPath, x, y, dx, dy);

			cfOutput.WriteString("\t\t</drawing>\n");
			break;
		}
		case OBJECT_TYPE_Component: // is a RectPageObject
		{
			ComponentObject* pObj = (ComponentObject*)pObject;
			RComponentView* pComponentView = pObj->GetComponentView();
			if (!pComponentView)
				break;

			RImageInterface* pImageInterface = NULL;
			RGraphicInterface* pGraphicInterface = NULL;
			IPathInterface* pPathInterface = NULL;

			if (pImageInterface = dynamic_cast<RImageInterface*>(pComponentView->GetInterface(kImageInterfaceId)))
			{
				delete pImageInterface;
				WriteObjectProperties(cfOutput, "image", x, y, dx, dy, fAngle);

				CSize SizeImage;
				pObj->GetRawImageSize(&SizeImage);

				const CString* pstrPath = pObj->GetOriginalDataPath();
				CString strPath = *pstrPath;
				strPath.TrimLeft();
				strPath.TrimRight();
				bool bFound = FindImage(strProjectPath, strPath);
				if (!bFound)
				{
					CString strFileName = strPath.Mid(strPath.ReverseFind('\\')+1);
					if (ExtractEmbedded(pObj, strProjectPath, strFileName))
					{
						strPath = strFileName;
						bFound = true;
					}

					if (!bFound && !FindImageMisc(strPath))
					{
						CString strMessage;
						strMessage.Format("Can't find component '%s'", strPath);
						AfxMessageBox(strMessage, MB_OK);
						strPath += "~Missing";
					}
				}

				cfOutput.WriteString(" file=\"" + strPath + "\" />\n");
			}
			else
			if (pGraphicInterface = dynamic_cast<RGraphicInterface*>(pComponentView->GetInterface(kGraphicInterfaceId)))
			{
				delete pGraphicInterface;
				WriteObjectProperties(cfOutput, "graphic", x, y, dx, dy, fAngle);
				CObjectProperties Properties;
				ZeroMemory(&Properties.m_Fill, sizeof(Properties.m_Fill));
				ZeroMemory(&Properties.m_Outline, sizeof(Properties.m_Outline));
				ZeroMemory(&Properties.m_Shadow, sizeof(Properties.m_Shadow));
				pObj->GetObjectProperties(Properties);
				WriteDrawProperties(Properties, true/*bFill*/, cfOutput);
				cfOutput.WriteString(" />\n");
				AfxMessageBox("graphic!", MB_OK);
			}
			else
			if (pPathInterface = dynamic_cast<IPathInterface*>(pComponentView->GetInterface(kPathInterfaceId)))
			{
				int nPoints = 0;
				bool bBeziers = false;
				bool bSplines = false;
				int nOperators = 0;
				int nOutlines = 0;
				RRealPoint StartPoint;
				RRealPoint EndPoint;
				RPath* pPath = pPathInterface->GetPath();
				if (pPath)
				{
					nPoints = pPath->GetPointCount();
					bBeziers = !!pPath->UsesBeziers();
					bSplines = !!pPath->UsesSplines();
					nOperators = pPath->GetOperatorCount();
					nOutlines = pPath->GetOutlineCount();
					StartPoint = pPath->GetStartPoint();
					EndPoint = pPath->GetEndPoint();
				/*	bool bDefined = !!pPath->IsDefined();
					long l = pPath->GetLength();
					RIntSize s = pPath->GetPathSize();*/
				}

				bool bIsEllipse = (
					nPoints == 13 &&
					bBeziers);
				bool bIsRectangle = (
					nPoints == 5 &&
					!bBeziers &&
					!bSplines &&
					nOperators == 5 &&
					nOutlines == 1 &&
					!StartPoint.m_x &&
					!StartPoint.m_y &&
					!EndPoint.m_x &&
					!EndPoint.m_y);

				LPCSTR strType = (bIsRectangle ? "rectangle" : (bIsEllipse ? "ellipse" : "drawing"));
				WriteObjectProperties(cfOutput, strType, x, y, dx, dy, fAngle);
				CObjectProperties Properties;
				ZeroMemory(&Properties.m_Fill, sizeof(Properties.m_Fill));
				ZeroMemory(&Properties.m_Outline, sizeof(Properties.m_Outline));
				ZeroMemory(&Properties.m_Shadow, sizeof(Properties.m_Shadow));
				pObj->GetObjectProperties(Properties);
				WriteDrawProperties(Properties, true/*bFill*/, cfOutput);

				if (bIsRectangle || bIsEllipse)
					cfOutput.WriteString(" />\n");
				else
				{ // it's a drawing
					cfOutput.WriteString(">\n");

					WritePathProperties(cfOutput, pPath, x, y, dx, dy);

					cfOutput.WriteString("\t\t</");
					cfOutput.WriteString(strType);
					cfOutput.WriteString(">\n");
				}

				delete pPathInterface;
			}
			else
			{
				// An unknown component; probably a Headline, etc. 
				WriteObjectProperties(cfOutput, "component", x, y, dx, dy, fAngle);
				cfOutput.WriteString(" />\n");
			}

			break;
		}
		case OBJECT_TYPE_Group: // is a GroupObject/RectPageObject
		{
			GroupObject* pGroupObject = (GroupObject*)pObject;
			WriteObjectProperties(cfOutput, "group", x, y, dx, dy, fAngle);
			cfOutput.WriteString(">\n");

			ObjectList* pObjectList = pGroupObject->object_list();

			// Run through each object.
			PMGPageObject* pObj = (PMGPageObject*)pObjectList->first_object();
			while (pObj)
			{
				ExamineObject(strProjectPath, pObj, ptOffset, cfOutput);
				pObj = (PMGPageObject*)pObj->next_object();
			}

			cfOutput.WriteString("\t\t</group>\n");
			break;
		}
		case OBJECT_TYPE_Calendar: // is a GroupObject/RectPageObject
		{
			CalendarObject* pCalendarObject = (CalendarObject*)pObject;
			WriteObjectProperties(cfOutput, "calendar", x, y, dx, dy, fAngle);
			cfOutput.WriteString(">\n");

			ObjectList* pObjectList = pCalendarObject->object_list();

			// Run through each object.
			PMGPageObject* pObj = (PMGPageObject*)pObjectList->first_object();
			while (pObj)
			{
				ExamineObject(strProjectPath, pObj, ptOffset, cfOutput);
				pObj = (PMGPageObject*)pObj->next_object();
			}

			cfOutput.WriteString("\t\t</calendar>\n");
			break;
		}
		case OBJECT_TYPE_WarpText: // is a CFrameObject/RectPageObject
		{
			CFrameObject* pObj = (CFrameObject*)pObject;
			AfxMessageBox("warptext!", MB_OK);
			break;
		}
		case OBJECT_TYPE_Background: // is a GraphicObject/RectPageObject
		{
			BackgroundObject* pObj = (BackgroundObject*)pObject;
			AfxMessageBox("background!", MB_OK);
			break;
		}
		case OBJECT_TYPE_OleObject: // is a RectPageObject
		{
			OleObject* pObj = (OleObject*)pObject;
			AfxMessageBox("oleobject!", MB_OK);
			break;
		}
		case OBJECT_TYPE_Serif: // is a RectPageObject
		{
			// Same as a OleObject
			OleObject* pObj = (OleObject*)pObject;
			AfxMessageBox("serif!", MB_OK);
			break;
		}
		default:
		{
			AfxMessageBox("unknown!", MB_OK);
			break;
		}
	}

	cfOutput.Flush();
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::ExamineFile(const CString& strProjectPath, const CString& strFileName, CExtractProjectTextProgressDialog* m_pProgressDialog, CStdioFile& cfOutput)
{
	CString& strPathName = strProjectPath + strFileName;

	CString strMessage;

	if (!Util::FileExists(strPathName))
	{
		strMessage.Format("Project file '%s' does not exist", strPathName);
		AfxMessageBox(strMessage, MB_OK);
		return;
	}

	// Attempt to open the PrintMaster document.
	CPmwDoc* pDocument = GET_PMWAPP()->OpenHiddenDocument(strPathName);
	if (!pDocument)
	{
		strMessage.Format("Can't open project file '%s'", strPathName);
		AfxMessageBox(strMessage, MB_OK);
		return;
	}

	PMGDatabase* pDatabase = pDocument->get_database();
	if (!pDatabase)
	{
		strMessage.Format("Can't get database for project file '%s'", strPathName);
		AfxMessageBox(strMessage, MB_OK);
		pDocument->OnCloseDocument();
		return;
	}

	DocumentRecord* pDocumentRecord = pDocument->DocumentRecord();
	if (!pDocumentRecord)
	{
		strMessage.Format("Can't get document record for project file '%s'", strPathName);
		AfxMessageBox(strMessage, MB_OK);
		pDocument->OnCloseDocument();
		return;
	}

	PROJECT_TYPE ProjectType = pDocumentRecord->get_project_type();
	if (ProjectType == PROJECT_TYPE_BlankPage || PROJECT_TYPE_Generic)
		ProjectType = PROJECT_TYPE_Poster; // Blank pages don't really exist

	// Compute a project type from the extension
	// We will compare this later to the type stored with the project
	CString strFileExtension =  strPathName.Mid(strPathName.ReverseFind('.')+1);
	strFileExtension.MakeUpper();
	PROJECT_TYPE MyProjectType = ProjectType;
	if (strFileExtension == "SIG")
		MyProjectType = PROJECT_TYPE_Poster;
	else
	if (strFileExtension == "CAR")
		MyProjectType = PROJECT_TYPE_Card;
	else
	if (strFileExtension == "BAN")
		MyProjectType = PROJECT_TYPE_Banner;
	else
	if (strFileExtension == "CAL")
		MyProjectType = PROJECT_TYPE_Calendar;
	else
	if (strFileExtension == "LBL")
		MyProjectType = PROJECT_TYPE_Label;
	else
	if (strFileExtension == "ENV")
		MyProjectType = PROJECT_TYPE_Envelope;
	else
	if (strFileExtension == "BIZ")
		MyProjectType = PROJECT_TYPE_BusinessCard;
	else
	if (strFileExtension == "CER")
		MyProjectType = PROJECT_TYPE_Certificate;
	else
	if (strFileExtension == "NOT")
		MyProjectType = PROJECT_TYPE_NoteCard;
	else
	if (strFileExtension == "FAX")
		MyProjectType = PROJECT_TYPE_FaxCover;
	else
	if (strFileExtension == "LET")
		MyProjectType = PROJECT_TYPE_Stationery;
	else
	if (strFileExtension == "NWS")
		MyProjectType = PROJECT_TYPE_Newsletter;
	else
	if (strFileExtension == "BRO")
		MyProjectType = PROJECT_TYPE_Brochure;
	else
	if (strFileExtension == "HCR")
		MyProjectType = PROJECT_TYPE_HalfCard;
	else
	if (strFileExtension == "WEB")
		MyProjectType = PROJECT_TYPE_WebPub;
	else
	if (strFileExtension == "PCR")
		MyProjectType = PROJECT_TYPE_PostCard;
	else
	if (strFileExtension == "STI")
		MyProjectType = PROJECT_TYPE_Sticker;
	else
	if (strFileExtension == "TSH")
		MyProjectType = PROJECT_TYPE_TShirt;
	else
	if (strFileExtension == "CFT")
		MyProjectType = PROJECT_TYPE_Craft;
	else
	if (strFileExtension == "PHP")
		MyProjectType = PROJECT_TYPE_PhotoProjects;

	if (ProjectType != MyProjectType)
		ProjectType = MyProjectType;

	CString strType;
	switch (ProjectType)
	{
		case PROJECT_TYPE_Poster:			{strType = "Poster";		break;}
		case PROJECT_TYPE_Card:				{strType = "QuarterCard";	break;}
		case PROJECT_TYPE_Banner:			{strType = "Banner";		break;}
		case PROJECT_TYPE_Calendar:			{strType = "Calendar";		break;}
		case PROJECT_TYPE_Label:			{strType = "Label";			break;}
		case PROJECT_TYPE_Envelope:			{strType = "Envelope";		break;}
		case PROJECT_TYPE_BusinessCard:		{strType = "BusinessCard";	break;}
		case PROJECT_TYPE_Certificate:		{strType = "Certificate";	break;}
		case PROJECT_TYPE_NoteCard:			{strType = "NoteCard";		break;}
		case PROJECT_TYPE_FaxCover:			{strType = "FaxCover";		break;}
		case PROJECT_TYPE_Stationery:		{strType = "Stationery";	break;}
		case PROJECT_TYPE_Newsletter:		{strType = "Newsletter";	break;}
		case PROJECT_TYPE_Brochure:			{strType = "Brochure";		break;}
		case PROJECT_TYPE_HalfCard:			{strType = "HalfCard";		break;}
		case PROJECT_TYPE_WebPub:			{strType = "WebPub";		break;}
		case PROJECT_TYPE_PostCard:			{strType = "PostCard";		break;}
		case PROJECT_TYPE_Sticker:			{strType = "Sticker";		break;}
		case PROJECT_TYPE_TShirt:			{strType = "TShirt";		break;}
		case PROJECT_TYPE_Craft:			{strType = "Craft";			break;}
		case PROJECT_TYPE_PhotoProjects:	{strType = "PhotoProjects";	break;}
		default:							{strType = "Unknown";		break;}
	}

	// Get the margin info
	// Add it to the page size if the project
	// was designed to be used with pre-cut paper
	CPaperInfo* pPaperInfo = pDocument->GetPaperInfo();
	PBOX Margins = {0,0,0,0};
	bool bUseMargins = false;
	bool bPortraitMargins = false;
	if (pPaperInfo)
	{
		const CString& strPaperName = pPaperInfo->GetName();
		bUseMargins = !strPaperName.IsEmpty();
		if (bUseMargins)
		{
			Margins = pPaperInfo->GetPageMargins();
			PPNT Dims = pPaperInfo->GetProjectDims();
			bPortraitMargins = (Dims.x < Dims.y);
		}
	}

	// Get the number of pages
	int nPages = pDocument->NumberOfPages();
	bool bCardDoc = false;

	// Adjust the page count if the document is a card; loop on panels instead
	if (pDocument->IsKindOf(RUNTIME_CLASS(CCardDoc)))
	{
		int nPanels = pDocument->number_of_panels();
		nPages = nPanels;
		bCardDoc = true;
	}

	// Set from the first page dimensions
	bool bPortrait = false;

	// Loop throught the panels/pages and process all of its objects
	for (int i = 0; i < nPages; i++)
	{
		// Select in the correct page/panel
		if (bCardDoc)
			((CCardDoc*)pDocument)->set_panel(i);
		else
			pDocument->GotoPage(i);

		// Get the page rectangle
		PBOX PageRect = {0,0,0,0};
		pDocument->get_panel_world(&PageRect, (bCardDoc ? i : 0));

		// If this is the projects's first page, set the portrait flag
		if (!i)
			bPortrait = (PageRect.Width() < PageRect.Height());

		// Add margins to the page rect if necessary
		if (bUseMargins)
		{
			bool bFlipMargins = (bPortraitMargins != bPortrait);
			PageRect.x0 -= (bFlipMargins ? Margins.x0 : Margins.y0);
			PageRect.x1 += (bFlipMargins ? Margins.x1 : Margins.y1);
			PageRect.y0 -= (bFlipMargins ? Margins.y0 : Margins.x0);
			PageRect.y1 += (bFlipMargins ? Margins.y1 : Margins.x1);
		}

		// Adjust the page rects of various project types to what we want them to be
		FixPageRect(PageRect, (int)ProjectType, i, bPortrait);

		// If this is the projects's first page,
		// Output the project tag along with the page orientation
		if (!i)
		{
			CString strOrient = (bPortrait ? "Portrait" : "Landscape");
			CString strProject;
			strProject.Format("<project type='%s' orient='%s' file=\"%s\">\n", strType, strOrient, strPathName);
			cfOutput.WriteString(strProject);
		}

		// Output the page tag along with the page size
		CString strPage;
		strPage.Format("\t<page dx='%s' dy='%s'>\n", Coord2Str(PageRect.Width()), Coord2Str(PageRect.Height()));
		cfOutput.WriteString(strPage);

		ObjectList* pObjectList = pDocument->object_list();						

		// Run through each object on the page
		POINT ptPageOffset = {PageRect.x0, PageRect.y0};
		PMGPageObject* pObj = (PMGPageObject*)pObjectList->first_object();
		while (pObj)
		{
			ExamineObject(strProjectPath, pObj, ptPageOffset, cfOutput);
			pObj = (PMGPageObject*)pObj->next_object();
		}

		cfOutput.WriteString("\t</page>\n");
	}

#ifdef NOTUSED
	// Write out the macros
	CStdMacroServer* pMacroServer = (CStdMacroServer*)(pDatabase->GetMacroServer());
	if (pMacroServer)
	{
		CMacroList* pMacroList = &(pMacroServer->MacroList());
		if (pMacroList)
		{
			for (int nMacroIndex = 0; nMacroIndex < pMacroList->Macros(); nMacroIndex++)
			{
				// Get the macro.
				CMacro* pMacro = pMacroList->Macro(nMacroIndex);
				if (!pMacro)
					continue;

				// Get the field value.
				bool bString = (pMacro->MacroValueType() == CMacro::VALUE_TYPE_String);
				CString strValue = (bString ? pMacro->Value() : pMacroServer->GetMacroTextString(pMacro));

				// Only record empty values if the macro is a "User Macro."
				if (!strValue.IsEmpty() || (pMacro->MacroType() == CMacro::MACRO_TYPE_User))
				{
					// Collect value text.
					CString strItem;
					for (int nCharacter = 0; nCharacter < strValue.GetLength(); nCharacter++)
						AddChar(strValue[nCharacter], strItem);

					// Write out the name and value pair
					cfOutput.WriteString("\t<macro " + pMacro->Name() + "=" + strItem + " />\n");
					cfOutput.Flush();
				}
			}
		}
	}
#endif NOTUSED

#ifdef NOTUSED
	DB_RECORD_NUMBER nRecord = 0;

	int nCount = pDocumentRecord->NumberOfPages();
	for (i = 0; i < nCount; i++)
	{
		nRecord = pDocumentRecord->GetPage(i);
		PageRecord* p = (PageRecord*)(pDatabase->get_record(nRecord, NULL, RECORD_TYPE_Page));
		if (p)
		{
			p->release();
			p = NULL;
		}
	}
	nCount = pDocumentRecord->NumberOfMasterPages();
	for (i = 0; i < nCount; i++)
	{
		nRecord = pDocumentRecord->GetMasterPage(i);
		PageRecord* p = (PageRecord*)(pDatabase->get_record(nRecord, NULL, RECORD_TYPE_Page));
		if (p)
		{
			p->release();
			p = NULL;
		}
	}
	nCount = pDocumentRecord->NumberOfInstructionPages();
	for (i = 0; i < nCount; i++)
	{
		nRecord = pDocumentRecord->GetInstructionPage(i);
		PageRecord* p = (PageRecord*)(pDatabase->get_record(nRecord, NULL, RECORD_TYPE_Page));
		if (p)
		{
			p->release();
			p = NULL;
		}
	}
	nCount = pDocumentRecord->NumberOfFonts();
	for (i = 0; i < nCount; i++)
	{
		nRecord = pDocumentRecord->GetFont(i);
		FontDataRecord* p = (FontDataRecord*)(pDatabase->get_record(nRecord, NULL, RECORD_TYPE_FontData));
		if (p)
		{
			p->release();
			p = NULL;
		}
	}
/*	nCount = pDocumentRecord->NumberOfLayouts();
	for (i = 0; i < nCount; i++)
	{
		nRecord = pDocumentRecord->GetLayout(i);
		ObjectLayoutRecord* p = (ObjectLayoutRecord*)(pDatabase->get_record(nRecord, NULL, RECORD_TYPE_ObjectLayout));
		if (p)
		{
			p->release();
			p = NULL;
		}
	}*/
	nCount = pDocumentRecord->NumberOfTextStyles();
	for (i = 0; i < nCount; i++)
	{
		nRecord = pDocumentRecord->GetTextStyle(i);
		CTextStyleRecord* p = (CTextStyleRecord*)(pDatabase->get_record(nRecord, NULL, RECORD_TYPE_TextStyle));
		if (p)
		{
			p->release();
			p = NULL;
		}
	}
/*	nCount = pDocumentRecord->NumberOfOutlinePaths();
	for (i = 0; i < nCount; i++)
	{
		nRecord = pDocumentRecord->GetOutlinePath(i);
		OutlinePathRecord* p = (OutlinePathRecord*)(pDatabase->get_record(nRecord, NULL, RECORD_TYPE_OutlinePath));
		if (p)
		{
			p->release();
			p = NULL;
		}
	}*/
/*	nCount = pDocumentRecord->NumberOfWarpFields();
	for (i = 0; i < nCount; i++)
	{
		nRecord = pDocumentRecord->GetWarpField(i);
		WarpFieldRecord* p = (WarpFieldRecord*)(pDatabase->get_record(nRecord, NULL, RECORD_TYPE_WarpField));
		if (p)
		{
			p->release();
			p = NULL;
		}
	}*/

	//j nRecord = object->FrameRecord(i);
	FrameRecord* p = (FrameRecord*)(pDatabase->get_record(nRecord, NULL, RECORD_TYPE_Frame));
	if (p)
	{
		p->release();
		p = NULL;
	}
#endif NOTUSED

	cfOutput.WriteString("</project>\n");
	cfOutput.WriteString("\n");
	cfOutput.Flush();

	// Close the document.
	pDocument->OnCloseDocument();
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::FixPageRect(PBOX& PageRect, int ProjectType, int i, bool bPortrait)
{
	int dx = PageRect.Width();
	int dy = PageRect.Height();
	int growx = 0;
	int growy = 0;

	switch (ProjectType)
	{
		// Full sheet projects
		case PROJECT_TYPE_Banner:
		case PROJECT_TYPE_Brochure:
		case PROJECT_TYPE_Calendar:
		case PROJECT_TYPE_Certificate:
		case PROJECT_TYPE_Craft:
		case PROJECT_TYPE_FaxCover:
		case PROJECT_TYPE_Newsletter:
		case PROJECT_TYPE_Poster:
		case PROJECT_TYPE_PhotoProjects:
		case PROJECT_TYPE_Stationery:
		case PROJECT_TYPE_TShirt:
		{
			if (bPortrait)
			{
				growx = 15300/*8.5 inches*/ - dx;
				growy = 19800/*11.0 inches*/ - dy;
				if (ProjectType == PROJECT_TYPE_Banner)
					growy = 0;
			}
			else
			{
				growy = 15300/*8.5 inches*/ - dy;
				growx = 19800/*11.0 inches*/ - dx;
				if (ProjectType == PROJECT_TYPE_Banner)
					growx = 0;
			}

			// Trap any obscure cases
			if (growx < 0 || growy < 0)
				ASSERT(false);
			if (growx > 1800/*1.0 inches*/ || growy > 1800/*1.0 inches*/)
				ASSERT(false);

			// Pad on all 4 sides
			PageRect.x0 -= growx/2;
			PageRect.x1 += (growx+1)/2;
			PageRect.y0 -= growy/2;
			PageRect.y1 += (growy+1)/2;
			break;
		}

		// Quarter sheet projects
		case PROJECT_TYPE_Card:
		case PROJECT_TYPE_NoteCard:
		{
			// Trap any obscure cases
			if (i < 0 || i > 2)
				ASSERT(false);

			if (bPortrait)
			{
				if (i == 0 || i == 2)
				{
					growx = 7650/*4.25 inches*/ - dx;
					growy = 9900/*5.5 inches*/ - dy;
				}
				else
				{
					growx = 15300/*8.5 inches*/ - dx;
					growy = 9900/*5.5 inches*/ - dy;
				}
			}
			else
			{
				if (i == 0 || i == 2)
				{
					growy = 7650/*4.25 inches*/ - dy;
					growx = 9900/*5.5 inches*/ - dx;
				}
				else
				{
					growy = 15300/*8.5 inches*/ - dy;
					growx = 9900/*5.5 inches*/ - dx;
				}
			}

			// Trap any obscure cases
			if (growx < 0 || growy < 0)
				ASSERT(false);
			if (growx > 1800/*1.0 inches*/ || growy > 1800/*1.0 inches*/)
				ASSERT(false);

			// Pad on all 4 sides
			PageRect.x0 -= growx/2;
			PageRect.x1 += (growx+1)/2;
			PageRect.y0 -= growy/2;
			PageRect.y1 += (growy+1)/2;
			break;
		}

		// Half sheet projects
		case PROJECT_TYPE_HalfCard:
		{
			// Trap any obscure cases
			if (i < 0 || i > 2)
				ASSERT(false);

			if (bPortrait)
			{
				if (i == 0 || i == 2)
				{	growx = 9900/*5.5 inches*/ - dx;
					growy = 15300/*8.5 inches*/ - dy;
				}
				else
				{
					growx = 19800/*11.0 inches*/ - dx;
					growy = 15300/*8.5 inches*/ - dy;
				}

				// Trap any obscure cases
				if (growx < 0 || growy < 0)
					ASSERT(false);
				if (growx > 1800/*1.0 inches*/ || growy > 1800/*1.0 inches*/)
					ASSERT(false);

				if (i == 0)
				{ // Page 0: Pad on the right, top, and bottom
					PageRect.x0 -= 0;
					PageRect.x1 += growx;
					PageRect.y0 -= growy/2;
					PageRect.y1 += (growy+1)/2;
				}
				else
				if (i == 1)
				{ // Page 1: Pad on all 4 sides
					PageRect.x0 -= growx/2;
					PageRect.x1 += (growx+1)/2;
					PageRect.y0 -= growy/2;
					PageRect.y1 += (growy+1)/2;
				}
				else
				{ // Page 2: Pad on the left, top, and bottom
					PageRect.x0 -= growx;
					PageRect.x1 += 0;
					PageRect.y0 -= growy/2;
					PageRect.y1 += (growy+1)/2;
				}
			}
			else
			{
				if (i == 0 || i == 2)
				{
					growy = 9900/*5.5 inches*/ - dy;
					growx = 15300/*8.5 inches*/ - dx;
				}
				else
				{
					growy = 19800/*11.0 inches*/ - dy;
					growx = 15300/*8.5 inches*/ - dx;
				}

				// Trap any obscure cases
				if (growx < 0 || growy < 0)
					ASSERT(false);
				if (growx > 1800/*1.0 inches*/ || growy > 1800/*1.0 inches*/)
					ASSERT(false);

				if (i == 0)
				{ // Page 0: Pad on the left, right, and bottom
					PageRect.x0 -= growx/2;
					PageRect.x1 += (growx+1)/2;
					PageRect.y0 -= 0;
					PageRect.y1 += growy;
				}
				else
				if (i == 1)
				{ // Page 1: Pad on all 4 sides
					PageRect.x0 -= growx/2;
					PageRect.x1 += (growx+1)/2;
					PageRect.y0 -= growy/2;
					PageRect.y1 += (growy+1)/2;
				}
				else
				{ // Page 2: Pad on the left, right, and bottom
					PageRect.x0 -= growx/2;
					PageRect.x1 += (growx+1)/2;
					PageRect.y0 -= 0;
					PageRect.y1 += growy;
				}
			}

			break;
		}

		// Pre-cut paper projects
		case PROJECT_TYPE_BusinessCard:
		case PROJECT_TYPE_Envelope:
		case PROJECT_TYPE_Label:
		case PROJECT_TYPE_PostCard:
		case PROJECT_TYPE_Sticker:
		case PROJECT_TYPE_WebPub:
		default:
		{
			// Do nothing for now
			break;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectText::Run(void)
{
	// Get the name of the input file
	CFileDialog OpenDialog(true, "*.txt", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		"Text Files (*.txt)|*.txt|All Files (*.*)|*.*||", AfxGetMainWnd());

	if (OpenDialog.DoModal() != IDOK)
		return;

	CString strInput = OpenDialog.GetPathName();

	// Get the name of the output file
	CFileDialog SaveDlg(FALSE, "*.xml", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"XML Files (*.xml)|*.xml|All Files (*.*)|*.*||", AfxGetMainWnd());

	if (SaveDlg.DoModal() != IDOK)
		return;

	CString strOutput = SaveDlg.GetPathName();

	// Create a progress dialog.
	CExtractProjectTextProgressDialog* m_pProgressDialog = new CExtractProjectTextProgressDialog(IDD_EXTRACT_PROJECT_TEXT_PROGRESS_DIALOG, AfxGetMainWnd());

	// Open up the input file.
	CString strMessage;
	strMessage.Format("Input File: %s", (LPCSTR)strInput);
	m_pProgressDialog->AddMessage(strMessage);
	CStdioFile cfInput;
	if (!cfInput.Open(strInput, CFile::modeRead|CFile::shareDenyNone|CFile::typeText))
	{
		strMessage.Format("Can't open input file %s", (LPCSTR)strInput);
		m_pProgressDialog->AddMessage(strMessage);
		return;
	}

	// Create the output file.
	strMessage.Format("Output File: %s", (LPCSTR)strOutput);
	m_pProgressDialog->AddMessage(strMessage);
	CStdioFile cfOutput;
	if (!cfOutput.Open(strOutput, CFile::modeCreate|CFile::modeReadWrite|CFile::shareExclusive|CFile::typeBinary))
	{
		strMessage.Format("Can't create output file %s", (LPCSTR)strOutput);
		m_pProgressDialog->AddMessage(strMessage);
		return;
	}

	cfOutput.WriteString("<?xml version='1.0' encoding='iso-8859-1' ?>\n");
	cfOutput.WriteString("<xml>\n");

	// Process each line from the input file.
	CString strFileName;
	while (Util::ReadString(&cfInput, strFileName))
	{
		if (m_pProgressDialog->CheckForAbort())
		   break;

		if (strFileName.IsEmpty())
			continue;

		if (strFileName.GetAt(0) == '@')
			break;

#ifdef TEST
		if (strFileName.GetAt(0) != '#')
			continue;
		strFileName = strFileName.Mid(1);
#endif TEST

		if (strFileName.GetAt(0) == '#')
			continue;

		CString strMessage;
		strMessage.Format("Processing '%s'", (LPCSTR)strFileName);
		m_pProgressDialog->AddMessage(strMessage);

		// Build the full path name of the project.
		CString strFullPath = strInput.Left(strInput.ReverseFind('\\')+1) + strFileName;
		CString strProjectPath = strFullPath.Left(strFullPath.ReverseFind('\\')+1);
		strFileName = strFullPath.Mid(strFullPath.ReverseFind('\\')+1);
		ExamineFile(strProjectPath, strFileName, m_pProgressDialog, cfOutput);
	}

	cfOutput.WriteString("</xml>\n");

	strMessage.Format("Normal termination");
	m_pProgressDialog->AddMessage(strMessage);

	if (m_pProgressDialog)
	{
		m_pProgressDialog->WaitForDone();
		m_pProgressDialog->Finish();
		delete m_pProgressDialog;
		m_pProgressDialog = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CExtractProjectTextProgressDialog, CDialog)
   //{{AFX_MSG_MAP(CExtractProjectTextProgressDialog)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtractProjectTextProgressDialog dialog

CExtractProjectTextProgressDialog::CExtractProjectTextProgressDialog(int IDD, CWnd* pParent /*=NULL*/)
   : CDialog(IDD, pParent)
{
   //{{AFX_DATA_INIT(CInstallProgressDialog)
      // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT

   m_fIsAborted = FALSE;
   m_fIsDone = FALSE;
   m_pMessageList = NULL;
   m_pParent = pParent;
   
   m_pParent->EnableWindow(FALSE);
   
   Create(IDD, pParent);
}

/////////////////////////////////////////////////////////////////////////////
CExtractProjectTextProgressDialog::~CExtractProjectTextProgressDialog()
{
   Finish();
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectTextProgressDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CExtractProjectTextProgressDialog)
      // NOTE: the ClassWizard will add DDX and DDV calls here
   //}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectTextProgressDialog::Breathe(void)
{
   MSG Message;

   while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
   {
      if (!m_hWnd || !IsDialogMessage(&Message))
      {
         ::TranslateMessage(&Message);
         ::DispatchMessage(&Message);
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectTextProgressDialog::WaitForDone(void)
{
   CWnd* pOK = GetDlgItem(IDOK);
   if (pOK)
   {
      pOK->EnableWindow(TRUE);
      CWnd* pCancel = GetDlgItem(IDCANCEL);
      if (pCancel)
      {
         pCancel->EnableWindow(FALSE);
      }

      while (!m_fIsDone)
      {
         Breathe();
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectTextProgressDialog::Finish(void)
{
   if (m_hWnd)
   {
      m_pParent->EnableWindow(TRUE);
      DestroyWindow();
   }
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectTextProgressDialog::AddMessage(LPCSTR pszAction)
{
   if (m_pMessageList)
   {
      int nIndex = m_pMessageList->AddString(pszAction);
		if (nIndex != LB_ERR)
		{
			m_pMessageList->SetCurSel(nIndex);
		}

      Breathe();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CExtractProjectTextProgressDialog message handlers

void CExtractProjectTextProgressDialog::OnCancel()
{
   // Make sure the user really wants to cancel the installation.
   if (AfxMessageBox("Do you really want to stop extracting project text?", MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION) == IDYES)
   {
      m_fIsAborted = TRUE;
   }
}

/////////////////////////////////////////////////////////////////////////////
void CExtractProjectTextProgressDialog::OnOK()
{
   m_fIsDone = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CExtractProjectTextProgressDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   CenterWindow();

   m_pMessageList = (CListBox*)GetDlgItem(IDC_PROGRESS_MESSAGE_LIST);
   
   return TRUE;  // return TRUE  unless you set the focus to a control
}
