// RedCompensationSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RedCompensationSample.h"
#include "RedCompensationSampleDlg.h"
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

// CRedCompensationSampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CRedCompensationSampleDlg::CRedCompensationSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRedCompensationSampleDlg::IDD, pParent)
	, m_iRThresh(250)
	, m_iGThresh(240)
	, m_iBThresh(220)
	, m_iRGain(100)
	, m_iGGain(50)
	, m_iBGain(50)
	, m_bFullScreen(FALSE)
	, m_bCompen(TRUE)
	, m_bSave(false)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;

    // Mark this buffer info as uninitialized so J_Image_Malloc() will be called when we receive the first image
	m_ImageBufferInfo.pImageBuffer = NULL;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;
}

void CRedCompensationSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RTHRESH, m_RThresh);
	DDX_Control(pDX, IDC_RTHRESHSPIN, m_RThreshSpin);
	DDX_Control(pDX, IDC_RGAIN, m_RGain);
	DDX_Control(pDX, IDC_GGAIN, m_GGain);
	DDX_Control(pDX, IDC_BGAIN, m_BGain);
	DDX_Control(pDX, IDC_RGAINSPIN, m_RGainSpin);
	DDX_Control(pDX, IDC_GGAINSPIN, m_GGainSpin);
	DDX_Control(pDX, IDC_BGAINSPIN, m_BGainSpin);
	DDX_Control(pDX, IDC_GTHRESHSPIN, m_GThreshSpin);
	DDX_Control(pDX, IDC_BTHRESHSPIN, m_BThreshSpin);
	DDX_Control(pDX, IDC_FULL, m_swFullScreen);
	DDX_Control(pDX, IDC_COMPEN, m_swCompen);
}

BEGIN_MESSAGE_MAP(CRedCompensationSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CRedCompensationSampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CRedCompensationSampleDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
	ON_NOTIFY(UDN_DELTAPOS, IDC_RTHRESHSPIN, &CRedCompensationSampleDlg::OnDeltaposRthreshspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_GTHRESHSPIN, &CRedCompensationSampleDlg::OnDeltaposGthreshspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BTHRESHSPIN, &CRedCompensationSampleDlg::OnDeltaposBthreshspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RGAINSPIN, &CRedCompensationSampleDlg::OnDeltaposRgainspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_GGAINSPIN, &CRedCompensationSampleDlg::OnDeltaposGgainspin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BGAINSPIN, &CRedCompensationSampleDlg::OnDeltaposBgainspin)
	ON_BN_CLICKED(IDC_FULL, &CRedCompensationSampleDlg::OnBnClickedFull)
	ON_BN_CLICKED(IDC_COMPEN, &CRedCompensationSampleDlg::OnBnClickedCompen)
	ON_BN_CLICKED(IDC_SAVE, &CRedCompensationSampleDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDCANCEL, &CRedCompensationSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CRedCompensationSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRedCompensationSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CRedCompensationSampleDlg::OnInitDialog()
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

    return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CRedCompensationSampleDlg::OnDestroy()
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

void CRedCompensationSampleDlg::OnPaint()
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
HCURSOR CRedCompensationSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CRedCompensationSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CRedCompensationSampleDlg::OpenFactoryAndCamera()
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
void CRedCompensationSampleDlg::CloseFactoryAndCamera()
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

    // Free the Image conversion buffer if it has been allocated
	if(m_ImageBufferInfo.pImageBuffer != NULL)
    {
		J_Image_Free(&m_ImageBufferInfo);
    	m_ImageBufferInfo.pImageBuffer = NULL;
    }
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CRedCompensationSampleDlg::OnBnClickedStart()
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

	m_iRThresh = static_cast<uint16_t>(m_RThreshSpin.GetPos32());
	m_iGThresh = static_cast<uint16_t>(m_GThreshSpin.GetPos32());
	m_iBThresh = static_cast<uint16_t>(m_BThreshSpin.GetPos32());

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
    retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CRedCompensationSampleDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
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

    EnableControls(TRUE, TRUE);
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CRedCompensationSampleDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
	if(m_hView)
	{
		//Convert the camera's pixel format value to one understood by the JAI SDK.
		uint64_t currentPixelFormat = pAqImageInfo->iPixelType;
		uint64_t jaiPixelFormat = 0;
		J_STATUS_TYPE iResult = J_Image_Get_PixelFormat(m_hCam, currentPixelFormat, &jaiPixelFormat);

		pAqImageInfo->iPixelType = jaiPixelFormat;

		if(GC_ERR_SUCCESS != iResult)
		{
			OutputDebugString(_T("Error with J_Image_Get_PixelFormat in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE.\n"));
			return;
		}

		// We need to allocate the conversion buffer once
		if(m_ImageBufferInfo.pImageBuffer == NULL)
		{
			iResult = J_Image_Malloc(pAqImageInfo, &m_ImageBufferInfo);
			if(GC_ERR_SUCCESS != iResult)
			{
				OutputDebugString(_T("Error with J_Image_Malloc in CStreamThread::StreamProcess.\n"));
				return;
			}
		}

		// Converts from RAW to internal image before applying the Red-compensation algorithm.
		iResult = J_Image_FromRawToImageEx(pAqImageInfo, &m_ImageBufferInfo, BAYER_STANDARD_MULTI);
		if(GC_ERR_SUCCESS != iResult)
		{
			OutputDebugString(_T("Error with J_Image_FromRawToImageEx in CStreamThread::StreamProcess.\n"));
			return;
		}

		if(m_bFullScreen)
		{	
			// Full Screen
			m_ColorCompen.NumOfROI = 1;
			m_ColorCompen.CompenParams[0].RectOfROI.left = 0;
			m_ColorCompen.CompenParams[0].RectOfROI.right = m_ImageBufferInfo.iSizeX - 1;
			m_ColorCompen.CompenParams[0].RectOfROI.top = 0;
			m_ColorCompen.CompenParams[0].RectOfROI.bottom = m_ImageBufferInfo.iSizeY - 1;

			// Threshold
			m_ColorCompen.CompenParams[0].RThreshold = m_iRThresh;
			m_ColorCompen.CompenParams[0].GThreshold = m_iGThresh;
			m_ColorCompen.CompenParams[0].BThreshold = m_iBThresh;

			// Compensation gain
			m_ColorCompen.CompenParams[0].RGain = 4096 * m_iRGain / 100;
			m_ColorCompen.CompenParams[0].GGain = 4096 * m_iGGain / 100;
			m_ColorCompen.CompenParams[0].BGain = 4096 * m_iBGain / 100;
		}
		else
		{	// Uses 3 ROI(windows)
			for(int i = 0 ; i < NUM_OF_COMPEN_PARAM ; i++)
			{
				// ROI
				m_ColorCompen.CompenParams[i].RectOfROI.left = m_ImageBufferInfo.iSizeX * i * 2 / 5;
				m_ColorCompen.CompenParams[i].RectOfROI.right = m_ImageBufferInfo.iSizeX * (i * 2 + 1) / 5 - 1;
				m_ColorCompen.CompenParams[i].RectOfROI.top = m_ImageBufferInfo.iSizeY * 2 / 5;
				m_ColorCompen.CompenParams[i].RectOfROI.bottom = m_ImageBufferInfo.iSizeY * 3 / 5 - 1;

				// Threshold
				m_ColorCompen.CompenParams[i].RThreshold = m_iRThresh;
				m_ColorCompen.CompenParams[i].GThreshold = m_iGThresh;
				m_ColorCompen.CompenParams[i].BThreshold = m_iBThresh;

				// Compensation gain
				m_ColorCompen.CompenParams[i].RGain = 4096 * m_iRGain / 100;	// 1.0 : 4096
				m_ColorCompen.CompenParams[i].GGain = 4096 * m_iGGain / 100;	// 1.0 : 4096
				m_ColorCompen.CompenParams[i].BGain = 4096 * m_iBGain / 100;	// 1.0 : 4096
			}
			m_ColorCompen.NumOfROI = NUM_OF_COMPEN_PARAM;
		}

		// Perform the Green-compensation
		if(m_bCompen)
		{
			J_Image_Processing(&m_ImageBufferInfo, RED_COMPENSATION, reinterpret_cast<void *>(&m_ColorCompen));
		}

		// Shows image
		J_Image_ShowImage(m_hView, &m_ImageBufferInfo);

		// Save Image
		if(m_bSave)
		{
			m_bSave = false;

			if(J_ST_SUCCESS != J_Image_SaveFile(&m_ImageBufferInfo, _T("CaptImage.tif")))
				OutputDebugString(_T("SaveFile Error\n"));
		}
	}
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CRedCompensationSampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

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

	if(m_ImageBufferInfo.pImageBuffer != NULL)
    {
		J_Image_Free(&m_ImageBufferInfo);
    	m_ImageBufferInfo.pImageBuffer = NULL;
    }

    EnableControls(TRUE, FALSE);
}
//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CRedCompensationSampleDlg::InitializeControls()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    int64_t int64Val;
    int64_t pixelFormat;

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

    // Get pixelformat from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get PixelFormat value!"), retval);
        return;
    }

	uint64_t jaiPixelFormat = 0;
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

    // Calculate number of bits (not bytes) per pixel using macro
    int bpp = J_BitsPerPixel(jaiPixelFormat);

	//////////////////////////////////////
	// Color compensation
	//
	// Threshold
	// Value of proper threshold
	//  Bayer 8bit or RGB 24bit
	//   R Threshold : 250
	//   G Threshold : 240
	//   B Threshold : 220
	//  Bayer 10bit or more or RGB 30bit or more
	//   R Threshold : 64000
	//   G Threshold : 61440
	//   B Threshold : 56320
	//
    if ((bpp == 8) || (bpp == 24))
    {
        m_RThreshSpin.SetRange32(0, 255);
        m_GThreshSpin.SetRange32(0, 255);
        m_BThreshSpin.SetRange32(0, 255);
    }
    else
    {
	    m_RThreshSpin.SetRange32(0, 65535);
        m_iRThresh *= 256;

	    m_GThreshSpin.SetRange32(0, 65535);
        m_iGThresh *= 256;

	    m_BThreshSpin.SetRange32(0, 65535);
        m_iBThresh *= 256;
    }

    m_RThreshSpin.SetPos32(m_iRThresh);
    m_GThreshSpin.SetPos32(m_iGThresh);
    m_BThreshSpin.SetPos32(m_iBThresh);

    // Red Compensation gain
    // Value of proper gain
    //  R Gain : 100 (%)
    //  G Gain :  50 (%)
    //  B Gain :  50 (%)
    m_RGainSpin.SetRange(0, 100);
    m_RGainSpin.SetPos(m_iRGain);

    m_GGainSpin.SetRange(0, 100);
    m_GGainSpin.SetPos(m_iGGain);

    m_BGainSpin.SetRange(0, 100);
    m_BGainSpin.SetPos(m_iBGain);

    // Full Screen
	if(m_bFullScreen)
		m_swFullScreen.SetCheck(BST_CHECKED);
	else
		m_swFullScreen.SetCheck(BST_UNCHECKED);

	// Compensation
	char buff[80];
	uint32_t iSize = sizeof(buff);
	retval = J_Camera_GetNodeByName(m_hCam, (int8_t*)"PixelFormat", &hNode);
	retval = J_Node_GetValueString(hNode, 0, (int8_t*)buff, &iSize);

	if (strstr(buff, "Bayer"))
	{
		m_bCompen = TRUE;
	}
	else
	{
		m_bCompen = FALSE;
	}

	m_swCompen.EnableWindow(m_bCompen);

	if(m_bCompen)
		m_swCompen.SetCheck(BST_CHECKED);
	else
		m_swCompen.SetCheck(BST_UNCHECKED);
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CRedCompensationSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
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
void CRedCompensationSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CRedCompensationSampleDlg::OnDeltaposRthreshspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_iRThresh = static_cast<uint16_t>(m_RThreshSpin.GetPos32());

	*pResult = 0;
}

void CRedCompensationSampleDlg::OnDeltaposGthreshspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_iGThresh = static_cast<uint16_t>(m_GThreshSpin.GetPos32());

	*pResult = 0;
}

void CRedCompensationSampleDlg::OnDeltaposBthreshspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_iBThresh = static_cast<uint16_t>(m_BThreshSpin.GetPos32());

	*pResult = 0;
}

void CRedCompensationSampleDlg::OnDeltaposRgainspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_iRGain = m_RGainSpin.GetPos();

	*pResult = 0;
}

void CRedCompensationSampleDlg::OnDeltaposGgainspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_iGGain = m_GGainSpin.GetPos();

	*pResult = 0;
}

void CRedCompensationSampleDlg::OnDeltaposBgainspin(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	m_iBGain = m_BGainSpin.GetPos();

	*pResult = 0;
}

void CRedCompensationSampleDlg::OnBnClickedFull()
{
	if(BST_CHECKED == m_swFullScreen.GetCheck())
		m_bFullScreen = TRUE;
	else
		m_bFullScreen = FALSE;
}

void CRedCompensationSampleDlg::OnBnClickedCompen()
{
	if(BST_CHECKED == m_swCompen.GetCheck())
		m_bCompen = TRUE;
	else
		m_bCompen = FALSE;
}

void CRedCompensationSampleDlg::OnBnClickedSave()
{
	m_bSave = true;
}

void CRedCompensationSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CRedCompensationSampleDlg::OnBnClickedOk()
{
	OnOK();
}
