
// hlh516View.cpp : implementation of the Chlh516View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "hlh516.h"
#endif

#include "hlh516Doc.h"
#include "hlh516View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "Algorithms.h"
#include "CHistogramDLG.h"

// Chlh516View

IMPLEMENT_DYNCREATE(Chlh516View, CScrollView)

BEGIN_MESSAGE_MAP(Chlh516View, CScrollView)
	ON_COMMAND(ID_GRAY, &Chlh516View::OnGray)
	ON_UPDATE_COMMAND_UI(ID_GRAY, &Chlh516View::OnUpdateGray)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_HISTOGRAM, &Chlh516View::OnHistogram)
	ON_UPDATE_COMMAND_UI(ID_HISTOGRAM, &Chlh516View::OnUpdateHistogram)
	ON_COMMAND(ID_LINEAR, &Chlh516View::OnLinear)
	ON_UPDATE_COMMAND_UI(ID_LINEAR, &Chlh516View::OnUpdateLinear)
END_MESSAGE_MAP()

// Chlh516View construction/destruction

Chlh516View::Chlh516View() noexcept
{
	// TODO: add construction code here

}

Chlh516View::~Chlh516View()
{
}

BOOL Chlh516View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// Chlh516View drawing

void Chlh516View::OnDraw(CDC* pDC)
{
	Chlh516Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	if (NULL == lpBitsInfo) return;

/*
	//将黑白二值图变为青红二色
	if (lpBitsInfo->bmiHeader.biClrUsed == 2) {
		RGBQUAD pal[2];
		pal[0].rgbRed = 255;
		pal[0].rgbGreen = 0;
		pal[0].rgbBlue = 0;
		pal[0].rgbReserved = 0;

		pal[1].rgbRed = 0;
		pal[1].rgbGreen = 255;
		pal[1].rgbBlue = 255;
		pal[1].rgbReserved = 0;
		memcpy(lpBitsInfo->bmiColors, pal, 8);
	}
*/

	LPVOID lpBits =(LPVOID)&lpBitsInfo->bmiColors[lpBitsInfo->bmiHeader.biClrUsed];
	StretchDIBits(
		pDC->GetSafeHdc(),
		0, 0,
		lpBitsInfo->bmiHeader.biWidth,
		lpBitsInfo->bmiHeader.biHeight,
		0, 0,
		lpBitsInfo->bmiHeader.biWidth,
		lpBitsInfo->bmiHeader.biHeight,
		lpBits,
		lpBitsInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);

}

void Chlh516View::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	CSize sizeTotal;
	// TODO: calculate the total size of this view
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
}


// Chlh516View diagnostics

#ifdef _DEBUG
void Chlh516View::AssertValid() const
{
	CScrollView::AssertValid();
}

void Chlh516View::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

Chlh516Doc* Chlh516View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Chlh516Doc)));
	return (Chlh516Doc*)m_pDocument;
}
#endif //_DEBUG


// Chlh516View message handlers

void gray();
void Chlh516View::OnGray()
{
	// TODO: Add your command handler code here
	gray();
	Invalidate();
}


void Chlh516View::OnUpdateGray(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lpBitsInfo != NULL && lpBitsInfo->bmiHeader.biBitCount == 24);
	
}

void pixel(int i, int j, char*);
void Chlh516View::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	char xy[100];
	memset(xy, 0, 100);
	sprintf(xy, "x:%d y:%d	", point.x, point.y);

	char rgb[100];
	memset(rgb, 0, 100);
	pixel(point.y, point.x, rgb);

	strcat(xy, rgb);


	((CFrameWnd*)GetParent())->SetMessageText(xy);

	CScrollView::OnMouseMove(nFlags, point);
}


void Chlh516View::OnHistogram()
{
	// TODO: Add your command handler code here
	Invalidate();
	CHistogramDLG dlg;
	dlg.DoModal();
}

bool IsGray();
void Chlh516View::OnUpdateHistogram(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void LinearTran(float, float);
void Chlh516View::OnLinear()
{
	// TODO: Add your command handler code here
	LinearTran(-1, 255);
	Invalidate();
}


void Chlh516View::OnUpdateLinear(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}
