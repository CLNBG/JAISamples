// AutoExposureSample.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CAutoExposureSampleApp:
// See AutoExposureSample.cpp for the implementation of this class
//

class CAutoExposureSampleApp : public CWinApp
{
private: 
	INT_PTR m_nReturnCode;

public:
	CAutoExposureSampleApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CAutoExposureSampleApp theApp;