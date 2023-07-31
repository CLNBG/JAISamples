// ZoomImageDisplaySampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ZoomImageDisplaySample.h"
#include "ZoomImageDisplaySampleDlg.h"
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

// CZoomImageDisplaySampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CZoomImageDisplaySampleDlg::CZoomImageDisplaySampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CZoomImageDisplaySampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hFactory = NULL;
	m_hCam = NULL;
	m_hView = NULL;
	m_hThread = NULL;
	m_LastImageInfo.pImageBuffer = NULL;
	m_ZoomRatio = 100;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;

	InitializeCriticalSection(&m_DiplayCriticalSection);
}

void CZoomImageDisplaySampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CZoomImageDisplaySampleDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_START, &CZoomImageDisplaySampleDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CZoomImageDisplaySampleDlg::OnBnClickedStop)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_ZOOM_IN_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedZoomInButton)
	ON_BN_CLICKED(IDC_ZOOM_OUT_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedZoomOutButton)
	ON_BN_CLICKED(IDC_RIGHT_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedRightButton)
	ON_BN_CLICKED(IDC_LEFT_UP_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedLeftUpButton)
	ON_BN_CLICKED(IDC_UP_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedUpButton)
	ON_BN_CLICKED(IDC_RIGHT_UP_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedRightUpButton)
	ON_BN_CLICKED(IDC_LEFT_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedLeftButton)
	ON_BN_CLICKED(IDC_CENTER_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedCenterButton)
	ON_BN_CLICKED(IDC_LEFT_DOWN_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedLeftDownButton)
	ON_BN_CLICKED(IDC_DOWN_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedDownButton)
	ON_BN_CLICKED(IDC_RIGHT_DOWN_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedRightDownButton)
	ON_BN_CLICKED(IDC_ZOOM_RESET_BUTTON, &CZoomImageDisplaySampleDlg::OnBnClickedZoomResetButton)
	ON_BN_CLICKED(IDCANCEL, &CZoomImageDisplaySampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CZoomImageDisplaySampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CZoomImageDisplaySampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CZoomImageDisplaySampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	BOOL retval;

	// Open factory & camera
	retval = OpenFactoryAndCamera();  
	if (retval)
	{
		GetDlgItem(IDC_CAMERAID)->SetWindowText(CString((char*)m_sCameraId));    // Display camera ID
		EnableControls(TRUE, FALSE);   // Enable Controls
	}
	else
	{
		GetDlgItem(IDC_CAMERAID)->SetWindowText(CString("error"));
		EnableControls(FALSE, FALSE);  // Enable Controls
	}
	InitializeControls();   // Initialize Controls

	return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CZoomImageDisplaySampleDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

	// Stop acquisition
	OnBnClickedStop();

	// Close factory & camera
	CloseFactoryAndCamera();

	DeleteCriticalSection(&m_DiplayCriticalSection);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CZoomImageDisplaySampleDlg::OnPaint()
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
HCURSOR CZoomImageDisplaySampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CZoomImageDisplaySampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CZoomImageDisplaySampleDlg::OpenFactoryAndCamera()
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
void CZoomImageDisplaySampleDlg::CloseFactoryAndCamera()
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
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CZoomImageDisplaySampleDlg::OnBnClickedStart()
{
	J_STATUS_TYPE   retval;
	int64_t int64Val;

	RECT	FrameRect;
	SIZE	MaxSize;

	if(!m_bEnableStreaming)
	{
		ShowErrorMsg(CString("Streaming not enabled on this device."), 0);
		return;
	}

	// Get Width from the camera
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
	MaxSize.cx = (LONG)int64Val;     // Set window size cx
	m_AcquisitionSize.cx = MaxSize.cx;

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

	// Get Height from the camera
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
	MaxSize.cy = (LONG)int64Val;     // Set window size cy
	m_AcquisitionSize.cy = MaxSize.cy;

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

	// Set window position
	FrameRect.top = 100;
	FrameRect.left = 50;
	FrameRect.right = FrameRect.left + MaxSize.cx;
	FrameRect.bottom = FrameRect.top + MaxSize.cy;

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
	retval = J_Image_OpenViewWindowEx(J_IVW_OVERLAPPED_STRETCH, _T("Image View Window"), &FrameRect, &MaxSize, NULL, &m_hView);
	if (retval != J_ST_SUCCESS) {
		AfxMessageBox(CString("Could not open view window!"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	TRACE("Opening view window succeeded\n");

	// Open stream
	retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CZoomImageDisplaySampleDlg::StreamCBFunc), &m_hThread, (MaxSize.cx*MaxSize.cy*bpp)/8);
	if (retval != J_ST_SUCCESS) {
		AfxMessageBox(CString("Could not open stream!"), MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	TRACE("Opening stream succeeded\n");

	// Start Acquisition
	retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);

	EnableControls(TRUE, TRUE);
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CZoomImageDisplaySampleDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
	memcpy(&m_LastImageInfo, pAqImageInfo, sizeof(J_tIMAGE_INFO));

	if(m_hView)
	{
		// Shows image
		J_Image_ShowImage(m_hView, pAqImageInfo);
	}
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CZoomImageDisplaySampleDlg::OnBnClickedStop()
{
	J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

	// Start Acquisition
	if (m_hCam) {
		retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);
	}

	if(m_hThread)
	{
		// Close stream
		retval = J_Image_CloseStream(m_hThread);
		m_hThread = NULL;
		TRACE("Closed stream\n");
	}

	if(m_hView)
	{
		// Close view window
		retval = J_Image_CloseViewWindow(m_hView);
		m_hView = NULL;
		TRACE("Closed view window\n");
	}

	EnableControls(TRUE, FALSE);
}
//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CZoomImageDisplaySampleDlg::InitializeControls()
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
		pSCtrl->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LBL_GAIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_GAIN)->ShowWindow(SW_HIDE);
	}
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CZoomImageDisplaySampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
	GetDlgItem(IDC_START)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
	GetDlgItem(IDC_SLIDER_WIDTH)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_WIDTH)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_SLIDER_HEIGHT)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_HEIGHT)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_SLIDER_GAIN)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
	GetDlgItem(IDC_GAIN)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnHScroll
//--------------------------------------------------------------------------------------------------
void CZoomImageDisplaySampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

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

		// Get Height Node
		retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_GAIN, int64Val);

		SetDlgItemInt(IDC_GAIN, iPos);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CZoomImageDisplaySampleDlg::ZoomIn()
{
	ZoomIn(1);
}

void CZoomImageDisplaySampleDlg::ZoomIn(int zoomSpeed)
{
	if ((zoomSpeed < 1) || (zoomSpeed > 100))
		return;

	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		POINT CenterPosition = CalculateCenter();

		m_ZoomRatio -= zoomSpeed;
		if (m_ZoomRatio < 1)
			m_ZoomRatio = 1;

		AdjustImageOffset(CenterPosition);

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::ZoomOut()
{
	ZoomOut(1);
}

void CZoomImageDisplaySampleDlg::ZoomOut(int zoomSpeed)
{
	if ((zoomSpeed < 1) || (zoomSpeed > 100))
		return;

	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		POINT CenterPosition = CalculateCenter();

		m_ZoomRatio += zoomSpeed;
		if (m_ZoomRatio > 100)
			m_ZoomRatio = 100;

		AdjustImageOffset(CenterPosition);

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::ZoomDirect(int zoomPosition)
{
	if ((zoomPosition < 1) || (zoomPosition > 100))
		return;

	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		POINT CenterPosition = CalculateCenter();

		m_ZoomRatio = zoomPosition;

		AdjustImageOffset(CenterPosition);

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::ZoomReset()
{
	// Remove zoom
	ZoomDirect(100);
}

void CZoomImageDisplaySampleDlg::AdjustImageOffset(POINT centerPosition)
{
	m_ImageOffset.x = centerPosition.x - (((m_ZoomRatio * m_AcquisitionSize.cx) / 100) / 2);
	if (m_ImageOffset.x < 0)
		m_ImageOffset.x = 0;
	if (m_ImageOffset.x > m_AcquisitionSize.cx)
		m_ImageOffset.x = m_AcquisitionSize.cx;

	m_ImageOffset.y = centerPosition.y - (((m_ZoomRatio * m_AcquisitionSize.cy) / 100) / 2);
	if (m_ImageOffset.y < 0)
		m_ImageOffset.y = 0;
	if (m_ImageOffset.y > m_AcquisitionSize.cy)
		m_ImageOffset.y = m_AcquisitionSize.cy;
}

POINT CZoomImageDisplaySampleDlg::CalculateCenter()
{
	POINT center;

	center.x = (((m_ZoomRatio * m_AcquisitionSize.cx) / 100) / 2) + m_ImageOffset.x;
	center.y = (((m_ZoomRatio * m_AcquisitionSize.cy) / 100) / 2) + m_ImageOffset.y;

	return center;
}

void CZoomImageDisplaySampleDlg::NavigateLeftUp()
{
	return NavigateLeftUp(1);
}

void CZoomImageDisplaySampleDlg::NavigateLeftUp(int stepSize)
{
	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		m_ImageOffset.x -= stepSize;
		if (m_ImageOffset.x < 0)
			m_ImageOffset.x = 0;
		m_ImageOffset.y -= stepSize;
		if (m_ImageOffset.y < 0)
			m_ImageOffset.y = 0;

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::NavigateUp()
{
	return NavigateUp(1);
}

void CZoomImageDisplaySampleDlg::NavigateUp(int stepSize)
{
	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		m_ImageOffset.y -= stepSize;
		if (m_ImageOffset.y < 0)
			m_ImageOffset.y = 0;

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::NavigateRightUp()
{
	return NavigateRightUp(1);
}

void CZoomImageDisplaySampleDlg::NavigateRightUp(int stepSize)
{
	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		m_ImageOffset.x += stepSize;
		//if (m_ImageOffset.x < 0)
		//    m_ImageOffset.x = 0;
		m_ImageOffset.y -= stepSize;
		if (m_ImageOffset.y < 0)
			m_ImageOffset.y = 0;

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::NavigateLeft()
{
	return NavigateLeft(1);
}

void CZoomImageDisplaySampleDlg::NavigateLeft(int stepSize)
{
	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		m_ImageOffset.x -= stepSize;
		if (m_ImageOffset.x < 0)
			m_ImageOffset.x = 0;

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}


void CZoomImageDisplaySampleDlg::NavigateCenter()
{
	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		POINT center;
		center.x = m_AcquisitionSize.cx/2;
		center.y = m_AcquisitionSize.cy/2;

		AdjustImageOffset(center);

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::NavigateRight()
{
	return NavigateRight(1);
}

void CZoomImageDisplaySampleDlg::NavigateRight(int stepSize)
{
	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		m_ImageOffset.x += stepSize;
		//if (m_ImageOffset.x < 0)
		//    m_ImageOffset.x = 0;

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::NavigateLeftDown()
{
	return NavigateLeftDown(1);
}

void CZoomImageDisplaySampleDlg::NavigateLeftDown(int stepSize)
{
	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		m_ImageOffset.x -= stepSize;
		if (m_ImageOffset.x < 0)
			m_ImageOffset.x = 0;
		m_ImageOffset.y += stepSize;
		//if (m_ImageOffset.y < 0)
		//    m_ImageOffset.y = 0;

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::NavigateDown()
{
	return NavigateDown(1);
}

void CZoomImageDisplaySampleDlg::NavigateDown(int stepSize)
{
	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		m_ImageOffset.y += stepSize;
		//if (m_ImageOffset.y < 0)
		//    m_ImageOffset.y = 0;

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}

void CZoomImageDisplaySampleDlg::NavigateRightDown()
{
	return NavigateRightDown(1);
}

void CZoomImageDisplaySampleDlg::NavigateRightDown(int stepSize)
{
	J_STATUS_TYPE   error = J_ST_SUCCESS;

	// We  might have an old copy though!?
	if (m_hView)
	{
		m_ImageOffset.x += stepSize;
		//if (m_ImageOffset.x < 0)
		//    m_ImageOffset.x = 0;
		m_ImageOffset.y += stepSize;
		//if (m_ImageOffset.y < 0)
		//    m_ImageOffset.y = 0;

		// Perform the zoom
		error = J_Image_SetImageOffset(m_hView, &m_ImageOffset, m_ZoomRatio);

		// Show the image in the view window
		EnterCriticalSection(&m_DiplayCriticalSection);
		if (m_LastImageInfo.pImageBuffer != NULL)
			error = J_Image_ShowImage(m_hView, &m_LastImageInfo);
		LeaveCriticalSection(&m_DiplayCriticalSection);
	}
}


void CZoomImageDisplaySampleDlg::OnBnClickedZoomInButton()
{
	ZoomIn();
}

void CZoomImageDisplaySampleDlg::OnBnClickedZoomOutButton()
{
	ZoomOut();
}

void CZoomImageDisplaySampleDlg::OnBnClickedZoomResetButton()
{
	ZoomReset();
}

void CZoomImageDisplaySampleDlg::OnBnClickedLeftUpButton()
{
	NavigateLeftUp();
}

void CZoomImageDisplaySampleDlg::OnBnClickedUpButton()
{
	NavigateUp();
}

void CZoomImageDisplaySampleDlg::OnBnClickedRightUpButton()
{
	NavigateRightUp();
}

void CZoomImageDisplaySampleDlg::OnBnClickedLeftButton()
{
	NavigateLeft();
}

void CZoomImageDisplaySampleDlg::OnBnClickedCenterButton()
{
	NavigateCenter();
}

void CZoomImageDisplaySampleDlg::OnBnClickedRightButton()
{
	NavigateRight();
}

void CZoomImageDisplaySampleDlg::OnBnClickedLeftDownButton()
{
	NavigateLeftDown();
}

void CZoomImageDisplaySampleDlg::OnBnClickedDownButton()
{
	NavigateDown();
}

void CZoomImageDisplaySampleDlg::OnBnClickedRightDownButton()
{
	NavigateRightDown();
}


void CZoomImageDisplaySampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CZoomImageDisplaySampleDlg::OnBnClickedOk()
{
	OnOK();
}
