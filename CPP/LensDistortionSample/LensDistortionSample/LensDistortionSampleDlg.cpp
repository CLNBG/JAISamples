// LensDistortionSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LensDistortionSample.h"
#include "LensDistortionSampleDlg.h"
#include <Math.h>
#include <omp.h>
#include <emmintrin.h>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_NUM_QUEUED_IMAGES 10	//May need adjusting for fast cameras

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

// CLensDistortionSampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CLensDistortionSampleDlg::CLensDistortionSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLensDistortionSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;
    m_bUndistort = false;
    m_bMono = true;

    m_ImageProcessingBuffer.pImageBuffer = NULL;
    m_ConversionBuffer.pImageBuffer = NULL;

    // Fill in the lens distortion values for now.
    // These values has to be calibrated so it fits the actual lens used in the system
    // Right here we just use some values that fits with 6 mm lens
    memset(&m_UndistortParameters,0,sizeof(J_tUNDISTORT_PARAMS));
    m_UndistortParameters.dCoeffK1 = -0.194;
    m_UndistortParameters.dCoeffK2 = 0.115;
    m_UndistortParameters.dCoeffK3 = 0.0; // Only used if it is a fish-eye lens
    m_UndistortParameters.dCoeffP1 = -0.000619;
    m_UndistortParameters.dCoeffP2 = 0.00115;
    m_UndistortParameters.iInterpolationType = INTERPOLATION_NONE;
    m_UndistortParameters.pMapCache = NULL; // IMPORTANT TO SET THIS TO NULL!!

    InitializeCriticalSection(&m_CriticalSection);
    InitializeCriticalSection(&m_UndistortionParameterCriticalSection);

    m_bCameraOpen = false;
    m_bAcquisitionRunning = false;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;
}

void CLensDistortionSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLensDistortionSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CLensDistortionSampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CLensDistortionSampleDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_UNDISTORT_CHECK, &CLensDistortionSampleDlg::OnBnClickedUndistortCheck)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_INTERPOLATION_CHECK, &CLensDistortionSampleDlg::OnBnClickedInterpolationCheck)
    ON_EN_CHANGE(IDC_P1_EDIT, &CLensDistortionSampleDlg::OnEnChangeP1Edit)
    ON_EN_CHANGE(IDC_OPTICAL_CENTER_X_EDIT, &CLensDistortionSampleDlg::OnEnChangeOpticalCenterXEdit)
    ON_EN_CHANGE(IDC_OPTICAL_CENTER_Y_EDIT, &CLensDistortionSampleDlg::OnEnChangeOpticalCenterYEdit)
    ON_EN_CHANGE(IDC_FOCAL_LENGTH_X_EDIT, &CLensDistortionSampleDlg::OnEnChangeFocalLengthXEdit)
    ON_EN_CHANGE(IDC_FOCAL_LENGTH_Y_EDIT, &CLensDistortionSampleDlg::OnEnChangeFocalLengthYEdit)
    ON_EN_CHANGE(IDC_K1_EDIT, &CLensDistortionSampleDlg::OnEnChangeK1Edit)
    ON_EN_CHANGE(IDC_K2_EDIT, &CLensDistortionSampleDlg::OnEnChangeK2Edit)
    ON_EN_CHANGE(IDC_K3_EDIT, &CLensDistortionSampleDlg::OnEnChangeK3Edit)
    ON_EN_CHANGE(IDC_P2_EDIT, &CLensDistortionSampleDlg::OnEnChangeP2Edit)
    ON_BN_CLICKED(IDC_APPLY_BUTTON, &CLensDistortionSampleDlg::OnBnClickedApplyButton)
	ON_BN_CLICKED(IDCANCEL, &CLensDistortionSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CLensDistortionSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLensDistortionSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CLensDistortionSampleDlg::OnInitDialog()
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
        m_bCameraOpen = true;
        InitializeControls();   // Initialize Controls
    }
    else
    {
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString("error"));
        m_bCameraOpen = false;
    }
    EnableControls();  // Enable Controls

    return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CLensDistortionSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here

    // Stop acquisition
    OnBnClickedStop();

    // Close factory & camera
    CloseFactoryAndCamera();

    DeleteCriticalSection(&m_CriticalSection);
    DeleteCriticalSection(&m_UndistortionParameterCriticalSection);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLensDistortionSampleDlg::OnPaint()
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
HCURSOR CLensDistortionSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CLensDistortionSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CLensDistortionSampleDlg::OpenFactoryAndCamera()
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
void CLensDistortionSampleDlg::CloseFactoryAndCamera()
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
void CLensDistortionSampleDlg::OnBnClickedStart()
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
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

	//Set frame grabber pixel format, if applicable
	SetFramegrabberPixelFormat(m_hCam, NODE_NAME_PIXELFORMAT, pixelFormat, &m_sCameraId[0]);

	// Calculate number of bits (not bytes) per pixel using macro
	int bpp = J_BitsPerPixel(jaiPixelFormat);

    // In order to optimize for speed then we know that the MONO8, MONO10, MONO12 and MONO16 pixel formats
    // will be able to be processed without further conversions. The rest (Bayer, RGB and "packed" Mono)
    // will have to be converted using J_Image_FromRawToImage() or J_Image_FromRawToDIB()!
    if (   (pixelFormat == J_GVSP_PIX_MONO8)
        || (pixelFormat == J_GVSP_PIX_MONO10)
        || (pixelFormat == J_GVSP_PIX_MONO12)
        || (pixelFormat == J_GVSP_PIX_MONO16))
    {
        m_bMono = true;
    }
    else
    {
        m_bMono = false;
    }

    // Open view window
    retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window!"), retval);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Open stream
    retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CLensDistortionSampleDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open stream!"), retval);
        return;
    }
    TRACE("Opening stream succeeded\n");

    // Start Acquision
    retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
        return;
    }
    
    m_bAcquisitionRunning = true;
    
    SetTimer(1, 100, NULL);

    EnableControls();
}

//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CLensDistortionSampleDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
    if(m_hView)
    {
        // Skip images if they start to queue up.
        if (pAqImageInfo->iAwaitDelivery > MAX_NUM_QUEUED_IMAGES)
            return;

	    // Allocate the buffer to hold converted the image
		if (m_ImageProcessingBuffer.pImageBuffer == NULL)
        {
            // If the image is a color image then we need to convert before the image processing part so we need to allocate
            // conversion buffer as well!
            if(!m_bMono)
            {
                // Allocate buffer to convert to "Image" format or "DIB" format
                // To allocate for Image format we use J_Image_Malloc() and later on call
                // J_Image_FromRawToImage()
                J_Image_Malloc(pAqImageInfo, &m_ConversionBuffer);
                // To allocate for DIB format we use J_Image_MallocDIB() and later on call
                // J_Image_FromRawToDIB()
                //J_Image_MallocDIB(pAqImageInfo, &m_ConversionBuffer);

                // Allocate the Lens Distortion Correction buffer
                J_Image_MallocEx(&m_ConversionBuffer, &m_ImageProcessingBuffer, PF_NONE);
            }
            else
            {
                // Allocate the Lens Distortion Correction buffer based on image buffer
                J_Image_MallocEx(pAqImageInfo, &m_ImageProcessingBuffer, PF_NONE);
            }

            // Initialize the internal undistortion map to improve processing speed
            J_Image_Processing(pAqImageInfo, LENS_DISTORTION_MAP_INIT, (void*)&m_UndistortParameters);
        }

        // Start stopwatch (just for info)
        m_StopWatch.Start();

        // Perform the lens distortion correction ?
        if (m_bUndistort)
        {
            // If the image is a color image then we need to convert before the image processing part!
            if(!m_bMono)
            {
                // First we need to convert the image into either 32-bit DIB or 48-bit "Image" format
                //J_Image_FromRawToDIBEx(pAqImageInfo, &m_ConversionBuffer, BAYER_STANDARD_MULTI);
                J_Image_FromRawToImageEx(pAqImageInfo, &m_ConversionBuffer, BAYER_STANDARD_MULTI);

                // Do the Lens Distortion Correction
                EnterCriticalSection(&m_UndistortionParameterCriticalSection);
                J_Image_ProcessingEx(&m_ConversionBuffer, &m_ImageProcessingBuffer, LENS_DISTORTION_MULTI, (void*)&m_UndistortParameters);
		        LeaveCriticalSection(&m_UndistortionParameterCriticalSection);
            }
            else
            {
                // Do the Lens Distortion Correction
                EnterCriticalSection(&m_UndistortionParameterCriticalSection);
                J_Image_ProcessingEx(pAqImageInfo, &m_ImageProcessingBuffer, LENS_DISTORTION_MULTI, (void*)&m_UndistortParameters);
		        LeaveCriticalSection(&m_UndistortionParameterCriticalSection);
            }

            // Shows undistorted image
            J_Image_ShowImage(m_hView, &m_ImageProcessingBuffer);
        }
        else
            // Shows image
            J_Image_ShowImage(m_hView, pAqImageInfo);

        m_StopWatch.Stop();

		EnterCriticalSection(&m_CriticalSection);
		m_dTimeSpan = m_StopWatch.GetAverageTime();
		LeaveCriticalSection(&m_CriticalSection);
    }
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CLensDistortionSampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

    // Stop Acquision
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

    // Free the internal undistortion map if it has been initialized
    J_Image_Processing(&m_ConversionBuffer, LENS_DISTORTION_MAP_FREE, (void*)&m_UndistortParameters);

	if (m_ImageProcessingBuffer.pImageBuffer)
        J_Image_Free(&m_ImageProcessingBuffer);

    m_ImageProcessingBuffer.pImageBuffer = NULL;

	if (m_ConversionBuffer.pImageBuffer)
        J_Image_Free(&m_ConversionBuffer);

    m_ConversionBuffer.pImageBuffer = NULL;

    m_StopWatch.Reset();

    m_bAcquisitionRunning = false;

    EnableControls();

    KillTimer(1);
}
//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CLensDistortionSampleDlg::InitializeControls()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    int64_t int64Val;
    CString valueString = _T("");

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

			EnterCriticalSection(&m_UndistortionParameterCriticalSection);
			// Here we initialize the Optical Center and Focal Lenght based on the image size!
			m_UndistortParameters.dFocalLengthX = (double) int64Val;
			m_UndistortParameters.dFocalLengthY = m_UndistortParameters.dFocalLengthX;
			m_UndistortParameters.dOpticalCenterX = m_UndistortParameters.dFocalLengthX/2.0;
			LeaveCriticalSection(&m_UndistortionParameterCriticalSection);

			valueString.Format(_T("%0.2f"), m_UndistortParameters.dOpticalCenterX);
			SetDlgItemText(IDC_OPTICAL_CENTER_X_EDIT, valueString);
			valueString.Format(_T("%0.2f"), m_UndistortParameters.dFocalLengthX);
			SetDlgItemText(IDC_FOCAL_LENGTH_X_EDIT, valueString);
			valueString.Format(_T("%0.2f"), m_UndistortParameters.dFocalLengthY);
			SetDlgItemText(IDC_FOCAL_LENGTH_Y_EDIT, valueString);
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

			SetDlgItemInt(IDC_HEIGHT, (int)int64Val);

			retval = J_Node_GetInc(hNode, &m_iHeightInc);
			m_iHeightInc = max(1, m_iHeightInc);

			//Set frame grabber dimension, if applicable
			SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

			EnterCriticalSection(&m_UndistortionParameterCriticalSection);
			// Here we initialize the Optical Center and Focal Lenght based on the image size!
			m_UndistortParameters.dOpticalCenterY = (double)int64Val/2.0;
			LeaveCriticalSection(&m_UndistortionParameterCriticalSection);

			valueString.Format(_T("%0.2f"), m_UndistortParameters.dOpticalCenterY);
			SetDlgItemText(IDC_OPTICAL_CENTER_Y_EDIT, valueString);
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

    valueString.Format(_T("%0.6f"), m_UndistortParameters.dCoeffK1);
    SetDlgItemText(IDC_K1_EDIT, valueString);
    valueString.Format(_T("%0.6f"), m_UndistortParameters.dCoeffK2);
    SetDlgItemText(IDC_K2_EDIT, valueString);
    valueString.Format(_T("%0.6f"), m_UndistortParameters.dCoeffK3);
    SetDlgItemText(IDC_K3_EDIT, valueString);
    valueString.Format(_T("%0.6f"), m_UndistortParameters.dCoeffP1);
    SetDlgItemText(IDC_P1_EDIT, valueString);
    valueString.Format(_T("%0.6f"), m_UndistortParameters.dCoeffP2);
    SetDlgItemText(IDC_P2_EDIT, valueString);

    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow(FALSE);
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CLensDistortionSampleDlg::EnableControls()
{
    GetDlgItem(IDC_START)->EnableWindow((m_bCameraOpen && m_bEnableStreaming) ? (m_bAcquisitionRunning ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_STOP)->EnableWindow((m_bCameraOpen && m_bEnableStreaming) ? (m_bAcquisitionRunning ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_SLIDER_WIDTH)->EnableWindow(m_bCameraOpen ? (m_bAcquisitionRunning ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_WIDTH)->EnableWindow(m_bCameraOpen ? (m_bAcquisitionRunning ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_HEIGHT)->EnableWindow(m_bCameraOpen ? (m_bAcquisitionRunning ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_HEIGHT)->EnableWindow(m_bCameraOpen ? (m_bAcquisitionRunning ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_GAIN)->EnableWindow(m_bCameraOpen ? TRUE : FALSE);
    GetDlgItem(IDC_GAIN)->EnableWindow(m_bCameraOpen ? TRUE : FALSE);

    GetDlgItem(IDC_OPTICAL_CENTER_X_EDIT)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_OPTICAL_CENTER_Y_EDIT)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_FOCAL_LENGTH_X_EDIT)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_FOCAL_LENGTH_Y_EDIT)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_K1_EDIT)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_K2_EDIT)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_K3_EDIT)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_P1_EDIT)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_P2_EDIT)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_INTERPOLATION_CHECK)->EnableWindow(m_bCameraOpen ? (m_bUndistort ? TRUE : FALSE) : FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnHScroll
//--------------------------------------------------------------------------------------------------
void CLensDistortionSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

void CLensDistortionSampleDlg::OnBnClickedUndistortCheck()
{
    if (((CButton*)GetDlgItem(IDC_UNDISTORT_CHECK))->GetCheck() == BST_CHECKED)
        m_bUndistort = true;
    else
        m_bUndistort = false;

    EnableControls();
}

void CLensDistortionSampleDlg::OnTimer(UINT_PTR nIDEvent)
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

void CLensDistortionSampleDlg::OnBnClickedInterpolationCheck()
{
    EnterCriticalSection(&m_UndistortionParameterCriticalSection);
    if (((CButton*)GetDlgItem(IDC_INTERPOLATION_CHECK))->GetCheck() == BST_CHECKED)
        m_UndistortParameters.iInterpolationType = INTERPOLATION_BILINEAR;
    else
        m_UndistortParameters.iInterpolationType = INTERPOLATION_NONE;
    LeaveCriticalSection(&m_UndistortionParameterCriticalSection);
}

void CLensDistortionSampleDlg::OnEnChangeP1Edit()
{
    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow();
}

void CLensDistortionSampleDlg::OnEnChangeOpticalCenterXEdit()
{
    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow();
}

void CLensDistortionSampleDlg::OnEnChangeOpticalCenterYEdit()
{
    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow();
}

void CLensDistortionSampleDlg::OnEnChangeFocalLengthXEdit()
{
    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow();
}

void CLensDistortionSampleDlg::OnEnChangeFocalLengthYEdit()
{
    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow();
}

void CLensDistortionSampleDlg::OnEnChangeK1Edit()
{
    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow();
}

void CLensDistortionSampleDlg::OnEnChangeK2Edit()
{
    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow();
}

void CLensDistortionSampleDlg::OnEnChangeK3Edit()
{
    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow();
}

void CLensDistortionSampleDlg::OnEnChangeP2Edit()
{
    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow();
}

void CLensDistortionSampleDlg::OnBnClickedApplyButton()
{
    CString valueString;

    EnterCriticalSection(&m_UndistortionParameterCriticalSection);
    GetDlgItemText(IDC_OPTICAL_CENTER_X_EDIT, valueString);
    _stscanf_s(valueString,_T("%lf"),&m_UndistortParameters.dOpticalCenterX);
    GetDlgItemText(IDC_OPTICAL_CENTER_Y_EDIT, valueString);
    _stscanf_s(valueString,_T("%lf"),&m_UndistortParameters.dOpticalCenterY);
    GetDlgItemText(IDC_FOCAL_LENGTH_X_EDIT, valueString);
    _stscanf_s(valueString,_T("%lf"),&m_UndistortParameters.dFocalLengthX);
    GetDlgItemText(IDC_FOCAL_LENGTH_Y_EDIT, valueString);
    _stscanf_s(valueString,_T("%lf"),&m_UndistortParameters.dFocalLengthY);
    GetDlgItemText(IDC_K1_EDIT, valueString);
    _stscanf_s(valueString,_T("%lf"),&m_UndistortParameters.dCoeffK1);
    GetDlgItemText(IDC_K2_EDIT, valueString);
    _stscanf_s(valueString,_T("%lf"),&m_UndistortParameters.dCoeffK2);
    GetDlgItemText(IDC_K3_EDIT, valueString);
    _stscanf_s(valueString,_T("%lf"),&m_UndistortParameters.dCoeffK3);
    GetDlgItemText(IDC_P1_EDIT, valueString);
    _stscanf_s(valueString,_T("%lf"),&m_UndistortParameters.dCoeffP1);
    GetDlgItemText(IDC_P2_EDIT, valueString);
    _stscanf_s(valueString,_T("%lf"),&m_UndistortParameters.dCoeffP2);

    // Re-initialize the internal undistortion map to improve processing speed
    J_Image_Processing(&m_ImageProcessingBuffer, LENS_DISTORTION_MAP_INIT, (void*)&m_UndistortParameters);
    LeaveCriticalSection(&m_UndistortionParameterCriticalSection);

    GetDlgItem(IDC_APPLY_BUTTON)->EnableWindow(FALSE);
}

void CLensDistortionSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CLensDistortionSampleDlg::OnBnClickedOk()
{
	OnOK();
}
