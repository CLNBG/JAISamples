// ReadPixelSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ReadPixelSample.h"
#include "ReadPixelSampleDlg.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CReadPixelSampleDlg dialog

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

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CReadPixelSampleDlg::CReadPixelSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CReadPixelSampleDlg::IDD, pParent)
, m_XPos(0)
, m_YPos(0)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;
    m_XPos = 100;
    m_YPos = 100;
    m_bPositionChanged = false;
    m_bStretchViewWindow = true;
    m_bEnableMouseZoom = true;
    m_bEnableMouseCursor = false;
    m_bEnableDoubleBuffering = false;
    m_bEnableViewWindowMoveEvents = true;
    m_bEnableViewWindowSizeEvents = true;
    m_bEnableMouseMoveEvents = true;
    m_bEnableMouseLButtonDownEvents = true;
    m_bEnableMouseLButtonUpEvents = true;
    m_bEnableMouseRButtonDownEvents = true;
    m_bEnableMouseRButtonUpEvents = true;
    m_bEnableZoomPosChangeEvents = true;
    m_bEnableAreaSelectionEvents = true;
    m_bEnableUserDrawImageEvents = true;
    m_bEnableUserDrawViewWindowEvents = true;

    m_ViewWindowEventText = "No View Window events received yet!";

    m_UserDrawImageEventText = "This text will be drawn as image overlay!";
    m_UserDrawViewWindowEventText = "This text will be drawn as View Window overlay!";

    m_bEnableUserDrawImageGrid = false;
    m_bEnableUserDrawViewWindowGrid = false;

    m_CnvImageInfo.pImageBuffer = NULL;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;

    // We use a critical section to protect the update of the pixel value display
    InitializeCriticalSection(&m_CriticalSection);

    // Initialize GDI+
    GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
}

void CReadPixelSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_VALUE_STATIC, m_ValueStaticCtrl);
}

BEGIN_MESSAGE_MAP(CReadPixelSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CReadPixelSampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CReadPixelSampleDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_STRETCHED_CHECK, &CReadPixelSampleDlg::OnBnClickedStretchedCheck)
    ON_BN_CLICKED(IDC_ENABLE_MOUSE_ZOOM_CHECK, &CReadPixelSampleDlg::OnBnClickedEnableMouseZoomCheck)
    ON_BN_CLICKED(IDC_ENABLE_MOUSE_CURSOR_CHECK, &CReadPixelSampleDlg::OnBnClickedEnableMouseCursorCheck)
    ON_BN_CLICKED(IDC_WINDOW_MOVE_EVENT_CHECK, &CReadPixelSampleDlg::OnBnClickedWindowMoveEventCheck)
    ON_BN_CLICKED(IDC_WINDOW_SIZE_EVENT_CHECK, &CReadPixelSampleDlg::OnBnClickedWindowSizeEventCheck)
    ON_BN_CLICKED(IDC_MOUSE_MOVE_EVENT_CHECK, &CReadPixelSampleDlg::OnBnClickedMouseMoveEventCheck)
    ON_BN_CLICKED(IDC_LBUTTON_DOWN_CHECK, &CReadPixelSampleDlg::OnBnClickedLbuttonDownCheck)
    ON_BN_CLICKED(IDC_LBUTTON_UP_CHECK, &CReadPixelSampleDlg::OnBnClickedLbuttonUpCheck)
    ON_BN_CLICKED(IDC_RBUTTON_DOWN_CHECK, &CReadPixelSampleDlg::OnBnClickedRbuttonDownCheck)
    ON_BN_CLICKED(IDC_RBUTTON_UP_CHECK, &CReadPixelSampleDlg::OnBnClickedRbuttonUpCheck)
    ON_BN_CLICKED(IDC_ZOOM_POS_EVENT_CHECK, &CReadPixelSampleDlg::OnBnClickedZoomPosEventCheck)
    ON_BN_CLICKED(IDC_AREA_EVENT_CHECK, &CReadPixelSampleDlg::OnBnClickedAreaEventCheck)
    ON_BN_CLICKED(IDC_USER_DRAW_IMAGE_EVENT_CHECK, &CReadPixelSampleDlg::OnBnClickedUserDrawImageEventCheck)
    ON_BN_CLICKED(IDC_USER_DRAW_VIEW_WINDOW_EVENT_CHECK, &CReadPixelSampleDlg::OnBnClickedUserDrawViewWindowEventCheck)
    ON_EN_CHANGE(IDC_USER_DRAW_IMAGE_TEXT_EDIT, &CReadPixelSampleDlg::OnEnChangeUserDrawImageTextEdit)
    ON_EN_CHANGE(IDC_USER_DRAW_VIEW_WINDOW_TEXT_EDIT, &CReadPixelSampleDlg::OnEnChangeUserDrawViewWindowTextEdit)
    ON_BN_CLICKED(IDC_ENABLE_DOUBLE_BUFFERING_CHECK, &CReadPixelSampleDlg::OnBnClickedEnableDoubleBufferingCheck)
    ON_BN_CLICKED(IDC_USER_DRAW_IMAGE_GRID_CHECK, &CReadPixelSampleDlg::OnBnClickedUserDrawImageGridCheck)
    ON_BN_CLICKED(IDC_USER_DRAW_VIEW_WINDOW_GRID_CHECK, &CReadPixelSampleDlg::OnBnClickedUserDrawViewWindowGridCheck)
	ON_BN_CLICKED(IDCANCEL, &CReadPixelSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CReadPixelSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CReadPixelSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CReadPixelSampleDlg::OnInitDialog()
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
    }
    else
    {
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString("error"));
        EnableControls(FALSE, FALSE);  // Enable Controls
    }
    InitializeControls();   // Initialize Controls

	if(!m_bEnableStreaming)
	{
		GetDlgItem(IDC_STRETCHED_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_STRETCHED_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENABLE_MOUSE_ZOOM_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENABLE_MOUSE_CURSOR_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_WINDOW_MOVE_EVENT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_WINDOW_SIZE_EVENT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_MOUSE_MOVE_EVENT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_LBUTTON_DOWN_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_LBUTTON_UP_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_RBUTTON_DOWN_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_RBUTTON_UP_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_ZOOM_POS_EVENT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_AREA_EVENT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER_DRAW_IMAGE_EVENT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER_DRAW_VIEW_WINDOW_EVENT_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER_DRAW_IMAGE_TEXT_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER_DRAW_VIEW_WINDOW_TEXT_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_ENABLE_DOUBLE_BUFFERING_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER_DRAW_IMAGE_GRID_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_USER_DRAW_VIEW_WINDOW_GRID_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_CAMERAID)->EnableWindow(FALSE);
	}

    EnterCriticalSection(&m_CriticalSection);
    m_PixelValue.PixelValueUnion.Mono16Type.Value = 0;
    m_PixelType = J_GVSP_PIX_MONO8;
    LeaveCriticalSection(&m_CriticalSection);

    return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CReadPixelSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Stop acquisition
    OnBnClickedStop();

    // Close factory & camera
    CloseFactoryAndCamera();

    DeleteCriticalSection(&m_CriticalSection);

    GdiplusShutdown(m_gdiplusToken);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CReadPixelSampleDlg::OnPaint()
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
HCURSOR CReadPixelSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CReadPixelSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CReadPixelSampleDlg::OpenFactoryAndCamera()
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
void CReadPixelSampleDlg::CloseFactoryAndCamera()
{
    J_STATUS_TYPE   retval;
    if (m_hCam)
    {
        // Close camera
        retval = J_Camera_Close(m_hCam);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close camera!"), retval);
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
            ShowErrorMsg(CString("Could not close factory!"), retval);
        }
        m_hFactory = NULL;
        TRACE("Closed factory\n");
    }
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CReadPixelSampleDlg::OnBnClickedStart()
{
    J_STATUS_TYPE   retval;
    int64_t int64Val;

    SIZE	ViewSize;
    RECT	ViewWindowRect;
    POINT	ViewWindowPos;

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
    ViewWindowPos.x = 100;
    ViewWindowPos.y = 50;
    ViewWindowRect.left = 100;
    ViewWindowRect.top = 50;
    ViewWindowRect.right = 100 + ViewSize.cx;
    ViewWindowRect.bottom = 50 + ViewSize.cy;

    // Open view window
    if (m_bStretchViewWindow)
    {
        retval = J_Image_OpenViewWindowEx(J_IVW_OVERLAPPED_STRETCH, _T("Image View Window"), &ViewWindowRect, &ViewSize, this->m_hWnd, &m_hView);
    }
    else
    {
        retval = J_Image_OpenViewWindowEx(J_IVW_OVERLAPPED, _T("Image View Window"), &ViewWindowRect, &ViewSize, this->m_hWnd, &m_hView);
    }

    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window!"), retval);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Assign the View Window notification callback
    retval = J_Image_SetViewWindowCallback(m_hView, reinterpret_cast<J_IVW_CALLBACK_OBJECT>(this), reinterpret_cast<J_IVW_CALLBACK_FUNCTION>(&CReadPixelSampleDlg::ViewWindowNotificationCallback));
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not set the view window notification callback!"), retval);
        return;
    }
    TRACE("View window notification callback assignment succeeded\n");

    // Set View Window options
    if (m_hView != NULL)
    {
        J_Image_SetViewWindowOption(m_hView, J_IVW_MOUSE_ZOOM, &m_bEnableMouseZoom);
        J_Image_SetViewWindowOption(m_hView, J_IVW_MOUSE_CURSOR, &m_bEnableMouseCursor);
        J_Image_SetViewWindowOption(m_hView, J_IVW_DOUBLE_BUFFERING, &m_bEnableDoubleBuffering);

        SetupViewWindowCallbackMask();
    }

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

    // Open stream
    retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CReadPixelSampleDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open stream!"), retval);
        return;
    }
    TRACE("Opening stream succeeded\n");

    // Start Acquisition
    retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
        return;
    }

    SetTimer(1, 100, NULL);

    EnableControls(TRUE, TRUE);
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CReadPixelSampleDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
    if(m_hView)
    {
        // Here we want to read a certain pixel value from the image:
        // In order to do so we need to convert the image from Raw to Image (to interpolate Bayer
        // if needed and unpack the Packed pixel formats as well)
 
        // Allocate the buffer to hold converted the image. (We only want to do this once for performance reasons)
        if (m_CnvImageInfo.pImageBuffer == NULL)
        {
            if (J_Image_Malloc(pAqImageInfo, &m_CnvImageInfo) != J_ST_SUCCESS)
                return;
        }

        if (m_CnvImageInfo.pImageBuffer != NULL)
        {
            // Convert the raw image to "normalized" image format
            if (J_Image_FromRawToImage(pAqImageInfo, &m_CnvImageInfo) == J_ST_SUCCESS)
            {
                // Now we can read the pixel value
                POINT pt;
                EnterCriticalSection(&m_CriticalSection);
                pt.x = m_XPos;
                pt.y = m_YPos;
                LeaveCriticalSection(&m_CriticalSection);

                PIXELVALUE pixelValue;

                if (J_Image_GetPixel(&m_CnvImageInfo, &pt, &pixelValue) == J_ST_SUCCESS)
                {
                    // Copy the pixel type and value so we can display it in the UI thread context
                    EnterCriticalSection(&m_CriticalSection);
                    m_PixelType = m_CnvImageInfo.iPixelType;
                    m_PixelValue = pixelValue;
                    LeaveCriticalSection(&m_CriticalSection);
                }
                else
                {
                    // Copy the pixel type and value so we can display it in the UI thread context
                    EnterCriticalSection(&m_CriticalSection);
                    m_PixelType = m_CnvImageInfo.iPixelType;
                    pixelValue.PixelValueUnion.BGR48Type.RValue = 0;
                    pixelValue.PixelValueUnion.BGR48Type.GValue = 0;
                    pixelValue.PixelValueUnion.BGR48Type.BValue = 0;
                    m_PixelValue = pixelValue;
                    LeaveCriticalSection(&m_CriticalSection);
                }
            }
        }
 
        // Shows image
        J_Image_ShowImage(m_hView, pAqImageInfo);
    }
}

void CReadPixelSampleDlg::ViewWindowNotificationCallback(VIEW_HANDLE hWin, J_IVW_CALLBACK_TYPE Type, void *pValue)
{
    HDC hDC;

    // Handle the different View Window events!
    // The event data type depends on the type of event.
    switch (Type)
    {
        case J_IVW_CB_WINDOW_MOVE:
            EnterCriticalSection(&m_CriticalSection);
            m_ViewWindowEventText.Format(_T("View Window Move event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
            LeaveCriticalSection(&m_CriticalSection);
            break;

        case J_IVW_CB_WINDOW_SIZE:
            EnterCriticalSection(&m_CriticalSection);
            m_ViewWindowEventText.Format(_T("View Window Size event: Size = (%d,%d)"), ((SIZE*)pValue)->cx, ((SIZE*)pValue)->cy);
            LeaveCriticalSection(&m_CriticalSection);
            break;

        case J_IVW_CB_MOUSE_MOVE:
            // Get the mouse cursor location
            EnterCriticalSection(&m_CriticalSection);
            m_XPos = ((POINT*)pValue)->x;
            m_YPos = ((POINT*)pValue)->y;

            m_bPositionChanged = true;
            m_ViewWindowEventText.Format(_T("Mouse Move event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
            LeaveCriticalSection(&m_CriticalSection);
            break;
        case J_IVW_CB_MOUSE_LBUTTON_DOWN:
            EnterCriticalSection(&m_CriticalSection);
            m_ViewWindowEventText.Format(_T("Mouse Left Button Down event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
            LeaveCriticalSection(&m_CriticalSection);
            break;

        case J_IVW_CB_MOUSE_LBUTTON_UP:
            EnterCriticalSection(&m_CriticalSection);
            m_ViewWindowEventText.Format(_T("Mouse Left Button Up event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
            LeaveCriticalSection(&m_CriticalSection);
            break;
        case J_IVW_CB_MOUSE_RBUTTON_DOWN:
            EnterCriticalSection(&m_CriticalSection);
            m_ViewWindowEventText.Format(_T("Mouse Right Button Down event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
            LeaveCriticalSection(&m_CriticalSection);
            break;
        case J_IVW_CB_MOUSE_RBUTTON_UP:
            EnterCriticalSection(&m_CriticalSection);
            m_ViewWindowEventText.Format(_T("Mouse Right Button Up event: Pos = (%d,%d)"), ((POINT*)pValue)->x, ((POINT*)pValue)->y);
            LeaveCriticalSection(&m_CriticalSection);
            break;
        case J_IVW_CB_ZOOM_POS:
            EnterCriticalSection(&m_CriticalSection);
            m_ViewWindowEventText.Format(_T("Zoom Position event: Offset = (%d,%d), ZoomRationX = %f, ZoomRationY = %f"), ((J_IVW_ZOOM_POS*)pValue)->Offset.x, ((J_IVW_ZOOM_POS*)pValue)->Offset.y, ((J_IVW_ZOOM_POS*)pValue)->ZoomRatioX, ((J_IVW_ZOOM_POS*)pValue)->ZoomRatioY);
            LeaveCriticalSection(&m_CriticalSection);
            break;

        case J_IVW_CB_AREA_SELECT:
            EnterCriticalSection(&m_CriticalSection);
            m_ViewWindowEventText.Format(_T("Area Selection event: Area (Left,Top,Right,Bottom)=(%d,%d,%d,%d)"), ((RECT*)pValue)->left, ((RECT*)pValue)->top, ((RECT*)pValue)->right, ((RECT*)pValue)->bottom);
            LeaveCriticalSection(&m_CriticalSection);
            break;

        case J_IVW_CB_USER_PAINT_IMAGE:
        {
            // Print out some stuff on the image!

            // First we need to get the Device Context from the callback parameter
            hDC = ((PJ_IVW_USER_PAINT)pValue)->hDC;

            // Create the GDI+ Graphics object used for the drawing
            Graphics g(hDC);

            // Set the text rendering for Cleartype
            g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

            // Create a red Solid Brush Color(A,R,G,B)
            SolidBrush brush(Color(255, 255, 0, 0));

            // Create a font
            FontFamily fontFamily(L"Arial");
            Font font(&fontFamily, 18, FontStyleRegular, UnitPoint);
 
            PointF pointF(100.0, 100.0);

            g.DrawString(m_UserDrawImageEventText, -1, &font, pointF, &brush);

            if (m_bEnableUserDrawImageGrid)
            {
                // Create a Solid red pen Color(A,R,G,B)
                Pen  redPen(Color(255, 255, 0, 0));

                for (int x=100; x<((PJ_IVW_USER_PAINT)pValue)->ImageWidth; x+=100)
                {
                    g.DrawLine(&redPen, x, 0, x, (int) ((PJ_IVW_USER_PAINT)pValue)->ImageHeight-1);
                }

                for (int y=100; y<((PJ_IVW_USER_PAINT)pValue)->ImageHeight; y+=100)
                {
                    g.DrawLine(&redPen, 0, y, (int) ((PJ_IVW_USER_PAINT)pValue)->ImageWidth-1, y);
                }
            }
        }
        break;


        case J_IVW_CB_USER_PAINT_WINDOW:
        {
            // First we need to get the Device Context from the callback parameter
            hDC = ((PJ_IVW_USER_PAINT)pValue)->hDC;

            // Create the GDI+ Graphics object used for the drawing
            Graphics g(hDC);

            // Set the text rendering for Cleartype
            g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

            // Create a blue Solid Brush Color(A,R,G,B)
            SolidBrush brush(Color(255, 0, 0, 255));

            // Create a font
            FontFamily fontFamily(L"Arial");
            Font font(&fontFamily, 18, FontStyleRegular, UnitPoint);
 
            PointF pointF(150.0, 150.0);

            g.DrawString(m_UserDrawViewWindowEventText, -1, &font, pointF, &brush);

            if (m_bEnableUserDrawViewWindowGrid)
            {
                // Create a Solid red pen Color(A,R,G,B)
                Pen  bluePen(Color(255, 0, 0, 255));

                for (int x=100; x<((PJ_IVW_USER_PAINT)pValue)->ImageWidth; x+=100)
                {
                    g.DrawLine(&bluePen, x, 0, x, (int) ((PJ_IVW_USER_PAINT)pValue)->ImageHeight-1);
                }

                for (int y=100; y<((PJ_IVW_USER_PAINT)pValue)->ImageHeight; y+=100)
                {
                    g.DrawLine(&bluePen, 0, y, (int) ((PJ_IVW_USER_PAINT)pValue)->ImageWidth-1, y);
                }
            }
        }
        break;

        default:
            EnterCriticalSection(&m_CriticalSection);
            m_ViewWindowEventText.Format(_T("Unknown event received!: event = %d"), Type);
            LeaveCriticalSection(&m_CriticalSection);
    }
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CReadPixelSampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

    // Start Acquisition
    if (m_hCam) {
        retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);
        if (retval != J_ST_SUCCESS) {
            ShowErrorMsg(CString("Could not Stop Acquisition!"), retval);
        }
    }

    if(m_hThread)
    {
        // Close stream
        retval = J_Image_CloseStream(m_hThread);
        if (retval != J_ST_SUCCESS) {
            ShowErrorMsg(CString("Could not close Stream!"), retval);
        }
        m_hThread = NULL;
        TRACE("Closed stream\n");
    }

    if(m_hView)
    {
        // Close view window
        retval = J_Image_CloseViewWindow(m_hView);
        if (retval != J_ST_SUCCESS) {
            ShowErrorMsg(CString("Could not close View Window!"), retval);
        }
        m_hView = NULL;
        TRACE("Closed view window\n");
    }

    KillTimer(1);

    if (m_CnvImageInfo.pImageBuffer != NULL)
    {
        retval = J_Image_Free(&m_CnvImageInfo);
        if (retval != J_ST_SUCCESS) {
            ShowErrorMsg(CString("Could not free image info!"), retval);
        }
        m_CnvImageInfo.pImageBuffer = NULL;
    }

    EnableControls(TRUE, FALSE);
}
//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CReadPixelSampleDlg::InitializeControls()
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
        //ShowErrorMsg(CString("Could not get Gain!"), retval);

        pSCtrl->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_LBL_GAIN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_GAIN)->ShowWindow(SW_HIDE);
    }

    // Update the check marks
    ((CButton*)GetDlgItem(IDC_STRETCHED_CHECK))->SetCheck(m_bStretchViewWindow);
    ((CButton*)GetDlgItem(IDC_ENABLE_MOUSE_ZOOM_CHECK))->SetCheck(m_bEnableMouseZoom);
    ((CButton*)GetDlgItem(IDC_ENABLE_MOUSE_CURSOR_CHECK))->SetCheck(m_bEnableMouseCursor);
    ((CButton*)GetDlgItem(IDC_ENABLE_DOUBLE_BUFFERING_CHECK))->SetCheck(m_bEnableDoubleBuffering);
    ((CButton*)GetDlgItem(IDC_WINDOW_MOVE_EVENT_CHECK))->SetCheck(m_bEnableViewWindowMoveEvents);
    ((CButton*)GetDlgItem(IDC_WINDOW_SIZE_EVENT_CHECK))->SetCheck(m_bEnableViewWindowSizeEvents);
    ((CButton*)GetDlgItem(IDC_MOUSE_MOVE_EVENT_CHECK))->SetCheck(m_bEnableMouseMoveEvents);
    ((CButton*)GetDlgItem(IDC_LBUTTON_DOWN_CHECK))->SetCheck(m_bEnableMouseLButtonDownEvents);
    ((CButton*)GetDlgItem(IDC_LBUTTON_UP_CHECK))->SetCheck(m_bEnableMouseLButtonUpEvents);
    ((CButton*)GetDlgItem(IDC_RBUTTON_DOWN_CHECK))->SetCheck(m_bEnableMouseRButtonDownEvents);
    ((CButton*)GetDlgItem(IDC_RBUTTON_UP_CHECK))->SetCheck(m_bEnableMouseRButtonUpEvents);
    ((CButton*)GetDlgItem(IDC_ZOOM_POS_EVENT_CHECK))->SetCheck(m_bEnableZoomPosChangeEvents);
    ((CButton*)GetDlgItem(IDC_AREA_EVENT_CHECK))->SetCheck(m_bEnableAreaSelectionEvents);
    ((CButton*)GetDlgItem(IDC_USER_DRAW_IMAGE_EVENT_CHECK))->SetCheck(m_bEnableUserDrawImageEvents);
    ((CButton*)GetDlgItem(IDC_USER_DRAW_VIEW_WINDOW_EVENT_CHECK))->SetCheck(m_bEnableUserDrawViewWindowEvents);
    ((CButton*)GetDlgItem(IDC_ENABLE_DOUBLE_BUFFERING_CHECK))->SetCheck(m_bEnableDoubleBuffering);

    SetDlgItemText(IDC_USER_DRAW_IMAGE_TEXT_EDIT, m_UserDrawImageEventText);
    SetDlgItemText(IDC_USER_DRAW_VIEW_WINDOW_TEXT_EDIT, m_UserDrawViewWindowEventText);
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CReadPixelSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
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
void CReadPixelSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

		//Set frame grabber dimension, if applicable
		SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

        SetDlgItemInt(IDC_GAIN, iPos);
    }

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CReadPixelSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
    CString valueString;

    if (nIDEvent == 1)
    {
        EnterCriticalSection(&m_CriticalSection);
        switch (m_PixelType)
        {
        case J_GVSP_PIX_MONO8:
            valueString.Format(_T("Mono8 PixelValue at (%d,%d): %d"), m_XPos, m_YPos, m_PixelValue.PixelValueUnion.Mono8Type.Value);
            break;

        case J_GVSP_PIX_MONO16:
            // Be aware that the Mono 16 values are "normalized" into 16-bit values by shifting the 10- and 12-bit values from the cameras up to MSB!
            // If you need to get 10-bit or 12-bit values instead of the normalized valeus you will ned to do like this:
            // value = m_PixelValue.PixelValueUnion.Mono16Type.Value & OxFFC0) >> 6 // 10bit mono
            // ... or
            // value = m_PixelValue.PixelValueUnion.Mono16Type.Value & OxFFF0) >> 4 // 12bit mono
            valueString.Format(_T("Mono16 Pixel Value at (%d,%d): %d"), m_XPos, m_YPos, m_PixelValue.PixelValueUnion.Mono16Type.Value);
            break;

        case J_GVSP_PIX_BGR8_PACKED:
            valueString.Format(_T("BGR24 Pixel Value at (%d,%d): (R,G,B)=(%d,%d,%d)"), m_XPos, m_YPos, m_PixelValue.PixelValueUnion.BGR24Type.RValue, m_PixelValue.PixelValueUnion.BGR24Type.GValue, m_PixelValue.PixelValueUnion.BGR24Type.BValue);
            break;

        case J_GVSP_PIX_BGR16_PACKED_INTERNAL:
            // Be aware that the BGR 16 values are "normalized" into 16-bit values by shifting the 10- and 12-bit values from the cameras up to MSB!
            // If you need to get 10-bit or 12-bit values instead of the normalized valeus you will ned to do like this:
            // RValue = m_PixelValue.PixelValueUnion.BGR48Type.RValue & OxFFC0) >> 6 // 10bit Color
            // GValue = m_PixelValue.PixelValueUnion.BGR48Type.GValue & OxFFC0) >> 6 // 10bit Color
            // BValue = m_PixelValue.PixelValueUnion.BGR48Type.BValue & OxFFC0) >> 6 // 10bit Color
            // ... or
            // RValue = m_PixelValue.PixelValueUnion.BGR48Type.RValue & OxFFF0) >> 4 // 12bit Color
            // GValue = m_PixelValue.PixelValueUnion.BGR48Type.GValue & OxFFF0) >> 4 // 12bit Color
            // BValue = m_PixelValue.PixelValueUnion.BGR48Type.BValue & OxFFF0) >> 4 // 12bit Color
            valueString.Format(_T("BGR48 Pixel Value at (%d,%d): (R,G,B)=(%d,%d,%d)"), m_XPos, m_YPos, m_PixelValue.PixelValueUnion.BGR48Type.RValue, m_PixelValue.PixelValueUnion.BGR48Type.GValue, m_PixelValue.PixelValueUnion.BGR48Type.BValue);
            break;

        }
        m_ValueStaticCtrl.SetWindowText(valueString);

        GetDlgItem(IDC_EVENT_STATIC)->SetWindowText(m_ViewWindowEventText);

        LeaveCriticalSection(&m_CriticalSection);
    }
    CDialog::OnTimer(nIDEvent);
}

void CReadPixelSampleDlg::OnBnClickedStretchedCheck()
{
    m_bStretchViewWindow = (((CButton*)GetDlgItem(IDC_STRETCHED_CHECK))->GetCheck() == BST_CHECKED);
    GetDlgItem(IDC_ENABLE_MOUSE_ZOOM_CHECK)->EnableWindow(m_bStretchViewWindow);
}

void CReadPixelSampleDlg::OnBnClickedEnableMouseZoomCheck()
{
    m_bEnableMouseZoom = (((CButton*)GetDlgItem(IDC_ENABLE_MOUSE_ZOOM_CHECK))->GetCheck() == BST_CHECKED);

    if (m_hView != NULL)
    {
        J_Image_SetViewWindowOption(m_hView, J_IVW_MOUSE_ZOOM, &m_bEnableMouseZoom);
    }
}

void CReadPixelSampleDlg::OnBnClickedEnableMouseCursorCheck()
{
    m_bEnableMouseCursor = (((CButton*)GetDlgItem(IDC_ENABLE_MOUSE_CURSOR_CHECK))->GetCheck() == BST_CHECKED);

    if (m_hView != NULL)
    {
        J_Image_SetViewWindowOption(m_hView, J_IVW_MOUSE_CURSOR, &m_bEnableMouseCursor);
    }
}

void CReadPixelSampleDlg::SetupViewWindowCallbackMask()
{
    J_STATUS_TYPE   retval = 0;

    if (m_hView != NULL)
    {
        // Build the mask for which event we want to enable
        uint32_t CallbackEventMask = 0;
        if (m_bEnableViewWindowMoveEvents)
            CallbackEventMask |= J_IVW_CB_WINDOW_MOVE;
        if (m_bEnableViewWindowSizeEvents)
            CallbackEventMask |= J_IVW_CB_WINDOW_SIZE;
        if (m_bEnableMouseMoveEvents)
            CallbackEventMask |= J_IVW_CB_MOUSE_MOVE;
        if (m_bEnableMouseLButtonDownEvents)
            CallbackEventMask |= J_IVW_CB_MOUSE_LBUTTON_DOWN;
        if (m_bEnableMouseLButtonUpEvents)
            CallbackEventMask |= J_IVW_CB_MOUSE_LBUTTON_UP;
        if (m_bEnableMouseRButtonDownEvents)
            CallbackEventMask |= J_IVW_CB_MOUSE_RBUTTON_DOWN;
        if (m_bEnableMouseRButtonUpEvents)
            CallbackEventMask |= J_IVW_CB_MOUSE_RBUTTON_UP;
        if (m_bEnableZoomPosChangeEvents)
            CallbackEventMask |= J_IVW_CB_ZOOM_POS;
        if (m_bEnableAreaSelectionEvents)
            CallbackEventMask |= J_IVW_CB_AREA_SELECT;
        if (m_bEnableUserDrawImageEvents)
            CallbackEventMask |= J_IVW_CB_USER_PAINT_IMAGE;
        if (m_bEnableUserDrawViewWindowEvents)
            CallbackEventMask |= J_IVW_CB_USER_PAINT_WINDOW;

        // Set the callback event mask
        retval = J_Image_SetViewWindowOption(m_hView, J_IVW_CALLBACK_MASK, &CallbackEventMask);
    }
}

void CReadPixelSampleDlg::OnBnClickedWindowMoveEventCheck()
{
    m_bEnableViewWindowMoveEvents = (((CButton*)GetDlgItem(IDC_WINDOW_MOVE_EVENT_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
}

void CReadPixelSampleDlg::OnBnClickedWindowSizeEventCheck()
{
    m_bEnableViewWindowSizeEvents = (((CButton*)GetDlgItem(IDC_WINDOW_SIZE_EVENT_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
}

void CReadPixelSampleDlg::OnBnClickedMouseMoveEventCheck()
{
    m_bEnableMouseMoveEvents = (((CButton*)GetDlgItem(IDC_MOUSE_MOVE_EVENT_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
}

void CReadPixelSampleDlg::OnBnClickedLbuttonDownCheck()
{
    m_bEnableMouseLButtonDownEvents = (((CButton*)GetDlgItem(IDC_LBUTTON_DOWN_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
}

void CReadPixelSampleDlg::OnBnClickedLbuttonUpCheck()
{
    m_bEnableMouseLButtonUpEvents = (((CButton*)GetDlgItem(IDC_LBUTTON_UP_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
}

void CReadPixelSampleDlg::OnBnClickedRbuttonDownCheck()
{
    m_bEnableMouseRButtonDownEvents = (((CButton*)GetDlgItem(IDC_RBUTTON_DOWN_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
}

void CReadPixelSampleDlg::OnBnClickedRbuttonUpCheck()
{
    m_bEnableMouseRButtonUpEvents = (((CButton*)GetDlgItem(IDC_RBUTTON_UP_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
}

void CReadPixelSampleDlg::OnBnClickedZoomPosEventCheck()
{
    m_bEnableZoomPosChangeEvents = (((CButton*)GetDlgItem(IDC_ZOOM_POS_EVENT_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
}

void CReadPixelSampleDlg::OnBnClickedAreaEventCheck()
{
    m_bEnableAreaSelectionEvents = (((CButton*)GetDlgItem(IDC_AREA_EVENT_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
}

void CReadPixelSampleDlg::OnBnClickedUserDrawImageEventCheck()
{
    m_bEnableUserDrawImageEvents = (((CButton*)GetDlgItem(IDC_USER_DRAW_IMAGE_EVENT_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();
    
    GetDlgItem(IDC_USER_DRAW_IMAGE_TEXT_EDIT)->EnableWindow(m_bEnableUserDrawImageEvents ? TRUE : FALSE);
    GetDlgItem(IDC_USER_DRAW_IMAGE_GRID_CHECK)->EnableWindow(m_bEnableUserDrawImageEvents ? TRUE : FALSE);
}

void CReadPixelSampleDlg::OnBnClickedUserDrawViewWindowEventCheck()
{
    m_bEnableUserDrawViewWindowEvents = (((CButton*)GetDlgItem(IDC_USER_DRAW_VIEW_WINDOW_EVENT_CHECK))->GetCheck() == BST_CHECKED);

    SetupViewWindowCallbackMask();

    GetDlgItem(IDC_USER_DRAW_VIEW_WINDOW_TEXT_EDIT)->EnableWindow(m_bEnableUserDrawViewWindowEvents ? TRUE : FALSE);
    GetDlgItem(IDC_USER_DRAW_VIEW_WINDOW_GRID_CHECK)->EnableWindow(m_bEnableUserDrawImageEvents ? TRUE : FALSE);
}

void CReadPixelSampleDlg::OnEnChangeUserDrawImageTextEdit()
{
    GetDlgItemText(IDC_USER_DRAW_IMAGE_TEXT_EDIT, m_UserDrawImageEventText);
}

void CReadPixelSampleDlg::OnEnChangeUserDrawViewWindowTextEdit()
{
    GetDlgItemText(IDC_USER_DRAW_VIEW_WINDOW_TEXT_EDIT, m_UserDrawViewWindowEventText);
}

void CReadPixelSampleDlg::OnBnClickedEnableDoubleBufferingCheck()
{
    m_bEnableDoubleBuffering = (((CButton*)GetDlgItem(IDC_ENABLE_DOUBLE_BUFFERING_CHECK))->GetCheck() == BST_CHECKED);

    if (m_hView != NULL)
    {
        J_Image_SetViewWindowOption(m_hView, J_IVW_DOUBLE_BUFFERING, &m_bEnableDoubleBuffering);
    }
}

void CReadPixelSampleDlg::OnBnClickedUserDrawImageGridCheck()
{
    m_bEnableUserDrawImageGrid = (((CButton*)GetDlgItem(IDC_USER_DRAW_IMAGE_GRID_CHECK))->GetCheck() == BST_CHECKED);
}

void CReadPixelSampleDlg::OnBnClickedUserDrawViewWindowGridCheck()
{
    m_bEnableUserDrawViewWindowGrid = (((CButton*)GetDlgItem(IDC_USER_DRAW_VIEW_WINDOW_GRID_CHECK))->GetCheck() == BST_CHECKED);
}

void CReadPixelSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CReadPixelSampleDlg::OnBnClickedOk()
{
	OnOK();
}
