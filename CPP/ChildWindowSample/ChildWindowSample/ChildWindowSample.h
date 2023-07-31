// ChildWindowSample.h : main header file for the ChildWindowSample application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CChildWindowSampleApp:
// See ChildWindowSample.cpp for the implementation of this class
//

class CChildWindowSampleApp : public CWinApp
{
private: 
	INT_PTR m_nReturnCode;

public:
	CChildWindowSampleApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CChildWindowSampleApp theApp;