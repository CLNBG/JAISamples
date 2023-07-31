//

#include "stdafx.h"
#include "Wheel.h"
#include "WheelDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWheelApp

BEGIN_MESSAGE_MAP(CWheelApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()



CWheelApp::CWheelApp() : m_nReturnCode(0)
{

}


CWheelApp theApp;



BOOL CWheelApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	SetRegistryKey(_T("Wheel_Test"));

	CWheelDlg dlg;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//NOTE: cmdInfo.m_bRunAutomated is TRUE only during internal JAI testing!
	if(FALSE == cmdInfo.m_bRunAutomated)
	{
		m_pMainWnd = &dlg;
		m_nReturnCode = dlg.DoModal();
	}
	else
	{
		m_nReturnCode = IDOK;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CWheelApp::ExitInstance()
{
	return (int)m_nReturnCode;
}
