// WheelDlg.cpp : 
//

#include "stdafx.h"
#include "Wheel.h"
#include "WheelDlg.h"
#include <process.h>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()





CWheelDlg::CWheelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWheelDlg::IDD, pParent)
	, m_iUDSelect(0)
	, m_uipTimerID(0)
	, m_iCurrentState(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWheelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UP, m_oOpen);
	DDX_Control(pDX, IDC_STOP, m_oStop);
	DDX_Control(pDX, IDC_DOWN, m_oClose);
}

BEGIN_MESSAGE_MAP(CWheelDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDCANCEL, &CWheelDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CWheelDlg::OnBnClickedOk)
END_MESSAGE_MAP()



BOOL CWheelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


    BOOL retval;

    // Open factory & camera
    retval = OpenFactoryAndCamera();  


	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	SetCheckMark(m_iCurrentState);
	

	return TRUE;
}

void CWheelDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CWheelDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CWheelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CWheelDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CWheelDlg::OnBnClickedOk()
{
	OnOK();
}

////////////////////////////////////////////////////////////////////
// Handler of WM_MOUSEWHEEL.
////////////////////////////////////////////////////////////////////
BOOL CWheelDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    J_STATUS_TYPE   retval;

	// Stops the timer if active.
	if(m_uipTimerID != 0)
	{
		KillTimer(m_uipTimerID);
		m_uipTimerID = 0;
	}

	// Wheel was rotated upward.
	if(zDelta > 0)
	{
		if(m_iCurrentState != 0)
		{	// If the state has changed ....
			// Send the Open command.
			//J_Camera_SetValueString(m_hCam, "MotorLensIris", "Open");
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensIris", (int8_t*)"Open");
			//retval = J_Camera_SetValueInt64(m_hCam, "MotorLensIris", 1);
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensIris=Open!"), retval);
				//return;
			}
		}

		m_iCurrentState = 0;
		SetCheckMark(m_iCurrentState);
	}
	// Wheel was rotated downward.
	else if(zDelta < 0)
	{
		if(m_iCurrentState != 2)
		{	// If the state has changed ....
			// Send the Close command.
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensIris", (int8_t*)"Close");
			//retval = J_Camera_SetValueInt64(m_hCam, "MotorLensIris", 2);
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensIris=Close!"), retval);
				//return;
			}
		}

		m_iCurrentState = 2;
		SetCheckMark(m_iCurrentState);
	}

	// Starts a timeout timer.
	//m_uipTimerID = SetTimer(1, 100, NULL);	// (100ms)
	m_uipTimerID = SetTimer(1, 10, NULL);		// (10ms)

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

////////////////////////////////////////////////////////////////////
// Handler of Timer.
////////////////////////////////////////////////////////////////////
void CWheelDlg::OnTimer(UINT_PTR nIDEvent)
{
    J_STATUS_TYPE   retval;

	if(nIDEvent == 1)
	{
		KillTimer(nIDEvent);
		m_uipTimerID = 0;

		if(m_iCurrentState != 1)
		{
			// Send the Stop command.
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensIris", (int8_t*)"Stop");
			//retval = J_Camera_SetValueInt64(m_hCam, "MotorLensIris", 0);
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensIris=Stop!"), retval);
				//return;
			}
		}

		m_iCurrentState = 1;
		SetCheckMark(m_iCurrentState);
	}

	CDialog::OnTimer(nIDEvent);
}

// Set check marks.
int CWheelDlg::SetCheckMark(int iSelect)
{
	switch(iSelect)
	{
	case	0:
		m_oOpen.SetCheck(1);
		m_oStop.SetCheck(0);
		m_oClose.SetCheck(0);
		break;
	case	1:
		m_oStop.SetCheck(1);
		m_oOpen.SetCheck(0);
		m_oClose.SetCheck(0);
		break;
	case	2:
		m_oClose.SetCheck(1);
		m_oOpen.SetCheck(0);
		m_oStop.SetCheck(0);
		break;
	}

	return 0;
}

void CWheelDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
{
    CString errorMsg;
    errorMsg.Format(_T("%s: Error = %d: "), message, error);

    switch(error)
    {
    case J_ST_INVALID_BUFFER_SIZE:	errorMsg += "Invalid buffer size ";	                break;
    case J_ST_INVALID_HANDLE:		errorMsg += "Invalid handle ";		                break;
    case J_ST_INVALID_ID:			errorMsg += "Invalid ID ";			                break;
    case J_ST_ACCESS_DENIED:		errorMsg += "Access denied ";		                break;
    case J_ST_NO_DATA:				errorMsg += "No data ";				                break;
    case J_ST_ERROR:				errorMsg += "Generic error ";		                break;
    case J_ST_INVALID_PARAMETER:	errorMsg += "Invalid parameter ";	                break;
    case J_ST_TIMEOUT:				errorMsg += "Timeout ";				                break;
    case J_ST_INVALID_FILENAME:		errorMsg += "Invalid file name ";	                break;
    case J_ST_INVALID_ADDRESS:		errorMsg += "Invalid address ";		                break;
    case J_ST_FILE_IO:				errorMsg += "File IO error ";		                break;
    case J_ST_GC_ERROR:				errorMsg += "GenICam error ";		                break;
    case J_ST_VALIDATION_ERROR:		errorMsg += "Settings File Validation Error ";		break;
    case J_ST_VALIDATION_WARNING:	errorMsg += "Settings File Validation Warning ";    break;
    }

    AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);

}

//--------------------------------------------------------------------------------------------------
// OpenFactoryAndCamera
//--------------------------------------------------------------------------------------------------
BOOL CWheelDlg::OpenFactoryAndCamera()
{
	J_STATUS_TYPE   retval;
	uint32_t        iSize;
	uint32_t        iNumDev;
	bool8_t         bHasChange;

	m_bEnableStreaming = false;

	// Open factory
	retval = J_Factory_Open((int8_t*)"", &m_hFactory);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not open factory!"), retval);
		return FALSE;
	}
	TRACE("Opening factory succeeded\n");

	//Update camera list
	retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not update camera list!"), retval);
		return FALSE;
	}
	TRACE("Updating camera list succeeded\n");

	// Get the number of Cameras
	retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get the number of cameras!"), retval);
		return FALSE;
	}

	if (iNumDev == 0)
	{
		ShowErrorMsg(CString("Invalid number of cameras!"), retval);
		return FALSE;
	}
	TRACE("%d cameras were found\n", iNumDev);

	// Get camera ID
	iSize = (uint32_t)sizeof(m_sCameraId);
	retval = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_sCameraId, &iSize);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get the camera ID!"), retval);
		return FALSE;
	}
	TRACE("Camera ID: %s\n", m_sCameraId);

	// Open camera
	retval = J_Camera_Open(m_hFactory, m_sCameraId, &m_hCam);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not open the camera!"), retval);
		return FALSE;
	}

	//Make sure streaming is supported!
	uint32_t numStreams = 0;
	retval = J_Camera_GetNumOfDataStreams(m_hCam, &numStreams);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Error with J_Camera_GetNumOfDataStreams."), retval);
		return FALSE;
	}

	if(0 == numStreams)
	{
		m_bEnableStreaming = false;
	}
	else
	{
		m_bEnableStreaming = true;
	}


	if(iNumDev > 0 && 0 != m_hCam)
	{
		TRACE("Opening camera succeeded\n");
	}
	else
	{
		ShowErrorMsg(CString("Invalid number of Devices!"), iNumDev);
		return FALSE;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CWheelDlg::CloseFactoryAndCamera()
{
    J_STATUS_TYPE   retval;

    if (m_hCam)
    {
        // Close camera
        retval = J_Camera_Close(m_hCam);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close the camera!"), retval);
        }
        m_hCam = NULL;
        TRACE("Closed camera\n");
    }

    if (m_hFactory)
    {
        // Close factory
        retval = J_Factory_Close(m_hFactory);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close the factory!"), retval);
        }
        m_hFactory = NULL;
        TRACE("Closed factory\n");
    }
}

void CWheelDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。

    // Close factory & camera
    CloseFactoryAndCamera();

}
