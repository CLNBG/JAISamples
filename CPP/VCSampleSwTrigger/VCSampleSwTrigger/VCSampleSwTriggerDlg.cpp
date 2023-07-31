// VCSampleSwTriggerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VCSampleSwTrigger.h"
#include "VCSampleSwTriggerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVCSampleSwTriggerDlg dialog

CVCSampleSwTriggerDlg::CVCSampleSwTriggerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVCSampleSwTriggerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hFactory	= NULL;
	m_hCamera	= NULL; 
	m_hView		= NULL;
	m_hThread	= NULL;
}

void CVCSampleSwTriggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMERALIST, m_cameraList);
	DDX_Control(pDX, IDC_CAMERAIDSTATIC, m_cameraIdStatic);
	DDX_Control(pDX, IDC_WIDTHSLIDER, m_widthSlider);
	DDX_Control(pDX, IDC_HEIGHTSLIDER, m_heightSlider);
	DDX_Control(pDX, IDC_GAINSLIDER, m_gainSlider);
	DDX_Control(pDX, IDC_WIDTHSTATIC, m_widthStatic);
	DDX_Control(pDX, IDC_HEIGHTSTATIC, m_heightStatic);
	DDX_Control(pDX, IDC_GAINSTATIC, m_gainStatic);
	DDX_Control(pDX, IDC_STARTBUTTON, m_startButton);
	DDX_Control(pDX, IDC_STOPBUTTON, m_stopButton);
	DDX_Control(pDX, IDC_WBALANCEBUTTON, m_wBalanceButton);
	DDX_Control(pDX, IDC_SWTRIGGERBUTTON, m_swTriggerButton);
	DDX_Control(pDX, IDC_STATUSBARSTATIC, m_statusBarStatic);
	DDX_Control(pDX, IDC_CONTINUOUSRADIO, m_continuousRadio);
	DDX_Control(pDX, IDC_SWTRIGRADIO, m_swtrigRadio);
}

BEGIN_MESSAGE_MAP(CVCSampleSwTriggerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_HSCROLL()
	ON_LBN_DBLCLK(IDC_CAMERALIST, &CVCSampleSwTriggerDlg::OnLbnDblclkCameralist)
	ON_BN_CLICKED(IDC_STARTBUTTON, &CVCSampleSwTriggerDlg::OnBnClickedStartbutton)
	ON_BN_CLICKED(IDC_STOPBUTTON, &CVCSampleSwTriggerDlg::OnBnClickedStopbutton)
	ON_BN_CLICKED(IDC_SWTRIGGERBUTTON, &CVCSampleSwTriggerDlg::OnBnClickedSwtriggerbutton)
	ON_BN_CLICKED(IDC_WBALANCEBUTTON, &CVCSampleSwTriggerDlg::OnBnClickedWbalancebutton)
	ON_BN_CLICKED(IDC_CONTINUOUSRADIO, &CVCSampleSwTriggerDlg::OnBnClickedContinuousradio)
	ON_BN_CLICKED(IDC_SWTRIGRADIO, &CVCSampleSwTriggerDlg::OnBnClickedSwtrigradio)
	ON_BN_CLICKED(IDCANCEL, &CVCSampleSwTriggerDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CVCSampleSwTriggerDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CVCSampleSwTriggerDlg message handlers

BOOL CVCSampleSwTriggerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if(!OpenFactory()) return FALSE;

	m_continuousRadio.EnableWindow(FALSE);
	m_swtrigRadio.EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVCSampleSwTriggerDlg::OnPaint()
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
HCURSOR CVCSampleSwTriggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CVCSampleSwTriggerDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	OnBnClickedStopbutton();

	CloseFactoryAndCamera();
}

void CVCSampleSwTriggerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
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

BOOL CVCSampleSwTriggerDlg::OpenFactory()
{
	//Open Factory and get list of cameras

	J_STATUS_TYPE	status = J_ST_SUCCESS;
	bool8_t			bHasChanged = false;
	uint32_t		iNumOfCameras =	0;

    CWaitCursor waitCursor;

    status = J_Factory_Open((int8_t*)"", &m_hFactory);
	if(status != J_ST_SUCCESS) 
	{
		ShowErrorMsg(CString("Could not open factory!"), status);
		return FALSE;
	}

	//Look for cameras
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
	else if(iNumOfCameras > 0)
	{

		for (uint32_t index = 0; index < iNumOfCameras; index++)
		{
			uint32_t iCameraIdSize = J_CAMERA_ID_SIZE;

			//status = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_CameraId, &iCameraIdSize);
			status = J_Factory_GetCameraIDByIndex(m_hFactory, index, m_CameraId, &iCameraIdSize);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not get camera ID!"), status);
				return FALSE;
			}

			int8_t   sCameraInfo[J_CAMERA_INFO_SIZE];
			int8_t	 sSerialNumber[J_CAMERA_INFO_SIZE];
			uint32_t iCameraInfoSize = J_CAMERA_INFO_SIZE;
			CString ListItemStr;
			
			//get model name
			status = J_Factory_GetCameraInfo(m_hFactory, m_CameraId, CAM_INFO_MODELNAME, sCameraInfo, &iCameraInfoSize);
			if(status != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not get camera ModelName info!"), status);
				return FALSE;
			}
			
			//Get serial number
			iCameraInfoSize = sizeof(sSerialNumber); //Don't forget to reset the size for each call to J_Factory_GetCameraInfo!
			status = J_Factory_GetCameraInfo(m_hFactory, m_CameraId, CAM_INFO_SERIALNUMBER, sSerialNumber, &iCameraInfoSize);
			if (status != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("Could not get serial number of the camera!"), status);
				return FALSE;
			}

            ListItemStr = (char*)sCameraInfo;

			//check if the interface is filter driver or socket driver

			if (strstr((char*)m_CameraId, "INT=>FD"))
				ListItemStr += " (Filter Driver)";
			else if (strstr((char*)m_CameraId, "INT=>SD"))
				ListItemStr += " (Socket Driver)";

			m_cameraList.InsertString(index, ListItemStr); 

			CString statusBar;
			statusBar.Format(_T("%d camera(s) found. Double click to connect to a camera"), iNumOfCameras);
			m_statusBarStatic.SetWindowText(statusBar);

		}//end of camera index

	}
	return TRUE;
}

void CVCSampleSwTriggerDlg::CloseFactoryAndCamera()
{

	J_STATUS_TYPE	status = J_ST_SUCCESS;

	if(m_hCamera)
	{
		status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerMode", (int8_t*)"Off");

		status = J_Camera_Close(m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not close camera!"), status);
		}
		m_hCamera = NULL;
	}

	if(m_hFactory)
	{
		status = J_Factory_Close(m_hFactory);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not close factory!"), status);
		}
		m_hFactory = NULL;
	}
}

void CVCSampleSwTriggerDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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

void CVCSampleSwTriggerDlg::InitializeControls()
{

	if(m_hCamera)
	{
		J_STATUS_TYPE	status = J_ST_SUCCESS;
		NODE_HANDLE		hNode;
		char buff[80];
		uint32_t iSize =  sizeof(buff);
		int64_t minval, maxval, nodeValue;
		CString editStr;


		m_cameraIdStatic.SetWindowText(CString((char*)m_CameraId));
		
		//initialize width slider
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"Width", &hNode); // get Width node handle
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not get Width node!"), status);
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
			ShowErrorMsg(CString("Could not get Height node!"), status);
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
		if(status == J_ST_SUCCESS) 
		{
			status = J_Node_GetMinInt64(hNode, &minval); //get min vlaue of the Gainraw node from GenICam 
			status = J_Node_GetMaxInt64(hNode, &maxval); //get max value of the Gainraw node from GenICam
			m_gainSlider.SetRange((int)minval,(int)maxval); //set min, max value in gainSlider
			m_gainSlider.SetTicFreq((int)(maxval - minval) / 10); 

			status = J_Node_GetValueInt64(hNode, 0, &nodeValue);	//get the gain value from GenICam
			m_gainSlider.SetPos((int)nodeValue); //set the gain value of gainSlider

			editStr.Format(_T("%d"),(int)nodeValue);
			m_gainStatic.SetWindowText(editStr);
		}
		else
		{
			m_gainSlider.EnableWindow(FALSE);
			m_gainStatic.EnableWindow(FALSE);
		}

		// check if it is a bayer camera
		iSize = sizeof(buff);
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"PixelFormat", &hNode);
        status = J_Node_GetValueString(hNode, 0, (int8_t*)buff, &iSize);

		if (strstr(buff, "Bayer"))
		{
			m_bayerCamera = true;
		}
        else
        {
			m_bayerCamera = false;
        }

		m_startButton.EnableWindow();
		m_widthSlider.EnableWindow();
		m_heightSlider.EnableWindow();
		m_gainSlider.EnableWindow();
		m_continuousRadio.EnableWindow();

		//check if the camera has software trigger feature
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SoftwareTrigger0", &hNode);
		if (status != J_ST_SUCCESS)
		{
			m_swtrigRadio.EnableWindow(FALSE);		
			return;
		}

		m_swtrigRadio.EnableWindow();		
	}
}
void CVCSampleSwTriggerDlg::OnLbnDblclkCameralist()
{
    CWaitCursor waitCursor;

    // close a previous camera and open a selected camera
	J_STATUS_TYPE	status = J_ST_SUCCESS;

	uint32_t index = m_cameraList.GetCurSel();

	//if the newly selected camera is the same as previous one, do nothing
	if(m_hCamera && m_OldCameraIndex != index)
	{
		status = J_Camera_Close(m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not close camera!"), status);
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
			ShowErrorMsg(CString("Could not get camera ID!"), status);
			return;
		}

		//open the selected camera
		status = J_Camera_Open(m_hFactory, m_CameraId, &m_hCamera);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not open camera!"), status);
			return;
		}

		InitializeControls();

		//select continuous mode by default when camera is opened
		CheckRadioButton(IDC_CONTINUOUSRADIO, IDC_SWTRIGGERRADIO, IDC_CONTINUOUSRADIO);
		PrepareContinuousSetting();

		m_OldCameraIndex = index;

		CString statusBar;
		m_cameraList.GetText(index, statusBar);
		statusBar += " has been opened. Select Expore Mode.";
		m_statusBarStatic.SetWindowText(statusBar);
	}
}

void CVCSampleSwTriggerDlg::OnBnClickedStartbutton()
{
	if(m_hCamera)
	{

		int64_t		width = 0;
		int64_t		height = 0;
		POINT		StartPoint;
		SIZE		MaxViewerSize;
		J_STATUS_TYPE status;

		//Get Max Width value
        J_Camera_GetValueInt64(m_hCamera, (int8_t*)"Width", &width);
		MaxViewerSize.cx = (LONG)width;

		//Get Max Height Value
        J_Camera_GetValueInt64(m_hCamera, (int8_t*)"Height", &height);
		MaxViewerSize.cy= (LONG)height;
	
		StartPoint.x = 100;
		StartPoint.y = 100;

		status = J_Image_OpenViewWindow(_T("GigE Image Viewer"), &StartPoint, &MaxViewerSize, &m_hView);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not open View Window!"), status);
			return;
		}

		status = J_Image_OpenStreamLight(m_hCamera, 0, &m_hThread);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not open StreamLight!"), status);
			return;
		}

		TCHAR cbuff[100];

		if (m_bayerCamera)
		{

			uint32_t rGain, gGain, bGain;

			GetPrivateProfileString (_T("WhiteBalance"), _T("RedGain"), _T("4096"), cbuff, sizeof(cbuff), _T(".\\rgbsettings.ini"));
			rGain = _ttoi(cbuff);

			GetPrivateProfileString (_T("WhiteBalance"), _T("GreenGain"), _T("4096"), cbuff, sizeof(cbuff), _T(".\\rgbsettings.ini"));
			gGain = _ttoi(cbuff);

			GetPrivateProfileString (_T("WhiteBalance"), _T("BlueGain"), _T("4096"), cbuff, sizeof(cbuff), _T(".\\rgbsettings.ini"));
			bGain = _ttoi(cbuff);

			status = J_Image_SetGain(m_hThread, rGain, gGain, bGain);

    		m_wBalanceButton.EnableWindow();
		}
        else
        {
    		m_wBalanceButton.EnableWindow(FALSE);
        }

        status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStart");
	    if(status != J_ST_SUCCESS) 
	    {
		    ShowErrorMsg(CString("Could not Start Acquisition!"), status);
		    return;
	    }

        m_startButton.EnableWindow(FALSE);
        m_stopButton.EnableWindow();

		m_widthSlider.EnableWindow(FALSE);
		m_widthStatic.EnableWindow(FALSE);
		m_heightSlider.EnableWindow(FALSE);
		m_heightStatic.EnableWindow(FALSE);
		m_continuousRadio.EnableWindow(FALSE);
		m_swtrigRadio.EnableWindow(FALSE);

	    //enable or disable swTrigger

		NODE_HANDLE hNode;
	    uint32_t iSize = sizeof(cbuff); 
        int64_t int64Val = 0;
		char abuff[100];

        // We have two possible ways of setting up triggers: JAI or GenICam SFNC
        // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
        // called "TriggerSelector". Therefor we have to determine which way to use here.
        // First we see if a node called "TriggerSelector" exists.
        hNode = NULL;
        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"TriggerSelector", &hNode);

        // Does the "TriggerSelector" node exist?
        if ((status == J_ST_SUCCESS) && (hNode != NULL))
        {
            // Now we need to find TriggerMode and TriggerSource for TriggerSelector=FrameStart 
            // in order to determin if the camera has got SW trigger enabled or not!

            // Set TriggerSelector=FrameStart
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");

            bool isSwTriggerEnabled = false;

            // Get TriggerMode[TriggerSelector] and check it is "On"
		    iSize = sizeof(abuff); 
            status = J_Camera_GetValueString(m_hCamera, (int8_t*)"TriggerMode", (int8_t*)abuff, &iSize);
		    if(strstr(abuff, "On"))
		    {
                // Get TriggerSource[TriggerSelector] and check it is "Software"
		        iSize = sizeof(abuff); 
                status = J_Camera_GetValueString(m_hCamera, (int8_t*)"TriggerSource", (int8_t*)abuff, &iSize);
		        if(strstr(abuff, "Software"))
		        {
                    isSwTriggerEnabled = true;
                }
            }

		    if(!isSwTriggerEnabled)
		    {
			    m_swTriggerButton.EnableWindow(FALSE);
			    m_statusBarStatic.SetWindowText(_T("Continuous Exposure Mode has been selected"));
		    }
		    else
		    {
			    m_swTriggerButton.EnableWindow();
			    m_statusBarStatic.SetWindowText(_T("Press Software Trigger button to get an image."));
		    }
        }
        else
        {
		    //enable or disable swTrigger
		    iSize = sizeof(abuff); 
            status = J_Camera_GetValueString(m_hCamera, (int8_t*)"ExposureMode", (int8_t*)abuff, &iSize);
		    if(strstr(abuff, "Continuous"))
		    {
			    m_swTriggerButton.EnableWindow(FALSE);
			    m_statusBarStatic.SetWindowText(_T("Continuous Exposure Mode has been selected"));
		    }
		    else if(strstr(abuff, "EdgePreSelect"))
		    {
			    m_swTriggerButton.EnableWindow();
			    m_statusBarStatic.SetWindowText(_T("Press Software Trigger button to get an image."));
		    }
        }
	}
}

void CVCSampleSwTriggerDlg::OnBnClickedStopbutton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

	// Write AcquisitionStop to Camera
	if( ::IsWindowEnabled( m_stopButton.GetSafeHwnd() ) )
	{
		status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"AcquisitionStop");
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not Stop Acquisition!"), status);
			return;
		}
	}
	
	// Close stream
	if(m_hThread)
	{
		status = J_Image_CloseStream(m_hThread);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not close Stream!"), status);
			return;
		}

		m_hThread = NULL;
	}

	// Close view window
	if(m_hView)
	{
		status = J_Image_CloseViewWindow(m_hView);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not close View Window!"), status);
			return;
		}

		m_hView = NULL;
	}

    m_startButton.EnableWindow();
    m_stopButton.EnableWindow(FALSE);
	m_wBalanceButton.EnableWindow(FALSE);
    m_swTriggerButton.EnableWindow(FALSE);

	m_widthSlider.EnableWindow();
	m_heightSlider.EnableWindow();
	m_widthStatic.EnableWindow();
	m_heightStatic.EnableWindow();

	m_continuousRadio.EnableWindow();
	m_swtrigRadio.EnableWindow();

	m_statusBarStatic.SetWindowText(_T("Closed Image Viewer. Press Exit button to quit."));

}

void CVCSampleSwTriggerDlg::OnBnClickedSwtriggerbutton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

    // We have two possible ways of software triggering: JAI or GenICam SFNC
    // The GenICam SFNC software trigger uses a node called "TriggerSoftware" and the JAI
    // uses nodes called "SoftwareTrigger0" to "SoftwareTrigger3".
    // Therefor we have to determine which way to use here.
    // First we see if a node called "TriggerSoftware" exists.
    NODE_HANDLE hNode = NULL;
    status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"TriggerSoftware", &hNode);

    // Does the "TriggerSoftware" node exist?
    if ((status == J_ST_SUCCESS) && (hNode != NULL))
    {
        // Here we assume that this is GenICam SFNC software trigger so we do the following:
        // TriggerSelector=FrameStart
        // Execute TriggerSoftware Command
        status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
	    if(status != J_ST_SUCCESS) 
	    {
		    ShowErrorMsg(CString("Could not set TriggerSelector!"), status);
		    return;
	    }
        status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"TriggerSoftware");
	    if(status != J_ST_SUCCESS) 
	    {
		    ShowErrorMsg(CString("Could not set TriggerSoftware!"), status);
		    return;
	    }
    }
    else
    {
        // Here we assume that this is JAI software trigger so we do the following:
        // SoftwareTrigger0=1
        // SoftwareTrigger0=0

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
}

void CVCSampleSwTriggerDlg::OnBnClickedWbalancebutton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;
	uint32_t rGain, gGain, bGain;

	if(m_hThread)
	{
		status = J_Image_ExecuteWhiteBalance(m_hThread);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not execute WhiteBalance!"), status);
			return;
		}

		Sleep(1000);

		status = J_Image_GetGain(m_hThread, &rGain, &gGain, &bGain);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not get Gain values!"), status);
			return;
		}

		CString gainString;

		gainString.Format(_T("%d"), rGain);
		WritePrivateProfileString(_T("WhiteBalance"), _T("RedGain"), gainString, _T(".\\rgbsettings.ini"));
		gainString.Format(_T("%d"), gGain);
		WritePrivateProfileString(_T("WhiteBalance"), _T("GreenGain"), gainString, _T(".\\rgbsettings.ini"));
		gainString.Format(_T("%d"), bGain);
		WritePrivateProfileString(_T("WhiteBalance"), _T("BlueGain"), gainString, _T(".\\rgbsettings.ini"));

		AfxMessageBox(_T("White Balance has been processed!"), MB_OK | MB_ICONINFORMATION);
	}
}

void CVCSampleSwTriggerDlg::OnBnClickedContinuousradio()
{
	PrepareContinuousSetting(); // set camera setting to continuous mode

	m_statusBarStatic.SetWindowText(_T("Continuous Mode has been selected. Press Start button to get images.")); 
}

void CVCSampleSwTriggerDlg::OnBnClickedSwtrigradio()
{
	PrepareSwTrigSetting(); // set camera setting to software trigger

	m_statusBarStatic.SetWindowText(_T("Software Trigger has been selected. Press Start button to open the viewer.")); 
}

void CVCSampleSwTriggerDlg::PrepareContinuousSetting()
{
	J_STATUS_TYPE status;

    if(m_hCamera)
	{
        // We have two possible ways of setting up triggers: JAI or GenICam SFNC
        // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
        // called "TriggerSelector". Therefor we have to determine which way to use here.
        // First we see if a node called "TriggerSelector" exists.
        NODE_HANDLE hNode = NULL;
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
	} // end of if(m_hCamera)

    m_swTriggerButton.EnableWindow(FALSE);
}

void CVCSampleSwTriggerDlg::PrepareSwTrigSetting()
{
	J_STATUS_TYPE status;
	if(m_hCamera)
	{
		NODE_HANDLE hNode;
		uint32_t iNumOfEnumEntries = 0;
        int64_t int64Val = 0;
		
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
            // TriggerSource=Software
            // SequenceMode=Off

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
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSource", (int8_t*)"Software");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set TriggerSource!"), status);
			    return;
		    }

            // Set SequenceMode="Off" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceMode", (int8_t*)"Off");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set SequenceMode!"), status);
			    return;
		    }
        }
        else
        {
            // Here we assume that this is JAI trigger so we do the following:
            // ExposureMode=EdgePreSelect
            // LineSelector=CameraTrigger0
            // LineSource=SoftwareTrigger0

		    // Set ExposureMode="SequentialEPSTrigger"
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"ExposureMode", (int8_t*)"EdgePreSelect");
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

		    // Set LineSource="SoftwareTrigger0" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSource", (int8_t*)"SoftwareTrigger0");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set LineSource!"), status);
			    return;
		    }
        }

		m_swTriggerButton.EnableWindow();
	} // end of if(m_hCamera)
}

void CVCSampleSwTriggerDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CVCSampleSwTriggerDlg::OnBnClickedOk()
{
	OnOK();
}
