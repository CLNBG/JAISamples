// Wheel.h : 
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'stdafx.h' をインクルードしてください"
#endif

#include "resource.h"


// CWheelApp:

class CWheelApp : public CWinApp
{
private: 
	INT_PTR m_nReturnCode;

public:
	CWheelApp();

	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CWheelApp theApp;