#pragma once

class COutlook
{
// Construction
public:
	COutlook(IDispatch* pApplication);
	void AddAppointment();
	void MapiSendmail();
	void AddContact();

// Implementation
protected:
	CComPtr<IDispatch> m_pApplication;
};
