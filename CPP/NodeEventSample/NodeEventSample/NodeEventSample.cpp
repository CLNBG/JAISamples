// NodeEventSample.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NodeEventSample.h"
#include "NodeEventSampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CNodeEventSampleApp

BEGIN_MESSAGE_MAP(CNodeEventSampleApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNodeEventSampleApp construction

CNodeEventSampleApp::CNodeEventSampleApp() : m_nReturnCode(0)
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}


// The one and only CNodeEventSampleApp object

CNodeEventSampleApp theApp;


// CNodeEventSampleApp initialization

BOOL CNodeEventSampleApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    // of your final executable, you should remove from the following
    // the specific initialization routines you do not need
    // Change the registry key under which our settings are stored
    // TODO: You should modify this string to be something appropriate
    // such as the name of your company or organization
    SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    CNodeEventSampleDlg dlg;

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
		Sleep(5000);
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CNodeEventSampleApp::ExitInstance()
{
	return (int)m_nReturnCode;
}