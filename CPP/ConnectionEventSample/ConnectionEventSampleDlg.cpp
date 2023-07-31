// ConnectionEventSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ConnectionEventSample.h"
#include "ConnectionEventSampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Implementation
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


// CConnectionEventSampleDlg dialog
//============================================================================
// Thread Process Caller
//============================================================================
DWORD ProcessCaller(CConnectionEventSampleDlg* pThread)
{
    pThread->ConnectionSupervisionProcess();

    return 0;
}

CConnectionEventSampleDlg::CConnectionEventSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CConnectionEventSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // Initialize module variables
    m_hFactory = NULL;
    m_hCam = NULL;
    m_hConnectionSupervisionThread = NULL;
    m_hEventKill = NULL;
    m_hView = NULL;
    m_hThread = NULL;
	m_bThreadRunning = false;
}

void CConnectionEventSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CONNECTION_STATIC, m_ConnectionStatusLabel);
}

BEGIN_MESSAGE_MAP(CConnectionEventSampleDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
	ON_BN_CLICKED(IDCANCEL, &CConnectionEventSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CConnectionEventSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CConnectionEventSampleDlg message handlers
BOOL CConnectionEventSampleDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
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

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // Open factory & camera
    BOOL retval = OpenFactoryAndCamera();  

    // Get current time
    CTime now = CTime::GetCurrentTime();

    // Did we succeed?
    if (retval)
    {
        // Update text with connection status
        m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Camera just connected"));

        // Create the connection supervision thread
        CreateConnectionSupervisionThread();
    }
    else
    {
        m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Error connecting to camera"));
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CConnectionEventSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CConnectionEventSampleDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CConnectionEventSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CConnectionEventSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Close thread
    TerminateConnectionSupervisionThread();

    // Close factory & camera
    CloseFactoryAndCamera();
}

//--------------------------------------------------------------------------------------------------
// OpenFactoryAndCamera
//--------------------------------------------------------------------------------------------------
BOOL CConnectionEventSampleDlg::OpenFactoryAndCamera()
{
    J_STATUS_TYPE   retval;
    uint32_t        iSize;
    uint32_t        iNumDev;
    bool8_t         bHasChange;

    // Open factory
    retval = J_Factory_Open((int8_t*)"" , &m_hFactory);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not open factory!"));
        return FALSE;
    }
    TRACE("Opening factory succeeded\n");

    // Update camera list (search for any GigE Vision cameras)
    retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not update camera list!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("Updating camera list succeeded\n");

    // Get the number of Cameras
    retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not get the number of cameras!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (iNumDev == 0)
    {
        AfxMessageBox(CString("There is no camera!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("%d cameras were found\n", iNumDev);

    // Get camera ID
    iSize = (uint32_t)sizeof(m_sCameraId);
    retval = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_sCameraId, &iSize);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not get the camera ID!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("Camera ID: %s\n", m_sCameraId);

    // Open camera
    retval = J_Camera_Open(m_hFactory, m_sCameraId, &m_hCam);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not open the camera!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("Opening camera succeeded\n");

	//This sample is appropriate only for GigE cameras.
	int8_t buffer[J_CAMERA_ID_SIZE] = {0};
	uint32_t mySize = J_CAMERA_ID_SIZE;
	retval = J_Camera_GetTransportLayerName(m_hCam, buffer, &mySize);
	if (retval != J_ST_SUCCESS)
	{
		AfxMessageBox(CString("Error calling J_Camera_GetTransportLayerName."), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	//Convert to upper case
	std::string strTransportName((char*)buffer);
	std::transform(strTransportName.begin(), strTransportName.end(), strTransportName.begin(), (int( *)(int)) toupper);

	if(std::string::npos == strTransportName.find("GEV") && std::string::npos == strTransportName.find("GIGEVISION"))
	{
		int32_t ret = AfxMessageBox(CString("This sample only works with GigE cameras.\n\nExit application?"), MB_YESNO | MB_ICONSTOP);
		if(IDYES == ret)
		{
			OnOK();
			return FALSE;
		}
	}

    // Now we want to prepare a Image View Window so live images can be displayed
    int64_t int64Val;
    SIZE	ViewSize;
    POINT	TopLeft;

    // Get Width from the camera
    retval = J_Camera_GetValueInt64(m_hCam, (int8_t*)"Width", &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not get Width!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

    // Get Height from the camera
    retval = J_Camera_GetValueInt64(m_hCam, (int8_t*)"Height", &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not get Height!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

    // Set window position
    TopLeft.x = 100;
    TopLeft.y = 50;

    // Open view window
    retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView);
    if (retval != J_ST_SUCCESS) {
        AfxMessageBox(CString("Could not open view window!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("Opening view window succeeded\n");

    return TRUE;
}
//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CConnectionEventSampleDlg::CloseFactoryAndCamera()
{
    if (m_hCam)
    {
        // Close camera
        J_Camera_Close(m_hCam);
        m_hCam = NULL;
        TRACE("Closed camera\n");
    }

    if (m_hFactory)
    {
        // Close factory
        J_Factory_Close(m_hFactory);
        m_hFactory = NULL;
        TRACE("Closed factory\n");
    }
}

//--------------------------------------------------------------------------------------------------
// Create Stream Thread
//--------------------------------------------------------------------------------------------------
BOOL CConnectionEventSampleDlg::CreateConnectionSupervisionThread()
{
    // Is it already created?
    if(m_hConnectionSupervisionThread)
        return FALSE;

    // Set the thread execution flag
    m_bEnableThread = true;
    m_bThreadRunning = false;

    // Create a Stream Thread.
    if(NULL == (m_hConnectionSupervisionThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProcessCaller, this, NULL, NULL)))
    {
        OutputDebugString(_T("*** Error : CreateThread\n"));
        return FALSE;
    }

    return TRUE;
}

//==============================================================////
// Terminate Connection Event Thread
//==============================================================////
BOOL CConnectionEventSampleDlg::TerminateConnectionSupervisionThread(void)
{
    J_STATUS_TYPE retval;

    // Reset the thread execution flag.
    m_bEnableThread = false;

    // Signal the thread to stop faster
    SetEvent(m_hEventKill);

    // Wait for the thread to end
    while (m_bThreadRunning)
        Sleep(100);

    if(m_hConnectionSupervisionThread)
    {
        CloseHandle(m_hConnectionSupervisionThread);
        m_hConnectionSupervisionThread = NULL;
    }

    retval = J_Camera_ExecuteCommand(m_hCam, (int8_t*)"AcquisitionStop");

    if(m_hThread)
    {
        // Close stream
        retval = J_Image_CloseStream(m_hThread);
        if (retval != J_ST_SUCCESS)
        {
            AfxMessageBox(CString("Could not close Stream!"), MB_OK | MB_ICONEXCLAMATION);
        }
        m_hThread = NULL;
        TRACE("Closed stream\n");
    }

    if(m_hView)
    {
        // Close view window
        retval = J_Image_CloseViewWindow(m_hView);
        if (retval != J_ST_SUCCESS)
        {
            AfxMessageBox(CString("Could not close View Window!"), MB_OK | MB_ICONEXCLAMATION);
        }
        m_hView = NULL;
        TRACE("Closed view window\n");
    }

    return TRUE;
}

//==============================================================////
// Stream Processing Function
//==============================================================////
void CConnectionEventSampleDlg::ConnectionSupervisionProcess(void)
{
    uint32_t    iSize;
    J_EVENT_DATA_CONNECTION	  DataConnectionInfo;
    J_STATUS_TYPE retval;

    // Mark thread as running
    m_bThreadRunning = true;

    DWORD	iWaitResult;
    HANDLE	hEventConnectionChanged = CreateEvent(NULL, false, false, NULL);

    // Is the kill event already defined?
    if (m_hEventKill != NULL)
        CloseHandle(m_hEventKill);

    // Create a new one
    m_hEventKill = CreateEvent(NULL, true, false, NULL);

    // Make a list of events to be used in WaitForMultipleObjects
    HANDLE	EventList[2];
    EventList[0] = hEventConnectionChanged;
    EventList[1] = m_hEventKill;

    EVT_HANDLE hEvent = NULL; // Connection event handle

    // Register the connection event with the transport layer
    retval = J_Camera_RegisterEvent(m_hCam, EVENT_CONNECTION, hEventConnectionChanged, &hEvent); 

	if(J_ST_SUCCESS == retval)
	{
		OutputDebugString(_T(">>> Start Connection Supervision Process Loop.\n"));

		while(m_bEnableThread)
		{
			// Wait for Connection event (or kill event)
			iWaitResult = WaitForMultipleObjects(2, EventList, false, 1000);

			// Did we get a connection change event?
			if(WAIT_OBJECT_0 == iWaitResult)
			{
				// Get the Connection Info from the event
				iSize = (uint32_t)sizeof(J_EVENT_DATA_CONNECTION);
				retval = J_Event_GetData(hEvent, &DataConnectionInfo,  &iSize);

				if (retval == J_ST_SUCCESS)
				{
					// Update the status label
					CTime now = CTime::GetCurrentTime();
					switch(DataConnectionInfo.m_iConnectionState)
					{
					case J_EDC_CONNECTED:
						m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Camera just connected"));

						// If the Live Video display has not been started yet then start it using the J_Image_OpenStreamLight()
						if (m_hThread == NULL)
						{
							// Open stream
							retval = J_Image_OpenStreamLight(m_hCam, 0, &m_hThread);
							if (retval != J_ST_SUCCESS) {
								AfxMessageBox(CString("Could not open Stream!"), MB_OK | MB_ICONEXCLAMATION);
							}

							// Start the streaming inside the camera
							retval = J_Camera_ExecuteCommand(m_hCam, (int8_t*)"AcquisitionStart");
						}
						break;
					case J_EDC_DISCONNECTED:
						m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Camera just disconnected"));

						// We lost connection so we want to close the stream channel so it can be restarted later on
						// when the camera is reconnected
						if(m_hThread)
						{
							// Close stream
							retval = J_Image_CloseStream(m_hThread);
							if (retval != J_ST_SUCCESS)
							{
								AfxMessageBox(CString("Could not close Stream!"), MB_OK | MB_ICONEXCLAMATION);
							}
							m_hThread = NULL;
							TRACE("Closed stream\n");
						}
						break;
					case J_EDC_LOST_CONTROL:
						m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": We lost control of the camera"));
						break;
					case J_EDC_GOT_CONTROL: // Deprecated!!
						m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": We regained control of the camera"));
						break;
					}
				}
			}
			else
			{
				switch(iWaitResult)
				{
					// Kill event
				case	WAIT_OBJECT_0 + 1:
					break;
					// Timeout
				case	WAIT_TIMEOUT:
					break;
					// Unknown?
				default:
					break;
				}
			}

			// Here we need to see if more events has been queued up!
			uint32_t    NumEventsInQueue = 0;
			iSize = sizeof(NumEventsInQueue);

			retval = J_Event_GetInfo(hEvent, (J_EVENT_INFO_ID)EVENT_INFO_NUM_ENTRYS_IN_QUEUE, &NumEventsInQueue, &iSize);

			if ((retval == J_ST_SUCCESS) && (NumEventsInQueue > 0))
			{
				SetEvent(hEventConnectionChanged);
			}
		
		}//while(m_bEnableThread)
	
	}//if(J_ST_SUCCESS == retval)

    OutputDebugString(_T(">>> Terminated Connection Supervision Process Loop.\n"));

	J_Camera_UnRegisterEvent(m_hCam, EVENT_CONNECTION);
    // Free the event object
    if (hEvent != NULL)
    {
        J_Event_Close(hEvent);
        hEvent = NULL;
    }

    // Free the kill event
    if (m_hEventKill != NULL)
    {
        CloseHandle(m_hEventKill);
        m_hEventKill = NULL;
    }

    // Free the connection event object
    if (hEventConnectionChanged != NULL)
    {
        CloseHandle(hEventConnectionChanged);
        hEventConnectionChanged = NULL;
    }

    m_bThreadRunning = false;
}

void CConnectionEventSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CConnectionEventSampleDlg::OnBnClickedOk()
{
	OnOK();
}
