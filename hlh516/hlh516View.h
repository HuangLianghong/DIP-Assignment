
// hlh516View.h : interface of the Chlh516View class
//

#pragma once


class Chlh516View : public CScrollView
{
protected: // create from serialization only
	Chlh516View() noexcept;
	DECLARE_DYNCREATE(Chlh516View)

// Attributes
public:
	Chlh516Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~Chlh516View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnGray();
	afx_msg void OnUpdateGray(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHistogram();
	afx_msg void OnUpdateHistogram(CCmdUI* pCmdUI);
	afx_msg void OnLinear();
	afx_msg void OnUpdateLinear(CCmdUI* pCmdUI);
	afx_msg void OnEqualize();
	afx_msg void OnUpdateEqualize(CCmdUI* pCmdUI);
	afx_msg void OnFt();
	afx_msg void OnUpdateFt(CCmdUI* pCmdUI);
	afx_msg void OnIft();
	afx_msg void OnUpdateIft(CCmdUI* pCmdUI);
	afx_msg void OnFft();
	afx_msg void OnUpdateFft(CCmdUI* pCmdUI);
	afx_msg void OnIfft();
	afx_msg void OnUpdateIfft(CCmdUI* pCmdUI);
	afx_msg void OnFilter();
	afx_msg void OnUpdateFilter(CCmdUI* pCmdUI);
	afx_msg void OnMedian();
	afx_msg void OnUpdateMedian(CCmdUI* pCmdUI);
	afx_msg void OnSharp();
	afx_msg void OnUpdateSharp(CCmdUI* pCmdUI);
	afx_msg void OnTemplate();
	afx_msg void OnUpdateTemplate(CCmdUI* pCmdUI);
};

#ifndef _DEBUG  // debug version in hlh516View.cpp
inline Chlh516Doc* Chlh516View::GetDocument() const
   { return reinterpret_cast<Chlh516Doc*>(m_pDocument); }
#endif

