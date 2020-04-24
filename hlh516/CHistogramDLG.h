#pragma once


// CHistogramDLG dialog

class CHistogramDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CHistogramDLG)

public:
	CHistogramDLG(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CHistogramDLG();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnIddChistogram();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
};
