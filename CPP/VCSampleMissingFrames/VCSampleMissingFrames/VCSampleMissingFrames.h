// VCSampleMissingFrames.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CVCSampleMissingFramesApp:
// See VCSampleMissingFrames.cpp for the implementation of this class
//

class CVCSampleMissingFramesApp : public CWinApp
{
private: 
	INT_PTR m_nReturnCode;

public:
	CVCSampleMissingFramesApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CVCSampleMissingFramesApp theApp;