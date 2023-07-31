// SaveAndLoadSettingsSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SaveAndLoadSettingsSample.h"
#include "SaveAndLoadSettingsSampleDlg.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CSaveAndLoadSettingsSampleDlg dialog
CSaveAndLoadSettingsSampleDlg::CSaveAndLoadSettingsSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSaveAndLoadSettingsSampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
	m_bEnableStreaming = false;
}

void CSaveAndLoadSettingsSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SAVE_SETTINGS_COMBO, m_SaveSettingsComboBox);
    DDX_Control(pDX, IDC_LOAD_SETTINGS_COMBO, m_LoadSettingsComboBox);
}

BEGIN_MESSAGE_MAP(CSaveAndLoadSettingsSampleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_SAVE_SETTINGS_BUTTON, &CSaveAndLoadSettingsSampleDlg::OnBnClickedSaveSettingsButton)
    ON_BN_CLICKED(IDC_LOAD_SETTINGS_BUTTON, &CSaveAndLoadSettingsSampleDlg::OnBnClickedLoadSettingsButton)
    ON_WM_DESTROY()
	ON_BN_CLICKED(IDOK, &CSaveAndLoadSettingsSampleDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSaveAndLoadSettingsSampleDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSaveAndLoadSettingsSampleDlg message handlers

BOOL CSaveAndLoadSettingsSampleDlg::OnInitDialog()
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

    // TODO: Add extra initialization here
    BOOL retval;

    m_SaveSettingsComboBox.SetCurSel(0);
    m_LoadSettingsComboBox.SetCurSel(0);
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSaveAndLoadSettingsSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Close factory & camera
    CloseFactoryAndCamera();
}

void CSaveAndLoadSettingsSampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSaveAndLoadSettingsSampleDlg::OnPaint()
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
HCURSOR CSaveAndLoadSettingsSampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSaveAndLoadSettingsSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
{
    J_STATUS_TYPE retsta = J_ST_SUCCESS;
	CString errorMsg;
    errorMsg.Format(_T("%s: Error = %d: "), message, error);
    tGenICamErrorInfo errorInfo;

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
            // Get the GenICam error text
            retsta = J_Factory_GetGenICamErrorInfo(&errorInfo);
            if (retsta == J_ST_SUCCESS)
            {
                errorMsg += "GenICam error: Node='";
                errorMsg += errorInfo.sNodeName;
                errorMsg += "', Error='";
                errorMsg += errorInfo.sDescription;
                errorMsg += "'";
            }
            else
            {
                errorMsg += "GenICam error!";
            }
            break;
		case J_ST_VALIDATION_ERROR:		errorMsg += "Settings File Validation Error ";		break;
		case J_ST_VALIDATION_WARNING:	errorMsg += "Settings File Validation Warning ";    break;
	}
	
	AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);

}

//--------------------------------------------------------------------------------------------------
// OpenFactoryAndCamera
//--------------------------------------------------------------------------------------------------
BOOL CSaveAndLoadSettingsSampleDlg::OpenFactoryAndCamera()
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
void CSaveAndLoadSettingsSampleDlg::CloseFactoryAndCamera()
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

void CSaveAndLoadSettingsSampleDlg::OnBnClickedSaveSettingsButton()
{
    // Prompt the user for a filename for the settings-file
    // szFilters is a text string that includes two file name filters:
    // "*.my" for "MyType Files" and "*.*' for "All Files."
    TCHAR szFilters[]= _T("Settings Files (*.txt)|*.txt|All Files (*.*)|*.*||");

    // Create an Open dialog; the default file name extension is ".txt".
    CFileDialog myFileDialog(FALSE, _T("txt"), _T("settings.txt"), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);

    if (myFileDialog.DoModal() == IDOK)
    {
        J_STATUS_TYPE retval = J_Camera_SaveSettings(m_hCam, myFileDialog.GetFileName(), (J_SAVE_SETTINGS_FLAG)m_SaveSettingsComboBox.GetCurSel());
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not save settings!"), retval);
        }
        else
        {
            AfxMessageBox(_T("Settings Saved OK"), MB_OKCANCEL | MB_ICONINFORMATION);
        }
    }
}

void CSaveAndLoadSettingsSampleDlg::OnBnClickedLoadSettingsButton()
{
    // Prompt the user for a filename for the settings-file
    // szFilters is a text string that includes two file name filters:
    // "*.my" for "MyType Files" and "*.*' for "All Files."
    TCHAR szFilters[]= L"Settings Files (*.txt)|*.txt|All Files (*.*)|*.*||";

    // Create an Open dialog; the default file name extension is ".avi".
    CFileDialog myFileDialog(TRUE, _T("txt"), _T("settings.txt"), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);

    if (myFileDialog.DoModal() == IDOK)
    {
        J_STATUS_TYPE retval = J_Camera_LoadSettings(m_hCam, myFileDialog.GetFileName(), (J_LOAD_SETTINGS_FLAG)m_LoadSettingsComboBox.GetCurSel());
        if (retval != J_ST_SUCCESS)
        {
            if ((J_LOAD_SETTINGS_FLAG)m_LoadSettingsComboBox.GetCurSel() == LOAD_VALIDATE_ONLY)
            {
                if ((retval == J_ST_VALIDATION_ERROR)||(retval == J_ST_VALIDATION_WARNING))
                {
                    uint32_t BufferSize = 0;
                    // First we need to see how big an error string buffer we need so
                    // we set the BufferSize to the value 0. BufferSize will then be 
                    // updated with the actual size required.
                    retval = J_Camera_GetSettingsValidationErrorInfo(m_hCam, NULL, &BufferSize);

                    if (retval == J_ST_SUCCESS)
                    {
                        // Allocate enough room for the info string!
                        int8_t *buffer = (int8_t *)malloc(BufferSize);

                        // And now we get the actual erro information
                        retval = J_Camera_GetSettingsValidationErrorInfo(m_hCam, buffer, &BufferSize);

                        CString message = CString((char*)buffer);
                        AfxMessageBox(message, MB_OKCANCEL | MB_ICONINFORMATION);

                        // Remember to free the buffer again
                        free(buffer);
                    }
                }
                else
                {
                    ShowErrorMsg(CString("Could not validate settings!"), retval);
                }
            }
            else
                ShowErrorMsg(CString("Could not load settings!"), retval);
        }
        else
        {
            if ((J_LOAD_SETTINGS_FLAG)m_LoadSettingsComboBox.GetCurSel() == LOAD_VALIDATE_ONLY)
                AfxMessageBox(_T("Settings Validated OK"), MB_OKCANCEL | MB_ICONINFORMATION);
            else
                AfxMessageBox(_T("Settings Loaded OK"), MB_OKCANCEL | MB_ICONINFORMATION);
        }
    }
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CSaveAndLoadSettingsSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
    GetDlgItem(IDC_SAVE_SETTINGS_BUTTON)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
    GetDlgItem(IDC_LOAD_SETTINGS_BUTTON)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
    GetDlgItem(IDC_SAVE_SETTINGS_COMBO)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
    GetDlgItem(IDC_LOAD_SETTINGS_COMBO)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
}

void CSaveAndLoadSettingsSampleDlg::OnBnClickedOk()
{
	OnOK();
}

void CSaveAndLoadSettingsSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}
