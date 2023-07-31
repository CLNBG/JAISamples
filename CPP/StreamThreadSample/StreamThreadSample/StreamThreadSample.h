// StreamThreadSample.h : main header file for the StreamThreadSample application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CStreamThreadSampleApp:
// See StreamThreadSample.cpp for the implementation of this class
//

class CStreamThreadSampleApp : public CWinApp
{
private: 
	INT_PTR m_nReturnCode;

public:
	CStreamThreadSampleApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CStreamThreadSampleApp theApp;