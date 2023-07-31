// HdrSequenceSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HdrSequenceSample.h"
#include "HdrSequenceSampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The following variables are used for timing the execution of the HDR algorithm
LARGE_INTEGER	liFreq;
LARGE_INTEGER	liStart;
LARGE_INTEGER	liStop;

//============================================================================
// Thread Process Caller
//============================================================================
DWORD ProcessCaller(CHdrSequenceSampleDlg* pThread)
{
    pThread->StreamProcess();

    return 0;
}

//============================================================================
// HDR Processing Thread Caller
//============================================================================
DWORD HDRProcessingThreadCaller(CHdrSequenceSampleDlg* pThread)
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



// CHdrSequenceSampleDlg dialog
CHdrSequenceSampleDlg::CHdrSequenceSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHdrSequenceSampleDlg::IDD, pParent)
    , m_GainValue(_T(""))
    , m_ProcessingTime(_T(""))
    , m_PixelFormatInt(0)
    , m_AutoTrig(TRUE)
    , m_LogOutput(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCamera = NULL;

    for (int i=0; i<VIEWCOUNT; i++)
        m_hView[i] = NULL;

    m_hStreamThread = NULL;
    m_hStreamEvent = NULL;
    m_hDS = NULL;
    m_iStreamChannel = 0;
    m_hNewImageCondition = NULL;
    m_PixelCount = 0;
    m_NewImage1BlackLevel = 8;
    m_NewImage2BlackLevel = 8;
    m_Image1BlackLevel = 8;
    m_Image2BlackLevel = 8;
    m_SaturationLevel = 253;
    m_DarkGain = 1.0;
    m_NewDarkGain = 1.0;
    m_DualSlope = 1.0; // Off
    m_AnalysisRequested = true;
    m_AnalysisOK = false;
    m_IsImage1Brighter = true;
    m_ProcessingTimeValue = 0;
    m_RGain = 1.0;
    m_BGain = 1.0;
    m_ImageMask = 0;

    m_hHDRProcessingThread = NULL;
    m_hHDRProcessingEvent = NULL;
    m_hHDRProcessingCondition = NULL;

    m_OutputImage.pImageBuffer = NULL;

    QueryPerformanceFrequency(&liFreq);

    // Initialize the Critical Section that protects the image data during acquisition and processing
    InitializeCriticalSection(&m_HDRProcessingCriticalSection);
}

void CHdrSequenceSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_IMAGE_1, m_PictureControl1);
    DDX_Control(pDX, IDC_IMAGE_2, m_PictureControl2);
    DDX_Control(pDX, IDC_IMAGE_3, m_PictureControl3);
    DDX_Control(pDX, IDC_CAMERA_EDIT, m_CameraIdEdit);
    DDX_Text(pDX, IDC_PROCESSING_TIME_STATIC, m_ProcessingTime);
    DDX_CBIndex(pDX, IDC_PIXELFORMAT_COMBO, m_PixelFormatInt);
    DDX_Check(pDX, IDC_AUTOTRIG_CHECK, m_AutoTrig);
    DDX_Check(pDX, IDC_LOG_CHECK, m_LogOutput);
    DDX_Control(pDX, IDC_DUAL_SLOPE_SLIDER, m_DualSlopeSliderCtrl);
    DDX_Control(pDX, IDC_PIXELFORMAT_COMBO, m_PixelFormatCtrl);
    DDX_Control(pDX, IDC_RGAIN_EDIT, m_RGainCtrl);
    DDX_Control(pDX, IDC_RGAIN_SPIN, m_RGainSpinCtrl);
    DDX_Control(pDX, IDC_BGAIN_EDIT, m_BGainCtrl);
    DDX_Control(pDX, IDC_BGAIN_SPIN, m_BGainSpinCtrl);
}

BEGIN_MESSAGE_MAP(CHdrSequenceSampleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_SEARCH_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedSearchButton)
    ON_BN_CLICKED(IDC_START_ACQ_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedStartAcqButton)
    ON_BN_CLICKED(IDC_STOP_ACQ_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedStopAcqButton)
    ON_BN_CLICKED(IDC_TRIG_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedTrigButton)
    ON_BN_CLICKED(IDC_OPEN_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedOpenButton)
    ON_BN_CLICKED(IDC_CLOSE_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedCloseButton)
    ON_BN_CLICKED(IDC_ANALYZE_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedAnalyzeButton)
    ON_WM_TIMER()
    ON_CBN_SELCHANGE(IDC_PIXELFORMAT_COMBO, &CHdrSequenceSampleDlg::OnCbnSelchangePixelformatCombo)
    ON_BN_CLICKED(IDC_AUTOTRIG_CHECK, &CHdrSequenceSampleDlg::OnBnClickedAutotrigCheck)
    ON_BN_CLICKED(IDC_LOG_CHECK, &CHdrSequenceSampleDlg::OnBnClickedLogCheck)
    ON_BN_CLICKED(IDOK, &CHdrSequenceSampleDlg::OnBnClickedOk)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_CHANGE_SEQUENCE_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedChangeSequenceButton)
    ON_EN_CHANGE(IDC_BLACK_LEVEL_EDIT1, &CHdrSequenceSampleDlg::OnEnChangeBlackLevelEdit1)
    ON_EN_CHANGE(IDC_BLACK_LEVEL_EDIT2, &CHdrSequenceSampleDlg::OnEnChangeBlackLevelEdit2)
    ON_NOTIFY(UDN_DELTAPOS, IDC_GAIN_SPIN, &CHdrSequenceSampleDlg::OnDeltaposGainSpin)
    ON_BN_CLICKED(IDC_SAVE_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedSaveButton)
    ON_BN_CLICKED(IDC_WB_BUTTON, &CHdrSequenceSampleDlg::OnBnClickedWbButton)
    ON_EN_CHANGE(IDC_RGAIN_EDIT, &CHdrSequenceSampleDlg::OnEnChangeRgainEdit)
    ON_NOTIFY(UDN_DELTAPOS, IDC_RGAIN_SPIN, &CHdrSequenceSampleDlg::OnDeltaposRgainSpin)
    ON_EN_CHANGE(IDC_BGAIN_EDIT, &CHdrSequenceSampleDlg::OnEnChangeBgainEdit)
    ON_NOTIFY(UDN_DELTAPOS, IDC_BGAIN_SPIN, &CHdrSequenceSampleDlg::OnDeltaposBgainSpin)
END_MESSAGE_MAP()


// CHdrSequenceSampleDlg message handlers

BOOL CHdrSequenceSampleDlg::OnInitDialog()
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

    m_DualSlopeSliderCtrl.SetRangeMin(0);
    m_DualSlopeSliderCtrl.SetRangeMax(1000);
    m_DualSlopeSliderCtrl.SetPos(1000);

    ((CSpinButtonCtrl*)GetDlgItem(IDC_BLACL_LEVEL_SPIN1))->SetRange(0,100);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_BLACL_LEVEL_SPIN2))->SetRange(0,100);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_GAIN_SPIN))->SetRange(0,1);

    m_RGainSpinCtrl.SetRange(1,400);

    int value = AfxGetApp()->GetProfileInt(_T("HDRSequenceSample"), _T("RGain"), 100);
    m_RGainSpinCtrl.SetPos(value);
    m_RGain = double(value) / 100.0;
    CString RGainString;
    RGainString.Format(_T("%0.2f"), m_RGain);
    m_RGainCtrl.SetWindowText(RGainString);

    m_BGainSpinCtrl.SetRange(1,400);

    value = AfxGetApp()->GetProfileInt(_T("HDRSequenceSample"), _T("BGain"), 100);
    m_BGainSpinCtrl.SetPos(value);
    m_BGain = double(value) / 100.0;

    CString BGainString;
    BGainString.Format(_T("%0.2f"), m_BGain);
    m_BGainCtrl.SetWindowText(BGainString);

    CString valueString;
    valueString.Format(_T("%0.3f"), m_DualSlope);
    GetDlgItem(IDC_DUALSLOPE_EDIT)->SetWindowText(valueString);

    OnBnClickedSearchButton();
    EnableControls(FALSE, FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHdrSequenceSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CHdrSequenceSampleDlg::OnPaint()
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
HCURSOR CHdrSequenceSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHdrSequenceSampleDlg::ResizeChildWindow(CStatic& PictureControl)
{
	CRect mainWindowRect;
	CRect clientWindowRect;
	this->GetClientRect(&mainWindowRect);
	this->ClientToScreen(&mainWindowRect);

	PictureControl.GetClientRect(&clientWindowRect);
	PictureControl.ClientToScreen(&clientWindowRect);
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

BOOL CHdrSequenceSampleDlg::DestroyWindow()
{
    J_STATUS_TYPE status;

    // Cleanup
    if (m_hCamera)
    {
        status = J_Camera_Close(m_hCamera);
        if (status != J_ST_SUCCESS)
            ShowErrorMsg(CString("Could not close camera!"), status);

        m_hCamera = NULL;
    }

    if (m_hFactory)
    {
        status = J_Factory_Close(m_hFactory);
        if (status != J_ST_SUCCESS)
            ShowErrorMsg(CString("Could not close factory!"), status);

        m_hFactory = NULL;
    }

    J_Image_Free(&m_OutputImage);

    // Release and delete the Critical Section again
    DeleteCriticalSection(&m_HDRProcessingCriticalSection);

    return CDialog::DestroyWindow();
}

void CHdrSequenceSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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


void CHdrSequenceSampleDlg::OnBnClickedSearchButton()
{
    J_STATUS_TYPE err = J_ST_SUCCESS;

    err = J_Factory_Open((int8_t*)"", &m_hFactory);
    if (err != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to open the JAI SDK Factory"), err);
        return;
    }

    bool8_t hasChanged = false;
    err = J_Factory_UpdateCameraList(m_hFactory, &hasChanged);
    if (err != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to update the camera list!"), err);
        return;
    }

    uint32_t numCameras = 0;
    err = J_Factory_GetNumOfCameras(m_hFactory, &numCameras);
    if (err != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get the number of detected cameras!"), err);
        return;
    }

    if (numCameras < 1)
    {
        AfxMessageBox(_T("No cameras detected!"));
        return;
    }

    uint32_t size = sizeof(m_CameraIDString);
    err = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_CameraIDString, &size);
    if (err != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get the camera ID string!"), err);
        return;
    }

    // Get the camera model from the Camera ID
    size = sizeof(m_CameraModelString);
    err = J_Factory_GetCameraInfo(m_hFactory, m_CameraIDString, CAM_INFO_MODELNAME, m_CameraModelString, &size);
    if (err != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get the camera model name string!"), err);
        return;
    }

    m_CameraIdEdit.SetWindowText(CString((char*)m_CameraModelString));

    InitializeControls();   // Initialize Controls

    UpdateData(FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CHdrSequenceSampleDlg::OnBnClickedStartAcqButton()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    int64_t int64Val;
    int64_t pixelFormat;

    SIZE	ViewSize;
    RECT    PictureRect;

    // Get Max Width from the camera based on GenICam name
    retval = J_Camera_GetNodeByName(m_hCamera, NODE_NAME_WIDTH, &hNode);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get Width node handle!"), retval);
        return;
    }
    retval = J_Node_GetMaxInt64(hNode, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get Width node max!"), retval);
        return;
    }
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

    // Get Max Height from the camera
    retval = J_Camera_GetNodeByName(m_hCamera, NODE_NAME_HEIGHT, &hNode);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get Height node handle!"), retval);
        return;
    }
    retval = J_Node_GetMaxInt64(hNode, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get Height node max!"), retval);
        return;
    }
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

    // Get pixelformat from the camera
    retval = J_Camera_GetValueInt64(m_hCamera, NODE_NAME_PIXELFORMAT, &pixelFormat);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Unable to get PixelFormat value!"), retval);
        return;
    }

    m_PixelCount = ViewSize.cx * ViewSize.cy;

    // Calculate number of bits (not bytes) per pixel using macro
    int bpp = J_BitsPerPixel(pixelFormat);

    // Get the client window rectangle to be used for the display
    m_PictureControl1.GetClientRect(&PictureRect);

	// Open view window
    retval = J_Image_OpenViewWindowEx(J_IVW_CHILD_STRETCH, _T("Image View Window"), &PictureRect, &ViewSize, m_PictureControl1.m_hWnd, &m_hView[0]);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window 1!"), retval);
        return;
    }

    m_PictureControl2.GetClientRect(&PictureRect);

    // Open view window
    retval = J_Image_OpenViewWindowEx(J_IVW_CHILD_STRETCH, _T("Image View Window"), &PictureRect, &ViewSize, m_PictureControl2.m_hWnd, &m_hView[1]);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window 2!"), retval);
        return;
    }

    m_PictureControl3.GetClientRect(&PictureRect);

    // Open view window
    retval = J_Image_OpenViewWindowEx(J_IVW_CHILD_STRETCH, _T("Image View Window"), &PictureRect, &ViewSize, m_PictureControl3.m_hWnd, &m_hView[2]);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window 3!"), retval);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Create the HDR Processing Thread
    CreateHDRProcessingThread();

    // Create image acquisition thread (this allocates buffers)
    CreateStreamThread(m_hCamera, 0, (ViewSize.cx * ViewSize.cy * bpp)/8);

    TRACE("Opening stream succeeded\n");

    // Start Acquisition
    retval = J_Camera_ExecuteCommand(m_hCamera, NODE_NAME_ACQSTART);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not start acquisition!"), retval);
        return;
    }

    // Enable controls
    EnableControls(TRUE, TRUE);

    SetTimer(0, 100, NULL);

    // Startup the automatic triggering
    if (m_AutoTrig)
        OnBnClickedTrigButton();

	ResizeChildWindow(m_PictureControl1);
	ResizeChildWindow(m_PictureControl2);
	ResizeChildWindow(m_PictureControl3);
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CHdrSequenceSampleDlg::OnBnClickedStopAcqButton()
{
    J_STATUS_TYPE retval;

    // Stop Acquision
    if (m_hCamera)
    {
        retval = J_Camera_ExecuteCommand(m_hCamera, NODE_NAME_ACQSTOP);
        if (retval != J_ST_SUCCESS) {
            ShowErrorMsg(CString("Could not stop acquisition!"), retval);
        }
    }

    // Close stream (this frees all allocated buffers)
    TerminateStreamThread();
    TRACE("Closed stream\n");

    TerminateHDRProcessingThread();

    // View window opened?
    for (int i=0; i<VIEWCOUNT; i++)
    {
        if(m_hView[i])
        {
            // Close view window
            retval = J_Image_CloseViewWindow(m_hView[i]);
            if (retval != J_ST_SUCCESS) {
                ShowErrorMsg(CString("Could not close view window!"), retval);
            }
            m_hView[i] = NULL;
            TRACE("Closed view window\n");
        }
    }
    EnableControls(TRUE, FALSE);

    KillTimer(0);
}

//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CHdrSequenceSampleDlg::InitializeControls()
{
    uint32_t numEnums;
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    NODE_HANDLE hEnumNode;
    int64_t int64Val;
    int comboboxIndex = 0;
    bool IsBayerCamera = false;

    if (m_hCamera)
    {
        // Here we want to populate the ComboBox with the pixelformats from the camera
        retval = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"PixelFormat", &hNode);
        if (retval == J_ST_SUCCESS)
        {
            retval = J_Node_GetNumOfEnumEntries(hNode, &numEnums);

            m_PixelFormatCtrl.ResetContent();

            for (int i=0; i<(int)numEnums; i++)
            {
                retval = J_Node_GetEnumEntryByIndex(hNode, i, &hEnumNode);
                if (retval == J_ST_SUCCESS)
                {
                    J_NODE_ACCESSMODE accessmode;

                    // Get access mode so we know if it is available or not
                    retval = J_Node_GetAccessMode(hEnumNode, &accessmode);

                    if ((accessmode != NI) && (accessmode != NA))
                    {
                        int8_t buffer[100];
                        uint32_t size = sizeof(buffer);
                        retval = J_Node_GetDescription(hEnumNode, buffer, &size);
                        if (retval == J_ST_SUCCESS)
                        {
                            CString *pValuestring = new CString((char*)buffer);
                            comboboxIndex = m_PixelFormatCtrl.AddString(pValuestring->GetString());
                            if (pValuestring->Find(_T("BAY"))!=-1)
                                IsBayerCamera = true;

                            delete pValuestring;
                        }
                    }
                }

                retval = J_Node_GetEnumEntryValue(hEnumNode, &int64Val);

                // Get the PixelFormat Enum value and put it in an array with same index as for the ComboBox
                m_PixelFormatValues[comboboxIndex] = int64Val;
            }
        }
        else
        {
            ShowErrorMsg(CString("Unable to get PixelFormat node handle!"), retval);
        }

        // Get the current selected PixelFormat value and select it in the ComboBox
        retval = J_Camera_GetValueInt64(m_hCamera, (int8_t*)"PixelFormat", &int64Val);

        if (retval == J_ST_SUCCESS)
        {
            for (int i=0; i<(int)numEnums; i++)
            {
                if (m_PixelFormatValues[i] == int64Val)
                {
                    m_PixelFormatInt = i;
                    m_PixelFormatCtrl.SetCurSel(i);
                    break;
                }
            }
        }

        if (IsBayerCamera)
        {
            GetDlgItem(IDC_WB_STATIC)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_RGAIN_EDIT)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_RGAIN_SPIN)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_BGAIN_EDIT)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_BGAIN_SPIN)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_RGAIN_STATIC)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_BGAIN_STATIC)->ShowWindow(SW_SHOW);
            GetDlgItem(IDC_WB_BUTTON)->ShowWindow(SW_SHOW);
        }
        else
        {
            GetDlgItem(IDC_WB_STATIC)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_RGAIN_EDIT)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_RGAIN_SPIN)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_BGAIN_EDIT)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_BGAIN_SPIN)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_RGAIN_STATIC)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_BGAIN_STATIC)->ShowWindow(SW_HIDE);
            GetDlgItem(IDC_WB_BUTTON)->ShowWindow(SW_HIDE);
        }
    }

    CSliderCtrl* pSCtrl;

    //- Exposure Time 1 ------------------------------------------------

    // Get SliderCtrl for Exposure Time 1
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_EXPOSURE_TIME_SLIDER1);

    // Set SequenceSelector="Sequence1"
    retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence1");

    // Get Sequence Exposure Time Raw Node
    retval = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceExposureTimeRaw", &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Min
        retval = J_Node_GetMinInt64(hNode, &int64Val);
        pSCtrl->SetRangeMin((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MIN_STATIC1, (int)int64Val);

        // Get/Set Max
        retval = J_Node_GetMaxInt64(hNode, &int64Val);
        pSCtrl->SetRangeMax((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MAX_STATIC1, (int)int64Val);

        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
        pSCtrl->SetPos((int)int64Val);

        SetDlgItemInt(IDC_EXPOSURE_TIME1, (int)int64Val);
        pSCtrl->EnableWindow(true);
        GetDlgItem(IDC_EXPOSURE_TIME1)->EnableWindow(true);
        GetDlgItem(IDC_MIN_STATIC1)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_MAX_STATIC1)->ShowWindow(SW_SHOW);
    }
    else
    {
        pSCtrl->EnableWindow(false);
        GetDlgItem(IDC_EXPOSURE_TIME1)->EnableWindow(false);
        GetDlgItem(IDC_MIN_STATIC1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_MAX_STATIC1)->ShowWindow(SW_HIDE);
    }

    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER1);

    // Get Sequence Gain Raw Node
    retval = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceMasterGain", &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Min
        retval = J_Node_GetMinInt64(hNode, &int64Val);
        pSCtrl->SetRangeMin((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MIN_GAIN_STATIC1, (int)int64Val);

        // Get/Set Max
        retval = J_Node_GetMaxInt64(hNode, &int64Val);
        pSCtrl->SetRangeMax((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MAX_GAIN_STATIC1, (int)int64Val);

        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
        pSCtrl->SetPos((int)int64Val);

        SetDlgItemInt(IDC_GAIN_EDIT1, (int)int64Val);
        pSCtrl->EnableWindow(true);
        GetDlgItem(IDC_GAIN_EDIT1)->EnableWindow(true);
        GetDlgItem(IDC_MIN_GAIN_STATIC1)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_MAX_GAIN_STATIC1)->ShowWindow(SW_SHOW);
    }
    else
    {
        pSCtrl->EnableWindow(false);
        GetDlgItem(IDC_GAIN_EDIT1)->EnableWindow(false);
        GetDlgItem(IDC_MIN_GAIN_STATIC1)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_MAX_GAIN_STATIC1)->ShowWindow(SW_HIDE);
    }

    //- Exposure Time 2 ------------------------------------------------

    // Get SliderCtrl for Exposure Time 2
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_EXPOSURE_TIME_SLIDER2);

    // Set SequenceSelector="Sequence2"
    retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence2");

    // Get Sequence Exposure Time Raw Node
    retval = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceExposureTimeRaw", &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Min
        retval = J_Node_GetMinInt64(hNode, &int64Val);
        pSCtrl->SetRangeMin((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MIN_STATIC2, (int)int64Val);

        // Get/Set Max
        retval = J_Node_GetMaxInt64(hNode, &int64Val);
        pSCtrl->SetRangeMax((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MAX_STATIC2, (int)int64Val);

        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
        pSCtrl->SetPos((int)int64Val);

        SetDlgItemInt(IDC_EXPOSURE_TIME2, (int)int64Val);
        pSCtrl->EnableWindow(true);
        GetDlgItem(IDC_EXPOSURE_TIME2)->EnableWindow(true);
        GetDlgItem(IDC_MIN_STATIC2)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_MAX_STATIC2)->ShowWindow(SW_SHOW);
    }
    else
    {
        pSCtrl->EnableWindow(false);
        GetDlgItem(IDC_EXPOSURE_TIME2)->EnableWindow(false);
        GetDlgItem(IDC_MIN_STATIC2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_MAX_STATIC2)->ShowWindow(SW_HIDE);
    }

    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER2);

        // Get Sequence Gain Raw Node
    retval = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceMasterGain", &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Min
        retval = J_Node_GetMinInt64(hNode, &int64Val);
        pSCtrl->SetRangeMin((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MIN_GAIN_STATIC2, (int)int64Val);

        // Get/Set Max
        retval = J_Node_GetMaxInt64(hNode, &int64Val);
        pSCtrl->SetRangeMax((int)int64Val, TRUE);
        SetDlgItemInt(IDC_MAX_GAIN_STATIC2, (int)int64Val);

        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
        pSCtrl->SetPos((int)int64Val);

        SetDlgItemInt(IDC_GAIN_EDIT2, (int)int64Val);
        pSCtrl->EnableWindow(true);
        GetDlgItem(IDC_GAIN_EDIT2)->EnableWindow(true);
        GetDlgItem(IDC_MIN_GAIN_STATIC2)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_MAX_GAIN_STATIC2)->ShowWindow(SW_SHOW);
    }
    else
    {
        pSCtrl->EnableWindow(false);
        GetDlgItem(IDC_GAIN_EDIT2)->EnableWindow(false);
        GetDlgItem(IDC_MIN_GAIN_STATIC2)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_MAX_GAIN_STATIC2)->ShowWindow(SW_HIDE);
    }
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CHdrSequenceSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
    GetDlgItem(IDC_OPEN_BUTTON)->EnableWindow(bIsCameraReady ? FALSE : TRUE);
    GetDlgItem(IDC_CLOSE_BUTTON)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
    GetDlgItem(IDC_PIXELFORMAT_COMBO)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_START_ACQ_BUTTON)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_STOP_ACQ_BUTTON)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_TRIG_BUTTON)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_AUTOTRIG_CHECK)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_ANALYZE_BUTTON)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
}

//--------------------------------------------------------------------------------------------------
// Create Stream Thread
//--------------------------------------------------------------------------------------------------
BOOL CHdrSequenceSampleDlg::CreateStreamThread(CAM_HANDLE hCam, uint32_t iChannel, uint32_t iBufferSize)
{
    // Is it already created?
    if(m_hStreamThread)
        return FALSE;

    // Open the stream channel(GVSP)
    if(m_hDS == NULL)
    {
        if((J_ST_SUCCESS != J_Camera_CreateDataStream(hCam, iChannel, &m_hDS))
            || (m_hDS == NULL))
        {
            OutputDebugString(_T("*** Error : J_Camera_CreateDataStream\n"));
            return FALSE;
        }
    }

    // Prepare the frame buffers (this announces the buffers to the acquisition engine)
    if(0 == PrepareBuffer(iBufferSize))
    {
        OutputDebugString(_T("*** Error : PrepareBuffer\n"));
        J_DataStream_Close(m_hDS);
        return FALSE;
    }

    // Stream thread event created?
    if(m_hStreamEvent == NULL)
        m_hStreamEvent = CreateEvent(NULL, true, false, NULL);
    else
        ResetEvent(m_hStreamEvent);

    // Set the thread execution flag
    m_bEnableThread = true;

    // Create a Stream Thread.
    if(NULL == (m_hStreamThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProcessCaller, this, NULL, NULL)))
    {
        OutputDebugString(_T("*** Error : CreateThread\n"));
        J_DataStream_Close(m_hDS);
        return FALSE;
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------------------
// Create Stream Thread
//--------------------------------------------------------------------------------------------------
BOOL CHdrSequenceSampleDlg::CreateHDRProcessingThread(void)
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
BOOL CHdrSequenceSampleDlg::TerminateStreamThread(void)
{
    // Is the data stream opened?
    if(m_hDS == NULL)
        return FALSE;

    // Reset the thread execution flag.
    m_bEnableThread = false;

    // Signal the image thread to stop faster
    J_Event_ExitCondition(m_hNewImageCondition);

    // Wait for the thread to end
    WaitForThreadToTerminate();

    // Stop the image acquisition engine
    J_DataStream_StopAcquisition(m_hDS, ACQ_STOP_FLAG_KILL);

    // UnPrepare Buffers (this removed the buffers from the acquisition engine and frees buffers)
    UnPrepareBuffer();

    // Close Stream
    if(m_hDS)
    {
        J_DataStream_Close(m_hDS);
        m_hDS = NULL;
    }

    return TRUE;
}

//==============================================================////
// Terminate HDR Processing Thread
//==============================================================////
BOOL CHdrSequenceSampleDlg::TerminateHDRProcessingThread(void)
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
void CHdrSequenceSampleDlg::StreamProcess(void)
{
    J_STATUS_TYPE	iResult;
    uint32_t		    tSize;
    BUF_HANDLE	    iBufferID;
    int state = 0;
    int             iImageMask = 0;

    // Create structure to be used for image display
    J_tIMAGE_INFO	tAqImageInfo = {0, 0, 0, 0, NULL};

    J_COND_WAIT_RESULT	WaitResult;

    // Create the condition used for signaling the new image event
    iResult = J_Event_CreateCondition(&m_hNewImageCondition);

    EVT_HANDLE	hEvent; // Buffer event handle
	EVENT_NEW_BUFFER_DATA eventData;	// Strut for EventGetData

    // Register the event with the acquisition engine
    J_DataStream_RegisterEvent(m_hDS, EVENT_NEW_BUFFER, m_hNewImageCondition, &hEvent); 

    // Start image acquisition
    iResult = J_DataStream_StartAcquisition(m_hDS, ACQ_START_NEXT_IMAGE, 0 );

    // Loop of Stream Processing
    OutputDebugString(_T(">>> Start Stream Process Loop.\n"));

    while(m_bEnableThread)
    {
        // Wait for Buffer event (or kill event)
        iResult = J_Event_WaitForCondition(m_hNewImageCondition, 1000, &WaitResult);

        // Did we get a new buffer event?
        if(J_COND_WAIT_SIGNAL == WaitResult)
        {
            // Trigger the next image!
            if (m_AutoTrig)
                OnBnClickedTrigButton();

            // Get the Buffer Handle from the event
            tSize = (uint32_t)sizeof(void *);
            iResult = J_Event_GetData(hEvent, &eventData,  &tSize);

			iBufferID = eventData.BufferHandle;

            if ((iResult == J_ST_SUCCESS) && (iBufferID != 0))
            {
                // Fill in structure for image display
                // Get the pointer to the frame buffer.
                tSize = (uint32_t)sizeof (void *);
                iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_BASE	, &(tAqImageInfo.pImageBuffer), &tSize);
                // Get the effective data size.
                tSize = (uint32_t)sizeof (uint32_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_SIZE	, &(tAqImageInfo.iImageSize), &tSize);
                // Get Pixel Format Type.
                tSize = (uint32_t)sizeof (uint32_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_PIXELTYPE, &(tAqImageInfo.iPixelType), &tSize);
                // Get Frame Width.
                tSize = (uint32_t)sizeof (uint32_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_WIDTH	, &(tAqImageInfo.iSizeX), &tSize);
                // Get Frame Height.
                tSize = (uint32_t)sizeof (uint32_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_HEIGHT	, &(tAqImageInfo.iSizeY), &tSize);
                // Get Timestamp.
                tSize = (uint32_t)sizeof (uint64_t);
                iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_TIMESTAMP, &(tAqImageInfo.iTimeStamp), &tSize);

                if(m_bEnableThread)
                {
                    // Display image
                    switch (state)
                    {
                        case 0: // Get first image
//                            if (m_AnalysisOK)
                            {
                                // Try to get the brightest Image in view 1
                                if (m_IsImage1Brighter)
                                {
                                    if(m_hView[0])
                                    {
                                        // Shows image
                                        J_Image_ShowImage(m_hView[0], &tAqImageInfo);
                                    }
                                }
                                else
                                {
                                    if(m_hView[1])
                                    {
                                        // Shows image
                                        J_Image_ShowImage(m_hView[1], &tAqImageInfo);
                                    }
                                }
                            }

                            // See if we are currently processing
                            EnterCriticalSection(&m_HDRProcessingCriticalSection);
                            iImageMask = m_ImageMask;
                            LeaveCriticalSection(&m_HDRProcessingCriticalSection);

                            // We have to check if we already have an image in the processing buffer!
                            if (!(iImageMask & (0x0001<<state)))
                            {
                                EnterCriticalSection(&m_HDRProcessingCriticalSection);
                                // Copy the Image Info contents
                                // It is important to notice that we don't copy the image data because we assume
                                // the the buffer will not be overwritten since the HDR processing is fast enought to
                                // be run in "real time"
                                memcpy((void*)&m_Image1Info, (const void*)&tAqImageInfo, sizeof(tAqImageInfo));

                                m_ImageMask |= (0x0001<<state);

                                // Now we need to check if we got a pair of images
                                if (m_ImageMask == 0x03)
                                {
                                    // Signal the HDR processing thread to start if it is not already active
                                    if (m_hHDRProcessingCondition != NULL)
                                        J_Event_SignalCondition(m_hHDRProcessingCondition);
                                }
                                LeaveCriticalSection(&m_HDRProcessingCriticalSection);
                            }
                            break;

                        case 1: // Get second image and start the processing
//                            if (m_AnalysisOK)
                            {
                                // Try to get the brightest Image in view 1
                                if (m_IsImage1Brighter)
                                {
                                    if(m_hView[1])
                                    {
                                        // Shows image
                                        J_Image_ShowImage(m_hView[1], &tAqImageInfo);
                                    }
                                }
                                else
                                {
                                    if(m_hView[0])
                                    {
                                        // Shows image
                                        J_Image_ShowImage(m_hView[0], &tAqImageInfo);
                                    }
                                }
                            }
                            // See if we are currently processing
                            EnterCriticalSection(&m_HDRProcessingCriticalSection);
                            iImageMask = m_ImageMask;
                            LeaveCriticalSection(&m_HDRProcessingCriticalSection);

                            // We have to check if we already have an image in the processing buffer!
                            if (!(iImageMask & (0x0001<<state)))
                            {
                                EnterCriticalSection(&m_HDRProcessingCriticalSection);
                                // Copy the Image Info contents
                                // It is important to notice that we don't copy the image data because we assume
                                // the the buffer will not be overwritten since the HDR processing is fast enought to
                                // be run in "real time"
                                memcpy((void*)&m_Image2Info, (const void*)&tAqImageInfo, sizeof(tAqImageInfo));

                                m_ImageMask |= (0x0001<<state);

                                // Now we need to check if we got a pair of images
                                if (m_ImageMask == 0x03)
                                {
                                    // Signal the HDR processing thread to start if it is not already active
                                    if (m_hHDRProcessingCondition != NULL)
                                        J_Event_SignalCondition(m_hHDRProcessingCondition);
                                }
                                LeaveCriticalSection(&m_HDRProcessingCriticalSection);
                            }
                            break;
                    }

                    // Queue This Buffer Again for reuse in acquisition engine
                    iResult = J_DataStream_QueueBuffer(m_hDS, iBufferID);
                }

                state++;
                if (state > 1)
                    state = 0;
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
    OutputDebugString(_T(">>> Terminated Stream Process Loop.\n"));

    // Unregister new buffer event with acquisition engine
    J_DataStream_UnRegisterEvent(m_hDS, EVENT_NEW_BUFFER); 

    // Free the event object
    if (hEvent != NULL)
    {
        J_Event_Close(hEvent);
        hEvent = NULL;
    }

    // Terminate the thread.
    TerminateThread();

    // Free the Condition
    if (m_hNewImageCondition != NULL)
    {
        J_Event_CloseCondition(m_hNewImageCondition);
        m_hNewImageCondition = NULL;
    }
}

//==============================================================////
// Terminate image acquisition thread
//==============================================================////
void CHdrSequenceSampleDlg::TerminateThread(void)
{
    SetEvent(m_hStreamEvent);
}

//==============================================================////
// Wait for thread to terminate
//==============================================================////
void CHdrSequenceSampleDlg::WaitForThreadToTerminate(void)
{
    WaitForSingleObject(m_hStreamEvent, INFINITE);

    // Close the thread handle and stream event handle
    CloseThreadHandle();
}

//==============================================================////
// Wait for HDR Processing thread to terminate
//==============================================================////
void CHdrSequenceSampleDlg::WaitForHDRProcessingThreadToTerminate()
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
void CHdrSequenceSampleDlg::CloseThreadHandle(void)
{
    if(m_hStreamThread)
    {
        CloseHandle(m_hStreamThread);
        m_hStreamThread = NULL;
    }

    if(m_hStreamEvent)
    {
        CloseHandle(m_hStreamEvent);
        m_hStreamEvent = NULL;
    }
}

//==============================================================////
// Prepare frame buffers
//==============================================================////
uint32_t CHdrSequenceSampleDlg::PrepareBuffer(int iBufferSize)
{
    J_STATUS_TYPE	iResult = J_ST_SUCCESS;
    int			i;

    m_iValidBuffers = 0;

    for(i = 0 ; i < NUM_OF_BUFFER ; i++)
    {
        // Make the buffer for one frame. 
        m_pAquBuffer[i] = (uint8_t*)_aligned_malloc(iBufferSize, 16);

        // Announce the buffer pointer to the Acquisition engine.
        if(J_ST_SUCCESS != J_DataStream_AnnounceBuffer(m_hDS, m_pAquBuffer[i] ,iBufferSize , NULL, &(m_pAquBufferID[i])))
        {
            delete m_pAquBuffer[i];
            break;
        }

        // Queueing it.
        if(J_ST_SUCCESS != J_DataStream_QueueBuffer(m_hDS, m_pAquBufferID[i]))
        {
            delete m_pAquBuffer[i];
            break;
        }

        m_iValidBuffers++;
    }

    return m_iValidBuffers;
}

//==============================================================////
// Unprepare buffers
//==============================================================////
BOOL CHdrSequenceSampleDlg::UnPrepareBuffer(void)
{
    void		*pPrivate;
    void		*pBuffer;
    uint32_t	i;

    // Flush Queues
    J_DataStream_FlushQueue(m_hDS, ACQ_QUEUE_INPUT_TO_OUTPUT);
    J_DataStream_FlushQueue(m_hDS, ACQ_QUEUE_OUTPUT_DISCARD);

    for(i = 0 ; i < m_iValidBuffers ; i++)
    {
        // Remove the frame buffer from the Acquisition engine.
        J_DataStream_RevokeBuffer(m_hDS, m_pAquBufferID[i], &pBuffer , &pPrivate);

        _aligned_free(m_pAquBuffer[i]);
        m_pAquBuffer[i] = NULL;
        m_pAquBufferID[i] = 0;
    }

    m_iValidBuffers = 0;

    return TRUE;
}

//==============================================================////
// HDR Processing Function
//==============================================================////
void CHdrSequenceSampleDlg::HDRProcessingThread(void)
{
    J_STATUS_TYPE	iResult;
    int             iImageMask = 0;
    J_COND_WAIT_RESULT	WaitResult;

    // Create the condition used for signaling the new image event
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
                    iResult = J_Image_AnalyzeHDR(&m_Image1Info, &m_Image2Info, &m_IsImage1Brighter, &m_NewImage1BlackLevel, &m_NewImage2BlackLevel, &m_NewDarkGain);
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
                        iResult = J_Image_MallocHDR(&m_Image1Info, &m_Image2Info, &m_OutputImage);

                    // For performance measurements only!
                    QueryPerformanceCounter(&liStart);

                    // Do the HDR merging of the two images
                    if (m_IsImage1Brighter)
                        J_Image_FuseHDR(&m_Image1Info, &m_Image2Info, &m_OutputImage, m_Image1BlackLevel, m_Image2BlackLevel, m_DarkGain, m_DualSlope, m_LogOutput);
                    else
                        J_Image_FuseHDR(&m_Image2Info, &m_Image1Info, &m_OutputImage, m_Image2BlackLevel, m_Image1BlackLevel, m_DarkGain, m_DualSlope, m_LogOutput);

                    QueryPerformanceCounter(&liStop);
                    int iTimeSpan = (int)((liStop.QuadPart - liStart.QuadPart) * 1000000 / liFreq.QuadPart);
                    m_ProcessingTimeValue = iTimeSpan;

                    // Finally show the 16-bit image to the display
                    J_Image_ShowImage(m_hView[2], &m_OutputImage, (uint32_t)(4096.0*m_RGain), 4096, (uint32_t)(4096.0*m_BGain));
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


void CHdrSequenceSampleDlg::PrepareContinuousSetting()
{
	if(m_hCamera)
	{
		J_STATUS_TYPE status;

        // Set up the camera for continuous exposure mode

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
}

void CHdrSequenceSampleDlg::PrepareSwTrigSetting()
{
	if(m_hCamera)
	{
		J_STATUS_TYPE status;
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
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"TriggerSource", (int8_t*)"Software");
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
            // Here we assume that this is JAI trigger so we do the following:
            // ExposureMode=SequentialEPSTrigger
            // LineSelector=CameraTrigger0
            // LineSource=SoftwareTrigger0

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

		    // Set LineSource="SoftwareTrigger0" 
            status = J_Camera_SetValueString(m_hCamera, (int8_t*)"LineSource", (int8_t*)"SoftwareTrigger0");
		    if(status != J_ST_SUCCESS) 
		    {
			    ShowErrorMsg(CString("Could not set LineSource!"), status);
			    return;
		    }
        }

        // Set the Sequence Repetition count to 0 (0=forever)
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceRepetitions", 0);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not set SequenceRepetitions!"), status);
			return;
		}

        // Set the last sequence number to 2 (2 step sequence)
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceEndingPosition", 2);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not set SequenceEndingPosition!"), status);
			return;
		}

        // Set the ROI's for the two sequences to the whole image size
        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceSelector", 0);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not set SequenceSelector!"), status);
			return;
		}

        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetX", 0);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not set SequenceROIOffsetX!"), status);
			return;
		}

        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetY", 0);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not set SequenceROIOffsetY!"), status);
			return;
		}

        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceROISizeX", &hNode);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not get SequenceROISizeX node handle!"), status);
			return;
		}
        J_Node_GetMaxInt64(hNode, &int64Val);
		J_Node_SetValueInt64(hNode, FALSE, int64Val);

        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceROISizeY", &hNode);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not get SequenceROISizeY node handle!"), status);
			return;
		}
        J_Node_GetMaxInt64(hNode, &int64Val);
		J_Node_SetValueInt64(hNode, FALSE, int64Val);

        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceSelector", 1);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not set SequenceSelector!"), status);
			return;
		}

        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetX", 0);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not set SequenceROIOffsetX!"), status);
			return;
		}

        status = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceROIOffsetY", 0);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not set SequenceROIOffsetY!"), status);
			return;
		}

        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceROISizeX", &hNode);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not get SequenceROISizeX node handle!"), status);
			return;
		}
        status = J_Node_GetMaxInt64(hNode, &int64Val);
		status = J_Node_SetValueInt64(hNode, FALSE, int64Val);

        status = J_Camera_GetNodeByName(m_hCamera, (int8_t*)"SequenceROISizeY", &hNode);
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not get SequenceROISizeY node handle!"), status);
			return;
		}
        status = J_Node_GetMaxInt64(hNode, &int64Val);
		status = J_Node_SetValueInt64(hNode, FALSE, int64Val);

        // Save Sequence Settings
        status = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"SequenceSaveCommand");
		if(status != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not execute SequenceSaveCommand command!"), status);
			return;
		}
	} // end of if(m_hCamera)
 }

 void CHdrSequenceSampleDlg::OnBnClickedTrigButton()
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


void CHdrSequenceSampleDlg::OnBnClickedOpenButton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

    // Open the camera
    if (strlen((char*)m_CameraIDString))
    {
        status = J_Camera_Open(m_hFactory, m_CameraIDString, &m_hCamera);

	    if(status != J_ST_SUCCESS) 
	    {
		    ShowErrorMsg(CString("Could not open camera!"), status);
		    return;
	    }
        EnableControls(TRUE, FALSE);
        PrepareSwTrigSetting();
        InitializeControls();
    }
    else
    {
        EnableControls(FALSE, FALSE);
    }
}

void CHdrSequenceSampleDlg::OnBnClickedCloseButton()
{
	J_STATUS_TYPE status = J_ST_SUCCESS;

    // Stop Acquisition if it is started
    OnBnClickedStopAcqButton();

    // ... and close the camera connection
    if (m_hCamera)
    {
        status = J_Camera_Close(m_hCamera);

	    if(status != J_ST_SUCCESS) 
	    {
		    ShowErrorMsg(CString("Could not close camera!"), status);
		    return;
	    }
        m_hCamera = NULL;
    }
    EnableControls(FALSE, FALSE);

    // Remove the White-Balance controls
    GetDlgItem(IDC_WB_STATIC)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RGAIN_EDIT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RGAIN_SPIN)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BGAIN_EDIT)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BGAIN_SPIN)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_RGAIN_STATIC)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BGAIN_STATIC)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_WB_BUTTON)->ShowWindow(SW_HIDE);
}

void CHdrSequenceSampleDlg::OnBnClickedAnalyzeButton()
{
    m_AnalysisOK = false;
    m_AnalysisRequested = true;
}

void CHdrSequenceSampleDlg::OnTimer(UINT_PTR nIDEvent)
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
            DarkBlackLevel = (float)m_Image2BlackLevel;
        else
            DarkBlackLevel = (float)m_Image1BlackLevel;

        float res = log(m_DarkGain*(1023.0f-DarkBlackLevel))/log(2.0f);
        if (m_PixelFormatInt == 0)
            res = log(m_DarkGain*(255.0f-DarkBlackLevel))/log(2.0f);

        valueString.Format(_T("%0.1f"), res);
        SetDlgItemText(IDC_RESOLUTION_EDIT, valueString);

        if (m_Image1BlackLevel != m_NewImage1BlackLevel)
        {
            m_Image1BlackLevel = m_NewImage1BlackLevel;
            SetDlgItemInt(IDC_BLACK_LEVEL_EDIT1, m_Image1BlackLevel);
        }
        if (m_Image2BlackLevel != m_NewImage2BlackLevel)
        {
            m_Image2BlackLevel = m_NewImage2BlackLevel;
            SetDlgItemInt(IDC_BLACK_LEVEL_EDIT2, m_Image2BlackLevel);
        }

        UpdateData(0);
    }

    CDialog::OnTimer(nIDEvent);
}

void CHdrSequenceSampleDlg::OnCbnSelchangePixelformatCombo()
{
    // Change the Pixel Format 
    int index = m_PixelFormatCtrl.GetCurSel();

    J_Camera_SetValueInt64(m_hCamera, (int8_t*)"PixelFormat", m_PixelFormatValues[index]);

    m_AnalysisOK = false;
    m_AnalysisRequested = true;
}

void CHdrSequenceSampleDlg::OnBnClickedAutotrigCheck()
{
    UpdateData();
}

void CHdrSequenceSampleDlg::OnBnClickedLogCheck()
{
    UpdateData();
}

void CHdrSequenceSampleDlg::OnBnClickedOk()
{
    // Close the camera connections
    // .. this will automatically close the Acquisition as well
    OnBnClickedCloseButton();

    OnOK();
}

void CHdrSequenceSampleDlg::ReAnalyze()
{
    m_AnalysisOK = false;
    m_AnalysisRequested = true;
}

void CHdrSequenceSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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
        // Set Sequence Selector to Sequence 1
        retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence1");

        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Get Sequence Exposure Time Raw Node
        retval = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceExposureTimeRaw", int64Val);

        SetDlgItemInt(IDC_EXPOSURE_TIME1, iPos);
        GetDlgItem(IDC_CHANGE_SEQUENCE_BUTTON)->EnableWindow(true);
        ReAnalyze();
    }

    // Get SliderCtrl for Exposure Time 1
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER1);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        // Set Sequence Selector to Sequence 1
        retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence1");

        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Get Sequence Exposure Time Raw Node
        retval = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceMasterGain", int64Val);

        SetDlgItemInt(IDC_GAIN_EDIT1, iPos);
        GetDlgItem(IDC_CHANGE_SEQUENCE_BUTTON)->EnableWindow(true);
        ReAnalyze();
    }

    // Get SliderCtrl for Exposure Time 2
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_EXPOSURE_TIME_SLIDER2);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        // Set Sequence Selector to Sequence 1
        retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence2");

        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Get Sequence Exposure Time Raw Node
        retval = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceExposureTimeRaw", int64Val);

        SetDlgItemInt(IDC_EXPOSURE_TIME2, iPos);
        GetDlgItem(IDC_CHANGE_SEQUENCE_BUTTON)->EnableWindow(true);
        ReAnalyze();
    }

        // Get SliderCtrl for Exposure Time 1
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER2);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        // Set Sequence Selector to Sequence 1
        retval = J_Camera_SetValueString(m_hCamera, (int8_t*)"SequenceSelector", (int8_t*)"Sequence2");

        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Get Sequence Exposure Time Raw Node
        retval = J_Camera_SetValueInt64(m_hCamera, (int8_t*)"SequenceMasterGain", int64Val);

        SetDlgItemInt(IDC_GAIN_EDIT2, iPos);
        GetDlgItem(IDC_CHANGE_SEQUENCE_BUTTON)->EnableWindow(true);
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

void CHdrSequenceSampleDlg::OnBnClickedChangeSequenceButton()
{
    // Update the Sequence Settings 
    J_STATUS_TYPE   retval;

    // Execute the 
    retval = J_Camera_ExecuteCommand(m_hCamera, (int8_t*)"SequenceSaveCommand");

    m_AnalysisOK = false;
    m_AnalysisRequested = true;
}

void CHdrSequenceSampleDlg::OnEnChangeBlackLevelEdit1()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    m_NewImage1BlackLevel = GetDlgItemInt(IDC_BLACK_LEVEL_EDIT1);
}

void CHdrSequenceSampleDlg::OnEnChangeBlackLevelEdit2()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    m_NewImage2BlackLevel = GetDlgItemInt(IDC_BLACK_LEVEL_EDIT2);
}

void CHdrSequenceSampleDlg::OnDeltaposGainSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    // TODO: Add your control notification handler code here
    m_NewDarkGain += (float) pNMUpDown->iDelta;
    *pResult = 0;
}

void CHdrSequenceSampleDlg::OnBnClickedSaveButton()
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
		    ShowErrorMsg(CString("Could not malloc image!"), retval);
            if (mypFileDialog)
                delete mypFileDialog;
		    return;
        }

        // Convert the raw image to image format
        retval = J_Image_FromRawToImage(&m_OutputImage, &tCnvImageInfo);
        if (retval != J_ST_SUCCESS)
        {
		    ShowErrorMsg(CString("Could not convert from Raw to Image format!"), retval);
            if (mypFileDialog)
                delete mypFileDialog;
		    return;
        }

        CString Filename = mypFileDialog->GetPathName();

        // Save the image to disk in TIFF format
        retval = J_Image_SaveFile(&tCnvImageInfo, (LPCTSTR)Filename);
        if (retval != J_ST_SUCCESS)
        {
		    ShowErrorMsg(CString("Could not save file!"), retval);
            if (mypFileDialog)
                delete mypFileDialog;
		    return;
        }

        // Free up the image buffer
        retval = J_Image_Free(&tCnvImageInfo);
        if (retval != J_ST_SUCCESS)
        {
		    ShowErrorMsg(CString("Could not free image!"), retval);
            if (mypFileDialog)
                delete mypFileDialog;
		    return;
        }
    }
    if (mypFileDialog)
        delete mypFileDialog;
}

void CHdrSequenceSampleDlg::OnBnClickedWbButton()
{
    J_tBGR48 Average;
    J_tIMAGE_INFO SampleImage;
    J_STATUS_TYPE error = J_ST_SUCCESS;

    if (m_OutputImage.pImageBuffer)
    {
        error = J_Image_Malloc(&m_OutputImage, &SampleImage);
        if (error)
        {
            ShowErrorMsg(CString("Could not malloc image!"), error);
        }

        // Convert image into Color
        error = J_Image_FromRawToImage(&m_OutputImage, &SampleImage);
        if (error)
        {
            ShowErrorMsg(CString("Could not convert from Raw to Image format!"), error);
        }

        RECT measurementRect;
        measurementRect.left = m_OutputImage.iOffsetX+10;
        measurementRect.top = m_OutputImage.iOffsetY+10;
        measurementRect.right = measurementRect.left + m_OutputImage.iSizeX - 20;
        measurementRect.bottom = measurementRect.top + m_OutputImage.iSizeY - 20;

        // Calculate new average for the output image
        error = J_Image_GetAverage(&SampleImage, &measurementRect, &Average);
        if (error)
        {
            ShowErrorMsg(CString("Could not get average!"), error);
        }

        // OK - here is the result from the average measurements!
        m_RGain = double(Average.G16) / double(Average.R16);
        m_BGain = double(Average.G16) / double(Average.B16);

        // Update the GUI values and write to registry
        CString GainString;
        GainString.Format(_T("%0.2f"), m_RGain);
        m_RGainCtrl.SetWindowText(GainString);

        AfxGetApp()->WriteProfileInt(_T("HDRSequenceSample"), _T("RGain"), int(m_RGain*100.0));
        m_RGainSpinCtrl.SetPos(int(m_RGain*100.0));

        GainString.Format(_T("%0.2f"), m_BGain);
        m_BGainCtrl.SetWindowText(GainString);

        AfxGetApp()->WriteProfileInt(_T("HDRSequenceSample"), _T("BGain"), int(m_BGain*100.0));
        m_BGainSpinCtrl.SetPos(int(m_BGain*100.0));


        error = J_Image_Free(&SampleImage);
        if (error)
        {
            ShowErrorMsg(CString("Could not free image!"), error);
        }
    }
}

void CHdrSequenceSampleDlg::OnEnChangeRgainEdit()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    //CString RGainString;
    //m_RGainCtrl.GetWindowText(RGainString);
    //m_RGain = atof((LPCSTR)RGainString.GetString());
    //m_RGainSpinCtrl.SetPos((int)(m_RGain*100.0));

//    RGainString.Format(_T("%0.2f"), m_RGain);
//    m_RGainCtrl.SetWindowText(RGainString);
}

void CHdrSequenceSampleDlg::OnDeltaposRgainSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
    
    m_RGain = (double)pNMUpDown->iPos/100.0;
    CString RGainString;
    RGainString.Format(_T("%0.2f"), m_RGain);
    m_RGainCtrl.SetWindowText(RGainString);

    AfxGetApp()->WriteProfileInt(_T("HDRSequenceSample"), _T("RGain"), int(m_RGain*100.0));

    *pResult = 0;
}

void CHdrSequenceSampleDlg::OnEnChangeBgainEdit()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    //CString BGainString;
    //m_BGainCtrl.GetWindowText(BGainString);
    //m_BGain = atof((LPCSTR)BGainString.GetString());
    //m_BGainSpinCtrl.SetPos((int)(m_BGain*100.0));

//    BGainString.Format(_T("%0.2f"), m_BGain);
//    m_BGainCtrl.SetWindowText(BGainString);
}

void CHdrSequenceSampleDlg::OnDeltaposBgainSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    m_BGain = (double)pNMUpDown->iPos/100.0;
    CString BGainString;
    BGainString.Format(_T("%0.2f"), m_BGain);
    m_BGainCtrl.SetWindowText(BGainString);

    AfxGetApp()->WriteProfileInt(_T("HDRSequenceSample"), _T("BGain"), int(m_BGain*100.0));

    *pResult = 0;
}
