// ImageProcessingThreadSample.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define	WIN_WIDTH	480
#define	WIN_HEIGHT	360


// CImageProcessingThreadSampleApp:
// See ImageProcessingThreadSample.cpp for the implementation of this class
//

class CImageProcessingThreadSampleApp : public CWinApp
{
private: 
	INT_PTR m_nReturnCode;

public:
    CImageProcessingThreadSampleApp();

    // Overrides
public:
    virtual BOOL InitInstance();
	virtual int ExitInstance();

    // Implementation

    DECLARE_MESSAGE_MAP()
};

extern CImageProcessingThreadSampleApp theApp;