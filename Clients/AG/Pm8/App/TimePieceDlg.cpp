/*	$Header: /PM8/App/TimePieceDlg.cpp 1     3/03/99 6:12p Gbeddow $
//
//	Definition of the CPTimePieceDlg class.
//
//	This class provides a wrapper for the MFC dialog class.  This dialog allows
// for the specification of a timepiece graphic.
//	
//	(c)Copyright 1999 The Learning Company, Inc., all rights reserved
//
// $Log: /PM8/App/TimePieceDlg.cpp $
// 
// 1     3/03/99 6:12p Gbeddow
// 
// 2     2/01/99 5:38p Rgrenfel
// Removed Dependency on RenaissanceResource.h
// 
// 1     2/01/99 11:43a Rgrenfel
// New timepieces dialog for the creation of custom time pieces.
*/

#include	"StdAfx.h"

ASSERTNAME

#include "TimePieceDlg.h"

//#include "RenaissanceResource.h"
#include "Resource.h"
#include "ApplicationGlobals.h"
#include "ResourceManager.h"
#include "ComponentDocument.h"
#include "ComponentView.h"
#include "ComponentAttributes.h"
#include "ComponentTypes.h"

#include "DateTimeInterface.h"
#include "TimepiecePlacementInterface.h"
#include "HeadlineInterface.h"

const uBYTE	kHourMin =			0;
const uBYTE	kHourMax =			23;
const uBYTE	kHourDefault =		12;
const uBYTE	kMinuteMin =		0;
const uBYTE	kMinuteMax =		59;
const uBYTE kMinuteDefault =	0;

const RColor	kDefaultHeadlineColor(kRed);

const YZLayer	kGraphicZLayer =	0;
const YZLayer	kHeadlineZLayer =	1;

RTimepieceDialog::RTimepieceDialog(RStandaloneDocument* pParentDocument, RComponentDocument* pEditedComponent )
:	RGenericGroupedObjectDialog(pParentDocument, pEditedComponent, IDD_TIMEPIECE),
	m_rTime(kHourDefault, kMinuteDefault, TRUE),	
	m_pGraphicView(NULL),
	m_pHeadlineView(NULL),
	m_pDateTimeInterface(NULL),	
	m_pHeadlineInterface(NULL),
	m_rCollectionArray(RCollectionManager::TheCollectionManager().OpenCollectionsOfType(kCustomCollectionType)),
	m_rSearcher(m_rCollectionArray),
	m_uTimerID(kInvalidTimerID)
{
	m_rGraphicAttributes.SetSelectable(FALSE);
	m_rGraphicAttributes.SetMovable(FALSE);
	m_rGraphicAttributes.SetRotatable(FALSE);
	m_rGraphicAttributes.SetFlippable(FALSE);	
#if 0 // GCB 3/9/98 - PS6 timepieces are now shadowable
	m_rGraphicAttributes.SetShadowable(FALSE);		
#endif
	m_rGraphicAttributes.SetClipToParent(TRUE);	
	m_rGraphicAttributes.SetZLayer(kGraphicZLayer);

	m_rHeadlineAttributes = m_rGraphicAttributes;
	m_rHeadlineAttributes.SetZLayer(kHeadlineZLayer);

	// Load the default timepiece font
	m_rTimepieceFont = ::GetResourceManager().GetResourceString(IDS_TIMEPIECE_FONT_DEFAULT);

	//	Load the current time
	m_rTime	= RTime::CurrentTime( );
}

RTimepieceDialog::~RTimepieceDialog()
{
	delete m_pDateTimeInterface;
	delete m_pHeadlineInterface;
	RCollectionManager::TheCollectionManager().CloseCollectionsOfType(kCustomCollectionType);
}

void RTimepieceDialog::UpdateTime()
{
	// Updates the timepiece based on the m_rTime member
	if (m_pDateTimeInterface)
	{
		// Set the time in the graphic component				
		m_pDateTimeInterface->SetDateTime(RDateTime(RDate(), m_rTime), FALSE);
		
		// Update the view
		m_pGraphicView->InvalidateRenderCache();
		m_pGraphicView->Invalidate();	
	}
	else
	if (m_pHeadlineInterface)
	{
		// Set the time text directly into the headline
		RMBCString rTime, rAmPm;
		m_rTime.Format(rTime, rAmPm);
		m_pHeadlineInterface->SetHeadlineText(rTime);
		m_pHeadlineInterface->Commit();
		
		// Update the view
		m_pHeadlineView->InvalidateRenderCache();
		m_pHeadlineView->Invalidate();	
	}
	else
	{
		TpsAssertAlways("No interfaces available to set time.");
	}
}

void RTimepieceDialog::UpdateTimepieceComponent()
{
	// Load the graphic from the m_pSearchResultArray member and either add or replace it	
	RComponentDocument* pNewGraphicDocument = (*m_pSearchResultArray)[m_cGraphicListBox.GetCurSel()].GetComponent(m_pCompInfo, FALSE);
	
	// Set the component attributes
	pNewGraphicDocument->SetComponentAttributes(m_rGraphicAttributes);
	
	// Add the component to the document
	if (m_pGraphicView) 
		ReplaceComponent(m_pGraphicView->GetComponentDocument(), pNewGraphicDocument);
	else
		NewComponent(pNewGraphicDocument);	

	// Remove the headline, if there is one	
	if (m_pHeadlineView)
	{	
		RemoveComponent(m_pHeadlineView->GetComponentDocument());
		m_pHeadlineView = NULL;
	}
	
	// Resize the component
	m_pGraphicView = pNewGraphicDocument->GetView(m_pControlView);
	m_pGraphicView->FitToParent();	
	
	// Recreate the DateTime interface.  If it is NULL, this is a digital timepiece.
	delete m_pDateTimeInterface;
	m_pDateTimeInterface = dynamic_cast<RDateTimeInterface*>(m_pGraphicView->GetInterface(kDateTimeInterfaceId));
	if (!m_pDateTimeInterface)
	{
		// It is a digital Timepiece and we need to create a headline component		
		m_pHeadlineView = AddComponent(kHeadlineComponent, RRealRect(0, 0, 1, 1), m_rHeadlineAttributes);			

		// Create the headline interface
		delete m_pHeadlineInterface;
		m_pHeadlineInterface = dynamic_cast<RHeadlineInterface*>(m_pHeadlineView->GetInterface(kHeadlineInterfaceId));
		m_pHeadlineInterface->SetDistortEffect(kNonDistort);
		m_pHeadlineInterface->SetDistort(TRUE);
		m_pHeadlineInterface->SetOutlineEffect(kSimpleFill);			
		m_pHeadlineInterface->SetOutlineFillColor(RSolidColor(kRed));
		
		// If the default font was found on the system, use it
		if (!m_rTimepieceFont.IsEmpty()) m_pHeadlineInterface->SetFontName(m_rTimepieceFont);
		m_pHeadlineInterface->Commit();

		m_pHeadlineView->FitToParent();
		
		// Get the placement rect	
		RTimepiecePlacementInterface* pTimepiecePlacementInterface = dynamic_cast<RTimepiecePlacementInterface*>(m_pGraphicView->GetInterface(kTimepiecePlacementInterfaceId));
		TpsAssert(pTimepiecePlacementInterface, "Graphic does not support a Timepiece Placement interface.");
		RRealRect rPlacementRect = pTimepiecePlacementInterface->GetTimepiecePlacement();	
		delete pTimepiecePlacementInterface;

		// Reposition the headline view
		RRealSize rHeadlineSize = m_pHeadlineView->GetSize();
		m_pHeadlineView->Offset(RIntSize(rHeadlineSize.m_dx * rPlacementRect.m_Left, rHeadlineSize.m_dx * rPlacementRect.m_Top));
				
		YComponentBoundingRect rBounds = m_pHeadlineView->GetBoundingRect();			
		rBounds.ScaleAboutPoint(rBounds.m_TopLeft, RRealSize(rPlacementRect.m_Right, rPlacementRect.m_Bottom));
		m_pHeadlineView->SetBoundingRect(rBounds);
	}
}


void RTimepieceDialog::DoDataExchange(CDataExchange* pDX)
{
	RGenericGroupedObjectDialog::DoDataExchange(pDX);	

	DDX_Control(pDX, CONTROL_EDIT_HOUR,							m_cEditHour);
	DDX_Control(pDX, CONTROL_EDIT_MINUTE,						m_cEditMinute);
	DDX_Control(pDX, CONTROL_SPIN_HOUR,							m_cSpinHour);
	DDX_Control(pDX, CONTROL_SPIN_MINUTE,						m_cSpinMinute);	
	DDX_Control(pDX, CONTROL_LIST_AVAILABLE_TIMEPIECES,	m_cGraphicListBox);
}

BOOL RTimepieceDialog::OnInitDialog()
{
	// Call the base member
	RGenericGroupedObjectDialog::OnInitDialog();

	// Find all the available timepiece graphics
	RAlert rAlert;	
	if (m_rCollectionArray.Count() == 0)
	{
		EndDialog(IDCANCEL);
		return TRUE;
	}	
	
	// NOTE 4/11/97 GKB Do not delete this pointer.  It points to an internal member of RTimepieceCollectionSearcher
	m_pSearchResultArray = m_rSearcher.SearchCollections();
	if (!(m_pSearchResultArray->Count() > 0))
	{		
		rAlert.AlertUser(IDS_ERROR_NO_TIMEPIECES);
		EndDialog(IDCANCEL);
		return TRUE;
	}

	// Add the names to the list box
	m_cGraphicListBox.ResetContent();
	for (int nIdx = 0; nIdx < m_pSearchResultArray->Count(); nIdx++)
	{
		if (m_cGraphicListBox.InsertString(nIdx, reinterpret_cast<LPCTSTR>((*m_pSearchResultArray)[nIdx].namePtr)) != nIdx)
		{
			// An error occurred
			EndDialog(IDCANCEL);
			throw ::kMemory;
		}		
   }
	
	// Load the time and the name of the graphic
	ReadUIContextData();

	// Default the list box selection to 0, but set it appropriately if we can
	m_cGraphicListBox.SetCurSel(0);
	if (!m_rGraphicName.IsEmpty())
	{
		int nIdx = m_cGraphicListBox.FindString(0, static_cast<LPCTSTR>(m_rGraphicName));
		if (nIdx != LB_ERR) m_cGraphicListBox.SetCurSel(nIdx);	
	}
	
	// Initialize the Timepiece component
	UpdateTimepieceComponent();

	// Initialize the controls
	m_cSpinHour.SetRange(kHourMin, kHourMax);
	m_cSpinMinute.SetRange(kMinuteMin, kMinuteMax);

	uBYTE uHour = m_rTime.GetHour();
	uBYTE uMinute = m_rTime.GetMinutes();

	m_cSpinHour.SetPos(uHour);
	m_cSpinMinute.SetPos(uMinute);

	// Update the time
	UpdateTime();
	
	return TRUE;
}

// Message handling
BEGIN_MESSAGE_MAP(RTimepieceDialog, RGenericGroupedObjectDialog)
	ON_EN_CHANGE		(CONTROL_EDIT_HOUR,						OnTimeChange)
	ON_EN_CHANGE		(CONTROL_EDIT_MINUTE,					OnTimeChange)
	ON_EN_UPDATE		(CONTROL_EDIT_HOUR,						OnUpdateHour)
	ON_EN_UPDATE		(CONTROL_EDIT_MINUTE,					OnUpdateMinute)
	ON_LBN_SELCHANGE	(CONTROL_LIST_AVAILABLE_TIMEPIECES,	OnGraphicChange)
	ON_WM_TIMER()
END_MESSAGE_MAP()

void RTimepieceDialog::OnTimeChange()
{
	if (::IsWindow(m_cEditHour.m_hWnd) && ::IsWindow(m_cEditMinute.m_hWnd))
	{
		const DWORD		kEditSelectAll = 0xFFFF0000;
		BOOLEAN			fSetTime			= TRUE;
		CString			cHour, cMinute;
		m_cEditHour.GetWindowText(cHour);
		m_cEditMinute.GetWindowText(cMinute);

		if (cHour.GetLength() <= 0 || cMinute.GetLength() <= 0) return;
		
		int	nHour		= atoi( static_cast<LPCTSTR>(cHour) );
		int	nMinute	= atoi( static_cast<LPCTSTR>(cMinute) );

		//	Validate the new information
		if ( nMinute < kMinuteMin || nMinute > kMinuteMax )
		{
			char pMinute[20];
			m_cEditMinute.SetWindowText(itoa(m_rTime.GetMinutes(), pMinute, 10));
			m_cEditMinute.SetSel(kEditSelectAll);
			fSetTime = FALSE;
		}
		if ( nHour < kHourMin || nHour > kHourMax )
		{
			char pHour[20];
			m_cEditHour.SetWindowText(itoa(m_rTime.GetHour(), pHour, 10));
			m_cEditHour.SetSel(kEditSelectAll);
			fSetTime = FALSE;
		}

		// Set the time member
		if ( fSetTime )
			m_rTime = RTime(static_cast<uBYTE>(atoi(static_cast<LPCTSTR>(cHour))), static_cast<uBYTE>(atoi(static_cast<LPCTSTR>(cMinute))), TRUE);			
		else
			RenaissanceBeep( );

		BOOL fResult = KillTimer(m_uTimerID);
		m_uTimerID = SetTimer(kTimerID, kRenderTimeout, NULL);
	}
}

void RTimepieceDialog::OnUpdateHour()
{
	// Make sure that the new value is within the acceptable range
	if (::IsWindow(m_cEditHour.m_hWnd))
	{
		CString cHour;
		m_cEditHour.GetWindowText(cHour);
		uBYTE uHour = static_cast<uBYTE>(atoi(static_cast<LPCTSTR>(cHour)));
		if (uHour > kHourMax || uHour < kHourMin)
		{
			// Set the window text to the last hour
			char pHour[20];
			m_cEditHour.SetWindowText(itoa(m_rTime.GetHour(), pHour, 10));

			// Set the focus back to the edit control and highlight the selection		
			const DWORD kEditSelectAll = 0xFFFF0000;
			m_cEditHour.SetSel(kEditSelectAll);
			m_cEditHour.SetFocus();
		}
	}
}

void RTimepieceDialog::OnUpdateMinute()
{
	// Make sure that the new value is within the acceptable range
	if (::IsWindow(m_cEditMinute.m_hWnd))
	{
		CString cMinute;
		m_cEditMinute.GetWindowText(cMinute);
		uBYTE uMinute = static_cast<uBYTE>(atoi(static_cast<LPCTSTR>(cMinute)));
		if (uMinute > kMinuteMax || uMinute < kMinuteMin)
		{
			// Set the window text to the last minute
			char pMinute[20];
			m_cEditMinute.SetWindowText(itoa(m_rTime.GetMinutes(), pMinute, 10));

			// Set the focus back to the edit control and highlight the selection		
			const DWORD kEditSelectAll = 0xFFFF0000;
			m_cEditMinute.SetSel(kEditSelectAll);
			m_cEditMinute.SetFocus();
		}
	}
}

void RTimepieceDialog::OnGraphicChange()
{
	// Set the name of the graphic
	CString cGraphicName;	
	m_cGraphicListBox.GetText(m_cGraphicListBox.GetCurSel(), cGraphicName);	
	m_rGraphicName = static_cast<RMBCString>(cGraphicName);

	// Load the selected graphic
	UpdateTimepieceComponent();

	// Update the time
	UpdateTime();
}

void RTimepieceDialog::OnTimer(UINT)
{
	// Stop the timer and update the headline attributes
	BOOL fResult = KillTimer(m_uTimerID);

	// Update the time
	UpdateTime();
}

// ****************************************************************************
//
//  Function Name:	RTimepieceDialog::GetComponentType
//
//  Description:		Gets the type of component that this dialog creates and
//							edits
//
//  Returns:			Component type
//
//  Exceptions:		None
//
// ****************************************************************************
//
const YComponentType& RTimepieceDialog::GetComponentType() const
{
	static YComponentType componentType = kTimepieceComponent;

	return componentType;
}

// ****************************************************************************
//
//  Function Name:	RTimepieceDialog::WriteUIContextData
//
//  Description:		Writes this dialogs UI context data
//
//  Returns:			Nothing
//
//  Exceptions:		None
//
// ****************************************************************************
//
void RTimepieceDialog::WriteUIContextData(RUIContextData& rContextData) const
{
	// Write out the time and the name of the graphic
	rContextData << m_rTime;
	rContextData << m_rGraphicName;
}

// ****************************************************************************
//
//  Function Name:	RTimepieceDialog::ReadUIContextData
//
//  Description:		Read this dialogs UI context data
//
//  Returns:			pointer to ContextData
//
//  Exceptions:		None
//
// ****************************************************************************
//
RUIContextData* RTimepieceDialog::ReadUIContextData() 
{
	// If we are editting a component, try to read its context data
	if (m_pEditedComponent)
	{
		try
		{
			// Get the UI context data
			RUIContextData& rUIContextData = m_pEditedComponent->GetUIContextData();
			rUIContextData >> m_rTime;
			rUIContextData >> m_rGraphicName;
		}
		catch(...)
		{
		}
	}
	
	return NULL;
}
