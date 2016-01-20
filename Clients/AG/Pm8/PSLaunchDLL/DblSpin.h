/* @doc
   @module DblSpin.h - the CBDoubleSpinner class |
   
  <c CBDoubleSpinner>, a spin control for double numbers

   <cp>Copyright 1995 Brøderbund Software, Inc., all rights reserved
*/

#ifndef DBLSPIN_H
#define DBLSPIN_H

/////////
// @class a spinner control for double numbers
//
class CBDoubleSpinner : public CSpinButtonCtrl
{
// Construction
public:
    // @access public members
    // @cmember constructor
    CBDoubleSpinner();
    // @cmember destructor
	virtual ~CBDoubleSpinner()
    {   }

    // @cmember set the range and interval
    void SetRange(double fFloor, double fCeiling, double fInterval);
    // @cmember set the position
    BOOL SetPosition(double fPosition);
    // @cmember get the current position
    double GetPosition();

    // @cmember set number of digits after decimal point (default=1)
    inline void SetPrecision(int n)
    {   m_nPrecision = n;   }

    // @cmember return lowest value allowed
    inline double GetMin() const
    {   return m_fFloor;    }

    // @cmember return highest value allowed
    inline double GetMax() const
    {   return m_fCeiling;  }

	//{{AFX_VIRTUAL(CBDoubleSpinner)
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CBDoubleSpinner)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDeltapos(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    // @access protected overrides prevent accidental use
    // @cmember calls CSpinButtonCtrl::SetPos()
    inline int SetPos(int nPos)
    {   return CSpinButtonCtrl::SetPos(nPos);   }

    // @cmember calls CSpinButtonCtrl::GetPos()
    inline int GetPos()
    {   return CSpinButtonCtrl::GetPos();   }

    // @cmember calls CSpinButtonCtrl::SetRange()
    inline void SetRange(int nLower, int nUpper)
    {   CSpinButtonCtrl::SetRange(nLower, nUpper);  }

    // @cmember update the associated text window
    void UpdateBuddy();

private:
    // @access private data
    // @cmember the lowest value allowed
    double m_fFloor;
    // @cmember the highest value allowed
    double m_fCeiling;
    // @cmember the amount that the current value changes on clicking up or down
    double m_fInterval;
    // @cmember the current value
    double m_fCurrent;

    // @cmember number of decimal points of precision (default=1)
    int m_nPrecision;

    // @cmember correct any range or roundoff errors in m_fCurrent
    void Fixup();

};


/////////////////////////////////////////////////////////////////////////////
// CBDoubleEdit window

/////////
// @class The edit control that is "buddied" to a <c CBDoubleSpinner>
//        control.
//
class CBDoubleEdit : public CEdit
{
public:
    // enable runtime type checking
    DECLARE_DYNCREATE(CBDoubleEdit)

    // @access public interface

    // @cmember default constructor
	inline CBDoubleEdit()
      : m_fValue(0.0),
        m_nPrecision(1),
        m_pSpinner(NULL)
    {   }

    // @cmember destructor
	inline virtual ~CBDoubleEdit()
    {   }

    // @cmember gets the associated floating point number
    inline double GetValue()
    {   return m_fValue;    }

    // @cmember sets the value and updates the string accordingly
    inline void SetValue(double d)
    {
        m_fValue = d;       
        UpdateString();
        Invalidate();
    }

    // @cmember sets the precision of the displayed string
    inline void SetPrecision(int n)
    {   
        if (m_nPrecision != n)
        {
            m_nPrecision = n;
            UpdateString();

            if (::IsWindow(m_hWnd))
            {
                Invalidate();
            }
        }
    }

    inline void AttachSpinner(CBDoubleSpinner *pSpinner)
    {   m_pSpinner = pSpinner;  }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBDoubleEdit)
	//}}AFX_VIRTUAL

protected:
    // @access protected interface
	// Generated message map functions
	//{{AFX_MSG(CBDoubleEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnSetText (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetText (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetTextLength (WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

#ifdef NEVER___
    // autoduck comments for AFX_MSG block
    // @cmember updates variable as control loses focus
	afx_msg void OnKillFocus(CWnd* pNewWnd);
#endif

private:
    // @access private variables

    // @cmember updates the text string to reflect m_fValue
	void UpdateString();

    // @cmember the number that the string in the control representes
    double m_fValue;

    // @cmember number of digits after the decimal point
    int m_nPrecision;

    // @cmember pointer to "Buddy" spin control
    CBDoubleSpinner *m_pSpinner;
};

#endif  // DBLSPIN_H
