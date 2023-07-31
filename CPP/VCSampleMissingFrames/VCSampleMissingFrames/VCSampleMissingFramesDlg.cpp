// VCSampleMissingFramesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VCSampleMissingFrames.h"
#include "VCSampleMissingFramesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVCSampleMissingFramesDlg dialog



CVCSampleMissingFramesDlg::CVCSampleMissingFramesDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVCSampleMissingFramesDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	m_hFactory	= NULL;
	m_hCamera	= NULL; 
	m_hView		= NULL;
	m_hThread	= NULL;

	// Prepare the critical section
    InitializeCriticalSection(&m_UICriticalSection);
}

void CVCSampleMissingFramesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMLISTCOMBO, m_cameraList);
	DDX_Control(pDX, IDC_CAMERAIDSTATIC, m_cameraIdStatic);
	DDX_Control(pDX, IDC_WIDTHSLIDER, m_widthSlider);
	DDX_Control(pDX, IDC_HEIGHTSLIDER, m_heightSlider);
	DDX_Control(pDX, IDC_GAINSLIDER, m_gainSlider);
	DDX_Control(pDX, IDC_WIDTHSTATIC, m_widthStatic);
	DDX_Control(pDX, IDC_HEIGHTSTATIC, m_heightStatic);
	DDX_Control(pDX, IDC_GAINSTATIC, m_gainStatic);
	DDX_Control(pDX, IDC_STARTBUTTON, m_startButton);
	DDX_Control(pDX, IDC_STOPBUTTON, m_stopButton);
	DDX_Control(pDX, IDC_MISSINGFRAMES_STATIC, m_missingFramesStatic);
	DDX_Control(pDX, IDC_FMISSINGPACKETS_STATIC, m_fMissingPacketsStatic);
	DDX_Control(pDX, IDC_STATUSBARSTATIC, m_statusBarStatic);
	DDX_Control(pDX, IDC_VIEWERPICTURECTRL, m_viewerPictureCtrl);
	DDX_Control(pDX, IDC_DROPFRAMESCHECK, m_dropFramesCheck);

	DDX_Control(pDX, IDC_TTLMISSINGPACKETS_STATIC, m_totalMissingPacketsStatic);
}

BEGIN_MESSAGE_MAP(CVCSampleMissingFramesDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_CAMLISTCOMBO, &CVCSampleMissingFramesDlg::OnCbnSelchangeCamlistcombo)
	ON_BN_CLICKED(IDC_STARTBUTTON, &CVCSampleMissingFramesDlg::OnBnClickedStartbutton)
	ON_BN_CLICKED(IDC_STOPBUTTON, &CVCSampleMissingFramesDlg::OnBnClickedStopbutton)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CVCSampleMissingFramesDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CVCSampleMissingFramesDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CVCSampleMissingFramesDlg message handlers

BOOL CVCSampleMissingFramesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if(OpenFactory() == FALSE) return FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVCSampleMissingFramesDlg::OnPaint()
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
HCURSOR CVCSampleMissingFramesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVCSampleMissingFramesDlg::OnDestroy()
{
	CDialog::OnDestroy();

	OnBnClickedStopbutton();

	CloseFactoryAndCamera();

	// Free the UI Critical section again
    DeleteCriticalSection(&m_UICriticalSection);
}

void CVCSampleMissingFramesDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	CString valueStr;

	if(pScrollBar == (CScrollBar*) &m_widthSlider)
	{
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"Width", (int64_t)m_widthSlider.GetPos());
		valueStr.Format(_T("%d"), m_widthSlider.GetPos());
		m_widthStatic.SetWindowText(valueStr);
	}

	if(pScrollBar == (CScrollBar*) &m_heightSlider)
	{
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"Height", (int64_t)m_heightSlider.GetPos());
		valueStr.Format(_T("%d"), m_heightSlider.GetPos());
		m_heightStatic.SetWindowText(valueStr);
	}


	if(pScrollBar == (CScrollBar*) &m_gainSlider)
	{
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"GainRaw", (int64_t)m_gainSlider.GetPos());
		valueStr.Format(_T("%d"), m_gainSlider.GetPos());
		m_gainStatic.SetWindowText(valueStr);
	}


	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
BOOL CVCSampleMissingFramesDlg::OpenFactory()
{
	J_STATUS_TYPE	status = J_ST_SUCCESS;
	bool8_t			bHasChanged = false;
	uint32_t		iNumOfCameras =	0;

	//Open Factory and get list of cameras
    status = J_Factory_Open((int8_t*)"", &m_hFactory);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(status);
		return FALSE;
	}

	//Device Discovery
	status = J_Factory_UpdateCameraList(m_hFactory, &bHasChanged); 

	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(status);
		return FALSE;
	}

	//Get number of cameras
	status = J_Factory_GetNumOfCameras(m_hFactory, &iNumOfCameras);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(status);
		return FALSE;
	}

	if(iNumOfCameras == 0)
	{
		AfxMessageBox(_T("No camera found!"), MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}
	else if(iNumOfCameras > 0)
	{

		for (uint32_t index = 0; index < iNumOfCameras; index++)
		{
			uint32_t iCameraIdSize = J_CAMERA_ID_SIZE;

			//status = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_CameraId, &iCameraIdSize);
			status = J_Factory_GetCameraIDByIndex(m_hFactory, index, m_CameraId, &iCameraIdSize);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(status);
				return FALSE;
			}

			int8_t   sCameraInfo[J_CAMERA_INFO_SIZE];
			uint32_t iCameraInfoSize = J_CAMERA_INFO_SIZE;
			CString ListItemStr;
			
			//get model name
			status = J_Factory_GetCameraInfo(m_hFactory, m_CameraId, CAM_INFO_MODELNAME, sCameraInfo, &iCameraInfoSize);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(status);
				return FALSE;
			}
			
			ListItemStr = (char*)sCameraInfo;

			//check if the interface is filter driver or socket driver
			if (strstr((char*)m_CameraId, "INT=>FD"))
				ListItemStr += " (Filter Driver)";
			else if (strstr((char*)m_CameraId, "INT=>SD"))
				ListItemStr += " (Socket Driver)";
			else 
            {
				int32_t ret = AfxMessageBox(CString("This sample only works with GigE cameras.\n\nExit application?"), MB_YESNO | MB_ICONSTOP);
				if(IDYES == ret)
				{
					OnOK();
				}
                return FALSE;
            }

			m_cameraList.InsertString(index, ListItemStr); 

			CString statusBar;
			statusBar.Format(_T("%d camera(s) found. Double click to connect to a camera"), iNumOfCameras);
			m_statusBarStatic.SetWindowText(statusBar);

		}//end of camera index

	}

	return TRUE;
}

void CVCSampleMissingFramesDlg::CloseFactoryAndCamera()
{

	J_STATUS_TYPE	status = J_ST_SUCCESS;

	// close camera
	if(m_hCamera)
	{
		status = J_Camera_Close(m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}
		m_hCamera = NULL;
	}

	// close factory
	if(m_hFactory)
	{
		status = J_Factory_Close(m_hFactory);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}
		m_hFactory = NULL;
	}
}
void CVCSampleMissingFramesDlg::ShowErrorMsg(J_STATUS_TYPE error)
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
	}
	
	AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);

}

void CVCSampleMissingFramesDlg::InitializeControls()
{

	if(m_hCamera)
	{
		J_STATUS_TYPE	status = J_ST_SUCCESS;
		NODE_HANDLE		hNode;
        char buff[80] = { 0 };
		uint32_t iSize =  sizeof(buff);
		int64_t minval, maxval, nodeValue;
		CString editStr;


		m_cameraIdStatic.SetWindowText(CString((char*)m_CameraId));
		
		//initialize width slider
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Width", &hNode); // get Width node handle
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}

		status = J_Node_GetMinInt64(hNode, &minval);	// Width min
		status = J_Node_GetMaxInt64(hNode, &maxval);	// Width max
		m_widthSlider.SetRange((int)minval, (int)maxval); // set width slider min and max
		m_widthSlider.SetTicFreq((int)(maxval - minval) / 10); 

		status = J_Node_GetValueInt64(hNode, 0, &nodeValue); // get currunt width value
		m_widthSlider.SetPos((int)nodeValue);	// set the value of the slider

		editStr.Format(_T("%d"), (int)nodeValue);	
		m_widthStatic.SetWindowText(editStr); // set the edit control value

		//initialize height slider
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Height", &hNode); // get height node handle
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}

		status = J_Node_GetMinInt64(hNode, &minval);	// height min
		status = J_Node_GetMaxInt64(hNode, &maxval); // height max
		m_heightSlider.SetRange((int)minval, (int)maxval); // set height slider min and max
		m_heightSlider.SetTicFreq((int)(maxval - minval) / 10); 

		J_Node_GetValueInt64(hNode, 0, &nodeValue); // get currunt height value
		m_heightSlider.SetPos((int)nodeValue);	// set the value of the slider

		editStr.Format(_T("%d"), (int)nodeValue);	
		m_heightStatic.SetWindowText(editStr); // set the edit control value


		// initialize gain slider
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"GainRaw", &hNode);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}

		status = J_Node_GetMinInt64(hNode, &minval); //get min vlaue of the Gainraw node from GenICam 
		status = J_Node_GetMaxInt64(hNode, &maxval); //get max value of the Gainraw node from GenICam
		m_gainSlider.SetRange((int)minval,(int)maxval); //set min, max value in gainSlider
		m_gainSlider.SetTicFreq((int)(maxval - minval) / 10); 

		status = J_Node_GetValueInt64(hNode, 0, &nodeValue);	//get the gain value from GenICam
		m_gainSlider.SetPos((int)nodeValue); //set the gain value of gainSlider

		editStr.Format(_T("%d"),(int)nodeValue);
		m_gainStatic.SetWindowText(editStr);

		m_widthSlider.EnableWindow();
		m_heightSlider.EnableWindow();
		m_gainSlider.EnableWindow();

		m_dropFramesCheck.EnableWindow();
		m_startButton.EnableWindow();

	}
}
void CVCSampleMissingFramesDlg::OnCbnSelchangeCamlistcombo()
{
	// close a previous camera and open a selected camera
	J_STATUS_TYPE	status = J_ST_SUCCESS;

	uint32_t index = m_cameraList.GetCurSel();

	//if the newly selected camera is the same as previous one, do nothing
	if(m_hCamera && m_OldCameraIndex != index)
	{
		status = J_Camera_Close(m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}
		m_hCamera = NULL;
	}
	
	if(!m_hCamera)
	{
		uint32_t iCameraIdSize = J_CAMERA_ID_SIZE;

		status = J_Factory_GetCameraIDByIndex(m_hFactory, index, m_CameraId, &iCameraIdSize);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}

		//open the selected camera
		status = J_Camera_Open(m_hFactory, m_CameraId, &m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}

		InitializeControls();

		m_OldCameraIndex = index;

		CString statusBar;
		m_cameraList.GetLBText(index, statusBar);
		statusBar += " has been opened.";
		m_statusBarStatic.SetWindowText(statusBar);

	}
}

void CVCSampleMissingFramesDlg::OnBnClickedStartbutton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	int64_t		widthMax = 0;
	int64_t		heightMax = 0;
	int64_t		pixelFormat = 0;
	POINT		StartPoint;
	SIZE		MaxViewerSize;
	NODE_HANDLE hNode; 
	CString editStr;

	if(m_hCamera)
	{	
		// drop frames with missing packet if the button is checked
		if (IsDlgButtonChecked(m_dropFramesCheck.GetDlgCtrlID()))
			m_bDropFramesWithMissingPacket = true;
		else 
			m_bDropFramesWithMissingPacket = false;

        char buff[80] = { 0 };
        //int64_t nodeValue;
		uint32_t iSize = sizeof(buff);
			
		m_iCountMissingFrames = 0; // Missing Frames
		m_iCountFramesWithMissingPackets = 0; // Frames with Missing Packets
		m_iCountTotalMissingPackets = 0; // Total Missing Packets
		m_iCountCurruptedFrames = 0; // not received Frames + Frames with Missing Packets
		m_iCountLostFrames = 0;	// received but lost frames
		m_iCountReceivedFrames = 0; // total received frames

		//Set Width to max width
        J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Width", &hNode);
		J_Node_GetMaxInt64(hNode, &widthMax); 
		J_Node_SetValueInt64(hNode, 0, widthMax);

		//Set Height to max height
        J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Height", &hNode);
		J_Node_GetMaxInt64(hNode, &heightMax); 
		J_Node_SetValueInt64(hNode, 0, heightMax);		

		StartPoint.x = 100;	// x coordinate for viewer position
		StartPoint.y = 100;	// y coordinate for viewer position
		MaxViewerSize.cx = (long)widthMax;
		MaxViewerSize.cy = (long)heightMax;

        // Get pixelformat from the camera
        J_Camera_GetValueInt64(m_hCamera, (int8_t*)"PixelFormat", &pixelFormat);

        // Calculate number of bits (not bytes) per pixel using macro
        int bpp = J_BitsPerPixel(pixelFormat);

        RECT frameRect; //to display images in picture control
		m_viewerPictureCtrl.GetClientRect(&frameRect);

		status = J_Image_OpenViewWindowEx(J_IVW_CHILD_STRETCH, _T("Test Light Intensity"), &frameRect, &MaxViewerSize, m_viewerPictureCtrl.m_hWnd, &m_hView);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}
		
		status = J_Image_OpenStream(m_hCamera, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CVCSampleMissingFramesDlg::StreamCBFunc), &m_hThread, (MaxViewerSize.cx * MaxViewerSize.cy * bpp)/8);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}

        status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStart");
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}

		m_startButton.EnableWindow(FALSE);
		m_stopButton.EnableWindow();
		m_widthSlider.EnableWindow(FALSE);
		m_heightSlider.EnableWindow(FALSE);
		m_dropFramesCheck.EnableWindow(FALSE);

		if (m_hView)
		{
			CRect mainWindowRect;
			CRect clientWindowRect;
			this->GetClientRect(&mainWindowRect);
			this->ClientToScreen(&mainWindowRect);

			GetDlgItem(IDC_VIEWERPICTURECTRL)->GetClientRect(&clientWindowRect);
			GetDlgItem(IDC_VIEWERPICTURECTRL)->ClientToScreen(&clientWindowRect);
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


void CVCSampleMissingFramesDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

	// if the checkbox button is checked, do not display the image with missing packets.
	if(m_hView)
	{
		if (m_bDropFramesWithMissingPacket == false ||
			(m_bDropFramesWithMissingPacket && pAqImageInfo->iMissingPackets == 0))
		{
			status = J_Image_ShowImage(m_hView, pAqImageInfo);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(status);
				return;
			}
		}
	}

	//char cBuff[100];
	uint64_t iCurruptedFrames = 0; // missing frames + uncompleted frames(frame with missing packets)
	uint64_t iLostFrames = 0; // received but thrown for lacking of buffers in acquisition engine
 	uint32_t iSize = sizeof(uint64_t);

	// get the number of lost frames due to a queue under run  
	J_Image_GetStreamInfo(m_hThread, STREAM_INFO_CMD_NUMBER_OF_FRAMES_LOST_QUEUE_UNDERRUN, &iLostFrames, &iSize);
	iSize = sizeof(uint64_t);
	// get the number of frames which has been corrupted 
	J_Image_GetStreamInfo(m_hThread, STREAM_INFO_CMD_NUMBER_OF_FRAMES_CORRUPT_ON_DELIEVRY, &iCurruptedFrames, &iSize);

    // Protect the update of the UI values
    EnterCriticalSection(&m_UICriticalSection);

	m_iCountReceivedFrames++;
	m_iCountLostFrames = iLostFrames;
	m_iCountCurruptedFrames = iCurruptedFrames;

	m_iCountTotalMissingPackets += pAqImageInfo->iMissingPackets;

	if (pAqImageInfo->iMissingPackets > 0)
	{
		m_iCountFramesWithMissingPackets++;
	}

    LeaveCriticalSection(&m_UICriticalSection);
}

void CVCSampleMissingFramesDlg::OnBnClickedStopbutton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

	// Stop Acquision
	if (m_hCamera) {
        status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStop");
	}

	// Close stream
	if(m_hThread)
	{
		status = J_Image_CloseStream(m_hThread);
		m_hThread = NULL;
	}

	// Close view window
	if(m_hView)
	{
		status = J_Image_CloseViewWindow(m_hView);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(status);
			return;
		}

		m_hView = NULL;
	}

	m_startButton.EnableWindow();
	m_stopButton.EnableWindow(FALSE);
	m_widthSlider.EnableWindow();
	m_heightSlider.EnableWindow();
	m_dropFramesCheck.EnableWindow();

    m_missingFramesStatic.SetWindowText(_T("0"));
	m_fMissingPacketsStatic.SetWindowText(_T("0/0"));
	m_totalMissingPacketsStatic.SetWindowText(_T("0"));

	// Stop UI update timer
    KillTimer(1);

}

void CVCSampleMissingFramesDlg::OnTimer(UINT_PTR nIDEvent)
{
	// UI update timer
    if (nIDEvent == 1)
    {
        CString missingFramesString;
        CString framesWithMissingPacketsString;
		CString totalMissingPacketsString;
		uint64_t iTotalMissingFrames = 0;

        // Protect from update when reading
        EnterCriticalSection(&m_UICriticalSection);
		
		iTotalMissingFrames = m_iCountLostFrames + m_iCountCurruptedFrames - m_iCountFramesWithMissingPackets;
		missingFramesString.Format(_T("%I64u"), iTotalMissingFrames);
		framesWithMissingPacketsString.Format(_T("%I64u/%I64u"),m_iCountFramesWithMissingPackets, m_iCountReceivedFrames);
		totalMissingPacketsString.Format(_T("%I64u"), m_iCountTotalMissingPackets);

		LeaveCriticalSection(&m_UICriticalSection);

		// update missing frames and missing packets in GUI
        m_missingFramesStatic.SetWindowText(missingFramesString);
		m_fMissingPacketsStatic.SetWindowText(framesWithMissingPacketsString);
		m_totalMissingPacketsStatic.SetWindowText(totalMissingPacketsString);

    }
	
	CDialog::OnTimer(nIDEvent);
}

void CVCSampleMissingFramesDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CVCSampleMissingFramesDlg::OnBnClickedOk()
{
	OnOK();
}
