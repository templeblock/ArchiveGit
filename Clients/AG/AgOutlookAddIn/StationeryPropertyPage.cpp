#include "stdafx.h"
#include ".\Stationerypropertypage.h"

CStationeryPropertyPage::CStationeryPropertyPage(void) : CPropertyPageImpl<CStationeryPropertyPage>(_T("Stationery"))
{
	m_psp.hInstance = _AtlBaseModule.GetResourceInstance();
	m_nClearDefault = 0;
	m_nClearCurrent = 0;
}
/////////////////////////////////////////////////////////////////////////////
CStationeryPropertyPage::~CStationeryPropertyPage(void)
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CStationeryPropertyPage::GetCurrentStatus()
{
	return m_nClearCurrent;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CStationeryPropertyPage::GetDefaultStatus()
{
	return m_nClearDefault;
}
/////////////////////////////////////////////////////////////////////////////
void CStationeryPropertyPage::SetDefaultStatus(BOOL bVal)
{
	m_nClearDefault = bVal;
}
/////////////////////////////////////////////////////////////////////////////
void CStationeryPropertyPage::SetCurrentStatus(BOOL bVal)
{
	m_nClearCurrent = bVal;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CStationeryPropertyPage::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    //DoDataExchange(false);
	ShowWindow(SW_SHOW);
    return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CStationeryPropertyPage::OnApply()
{
    return DoDataExchange(true) ? PSNRET_NOERROR : PSNRET_INVALID;
}
/////////////////////////////////////////////////////////////////////////////
int CStationeryPropertyPage::OnSetActive()
{
    return CPropertyPageImpl<CStationeryPropertyPage>::OnSetActive(); //this returns TRUE
}