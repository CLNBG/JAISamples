// TestMultiAcqisitionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestMultiAcqisition.h"
#include "TestMultiAcqisitionDlg.h"
#include "Camera.h"
#include <strstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App About
#define SECTION_NAME		"Settings"

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


// CTestMultiAcqisitionDlg dialog




CTestMultiAcqisitionDlg::CTestMultiAcqisitionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestMultiAcqisitionDlg::IDD, pParent)
	, m_nRadioThreadType(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_hFactory = NULL;
}

void CTestMultiAcqisitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_cameraList);
	DDX_Control(pDX, IDC_CHECK_DISABLE_DISPLAY, m_ctrlEnableDisplay);
	DDX_Control(pDX, IDC_CHECK_HIGH_PRIORITY, m_ctrlHighPriority);
	DDX_Control(pDX, IDC_LIST_FRAMES, m_ctrlFrameCount);
	DDX_Control(pDX, IDC_LIST_BAD_FRAMES, m_ctrlBadFrames);
	DDX_Control(pDX, IDC_SEARCH, m_ctrlSearch);
	DDX_Control(pDX, IDC_CHECK_HIGH_PRIORITY2, m_ctrlProcessImages);
	DDX_Radio(pDX, IDC_RADIO_CUSTOM_THREAD, m_nRadioThreadType);
	DDV_MinMaxInt(pDX, m_nRadioThreadType, 0, 1);
	DDX_Control(pDX, IDC_CHECK_USE_TEST_PATTERN, m_ctrlUseTestPattern);
}

BEGIN_MESSAGE_MAP(CTestMultiAcqisitionDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_START, &CTestMultiAcqisitionDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, &CTestMultiAcqisitionDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDOK, &CTestMultiAcqisitionDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_SEARCH, &CTestMultiAcqisitionDlg::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_CLOSE_CAMERA, &CTestMultiAcqisitionDlg::OnBnClickedCloseCamera)
END_MESSAGE_MAP()


// CTestMultiAcqisitionDlg message handlers

BOOL CTestMultiAcqisitionDlg::OnInitDialog()
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

	int nPriority = AfxGetApp()->GetProfileInt(SECTION_NAME, "m_ctrlHighPriority", 0);
	if(1 == nPriority)
		m_ctrlHighPriority.SetCheck(TRUE);
	else
		m_ctrlHighPriority.SetCheck(FALSE);

	int nUseTestPattern = AfxGetApp()->GetProfileInt(SECTION_NAME, "m_ctrlUseTestPattern", 0);
	if(1 == nUseTestPattern)
		m_ctrlUseTestPattern.SetCheck(TRUE);
	else
		m_ctrlUseTestPattern.SetCheck(FALSE);

	int nEnableDisplay = AfxGetApp()->GetProfileInt(SECTION_NAME, "m_ctrlEnableDisplay", 0);
	if(1 == nEnableDisplay)
		m_ctrlEnableDisplay.SetCheck(TRUE);
	else
		m_ctrlEnableDisplay.SetCheck(FALSE);

	int nProcessImages = AfxGetApp()->GetProfileInt(SECTION_NAME, "m_ctrlProcessImages", 0);
	if(1 == nProcessImages)
		m_ctrlProcessImages.SetCheck(TRUE);
	else
		m_ctrlProcessImages.SetCheck(FALSE);

	m_nRadioThreadType = AfxGetApp()->GetProfileInt(SECTION_NAME, "m_nRadioThreadType", 0);

	UpdateData(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestMultiAcqisitionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTestMultiAcqisitionDlg::OnPaint()
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
HCURSOR CTestMultiAcqisitionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestMultiAcqisitionDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CTestMultiAcqisitionDlg::OpenFactoryAndCamera()
{
	J_STATUS_TYPE   retval;
	uint32_t        iNumDev;
	bool8_t         bHasChange;

	CWaitCursor cursor;

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
		ShowErrorMsg(CString("Could not get the number of cameras!"), retval);
		return FALSE;
	}
	TRACE("%d cameras were found\n", iNumDev);

	int count = 0;

	for(uint32_t i=0; i<iNumDev; i++)
	{
		CWaitCursor cursor2;

		CCamera* pCamera = new CCamera();
		if(!pCamera)
		{
			return FALSE;
		}

		if(!pCamera->OpenCamera(m_hFactory
								, i
								, this->GetSafeHwnd()
								, m_nRadioThreadType
								)
							)
		{
			delete pCamera;
			continue;
		}

		m_CameraVector.push_back(pCamera);

		m_cameraList.InsertString(count++, pCamera->CameraModelStr); 
	}

	return TRUE;
}

void CTestMultiAcqisitionDlg::CloseFactoryAndCamera()
{
	CWaitCursor cursor;

	J_STATUS_TYPE   retval;

	std::vector<CCamera*>::iterator iter = m_CameraVector.begin();
	for(; iter != m_CameraVector.end(); ++iter)
	{
		CCamera* pCamera = *iter;
		if(pCamera)
		{
			delete pCamera;
		}
	}

	m_CameraVector.clear();

	// Factory open?
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

void CTestMultiAcqisitionDlg::OnBnClickedStart()
{
	CWaitCursor cursor;

	int     bpp = 0;

	UpdateData();

	BOOL bEnableDisplay = m_ctrlEnableDisplay.GetCheck();
	BOOL bHighPriority = m_ctrlHighPriority.GetCheck();
	BOOL bProcessImages = m_ctrlProcessImages.GetCheck();
	BOOL bUseTestPattern = m_ctrlUseTestPattern.GetCheck();

	CCamera::ResetWindowStartPosition();

	std::vector<CCamera*>::iterator iter = m_CameraVector.begin();
	for(size_t i=0; i<m_CameraVector.size(); i++)
	{
		CCamera* pCamera = m_CameraVector[i];
		assert(pCamera);

		if(!pCamera)
		{
			ShowErrorMsg(CString("Could not allocate camera!"), J_ST_INVALID_HANDLE);
			return;
		}

		pCamera->m_bDisableDisplay = !bEnableDisplay;
		pCamera->m_bHighPriority = bHighPriority;
		pCamera->m_bProcessImages = bProcessImages;
		pCamera->m_bUseTestPattern = bUseTestPattern;
		pCamera->m_nThreadType = m_nRadioThreadType;

		std::clog << "Starting camera " << pCamera->CameraModelStr << std::endl;

		if (!pCamera->Start())
		{
			ShowErrorMsg(CString("Could not start camera!"), J_ST_ERR_ABORT);
			//return; //Don't exit this routine here!
		}
		
	}

	CButton* pButtonStart = (CButton*)GetDlgItem(IDC_START);
	if(pButtonStart)
	{
		pButtonStart->EnableWindow(FALSE);
	}

	CButton* pCloseCameras = (CButton*)GetDlgItem(IDC_CLOSE_CAMERA);
	if(pCloseCameras)
	{
		pCloseCameras->EnableWindow(FALSE);
	}

	CButton* pButtonStop = (CButton*)GetDlgItem(IDC_STOP);
	if(pButtonStop)
	{
		pButtonStop->EnableWindow(TRUE);
	}

	m_ctrlEnableDisplay.EnableWindow(FALSE);
	m_ctrlUseTestPattern.EnableWindow(FALSE);
	m_ctrlSearch.EnableWindow(FALSE);
	m_ctrlProcessImages.EnableWindow(FALSE);
	m_ctrlHighPriority.EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_CUSTOM_THREAD)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_CALLBACK)->EnableWindow(FALSE);

	m_nWindowTimer = SetTimer(1, 1000, NULL);
}

void CTestMultiAcqisitionDlg::OnBnClickedStop()
{
	CWaitCursor cursor;

	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

	KillTimer(m_nWindowTimer);

	std::vector<CCamera*>::iterator iter = m_CameraVector.begin();
	for(size_t i=0; i<m_CameraVector.size(); i++)
	{
		CCamera* pCamera = m_CameraVector[i];
		assert(pCamera);

		if(!pCamera)
		{
			ShowErrorMsg(CString("Could not allocate camera!"), J_ST_INVALID_HANDLE);
			continue;
		}

		pCamera->Stop();
	}

	CButton* pButtonStart = (CButton*)GetDlgItem(IDC_START);
	if(pButtonStart)
	{
		pButtonStart->EnableWindow(TRUE);
	}

	CButton* pCloseCameras = (CButton*)GetDlgItem(IDC_CLOSE_CAMERA);
	if(pCloseCameras)
	{
		pCloseCameras->EnableWindow(TRUE);
	}

	CButton* pButtonStop = (CButton*)GetDlgItem(IDC_STOP);
	if(pButtonStop)
	{
		pButtonStop->EnableWindow(FALSE);
	}

	m_ctrlEnableDisplay.EnableWindow(TRUE);
	m_ctrlUseTestPattern.EnableWindow(TRUE);
	m_ctrlSearch.EnableWindow(TRUE);
	m_ctrlProcessImages.EnableWindow(TRUE);
	m_ctrlHighPriority.EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_CUSTOM_THREAD)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_CALLBACK)->EnableWindow(TRUE);
}

void CTestMultiAcqisitionDlg::OnBnClickedOk()
{
	CWaitCursor cursor;

	UpdateData(TRUE);

	// TODO: Add your control notification handler code here
	OnOK();

	// Stop acquisition
	OnBnClickedStop();

	// Close factory & camera
	CloseFactoryAndCamera();

	if(0 == m_ctrlHighPriority.GetCheck())
		AfxGetApp()->WriteProfileInt(SECTION_NAME, "m_ctrlHighPriority", 0);
	else
		AfxGetApp()->WriteProfileInt(SECTION_NAME, "m_ctrlHighPriority", 1);

	if(0 == m_ctrlUseTestPattern.GetCheck())
		AfxGetApp()->WriteProfileInt(SECTION_NAME, "m_ctrlUseTestPattern", 0);
	else
		AfxGetApp()->WriteProfileInt(SECTION_NAME, "m_ctrlUseTestPattern", 1);

	if(0 == m_ctrlEnableDisplay.GetCheck())
		AfxGetApp()->WriteProfileInt(SECTION_NAME, "m_ctrlEnableDisplay", 0);
	else
		AfxGetApp()->WriteProfileInt(SECTION_NAME, "m_ctrlEnableDisplay", 1);

	if(0 == m_ctrlProcessImages.GetCheck())
		AfxGetApp()->WriteProfileInt(SECTION_NAME, "m_ctrlProcessImages", 0);
	else
		AfxGetApp()->WriteProfileInt(SECTION_NAME, "m_ctrlProcessImages", 1);
	
	AfxGetApp()->WriteProfileInt(SECTION_NAME, "m_nRadioThreadType", m_nRadioThreadType);
}

void CTestMultiAcqisitionDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// Stop acquisition
	OnBnClickedStop();

	// Close factory & camera
	CloseFactoryAndCamera();
}

void CTestMultiAcqisitionDlg::OnTimer(UINT_PTR nIDEvent)
{
	CTime timeNow = CTime::GetCurrentTime();

	std::vector<CCamera*>::iterator iter = m_CameraVector.begin();
	std::stringstream str1;
	str1 << timeNow.GetHour() << ":" << timeNow.GetMinute() << ":" << timeNow.GetSecond() << " - " << "Frames: ";

	m_ctrlFrameCount.ResetContent();
	m_ctrlBadFrames.ResetContent();

	std::stringstream strOutput;

	for(int i=0; i<(int)m_CameraVector.size(); i++)
	{
		CCamera* pCamera = m_CameraVector[i];
		assert(pCamera);
		if(pCamera)
		{
			str1 << i+1 << "=>" << pCamera->m_FrameCount << " ";

			strOutput.str( std::string() );
			strOutput.clear();
			strOutput << pCamera->m_FrameCount;
			m_ctrlFrameCount.InsertString(i, strOutput.str().c_str());

			strOutput.str( std::string() );
			strOutput.clear();
			strOutput << pCamera->m_BadFrameCount;
			m_ctrlBadFrames.InsertString(i, strOutput.str().c_str());
		}
	}

	str1 << "\n";
	std::string s = str1.str();
	OutputDebugString(s.c_str());
}

void CTestMultiAcqisitionDlg::OnBnClickedSearch()
{
	CWaitCursor cursor;

	m_ctrlSearch.EnableWindow(FALSE);

	m_cameraList.ResetContent();
	m_ctrlFrameCount.ResetContent();
	m_ctrlBadFrames.ResetContent();

	m_cameraList.UpdateWindow();
	m_ctrlFrameCount.UpdateWindow();
	m_ctrlBadFrames.UpdateWindow();

	// Stop acquisition
	OnBnClickedStop();

	// Close factory & camera
	CloseFactoryAndCamera();

	// Open factory & camera
	BOOL retval = OpenFactoryAndCamera();  

	CButton* pButtonStart = (CButton*)GetDlgItem(IDC_START);
	if(pButtonStart)
	{
		pButtonStart->EnableWindow(TRUE);
	}

	CButton* pCloseCameras = (CButton*)GetDlgItem(IDC_CLOSE_CAMERA);
	if(pCloseCameras)
	{
		pCloseCameras->EnableWindow(TRUE);
	}

	m_ctrlSearch.EnableWindow(TRUE);
}

BOOL CTestMultiAcqisitionDlg::DestroyWindow()
{
	OnBnClickedOk();

	return CDialog::DestroyWindow();
}

void CTestMultiAcqisitionDlg::OnBnClickedCloseCamera()
{
	CButton* pButton = (CButton*)GetDlgItem(IDC_START);
	if(pButton)
	{
		pButton->EnableWindow(FALSE);
	}

	pButton = (CButton*)GetDlgItem(IDC_CLOSE_CAMERA);
	if(pButton)
	{
		pButton->EnableWindow(FALSE);
	}

	CloseFactoryAndCamera();

	m_cameraList.ResetContent();
	m_ctrlFrameCount.ResetContent();
	m_ctrlBadFrames.ResetContent();

	m_cameraList.UpdateWindow();
	m_ctrlFrameCount.UpdateWindow();
	m_ctrlBadFrames.UpdateWindow();
}
