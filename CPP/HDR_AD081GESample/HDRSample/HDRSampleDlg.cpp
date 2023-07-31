// HdrSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HdrSample.h"
#include "HdrSampleDlg.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The following variables are used for timing the execution of the HDR algorithm
LARGE_INTEGER	liFreq;
LARGE_INTEGER	liStart;
LARGE_INTEGER	liStop;

//============================================================================
// Stream Thread Process Caller for image 1
//============================================================================
DWORD StreamThreadCaller0(CHdrSampleDlg* pThread)
{
    pThread->StreamThread(0);
    return 0;
}

//============================================================================
// Stream Thread Process Caller for image 2
//============================================================================
DWORD StreamThreadCaller1(CHdrSampleDlg* pThread)
{
    pThread->StreamThread(1);
    return 0;
}

//============================================================================
// HDR Processing Thread Caller
//============================================================================
DWORD HDRProcessingThreadCaller(CHdrSampleDlg* pThread)
{
    pThread->HDRProcessingThread();
    return 0;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CHdrSampleDlg dialog
CHdrSampleDlg::CHdrSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHdrSampleDlg::IDD, pParent)
    , m_GainValue(_T(""))
    , m_ProcessingTime(_T(""))
    , m_PixelFormatInt(0)
    , m_LogOutput(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;

    for (int index=0; index<2; index++)
    {
        m_hCamera[index] = NULL;
        m_hStreamThread[index] = NULL;
        m_hStreamEvent[index] = NULL;
        m_hDS[index] = NULL;
        m_hNewImageCondition[index] = NULL;
        m_NewImageBlackLevel[index] = 8;
        m_ImageBlackLevel[index] = 8;
    }

    m_hHDRProcessingThread = NULL;
    m_hHDRProcessingEvent = NULL;
    m_hHDRProcessingCondition = NULL;

    for (int i=0; i<VIEWCOUNT; i++)
        m_hView[i] = NULL;

    m_SaturationLevel = 253;
    m_DarkGain = 1.0;
    m_NewDarkGain = 1.0;
    m_DualSlope = 1.0; // Off
    m_AnalysisRequested = true;
    m_AnalysisOK = false;
    m_IsImage1Brighter = false;
    m_ProcessingTimeValue = 0;
    m_ImageMask = 0;

    m_OutputImage.pImageBuffer = NULL;

    // Initialize the high speed time measurement
    QueryPerformanceFrequency(&liFreq);

    // Initialize the Critical Section that protects the image data during acquisition and processing
    InitializeCriticalSection(&m_HDRProcessingCriticalSection);
}

void CHdrSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IMAGE_1, m_PictureControl1);
    DDX_Control(pDX, IDC_IMAGE_2, m_PictureControl2);
    DDX_Control(pDX, IDC_IMAGE_3, m_PictureControl3);
    DDX_Control(pDX, IDC_CAMERA_EDIT, m_CameraIdEdit);
    DDX_Text(pDX, IDC_PROCESSING_TIME_STATIC, m_ProcessingTime);
    DDX_CBIndex(pDX, IDC_PIXELFORMAT_COMBO, m_PixelFormatInt);
    DDX_Check(pDX, IDC_LOG_CHECK, m_LogOutput);
    DDX_Control(pDX, IDC_DUAL_SLOPE_SLIDER, m_DualSlopeSliderCtrl);
}

BEGIN_MESSAGE_MAP(CHdrSampleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_SEARCH_BUTTON, &CHdrSampleDlg::OnBnClickedSearchButton)
    ON_BN_CLICKED(IDC_START_ACQ_BUTTON, &CHdrSampleDlg::OnBnClickedStartAcqButton)
    ON_BN_CLICKED(IDC_STOP_ACQ_BUTTON, &CHdrSampleDlg::OnBnClickedStopAcqButton)
    ON_BN_CLICKED(IDC_OPEN_BUTTON, &CHdrSampleDlg::OnBnClickedOpenButton)
    ON_BN_CLICKED(IDC_CLOSE_BUTTON, &CHdrSampleDlg::OnBnClickedCloseButton)
    ON_BN_CLICKED(IDC_ANALYZE_BUTTON, &CHdrSampleDlg::OnBnClickedAnalyzeButton)
    ON_WM_TIMER()
    ON_CBN_SELCHANGE(IDC_PIXELFORMAT_COMBO, &CHdrSampleDlg::OnCbnSelchangePixelformatCombo)
    ON_BN_CLICKED(IDC_LOG_CHECK, &CHdrSampleDlg::OnBnClickedLogCheck)
    ON_BN_CLICKED(IDOK, &CHdrSampleDlg::OnBnClickedOk)
    ON_WM_HSCROLL()
    ON_EN_CHANGE(IDC_BLACK_LEVEL_EDIT1, &CHdrSampleDlg::OnEnChangeBlackLevelEdit1)
    ON_EN_CHANGE(IDC_BLACK_LEVEL_EDIT2, &CHdrSampleDlg::OnEnChangeBlackLevelEdit2)
    ON_NOTIFY(UDN_DELTAPOS, IDC_GAIN_SPIN, &CHdrSampleDlg::OnDeltaposGainSpin)
    ON_BN_CLICKED(IDC_SAVE_BUTTON, &CHdrSampleDlg::OnBnClickedSaveButton)
END_MESSAGE_MAP()


// CHdrSampleDlg message handlers

BOOL CHdrSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    // Initialize the range of the Dual-Slope slider control to be between [0..1000]
    m_DualSlopeSliderCtrl.SetRangeMin(0);
    m_DualSlopeSliderCtrl.SetRangeMax(1000);
    m_DualSlopeSliderCtrl.SetPos(1000);

    // Initialize the range of the Black-Level and HDR Processing Gain spin buttons
    ((CSpinButtonCtrl*)GetDlgItem(IDC_BLACL_LEVEL_SPIN1))->SetRange(0,100);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_BLACL_LEVEL_SPIN2))->SetRange(0,100);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_GAIN_SPIN))->SetRange(0,1);

    // Display the Dual-Slope value in Edit Control
    CString valueString;
    valueString.Format(_T("%0.3f"), m_DualSlope);
    GetDlgItem(IDC_DUALSLOPE_EDIT)->SetWindowText(valueString);

    // Try to find the camera
    OnBnClickedSearchButton();
    
    // Initialize the enable state of all controls
    EnableControls(FALSE, FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHdrSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHdrSampleDlg::OnPaint()
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
HCURSOR CHdrSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CHdrSampleDlg::DestroyWindow()
{
    J_STATUS_TYPE   retval;

    // Cleanup: Close camera connections and close the Factory object
    if (m_hCamera[0])
    {
        retval = J_Camera_Close(m_hCamera[0]);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close the camera[0]!"), retval);
        }
        m_hCamera[0] = NULL;
    }

    if (m_hCamera[1])
    {
        retval = J_Camera_Close(m_hCamera[1]);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close the camera[1]!"), retval);
        }
        m_hCamera[1] = NULL;
    }

    if (m_hFactory)
    {
        retval = J_Factory_Close(m_hFactory);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close the Factory!"), retval);
        }
        m_hFactory = NULL;
    }

    // Remember to free the memory allocated for the HDR processing output image
    if (m_OutputImage.pImageBuffer)
    {
        retval = J_Image_Free(&m_OutputImage);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not free the Output Image!"), retval);
        }
        m_OutputImage.pImageBuffer = NULL;
    }

    // Release and delete the Critical Section again
    DeleteCriticalSection(&m_HDRProcessingCriticalSection);

    return CDialog::DestroyWindow();
}

// Dislay a message box with error information
void CHdrSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
{
    CString errorMsg;
    errorMsg.Format(_T("%s: Error = %d: "), message, error);
    tGenICamErrorInfo GenICamError;

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
    case J_ST_GC_ERROR:				
        errorMsg += "GenICam error :";
        J_Factory_GetGenICamErrorInfo(&GenICamError);
        errorMsg += "Node='";
        errorMsg += GenICamError.sNodeName;
        errorMsg += "'\nError message='";
        errorMsg += GenICamError.sDescription;
        errorMsg += "' ";
        break;
    case J_ST_VALIDATION_ERROR:		errorMsg += "Settings File Validation Error ";		break;
    case J_ST_VALIDATION_WARNING:	errorMsg += "Settings File Validation Warning ";    break;
    }

    AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);

}


// Search for cameras o
void CHdrSampleDlg::OnBnClickedSearchButton()
{
    J_STATUS_TYPE retval = J_ST_SUCCESS;
    m_CameraIdEdit.SetWindowText(_T("No JAI HDR camera found!"));

    // Check to see if the Factory has already been opened
    if (m_hFactory == NULL)
    {
        // Try to open it
        retval = J_Factory_Open((int8_t*)"", &m_hFactory);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Unable to open the JAI SDK Factory!"), retval);
            return;
        }
    }

    // Perform the Device Discovery
    bool8_t hasChanged = false;
    retval = J_Factory_UpdateCameraList(m_hFactory, &hasChanged);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to update the camera list!"), retval);
        return;
    }

    // Check the result of the Device Discovery
    uint32_t numCameras = 0;
    retval = J_Factory_GetNumOfCameras(m_hFactory, &numCameras);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get the number of detected cameras!"), retval);
        return;
    }

    // We need at least two cameras in the list since the AD-081GE has two ports
    if (numCameras < 2)
    {
        AfxMessageBox(_T("No cameras detected!"));
        return;
    }

    // Try to get the two ID strings for the camera
    for (int index=0; index<2; index++)
    {
        uint32_t size = sizeof(m_CameraIDString[index]);
        retval = J_Factory_GetCameraIDByIndex(m_hFactory, index, m_CameraIDString[index], &size);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Unable to get the camera ID string!"), retval);
            return;
        }

        // Get the camera model from the Camera ID
        size = sizeof(m_CameraModelString[index]);
        retval = J_Factory_GetCameraInfo(m_hFactory, m_CameraIDString[index], CAM_INFO_MODELNAME, m_CameraModelString[index], &size);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Unable to get the camera Model Name string!"), retval);
            return;
        }

        // Now we need to check to see if the camera model is correct
        char buffer[20];
        sprintf_s(buffer, sizeof(buffer), "AD-081GE_#%d", index);
        if (strcmp((const char*) m_CameraModelString[index], buffer))
        {
			sprintf_s(buffer, sizeof(buffer), "AD-131GE_#%d", index);
			if (strcmp((const char*) m_CameraModelString[index], buffer))
			{
				sprintf_s(buffer, sizeof(buffer), "AD-132GE_#%d", index);
				if (strcmp((const char*) m_CameraModelString[index], buffer))
				{
					ShowErrorMsg(CString("Wrong camera model detected! This sample is only valid for AD-081, AD-131 or AD-132GE cameras!"), retval);
					return;
				}
			}
        }
    }

    // Show that the camera is found OK
    m_CameraIdEdit.SetWindowText(_T("JAI HDR camera found"));

    //// Initaliaze all the Slider Controls with values read from the camera
    //InitializeControls();   // Initialize Controls

    UpdateData(FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CHdrSampleDlg::OnBnClickedStartAcqButton()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    int64_t int64Val;
    int64_t pixelFormat;

    SIZE	ViewSize;
    RECT    PictureRect;

    // Get Max Width from the camera based on GenICam name
    retval = J_Camera_GetNodeByName(m_hCamera[0], (int8_t*)"Width", &hNode);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Width node!"), retval);
        return;
    }
    retval = J_Node_GetMaxInt64(hNode, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Max value for the Width node!"), retval);
        return;
    }
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

    // Get Max Height from the camera
    retval = J_Camera_GetNodeByName(m_hCamera[0], (int8_t*)"Height", &hNode);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Height node!"), retval);
        return;
    }
    retval = J_Node_GetMaxInt64(hNode, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Max value for the Height node!"), retval);
        return;
    }
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

    // Get PixelFormat from the camera
    retval = J_Camera_GetValueInt64(m_hCamera[0], (int8_t*)"PixelFormat", &pixelFormat);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get PixelFormat!"), retval);
        return;
    }

    // Calculate number of bits (not bytes) per pixel using macro
    int bpp = J_BitsPerPixel(pixelFormat);

    // Get the client window rectangle to be used for the display
    m_PictureControl1.GetClientRect(&PictureRect);

    // Open view window
    retval = J_Image_OpenViewWindowEx(J_IVW_CHILD_STRETCH, _T("Image View Window"), &PictureRect, &ViewSize, m_PictureControl1.m_hWnd, &m_hView[0]);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window[0]!"), retval);
        return;
    }

    m_PictureControl2.GetClientRect(&PictureRect);

    // Open view window
    retval = J_Image_OpenViewWindowEx(J_IVW_CHILD_STRETCH, _T("Image View Window"), &PictureRect, &ViewSize, m_PictureControl2.m_hWnd, &m_hView[1]);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window[1]!"), retval);
        return;
    }

    m_PictureControl3.GetClientRect(&PictureRect);

    // Open view window
    retval = J_Image_OpenViewWindowEx(J_IVW_CHILD_STRETCH, _T("Image View Window"), &PictureRect, &ViewSize, m_PictureControl3.m_hWnd, &m_hView[2]);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window[2]!"), retval);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Create the HDR Processing Thread
    CreateHDRProcessingThread();

    // Create image acquisition threads (this allocates buffers)
    CreateStreamThread(0, 0, (ViewSize.cx * ViewSize.cy * bpp)/8);
    CreateStreamThread(1, 0, (ViewSize.cx * ViewSize.cy * bpp)/8);

    TRACE("Opening stream succeeded\n");

    // Start Acquision on both camera parts
    retval = J_Camera_ExecuteCommand(m_hCamera[0], (int8_t*)"AcquisitionStart");
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not Start Acquisition[0]!"), retval);
    }
    retval = J_Camera_ExecuteCommand(m_hCamera[1], (int8_t*)"AcquisitionStart");
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not Start Acquisition[1]!"), retval);
    }

    // Enable controls
    EnableControls(TRUE, TRUE);

    SetTimer(0, 100, NULL);
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CHdrSampleDlg::OnBnClickedStopAcqButton()
{
    J_STATUS_TYPE retval;

    // Stop Acquision
    if (m_hCamera[0])
    {
        retval = J_Camera_ExecuteCommand(m_hCamera[0], (int8_t*)"AcquisitionStop");
        if (retval != J_ST_SUCCESS) {
            ShowErrorMsg(CString("Could not Stop Acquisition[0]!"), retval);
        }
    }

    // Stop Acquision
    if (m_hCamera[1])
    {
        retval = J_Camera_ExecuteCommand(m_hCamera[1], (int8_t*)"AcquisitionStop");
        if (retval != J_ST_SUCCESS) {
            ShowErrorMsg(CString("Could not Stop Acquisition[1]!"), retval);
        }
    }

    // Close stream (this frees all allocated buffers)
    TerminateStreamThread(0);
    TerminateStreamThread(1);
    TRACE("Closed stream\n");
    TerminateHDRProcessingThread();
    TRACE("Stopped HDR Processing Thread\n");

    // View window opened?
    for (int i=0; i<VIEWCOUNT; i++)
    {
        if(m_hView[i])
        {
            // Close view window
            retval = J_Image_CloseViewWindow(m_hView[i]);
            if (retval != J_ST_SUCCESS) {
                ShowErrorMsg(CString("Could not Close View Window!"), retval);
            }
            m_hView[i] = NULL;
            TRACE("Closed view window\n");
        }
    }
    EnableControls(TRUE, FALSE);

    // Stop the timer again
    KillTimer(0);
}

//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CHdrSampleDlg::InitializeControls()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    int64_t int64Val;

    if (m_hCamera[0])
    {
        retval = J_Camera_GetValueInt64(m_hCamera[0], (int8_t*)"PixelFormat", &int64Val);

        if (retval == J_ST_SUCCESS)
        {
            if (int64Val == (int64_t)J_GVSP_PIX_MONO8)
                m_PixelFormatInt = 0;
            else if (int64Val == (int64_t)J_GVSP_PIX_MONO10)
                m_PixelFormatInt = 1;
            else if (int64Val == (int64_t)J_GVSP_PIX_MONO10_PACKED)
                m_PixelFormatInt = 2;
            else
                m_PixelFormatInt = 2;
        }
        else
        {
            ShowErrorMsg(CString("Could not get PixelFormat!"), retval);
        }
    }
    UpdateData(0);

    CSliderCtrl* pSCtrl;

    //- Exposure Time 1 ------------------------------------------------
    // Get SliderCtrl for Exposure Time 1
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_EXPOSURE_TIME_SLIDER1);

    // We have two possible ways of setting up Exposure time: JAI or GenICam SFNC
    // The JAI Exposure time setup uses a node called "ShutterMode" and the SFNC 
    // does not need to set up anything in order to be able to control the exposure time.
    // Therefor we have to determine which way to use here.
    // First we see if a node called "ShutterMode" exists.
    hNode = NULL;
    retval = J_Camera_GetNodeByName(m_hCamera[0], (int8_t*)"ShutterMode", &hNode);

    // Does the "ShutterMode" node exist?
    if ((retval == J_ST_SUCCESS) && (hNode != NULL))
    {
        // Here we assume that this is JAI way so we do the following:
        // ShutterMode=ProgrammableExposure
        // Make sure that the ExposureMode selector is set to ProgrammableExposure
        retval = J_Camera_SetValueString(m_hCamera[0], (int8_t*)"ShutterMode", (int8_t*)"ProgrammableExposure");
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not set ShutterMode=ProgrammableExposure!"), retval);
        }
    }
    else
    {
        ShowErrorMsg(CString("Could not get ShutterMode node!"), retval);
    }

    // Get Exposure Time Raw Node
    retval = J_Camera_GetNodeByName(m_hCamera[0], (int8_t*)"ExposureTimeRaw", &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Min
        retval = J_Node_GetMinInt64(hNode, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get minimum value for ExposureTimeRaw!"), retval);
        }
        pSCtrl->SetRangeMin((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MIN_STATIC1, (int)int64Val);

        // Get/Set Max
        retval = J_Node_GetMaxInt64(hNode, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get maximum value for ExposureTimeRaw!"), retval);
        }
        pSCtrl->SetRangeMax((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MAX_STATIC1, (int)int64Val);

        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get value for ExposureTimeRaw!"), retval);
        }
        pSCtrl->SetPos((int)int64Val);

        SetDlgItemInt(IDC_EXPOSURE_TIME1, (int)int64Val);
        pSCtrl->EnableWindow(true);
        GetDlgItem(IDC_EXPOSURE_TIME1)->EnableWindow(true);
        GetDlgItem(IDC_MIN_STATIC1)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_MAX_STATIC1)->ShowWindow(SW_SHOW);
    }
    else
    {
        ShowErrorMsg(CString("Could not get ExposureTimeRaw node!"), retval);

        pSCtrl->EnableWindow(false);
        GetDlgItem(IDC_EXPOSURE_TIME1)->EnableWindow(false);
        GetDlgItem(IDC_MIN_STATIC1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_MAX_STATIC1)->ShowWindow(SW_HIDE);
    }

    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER1);

    // Get  Gain Raw Node
    retval = J_Camera_GetNodeByName(m_hCamera[0], (int8_t*)"GainRaw", &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Min
        retval = J_Node_GetMinInt64(hNode, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get minimum value for GainRaw!"), retval);
        }
        pSCtrl->SetRangeMin((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MIN_GAIN_STATIC1, (int)int64Val);

        // Get/Set Max
        retval = J_Node_GetMaxInt64(hNode, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get maximum value for GainRaw!"), retval);
        }
        pSCtrl->SetRangeMax((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MAX_GAIN_STATIC1, (int)int64Val);

        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get value for GainRaw!"), retval);
        }
        pSCtrl->SetPos((int)int64Val);

        SetDlgItemInt(IDC_GAIN_EDIT1, (int)int64Val);
        pSCtrl->EnableWindow(true);
        GetDlgItem(IDC_GAIN_EDIT1)->EnableWindow(true);
        GetDlgItem(IDC_MIN_GAIN_STATIC1)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_MAX_GAIN_STATIC1)->ShowWindow(SW_SHOW);
    }
    else
    {
        ShowErrorMsg(CString("Could not get ExposureTimeRaw node!"), retval);

        pSCtrl->EnableWindow(false);
        GetDlgItem(IDC_GAIN_EDIT1)->EnableWindow(false);
        GetDlgItem(IDC_MIN_GAIN_STATIC1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_MAX_GAIN_STATIC1)->ShowWindow(SW_HIDE);
    }

    //- Exposure Time 2 ------------------------------------------------

    // Get SliderCtrl for Exposure Time 2
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_EXPOSURE_TIME_SLIDER2);

    // We have two possible ways of setting up Exposure time: JAI or GenICam SFNC
    // The JAI Exposure time setup uses a node called "ShutterMode" and the SFNC 
    // does not need to set up anything in order to be able to control the exposure time.
    // Therefor we have to determine which way to use here.
    // First we see if a node called "ShutterMode" exists.
    hNode = NULL;
    retval = J_Camera_GetNodeByName(m_hCamera[1], (int8_t*)"ShutterMode", &hNode);

    // Does the "ShutterMode" node exist?
    if ((retval == J_ST_SUCCESS) && (hNode != NULL))
    {
        // Here we assume that this is JAI way so we do the following:
        // ShutterMode=ProgrammableExposure
        // Make sure that the ExposureMode selector is set to ProgrammableExposure
        retval = J_Camera_SetValueString(m_hCamera[1], (int8_t*)"ShutterMode", (int8_t*)"ProgrammableExposure");
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not set ShutterMode=ProgrammableExposure!"), retval);
        }
    }
    else
    {
        ShowErrorMsg(CString("Could not get ShutterMode node!"), retval);
    }

    // Get  Exposure Time Raw Node
    retval = J_Camera_GetNodeByName(m_hCamera[1], (int8_t*)"ExposureTimeRaw", &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Min
        retval = J_Node_GetMinInt64(hNode, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get minimum value for ExposureTimeRaw!"), retval);
        }
        pSCtrl->SetRangeMin((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MIN_STATIC2, (int)int64Val);

        // Get/Set Max
        retval = J_Node_GetMaxInt64(hNode, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get maximum value for ExposureTimeRaw!"), retval);
        }
        pSCtrl->SetRangeMax((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MAX_STATIC2, (int)int64Val);

        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get value for ExposureTimeRaw!"), retval);
        }
        pSCtrl->SetPos((int)int64Val);

        SetDlgItemInt(IDC_EXPOSURE_TIME2, (int)int64Val);
        pSCtrl->EnableWindow(true);
        GetDlgItem(IDC_EXPOSURE_TIME2)->EnableWindow(true);
        GetDlgItem(IDC_MIN_STATIC2)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_MAX_STATIC2)->ShowWindow(SW_SHOW);
    }
    else
    {
        ShowErrorMsg(CString("Could not get ExposureTimeRaw node!"), retval);

        pSCtrl->EnableWindow(false);
        GetDlgItem(IDC_EXPOSURE_TIME2)->EnableWindow(false);
        GetDlgItem(IDC_MIN_STATIC2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_MAX_STATIC2)->ShowWindow(SW_HIDE);
    }

    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER2);

        // Get  Gain Raw Node
    retval = J_Camera_GetNodeByName(m_hCamera[1], (int8_t*)"GainRaw", &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Min
        retval = J_Node_GetMinInt64(hNode, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get minimum value for GainRaw!"), retval);
        }
        pSCtrl->SetRangeMin((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MIN_GAIN_STATIC2, (int)int64Val);

        // Get/Set Max
        retval = J_Node_GetMaxInt64(hNode, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get maximum value for GainRaw!"), retval);
        }
        pSCtrl->SetRangeMax((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MAX_GAIN_STATIC2, (int)int64Val);

        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not get value for GainRaw!"), retval);
        }
        pSCtrl->SetPos((int)int64Val);

        SetDlgItemInt(IDC_GAIN_EDIT2, (int)int64Val);
        pSCtrl->EnableWindow(true);
        GetDlgItem(IDC_GAIN_EDIT2)->EnableWindow(true);
        GetDlgItem(IDC_MIN_GAIN_STATIC2)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_MAX_GAIN_STATIC2)->ShowWindow(SW_SHOW);
    }
    else
    {
        ShowErrorMsg(CString("Could not get ExposureTimeRaw node!"), retval);

        pSCtrl->EnableWindow(false);
        GetDlgItem(IDC_GAIN_EDIT2)->EnableWindow(false);
        GetDlgItem(IDC_MIN_GAIN_STATIC2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_MAX_GAIN_STATIC2)->ShowWindow(SW_HIDE);
    }
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CHdrSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
    GetDlgItem(IDC_OPEN_BUTTON)->EnableWindow(bIsCameraReady ? FALSE : TRUE);
    GetDlgItem(IDC_CLOSE_BUTTON)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
    GetDlgItem(IDC_PIXELFORMAT_COMBO)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_START_ACQ_BUTTON)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_STOP_ACQ_BUTTON)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_ANALYZE_BUTTON)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
}

//--------------------------------------------------------------------------------------------------
// Create Stream Thread
//--------------------------------------------------------------------------------------------------
BOOL CHdrSampleDlg::CreateStreamThread(int iCameraIndex, uint32_t iChannel, uint32_t iBufferSize)
{
    // Is it already created?
    if(m_hStreamThread[iCameraIndex])
        return FALSE;

    // Open the stream channel(GVSP)
    if(m_hDS[iCameraIndex] == NULL)
    {
        if((J_ST_SUCCESS != J_Camera_CreateDataStream(m_hCamera[iCameraIndex], iChannel, &m_hDS[iCameraIndex]))
            || (m_hDS[iCameraIndex] == NULL))
        {
            OutputDebugString(_T("*** Error : J_Camera_CreateDataStream\n"));
            return FALSE;
        }
    }

    // Prepare the frame buffers (this announces the buffers to the acquisition engine)
    if(0 == PrepareBuffer(iCameraIndex, iBufferSize))
    {
        OutputDebugString(_T("*** Error : PrepareBuffer\n"));
        J_DataStream_Close(m_hDS[iCameraIndex]);
        return FALSE;
    }

    // Stream thread event created?
    if(m_hStreamEvent[iCameraIndex] == NULL)
        m_hStreamEvent[iCameraIndex] = CreateEvent(NULL, true, false, NULL);
    else
        ResetEvent(m_hStreamEvent[iCameraIndex]);

    // Set the thread execution flag
    m_bEnableThread[iCameraIndex] = true;

    // Create a Stream Thread.
    if (iCameraIndex == 0)
    {
        if(NULL == (m_hStreamThread[iCameraIndex] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StreamThreadCaller0, this, NULL, NULL)))
        {
            OutputDebugString(_T("*** Error : CreateThread\n"));
            J_DataStream_Close(m_hDS[iCameraIndex]);
            return FALSE;
        }
    }
    else if (iCameraIndex == 1)
    {
        if(NULL == (m_hStreamThread[iCameraIndex] = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)StreamThreadCaller1, this, NULL, NULL)))
        {
            OutputDebugString(_T("*** Error : CreateThread\n"));
            J_DataStream_Close(m_hDS[iCameraIndex]);
            return FALSE;
        }
    }


    return TRUE;
}

//--------------------------------------------------------------------------------------------------
// Create Stream Thread
//--------------------------------------------------------------------------------------------------
BOOL CHdrSampleDlg::CreateHDRProcessingThread(void)
{
    // Is it already created?
    if(m_hHDRProcessingThread)
        return FALSE;

    // Processing thread event created?
    if(m_hHDRProcessingEvent == NULL)
        m_hHDRProcessingEvent = CreateEvent(NULL, true, false, NULL);
    else
        ResetEvent(m_hHDRProcessingEvent);

    // Set the thread execution flag
    m_bEnableHDRProcessingThread = true;

    if(NULL == (m_hHDRProcessingThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)HDRProcessingThreadCaller, this, NULL, NULL)))
    {
        OutputDebugString(_T("*** Error : CreateThread\n"));
        return FALSE;
    }

    return TRUE;
}

//==============================================================////
// Terminate Stream Thread
//==============================================================////
BOOL CHdrSampleDlg::TerminateStreamThread(int iCameraIndex)
{
    // Is the data stream opened?
    if(m_hDS[iCameraIndex] == NULL)
        return FALSE;

    // Reset the thread execution flag.
    m_bEnableThread[iCameraIndex] = false;

    // Signal the image thread to stop faster
    J_Event_ExitCondition(m_hNewImageCondition[iCameraIndex]);

    // Wait for the thread to end
    WaitForThreadToTerminate(iCameraIndex);

    // Stop the image acquisition engine
    J_DataStream_StopAcquisition(m_hDS[iCameraIndex], ACQ_STOP_FLAG_KILL);

    // UnPrepare Buffers (this removed the buffers from the acquisition engine and frees buffers)
    UnPrepareBuffer(iCameraIndex);

    // Close Stream
    if(m_hDS[iCameraIndex])
    {
        J_DataStream_Close(m_hDS[iCameraIndex]);
        m_hDS[iCameraIndex] = NULL;
    }

    return TRUE;
}

//==============================================================////
// Terminate HDR Processing Thread
//==============================================================////
BOOL CHdrSampleDlg::TerminateHDRProcessingThread(void)
{
    // Reset the thread execution flag.
    m_bEnableHDRProcessingThread = false;

    // Signal the image thread to stop faster
    J_Event_ExitCondition(m_hHDRProcessingCondition);

    // Wait for the thread to end
    WaitForHDRProcessingThreadToTerminate();

    return TRUE;
}

//==============================================================////
// Stream Processing Function
//==============================================================////
void CHdrSampleDlg::StreamThread(int iCameraIndex)
{
    J_STATUS_TYPE	iResult;
    uint32_t		    tSize;
    BUF_HANDLE	    iBufferID;
	uint64_t	    iQueued = 0;
    bool            bShowImage = false;
    int             iImageMask = 0;

    // Create structure to be used for image display
    J_tIMAGE_INFO	tAqImageInfo = {0, 0, 0, 0, NULL};

    J_COND_WAIT_RESULT	WaitResult;

    // Create the condition used for signalling the new image event
    iResult = J_Event_CreateCondition(&m_hNewImageCondition[iCameraIndex]);

    EVT_HANDLE	hEvent; // Buffer event handle
	EVENT_NEW_BUFFER_DATA eventData;	// Strut for EventGetData

    // Register the event with the acquisition engine
    J_DataStream_RegisterEvent(m_hDS[iCameraIndex], EVENT_NEW_BUFFER, m_hNewImageCondition[iCameraIndex], &hEvent); 

    // Start image acquisition
    iResult = J_DataStream_StartAcquisition(m_hDS[iCameraIndex], ACQ_START_NEXT_IMAGE, 0 );

    // Loop of Stream Processing
    OutputDebugString(_T(">>> Start Stream Process Loop.\n"));

    while(m_bEnableThread[iCameraIndex])
    {
        // Wait for Buffer event (or kill event)
        iResult = J_Event_WaitForCondition(m_hNewImageCondition[iCameraIndex], 1000, &WaitResult);

        // Did we get a new buffer event?
        if(J_COND_WAIT_SIGNAL == WaitResult)
        {
            // Get the Buffer Handle from the event
            tSize = (uint32_t)sizeof(void *);
            iResult = J_Event_GetData(hEvent, &eventData,  &tSize);

			iBufferID = eventData.BufferHandle;

            if ((iResult == J_ST_SUCCESS) && (iBufferID != 0))
            {
                // Fill in structure for image display
                // Get the pointer to the frame buffer.
                tSize = (uint32_t)sizeof (void *);
                iResult = J_DataStream_GetBufferInfo(m_hDS[iCameraIndex], iBufferID, BUFFER_INFO_BASE	, &(tAqImageInfo.pImageBuffer), &tSize);
                // Get the effective data size.
                tSize = (uint32_t)sizeof (uint32_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS[iCameraIndex], iBufferID, BUFFER_INFO_SIZE	, &(tAqImageInfo.iImageSize), &tSize);
                // Get Pixel Format Type.
                tSize = (uint32_t)sizeof (uint32_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS[iCameraIndex], iBufferID, BUFFER_INFO_PIXELTYPE, &(tAqImageInfo.iPixelType), &tSize);
                // Get Frame Width.
                tSize = (uint32_t)sizeof (uint32_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS[iCameraIndex], iBufferID, BUFFER_INFO_WIDTH	, &(tAqImageInfo.iSizeX), &tSize);
                // Get Frame Height.
                tSize = (uint32_t)sizeof (uint32_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS[iCameraIndex], iBufferID, BUFFER_INFO_HEIGHT	, &(tAqImageInfo.iSizeY), &tSize);
                // Get Timestamp.
                tSize = (uint32_t)sizeof (uint64_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS[iCameraIndex], iBufferID, BUFFER_INFO_TIMESTAMP, &(tAqImageInfo.iTimeStamp), &tSize);

                // Get # of frames awaiting delivery
                tSize = sizeof(uint64_t);
                iResult = J_DataStream_GetStreamInfo(m_hDS[iCameraIndex], STREAM_INFO_CMD_NUMBER_OF_FRAMES_AWAIT_DELIVERY, &iQueued, &tSize);
                if((iResult == J_ST_SUCCESS) && (iQueued < 2))
                    bShowImage = true;
                else
                    bShowImage = false;

                if(m_bEnableThread[iCameraIndex])
                {
                    if (bShowImage)
                    {
                        // Display image
                        if(m_hView[iCameraIndex])
                        {
                            // Shows image
                            J_Image_ShowImage(m_hView[iCameraIndex], &tAqImageInfo);
                        }
                    }

                    // See if we are currently processing
                    EnterCriticalSection(&m_HDRProcessingCriticalSection);
                    iImageMask = m_ImageMask;
                    LeaveCriticalSection(&m_HDRProcessingCriticalSection);

                    // We have to check if we already have an image in the processing buffer!
                    if (!(iImageMask & (0x0001<<iCameraIndex)))
                    {
                        EnterCriticalSection(&m_HDRProcessingCriticalSection);
                        // Copy the Image Info contents
                        // It is important to notice that we dont copy the image data because we assume
                        // the the buffer will not be overwritten since the HDR processing is fast enought to
                        // be run in "real time"
                        memcpy((void*)&m_ImageInfo[iCameraIndex], (const void*)&tAqImageInfo, sizeof(tAqImageInfo));

                        m_ImageMask |= (0x0001<<iCameraIndex);

                        // Now we need to check if we got a pair of images
                        if (m_ImageMask == 0x03)
                        {
                            // Signal the HDR processing thread to start if it is not already active
                            if (m_hHDRProcessingCondition != NULL)
                                J_Event_SignalCondition(m_hHDRProcessingCondition);
                        }
                        LeaveCriticalSection(&m_HDRProcessingCriticalSection);
                    }
                }

                // Queue This Buffer Again for reuse in acquisition engine
                iResult = J_DataStream_QueueBuffer(m_hDS[iCameraIndex], iBufferID);
            }

            // Get # of frames awaiting delivery
            tSize = sizeof(uint64_t);
            iResult = J_DataStream_GetStreamInfo(m_hDS[iCameraIndex], STREAM_INFO_CMD_NUMBER_OF_FRAMES_AWAIT_DELIVERY, &iQueued, &tSize);
        }
        else
        {
            switch(WaitResult)
            {
                // Kill event
                case	J_COND_WAIT_EXIT:
                    iResult = 1;
                    break;
                // Timeout
                case	J_COND_WAIT_TIMEOUT:
                    iResult = 2;
                    break;
                // Error event
                case	J_COND_WAIT_ERROR:
                    iResult = 3;
                    break;
                // Unknown?
                default:
                    iResult = 4;
                    break;
            }
        }
    }
    OutputDebugString(_T(">>> Terminated Stream Process Loop.\n"));

    // Unregister new buffer event with acquisition engine
    J_DataStream_UnRegisterEvent(m_hDS[iCameraIndex], EVENT_NEW_BUFFER); 

    // Free the event object
    if (hEvent != NULL)
    {
        J_Event_Close(hEvent);
        hEvent = NULL;
    }

    // Terminate the thread.
    SetEvent(m_hStreamEvent[iCameraIndex]);

    // Free the Condition
    if (m_hNewImageCondition[iCameraIndex] != NULL)
    {
        J_Event_CloseCondition(m_hNewImageCondition[iCameraIndex]);
        m_hNewImageCondition[iCameraIndex] = NULL;
    }
}

//==============================================================////
// HDR Processing Function
//==============================================================////
void CHdrSampleDlg::HDRProcessingThread(void)
{
    J_STATUS_TYPE	iResult;
    int             iImageMask = 0;
    J_COND_WAIT_RESULT	WaitResult;

    // Create the condition used for signalling the new image event
    iResult = J_Event_CreateCondition(&m_hHDRProcessingCondition);

    while(m_bEnableHDRProcessingThread)
    {
        // Wait for Buffer event (or kill event)
        iResult = J_Event_WaitForCondition(m_hHDRProcessingCondition, 1000, &WaitResult);

        // Did we get two images to process?
        if(J_COND_WAIT_SIGNAL == WaitResult)
        {
            EnterCriticalSection(&m_HDRProcessingCriticalSection);
            iImageMask = m_ImageMask;
            LeaveCriticalSection(&m_HDRProcessingCriticalSection);

            // Now we need to check if we got a pair of images
            if (iImageMask == 0x03)
            {
                // Has an analysis been requested?
                if (m_AnalysisRequested)
                {
                    // Try to get more info about the relationship between the two images
                    iResult = J_Image_AnalyzeHDR(&m_ImageInfo[0], &m_ImageInfo[1], &m_IsImage1Brighter, &m_NewImageBlackLevel[0], &m_NewImageBlackLevel[1], &m_NewDarkGain);
                    if (iResult == J_ST_SUCCESS)
                    {
                        m_AnalysisOK = true;
                    }

                    // We keep on trying to analyze if it failed
                    if (m_AnalysisOK)
                        m_AnalysisRequested = false;
                }

                // Do we have an analysis result ready?
                if (m_AnalysisOK)
                {
                    // Check if we have an output image allocated already or not
                    if (m_OutputImage.pImageBuffer == NULL)
                        iResult = J_Image_MallocHDR(&m_ImageInfo[0], &m_ImageInfo[1], &m_OutputImage);

                    // For performance measurements only!
                    QueryPerformanceCounter(&liStart);

                    // Do the HDR merging of the two images
                    if (m_IsImage1Brighter)
                        J_Image_FuseHDR(&m_ImageInfo[0], &m_ImageInfo[1], &m_OutputImage, m_ImageBlackLevel[0], m_ImageBlackLevel[1], m_DarkGain, m_DualSlope, m_LogOutput);
                    else
                        J_Image_FuseHDR(&m_ImageInfo[1], &m_ImageInfo[0], &m_OutputImage, m_ImageBlackLevel[1], m_ImageBlackLevel[0], m_DarkGain, m_DualSlope, m_LogOutput);

                    QueryPerformanceCounter(&liStop);
                    int iTimeSpan = (int)((liStop.QuadPart - liStart.QuadPart) * 1000000 / liFreq.QuadPart);
                    m_ProcessingTimeValue = iTimeSpan;

                    // Finally show the 16-bit image to the display
                    J_Image_ShowImage(m_hView[2], &m_OutputImage);
                }

                // We are done processing the pair!
                EnterCriticalSection(&m_HDRProcessingCriticalSection);
                m_ImageMask = 0;
                LeaveCriticalSection(&m_HDRProcessingCriticalSection);
            }
        }
        else
        {
            switch(WaitResult)
            {
                // Kill event
                case	J_COND_WAIT_EXIT:
                    iResult = 1;
                    break;
                // Timeout
                case	J_COND_WAIT_TIMEOUT:
                    iResult = 2;
                    break;
                // Error event
                case	J_COND_WAIT_ERROR:
                    iResult = 3;
                    break;
                // Unknown?
                default:
                    iResult = 4;
                    break;
            }
        }
    }
    OutputDebugString(_T(">>> Terminated HDR Processing Thread Loop.\n"));

    // Terminate the thread.
    SetEvent(m_hHDRProcessingEvent);

    // Free the Condition
    if (m_hHDRProcessingCondition != NULL)
    {
        J_Event_CloseCondition(m_hHDRProcessingCondition);
        m_hHDRProcessingCondition = NULL;
    }
}

//==============================================================////
// Wait for thread to terminate
//==============================================================////
void CHdrSampleDlg::WaitForThreadToTerminate(int iCameraIndex)
{
    WaitForSingleObject(m_hStreamEvent[iCameraIndex], INFINITE);

    // Close the thread handle and stream event handle
    CloseThreadHandle(iCameraIndex);
}

//==============================================================////
// Wait for HDR Processing thread to terminate
//==============================================================////
void CHdrSampleDlg::WaitForHDRProcessingThreadToTerminate()
{
    WaitForSingleObject(m_hHDRProcessingEvent, INFINITE);

    if(m_hHDRProcessingThread)
    {
        CloseHandle(m_hHDRProcessingThread);
        m_hHDRProcessingThread = NULL;
    }
}

//==============================================================////
// Close handles and stream
//==============================================================////
void CHdrSampleDlg::CloseThreadHandle(int iCameraIndex)
{
    if(m_hStreamThread[iCameraIndex])
    {
        CloseHandle(m_hStreamThread[iCameraIndex]);
        m_hStreamThread[iCameraIndex] = NULL;
    }

    if(m_hStreamEvent[iCameraIndex])
    {
        CloseHandle(m_hStreamEvent[iCameraIndex]);
        m_hStreamEvent[iCameraIndex] = NULL;
    }
}

//==============================================================////
// Prepare frame buffers
//==============================================================////
uint32_t CHdrSampleDlg::PrepareBuffer(int iCameraIndex, int iBufferSize)
{
    J_STATUS_TYPE	iResult = J_ST_SUCCESS;
    int			i;

    m_iValidBuffers[iCameraIndex] = 0;

    for(i = 0 ; i < NUM_OF_BUFFER ; i++)
    {
        // Make the buffer for one frame. 
        m_pAquBuffer[iCameraIndex][i] = (uint8_t*)_aligned_malloc(iBufferSize, 16);

        // Announce the buffer pointer to the Acquisition engine.
        if(J_ST_SUCCESS != J_DataStream_AnnounceBuffer(m_hDS[iCameraIndex], m_pAquBuffer[iCameraIndex][i] ,iBufferSize , NULL, &(m_pAquBufferID[iCameraIndex][i])))
        {
            delete m_pAquBuffer[iCameraIndex][i];
            break;
        }

        // Queueing it.
        if(J_ST_SUCCESS != J_DataStream_QueueBuffer(m_hDS[iCameraIndex], m_pAquBufferID[iCameraIndex][i]))
        {
            delete m_pAquBuffer[iCameraIndex][i];
            break;
        }

        m_iValidBuffers[iCameraIndex]++;
    }

    return m_iValidBuffers[iCameraIndex];
}

//==============================================================////
// Unprepare buffers
//==============================================================////
BOOL CHdrSampleDlg::UnPrepareBuffer(int iCameraIndex)
{
    void		*pPrivate;
    void		*pBuffer;
    uint32_t	i;

    // Flush Queues
    J_DataStream_FlushQueue(m_hDS[iCameraIndex], ACQ_QUEUE_INPUT_TO_OUTPUT);
    J_DataStream_FlushQueue(m_hDS[iCameraIndex], ACQ_QUEUE_OUTPUT_DISCARD);

    for(i = 0 ; i < m_iValidBuffers[iCameraIndex] ; i++)
    {
        // Remove the frame buffer from the Acquisition engine.
        J_DataStream_RevokeBuffer(m_hDS[iCameraIndex], m_pAquBufferID[iCameraIndex][i], &pBuffer , &pPrivate);

        _aligned_free(m_pAquBuffer[iCameraIndex][i]);
        m_pAquBuffer[iCameraIndex][i] = NULL;
        m_pAquBufferID[iCameraIndex][i] = 0;
    }

    m_iValidBuffers[iCameraIndex] = 0;

    return TRUE;
}

void CHdrSampleDlg::PrepareCamera(int iCameraIndex)
{
	if(m_hCamera[iCameraIndex])
	{
		J_STATUS_TYPE retval;
		NODE_HANDLE hNode;
		uint32_t iNumOfEnumEntries = 0;

        // Set up the camera for continuous exposure mode

        // We have two possible ways of setting up triggers: JAI or GenICam SFNC
        // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
        // called "TriggerSelector". Therefor we have to determine which way to use here.
        // First we see if a node called "TriggerSelector" exists.
        hNode = NULL;

        retval = J_Camera_GetNodeByName(m_hCamera[iCameraIndex], (int8_t*)"TriggerSelector", &hNode);

        // Does the "TriggerSelector" node exist?
        if ((retval == J_ST_SUCCESS) && (hNode != NULL))
        {
            // Here we assume that this is GenICam SFNC trigger so we do the following:
            // TriggerSelector=FrameStart
            // TriggerMode=Off
            retval = J_Camera_SetValueString(m_hCamera[iCameraIndex], (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
            if (retval != J_ST_SUCCESS)
            {
                ShowErrorMsg(CString("Could not set TriggerSelector=FrameStart!"), retval);
                return;
            }
            retval = J_Camera_SetValueString(m_hCamera[iCameraIndex], (int8_t*)"TriggerMode", (int8_t*)"Off");
            if (retval != J_ST_SUCCESS)
            {
                ShowErrorMsg(CString("Could not set TriggerMode=Off!"), retval);
                return;
            }
        }
        else
        {
            // Here we assume that this is JAI trigger so we do the following:
            // ExposureMode=Continuous

            // Set ExposureMode=Continuous
            retval = J_Camera_SetValueString(m_hCamera[iCameraIndex], (int8_t*)"ExposureMode", (int8_t*)"Continuous");
		    if(retval != J_ST_SUCCESS) 
		    {
                ShowErrorMsg(CString("Could not set ExposureMode=Continuous!"), retval);
			    return;
		    }
        }

        // Here we switch off the internal HDR mode and force the two sensors to be synchronized
        // SyncMode=Sync
        // This is only valid for sensor iCameraIndex=0 since the other channel is the "slave"
        if (iCameraIndex == 0)
        {
            hNode = NULL;
            retval = J_Camera_GetNodeByName(m_hCamera[iCameraIndex], (int8_t*)"SyncMode", &hNode);

            // Does the "ExposureMode" node exist?
            if ((retval == J_ST_SUCCESS) && (hNode != NULL))
            {
                retval = J_Camera_SetValueString(m_hCamera[iCameraIndex], (int8_t*)"SyncMode", (int8_t*)"Sync");
                if (retval != J_ST_SUCCESS)
                {
                    ShowErrorMsg(CString("Could not set SyncMode=Sync!"), retval);
                    return;
                }
            }
        }

        // Set up the Packet-Delay!!
        // This is because of a firmware error in older Beta cameras
        retval = J_Camera_GetNodeByName(m_hCamera[iCameraIndex], (int8_t*)"GevSCPD", &hNode);
		if(retval != J_ST_SUCCESS) 
		{
            ShowErrorMsg(CString("Could not get GevSCPD node!"), retval);
			return;
		}

        int64_t value = 0;
        retval = J_Node_GetValueInt64(hNode, true, &value);
		if(retval != J_ST_SUCCESS) 
		{
            ShowErrorMsg(CString("Could not adjust GevSCPD value!"), retval);
			return;
		}
        retval = J_Node_SetValueInt64(hNode, true, value+1);
		if(retval != J_ST_SUCCESS) 
		{
            ShowErrorMsg(CString("Could not adjust GevSCPD value!"), retval);
			return;
		}
        retval = J_Node_SetValueInt64(hNode, true, value);
		if(retval != J_ST_SUCCESS) 
		{
            ShowErrorMsg(CString("Could not adjust GevSCPD value!"), retval);
			return;
		}
	} // end of if(m_hCamera[iCameraIndex])
}

void CHdrSampleDlg::OnBnClickedOpenButton()
{
	J_STATUS_TYPE retval = J_ST_SUCCESS;

    // Open the camera
    if (strlen((char*)m_CameraIDString[0]) && strlen((char*)m_CameraIDString[1]))
    {
        retval = J_Camera_Open(m_hFactory, m_CameraIDString[0], &m_hCamera[0]);

	    if(retval != J_ST_SUCCESS) 
	    {
            ShowErrorMsg(CString("Could not open Camera[0]!"), retval);
		    return;
	    }

        retval = J_Camera_Open(m_hFactory, m_CameraIDString[1], &m_hCamera[1]);

	    if(retval != J_ST_SUCCESS) 
	    {
            ShowErrorMsg(CString("Could not open Camera[1]!"), retval);
		    return;
	    }
        EnableControls(TRUE, FALSE);
        PrepareCamera(0);
        PrepareCamera(1);
        InitializeControls();
    }
    else
    {
        EnableControls(FALSE, FALSE);
    }
}

void CHdrSampleDlg::OnBnClickedCloseButton()
{
	J_STATUS_TYPE retval = J_ST_SUCCESS;

    // Stop Acquisition if it is started
    OnBnClickedStopAcqButton();

    // ... and close the camera connection
    if (m_hCamera[0])
    {
        retval = J_Camera_Close(m_hCamera[0]);

	    if(retval != J_ST_SUCCESS) 
	    {
            ShowErrorMsg(CString("Could not close Camera[0]!"), retval);
		    return;
	    }
        m_hCamera[0] = NULL;
    }

    if (m_hCamera[1])
    {
        retval = J_Camera_Close(m_hCamera[1]);

	    if(retval != J_ST_SUCCESS) 
	    {
            ShowErrorMsg(CString("Could not close Camera[1]!"), retval);
		    return;
	    }
        m_hCamera[1] = NULL;
    }
    EnableControls(FALSE, FALSE);
}

void CHdrSampleDlg::OnBnClickedAnalyzeButton()
{
    m_AnalysisOK = false;
    m_ImageMask = 0;
    m_AnalysisRequested = true;
}

void CHdrSampleDlg::OnTimer(UINT_PTR nIDEvent)
{

    if (nIDEvent == 0)
    {
        int iTimeSpan = m_ProcessingTimeValue;
	    int	iUnitms = iTimeSpan / 1000;
	    int	iUnitus = iTimeSpan % 1000;

	    CString valueString;
        valueString.Format(_T("HDR Execution Time: %d.%03dms"), iUnitms, iUnitus);
	    m_ProcessingTime = valueString;

        if (m_DarkGain != m_NewDarkGain)
        {
            m_DarkGain = m_NewDarkGain;
            valueString.Format(_T("%0.3f"), m_DarkGain);
            SetDlgItemText(IDC_GAIN_EDIT, valueString);
        }

        float DarkBlackLevel = 0.0f;
        if (m_IsImage1Brighter)
            DarkBlackLevel = (float)m_ImageBlackLevel[1];
        else
            DarkBlackLevel = (float)m_ImageBlackLevel[0];

        float res = log(m_DarkGain*(1023.0f-DarkBlackLevel))/log(2.0f);
        if (m_PixelFormatInt == 0)
            res = log(m_DarkGain*(255.0f-DarkBlackLevel))/log(2.0f);

        valueString.Format(_T("%0.1f"), res);
        SetDlgItemText(IDC_RESOLUTION_EDIT, valueString);

        if (m_ImageBlackLevel[0] != m_NewImageBlackLevel[0])
        {
            m_ImageBlackLevel[0] = m_NewImageBlackLevel[0];
            SetDlgItemInt(IDC_BLACK_LEVEL_EDIT1, m_ImageBlackLevel[0]);
        }
        if (m_ImageBlackLevel[1] != m_NewImageBlackLevel[1])
        {
            m_ImageBlackLevel[1] = m_NewImageBlackLevel[1];
            SetDlgItemInt(IDC_BLACK_LEVEL_EDIT2, m_ImageBlackLevel[1]);
        }

        UpdateData(0);
    }

    CDialog::OnTimer(nIDEvent);
}

void CHdrSampleDlg::OnCbnSelchangePixelformatCombo()
{
    // Change the Pixel Format 
    UpdateData();

    NODE_HANDLE hNode = NULL;
    int64_t value = 0;

    if (m_PixelFormatInt == 0)
    {
        J_Camera_SetValueInt64(m_hCamera[0], (int8_t*)"PixelFormat", J_GVSP_PIX_MONO8);
        J_Camera_SetValueInt64(m_hCamera[1], (int8_t*)"PixelFormat", J_GVSP_PIX_MONO8);
    }
    else if (m_PixelFormatInt == 1)
    {
        J_Camera_SetValueInt64(m_hCamera[0], (int8_t*)"PixelFormat", J_GVSP_PIX_MONO10);
        J_Camera_SetValueInt64(m_hCamera[1], (int8_t*)"PixelFormat", J_GVSP_PIX_MONO10);
    }
    else if (m_PixelFormatInt == 2)
    {
        J_Camera_SetValueInt64(m_hCamera[0], (int8_t*)"PixelFormat", J_GVSP_PIX_MONO10_PACKED);
        J_Camera_SetValueInt64(m_hCamera[1], (int8_t*)"PixelFormat", J_GVSP_PIX_MONO10_PACKED);
    }

    m_AnalysisOK = false;
    m_ImageMask = 0;
    m_AnalysisRequested = true;
}

void CHdrSampleDlg::OnBnClickedLogCheck()
{
    UpdateData();
}

void CHdrSampleDlg::OnBnClickedOk()
{
    // Close the camera connections
    // .. this will automatically close the Acquisition as well
    OnBnClickedCloseButton();

    OnOK();
}

void CHdrSampleDlg::ReAnalyze()
{
    m_AnalysisOK = false;
    m_ImageMask = 0;
    m_AnalysisRequested = true;
}

void CHdrSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // Get SliderCtrl for Width
    CString valueString;
    CSliderCtrl* pSCtrl;
    int iPos;
    J_STATUS_TYPE   retval;
    int64_t int64Val;

    //- Width ------------------------------------------------

    // Get SliderCtrl for Exposure Time 1
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_EXPOSURE_TIME_SLIDER1);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Get Exposure Time Raw Node
        retval = J_Camera_SetValueInt64(m_hCamera[0], (int8_t*)"ExposureTimeRaw", int64Val);

        SetDlgItemInt(IDC_EXPOSURE_TIME1, iPos);
        ReAnalyze();
    }

    // Get SliderCtrl for Exposure Time 1
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER1);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Get  Exposure Time Raw Node
        retval = J_Camera_SetValueInt64(m_hCamera[0], (int8_t*)"GainRaw", int64Val);

        SetDlgItemInt(IDC_GAIN_EDIT1, iPos);
        ReAnalyze();
    }

    // Get SliderCtrl for Exposure Time 2
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_EXPOSURE_TIME_SLIDER2);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Get Exposure Time Raw Node
        retval = J_Camera_SetValueInt64(m_hCamera[1], (int8_t*)"ExposureTimeRaw", int64Val);

        SetDlgItemInt(IDC_EXPOSURE_TIME2, iPos);
        ReAnalyze();
    }

    // Get SliderCtrl for Exposure Time 1
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER2);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Get  Exposure Time Raw Node
        retval = J_Camera_SetValueInt64(m_hCamera[1], (int8_t*)"GainRaw", int64Val);

        SetDlgItemInt(IDC_GAIN_EDIT2, iPos);
        ReAnalyze();
    }

    if (&m_DualSlopeSliderCtrl == (CSliderCtrl*)pScrollBar)
    {
        m_DualSlope = (float)m_DualSlopeSliderCtrl.GetPos()/1000.0f;
        valueString.Format(_T("%0.3f"), m_DualSlope);

        GetDlgItem(IDC_DUALSLOPE_EDIT)->SetWindowText(valueString);
    }
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CHdrSampleDlg::OnEnChangeBlackLevelEdit1()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    m_NewImageBlackLevel[0] = GetDlgItemInt(IDC_BLACK_LEVEL_EDIT1);
}

void CHdrSampleDlg::OnEnChangeBlackLevelEdit2()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    m_NewImageBlackLevel[1] = GetDlgItemInt(IDC_BLACK_LEVEL_EDIT2);
}

void CHdrSampleDlg::OnDeltaposGainSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
    m_NewDarkGain += (float) pNMUpDown->iDelta;
    *pResult = 0;
}

void CHdrSampleDlg::OnBnClickedSaveButton()
{
    J_STATUS_TYPE   retval;

    CString szFilter = _T("Tiff Image Files (*.tif)|*.tif|All Files (*.*)|*.*||");
    // Get the filename
    CFileDialog *mypFileDialog = new CFileDialog(FALSE, _T("tif"), _T("HDRImage.tif"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter);

    mypFileDialog->DoModal();

    if (m_OutputImage.pImageBuffer != NULL)
    {
        J_tIMAGE_INFO tCnvImageInfo;    // Image info structure

        // Allocate the buffer to hold converted the image
        retval = J_Image_Malloc(&m_OutputImage, &tCnvImageInfo);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not Malloc Image!"), retval);
		    return;
        }

        // Convert the raw image to image format
        retval = J_Image_FromRawToImage(&m_OutputImage, &tCnvImageInfo);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not convert Output Image from Raw format!"), retval);
		    return;
        }

        CString Filename = mypFileDialog->GetPathName();

        // Save the image to disk in TIFF format
        retval = J_Image_SaveFile(&tCnvImageInfo, Filename);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not save Image to disk!"), retval);
		    return;
        }

        // Free up the image buffer
        retval = J_Image_Free(&tCnvImageInfo);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not Free Image!"), retval);
		    return;
        }
    }
}
