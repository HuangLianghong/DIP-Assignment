// CHistogramDLG.cpp : implementation file
//

#include "pch.h"
#include "hlh516.h"
#include "CHistogramDLG.h"
#include "afxdialogex.h"


// CHistogramDLG dialog

IMPLEMENT_DYNAMIC(CHistogramDLG, CDialogEx)

CHistogramDLG::CHistogramDLG(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CHistogramDLG::~CHistogramDLG()
{
}

void CHistogramDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHistogramDLG, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CHistogramDLG message handlers

DWORD H[256];
void Histogram();
BOOL CHistogramDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	Histogram();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CHistogramDLG::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CDialogEx::OnPaint() for painting messages
	int i;
	DWORD max;

	dc.Rectangle(20, 20, 277, 221);

	max = 0;
	for (i = 0; i < 256; i++) {
		if (H[i] > max)
			max = H[i];
	}
	for (i = 0; i < 256; i++) {
		dc.MoveTo(i + 20, 220);
		dc.LineTo(i + 20, 220 - (int)(H[i] * 200 / max));
	}
}
