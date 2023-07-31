// ImageConversionSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageConversionSample.h"
#include "ImageConversionSampleDlg.h"
#include <VLib.h>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int64_t NUM_LINESCAN_LINES = 128;

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

// CImageConversionSampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CImageConversionSampleDlg::CImageConversionSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CImageConversionSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;

    m_BufferInfo.pImageBuffer = NULL;
    m_YBufferInfo.pImageBuffer = NULL;
    m_bConvertImage = false;
	m_bHasAlphaChannel = false;

	m_iWidthInc = 1;
	m_iHeightInc = 1;

	m_bEnableStreaming = false;
}

void CImageConversionSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CONVERT_CHECK, m_ConvertCheckCtrl);
}

BEGIN_MESSAGE_MAP(CImageConversionSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CImageConversionSampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CImageConversionSampleDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
	ON_BN_CLICKED(IDCANCEL, &CImageConversionSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CImageConversionSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CImageConversionSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CImageConversionSampleDlg::OnInitDialog()
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
void CImageConversionSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // TODO: Add your message handler code here

    // Stop acquisition
    OnBnClickedStop();

    // Close factory & camera
    CloseFactoryAndCamera();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CImageConversionSampleDlg::OnPaint()
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
HCURSOR CImageConversionSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CImageConversionSampleDlg::ShowErrorMsg(J_STATUS_TYPE error)
{
	CString errorMsg;
	errorMsg.Format(_T("Error = %d: "), error);

	switch(error)
	{
		case J_ST_INVALID_BUFFER_SIZE:	errorMsg += "Invalid buffer size ";	break;
		case J_ST_INVALID_HANDLE:		errorMsg += "Invalid handle ";		break;
		case J_ST_INVALID_ID:			errorMsg += "Invalid ID ";			break;
		case J_ST_ACCESS_DENIED:		errorMsg += "Access denied ";		break;
		case J_ST_NO_DATA:				errorMsg += "No data ";				break;
		case J_ST_ERROR:				errorMsg += "Generic error ";		break;
		case J_ST_INVALID_PARAMETER:	errorMsg += "Invalid parameter ";	break;
		case J_ST_TIMEOUT:				errorMsg += "Timeout ";				break;
		case J_ST_INVALID_FILENAME:		errorMsg += "Invalid file name ";	break;
		case J_ST_INVALID_ADDRESS:		errorMsg += "Invalid address ";		break;
		case J_ST_FILE_IO:				errorMsg += "File IO error ";		break;
		case J_ST_GC_ERROR:				errorMsg += "GenICam error ";		break;
		case J_ST_VALIDATION_ERROR:		errorMsg += "Settings File Validation Error ";		break;
		case J_ST_VALIDATION_WARNING:	errorMsg += "Settings File Validation Warning ";    break;
	}
	
	AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);
}

void CImageConversionSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CImageConversionSampleDlg::OpenFactoryAndCamera()
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

	int64_t pixelFormat = 0;
	uint64_t jaiPixelFormat = 0;
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

	EnableConversion();

	PrepareContinuousSetting();

	return TRUE;
}

//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CImageConversionSampleDlg::CloseFactoryAndCamera()
{
    if (m_hCam)
    {
		J_Camera_SetValueString(m_hCam, (int8_t*)"TriggerMode", (int8_t*)"Off");

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

    // Frees buffer memory.
    if (m_YBufferInfo.pImageBuffer != NULL)
    {
        J_Image_Free(&m_YBufferInfo);
        m_YBufferInfo.pImageBuffer = NULL;
    }

    // Frees buffer memory.
    if (m_BufferInfo.pImageBuffer != NULL)
    {
        J_Image_Free(&m_BufferInfo);
        m_BufferInfo.pImageBuffer = NULL;
    }
}

void CImageConversionSampleDlg::PrepareContinuousSetting()
{
    if(m_hCam)
    {
        J_STATUS_TYPE status;

        // Set up the camera for continuous exposure mode

        // We have two possible ways of setting up triggers: JAI or GenICam SFNC
        // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
        // called "TriggerSelector". Therefor we have to determine which way to use here.
        // First we see if a node called "TriggerSelector" exists.
        NODE_HANDLE hNode = NULL;
        status = J_Camera_GetNodeByName(m_hCam, (int8_t*)"TriggerSelector", &hNode);

        // Does the "TriggerSelector" node exist?
        if ((status == J_ST_SUCCESS) && (hNode != NULL))
        {
            // Here we assume that this is GenICam SFNC trigger so we do the following:
            // TriggerSelector=FrameStart
            // TriggerMode=Off
            status = J_Camera_SetValueString(m_hCam, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
            if (status != J_ST_SUCCESS)
            {
                //ShowErrorMsg(status);
                //return;
            }
            status = J_Camera_SetValueString(m_hCam, (int8_t*)"TriggerMode", (int8_t*)"Off");
            if (status != J_ST_SUCCESS)
            {
                //ShowErrorMsg(status);
                //return;
            }
        }
        else
        {
            // Here we assume that this is JAI trigger so we do the following:
            // ExposureMode=Continuous

            // Set ExposureMode="Continuous"		
            status = J_Camera_SetValueString(m_hCam, (int8_t*)"ExposureMode", (int8_t*)"Continuous");
            if(status != J_ST_SUCCESS) 
            {
                //ShowErrorMsg(status);
                //return;
            }
        }
    } // end of if(m_hCamera)
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CImageConversionSampleDlg::OnBnClickedStart()
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
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

    // Get Height from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);

	//If this is a line-scan camera, set height to #lines:
	if(1 == int64Val)
	{
		J_Camera_SetValueInt64(m_hCam, NODE_NAME_HEIGHT, NUM_LINESCAN_LINES);

		retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);

		assert(NUM_LINESCAN_LINES == int64Val);
	}

    ViewSize.cy = (LONG)int64Val;     // Set window size cy

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

	// Get the pixelformat from the camera
	int64_t pixelFormat = 0;
	uint64_t jaiPixelFormat = 0;
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

	//Set frame grabber pixel format, if applicable
	SetFramegrabberPixelFormat(m_hCam, NODE_NAME_PIXELFORMAT, pixelFormat, &m_sCameraId[0]);

	// Calculate number of bits (not bytes) per pixel using macro
	int bpp = J_BitsPerPixel(jaiPixelFormat);

    // Set window position
    TopLeft.x = 100;
    TopLeft.y = 50;

    // Open view window
    retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView);
    if (retval != J_ST_SUCCESS) {
        AfxMessageBox(CString("Could not open view window!"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Open stream
    retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CImageConversionSampleDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
    if (retval != J_ST_SUCCESS) {
        AfxMessageBox(CString("Could not open stream!"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    TRACE("Opening stream succeeded\n");

    if (m_ConvertCheckCtrl.GetCheck() == BST_CHECKED)
        m_bConvertImage = true;
    else
        m_bConvertImage = false;

    m_ConvertCheckCtrl.EnableWindow(FALSE);

    // Start Acquisition
    retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);

    EnableControls(TRUE, TRUE);
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CImageConversionSampleDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
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
			OutputDebugString(_T("Error with J_Image_Get_PixelFormat in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
			pAqImageInfo->iPixelType = GVSP_PIX_MONO8;
		}

		if(m_bHasAlphaChannel && m_bConvertImage)
		{
			//Extract and display the alpha channel:

			// Allocates buffer memory for alpha image.
			J_PIXEL_FORMAT pf = PF_EXTRACT_ALPHA;
			if (m_YBufferInfo.pImageBuffer == NULL)
			{
				memset(&m_YBufferInfo, 0, sizeof(J_tIMAGE_INFO));
				iResult = J_Image_MallocEx(&m_BufferInfo, &m_YBufferInfo, pf);
				if(NULL == m_YBufferInfo.pImageBuffer || J_ST_SUCCESS != iResult)
				{
					return;
				}
			}

			// If allocation succeeds, extract alpha channel
			if(NULL != m_YBufferInfo.pImageBuffer && J_ST_SUCCESS == J_Image_ConvertImage(pAqImageInfo, &m_YBufferInfo, pf))
			{
				// Display the extracted (monochrome) image
				J_Image_ShowImage(m_hView, &m_YBufferInfo);
			}
			else
			{
				return;
			}
		}
        else if (m_bHasAlphaChannel)
        {
			//Extract and display the RGB channels and do not extract the alpha channel:

			if (m_BufferInfo.pImageBuffer == NULL)
			{
				memset(&m_BufferInfo, 0, sizeof(J_tIMAGE_INFO));

				//This will allocate memory for all of the four colors.
				J_PIXEL_FORMAT pf = PF_NONE;
				iResult = J_Image_MallocEx(pAqImageInfo, &m_BufferInfo, pf);
				if(NULL == m_BufferInfo.pImageBuffer || J_ST_SUCCESS != iResult)
				{
					return;
				}
			}

			//This sets the alpha channel to 255.
			if(J_ST_SUCCESS == J_Image_FromRawToImageEx(pAqImageInfo, &m_BufferInfo, BAYER_STANDARD_MULTI))
			{
				// Display the RGB image
				J_Image_ShowImage(m_hView, &m_BufferInfo);
			}
			else
            {
				return;
            }
        }
		else if (m_bConvertImage)
		{
			//NO alpha channel - convert RGB to Y and display:

			if (m_BufferInfo.pImageBuffer == NULL)
				J_Image_Malloc(pAqImageInfo, &m_BufferInfo);

			// Allocates buffer memory for RGB image.
			if (m_BufferInfo.pImageBuffer != NULL)
			{
				// Converts from RAW to full bit image using extended (zipper-effect free) color interpolation.
				if(J_ST_SUCCESS == J_Image_FromRawToImageEx(pAqImageInfo, &m_BufferInfo, BAYER_EXTEND))
				{
					// Allocates buffer memory for Y image.
					J_PIXEL_FORMAT pf = PF_Y;
					if (m_YBufferInfo.pImageBuffer == NULL)
						J_Image_MallocEx(&m_BufferInfo, &m_YBufferInfo, pf);

					if(m_YBufferInfo.pImageBuffer != NULL)
					{
						// Converts from RGB to Y.
						if(J_ST_SUCCESS == J_Image_ConvertImage(&m_BufferInfo, &m_YBufferInfo, pf))
						{
							// Display the converted (monochrome) image
							J_Image_ShowImage(m_hView, &m_YBufferInfo);
						}
					}
				}
			}
		}
        else
        {
            // NO Alpha channel: shows image without conversion
            J_Image_ShowImage(m_hView, pAqImageInfo, BAYER_EXTEND);
        }
    }
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CImageConversionSampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

    // Start Acquision
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

    m_ConvertCheckCtrl.EnableWindow(TRUE);
    EnableControls(TRUE, FALSE);
}
//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CImageConversionSampleDlg::InitializeControls()
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

	// Get pixel format from the camera
	int64_t pixelFormat = 0;
	uint64_t jaiPixelFormat = 0;
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

	EnableConversion();

}

void CImageConversionSampleDlg::EnableConversion()
{
	char buff[80];
	NODE_HANDLE hNode;
	uint32_t iSize = sizeof(buff);
	J_STATUS_TYPE retval = J_Camera_GetNodeByName(m_hCam, (int8_t*)"PixelFormat", &hNode);
	if(NULL == hNode)
	{
		AfxMessageBox(L"Cannot access Pixel Format node.", MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	retval = J_Node_GetValueString(hNode, 0, (int8_t*)buff, &iSize);

	if (strstr(buff, "Bayer"))
	{
		m_ConvertCheckCtrl.EnableWindow(TRUE);
		m_ConvertCheckCtrl.SetWindowText(L"Convert RGB to Y");
	}
	else if (strstr(buff, "RGBa8"))
	{
		m_ConvertCheckCtrl.EnableWindow(TRUE);
		m_bHasAlphaChannel = true;
		m_ConvertCheckCtrl.SetWindowText(L"Extract Alpha from RGBa8");
	}
	else if (strstr(buff, "RGBa10"))
	{
		m_ConvertCheckCtrl.EnableWindow(TRUE);
		m_bHasAlphaChannel = true;
		m_ConvertCheckCtrl.SetWindowText(L"Extract Alpha from RGBa10");
	}
	else if (strstr(buff, "RGBa10p"))
	{
		m_ConvertCheckCtrl.EnableWindow(TRUE);
		m_bHasAlphaChannel = true;
		m_ConvertCheckCtrl.SetWindowText(L"Extract Alpha from RGBa10p");
	}
	else if (strstr(buff, "RGBa12"))
	{
		m_ConvertCheckCtrl.EnableWindow(TRUE);
		m_bHasAlphaChannel = true;
		m_ConvertCheckCtrl.SetWindowText(L"Extract Alpha from RGBa12");
	}
	else if (strstr(buff, "RGBa12p"))
	{
		m_ConvertCheckCtrl.EnableWindow(TRUE);
		m_bHasAlphaChannel = true;
		m_ConvertCheckCtrl.SetWindowText(L"Extract Alpha from RGBa12p");
	}
	else
	{
		m_bConvertImage = false;
		m_bHasAlphaChannel = false;
		m_ConvertCheckCtrl.EnableWindow(FALSE);
	}
}
//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CImageConversionSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
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
void CImageConversionSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

        // Get Width Node
        retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_WIDTH, int64Val);

        SetDlgItemInt(IDC_WIDTH, iPos);
    }

    //- Height -----------------------------------------------

    // Get SliderCtrl for Height
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) {

        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Get Height Node
        retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_HEIGHT, int64Val);

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

void CImageConversionSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CImageConversionSampleDlg::OnBnClickedOk()
{
	OnOK();
}
