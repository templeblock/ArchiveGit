// RegCtrl.h: interface for the CRegisterControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGCTRL_H__96F68320_641E_11D2_8BEF_ACC4E039B776__INCLUDED_)
#define AFX_REGCTRL_H__96F68320_641E_11D2_8BEF_ACC4E039B776__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CRegisterControl  
{
public:
   enum ErrorCode
   {
      ecNone,
      ecFileNotFound,
      ecLoadLibraryFailed,
      ecRegisterProcNotFound,
      ecRegisterFailed,
      ecInternalError
   };

	CRegisterControl();
	virtual ~CRegisterControl();

   ErrorCode   Register(LPCSTR szFileName);
   HRESULT     GetRegisterErrorCode();

// Helpers
protected:
   void        SetRegisterErrorCode(HRESULT hError);

// Data Members
private:
   HRESULT     m_hrRegisterErrorCode;
};

inline HRESULT CRegisterControl::GetRegisterErrorCode()
{
   return m_hrRegisterErrorCode;
}

inline void CRegisterControl::SetRegisterErrorCode(HRESULT hError)
{
   m_hrRegisterErrorCode = hError;
}

#endif // !defined(AFX_REGCTRL_H__96F68320_641E_11D2_8BEF_ACC4E039B776__INCLUDED_)
