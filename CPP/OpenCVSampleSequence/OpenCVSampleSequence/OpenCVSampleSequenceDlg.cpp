// OpenCVSampleSequenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OpenCVSampleSequence.h"
#include "OpenCVSampleSequenceDlg.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The following code links to the OpenCV libraries.
// It is important to install the correct version of OpenCV before compiling this project
#ifdef _DEBUG
#	pragma comment (lib, "opencv_highgui245d.lib")
#	pragma comment (lib, "opencv_core245d.lib")
#	pragma comment (lib, "opencv_imgproc245d.lib")
#else
#	pragma comment (lib, "opencv_highgui245.lib")
#	pragma comment (lib, "opencv_core245.lib")
#	pragma comment (lib, "opencv_imgproc245.lib")
#endif

// COpenCVSampleSequenceDlg dialog
COpenCVSampleSequenceDlg::COpenCVSampleSequenceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COpenCVSampleSequenceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// initialize factory and camera handle
	m_hFactory	= NULL;
	m_hCamera	= NULL;
	m_hThread	= NULL;

	// initialize openCV Image buffers
	m_cvImgRaw[0] = NULL;		// Raw Image
	m_cvImgRaw[1] = NULL;		// Sequence Image
	m_cvImgBufferWB[0] = NULL;
	m_cvImgBufferWB[1] = NULL;

	m_cvImgBufferColor	= NULL;
	m_cvImgBufferMono	= NULL;
	m_cvImgBuffer32Bit	= NULL;
}

void COpenCVSampleSequenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMLISTCOMBO, m_cameraList);
	DDX_Control(pDX, IDC_CONTINUOUSRADIO, m_continuousRadio);
	DDX_Control(pDX, IDC_SEQTRIGRADIO, m_seqTrigRadio);
	DDX_Control(pDX, IDC_STARTBUTTON, m_startButton);
	DDX_Control(pDX, IDC_STOPBUTTON, m_stopButton);
	DDX_Control(pDX, IDC_STATUSBARSTATIC, m_statusBarStatic);
	DDX_Control(pDX, IDC_CAMERAINFOSTATIC, m_cameraInfoStatic);
	DDX_Control(pDX, IDC_WBALANCEBUTTON, m_wbButton);
}

BEGIN_MESSAGE_MAP(COpenCVSampleSequenceDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CONTINUOUSRADIO, &COpenCVSampleSequenceDlg::OnBnClickedContinuousradio)
	ON_BN_CLICKED(IDC_SEQTRIGRADIO, &COpenCVSampleSequenceDlg::OnBnClickedSeqtrigradio)
	ON_CBN_SELCHANGE(IDC_CAMLISTCOMBO, &COpenCVSampleSequenceDlg::OnCbnSelchangeCamlistcombo)
	ON_BN_CLICKED(IDC_STARTBUTTON, &COpenCVSampleSequenceDlg::OnBnClickedStartbutton)
	ON_BN_CLICKED(IDC_STOPBUTTON, &COpenCVSampleSequenceDlg::OnBnClickedStopbutton)
	ON_BN_CLICKED(IDC_WBALANCEBUTTON, &COpenCVSampleSequenceDlg::OnBnClickedWbalancebutton)
END_MESSAGE_MAP()


// COpenCVSampleSequenceDlg message handlers

BOOL COpenCVSampleSequenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (OpenFactory() == FALSE ) return FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void COpenCVSampleSequenceDlg::OnPaint()
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
HCURSOR COpenCVSampleSequenceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void COpenCVSampleSequenceDlg::InitializeOpenCVImages()
{

	cvDestroyAllWindows();

	// Release openCV Images before initialization
	if (m_cvImgRaw[0] != NULL) cvReleaseImage(&m_cvImgRaw[0]);
	if (m_cvImgRaw[1] != NULL) cvReleaseImage(&m_cvImgRaw[1]);
	if (m_cvImgBufferWB[0] != NULL) cvReleaseImage(&m_cvImgBufferWB[0]);
	if (m_cvImgBufferWB[1] != NULL) cvReleaseImage(&m_cvImgBufferWB[1]);

	if (m_cvImgBufferColor != NULL) cvReleaseImage(&m_cvImgBufferColor);
	if (m_cvImgBufferMono != NULL) cvReleaseImage(&m_cvImgBufferMono);
	if (m_cvImgBuffer32Bit !=NULL) cvReleaseImage(&m_cvImgBuffer32Bit);


	int64_t		iValue = 0;
	
	J_STATUS_TYPE status = J_ST_SUCCESS;

	// define cv img size
	if (m_hCamera)
	{
		//get width
		status = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"Width", &iValue);

		m_cvImgSize.width  = (int)iValue;

		//get height
        status = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"Height", &iValue);

		m_cvImgSize.height = (int) iValue;

		if (m_bSeqTriggerMode)
		{
			// to get sequence ROI size, select Sequence1
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence1");

            status = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeX", &iValue);
			m_cvSeqImgSize.width  = (int)(iValue);

            status = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeY", &iValue);
			m_cvSeqImgSize.height = (int)(iValue);
		}
	}
	else
	{
		return;
	}

	// define openCV display image buffers 
	m_cvImgBufferMono = cvCreateImage(m_cvImgSize, IPL_DEPTH_8U,1);		// Mono image buffer
	m_cvImgBufferColor = cvCreateImage(m_cvImgSize, IPL_DEPTH_8U, 3);   // Color image buffer
	m_cvImgBuffer32Bit = cvCreateImage(m_cvImgSize, IPL_DEPTH_32F, 3);	// 32bit for color interpolation
	m_cvImgRaw[0] = cvCreateImage(m_cvImgSize, IPL_DEPTH_8U, 1); // raw image buffer for max size

	// initialize CV Img Buffers
	cvSetZero(m_cvImgBuffer32Bit);
	cvSetZero(m_cvImgBufferColor);
	cvSetZero(m_cvImgBufferMono);
	cvSetZero(m_cvImgRaw[0]);

	if (m_bSeqTriggerMode)
	{
		m_cvImgRaw[1] = cvCreateImage(m_cvSeqImgSize, IPL_DEPTH_8U, 1); // raw image buffer for sequence size
		cvSetZero(m_cvImgRaw[1]);
	}



	if (m_bBayerCamera)
	{
		// initialize white balance buffer
		double rGain, gGain, bGain;
		TCHAR cbuff[100];
		CString sFileName = _T(".\\rgbSettings");
		sFileName += m_sCameraMac;
		sFileName += ".ini";

		GetPrivateProfileString(_T("WhiteBalance"), _T("RedGain"), _T("1"), cbuff, sizeof(cbuff), sFileName);
		rGain = _tstof(cbuff);

		GetPrivateProfileString(_T("WhiteBalance"), _T("GreenGain"), _T("1"), cbuff, sizeof(cbuff), sFileName);
		gGain = _tstof(cbuff);

		GetPrivateProfileString(_T("WhiteBalance"), _T("BlueGain"), _T("1"), cbuff, sizeof(cbuff), sFileName);
		bGain = _tstof(cbuff);

		// Initialze Buffer for wb
		//m_cvImgBufferWB[i] = cvCloneImage(m_cvImgBufferColor);
		m_cvScalarWB.val[0] = rGain;	// 
		m_cvScalarWB.val[1] = gGain;
		m_cvScalarWB.val[2] = bGain;
		m_cvScalarWB.val[3] = 1;

		m_cvImgBufferWB[0] = cvCreateImage(m_cvImgSize, IPL_DEPTH_32F, 3); // 32bit
		m_cvImgBufferWB[1] = cvCreateImage(m_cvImgSize, IPL_DEPTH_32F, 3); // 32bit

		cvSet(m_cvImgBufferWB[0], m_cvScalarWB);

		if (m_bSeqTriggerMode)
		{
			cvSet(m_cvImgBufferWB[1], m_cvScalarWB);
		}
	}
	
	// Initialize Display
	cvNamedWindow("GigE Image Viewer", 0);
	cvMoveWindow("GigE Image Viewer", 306, -1);		// Define window position
	cvResizeWindow("GigE Image Viewer", m_cvImgSize.width, m_cvImgSize.height);// Define window size
}

void COpenCVSampleSequenceDlg::ConvertAndDisplay()
{

	CvRect		cvRectToDisplay;
	CString str;

	// initialize m_cvRect size
	cvRectToDisplay.height = m_cvImgRaw[m_bSeqTriggerMode]->height;	// height
	cvRectToDisplay.width = m_cvImgRaw[m_bSeqTriggerMode]->width;	// width
	cvRectToDisplay.x = 0;		// offset x 
	cvRectToDisplay.y = 0;		// offset y

	// set m_cvRect position
	// if it is a Sequence image
	if (m_bSeqTriggerMode)	
	{
		if (m_iSequenceCount == 4) m_iSequenceCount = 0;

		m_iSequenceCount++;
		int iGainValue;

		switch (m_iSequenceCount)
		{
		case 1: iGainValue = GAIN_SEQ_1; 
			break;
		case 2:
			cvRectToDisplay.x = cvRectToDisplay.width; // Sequence 2 position: top right
			iGainValue = GAIN_SEQ_2;
			break;
		case 3:
			cvRectToDisplay.y = cvRectToDisplay.height; // Sequence 3 position: bottom left 
			iGainValue = GAIN_SEQ_3;
			break;
		case 4:
			cvRectToDisplay.x = cvRectToDisplay.width;	// Sequence 4 posiiton: bottom right
			cvRectToDisplay.y = cvRectToDisplay.height;
			iGainValue = GAIN_SEQ_4;
			break;
		}

		str.Format(_T("Sequence Image %d: Gain %d"), m_iSequenceCount, iGainValue);

	}
	else 
	{
		str.Format(_T("Image Size %d x %d"), m_cvImgSize.width, m_cvImgSize.height); 
	}
		
	CvFont font;
	cvInitFont(&font, CV_FONT_VECTOR0, 0.6, 0.6, 0.0, 2);
	CvPoint cvTxPt;
	cvTxPt.x = cvRectToDisplay.x;
	cvTxPt.y = cvRectToDisplay.y + 15;
	
	if (m_bBayerCamera)
	{

		if (m_bSeqTriggerMode && m_iSequenceCount == 1) 
		{
			// if it is Sequence image and the Sequcne 1, empty the image display buffer
			cvSetZero(m_cvImgBufferColor);
		}

		// Set image ROI to given rectangle
		cvSetImageROI(m_cvImgBufferColor, cvRectToDisplay);

		cvSetImageROI(m_cvImgBufferWB[m_bSeqTriggerMode], cvRectToDisplay);

		cvSetImageROI(m_cvImgBuffer32Bit, cvRectToDisplay);

		// color interpolation
		cvCvtColor(m_cvImgRaw[m_bSeqTriggerMode], m_cvImgBufferColor, CV_BayerBG2BGR);

		// copy 8bit buffer to 32bit
		cvConvertScale(m_cvImgBufferColor, m_cvImgBuffer32Bit);

		// multiply 32Bit data and WB data
		cvMul(m_cvImgBuffer32Bit, m_cvImgBufferWB[m_bSeqTriggerMode], m_cvImgBuffer32Bit);
		
		// convert 32bit to 8bit
		cvConvertScale(m_cvImgBuffer32Bit, m_cvImgBufferColor);

		// release image roi
		cvResetImageROI(m_cvImgBufferColor);

        CT2A ascii(str); 
		cvPutText(m_cvImgBufferColor, ascii, cvTxPt, &font, CV_RGB(190,190,190)); // grey color

		// display image
		cvShowImage("GigE Image Viewer", m_cvImgBufferColor);
	}
	// Monochrome display
	else if(m_bBayerCamera == false)
	{
		if (m_bSeqTriggerMode && m_iSequenceCount == 1) 
		{
			// if it is Sequence image and the Sequcne 1, empty the image display buffer
			cvSetZero(m_cvImgBufferMono);
		}

		// Sets image ROI to given rectangle
		cvSetImageROI(m_cvImgBufferMono, cvRectToDisplay);
		
		// copy the image to imagebuffer mono
		cvCopy(m_cvImgRaw[m_bSeqTriggerMode], m_cvImgBufferMono);

		// Release image ROI 
		cvResetImageROI(m_cvImgBufferMono);

        CT2A ascii(str); 
		cvPutText(m_cvImgBufferMono, ascii, cvTxPt, &font, CV_RGB(190,190,190));

		// display
		cvShowImage("GigE Image Viewer", m_cvImgBufferMono);
	}
}


void COpenCVSampleSequenceDlg::WhiteBalance()
{
	m_cvScalarWB = cvAvg(m_cvImgBufferColor);
	m_cvScalarWB.val[0] = m_cvScalarWB.val[1] / m_cvScalarWB.val[0];	// R gain
	m_cvScalarWB.val[2] = m_cvScalarWB.val[1] / m_cvScalarWB.val[2];	// B gain
	m_cvScalarWB.val[1] = 1;											// G gain
	cvSet(m_cvImgBufferWB[0], m_cvScalarWB);
	cvSet(m_cvImgBufferWB[1], m_cvScalarWB);

	CString sGain;
	CString sFileName = _T(".\\rgbSettings");
	sFileName += m_sCameraMac;
	sFileName += ".ini";

	double rGain, gGain, bGain;
	rGain = m_cvScalarWB.val[0];
	gGain = m_cvScalarWB.val[1];
	bGain = m_cvScalarWB.val[2];

	sGain.Format(_T("%2.5f"), rGain);
	WritePrivateProfileString(_T("WhiteBalance"), _T("RedGain"), sGain, sFileName);
	sGain.Format(_T("%2.5f"), gGain);
	WritePrivateProfileString(_T("WhiteBalance"), _T("GreenGain"), sGain, sFileName);
	sGain.Format(_T("%2.5f"), bGain);
	WritePrivateProfileString(_T("WhiteBalance"), _T("BlueGain"), sGain, sFileName);
}
void COpenCVSampleSequenceDlg::SendSoftwareTrigger()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

    // We trigger the PulseGenerator0 with SoftwareTrigger0 to create the triggers:
    // SoftwareTrigger0=0
    // SoftwareTrigger0=1
    // SoftwareTrigger0=0

    status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
    if(status != J_ST_SUCCESS) 
    {
	    ShowErrorMsg(CString("Could not set SoftwareTrigger0!"), status);
	    return;
    }

    status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 1);
    if(status != J_ST_SUCCESS) 
    {
	    ShowErrorMsg(CString("Could not set SoftwareTrigger0!"), status);
	    return;
    }

    status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SoftwareTrigger0", 0);
    if(status != J_ST_SUCCESS) 
    {
	    ShowErrorMsg(CString("Could not set SoftwareTrigger0!"), status);
	    return;
    }
}
void COpenCVSampleSequenceDlg::DisplayCameraInfo(int index)
{
	CString ModelName;
	CString CamInfo;

	m_cameraList.GetLBText(index, ModelName);

	if (_tcsstr(ModelName, _T("CM-200GE"))) CamInfo = "Model: CM-200GE\n" 
												      "Resolution: 1624 x 1236\n" 
												      "Frame Rate: 25fps\n"
												      "Cell Size: 4.4\u00B5m x 4.4\u00B5m";
	else if (_tcsstr(ModelName, _T("CB-200GE"))) CamInfo = "Model: CB-200GE\n"
												"Resolution: 1624 x 1236\n" 
												"Frame Rate: 25fps\n" 
												"Cell Size: 4.4\u00B5m x 4.4\u00B5m";
	else if (_tcsstr(ModelName, _T("CM-140GE"))) CamInfo = "Model: CM-140GE\nResolution: 1392 x 1040\nFrame Rate: 31.08fps\nCell Size: 4.65\u00B5m x 4.65\u00B5m";
	else if (_tcsstr(ModelName, _T("CB-140GE"))) CamInfo = "Model: CB-140GE\nResolution: 1392 x 1040\nFrame Rate: 31.08fps\nCell Size: 4.65\u00B5m x 4.65\u00B5m";
	else if (_tcsstr(ModelName, _T("CM-080GE"))) CamInfo = "Model: CM-080GE\nResolution: 1032 x 778\nFrame Rate: 30fps\nCell Size: 4.65\u00B5m x 4.65\u00B5m";
	else if (_tcsstr(ModelName, _T("CB-080GE"))) CamInfo = "Model: CB-080GE\nResolution: 1032 x 778\nFrame Rate: 30fps\nCell Size: 4.65\u00B5m x 4.65\u00B5m";
	else if (_tcsstr(ModelName, _T("CM-040GE"))) CamInfo = "Model: CM-040GE\nResolution: 776 x 582\nFrame Rate: 61.15fps\nCell Size: 8.3\u00B5m x 8.3\u00B5m";
	else if (_tcsstr(ModelName, _T("CB-040GE"))) CamInfo = "Model: CB-040GE\nResolution: 776 x 582\nFrame Rate: 61.15fps\nCell Size: 8.3\u00B5m x 8.3\u00B5m";
	else if (_tcsstr(ModelName, _T("CM-030GE"))) CamInfo = "Model: CM-030GE\nResolution: 656 x 492\nFrame Rate: 90.5fps\nCell Size: 7.4\u00B5m x 7.4\u00B5m";
	else if (_tcsstr(ModelName, _T("CB-030GE"))) CamInfo = "Model: CB-030GE\nResolution: 656 x 492\nFrame Rate: 90.5fps\nCell Size: 7.4\u00B5m x 7.4\u00B5m";
	else 
		m_statusBarStatic.SetWindowText(_T("Unknown Device Name!"));

	m_cameraInfoStatic.SetWindowText(CamInfo);
}

void COpenCVSampleSequenceDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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

BOOL COpenCVSampleSequenceDlg::OpenFactory()
{
	//Open Factory and get list of cameras
	J_STATUS_TYPE	status = J_ST_SUCCESS;
	bool8_t			bHasChanged = false;
	uint32_t		iNumOfCameras =	0;

    status = J_Factory_Open((int8_t*)"", &m_hFactory);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("Could not open factory!"), status);
		return FALSE;
	}

	// Decice Discovery
	status = J_Factory_UpdateCameraList(m_hFactory, &bHasChanged); 
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("Could not update camera list!"), status);
		return FALSE;
	}

	//Get number of cameras
	status = J_Factory_GetNumOfCameras(m_hFactory, &iNumOfCameras);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("Could not get number of cameras!"), status);
		return FALSE;
	}

	if(iNumOfCameras == 0)
	{
		AfxMessageBox(_T("No camera found!"), MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}
	else
	{
		m_cameraList.Clear();

		for (uint32_t index = 0; index < iNumOfCameras; index++)
		{
			uint32_t iCameraIdSize = J_CAMERA_ID_SIZE;

			status = J_Factory_GetCameraIDByIndex(m_hFactory, index, m_CameraId, &iCameraIdSize);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not get camera ID!"), status);
				return FALSE;
			}
			
			//check if the interface is filter driver
			if (strstr((char*)m_CameraId, "INT=>FD"))
			{
				int8_t   sCameraInfo[J_CAMERA_INFO_SIZE];
				uint32_t iCameraInfoSize = J_CAMERA_INFO_SIZE;
				CString  sCameraModel;

				// get model name
				status = J_Factory_GetCameraInfo(m_hFactory, m_CameraId, CAM_INFO_MODELNAME, sCameraInfo, &iCameraInfoSize);
				if(status != J_ST_SUCCESS) 
				{
					ShowErrorMsg(CString("Could not get camera info!"), status);
					return FALSE;
				}

				sCameraModel = (char*)sCameraInfo;

				m_cameraList.InsertString(index, sCameraModel);
			}

		}//end of camera index

		// select first one from the camera list
		m_cameraList.SetCurSel(0); 

		// open the first camera
		OnCbnSelchangeCamlistcombo();

		m_continuousRadio.EnableWindow();
		m_seqTrigRadio.EnableWindow();

		m_sStatusBar.Format(_T("%d camera(s) found. Select a camera from the camera list"), (int)(iNumOfCameras / 2) );
		m_statusBarStatic.SetWindowText(m_sStatusBar);

	}

	return TRUE;
}

void COpenCVSampleSequenceDlg::CloseFactory()
{
	J_STATUS_TYPE	status = J_ST_SUCCESS;

	if(m_hFactory)
	{
		status = J_Factory_Close(m_hFactory);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not close factory!"), status);
			return;
		}

		m_hFactory = NULL;
	}
}


void COpenCVSampleSequenceDlg::CloseCamera()
{
	J_STATUS_TYPE	status = J_ST_SUCCESS;

	if (m_hCamera)
	{
		// close camera
		status = J_Camera_Close(m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not open camera!"), status);
			return;
		}

		m_hCamera = NULL;
	}
}

void COpenCVSampleSequenceDlg::InitializePixelFormat()
{
	// this member funtion initialize m_bBayercamera[] and sets pixel depth to 8 bit
	// because openCV supports only 8bit pixel format
	
	J_STATUS_TYPE	status = J_ST_SUCCESS;
	NODE_HANDLE		hNode;
    int8_t buff[80];
	uint32_t iSize =  sizeof(buff);
	CString msgStr;
	CString pixelformatStr;
	int64_t iNodeValue;

	if(m_hCamera)
	{

		// We need the pixel format value for the image handling
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"PixelFormat", &hNode);
		status = J_Node_GetValueInt64(hNode, 0, &iNodeValue);
		status = J_Node_GetValueString(hNode, 0, buff, &iSize);

		switch (iNodeValue)
		{
		case J_GVSP_PIX_MONO8:
		case J_GVSP_PIX_MONO10:
		case J_GVSP_PIX_MONO10_PACKED:
		case J_GVSP_PIX_MONO12:
		case J_GVSP_PIX_MONO12_PACKED:
			{
				m_bBayerCamera = false;
				// set Piexel format to 8 bit
				status = J_Node_SetValueInt64(hNode, 0, (int64_t) J_GVSP_PIX_MONO8);
				break;
			}
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
			{
				m_bBayerCamera = true;
				// set Piexel format to 8 bit
				status = J_Node_SetValueInt64(hNode, 0, (int64_t) J_GVSP_PIX_BAYRG8);
				break;
			}
		default:
            pixelformatStr = (char*)buff;
			msgStr.Format(_T("Error: %s is unsupported pixel format!"), pixelformatStr);
			AfxMessageBox(msgStr, MB_OKCANCEL | MB_ICONINFORMATION);
			return;
		}
	}
}
void COpenCVSampleSequenceDlg::PrepareContinuousSetting()
{
	if(m_hCamera)
	{
		J_STATUS_TYPE status;
		NODE_HANDLE hNode;
		char cBuff[80];
		uint32_t iSize = sizeof(cBuff);
		int64_t iValue = 0;

        // Set up the camera for continuous exposure mode

        // We have two possible ways of setting up triggers: JAI or GenICam SFNC
        // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
        // called "TriggerSelector". Therefor we have to determine which way to use here.
        // First we see if a node called "TriggerSelector" exists.
        hNode = NULL;
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"TriggerSelector", &hNode);

        // Does the "TriggerSelector" node exist?
        if ((status == J_ST_SUCCESS) && (hNode != NULL))
        {
            // Here we assume that this is GenICam SFNC trigger so we do the following:
            // TriggerSelector=FrameStart
            // TriggerMode=Off
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
            if (status != J_ST_SUCCESS)
            {
                ShowErrorMsg(CString("Could not set TriggerSelector!"), status);
                return;
            }
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerMode", (int8_t*)"Off");
            if (status != J_ST_SUCCESS)
            {
                ShowErrorMsg(CString("Could not set TriggerMode!"), status);
                return;
            }
        }
        else
        {
            // Here we assume that this is JAI trigger so we do the following:
            // ExposureMode=Continuous

		    // Set ExposureMode="Continuous"		
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"ExposureMode", (int8_t*)"Continuous");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set ExposureMode!"), status);
			    return;
		    }
        }

		// set ROI
		// set Offset to 0
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"OffsetX", 0);
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"OffsetY", 0);

		//get max width, and set width to max value
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Width", &hNode);
		status = J_Node_GetMaxInt64(hNode, &iValue); 
		status = J_Node_SetValueInt64(hNode, 0, iValue); 

		//get max height, and set height to max value
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Height", &hNode);
		status = J_Node_GetMaxInt64(hNode, &iValue); 
		status = J_Node_SetValueInt64(hNode, 0, iValue); 

	} // end of if(m_hCamera)

}

void COpenCVSampleSequenceDlg::PrepareSeqTrigSetting()
{
	if(m_hCamera)
	{
		J_STATUS_TYPE status;
		NODE_HANDLE hNode;
		char cBuff[80];
		uint32_t iSize = sizeof(cBuff);

		int64_t iSeqRoiX = 0;
		int64_t iSeqRoiY = 0;
		int64_t iWidth = 0;		// to store Sequence Offset X
		int64_t iHeight = 0;	// to store Sequence Offset X
		int64_t iValue = 0;

		// verify sequence value  
		//get Width to max width
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Width", &hNode);
		status = J_Node_GetMaxInt64(hNode, &iWidth); 
		
		// try to set width to a half the max value 
		iSeqRoiX = (uint64_t)(iWidth / 2);
		status =  J_Node_SetValueInt64(hNode, 0, iSeqRoiX); 
		
		// get width value to verify whether it is set to a half the max value
		status = J_Node_GetValueInt64(hNode, 0, &iValue); 
		if (iSeqRoiX != iValue) 		iSeqRoiX = iValue;

		// Set Width to Max value
		status =  J_Node_SetValueInt64(hNode, 0, iWidth); 

		// get max height
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Height", &hNode);
		status =  J_Node_GetMaxInt64(hNode, &iHeight); 

		// try to set height to a half the max value 
		iSeqRoiY = (uint64_t)(iHeight / 2);
		status =  J_Node_SetValueInt64(hNode, 0, iSeqRoiY);

		// get height value to verify whether it is set to a half the max value
		status = J_Node_GetValueInt64(hNode, 0, &iValue); 
		if (iSeqRoiY != iValue)			iSeqRoiY = iValue;

		// Set Height to Max value
		status =  J_Node_SetValueInt64(hNode, 0, iHeight); 

        // We have two possible ways of setting up triggers: JAI or GenICam SFNC
        // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
        // called "TriggerSelector". Therefor we have to determine which way to use here.
        // First we see if a node called "TriggerSelector" exists.
        hNode = NULL;
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"TriggerSelector", &hNode);

        // Does the "TriggerSelector" node exist?
        if ((status == J_ST_SUCCESS) && (hNode != NULL))
        {
            // Here we assume that this is GenICam SFNC trigger so we do the following:
            // TriggerSelector=FrameStart
            // TriggerMode=On
            // TriggerSource=PulseGenerator0
            // SequenceMode=On

            // Set TriggerSelector="FrameStart"
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set TriggerSelector!"), status);
			    return;
		    }

		    // Set TriggerMode="On"
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerMode", (int8_t*)"On");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set TriggerMode!"), status);
			    return;
		    }

		    // Set TriggerSource="Software" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSource", (int8_t*)"PulseGenerator0");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set TriggerSource!"), status);
			    return;
		    }

            // Set SequenceMode="On" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceMode", (int8_t*)"On");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set SequenceMode!"), status);
			    return;
		    }

        }
        else
        {
            // Camera get triggerred from Pulse Generator, and
    		// Pulse Generator's source is Software Trigger

            // Here we assume that this is JAI trigger so we do the following:
            // ExposureMode=SequentialEPSTrigger
            // LineSelector=CameraTrigger0
            // LineSource=PulseGenerator0

		    // Set ExposureMode="SequentialEPSTrigger"
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"ExposureMode", (int8_t*)"SequentialEPSTrigger");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set ExposureMode!"), status);
			    return;
		    }

		    // Set LineSelector="CameraTrigger0" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSelector", (int8_t*)"CameraTrigger0");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set LineSelector!"), status);
			    return;
		    }

		    // Set LineSource="PulseGenerator0" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSource", (int8_t*)"PulseGenerator0");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set LineSource!"), status);
			    return;
		    }
        }

		// Pulse Generator Settings
		// Use 25MHz Clock Source, and 2500 PreScaler because this makes the frequency 10,000Hz (0.1ms/tick)
		// so it is easy to calculate the pulse length
        status = J_Camera_SetValueString(m_hCamera, (int8_t*)"ClockSource", (int8_t*)"MHz25");

		// Clock Pre-scaler to 2500
		iValue = 2500;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"ClockPreScaler", iValue);


        // We have two possible ways of setting Pulse Generators: JAI or GenICam SFNC
        // First we see if a node called "PulseGeneratorClearSource" exists.
        hNode = NULL;
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"PulseGeneratorClearSource", &hNode);

        // Does the "PulseGeneratorClearSource" node exist?
        if ((status == J_ST_SUCCESS) && (hNode != NULL))
        {
            // We use SoftwareTrigger0 to trigger PulseGenerator0
            // We then have to set the Clear Source for the PulseGenerator to SoftwareTrigger0
            // PulseGeneratorSelector=PulseGenerator0
            // PulseGeneratorClearSource[PulseGeneratorSelector]=SoftwareTrigger0
            // PulseGeneratorLength[PulseGeneratorSelector]=PULSE_GEN_LENGTH
            // PulseGeneratorStartPoint[PulseGeneratorSelector]=1
            // PulseGeneratorEndPoint[PulseGeneratorSelector]=100
            // PulseGeneratorClearActivation[PulseGeneratorSelector]=FallingEdge

            // Set PulseGeneratorSelector="PulseGenerator0" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"PulseGeneratorSelector", (int8_t*)"PulseGenerator0");
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorSelector!"), status);
		        return;
	        }

	        // Set PulseGeneratorClearSource="SoftwareTrigger0" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"PulseGeneratorClearSource", (int8_t*)"SoftwareTrigger0");
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorClearSource!"), status);
		        return;
	        }

		    // Set Pulse Generator Length
		    iValue = PULSE_GEN_LENGTH;
            status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"PulseGeneratorLength", iValue);
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorLength!"), status);
		        return;
	        }

		    // Pulse Generator Start Point: 1
		    iValue = 1;
            status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"PulseGeneratorStartPoint", iValue);
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorStartPoint!"), status);
		        return;
	        }

		    // Pulse Generator End Point: 100
		    iValue = 100;
            status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"PulseGeneratorEndPoint", iValue);
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorEndPoint!"), status);
		        return;
	        }

		    // Pulse Generator Repeat Count:
		    iValue = PULSE_GEN_REPEAT_COUNT;
            status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"PulseGeneratorRepeatCount", iValue);
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorRepeatCount!"), status);
		        return;
	        }

		    // Clear Mode for the Pulse Generators: Falling Edge
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"PulseGeneratorClearActivation", (int8_t*)"FallingEdge");
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorClearActivation!"), status);
		        return;
	        }
        }
        else
        {
            // We use SoftwareTrigger0 to trigger PulseGenerator0
            // LineSelector=PulseGenerator0
            // LineSource=SoftwareTrigger0
            // PulseGeneratorSelector=PulseGenerator0
            // PulseGeneratorLength[PulseGeneratorSelector]=PULSE_GEN_LENGTH
            // PulseGeneratorStartPoint[PulseGeneratorSelector]=1
            // PulseGeneratorEndPoint[PulseGeneratorSelector]=100
            // PulseGeneratorClear[PulseGeneratorSelector]=FallingEdge

            // Set LineSelector="PulseGenerator0" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSelector", (int8_t*)"PulseGenerator0");
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set LineSelector!"), status);
		        return;
	        }

	        // Set LineSource="SoftwareTrigger0" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSource", (int8_t*)"SoftwareTrigger0");
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set LineSource!"), status);
		        return;
	        }
		    // Set Pulse Generator Length
		    iValue = PULSE_GEN_LENGTH;
            status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"PulseGeneratorLength", iValue);
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorLength!"), status);
		        return;
	        }

		    // Pulse Generator Start Point: 1
		    iValue = 1;
            status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"PulseGeneratorStartPoint", iValue);
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorStartPoint!"), status);
		        return;
	        }

		    // Pulse Generator End Point: 100
		    iValue = 100;
            status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"PulseGeneratorEndPoint", iValue);
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorEndPoint!"), status);
		        return;
	        }

		    // Pulse Generator Repeat Count:
		    iValue = PULSE_GEN_REPEAT_COUNT;
            status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"PulseGeneratorRepeatCount", iValue);
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorRepeatCount!"), status);
		        return;
	        }

		    // Clear Mode for the Pulse Generators: Falling Edge
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"PulseGeneratorClear", (int8_t*)"FallingEdge");
	        if(status != J_ST_SUCCESS) 
	        {
		        ShowErrorMsg(CString("Could not set PulseGeneratorClear!"), status);
		        return;
	        }
        }

		// Sequence Settings
		// set Sequence Repetition Count to 0 for infinite repeat
		iValue = 0;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceRepetitions", iValue);

		// set Last Sequence to 4 
		iValue = PULSE_GEN_REPEAT_COUNT;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceEndingPosition", iValue);

		// Sequence1: top left side of the max size image
		// set Sequence Selector: Sequence 1
        status = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence1");

		// set Sequence Master Gain Raw: -89
		iValue = GAIN_SEQ_1;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceMasterGain", iValue);

		// set Sequence ExposureTime: Max
        hNode = NULL;
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceExposureTimeRaw", &hNode);
		iValue = 0;
        status = J_Node_GetMaxInt64(hNode, &iValue);
        status = J_Node_SetValueInt64(hNode, true, iValue);

        // set Sequence ROI Size X
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeX", iSeqRoiX);

		// set Sequence ROI Size Y 
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeY", iSeqRoiY);

		// Set Sequence Offset X
		iValue = 0;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetX", iValue);

		// Set Sequence Offset Y
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetY", iValue);

		// Sequence 2: top right side of the max size image
		// set Sequence Selector: Sequence 2
        status = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence2");

		// set Sequence Master Gain Raw: 100
		iValue = GAIN_SEQ_2;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceMasterGain", iValue);

		// set Sequence ExposureTime: Max
        hNode = NULL;
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceExposureTimeRaw", &hNode);
		iValue = 0;
        status = J_Node_GetMaxInt64(hNode, &iValue);
        status = J_Node_SetValueInt64(hNode, true, iValue);

        // set Sequence ROI Size X
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeX", iSeqRoiX);

		// set Sequence ROI Size Y 
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeY", iSeqRoiY);

		// Set Sequence Offset X
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetX", iSeqRoiX);

		// Set Sequence Offset Y
		iValue = 0;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetY", iValue);

		// Sequence 3: bottom right side of the max size image
		// set Sequence Selector: Sequence 3
        status = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence3");

		// set Sequence Master Gain Raw:
		iValue = GAIN_SEQ_3;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceMasterGain", iValue);

		// set Sequence ExposureTime: Max
        hNode = NULL;
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceExposureTimeRaw", &hNode);
		iValue = 0;
        status = J_Node_GetMaxInt64(hNode, &iValue);
        status = J_Node_SetValueInt64(hNode, true, iValue);

        // set Sequence ROI Size X
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeX", iSeqRoiX);

		// set Sequence ROI Size Y 
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeY", iSeqRoiY);

		// Set Sequence Offset X
		iValue = 0;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetX", iValue);

		// Set Sequence Offset Y
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetY", iSeqRoiY);

		// Sequence 4: bottome right side of the max size image
		// set Sequence Selector: Sequence 4
        status = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence4");

		// set Sequence Master Gain Raw
		iValue = GAIN_SEQ_4;
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceMasterGain", iValue);

		// set Sequence ExposureTime: Max
        hNode = NULL;
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceExposureTimeRaw", &hNode);
		iValue = 0;
        status = J_Node_GetMaxInt64(hNode, &iValue);
        status = J_Node_SetValueInt64(hNode, true, iValue);

        // set Sequence ROI Size X
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeX", iSeqRoiX);

		// set Sequence ROI Size Y 
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROISizeY", iSeqRoiY);

		// Set Sequence Offset X
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetX", iSeqRoiX);

		// Set Sequence Offset Y
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetY", iSeqRoiY);
	} // end of 	if(m_hCamera)
}

void COpenCVSampleSequenceDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// Close Cameras and Factory
	CloseCamera();
	CloseFactory();

	cvDestroyAllWindows();
	// Release openCV Images
	if (m_cvImgRaw[0] != NULL) cvReleaseImage(&m_cvImgRaw[0]);
	if (m_cvImgRaw[1] != NULL) cvReleaseImage(&m_cvImgRaw[1]);
	if (m_cvImgBufferWB[0] != NULL) cvReleaseImage(&m_cvImgBufferWB[0]);
	if (m_cvImgBufferWB[1] != NULL) cvReleaseImage(&m_cvImgBufferWB[1]);

	if (m_cvImgBufferColor != NULL) cvReleaseImage(&m_cvImgBufferColor);
	if (m_cvImgBufferMono != NULL) cvReleaseImage(&m_cvImgBufferMono);
	if (m_cvImgBuffer32Bit !=NULL) cvReleaseImage(&m_cvImgBuffer32Bit);
}

void COpenCVSampleSequenceDlg::OnTimer(UINT_PTR nIDEvent)
{
	// send software trigger
	SendSoftwareTrigger();

	CDialog::OnTimer(nIDEvent);
}

void COpenCVSampleSequenceDlg::OnBnClickedContinuousradio()
{
	m_bSeqTriggerMode = false;

	PrepareContinuousSetting();

	InitializeOpenCVImages();

	m_cameraList.GetLBText(m_iCurrentCameraIndex, m_sStatusBar);

	m_sStatusBar += ": Ready to start free running, press Start button.";
	m_statusBarStatic.SetWindowText(m_sStatusBar);

}


void COpenCVSampleSequenceDlg::OnBnClickedSeqtrigradio()
{
	m_bSeqTriggerMode = true;

	PrepareSeqTrigSetting(); // set camera setting to sequence trigger operation

	InitializeOpenCVImages();

	m_cameraList.GetLBText(m_iCurrentCameraIndex, m_sStatusBar);

	m_sStatusBar += ": Ready to start Sequential Trigger Operation, press Start button.";

	m_statusBarStatic.SetWindowText(m_sStatusBar);

}

void COpenCVSampleSequenceDlg::OnCbnSelchangeCamlistcombo()
{
	// close a previous camera and open a selected camera
	J_STATUS_TYPE	status = J_ST_SUCCESS;
	uint32_t index = m_cameraList.GetCurSel();
	char buff[80];
	uint32_t iSize =  sizeof(buff);
	NODE_HANDLE hNode;

	//if the newly selected camera is the same as previous one, do nothing
	if(m_hCamera && m_iCurrentCameraIndex != index)
	{
		CloseCamera();
	}
	
	if(!m_hCamera)
	{
 		uint32_t iCameraIdSize = J_CAMERA_ID_SIZE;

		status = J_Factory_GetCameraIDByIndex(m_hFactory, index, m_CameraId, &iCameraIdSize);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not get camera ID!"), status);
			return;
		}

		int8_t   sCameraInfo[J_CAMERA_INFO_SIZE];
		uint32_t iCameraInfoSize = J_CAMERA_INFO_SIZE;

		status = J_Factory_GetCameraInfo(m_hFactory, m_CameraId, CAM_INFO_MAC, sCameraInfo, &iCameraInfoSize);
		m_sCameraMac = (char*)sCameraInfo;

		//open the selected camera
		status = J_Camera_Open(m_hFactory, m_CameraId, &m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not open camera!"), status);
			return;
		}

		//check if the camera has sequence feature
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceSelector", &hNode);
		if (status != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Sequence feature is not found!"), status);
			return;
		}

		InitializePixelFormat(); // set Pixel format to 8 bit and recognize bayer 

		m_cameraList.GetLBText(index, m_sStatusBar);
		m_sStatusBar += " has been opened.";

        status = J_Camera_GetValueString(m_hCamera, (int8_t*)"ExposureMode", (int8_t*)buff, &iSize);

		if (strstr(buff, "SequentialEPSTrigger"))
		{
			// Check Sequential Trigger Radio button
			CheckRadioButton(IDC_CONTINUOUSRADIO, IDC_SEQTRIGRADIO, IDC_SEQTRIGRADIO);

			PrepareSeqTrigSetting();

			m_bSeqTriggerMode = true;

			m_sStatusBar += " Ready to start Sequential Trigger Operation, press Start button";
		}
		else 
		{
			// Select Free Running Radio button  
			CheckRadioButton(IDC_CONTINUOUSRADIO, IDC_SEQTRIGRADIO, IDC_CONTINUOUSRADIO);

			PrepareContinuousSetting();

			m_bSeqTriggerMode = false;

			m_sStatusBar += " Ready to start free running, press Start button";
		}

		InitializeOpenCVImages(); // initialize openCV image buffer

		m_iCurrentCameraIndex = index;

		// Display Camera information
		DisplayCameraInfo(index);

		m_startButton.EnableWindow();

		m_statusBarStatic.SetWindowText(m_sStatusBar);
	}

}

void COpenCVSampleSequenceDlg::OnBnClickedStartbutton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

	int64_t		iWidth = 0;
	int64_t		iHeight = 0;
	int64_t		iPixelFormat = 0;

	if (m_hCamera)
	{
        // Get pixelformat from the camera
        J_Camera_GetValueInt64(m_hCamera, (int8_t*)"PixelFormat", &iPixelFormat);

        // Calculate number of bits (not bytes) per pixel using macro
        int bpp = J_BitsPerPixel(iPixelFormat);

		status = J_Image_OpenStream(m_hCamera, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&COpenCVSampleSequenceDlg::StreamCBFunc), &m_hThread,  (uint32_t) ((m_cvImgSize.width * m_cvImgSize.height * bpp)/8) );
        status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStart");
	}

	m_wbButton.EnableWindow(m_bBayerCamera && !m_bSeqTriggerMode); 

	if (m_bSeqTriggerMode)
	{
		m_iSequenceCount = 0;

		// Start timer to send software trigger
		SetTimer(1, SWTRIG_TIMER, NULL);
	}

	m_startButton.EnableWindow(FALSE);
	m_stopButton.EnableWindow();

	m_continuousRadio.EnableWindow(FALSE);
	m_seqTrigRadio.EnableWindow(FALSE);
	m_cameraList.EnableWindow(FALSE);

	m_statusBarStatic.SetWindowText(_T("Press Stop button to stop image acquisision"));

}

void COpenCVSampleSequenceDlg::OnBnClickedStopbutton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

	// Stop timer
	if (m_bSeqTriggerMode)
	{
		KillTimer(1);
	}

	// Stop Acquision
	if (m_hCamera) 
	{
        status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStop");

		// Close stream
		if(m_hThread)
		{
			status = J_Image_CloseStream(m_hThread);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not close stream!"), status);
				return;
			}

			m_hThread = NULL;
		}

	}

	m_cameraList.EnableWindow();

	m_startButton.EnableWindow();
	m_stopButton.EnableWindow(FALSE);

	m_continuousRadio.EnableWindow();
	m_seqTrigRadio.EnableWindow();

	m_wbButton.EnableWindow(FALSE);

	m_statusBarStatic.SetWindowText(_T("Image streaming stopped"));
}

void COpenCVSampleSequenceDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

	//// Skip showing image.
	//if ((pAqImageInfo->iAnnouncedBuffers - pAqImageInfo->iQueuedBuffers) > 1)
	//	return;

	memcpy(m_cvImgRaw[m_bSeqTriggerMode]->imageData, pAqImageInfo->pImageBuffer, pAqImageInfo->iSizeX * pAqImageInfo->iSizeY); 

	ConvertAndDisplay();

}

void COpenCVSampleSequenceDlg::OnBnClickedWbalancebutton()
{
	// perform white balance if it is bayer camera
	if (m_bBayerCamera)
	{
		WhiteBalance();
	}
}