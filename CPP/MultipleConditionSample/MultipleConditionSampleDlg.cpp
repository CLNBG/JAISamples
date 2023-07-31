// MultipleConditionSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MultipleConditionSample.h"
#include "MultipleConditionSampleDlg.h"

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


// CMultipleConditionSampleDlg dialog
//============================================================================
// Thread Process Caller
//============================================================================
DWORD ProcessCaller(CMultipleConditionSampleDlg* pThread)
{
    pThread->SupervisionProcess();

    return 0;
}

CMultipleConditionSampleDlg::CMultipleConditionSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CMultipleConditionSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // Initialize module variables
    m_hFactory = NULL;
    m_hCamera = NULL;
    m_hSupervisionThread = NULL;
    m_hGigEVisionEventCondition = NULL;
    m_hCameraConnectionCondition = NULL;
	m_hView		= NULL;
	m_hThread	= NULL;
    m_hDS = NULL;
	m_iValidBuffers = 0;
	m_bThreadRunning = false;
}

void CMultipleConditionSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EVENT_STATIC, m_StatusLabel);
    DDX_Control(pDX, IDC_EVENT_STATIC2, m_ConnectionStatusLabel);
}

BEGIN_MESSAGE_MAP(CMultipleConditionSampleDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_TRIG_BUTTON, &CMultipleConditionSampleDlg::OnBnClickedTrigButton)
	ON_BN_CLICKED(IDCANCEL, &CMultipleConditionSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CMultipleConditionSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMultipleConditionSampleDlg message handlers
BOOL CMultipleConditionSampleDlg::OnInitDialog()
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

    // Open factory & first camera found
    BOOL retval = OpenFactoryAndCamera();  

    // Get current time
    CTime now = CTime::GetCurrentTime();

    // Did we succeed opening a camera?
    if (retval)
    {
        // Update text with Event status
        m_StatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": No events received yet! Please start acquisition and trigger the camera to start receiving events"));

        // Update text with connection status
        m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Camera just connected"));

        // Now we set up a View Window to display the images in
        retval = PrepareImageViewWindow();

        // Create the supervision thread
        retval = CreateSupervisionThread();

	    // Execute AcquisitionStart command in Camera
        retval = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStart");
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
            return TRUE;
        }

        GetDlgItem(IDC_TRIG_BUTTON)->EnableWindow();
    }
    else
    {
        m_StatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Error connecting to camera"));
        m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Error connecting to camera"));
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMultipleConditionSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMultipleConditionSampleDlg::OnPaint()
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
HCURSOR CMultipleConditionSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}


void CMultipleConditionSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Close thread
    TerminateSupervisionThread();

    // Close factory & camera
    CloseFactoryAndCamera();
}

void CMultipleConditionSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CMultipleConditionSampleDlg::OpenFactoryAndCamera()
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
void CMultipleConditionSampleDlg::CloseFactoryAndCamera()
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
BOOL CMultipleConditionSampleDlg::CreateSupervisionThread(void)
{
    // Is it already created?
    if(m_hSupervisionThread)
        return FALSE;

    // Prepare for image capture by creating the Data Stream
    CreateDataStream();

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
BOOL CMultipleConditionSampleDlg::TerminateSupervisionThread(void)
{
    J_STATUS_TYPE   retval;

    // Reset the thread execution flag.
    m_bEnableThread = false;

    // Signal the thread to stop faster
    // Stop the image acquisition engine
	if(NULL != m_hDS)
	{
		retval = J_DataStream_StopAcquisition(m_hDS, ACQ_STOP_FLAG_KILL);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not stop DataStream acquisition!"), retval);
		}
	}

	if(NULL != m_hGigEVisionEventCondition)
	{
		retval = J_Event_ExitCondition(m_hGigEVisionEventCondition);
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

    CloseDataStream();
    return TRUE;
}

//==============================================================////
// GigE Vision Event and Camera Connection Processing Function
//==============================================================////
void CMultipleConditionSampleDlg::SupervisionProcess(void)
{
    J_STATUS_TYPE	              iResult;
    uint32_t                      iSize;
    J_EVENT_DATA_GEV_EVENT_CMD	  GigEEventInfo;
    J_EVENT_DATA_CONNECTION	      DataConnectionInfo;
    HANDLE                        ConditionList[3];
    BUF_HANDLE	                  iBufferID;
    uint32_t                      EventInfo = 0;
	uint64_t	                  iQueued = 0;

    // Create structure to be used for image display
    J_tIMAGE_INFO	tAqImageInfo = {0, 0, 0, 0, NULL, 0, 0, 0, 0, 0, 0};

    // Mark thread as running
    m_bThreadRunning = true;

    J_COND_WAIT_RESULT	WaitResult;

    // Create the condition used for signalling the new image event
    iResult = J_Event_CreateCondition(&m_hNewImageCondition);

    // Create the condition used for signalling the GigE Vision Event
    iResult = J_Event_CreateCondition(&m_hGigEVisionEventCondition);

    // Create the condition used for signalling the Camera Connection Event
    iResult = J_Event_CreateCondition(&m_hCameraConnectionCondition);

    // Set up the Condition List
    ConditionList[0] = m_hNewImageCondition;
    ConditionList[1] = m_hGigEVisionEventCondition;
    ConditionList[2] = m_hCameraConnectionCondition;

    EVT_HANDLE	hNewImageEvent = NULL;         // New Image event handle returned from DataStream
    EVT_HANDLE	hGigEVisionEvent = NULL;       // GigE Vision event handle returned from Transport Layer
    EVT_HANDLE	hCameraConnectionEvent = NULL; // Camera Connection event handle returned from Transport Layer

    // Register the event with the acquisition engine
    iResult = J_DataStream_RegisterEvent(m_hDS, EVENT_NEW_BUFFER, m_hNewImageCondition, &hNewImageEvent); 

    // Start image acquisition
    iResult = J_DataStream_StartAcquisition(m_hDS, ACQ_START_NEXT_IMAGE, 0 );

    // Register the GigE Vision event and Camera Connection event with the transport layer
    J_STATUS_TYPE statusEvent1 = J_Camera_RegisterEvent(m_hCamera, EVENT_GEV_EVENT_CMD, m_hGigEVisionEventCondition, &hGigEVisionEvent); 
    J_STATUS_TYPE statusEvent2 = J_Camera_RegisterEvent(m_hCamera, EVENT_CONNECTION, m_hCameraConnectionCondition, &hCameraConnectionEvent); 

	if(J_ST_SUCCESS == statusEvent1 && J_ST_SUCCESS == statusEvent2)
	{
		// Now we check if there are any pending events! If this is the case then we force it to be read right away
		EventInfo = 0;
		iSize = sizeof(EventInfo);
		iResult = J_Event_GetInfo(hGigEVisionEvent, EVENT_INFO_NUM_ENTRYS_IN_QUEUE, &EventInfo, &iSize);

		// Signal the event so the WaitForMultipleObjects will be activated right away
		if ((iResult == J_ST_SUCCESS) && (EventInfo > 0))
			J_Event_SignalCondition(m_hGigEVisionEventCondition);

		EventInfo = 0;
		iSize = sizeof(EventInfo);
		iResult = J_Event_GetInfo(hCameraConnectionEvent, EVENT_INFO_NUM_ENTRYS_IN_QUEUE, &EventInfo, &iSize);

		// Signal the event so the WaitForMultipleObjects will be activated right away
		if ((iResult == J_ST_SUCCESS) && (EventInfo > 0))
			J_Event_SignalCondition(m_hCameraConnectionCondition);

		OutputDebugString(_T(">>> Start Supervision Process Loop.\n"));

		uint64_t infoValue = 0;
		uint64_t jaiPixelFormat = 0;

		EVENT_NEW_BUFFER_DATA eventData;	// Strut for EventGetData

		while(m_bEnableThread)
		{
			// Wait for Buffer event (or kill event)
			iResult = J_Event_WaitForMultipleConditions(3, ConditionList, 1000, &WaitResult);

			// Did we get a new buffer event?
			if(J_COND_WAIT_SIGNAL == WaitResult)
			{
				// Get the Buffer Handle from the event
				iSize = (uint32_t)sizeof(void *);
				iResult = J_Event_GetData(hNewImageEvent, &eventData,  &iSize);

				iBufferID = eventData.BufferHandle;

				// Did we receive the event data?
				if (iResult == J_ST_SUCCESS)
				{
					// Fill in structure for image display
					// Get the pointer to the frame buffer.
					iSize = (uint32_t)sizeof (void *);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_BASE	, &(tAqImageInfo.pImageBuffer), &iSize);
					
					// Get the effective data size.
					iSize = (uint32_t)sizeof (uint32_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_SIZE	, &(tAqImageInfo.iImageSize), &iSize);

					// Get Pixel Format Type.
					iSize = (uint32_t)sizeof (uint32_t);
					// Get Pixel Format Type.
					iSize = sizeof(uint64_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_PIXELTYPE, &(infoValue), &iSize);
					if (GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
						tAqImageInfo.iPixelType = GVSP_PIX_MONO8;
					}

					//Convert the camera's pixel format value to one understood by the JAI SDK.
					iResult = J_Image_Get_PixelFormat(m_hCamera, infoValue, &jaiPixelFormat);

					tAqImageInfo.iPixelType = jaiPixelFormat;

					if (GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_Image_Get_PixelFormat in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
						tAqImageInfo.iPixelType = GVSP_PIX_MONO8;
					}

					// Get Frame Width.
					iSize = (uint32_t)sizeof (uint32_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_WIDTH, &(tAqImageInfo.iSizeX), &iSize);
					// Get Frame Height.
					iSize = (uint32_t)sizeof (uint32_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_HEIGHT, &(tAqImageInfo.iSizeY), &iSize);
					// Get Timestamp.
					iSize = (uint32_t)sizeof (uint64_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_TIMESTAMP, &(tAqImageInfo.iTimeStamp), &iSize);
					// Get # of missing packets in frame.
					iSize = (uint32_t)sizeof (uint32_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_NUM_PACKETS_MISSING, &(tAqImageInfo.iMissingPackets), &iSize);

					// Initialize number of valid buffers announced
					tAqImageInfo.iAnnouncedBuffers = m_iValidBuffers;

					// Get # of buffers queued
					iSize = sizeof(uint64_t);
					iResult = J_DataStream_GetStreamInfo(m_hDS, STREAM_INFO_CMD_NUMBER_OF_FRAMES_QUEUED, &iQueued, &iSize);
					tAqImageInfo.iQueuedBuffers = static_cast<uint32_t>(iQueued & 0x0ffffffffL);

					// Get X-offset.
					iSize = (uint32_t)sizeof (uint32_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_XOFFSET, &(tAqImageInfo.iOffsetX), &iSize);
					// Get Y-offset.
					iSize = (uint32_t)sizeof (uint32_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_YOFFSET, &(tAqImageInfo.iOffsetY), &iSize);
					// Get Block ID
					iSize = (uint32_t)sizeof (uint64_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_BLOCKID, &(tAqImageInfo.iBlockId), &iSize);

					if(m_bEnableThread)
					{
						// Display image
						if(m_hView)
						{
							// Shows image
							J_Image_ShowImage(m_hView, &tAqImageInfo);
						}
					}

					// Queue This Buffer Again for reuse in acquisition engine
					iResult = J_DataStream_QueueBuffer(m_hDS, iBufferID);
				}

				// Get # of frames awaiting delivery
				iSize = sizeof(uint64_t);
				iResult = J_DataStream_GetStreamInfo(m_hDS, STREAM_INFO_CMD_NUMBER_OF_FRAMES_AWAIT_DELIVERY, &iQueued, &iSize);
			}
			else if(J_COND_WAIT_SIGNAL_1 == WaitResult)
			{
				// Get the GigE Vision Event Info from the transport layer
				iSize = (uint32_t)sizeof(J_EVENT_DATA_GEV_EVENT_CMD);
				iResult = J_Event_GetData(hGigEVisionEvent, &GigEEventInfo,  &iSize);

				if (iResult == J_ST_SUCCESS)
				{
					// Update the status label
					CTime now = CTime::GetCurrentTime();
					CString eventString;
					eventString.Format(_T("%s: We got event ID=%d, Stream Ch=%d, Block ID=%d, Timestamp=%lld"), now.Format("%c"), GigEEventInfo.m_EventID, GigEEventInfo.m_StreamChannelIndex,GigEEventInfo.m_BlockID, GigEEventInfo.m_Timestamp);
					m_StatusLabel.SetWindowText(eventString);
				}

				// Now we check if there are any pending events! If this is the case then we force it to be read right away
				EventInfo = 0;
				iSize = sizeof(EventInfo);
				iResult = J_Event_GetInfo(hGigEVisionEvent, EVENT_INFO_NUM_ENTRYS_IN_QUEUE, &EventInfo, &iSize);

				// Signal the event so the WaitForMultipleObjects will be activated right away
				if ((iResult == J_ST_SUCCESS) && (EventInfo > 0))
					J_Event_SignalCondition(m_hGigEVisionEventCondition);
			}
			else if(J_COND_WAIT_SIGNAL_2 == WaitResult)
			{
				// Get the Connection Info from the event
				iSize = (uint32_t)sizeof(J_EVENT_DATA_CONNECTION);
				iResult = J_Event_GetData(hCameraConnectionEvent, &DataConnectionInfo,  &iSize);

				if (iResult == J_ST_SUCCESS)
				{
					// Update the status label
					CTime now = CTime::GetCurrentTime();
					switch(DataConnectionInfo.m_iConnectionState)
					{
					case J_EDC_CONNECTED:
						m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Camera just connected"));

						// If the Data Stream was originally opened then we restart the Acquisition inside the camera
						if ((m_hDS != NULL) && (m_hCamera != NULL))
						{
							// Start the streaming inside the camera
							iResult = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStart");
						}
						GetDlgItem(IDC_TRIG_BUTTON)->EnableWindow(TRUE);
						break;
					case J_EDC_DISCONNECTED:
						m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": Camera just disconnected"));
						GetDlgItem(IDC_TRIG_BUTTON)->EnableWindow(FALSE);
						break;
					case J_EDC_LOST_CONTROL:
						m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": We lost control of the camera"));
						break;
					case J_EDC_GOT_CONTROL: // Deprecated!!
						m_ConnectionStatusLabel.SetWindowText(now.Format(_T("%c")) + _T(": We regained control of the camera"));
						break;
					}
				}

				// Now we check if there are any pending events! If this is the case then we force it to be read right away
				EventInfo = 0;
				iSize = sizeof(EventInfo);
				iResult = J_Event_GetInfo(hCameraConnectionEvent, EVENT_INFO_NUM_ENTRYS_IN_QUEUE, &EventInfo, &iSize);

				// Signal the event so the WaitForMultipleObjects will be activated right away
				if ((iResult == J_ST_SUCCESS) && (EventInfo > 0))
					J_Event_SignalCondition(m_hCameraConnectionCondition);
			}
			else
			{
				switch(WaitResult)
				{
					// Kill event
				case	J_COND_WAIT_EXIT:
				case	J_COND_WAIT_EXIT_1:
				case	J_COND_WAIT_EXIT_2:
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

		// Unregister new buffer event with acquisition engine
		iResult = J_DataStream_UnRegisterEvent(m_hDS, EVENT_NEW_BUFFER); 

		// Free the event object
		if (hNewImageEvent != NULL)
		{
			iResult = J_Event_Close(hNewImageEvent);
			hNewImageEvent = NULL;
		}

		// Un-register the GigE Vision event with the transport layer
		iResult = J_Camera_UnRegisterEvent(m_hCamera, EVENT_GEV_EVENT_CMD); 

		// Free the event object
		if (hGigEVisionEvent != NULL)
		{
			J_Event_Close(hGigEVisionEvent);
			hGigEVisionEvent = NULL;
		}

		// Un-register the Camera Connection event with the transport layer
		iResult = J_Camera_UnRegisterEvent(m_hCamera, EVENT_CONNECTION); 

		// Free the event object
		if (hCameraConnectionEvent != NULL)
		{
			J_Event_Close(hCameraConnectionEvent);
			hCameraConnectionEvent = NULL;
		}
	
	}//if(J_ST_SUCCESS == statusEvent1 && J_ST_SUCCESS == statusEvent2)

	// Free the New Image Condition
	if (m_hNewImageCondition != NULL)
	{
		J_Event_CloseCondition(m_hNewImageCondition);
		m_hNewImageCondition = NULL;
	}

	// Free the GigE Vision Event Condition
	if (m_hGigEVisionEventCondition != NULL)
	{
		J_Event_CloseCondition(m_hGigEVisionEventCondition);
		m_hGigEVisionEventCondition = NULL;
	}

	// Free the Camera Connection Condition
	if (m_hCameraConnectionCondition != NULL)
	{
		J_Event_CloseCondition(m_hCameraConnectionCondition);
		m_hCameraConnectionCondition = NULL;
	}

    m_bThreadRunning = false;
}


//==============================================================////
// Prepare frame buffers
//==============================================================////
uint32_t CMultipleConditionSampleDlg::PrepareBuffer(int iBufferSize)
{
    int			i;

    m_iValidBuffers = 0;

    for(i = 0 ; i < NUM_OF_BUFFER ; i++)
    {
        // Make the buffer for one frame. 
        m_pAquBuffer[i] = new uint8_t[iBufferSize];

        // Announce the buffer pointer to the Acquisition engine.
        if(J_ST_SUCCESS != J_DataStream_AnnounceBuffer(m_hDS, m_pAquBuffer[i] ,iBufferSize , NULL, &(m_pAquBufferID[i])))
        {
            delete m_pAquBuffer[i];
            break;
        }

        // Queueing it.
        if(J_ST_SUCCESS != J_DataStream_QueueBuffer(m_hDS, m_pAquBufferID[i]))
        {
            delete m_pAquBuffer[i];
            break;
        }

        m_iValidBuffers++;
    }

    return m_iValidBuffers;
}

//==============================================================////
// Unprepare buffers
//==============================================================////
BOOL CMultipleConditionSampleDlg::UnPrepareBuffer(void)
{
    void		*pPrivate;
    void		*pBuffer;
    uint32_t	i;

	if(NULL == m_hDS)
	{
		return FALSE;
	}

    // Flush Queues
    J_DataStream_FlushQueue(m_hDS, ACQ_QUEUE_INPUT_TO_OUTPUT);
    J_DataStream_FlushQueue(m_hDS, ACQ_QUEUE_OUTPUT_DISCARD);

    for(i = 0 ; i < m_iValidBuffers ; i++)
    {
        // Remove the frame buffer from the Acquisition engine.
        J_DataStream_RevokeBuffer(m_hDS, m_pAquBufferID[i], &pBuffer , &pPrivate);

        delete m_pAquBuffer[i];
        m_pAquBuffer[i] = NULL;
        m_pAquBufferID[i] = 0;
    }

    m_iValidBuffers = 0;

    return TRUE;
}

BOOL CMultipleConditionSampleDlg::PrepareImageViewWindow(void)
{
	J_STATUS_TYPE retval;

    if(m_hCamera)
	{
        // Prepare View Window and calculate the buffer size
        int64_t int64Val;
		POINT	StartPoint;
		SIZE	MaxViewerSize;

        // Get Width from the camera
        retval = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"Width", &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get Width!"), retval);
            return FALSE;
        }
		MaxViewerSize.cx = (LONG)int64Val;

        // Get Height from the camera
        retval = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"Height", &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get Height!"), retval);
            return FALSE;
        }
		MaxViewerSize.cy= (LONG)int64Val;
	
        StartPoint.x = 100;
		StartPoint.y = 100;

		retval = J_Image_OpenViewWindow(_T("GigE Image Viewer"), &StartPoint, &MaxViewerSize, &m_hView);
		if(retval != J_ST_SUCCESS) 
		{
            ShowErrorMsg(CString("Could not open view window!"), retval);
			return FALSE;
		}
    }
	return TRUE;
}

void CMultipleConditionSampleDlg::PrepareSwTrigSetting()
{
	J_STATUS_TYPE retval;

    if(m_hCamera)
	{
		NODE_HANDLE hNode = NULL;
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

		// We have two possible ways of setting up triggers: JAI or GenICam SFNC
		// The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
		// called "TriggerSelector". Therefor we have to determine which way to use here.
		// First we see if a node called "TriggerSelector" exists.
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

        GetDlgItem(IDC_TRIG_BUTTON)->EnableWindow(FALSE);

	} // end of if(m_hCamera)
}

BOOL CMultipleConditionSampleDlg::CreateDataStream()
{
	J_STATUS_TYPE retval;
    int64_t int64Val;

    // Is the Data Stream already created?
    if(m_hDS != NULL)
        return TRUE;

    // Open the stream channel(GVSP)
    if(m_hDS == NULL)
    {
        retval = J_Camera_CreateDataStream(m_hCamera, 0, &m_hDS);

        if(   (retval != J_ST_SUCCESS)
           || (m_hDS == NULL))
        {
            if (retval != J_ST_SUCCESS)
            {
                ShowErrorMsg(CString("Could not create DataStream!"), retval);
            }

            return FALSE;
        }
    }

    // Get PayloadSize from the camera
    retval = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"PayloadSize", &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get PayloadSize!"), retval);
        return FALSE;
    }

	int iBufferSize = (int) int64Val;

    // Prepare the frame buffers (this announces the buffers to the acquisition engine)
    if(0 == PrepareBuffer(iBufferSize))
    {
        ShowErrorMsg(CString("PrepareBuffer failed!"), J_ST_ERROR);

        retval = J_DataStream_Close(m_hDS);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close DataStream!"), retval);
        }
        return FALSE;
    }

    return TRUE;
}

BOOL CMultipleConditionSampleDlg::CloseDataStream()
{
	J_STATUS_TYPE retval;

    // UnPrepare Buffers (this removed the buffers from the acquisition engine and frees buffers)
    UnPrepareBuffer();

    // Close Stream
    if(m_hDS)
    {
        retval = J_DataStream_Close(m_hDS);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close DataStream!"), retval);
        }
        m_hDS = NULL;
    }

    return TRUE;
}

void CMultipleConditionSampleDlg::OnBnClickedTrigButton()
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

void CMultipleConditionSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CMultipleConditionSampleDlg::OnBnClickedOk()
{
	OnOK();
}
