// TransformSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TransformSample.h"
#include "TransformSampleDlg.h"
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

// CTransformSampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CTransformSampleDlg::CTransformSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTransformSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;
    m_bFlipHorizontal = false;
    m_bFlipVertical = false;

    m_bRotate = false;
    m_RotateType = J_ROTATE_90_DEG_CW;
    m_dTimeSpan = 0.0;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;

    InitializeCriticalSection(&m_CriticalSection);
}

void CTransformSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTransformSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CTransformSampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CTransformSampleDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_FLIP_CHECK, &CTransformSampleDlg::OnBnClickedFlipCheck)
    ON_BN_CLICKED(IDC_MIRROR_CHECK, &CTransformSampleDlg::OnBnClickedMirrorCheck)
    ON_BN_CLICKED(IDC_ROTATION_0_DEG_RADIO, &CTransformSampleDlg::OnBnClickedRotation0DegRadio)
    ON_BN_CLICKED(IDC_ROTATION_90_DEG_CW_RADIO, &CTransformSampleDlg::OnBnClickedRotation90DegCwRadio)
    ON_BN_CLICKED(IDC_ROTATION_90_DEG_CCW_RADIO, &CTransformSampleDlg::OnBnClickedRotation90DegCcwRadio)
    ON_BN_CLICKED(IDC_ROTATION_180_DEG_RADIO, &CTransformSampleDlg::OnBnClickedRotation180DegRadio)
    ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CTransformSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CTransformSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CTransformSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CTransformSampleDlg::OnInitDialog()
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
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString((char*)m_sCameraId));    // Display camera ID
        EnableControls(TRUE, FALSE);   // Enable Controls

        InitializeControls();   // Initialize Controls
    }
    else
    {
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString("error"));
        EnableControls(FALSE, FALSE);  // Enable Controls
    }

	if(m_bEnableStreaming)
	{
		((CButton*)GetDlgItem(IDC_ROTATION_0_DEG_RADIO))->SetCheck(1);
	}
	else
	{
		GetDlgItem(IDC_FLIP_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_MIRROR_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_ROTATION_0_DEG_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_ROTATION_90_DEG_CW_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_ROTATION_90_DEG_CCW_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_ROTATION_180_DEG_RADIO)->EnableWindow(FALSE);
	}

    return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CTransformSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Stop acquisition
    OnBnClickedStop();

    // Close factory & camera
    CloseFactoryAndCamera();

    DeleteCriticalSection(&m_CriticalSection);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTransformSampleDlg::OnPaint()
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
HCURSOR CTransformSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CTransformSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CTransformSampleDlg::OpenFactoryAndCamera()
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
void CTransformSampleDlg::CloseFactoryAndCamera()
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

//--------------------------------------------------------------------------------------------------
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CTransformSampleDlg::OnBnClickedStart()
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

    // Get Width from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Width!"), retval);
        return;
    }
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

    // Get Height from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Height!"), retval);
        return;
    }
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

    // Set window position
    TopLeft.x = 100;
    TopLeft.y = 50;

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

	// Calculate number of bits (not bytes) per pixel using macro
	int bpp = J_BitsPerPixel(jaiPixelFormat);
    // Open view window
    retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window!"), retval);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Open stream
    retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CTransformSampleDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open stream!"), retval);
        return;
    }
    TRACE("Opening stream succeeded\n");

    // Start Acquisition
    retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
        return;
    }

    SetTimer(1, 100, NULL);
    EnableControls(TRUE, TRUE);
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CTransformSampleDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
    if(m_hView)
    {
        // For performance measurements only!
        m_StopWatch.Start();

        if (m_bFlipHorizontal) 
            J_Image_Flip(pAqImageInfo, J_FLIP_HORIZONTAL);

        if (m_bFlipVertical) 
            J_Image_Flip(pAqImageInfo, J_FLIP_VERTICAL);

        if (m_bRotate)
            J_Image_Rotate(pAqImageInfo, m_RotateType);

        m_StopWatch.Stop();

        EnterCriticalSection(&m_CriticalSection);
        m_dTimeSpan = m_StopWatch.GetAverageTime();
        LeaveCriticalSection(&m_CriticalSection);

        // Shows image
        J_Image_ShowImage(m_hView, pAqImageInfo);
    }
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CTransformSampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

    KillTimer(1);

    // Stop Acquisition
    if (m_hCam) {
        retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not Stop Acquisition!"), retval);
        }
    }

    if(m_hThread)
    {
        // Close stream
        retval = J_Image_CloseStream(m_hThread);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close Stream!"), retval);
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
            ShowErrorMsg(CString("Could not close View Window!"), retval);
        }
        m_hView = NULL;
        TRACE("Closed view window\n");
    }

    EnableControls(TRUE, FALSE);

    m_StopWatch.Reset();
}
//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CTransformSampleDlg::InitializeControls()
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
void CTransformSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
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
void CTransformSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
            ShowErrorMsg(CString("Could not set Gain!"), retval);
        }

        SetDlgItemInt(IDC_GAIN, iPos);
    }

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTransformSampleDlg::OnBnClickedFlipCheck()
{
    CButton* pButton = (CButton*)GetDlgItem(IDC_FLIP_CHECK);
    if (pButton->GetCheck())
        m_bFlipVertical = true;
    else
        m_bFlipVertical = false;

    m_StopWatch.Reset();
}

void CTransformSampleDlg::OnBnClickedMirrorCheck()
{
    CButton* pButton = (CButton*)GetDlgItem(IDC_MIRROR_CHECK);
    if (pButton->GetCheck())
        m_bFlipHorizontal = true;
    else
        m_bFlipHorizontal = false;

    m_StopWatch.Reset();
}

void CTransformSampleDlg::OnBnClickedRotation0DegRadio()
{
    m_bRotate = false;
    m_RotateType = J_ROTATE_90_DEG_CW;

    m_StopWatch.Reset();
}

void CTransformSampleDlg::OnBnClickedRotation90DegCwRadio()
{
    m_bRotate = true;
    m_RotateType = J_ROTATE_90_DEG_CW;

    m_StopWatch.Reset();
}

void CTransformSampleDlg::OnBnClickedRotation90DegCcwRadio()
{
    m_bRotate = true;
    m_RotateType = J_ROTATE_90_DEG_CCW;

    m_StopWatch.Reset();
}

void CTransformSampleDlg::OnBnClickedRotation180DegRadio()
{
    m_bRotate = true;
    m_RotateType = J_ROTATE_180_DEG;

    m_StopWatch.Reset();
}

void CTransformSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
    // Update the GUI with latest processing time value
    if (nIDEvent == 1)
    {
        CString TimeSpan = _T("");
        EnterCriticalSection(&m_CriticalSection);
        TimeSpan.Format(_T("%0.1f"), m_dTimeSpan);
        LeaveCriticalSection(&m_CriticalSection);
        SetDlgItemText(IDC_EDIT1, TimeSpan);
    }
    CDialog::OnTimer(nIDEvent);
}

void CTransformSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CTransformSampleDlg::OnBnClickedOk()
{
	OnOK();
}
