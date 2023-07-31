// GigEVisionEventSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GigEVisionEventSample.h"
#include "GigEVisionEventSampleDlg.h"

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


// CGigEVisionEventSampleDlg dialog
//============================================================================
// Thread Process Caller
//============================================================================
DWORD ProcessCaller(CGigEVisionEventSampleDlg* pThread)
{
    pThread->SupervisionProcess();

    return 0;
}

CGigEVisionEventSampleDlg::CGigEVisionEventSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGigEVisionEventSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // Initialize module variables
    m_hFactory = NULL;
    m_hCamera = NULL;
    m_hSupervisionThread = NULL;
    m_hCondition = NULL;
	m_hView		= NULL;
	m_hThread	= NULL;
	m_bThreadRunning = false;
}

void CGigEVisionEventSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EVENT_STATIC, m_StatusLabel);
}

BEGIN_MESSAGE_MAP(CGigEVisionEventSampleDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START_BUTTON, &CGigEVisionEventSampleDlg::OnBnClickedStartButton)
    ON_BN_CLICKED(IDC_STOP_BUTTON, &CGigEVisionEventSampleDlg::OnBnClickedStopButton)
    ON_BN_CLICKED(IDC_TRIG_BUTTON, &CGigEVisionEventSampleDlg::OnBnClickedTrigButton)
	ON_BN_CLICKED(IDCANCEL, &CGigEVisionEventSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CGigEVisionEventSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGigEVisionEventSampleDlg message handlers
BOOL CGigEVisionEventSampleDlg::OnInitDialog()
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
        m_StatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": No events received yet! Please start acquisition and trigger the camera to start receiving events"));

        // Create the connection supervision thread
        CreateSupervisionThread();
    }
    else
    {
        m_StatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Error connecting to camera"));
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGigEVisionEventSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGigEVisionEventSampleDlg::OnPaint()
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
HCURSOR CGigEVisionEventSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CGigEVisionEventSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Close thread
	OnBnClickedStopButton();

    TerminateSupervisionThread();

    // Close factory & camera
    CloseFactoryAndCamera();
}

void CGigEVisionEventSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CGigEVisionEventSampleDlg::OpenFactoryAndCamera()
{
    J_STATUS_TYPE   retval;
    uint32_t        iSize;
    uint32_t        iNumDev;
    bool8_t         bHasChange;

    // Open factory
    retval = J_Factory_Open((int8_t*)"", &m_hFactory);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not open factory!"), retval);
        return FALSE;
    }
    TRACE("Opening factory succeeded\n");

    // Update camera list (search for any GigE Vision cameras)
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
        AfxMessageBox(CString("There is no camera!"), MB_OK | MB_ICONEXCLAMATION);
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
    retval = J_Camera_Open(m_hFactory, m_sCameraId, &m_hCamera);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not open the camera!"), retval);
        return FALSE;
    }
    TRACE("Opening camera succeeded\n");

	//This sample is appropriate only for GigE cameras.
	int8_t buffer[J_CAMERA_ID_SIZE] = {0};
	uint32_t mySize = J_CAMERA_ID_SIZE;
	retval = J_Camera_GetTransportLayerName(m_hCamera, buffer, &mySize);
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

    // Prepare the camera for Software Trigger
    PrepareSwTrigSetting();

    return TRUE;
}
//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CGigEVisionEventSampleDlg::CloseFactoryAndCamera()
{
    J_STATUS_TYPE   retval;

    if (m_hCamera)
    {
		retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerMode", (int8_t*)"Off");

        // Close camera
        retval = J_Camera_Close(m_hCamera);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close the camera!"), retval);
        }
        m_hCamera = NULL;
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

//--------------------------------------------------------------------------------------------------
// Create Stream Thread
//--------------------------------------------------------------------------------------------------
BOOL CGigEVisionEventSampleDlg::CreateSupervisionThread()
{
    // Is it already created?
    if(m_hSupervisionThread)
        return FALSE;

    // Set the thread execution flag
    m_bEnableThread = true;
    m_bThreadRunning = false;

    // Create a Stream Thread.
    if(NULL == (m_hSupervisionThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProcessCaller, this, NULL, NULL)))
    {
        OutputDebugString(_T("*** Error : CreateThread\n"));
        return FALSE;
    }

    return TRUE;
}

//==============================================================////
// Terminate Connection Event Thread
//==============================================================////
BOOL CGigEVisionEventSampleDlg::TerminateSupervisionThread(void)
{
    J_STATUS_TYPE   retval;

    // Reset the thread execution flag.
    m_bEnableThread = false;

    // Signal the thread to stop faster
	if(NULL != m_hCondition)
	{
		retval = J_Event_ExitCondition(m_hCondition);
		m_hCondition = NULL;
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not Exit Condition!"), retval);
			return FALSE;
		}
	}

    // Wait for the thread to end
    while (m_bThreadRunning)
        Sleep(100);

    if(m_hSupervisionThread)
    {
        CloseHandle(m_hSupervisionThread);
        m_hSupervisionThread = NULL;
    }

    return TRUE;
}

//==============================================================////
// GigE Vision Event Processing Function
//==============================================================////
void CGigEVisionEventSampleDlg::SupervisionProcess(void)
{
    J_STATUS_TYPE	iResult;
    uint32_t    iSize;
    J_EVENT_DATA_GEV_EVENT_CMD	  GigEEventInfo;

    // Mark thread as running
    m_bThreadRunning = true;

    J_COND_WAIT_RESULT	WaitResult;

    // Create the condition used for signalling the new image event
    iResult = J_Event_CreateCondition(&m_hCondition);

    EVT_HANDLE	hEvent; // GigE Vision event handle returned from Transport Layer

    // Register the GigE Vision event with the transport layer
    J_Camera_RegisterEvent(m_hCamera, EVENT_GEV_EVENT_CMD, m_hCondition, &hEvent); 

    OutputDebugString(_T(">>> Start Supervision Process Loop.\n"));

    while(m_bEnableThread)
    {
        // Wait for Buffer event (or kill event)
        iResult = J_Event_WaitForCondition(m_hCondition, 1000, &WaitResult);

        // Did we get a new buffer event?
        if(J_COND_WAIT_SIGNAL == WaitResult)
        {
            // Get the GigE Vision Event Info from the transport layer
            iSize = (uint32_t)sizeof(J_EVENT_DATA_GEV_EVENT_CMD);
            J_STATUS_TYPE error = J_Event_GetData(hEvent, &GigEEventInfo,  &iSize);

            if (error == J_ST_SUCCESS)
            {
               // Update the status label
               CTime now = CTime::GetCurrentTime();
               CString eventString;
               eventString.Format(_T("%s: We got event ID=%d, Stream Ch=%d, Block ID=%d, Timestamp=%lld"), now.Format("%c"), GigEEventInfo.m_EventID, GigEEventInfo.m_StreamChannelIndex,GigEEventInfo.m_BlockID, GigEEventInfo.m_Timestamp);
               m_StatusLabel.SetWindowText(eventString);
            }

            // Now we check if there are any pending events! If this is the case then we force it to be read right away
            uint32_t    EventInfo = 0;
            iSize = sizeof(EventInfo);
            error = J_Event_GetInfo(hEvent, EVENT_INFO_NUM_ENTRYS_IN_QUEUE, &EventInfo, &iSize);

            // Signal the event so the WaitForMultipleObjects will be activated right away
            if ((error == J_ST_SUCCESS) && (EventInfo > 0))
                J_Event_SignalCondition(m_hCondition);
        }
        else
        {
            switch(WaitResult)
            {
                // Kill event
                case	J_COND_WAIT_EXIT:
                    iResult = 1;
                    break;
                // Timeout
                case	J_COND_WAIT_TIMEOUT:
                    iResult = 2;
                    break;
                // Error event
                case	J_COND_WAIT_ERROR:
                    iResult = 3;
                    break;
                // Unknown?
                default:
                    iResult = 4;
                    break;
            }
        }
    }

    OutputDebugString(_T(">>> Terminated Supervision Process Loop.\n"));
    
    // Un-register the GigE Vision event with the transport layer
    J_Camera_UnRegisterEvent(m_hCamera, EVENT_GEV_EVENT_CMD); 

    // Free the event object
    if (hEvent != NULL)
    {
        J_Event_Close(hEvent);
        hEvent = NULL;
    }

    // Free the Condition
    if (m_hCondition != NULL)
    {
        J_Event_CloseCondition(m_hCondition);
        m_hCondition = NULL;
    }

    m_bThreadRunning = false;
}


void CGigEVisionEventSampleDlg::PrepareSwTrigSetting()
{
	J_STATUS_TYPE retval;

    if(m_hCamera)
	{
        // We have two possible ways of setting up triggers: JAI or GenICam SFNC
        // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
        // called "TriggerSelector". Therefor we have to determine which way to use here.
        // First we see if a node called "TriggerSelector" exists.
        NODE_HANDLE hNode = NULL;
        retval = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"TriggerSelector", &hNode);

        // Does the "TriggerSelector" node exist?
        if ((retval == J_ST_SUCCESS) && (hNode != NULL))
        {
            // Here we assume that this is GenICam SFNC trigger so we do the following:
            // TriggerSelector=FrameStart
            // TriggerMode=On
            // TriggerSource=Software

            // Set TriggerSelector="FrameStart"
            retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
		    if(retval != J_ST_SUCCESS) 
		    {
                ShowErrorMsg(CString("Could not set TriggerSelector=FrameStart!"), retval);
			    return;
		    }

		    // Set TriggerMode="On"
            retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerMode", (int8_t*)"On");
		    if(retval != J_ST_SUCCESS) 
		    {
                ShowErrorMsg(CString("Could not set TriggerMode=On!"), retval);
			    return;
		    }

		    // Set TriggerSource="Software" 
            retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSource", (int8_t*)"Software");
		    if(retval != J_ST_SUCCESS) 
		    {
                ShowErrorMsg(CString("Could not set TriggerSource=Software!"), retval);
			    return;
		    }
        }
        else
        {
            // Here we assume that this is JAI trigger so we do the following:
            // ExposureMode=EdgePreSelect
            // LineSelector=CameraTrigger0
            // LineSource=SoftwareTrigger0

            // Set ExposureMode="EdgePreSelect"
            retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"ExposureMode", (int8_t*)"EdgePreSelect");
		    if(retval != J_ST_SUCCESS) 
		    {
                ShowErrorMsg(CString("Could not set ExposureMode=EdgePreSelect!"), retval);
			    return;
		    }

		    // Set LineSelector="CameraTrigger0"
            retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSelector", (int8_t*)"CameraTrigger0");
		    if(retval != J_ST_SUCCESS) 
		    {
                ShowErrorMsg(CString("Could not set LineSelector=CameraTrigger0!"), retval);
			    return;
		    }

		    // Set LineSource="SoftwareTrigger0" 
            retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSource", (int8_t*)"SoftwareTrigger0");
		    if(retval != J_ST_SUCCESS) 
		    {
                ShowErrorMsg(CString("Could not set LineSource=SoftwareTrigger0!"), retval);
			    return;
		    }
        }


        // Enable the GigE Events for the trigger

        // We have two possible ways of setting up GigE Events: JAI or GenICam SFNC
        // The GenICam SFNC GigE Events setup uses a node called "EventSelector".
        // Therefor we have to determine which way to use here.
        // First we see if a node called "EventSelector" exists.
        hNode = NULL;
        retval = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"EventSelector", &hNode);

        // Does the "EventSelector" node exist?
        if ((retval == J_ST_SUCCESS) && (hNode != NULL))
        {
            // Here we assume that this is JAI GigE Events setup so we do the following:
            // EventSelector=AcquisitionTrigger
            // EventNotification=On
            retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"EventSelector", (int8_t*)"AcquisitionTrigger");
		    if(retval != J_ST_SUCCESS) 
		    {
                ShowErrorMsg(CString("Could not set EventSelector=AcquisitionTrigger!"), retval);
			    return;
		    }
            retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"EventNotification", (int8_t*)"On");
		    if(retval != J_ST_SUCCESS) 
		    {
                retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"EventNotification", (int8_t*)"GigEVisionEvent");
		        if(retval != J_ST_SUCCESS) 
		        {
                    ShowErrorMsg(CString("Could not set EventNotification=On!"), retval);
			        return;
		        }
		    }
        }
        else
        {
            // Here we assume that this is JAI GigE Events setup so we do the following:
            // GevEventTrigger=true
            retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"GevEventTrigger", (int8_t*)"true");
		    if(retval != J_ST_SUCCESS) 
		    {
                ShowErrorMsg(CString("Could not set GevEventTrigger=true!"), retval);
			    return;
		    }
        }

        GetDlgItem(IDC_START_BUTTON)->EnableWindow();
        GetDlgItem(IDC_STOP_BUTTON)->EnableWindow(FALSE);
        GetDlgItem(IDC_TRIG_BUTTON)->EnableWindow(FALSE);

	} // end of if(m_hCamera)
}

void CGigEVisionEventSampleDlg::OnBnClickedStartButton()
{
	if(m_hCamera)
	{
        int64_t int64Val;
		POINT	StartPoint;
		SIZE	MaxViewerSize;
		J_STATUS_TYPE retval;

        // Get Width from the camera
        retval = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"Width", &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get Width!"), retval);
            return;
        }
		MaxViewerSize.cx = (LONG)int64Val;

        // Get Height from the camera
        retval = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"Height", &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get Height!"), retval);
            return;
        }
		MaxViewerSize.cy= (LONG)int64Val;
	
		StartPoint.x = 100;
		StartPoint.y = 100;

		retval = J_Image_OpenViewWindow(_T("GigE Image Viewer"), &StartPoint, &MaxViewerSize, &m_hView);
		if(retval != J_ST_SUCCESS) 
		{
            ShowErrorMsg(CString("Could not open view window!"), retval);
			return;
		}

		retval = J_Image_OpenStreamLight(m_hCamera, 0, &m_hThread);
		if(retval != J_ST_SUCCESS) 
		{
            ShowErrorMsg(CString("Could not open Stream Light!"), retval);
			return;
		}

	    // Execute AcquisitionStart command in Camera
        retval = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStart");
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
            return;
        }

        GetDlgItem(IDC_START_BUTTON)->EnableWindow(FALSE);
        GetDlgItem(IDC_STOP_BUTTON)->EnableWindow();
        GetDlgItem(IDC_TRIG_BUTTON)->EnableWindow();
	}
}

void CGigEVisionEventSampleDlg::OnBnClickedStopButton()
{
	J_STATUS_TYPE retval = J_ST_SUCCESS;

	// Execute AcquisitionStop command in Camera
    retval = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStop");
	if(retval != J_ST_SUCCESS) 
	{
        ShowErrorMsg(CString("Could not Stop Acquisition!"), retval);
		return;
	}
	
	// Close stream
	if(m_hThread)
	{
		retval = J_Image_CloseStream(m_hThread);
		if(retval != J_ST_SUCCESS) 
		{
            ShowErrorMsg(CString("Could not close Stream!"), retval);
			return;
		}

		m_hThread = NULL;
	}

	// Close view window
	if(m_hView)
	{
		retval = J_Image_CloseViewWindow(m_hView);
		if(retval != J_ST_SUCCESS) 
		{
            ShowErrorMsg(CString("Could not close View Window!"), retval);
			return;
		}

		m_hView = NULL;
	}

    GetDlgItem(IDC_START_BUTTON)->EnableWindow();
    GetDlgItem(IDC_STOP_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_TRIG_BUTTON)->EnableWindow(FALSE);
}

void CGigEVisionEventSampleDlg::OnBnClickedTrigButton()
{
	J_STATUS_TYPE retval = J_ST_SUCCESS;

    // We have two possible ways of software triggering: JAI or GenICam SFNC
    // The GenICam SFNC software trigger uses a node called "TriggerSoftware" and the JAI
    // uses nodes called "SoftwareTrigger0" to "SoftwareTrigger3".
    // Therefor we have to determine which way to use here.
    // First we see if a node called "TriggerSoftware" exists.
    NODE_HANDLE hNode = NULL;
    retval = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"TriggerSoftware", &hNode);

    // Does the "TriggerSoftware" node exist?
    if ((retval == J_ST_SUCCESS) && (hNode != NULL))
    {
        // Here we assume that this is GenICam SFNC software trigger so we do the following:
        // TriggerSelector=FrameStart
        // Execute TriggerSoftware Command
        retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
	    if(retval != J_ST_SUCCESS) 
	    {
            ShowErrorMsg(CString("Could not set TriggerSelector=FrameStart!"), retval);
		    return;
	    }
        retval = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"TriggerSoftware");
	    if(retval != J_ST_SUCCESS) 
	    {
            ShowErrorMsg(CString("Could not execute command TriggerSoftware!"), retval);
		    return;
	    }
    }
    else
    {
        // Here we assume that this is JAI software trigger so we do the following:
        // SoftwareTrigger0=0
        // SoftwareTrigger0=1
        // SoftwareTrigger0=0

        retval = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
	    if(retval != J_ST_SUCCESS) 
	    {
            ShowErrorMsg(CString("Could not set SoftwareTrigger0=0!"), retval);
		    return;
	    }

        retval = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 1);
	    if(retval != J_ST_SUCCESS) 
	    {
            ShowErrorMsg(CString("Could not set SoftwareTrigger0=1!"), retval);
		    return;
	    }

        retval = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
	    if(retval != J_ST_SUCCESS) 
	    {
            ShowErrorMsg(CString("Could not set SoftwareTrigger0=0!"), retval);
		    return;
	    }
    }
}

void CGigEVisionEventSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CGigEVisionEventSampleDlg::OnBnClickedOk()
{
	OnOK();
}
