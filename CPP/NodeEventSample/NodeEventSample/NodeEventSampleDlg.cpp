// NodeEventSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NodeEventSample.h"
#include "NodeEventSampleDlg.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HWND hWnd = NULL;   // Global copy of the Windows Handle for the main window

//--------------------------------------------------------------------------------------------------
// Callback to be called every time the PayloadSize node changes value/status
//--------------------------------------------------------------------------------------------------
void __stdcall PayloadSizeChangeCallback(NODE_HANDLE hNode)
{
    J_STATUS_TYPE   retval;
    int64_t int64Val;

    // Get new Value
    retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);

    // Update the UI
    SetDlgItemInt(hWnd, IDC_PAYLOADSIZE, (int)int64Val, TRUE);
}

// CNodeEventSampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CNodeEventSampleDlg::CNodeEventSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CNodeEventSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
	m_bEnableStreaming = false;
}

void CNodeEventSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PAYLOADSIZE, m_PayloadSizeCtrl);
}

BEGIN_MESSAGE_MAP(CNodeEventSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDCANCEL, &CNodeEventSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDCANCEL, &CNodeEventSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CNodeEventSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CNodeEventSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CNodeEventSampleDlg::OnInitDialog()
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

    hWnd = m_hWnd;

    InitializeControls();   // Initialize Controls

    return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CNodeEventSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Close factory & camera
    CloseFactoryAndCamera();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CNodeEventSampleDlg::OnPaint()
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
HCURSOR CNodeEventSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CNodeEventSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CNodeEventSampleDlg::OpenFactoryAndCamera()
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
void CNodeEventSampleDlg::CloseFactoryAndCamera()
{
    if (m_hCam)
    {
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
}

//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CNodeEventSampleDlg::InitializeControls()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    int64_t int64Val;

    CSliderCtrl* pSCtrl;

    //- Width ------------------------------------------------

    // Get SliderCtrl for Width
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WIDTH);

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

    // Get Height Node
    retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_HEIGHT, &hNode);
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

        SetDlgItemInt(IDC_HEIGHT, (int)int64Val);
    }
    else
    {
        pSCtrl->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_LBL_HEIGHT)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_HEIGHT)->ShowWindow(SW_HIDE);
    }

    //- Payload Size -----------------------------------------

    // Get PayloadSize Node
    retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_PAYLOADSIZE, &hNode);
    if (retval == J_ST_SUCCESS)
    {
        // Get/Set Value
        retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);

        SetDlgItemInt(IDC_PAYLOADSIZE, (int)int64Val);

        // Here we want to assign a callback event to any value or state change of the PayloadSize node
        // This is done using the J_Node_RegisterCallback()
        retval = J_Node_RegisterCallback(hNode, reinterpret_cast<J_NODE_CALLBACK_FUNCTION>(PayloadSizeChangeCallback));
    }
    else
    {
        GetDlgItem(IDC_LBL_PAYLOADSIZE)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_PAYLOADSIZE)->ShowWindow(SW_HIDE);
    }
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CNodeEventSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
    GetDlgItem(IDC_SLIDER_WIDTH)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_WIDTH)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_HEIGHT)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_HEIGHT)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_PAYLOADSIZE)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnHScroll
//--------------------------------------------------------------------------------------------------
void CNodeEventSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
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
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CNodeEventSampleDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    OnCancel();
}

void CNodeEventSampleDlg::OnBnClickedOk()
{
	OnOK();
}
