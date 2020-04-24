
// hlh516.h : main header file for the hlh516 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// Chlh516App:
// See hlh516.cpp for the implementation of this class
//

class Chlh516App : public CWinApp
{
public:
	Chlh516App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Chlh516App theApp;
