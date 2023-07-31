// VCSampleLIntensityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VCSampleLIntensity.h"
#include "VCSampleLIntensityDlg.h"
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

// CVCSampleLIntensityDlg dialog

CVCSampleLIntensityDlg::CVCSampleLIntensityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVCSampleLIntensityDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hFactory	= NULL;
	m_hCam	= NULL; 
	m_hView		= NULL;
	m_hThread	= NULL;

	m_CnvImageInfo.pImageBuffer = NULL;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;
	m_fp = NULL;

    // Prepare the critical section
    InitializeCriticalSection(&m_UICriticalSection);
}

void CVCSampleLIntensityDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_GAINSLIDER, m_gainSlider);
    DDX_Control(pDX, IDC_GAINSTATIC, m_gainStatic);
    DDX_Control(pDX, IDC_CAMERAIDSTATIC, m_cameraidStatic);
    DDX_Control(pDX, IDC_VIEWERPICTURECTRL, m_viewerPictureCtl);
    DDX_Control(pDX, IDC_STARTBUTTON, m_testStartButton);
    DDX_Control(pDX, IDC_STOPBUTTON, m_testStopButton);
    DDX_Control(pDX, IDC_INTENSITY_STATIC, m_IntensityString);
    DDX_Control(pDX, IDC_SAMPLE_NUMBER, m_SampleNumber);
}

BEGIN_MESSAGE_MAP(CVCSampleLIntensityDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_STARTBUTTON, &CVCSampleLIntensityDlg::OnBnClickedStartbutton)
	ON_BN_CLICKED(IDC_STOPBUTTON, &CVCSampleLIntensityDlg::OnBnClickedStopbutton)
	ON_WM_HSCROLL()
	ON_WM_DESTROY()
    ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CVCSampleLIntensityDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CVCSampleLIntensityDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CVCSampleLIntensityDlg message handlers

BOOL CVCSampleLIntensityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	J_STATUS_TYPE	status = J_ST_SUCCESS;

	if(OpenFactoryAndCamera())
	{	
		//Initailze controls
		NODE_HANDLE		hNode;	// Node handle
        char buff[80] = {0};
		uint32_t iSize =  sizeof(buff);
		int64_t minval, maxval, nodeValue;
		CString editStr;

		m_cameraidStatic.SetWindowText(CString((char*)m_sCameraId));

		//enable Start button, disable Stop button
		m_testStartButton.EnableWindow();
		m_testStopButton.EnableWindow(FALSE);
		
		// initialize gain slider
		m_gainStatic.EnableWindow(FALSE);
		m_gainSlider.EnableWindow(FALSE);

        status = J_Camera_GetNodeByName(m_hCam, (int8_t*)"GainRaw", &hNode);
		if(status != J_ST_SUCCESS) 
		{
			return FALSE;
		}

		status = J_Node_GetMinInt64(hNode, &minval); //get min value of the Gainraw node from GenICam 
		if(status != J_ST_SUCCESS) 
		{
			return FALSE;
		}

		status = J_Node_GetMaxInt64(hNode, &maxval); //get max value of the Gainraw node from GenICam
		if(status != J_ST_SUCCESS) 
		{
			return FALSE;
		}

		m_gainSlider.SetRange((int)minval,(int)maxval); //set min, max value in gainSlider
        m_gainSlider.SetTicFreq((int)(maxval-minval)/10);

		status = J_Node_GetValueInt64(hNode, 0, &nodeValue);	//get the gain value from GenICam
		m_gainSlider.SetPos((int)nodeValue); //set the gain value of gainSlider

		editStr.Format(_T("%d"),(int)nodeValue);
		m_gainStatic.SetWindowText(editStr);

		m_gainStatic.EnableWindow(TRUE);
		m_gainSlider.EnableWindow(TRUE);
	}
	else return FALSE; 

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVCSampleLIntensityDlg::OnPaint()
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
HCURSOR CVCSampleLIntensityDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CVCSampleLIntensityDlg::OpenFactoryAndCamera()
{
	J_STATUS_TYPE   retval;
	uint32_t        iSize;
	uint32_t        iNumDev;
	bool8_t         bHasChange;
	int64_t			int64Val;

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

	// Get Width from the camera
	retval = J_Camera_GetValueInt64(m_hCam, (int8_t *)NODE_NAME_WIDTH, &int64Val);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get the Width!"), retval);
		return FALSE;
	}

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

	// Get Height from the camera
	retval = J_Camera_GetValueInt64(m_hCam, (int8_t *)NODE_NAME_HEIGHT, &int64Val);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get the Height!"), retval);
		return FALSE;
	}

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

	// Get the pixelformat from the camera
	int64_t pixelFormat = 0;
	uint64_t jaiPixelFormat = 0;
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

	//Set frame grabber pixel format, if applicable
	SetFramegrabberPixelFormat(m_hCam, NODE_NAME_PIXELFORMAT, pixelFormat, &m_sCameraId[0]);

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

void CVCSampleLIntensityDlg::CloseFactoryAndCamera()
{
	J_STATUS_TYPE	status = J_ST_SUCCESS;

	if(m_hCam)
	{
		status = J_Camera_Close(m_hCam);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Unable to close camera!"), status);
			return;
		}
		m_hCam = NULL;
	}

	if(m_hFactory)
	{
		status = J_Factory_Close(m_hFactory);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Unable to close factory!"), status);
			return;
		}
		m_hFactory = NULL;
	}

    // Stop UI update timer
    KillTimer(1);
}

void CVCSampleLIntensityDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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

void CVCSampleLIntensityDlg::OnBnClickedStartbutton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	int64_t		widthMax = 0;
	int64_t		heightMax = 0;
	POINT		StartPoint;
	SIZE		MaxViewerSize;
	NODE_HANDLE hNode; 
	CString editStr;
    int64_t pixelFormat;

	if(!m_bEnableStreaming)
	{
		ShowErrorMsg(CString("Streaming not enabled on this device."), 0);
		return;
	}

	if(m_hCam)
	{
		char buff[80];
        int64_t nodeValue;
		uint32_t iSize = sizeof(buff);
		// Get PixelFormat
        status = J_Camera_GetNodeByName(m_hCam, (int8_t*)"PixelFormat", &hNode);

        // We need the pixel format value for the image handling
        status = J_Node_GetValueInt64(hNode, 0, &nodeValue);
       
        // .. and the pixel format string to printout nicely
        status = J_Node_GetValueString(hNode, 0, (int8_t*)buff, &iSize);

        if (status == J_ST_SUCCESS)
		{
			uint64_t jaiPixelFormat = 0;
			status = J_Image_Get_PixelFormat(m_hCam, nodeValue, &jaiPixelFormat);
			if (status != J_ST_SUCCESS)
			{
				AfxMessageBox(_T("Invalid pixel format found."), MB_OKCANCEL | MB_ICONINFORMATION);
				return;
			}
            m_pixelformat = jaiPixelFormat;
		}

        errno_t err = _tfopen_s(&m_fp, _T(".\\TestResult.txt"), _T("w"));
        if(err!=0) 
		{
			AfxMessageBox(_T("Cannot open file!"), MB_OKCANCEL | MB_ICONINFORMATION);
            return;
		}
		else 
		{
            CString valueString = CString(buff);

            _ftprintf(m_fp, _T("             <Test Result>\n"));
			_ftprintf(m_fp, _T("Pixel Format: %s\n"), valueString);

            status = J_Camera_GetNodeByName(m_hCam, (int8_t*)"DeviceModelName", &hNode);
			iSize = sizeof(buff);
            status = J_Node_GetValueString(hNode, 0, (int8_t*)buff, &iSize);
            valueString = CString(buff);
			_ftprintf(m_fp, _T("Device Model Name: %s\n\n\n"), valueString);
            switch (m_pixelformat)
            {
                case J_GVSP_PIX_MONO8:
                case J_GVSP_PIX_MONO10:
                case J_GVSP_PIX_MONO10_PACKED:
                case J_GVSP_PIX_MONO12:
                case J_GVSP_PIX_MONO12_PACKED:
    	    		_ftprintf(m_fp, _T("G\n"));
                    break;

                case J_GVSP_PIX_BAYGR8:
                case J_GVSP_PIX_BAYRG8:
                case J_GVSP_PIX_BAYGB8:
                case J_GVSP_PIX_BAYBG8:
                case J_GVSP_PIX_BAYGR10:
                case J_GVSP_PIX_BAYRG10:
                case J_GVSP_PIX_BAYGB10:
                case J_GVSP_PIX_BAYBG10:
                case J_GVSP_PIX_RGB10_PACKED:
                case J_GVSP_PIX_BGR10_PACKED:
                case J_GVSP_PIX_RGB10_PLANAR:
                case J_GVSP_PIX_BAYGR12:
                case J_GVSP_PIX_BAYRG12:
                case J_GVSP_PIX_BAYGB12:
                case J_GVSP_PIX_BAYBG12:
                case J_GVSP_PIX_RGB12_PACKED:
                case J_GVSP_PIX_BGR12_PACKED:
                case J_GVSP_PIX_RGB12_PLANAR:
    	    		_ftprintf(m_fp, _T("R\tG\tB\n"));
                    break;
            }
		}
			
		m_iCountMesuredImages = 0;
		m_RIntensity = 0;
		m_GIntensity = 0;
		m_BIntensity = 0;

		//Set Width to max width
        status = J_Camera_GetNodeByName(m_hCam, (int8_t*)"Width", &hNode);
		status = J_Node_GetMaxInt64(hNode, &widthMax); 
		status = J_Node_SetValueInt64(hNode, 0, widthMax);

		//Set Height to max height
        status = J_Camera_GetNodeByName(m_hCam, (int8_t*)"Height", &hNode);
		status = J_Node_GetMaxInt64(hNode, &heightMax); 
		status = J_Node_SetValueInt64(hNode, 0, heightMax);		

        // Get pixelformat from the camera
        status = J_Camera_GetValueInt64(m_hCam, (int8_t*)"PixelFormat", &pixelFormat);
        if (status != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Unable to get PixelFormat value!"), status);
            return;
        }

        // Calculate number of bits (not bytes) per pixel using macro
        int bpp = J_BitsPerPixel(pixelFormat);

		StartPoint.x = 100;	// x coordinate for viewer position
		StartPoint.y = 100;	// y coordinate for viewer position
		MaxViewerSize.cx = (long)widthMax;
		MaxViewerSize.cy = (long)heightMax;

		m_RectToMeasure.top = 0;
		m_RectToMeasure.left = 0;
		m_RectToMeasure.right = (long)widthMax;
		m_RectToMeasure.bottom = (long)heightMax;

		//status = J_Image_OpenViewWindow(_T("Mean Image of 100 Images"), &StartPoint, &MaxViewerSize, &m_hView);

		RECT frameRect; //to display images in picture control
		m_viewerPictureCtl.GetClientRect(&frameRect);

		status = J_Image_OpenViewWindowEx(J_IVW_CHILD_STRETCH, _T("Test Light Intensity"), &frameRect, &MaxViewerSize, m_viewerPictureCtl.m_hWnd, &m_hView);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Unable to open View Window!"), status);
			return;
		}
		
		status = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CVCSampleLIntensityDlg::StreamCBFunc), &m_hThread, (MaxViewerSize.cx * MaxViewerSize.cy * bpp)/8);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Unable to open Stream!"), status);
			return;
		}

        status = J_Camera_ExecuteCommand(m_hCam, (int8_t*) "AcquisitionStart");
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Unable to Start Acquisition!"), status);
			return;
		}

		m_testStartButton.EnableWindow(FALSE);
		m_testStopButton.EnableWindow();

		if (m_hView)
		{
			CRect mainWindowRect;
			CRect clientWindowRect;
			this->GetClientRect(&mainWindowRect);
			this->ClientToScreen(&mainWindowRect);

			m_viewerPictureCtl.GetClientRect(&clientWindowRect);
			m_viewerPictureCtl.ClientToScreen(&clientWindowRect);
			int m_LeftMargin = clientWindowRect.left - mainWindowRect.left;
			int m_RightMargin = mainWindowRect.right - clientWindowRect.right;
			int m_TopMargin = clientWindowRect.top - mainWindowRect.top;
			int m_BottomMargin = mainWindowRect.bottom - clientWindowRect.bottom;

			RECT childRect;
			childRect.top = 0;
			childRect.left = 0;
			childRect.right = mainWindowRect.Width() - (m_RightMargin+m_LeftMargin);
			childRect.bottom = mainWindowRect.Height() - (m_TopMargin+m_BottomMargin);

			J_Image_ResizeChildWindow(m_hView, &childRect);
		}

        // Start UI update timer
        SetTimer(1, 100, NULL);
	}
}

void CVCSampleLIntensityDlg::OnBnClickedStopbutton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

	if(!m_bEnableStreaming)
	{
		return;
	}

	// Stop Acquisition
	if (m_hCam) {
        status = J_Camera_ExecuteCommand(m_hCam, (int8_t*)"AcquisitionStop");
	}

	if(m_hThread)
	{
		// Close stream
		status = J_Image_CloseStream(m_hThread);
		m_hThread = NULL;
	}

	// Close view window
	if(m_hView)
	{
		status = J_Image_CloseViewWindow(m_hView);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Unable to close View Window!"),status);
			return;
		}

		m_hView = NULL;
	}

    // Free the conversion buffer
    if (m_CnvImageInfo.pImageBuffer != NULL)  
    {
        J_Image_Free(&m_CnvImageInfo);
        m_CnvImageInfo.pImageBuffer = NULL;
    }

	if(m_fp) 
	{
		fprintf(m_fp, "\n\nTotal Mesured Images : %d\n", m_iCountMesuredImages);
		fclose(m_fp);
		m_fp = NULL;

		CString MsgStr;
		MsgStr.Format(_T("%d images has been processed.\nTest results are saved in the \'TestResult.txt\'."), m_iCountMesuredImages);
		AfxMessageBox(MsgStr, MB_OK | MB_ICONINFORMATION);
		ShellExecute(NULL, _T("open"), _T("TestResult.txt"), NULL, _T(".\\"), SW_SHOW);
	}

	m_testStartButton.EnableWindow();
	m_testStopButton.EnableWindow(FALSE);

    m_IntensityString.SetWindowText(_T("0,0,0"));
}

void CVCSampleLIntensityDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	CString valueStr;

	if(pScrollBar == (CScrollBar*) &m_gainSlider)
	{
        status = J_Camera_SetValueInt64(m_hCam, (int8_t*)"GainRaw", (int64_t)m_gainSlider.GetPos());
		valueStr.Format(_T("%d"), m_gainSlider.GetPos());
		m_gainStatic.SetWindowText(valueStr);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CVCSampleLIntensityDlg::OnDestroy()
{
	CDialog::OnDestroy();

	OnBnClickedStopbutton();

	CloseFactoryAndCamera();

    // Free the UI Critical section again
    DeleteCriticalSection(&m_UICriticalSection);
}

void CVCSampleLIntensityDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

	// allocate memory for converted image only once per test
    if (m_CnvImageInfo.pImageBuffer == NULL)
    {
        status = J_Image_Malloc(pAqImageInfo, &m_CnvImageInfo);
	    if(status != J_ST_SUCCESS) 
	    {
		    ShowErrorMsg(CString("Unable to Malloc image buffer!"),status);
		    return;
	    }
    }

    //convert raw to tiff 
	status = J_Image_FromRawToImage(pAqImageInfo, &m_CnvImageInfo);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("Unable to convert image from Raw to Image format!"),status);
		return;
	}

	if(m_hView)
	{
		status = J_Image_ShowImage(m_hView, &m_CnvImageInfo);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Unable to show image!"),status);
			return;
		}
	}

	J_tBGR48 RGBMean;

	status = J_Image_GetAverage(&m_CnvImageInfo, &m_RectToMeasure, &RGBMean);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("Unable to get image average!"),status);
		return;
	}
	
    switch (m_pixelformat)
    {
        case J_GVSP_PIX_MONO10:
        case J_GVSP_PIX_MONO10_PACKED:
        case J_GVSP_PIX_BAYGR10:
        case J_GVSP_PIX_BAYRG10:
        case J_GVSP_PIX_BAYGB10:
        case J_GVSP_PIX_BAYBG10:
        case J_GVSP_PIX_RGB10_PACKED:
        case J_GVSP_PIX_BGR10_PACKED:
        case J_GVSP_PIX_RGB10_PLANAR:
		    RGBMean.B16 = (RGBMean.B16 & 0xFFC0) >> 6;
		    RGBMean.G16 = (RGBMean.G16 & 0xFFC0) >> 6;
		    RGBMean.R16 = (RGBMean.R16 & 0xFFC0) >> 6;
            break;

        case J_GVSP_PIX_MONO12:
        case J_GVSP_PIX_MONO12_PACKED:
        case J_GVSP_PIX_BAYGR12:
        case J_GVSP_PIX_BAYRG12:
        case J_GVSP_PIX_BAYGB12:
        case J_GVSP_PIX_BAYBG12:
        case J_GVSP_PIX_RGB12_PACKED:
        case J_GVSP_PIX_BGR12_PACKED:
        case J_GVSP_PIX_RGB12_PLANAR:
		    RGBMean.B16 = (RGBMean.B16 & 0xFFF0) >> 4;
		    RGBMean.G16 = (RGBMean.G16 & 0xFFF0) >> 4;
		    RGBMean.R16 = (RGBMean.R16 & 0xFFF0) >> 4;
            break;
    }

/*
	//To calcualte mean in float type
	uint8_t iPixel8[3];
	uint16_t iPixel16[3];
	uint32_t iSum[3];
	float fMean[3];
	uint8_t *pPixel = cnvImageInfo.pImageBuffer;
	memset(iSum, 0, sizeof(uint32_t)*3);
	

	if(m_pixelformat == J_GVSP_PIX_MONO8)
	{
		for(uint32_t i = 0; i < cnvImageInfo.iImageSize; i++)
		{
			iSum[0] += *pPixel;
			pPixel++;
		}
		fMean[0] = (float) iSum[0] / (float) cnvImageInfo.iImageSize;
	}
	else if(m_pixelformat == J_GVSP_PIX_MONO10)
	{
		for(uint32_t i = 0; i < cnvImageInfo.iSizeX * cnvImageInfo.iSizeY; i++)
		{
			memcpy(iPixel16, pPixel, sizeof(uint16_t));
			iPixel16[0] = (iPixel16[0] & 0xFFC0) >> 6;
			iSum[0] += iPixel16[0];
			pPixel += 2;
		}
		fMean[0] = (float) iSum[0] / ((float) cnvImageInfo.iSizeX * (float) cnvImageInfo.iSizeY);
	}
	else if((m_pixelformat == J_GVSP_PIX_BAYGR8)||(m_pixelformat == J_GVSP_PIX_BAYGB8)||(m_pixelformat == J_GVSP_PIX_BAYRG8)||(m_pixelformat == J_GVSP_PIX_BAYRB8))
	{
		for(uint32_t i = 0; i < cnvImageInfo.iSizeX * cnvImageInfo.iSizeY; i++)
		{
			memcpy(iPixel8, pPixel, sizeof(uint8_t) * 3);
			for(uint8_t j = 0; j < 3; j++)	iSum[j] += iPixel8[j];
		}
		for(uint8_t i = 0; i < 3; i++)
			fMean[i] = (float) iSum[i] / ((float) cnvImageInfo.iSizeX * (float) cnvImageInfo.iSizeY);
	}
	else if((m_pixelformat == J_GVSP_PIX_BAYGR10)||(m_pixelformat == J_GVSP_PIX_BAYGB10)||(m_pixelformat == J_GVSP_PIX_BAYRG10)||(m_pixelformat == J_GVSP_PIX_BAYRB10))
	{
		for(uint32_t i = 0; i < cnvImageInfo.iSizeX * cnvImageInfo.iSizeY; i++)
		{
			memcpy(iPixel16, pPixel, sizeof(uint16_t) * 3);
			for(uint8_t j = 0; j < 3; j++)
			{	
				iPixel16[j] = (iPixel16[j] && 0xFFC0) >> 6; 
				iSum[j] += iPixel16[j];
			}
		}
		for(uint8_t i = 0; i < 3; i++)
			fMean[i] = (float) iSum[i] / ((float) cnvImageInfo.iSizeX * (float) cnvImageInfo.iSizeY);
	}
*/

    if(m_fp)
	{
        // Check the ImageFormat and decide if it is color or monochrome
         switch (m_pixelformat)
        {
            case J_GVSP_PIX_MONO8:
            case J_GVSP_PIX_MONO10:
            case J_GVSP_PIX_MONO10_PACKED:
            case J_GVSP_PIX_MONO12:
            case J_GVSP_PIX_MONO12_PACKED:
    			fprintf(m_fp, "%d \n", RGBMean.G16);
                break;

            case J_GVSP_PIX_BAYGR8:
            case J_GVSP_PIX_BAYRG8:
            case J_GVSP_PIX_BAYGB8:
            case J_GVSP_PIX_BAYBG8:
            case J_GVSP_PIX_BAYGR10:
            case J_GVSP_PIX_BAYRG10:
            case J_GVSP_PIX_BAYGB10:
            case J_GVSP_PIX_BAYBG10:
            case J_GVSP_PIX_RGB10_PACKED:
            case J_GVSP_PIX_BGR10_PACKED:
            case J_GVSP_PIX_RGB10_PLANAR:
            case J_GVSP_PIX_BAYGR12:
            case J_GVSP_PIX_BAYRG12:
            case J_GVSP_PIX_BAYGB12:
            case J_GVSP_PIX_BAYBG12:
            case J_GVSP_PIX_RGB12_PACKED:
            case J_GVSP_PIX_BGR12_PACKED:
            case J_GVSP_PIX_RGB12_PLANAR:
    			fprintf(m_fp, "%d\t%d\t%d \n",RGBMean.R16, RGBMean.G16, RGBMean.B16);
                break;
        }
	}

    // Protect the update of the UI values
    EnterCriticalSection(&m_UICriticalSection);
    m_RIntensity = RGBMean.R16;
    m_GIntensity = RGBMean.G16;
    m_BIntensity = RGBMean.B16;
	m_iCountMesuredImages++;
    LeaveCriticalSection(&m_UICriticalSection);
}
void CVCSampleLIntensityDlg::OnTimer(UINT_PTR nIDEvent)
{
    // UI update timer
    if (nIDEvent == 1)
    {
        CString intensityString;
        CString sampleNumberString;

        // Protect from update when reading
        EnterCriticalSection(&m_UICriticalSection);
        intensityString.Format(_T("%d,%d,%d"), m_RIntensity, m_GIntensity, m_BIntensity);
        sampleNumberString.Format(_T("%d"),m_iCountMesuredImages);
        LeaveCriticalSection(&m_UICriticalSection);

        m_IntensityString.SetWindowText(intensityString);
        m_SampleNumber.SetWindowText(sampleNumberString);
    }

    CDialog::OnTimer(nIDEvent);
}

void CVCSampleLIntensityDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CVCSampleLIntensityDlg::OnBnClickedOk()
{
	OnOK();
}
