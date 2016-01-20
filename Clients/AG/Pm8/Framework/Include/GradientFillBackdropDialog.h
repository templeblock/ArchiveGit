// GradientFillBackdropDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// RGradientFillBackdropDialog dialog

#ifdef BYTE_ALIGNMENT
#pragma pack(push, BYTE_ALIGNMENT)
#endif

class RGradientFillBackdropDialog : public CDialog
{
// Construction
public:
	RGradientFillBackdropDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	BOOL	m_fGradientFillBackdrop;
	float	m_pctHilitePosition;
	float	m_angHiliteAngle;
	float	m_pctPrecision;
	float	m_pctStartX;
	float	m_pctStartY;
	float	m_pctEndX;
	float	m_pctEndY;
	int	m_iGradType;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:

	afx_msg void OnGradientfillbackdrop();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};

#ifdef BYTE_ALIGNMENT
#pragma pack(pop)
#endif
