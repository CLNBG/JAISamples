// StreamThreadSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "StreamThreadSample.h"
#include "StreamThreadSampleDlg.h"
#include <VLib.h>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


//Utility function to set the frame grabber's width/height (if one is present in the system).
void SetFramegrabberValue(CAM_HANDLE hCam, int8_t* szName, int64_t int64Val, int8_t* sCameraId)
{
	//Set frame grabber value, if applicable
	DEV_HANDLE hDev = NULL; //If a frame grabber exists, it is at the GenTL "local device layer".
	J_STATUS_TYPE retval = J_Camera_GetLocalDeviceHandle(hCam, &hDev);
	if(J_ST_SUCCESS != retval)
		return;

	if(NULL == hDev)
		return;

	NODE_HANDLE hNode;
	retval = J_Camera_GetNodeByName(hDev, szName, &hNode);
	if(J_ST_SUCCESS != retval)
		return;

	retval = J_Node_SetValueInt64(hNode, false, int64Val);
	if(J_ST_SUCCESS != retval)
		return;

	//Special handling for Active Silicon CXP boards, which also has nodes prefixed
	//with "Incoming":
	std::string strTransportName((char*)sCameraId);
	if(std::string::npos != strTransportName.find("TLActiveSilicon"))
	{
		std::string strName((char*)szName);
		if(std::string::npos != strName.find("Width") 
			|| std::string::npos != strName.find("Height"))
		{
			std::string strIncoming = "Incoming" + strName;
			NODE_HANDLE hNodeIncoming;
			J_STATUS_TYPE retval = J_Camera_GetNodeByName(hDev, (int8_t*)strIncoming.c_str(), &hNodeIncoming);
			if (retval == J_ST_SUCCESS)
			{
				retval = J_Node_SetValueInt64(hNodeIncoming, false, int64Val);
			}
		}

	}//if(std::string::npos != strTransportName.find("TLActiveSilicon"))
}

//Utility function to set the frame grabber's pixel format (if one is present in the system).
void SetFramegrabberPixelFormat(CAM_HANDLE hCam, int8_t* szName, int64_t jaiPixelFormat, int8_t* sCameraId)
{
	DEV_HANDLE hDev = NULL; //If a frame grabber exists, it is at the GenTL "local device layer".
	J_STATUS_TYPE retval = J_Camera_GetLocalDeviceHandle(hCam, &hDev);
	if(J_ST_SUCCESS != retval)
		return;

	if(NULL == hDev)
		return;

	int8_t szJaiPixelFormatName[512];
	uint32_t iSize = 512;
	retval = J_Image_Get_PixelFormatName(hCam, jaiPixelFormat, szJaiPixelFormatName, iSize);
	if(J_ST_SUCCESS != retval)
		return;

	NODE_HANDLE hLocalDeviceNode = 0;
	retval = J_Camera_GetNodeByName(hDev, (int8_t *)"PixelFormat", &hLocalDeviceNode);
	if(J_ST_SUCCESS != retval)
		return;

	if(0 == hLocalDeviceNode)
		return;

	//NOTE: this may fail if the camera and/or frame grabber does not use the SFNC naming convention for pixel formats!
	//Check the camera and frame grabber for details.
	retval = J_Node_SetValueString(hLocalDeviceNode, false, szJaiPixelFormatName);
	if(J_ST_SUCCESS != retval)
		return;

	//Special handling for Active Silicon CXP boards, which also has nodes prefixed
	//with "Incoming":
	std::string strTransportName((char*)sCameraId);
	if(std::string::npos != strTransportName.find("TLActiveSilicon"))
	{
		std::string strIncoming = std::string("Incoming") + std::string((char*)szName);
		NODE_HANDLE hNodeIncoming;
		J_STATUS_TYPE retval = J_Camera_GetNodeByName(hDev, (int8_t*)strIncoming.c_str(), &hNodeIncoming);
		if (retval == J_ST_SUCCESS)
		{
			//NOTE: this may fail if the camera and/or frame grabber does not use the SFNC naming convention for pixel formats!
			//Check the camera and frame grabber for details.
			retval = J_Node_SetValueString(hNodeIncoming, false, szJaiPixelFormatName);
		}
	}
}

//============================================================================
// Thread Process Caller
//============================================================================
DWORD ProcessCaller(CStreamThreadSampleDlg* pThread)
{
    pThread->StreamProcess();

    return 0;
}

// CStreamThreadSampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CStreamThreadSampleDlg::CStreamThreadSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CStreamThreadSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // Initialize all handles
    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;

    m_hStreamThread = NULL;
    m_hStreamEvent = NULL;
    m_hDS = NULL;
    m_iStreamChannel = 0;
    m_hCondition = NULL;
    m_bStreamStarted = false;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;
}

void CStreamThreadSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStreamThreadSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CStreamThreadSampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CStreamThreadSampleDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
	ON_BN_CLICKED(IDCANCEL, &CStreamThreadSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CStreamThreadSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CStreamThreadSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CStreamThreadSampleDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    BOOL retval;

    // Open factory & camera
    retval = OpenFactoryAndCamera();  
    if (retval)
    {
        // Display camera ID
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString((char*)m_sCameraId));    

        // Enable Controls
        EnableControls(TRUE, FALSE);   
    }
    else
    {
        // Write error message in textbox
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString("Error opening factory and camera"));

        // Disable controls
        EnableControls(FALSE, FALSE);
    }
    InitializeControls();   // Initialize Controls

    return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CStreamThreadSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Stop acquisition
    OnBnClickedStop();

    // Close factory & camera
    CloseFactoryAndCamera();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CStreamThreadSampleDlg::OnPaint()
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
HCURSOR CStreamThreadSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CStreamThreadSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CStreamThreadSampleDlg::OpenFactoryAndCamera()
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
		ShowErrorMsg(CString("Could not get the number of cameras!"), retval);
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
void CStreamThreadSampleDlg::CloseFactoryAndCamera()
{
    J_STATUS_TYPE   retval;
    // Any camera open?
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

    // Factory open?
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
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CStreamThreadSampleDlg::OnBnClickedStart()
{
    J_STATUS_TYPE   retval;
    int64_t int64Val;

    SIZE	ViewSize;
    POINT	TopLeft;

	if(!m_bEnableStreaming)
	{
		ShowErrorMsg(CString("Streaming not enabled on this device."), 0);
		return;
	}

    // Get Width from the camera based on GenICam name
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Width value!"), retval);
        return;
    }
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

    // Get Height from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Height value!"), retval);
        return;
    }
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

	// Get the pixelformat from the camera
	int64_t pixelFormat = 0;
	uint64_t jaiPixelFormat = 0;
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Unable to get PixelFormat value!"), retval);
		return;
	}
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

	//Set frame grabber pixel format, if applicable
	SetFramegrabberPixelFormat(m_hCam, NODE_NAME_PIXELFORMAT, pixelFormat, &m_sCameraId[0]);

	int64_t payloadSize = 0;
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PAYLOADSIZE, &payloadSize);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Unable to get PayloadSize value!"), retval);
		return;
	}

    // Set window position
    TopLeft.x = 100;
    TopLeft.y = 50;

    // Open view window
    retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window!"), retval);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Create image acquisition thread (this allocates buffers)
    CreateStreamThread(m_hCam, 0, (uint32_t)payloadSize);

    // Wait for the stream thread to start the acquisition
    while(!m_bStreamStarted)
        Sleep(10);

    TRACE("Opening stream succeeded\n");

    // Start Acquisition
    retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
        return;
    }

    // Enable controls
    EnableControls(TRUE, TRUE);
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CStreamThreadSampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

    // Stop Acquisition
    if (m_hCam && m_hDS) 
	{
        retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not Stop Acquisition!"), retval);
        }
    }

    // Close stream (this frees all allocated buffers)
    TerminateStreamThread();
    TRACE("Closed stream\n");

    // View window opened?
    if(m_hView)
    {
        // Close view window
        retval = J_Image_CloseViewWindow(m_hView);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close View Window!"), retval);
        }
        m_hView = NULL;
        TRACE("Closed view window\n");
    }

    EnableControls(TRUE, FALSE);
}

//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CStreamThreadSampleDlg::InitializeControls()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    int64_t int64Val;

    CSliderCtrl* pSCtrl;

    //- Width ------------------------------------------------

	// Get SliderCtrl for Width
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WIDTH);
	if(m_bEnableStreaming)
	{
		// Get Width Node
		retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_WIDTH, &hNode);
		if (retval == J_ST_SUCCESS)
		{
			// Get/Set Min
			retval = J_Node_GetMinInt64(hNode, &int64Val);
			pSCtrl->SetRangeMin((int)int64Val, TRUE);

			// Get/Set Max
			retval = J_Node_GetMaxInt64(hNode, &int64Val);
			pSCtrl->SetRangeMax((int)int64Val, TRUE);

			// Get/Set Value
			retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
			pSCtrl->SetPos((int)int64Val);

			SetDlgItemInt(IDC_WIDTH, (int)int64Val);

			retval = J_Node_GetInc(hNode, &m_iWidthInc);
			m_iWidthInc = max(1, m_iWidthInc);

			//Set frame grabber dimension, if applicable
			SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);
		}
		else
		{
			ShowErrorMsg(CString("Could not get Width node!"), retval);

			pSCtrl->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_WIDTH)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_WIDTH)->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		pSCtrl->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_WIDTH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_WIDTH)->ShowWindow(SW_HIDE);
	}

	//- Height -----------------------------------------------
	// Get SliderCtrl for Height
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT);

	if(m_bEnableStreaming)
	{
		// Get Height Node
		retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_HEIGHT, &hNode);
		if (retval == J_ST_SUCCESS && m_bEnableStreaming)
		{
			// Get/Set Min
			retval = J_Node_GetMinInt64(hNode, &int64Val);
			pSCtrl->SetRangeMin((int)int64Val, TRUE);

			// Get/Set Max
			retval = J_Node_GetMaxInt64(hNode, &int64Val);
			pSCtrl->SetRangeMax((int)int64Val, TRUE);

			// Get/Set Value
			retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
			pSCtrl->SetPos((int)int64Val);

			SetDlgItemInt(IDC_HEIGHT, (int)int64Val);

			retval = J_Node_GetInc(hNode, &m_iHeightInc);
			m_iHeightInc = max(1, m_iHeightInc);

			//Set frame grabber dimension, if applicable
			SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);
		}
		else
		{
			ShowErrorMsg(CString("Could not get Height node!"), retval);

			pSCtrl->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_HEIGHT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_HEIGHT)->ShowWindow(SW_HIDE);
		}
	}
	else
	{
		pSCtrl->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_HEIGHT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_HEIGHT)->ShowWindow(SW_HIDE);
	}

    //- Gain -----------------------------------------------

    // Get SliderCtrl for Gain
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);

    // Get Gain Node
    retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_GAIN, &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Min
        retval = J_Node_GetMinInt64(hNode, &int64Val);
        pSCtrl->SetRangeMin((int)int64Val, TRUE);

        // Get/Set Max
        retval = J_Node_GetMaxInt64(hNode, &int64Val);
        pSCtrl->SetRangeMax((int)int64Val, TRUE);

        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
        pSCtrl->SetPos((int)int64Val);

        SetDlgItemInt(IDC_GAIN, (int)int64Val);
    }
    else
    {
        //ShowErrorMsg(CString("Could not get Gain node!"), retval);

        pSCtrl->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_LBL_GAIN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_GAIN)->ShowWindow(SW_HIDE);
    }
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CStreamThreadSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
	GetDlgItem(IDC_START)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_SLIDER_WIDTH)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_WIDTH)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_HEIGHT)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_HEIGHT)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_GAIN)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
    GetDlgItem(IDC_GAIN)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnHScroll
//--------------------------------------------------------------------------------------------------
void CStreamThreadSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // Get SliderCtrl for Width
    CSliderCtrl* pSCtrl;
    int iPos;
    J_STATUS_TYPE   retval;
    int64_t int64Val;

    //- Width ------------------------------------------------

	// Get SliderCtrl for Width
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WIDTH);
	if (pSCtrl == (CSliderCtrl*)pScrollBar) {

		iPos = pSCtrl->GetPos();
		int64Val = (int64_t)iPos;

		//To avoid J_Camera_SetValueInt64 errors,
		//make the slider value be an even multiple of the increment + min value
		int64_t inc = m_iWidthInc;
		if(inc > 1)
		{
			int64_t iMin = pSCtrl->GetRangeMin();

			int64_t diff = int64Val - iMin;

			ldiv_t divResult = div((long)diff, ( long)inc);
			int64Val = iMin + (divResult.quot * inc);

			if(iPos != (int)int64Val)
			{
				pSCtrl->SetPos((int)int64Val);
			}
		}

		// Set Width value
		retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_WIDTH, int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not set Width!"), retval);
		}

		//Set frame grabber dimension, if applicable
		SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

		SetDlgItemInt(IDC_WIDTH, iPos);
	}

	//- Height -----------------------------------------------

	// Get SliderCtrl for Height
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT);
	if (pSCtrl == (CSliderCtrl*)pScrollBar) {

		iPos = pSCtrl->GetPos();
		int64Val = (int64_t)iPos;

		//To avoid J_Camera_SetValueInt64 errors,
		//make the slider value be an even multiple of the increment + min value
		int64_t inc = m_iHeightInc;
		if(inc > 1)
		{
			int64_t iMin = pSCtrl->GetRangeMin();

			int64_t diff = int64Val - iMin;

			ldiv_t divResult = div((long)diff, ( long)inc);
			int64Val = iMin + (divResult.quot * inc);

			if(iPos != (int)int64Val)
			{
				pSCtrl->SetPos((int)int64Val);
			}
		}

		// Set Height Value
		retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_HEIGHT, int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not set Height!"), retval);
		}

		//Set frame grabber dimension, if applicable
		SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

		SetDlgItemInt(IDC_HEIGHT, iPos);
	}

    //- Gain -----------------------------------------------

    // Get SliderCtrl for Gain
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) {

        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Set Gain value
        retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_GAIN, int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not set Gain value!"), retval);
        }

        SetDlgItemInt(IDC_GAIN, iPos);
    }

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//--------------------------------------------------------------------------------------------------
// Create Stream Thread
//--------------------------------------------------------------------------------------------------
BOOL CStreamThreadSampleDlg::CreateStreamThread(CAM_HANDLE hCam, uint32_t iChannel, uint32_t iBufferSize)
{
    J_STATUS_TYPE   retval;

    // Is it already created?
    if(m_hStreamThread)
        return FALSE;

    // Open the stream channel(GVSP)
    if(m_hDS == NULL)
    {
        retval = J_Camera_CreateDataStream(hCam, iChannel, &m_hDS);

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

    // Prepare the frame buffers (this announces the buffers to the acquisition engine)
    if(0 == PrepareBuffer(iBufferSize))
    {
        ShowErrorMsg(CString("PrepareBuffer failed!"), retval);

        retval = J_DataStream_Close(m_hDS);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close DataStream!"), retval);
        }
        return FALSE;
    }

    // Stream thread event created?
    if(m_hStreamEvent == NULL)
        m_hStreamEvent = CreateEvent(NULL, true, false, NULL);
    else
        ResetEvent(m_hStreamEvent);

    // Set the thread execution flag
    m_bEnableThread = true;

    // Create a Stream Thread.
    if(NULL == (m_hStreamThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProcessCaller, this, NULL, NULL)))
    {
        ShowErrorMsg(CString("CreateThread failed!"), retval);
        retval = J_DataStream_Close(m_hDS);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close DataStream!"), retval);
        }
        return FALSE;
    }

    return TRUE;
}

//==============================================================////
// Terminate Stream Thread
//==============================================================////
BOOL CStreamThreadSampleDlg::TerminateStreamThread(void)
{
    J_STATUS_TYPE   retval;

    // Is the data stream opened?
    if(m_hDS == NULL)
        return FALSE;

    // Reset the thread execution flag.
    m_bEnableThread = false;

    // Signal the image thread to stop faster
    if (m_hCondition)
    {
        retval = J_Event_ExitCondition(m_hCondition);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not exit Condition!"), retval);
        }
    }

    // Stop the image acquisition engine
    retval = J_DataStream_StopAcquisition(m_hDS, ACQ_STOP_FLAG_KILL);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not stop DataStream acquisition!"), retval);
    }

    // Mark stream acquisition as stopped
    m_bStreamStarted = false;

    // Wait for the thread to end
    WaitForThreadToTerminate();

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

//==============================================================////
// Stream Processing Function
//==============================================================////
void CStreamThreadSampleDlg::StreamProcess(void)
{
    J_STATUS_TYPE	iResult;
    uint32_t        iSize;
    BUF_HANDLE	    iBufferID;
	uint64_t	    iQueued = 0;

    // Create structure to be used for image display
    J_tIMAGE_INFO	tAqImageInfo = {0, 0, 0, 0, NULL, 0, 0, 0, 0, 0, 0};

    J_COND_WAIT_RESULT	WaitResult;

    // Create the condition used for signaling the new image event
    iResult = J_Event_CreateCondition(&m_hCondition);

    EVT_HANDLE	hEvent;					// Buffer event handle
	EVENT_NEW_BUFFER_DATA eventData;	// Strut for EventGetData

	// YT: 2018/08/31 Added
	// iImageStatus was added in J_tIMAGE_INFO structure at JAI SDK 3.0.2.3.
	//
	// It will be used for VisionLib to know whether or not a image buffer associated with the
	// tAqImageInfo is allocated and whether or not the image buffer holds an acquisition data.
	// The iImageStatus control codes must be implemented when J_Image_* functions are called in
	// this stream thread.
	//
	// The IMAGE_STATUS_NONE indicates that the tAqImageInfo has no image buffer here.
	tAqImageInfo.iImageStatus = IMAGE_STATUS_NONE;

	// Register the event with the acquisition engine
	uint32_t iAcquisitionFlag = ACQ_START_NEXT_IMAGE;

	iResult = J_DataStream_RegisterEvent(m_hDS, EVENT_NEW_BUFFER, m_hCondition, &hEvent); 

    // Start image acquisition
    iResult = J_DataStream_StartAcquisition(m_hDS, iAcquisitionFlag, ULLONG_MAX);

    // Mark stream acquisition as started
    m_bStreamStarted = true;

    // Loop of Stream Processing
    OutputDebugString(_T(">>> Start Stream Process Loop.\n"));

	const uint64_t iTimeout = 1000;
	uint64_t jaiPixelFormat = 0;
	uint64_t chunkLayoutIDValue = 0; // 0 means no Chunk
	uint64_t lastChunkLayoutID = 0;
	size_t iNumChunks = 0;
	size_t iLastNumChunks = 0;
	J_SINGLE_CHUNK_DATA* pChunkInfoArray = NULL;

    while(m_bEnableThread)
    {
        // Wait for Buffer event (or kill event)
		iResult = J_Event_WaitForCondition(m_hCondition, iTimeout, &WaitResult);
		if(J_ST_SUCCESS != iResult)
		{
			OutputDebugString(_T("J_Event_WaitForCondition Error.\n"));
		}

        // Did we get a new buffer event?
        if(J_COND_WAIT_SIGNAL == WaitResult)
        {
            // Get the Buffer Handle from the event
            iSize = (uint32_t)sizeof(EVENT_NEW_BUFFER_DATA);

			iResult = J_Event_GetData(hEvent, &eventData,  &iSize);
			if(J_ST_SUCCESS != iResult)
			{
				OutputDebugString(_T("J_Event_GetData Error.\n"));
			}

			iBufferID = eventData.BufferHandle;

            // Did we receive the event data?
            if (iResult == J_ST_SUCCESS)
            {
				// YT: 2018/08/31 Added
				// The ISTATUS_EMPTY indicates that the tAqImageInfo has an empty image buffer here.
				tAqImageInfo.iImageStatus = ISTATUS_EMPTY;

                // Fill in structure for image display

                // Get the pointer to the frame buffer.
				uint64_t infoValue = 0;
                iSize = (uint32_t)sizeof (void *);
                iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_BASE, &(infoValue), &iSize);
				tAqImageInfo.pImageBuffer = (uint8_t*)infoValue;
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_BASE.\n"));
					continue;
				}
                
				// Get the effective data size.
				iSize = sizeof (size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_SIZE	, &(infoValue), &iSize);
				tAqImageInfo.iImageSize = (uint32_t)infoValue;
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_SIZE.\n"));
					continue;
				}

				// Get Frame Width.
				iSize = sizeof (size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_WIDTH	, &(infoValue), &iSize);
				tAqImageInfo.iSizeX = (uint32_t)infoValue;
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_WIDTH.\n"));
					continue;
				}
                
				// Get Frame Height.
				iSize = sizeof (size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_HEIGHT	, &(infoValue), &iSize);
				tAqImageInfo.iSizeY = (uint32_t)infoValue;
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_HEIGHT.\n"));
					continue;
				}
                
				// Get Pixel Format Type.
				try
				{
					iSize = sizeof (uint64_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_PIXELTYPE, &(infoValue), &iSize);
					if(GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
						tAqImageInfo.iPixelType = GVSP_PIX_MONO8;
					}
					
					//Convert the camera's pixel format value to one understood by the JAI SDK.
					iResult = J_Image_Get_PixelFormat(m_hCam, infoValue, &jaiPixelFormat);

					tAqImageInfo.iPixelType = jaiPixelFormat;

					if(GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_Image_Get_PixelFormat in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
						tAqImageInfo.iPixelType = GVSP_PIX_MONO8;
					}
				}
				catch (...)
				{
					OutputDebugString(_T("Exception with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
					tAqImageInfo.iPixelType = GVSP_PIX_MONO8;
				}

				// Get Timestamp.
				try
				{
					iSize = sizeof (infoValue);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_TIMESTAMP, &(infoValue), &iSize);
					tAqImageInfo.iTimeStamp = infoValue;
					if(GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_TIMESTAMP.\n"));
						//continue;
					}
				}
				catch(...)
				{
					OutputDebugString(_T("Exception with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_TIMESTAMP.\n"));
				}
                
				// Get # of missing packets in frame.
				iSize = (uint32_t)sizeof (uint32_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_NUM_PACKETS_MISSING, &(tAqImageInfo.iMissingPackets), &iSize);                

                // Initialize number of valid buffers announced
                tAqImageInfo.iAnnouncedBuffers = m_iValidBuffers;

                // Get # of buffers queued
				iSize = sizeof (infoValue);
				iResult = J_DataStream_GetStreamInfo(m_hDS, STREAM_INFO_CMD_NUMBER_OF_FRAMES_QUEUED, &iQueued, &iSize);
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> STREAM_INFO_NUM_QUEUED.\n"));
					//continue;
				}
				tAqImageInfo.iQueuedBuffers = static_cast<uint32_t>(iQueued & 0x0ffffffffL);

                // Get X-offset.
				try
				{
					iSize = sizeof (size_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_XOFFSET, &infoValue, &iSize);
					tAqImageInfo.iOffsetX = (uint32_t)(infoValue);
					if(GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_XOFFSET.\n"));
						//continue;
					}
				}
				catch(...)
				{
					OutputDebugString(_T("Exception with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_XOFFSET.\n"));
				}

                
				// Get Y-offset.
				try
				{
					iSize = sizeof (size_t);
					uint64_t infoValue = 0;
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_YOFFSET, &infoValue, &iSize);
					tAqImageInfo.iOffsetY = (uint32_t)(infoValue);
					if(GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_YOFFSET.\n"));
						//continue;
					}
				}
				catch(...)
				{
					OutputDebugString(_T("Exception with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_YOFFSET.\n"));
					//continue;
				}
                
				// YT: 2018/08/31 Added
				// iPaddingX was added in J_tIMAGE_INFO structure at JAI SDK 3.0.2.3.
				// Get X-padding (number of extra bytes provided at the end of each line)
				try
				{
					iSize = sizeof (size_t);
					uint64_t infoValue = 0;
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_XPADDING, &infoValue, &iSize);
					tAqImageInfo.iPaddingX = (uint32_t)(infoValue);
					if(GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_XPADDING.\n"));
						//continue;
					}
				}
				catch(...)
				{
					OutputDebugString(_T("Exception with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_XPADDING.\n"));
					//continue;
				}
                
				// Get Block ID
				iSize = sizeof (uint64_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_BLOCKID, &(tAqImageInfo.iBlockId), &iSize);
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_BLOCKID.\n"));
					//continue;
				}

				// YT: 2017/08/31 Added
				// The IMAGE_STATUS_ACQUISITION indicates that the tAqImageInfo's image buffer holds an acquisition data here.
				tAqImageInfo.iImageStatus = IMAGE_STATUS_ACQUISITION;

				// Get Chunk Layout ID.
				chunkLayoutIDValue = 0;
				try
				{
					iSize = sizeof (chunkLayoutIDValue);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_CHUNKLAYOUTID, &(chunkLayoutIDValue), &iSize);
					if(GC_ERR_SUCCESS != iResult)
					{
						OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_CHUNKLAYOUTID.\n"));
						//continue;
					}
					else
					{
						// OK - we got Chunk Layout ID number so we can check whether it has changed!
						// If it changes then we will have to get the new Chunk Information from the datastream
						if (chunkLayoutIDValue != lastChunkLayoutID)
						{
							lastChunkLayoutID = chunkLayoutIDValue;
							
							// First we have to get the current number of chunks
							iNumChunks = 0;
							iResult = J_DataStream_GetBufferChunkData(m_hDS, iBufferID, NULL, &iNumChunks);
							if(GC_ERR_SUCCESS != iResult)
							{
								OutputDebugString(_T("Error with J_DataStream_GetBufferChunkData in CStreamThread::StreamProcess.\n"));
							}
							else
							{
								// Did the number of chunks change? 
								// Then we would have to re-allocate the Chunk Info structure
								if (iNumChunks != iLastNumChunks)
								{
									iLastNumChunks = iNumChunks;

									if (pChunkInfoArray != NULL)
										delete pChunkInfoArray;
									pChunkInfoArray = (J_SINGLE_CHUNK_DATA*) malloc (iNumChunks*sizeof(J_SINGLE_CHUNK_DATA));
								}

								iResult = J_DataStream_GetBufferChunkData(m_hDS, iBufferID, pChunkInfoArray, &iNumChunks);
								if(GC_ERR_SUCCESS != iResult)
								{
									OutputDebugString(_T("Error with J_DataStream_GetBufferChunkData in CStreamThread::StreamProcess.\n"));
								}
								else
								{
									// Now we have all the chunk information we need!
									iResult = J_Camera_AttachChunkData(m_hCam, tAqImageInfo.pImageBuffer, pChunkInfoArray, iNumChunks);
									if(GC_ERR_SUCCESS != iResult)
									{
										OutputDebugString(_T("Error with J_Camera_AttachChunkData in CStreamThread::StreamProcess.\n"));
									}
								}
							}
						}
						else
						{
							// OK - here we can assume that the chunk layout is unchanges so we only need to update the chunk data
							iResult = J_Camera_UpdateChunkData(m_hCam, tAqImageInfo.pImageBuffer);
							if(GC_ERR_SUCCESS != iResult)
							{
								OutputDebugString(_T("Error with J_Camera_UpdateChunkData in CStreamThread::StreamProcess.\n"));
							}
						}
					}
				}
				catch(...)
				{
					OutputDebugString(_T("Exception with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_TIMESTAMP.\n"));
				}

				// Just testing Chunk
				int64_t chunkDataValue = 0;
				if (chunkLayoutIDValue != 0) 
				{
					J_STATUS_TYPE err = J_ST_SUCCESS;

					// If this is "pure" chunk data then we probably need to get the Image information from the chunk data!
					if ((tAqImageInfo.iSizeX == 0) && (tAqImageInfo.iSizeY == 0))
					{
						err = J_Camera_GetValueInt64(m_hCam, (int8_t*)"ChunkWidth", &chunkDataValue);

						if (err == J_ST_SUCCESS)
						{
							tAqImageInfo.iSizeX = (uint32_t)chunkDataValue;
							//WCHAR buffer[100];
							//wsprintf(buffer, _T("Chunk Timestamp=%I64u\n"), chunkDataValue);
							//OutputDebugString(buffer);
						}

						err = J_Camera_GetValueInt64(m_hCam, (int8_t*)"ChunkHeight", &chunkDataValue);

						if (err == J_ST_SUCCESS)
						{
							tAqImageInfo.iSizeY = (uint32_t)chunkDataValue;
							//WCHAR buffer[100];
							//wsprintf(buffer, _T("Chunk Timestamp=%I64u\n"), chunkDataValue);
							//OutputDebugString(buffer);
						}

						err = J_Camera_GetValueInt64(m_hCam, (int8_t*)"ChunkHeight", &chunkDataValue);

						if (err == J_ST_SUCCESS)
						{
							tAqImageInfo.iSizeY = (uint32_t)chunkDataValue;
							//WCHAR buffer[100];
							//wsprintf(buffer, _T("Chunk Timestamp=%I64u\n"), chunkDataValue);
							//OutputDebugString(buffer);
						}

						err = J_Camera_GetValueInt64(m_hCam, (int8_t*)"ChunkOffsetX", &chunkDataValue);

						if (err == J_ST_SUCCESS)
						{
							tAqImageInfo.iOffsetX = (uint32_t)chunkDataValue;
							//WCHAR buffer[100];
							//wsprintf(buffer, _T("Chunk Timestamp=%I64u\n"), chunkDataValue);
							//OutputDebugString(buffer);
						}
						err = J_Camera_GetValueInt64(m_hCam, (int8_t*)"ChunkOffsetY", &chunkDataValue);

						if (err == J_ST_SUCCESS)
						{
							tAqImageInfo.iOffsetY = (uint32_t)chunkDataValue;
							//WCHAR buffer[100];
							//wsprintf(buffer, _T("Chunk Timestamp=%I64u\n"), chunkDataValue);
							//OutputDebugString(buffer);
						}
						err = J_Camera_GetValueInt64(m_hCam, (int8_t*)"ChunkPixelFormat", &chunkDataValue);

						if (err == J_ST_SUCCESS)
						{
							tAqImageInfo.iPixelType = chunkDataValue;
							//WCHAR buffer[100];
							//wsprintf(buffer, _T("Chunk Timestamp=%I64u\n"), chunkDataValue);
							//OutputDebugString(buffer);
						}
					}
				}

                if(m_bEnableThread)
                {
                    // Display image
                    if(m_hView)
                    {
                        // Shows image
                        J_Image_ShowImage(m_hView, &tAqImageInfo);
                    }
                }

                // YT: 2017/08/31 Added
                // The ISTATUS_EMPTY indicates that the tAqImageInfo has an empty image buffer here.
                // This is needed because we call the J_DataStream_QueueBuffer next.
                tAqImageInfo.iImageStatus = IMAGE_STATUS_EMPTY;

                // Queue This Buffer Again for reuse in acquisition engine
                iResult = J_DataStream_QueueBuffer(m_hDS, iBufferID);
            
			}//if (iResult == J_ST_SUCCESS)

            // Get # of frames awaiting delivery
            iSize = sizeof(size_t);
            iResult = J_DataStream_GetStreamInfo(m_hDS, STREAM_INFO_CMD_NUMBER_OF_FRAMES_AWAIT_DELIVERY, &iQueued, &iSize);
        }
        else
        {
            switch(WaitResult)
            {
                // Kill event
                case	J_COND_WAIT_EXIT:
					OutputDebugString(_T(">>> Wait error J_COND_WAIT_EXIT.\n"));
					iResult = 1;
					break;
                // Timeout
                case	J_COND_WAIT_TIMEOUT:
					OutputDebugString(_T(">>> Wait error J_COND_WAIT_TIMEOUT.\n"));
					iResult = 2;
					break;
                // Error event
                case	J_COND_WAIT_ERROR:
					OutputDebugString(_T(">>> Wait error J_COND_WAIT_ERROR.\n"));
					iResult = 3;
					break;
                // Unknown?
                default:
					OutputDebugString(_T(">>> Wait error Unknown.\n"));
					iResult = 4;
					break;
            }
        }
    }
    OutputDebugString(_T(">>> Terminated Stream Process Loop.\n"));

	// OK - chunk data might have been used so we want to detach any buffers and Chunk Info structures
	iResult = J_Camera_DetachChunkData(m_hCam);
	if(GenICam::Client::GC_ERR_SUCCESS != iResult)
	{
		OutputDebugString(_T("Error with J_Camera_DetachChunkData in CStreamThread::StreamProcess.\n"));
	}

    // Unregister new buffer event with acquisition engine
    iResult = J_DataStream_UnRegisterEvent(m_hDS, EVENT_NEW_BUFFER); 

    // Free the event object
    if (hEvent != NULL)
    {
        iResult = J_Event_Close(hEvent);
        hEvent = NULL;
    }

    // Terminate the thread.
    TerminateThread();

    // Free the Condition
    if (m_hCondition != NULL)
    {
        iResult = J_Event_CloseCondition(m_hCondition);
        m_hCondition = NULL;
    }

	// Free the Chunk Info data
	if (pChunkInfoArray != NULL)
	{
		delete pChunkInfoArray;
		pChunkInfoArray = NULL;
	}
}

//==============================================================////
// Terminate image acquisition thread
//==============================================================////
void CStreamThreadSampleDlg::TerminateThread(void)
{
    SetEvent(m_hStreamEvent);
}

//==============================================================////
// Wait for thread to terminate
//==============================================================////
void CStreamThreadSampleDlg::WaitForThreadToTerminate(void)
{
    WaitForSingleObject(m_hStreamEvent, INFINITE);

    // Close the thread handle and stream event handle
    CloseThreadHandle();
}

//==============================================================////
// Close handles and stream
//==============================================================////
void CStreamThreadSampleDlg::CloseThreadHandle(void)
{
    if(m_hStreamThread)
    {
        CloseHandle(m_hStreamThread);
        m_hStreamThread = NULL;
    }

    if(m_hStreamEvent)
    {
        CloseHandle(m_hStreamEvent);
        m_hStreamEvent = NULL;
    }
}

//==============================================================////
// Prepare frame buffers
//==============================================================////
uint32_t CStreamThreadSampleDlg::PrepareBuffer(int iBufferSize)
{
    J_STATUS_TYPE	iResult = J_ST_SUCCESS;
    int			i;

    m_iValidBuffers = 0;

    for(i = 0 ; i < NUM_OF_BUFFERS ; i++)
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
BOOL CStreamThreadSampleDlg::UnPrepareBuffer(void)
{
    void		*pPrivate;
    void		*pBuffer;
    uint32_t	i;

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

void CStreamThreadSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CStreamThreadSampleDlg::OnBnClickedOk()
{
	OnOK();
}
