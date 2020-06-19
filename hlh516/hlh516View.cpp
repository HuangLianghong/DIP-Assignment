
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
	ON_COMMAND(ID_EQUALIZE, &Chlh516View::OnEqualize)
	ON_UPDATE_COMMAND_UI(ID_EQUALIZE, &Chlh516View::OnUpdateEqualize)
	ON_COMMAND(ID_FT, &Chlh516View::OnFt)
	ON_UPDATE_COMMAND_UI(ID_FT, &Chlh516View::OnUpdateFt)
	ON_COMMAND(ID_IFT, &Chlh516View::OnIft)
	ON_UPDATE_COMMAND_UI(ID_IFT, &Chlh516View::OnUpdateIft)
	ON_COMMAND(ID_FFT, &Chlh516View::OnFft)
	ON_UPDATE_COMMAND_UI(ID_FFT, &Chlh516View::OnUpdateFft)
	ON_COMMAND(ID_IFFT, &Chlh516View::OnIfft)
	ON_UPDATE_COMMAND_UI(ID_IFFT, &Chlh516View::OnUpdateIfft)
	ON_COMMAND(ID_FILTER, &Chlh516View::OnFilter)
	ON_UPDATE_COMMAND_UI(ID_FILTER, &Chlh516View::OnUpdateFilter)
	ON_COMMAND(ID_MEDIAN, &Chlh516View::OnMedian)
	ON_UPDATE_COMMAND_UI(ID_MEDIAN, &Chlh516View::OnUpdateMedian)
	ON_COMMAND(ID_SHARP, &Chlh516View::OnSharp)
	ON_UPDATE_COMMAND_UI(ID_SHARP, &Chlh516View::OnUpdateSharp)
	ON_COMMAND(ID_TEMPLATE, &Chlh516View::OnTemplate)
	ON_UPDATE_COMMAND_UI(ID_TEMPLATE, &Chlh516View::OnUpdateTemplate)
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
extern BITMAPINFO* lpDIB_FFT;
extern BITMAPINFO* lpDIB_IFFT;
void Chlh516View::OnDraw(CDC* pDC)
{
	Chlh516Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	if (NULL == lpBitsInfo) return;
	if (lpDIB_FFT)
	{
		BYTE* lpBits = (BYTE*)&lpDIB_FFT->bmiColors[256];
		StretchDIBits(
			pDC->GetSafeHdc(),
			520, 0,
			lpDIB_FFT->bmiHeader.biWidth,
			lpDIB_FFT->bmiHeader.biHeight,
			0, 0,
			lpDIB_FFT->bmiHeader.biWidth,
			lpDIB_FFT->bmiHeader.biHeight,
			lpBits,
			lpDIB_FFT, // bitmap data 
			DIB_RGB_COLORS,
			SRCCOPY);
	}
	if (lpDIB_IFFT)
	{
		BYTE* lpBits = (BYTE*)&lpDIB_IFFT->bmiColors[256];
		StretchDIBits(
			pDC->GetSafeHdc(),
			1040, 0,
			lpDIB_IFFT->bmiHeader.biWidth,
			lpDIB_IFFT->bmiHeader.biHeight,
			0, 0,
			lpDIB_IFFT->bmiHeader.biWidth,
			lpDIB_IFFT->bmiHeader.biHeight,
			lpBits,
			lpDIB_IFFT, // bitmap data 
			DIB_RGB_COLORS,
			SRCCOPY);
	}



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
	LinearTran(2, 15);
	Invalidate();
}


void Chlh516View::OnUpdateLinear(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void Equalize();
void Chlh516View::OnEqualize()
{
	// TODO: Add your command handler code here
	Equalize();
	Invalidate();
}


void Chlh516View::OnUpdateEqualize(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void Fourier();
void Chlh516View::OnFt()
{
	// TODO: Add your command handler code here
	Fourier();
	Invalidate();
}


void Chlh516View::OnUpdateFt(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void IFourier();
void Chlh516View::OnIft()
{
	// TODO: Add your command handler code here
	IFourier();
	Invalidate();
}

BOOL gFD_isValid();
void Chlh516View::OnUpdateIft(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(gFD_isValid());
}

void FFourier();
void Chlh516View::OnFft()
{
	// TODO: Add your command handler code here
	if (lpDIB_FFT)
		free(lpDIB_FFT);
	FFourier();
	Invalidate();
}


void Chlh516View::OnUpdateFft(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void IFFourier();
void Chlh516View::OnIfft()
{
	// TODO: Add your command handler code here
	if (lpDIB_IFFT)
		free(lpDIB_IFFT);

	IFFourier();
	Invalidate();

}


void Chlh516View::OnUpdateIfft(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(gFD_isValid());
}

void Ideal_Filter_FFT(int );
void Chlh516View::OnFilter()
{
	// TODO: Add your command handler code here
	Ideal_Filter_FFT(80);
	Invalidate();
}


void Chlh516View::OnUpdateFilter(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(gFD_isValid());
}

void MedianFilter();
void Chlh516View::OnMedian()
{
	// TODO: Add your command handler code here
	MedianFilter();
	Invalidate();
}


void Chlh516View::OnUpdateMedian(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lpBitsInfo != NULL && lpBitsInfo->bmiHeader.biBitCount == 8);
}

void GradientSharp();
void Chlh516View::OnSharp()
{
	// TODO: Add your command handler code here
	GradientSharp();
	Invalidate();
}


void Chlh516View::OnUpdateSharp(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}

void Template(int* Array, float coef);
void Chlh516View::OnTemplate()
{
	// TODO: Add your command handler code here
	int array[9];
	for (int i = 0; i < 9; ++i)	array[i] = 1;
	Template(array, 1.0/9.0);
	Invalidate();
}


void Chlh516View::OnUpdateTemplate(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lpBitsInfo != NULL && IsGray());
}
