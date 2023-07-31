// VCSampleMulticastMasterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VCSampleMulticastMaster.h"
#include "VCSampleMulticastMasterDlg.h"
#include <Winsock2.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CVCSampleMulticastMasterDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CVCSampleMulticastMasterDlg::CVCSampleMulticastMasterDlg(CWnd* pParent /*=NULL*/)
: CDialog(CVCSampleMulticastMasterDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;

    m_dwMulticastIP = DEFAULT_IPADDR;
}

void CVCSampleMulticastMasterDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVCSampleMulticastMasterDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CVCSampleMulticastMasterDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CVCSampleMulticastMasterDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_OPENCAMERA, &CVCSampleMulticastMasterDlg::OnBnClickedOpencamera)
    ON_BN_CLICKED(IDC_CLOSECAMERA, &CVCSampleMulticastMasterDlg::OnBnClickedClosecamera)
	ON_BN_CLICKED(IDCANCEL, &CVCSampleMulticastMasterDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CVCSampleMulticastMasterDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CVCSampleMulticastMasterDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CVCSampleMulticastMasterDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
    BOOL retval;

    // Open factory & camera
    retval = OpenFactory();  
    if (retval)
    {
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString((char*)m_sCameraId));    // Display camera ID
        GetDlgItem(IDC_OPENCAMERA)->EnableWindow(TRUE);
    }
    else
    {
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString("error"));
		return FALSE;
    }
    InitializeControls();   // Initialize Controls

    // Display IP Address
    CIPAddressCtrl *ipc = (CIPAddressCtrl *)GetDlgItem(IDC_IP_ADDRESS);
    ipc->SetAddress(m_dwMulticastIP);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::OnDestroy()
{
    CDialog::OnDestroy();

	OnBnClickedStop();

    CloseFactoryAndCamera();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVCSampleMulticastMasterDlg::OnPaint()
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
HCURSOR CVCSampleMulticastMasterDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

//--------------------------------------------------------------------------------------------------
// OpenFactory
//--------------------------------------------------------------------------------------------------
BOOL CVCSampleMulticastMasterDlg::OpenFactory()
{
    J_STATUS_TYPE   retval;
    uint32_t          iSize;
    // 	uint32_t        iNumDev;
    bool8_t         bHasChange;

    // Open factory
    retval = J_Factory_Open((int8_t*)"", &m_hFactory);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not open factory!"));
        return FALSE;
    }
    TRACE("Opening factory succeeded\n");

    // Update camera list
    retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not update camera list!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("Updating camera list succeeded\n");

    // Get the number of Cameras
    retval = J_Factory_GetNumOfCameras(m_hFactory, &m_nCameras);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not get the number of cameras!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (m_nCameras == 0)
    {
        AfxMessageBox(CString("There is no camera!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("%d cameras were found\n", m_nCameras);

    // Get camera ID
    iSize = (uint32_t)sizeof(m_sCameraId);
    retval = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_sCameraId, &iSize);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not get the camera ID!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("Camera ID: %s\n", m_sCameraId);

	if (NULL == strstr((char*)m_sCameraId, "INT=>FD") && NULL == strstr((char*)m_sCameraId, "INT=>SD"))
	{
		int32_t ret = AfxMessageBox(CString("This sample only works with GigE cameras.\n\nExit application?"), MB_YESNO | MB_ICONSTOP);
		if(IDYES == ret)
		{
			OnOK();
		}
		return FALSE;
	}

    return TRUE;
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedOpencamera
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::OnBnClickedOpencamera()
{
    if (m_nCameras < 1)   return;

    BeginWaitCursor();

    // Get IP Address
    CIPAddressCtrl *ipc = (CIPAddressCtrl *)GetDlgItem(IDC_IP_ADDRESS);
    ipc->GetAddress(m_dwMulticastIP);

    J_STATUS_TYPE   retval;

    // Open camera with Multicast
    retval = J_Camera_OpenMc(m_hFactory, m_sCameraId, &m_hCam, DEVICE_ACCESS_CONTROL, m_dwMulticastIP);
    if (retval != J_ST_SUCCESS)
    {
        EndWaitCursor();
        AfxMessageBox(CString("Could not open the camera!"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    TRACE("Opening camera succeeded\n");

    GetDlgItem(IDC_OPENCAMERA)->EnableWindow(FALSE);
    GetDlgItem(IDC_CLOSECAMERA)->EnableWindow(TRUE);
    GetDlgItem(IDC_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_IP_ADDRESS)->EnableWindow(FALSE);

    InitializeControls();   // Initialize Controls

    EndWaitCursor();
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedClosecamera
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::OnBnClickedClosecamera()
{
    BeginWaitCursor();

    if (m_hCam)
    {
        // Close camera
        J_Camera_Close(m_hCam);
        m_hCam = NULL;
        TRACE("Closed camera\n");
    }

    GetDlgItem(IDC_OPENCAMERA)->EnableWindow(TRUE);
    GetDlgItem(IDC_CLOSECAMERA)->EnableWindow(FALSE);
    GetDlgItem(IDC_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_IP_ADDRESS)->EnableWindow(TRUE);

    InitializeControls();   // Initialize Controls

    EndWaitCursor();
}
//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::CloseFactoryAndCamera()
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
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::OnBnClickedStart()
{
    J_STATUS_TYPE   retval;
    int64_t int64Val;
    int64_t pixelFormat;

    SIZE	ViewSize;
    POINT	TopLeft;

    CButton* pCheckBox = (CButton*)GetDlgItem(IDC_VIDEO_CHECK);
    bool bDisplayVideo = (pCheckBox->GetCheck()!=0);
    pCheckBox->EnableWindow(FALSE);

    if (bDisplayVideo)
    {
        // Get Width from the camera
        retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
        ViewSize.cx = (LONG)int64Val;     // Set window size cx

        // Get Height from the camera
        retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
        ViewSize.cy = (LONG)int64Val;     // Set window size cy

        // Set window position
        TopLeft.x = 100;
        TopLeft.y = 50;

        // Get pixelformat from the camera
        retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &int64Val);
        pixelFormat = int64Val;

        // Calculate number of bits (not bytes) per pixel using macro
        int bpp = J_BitsPerPixel(pixelFormat);

        // Open view window
        retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView);
        if (retval != J_ST_SUCCESS) {
            AfxMessageBox(CString("Could not open view window!"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        TRACE("Opening view window succeeded\n");

        // Open stream
        retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CVCSampleMulticastMasterDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8, m_dwMulticastIP);
        if (retval != J_ST_SUCCESS) {
            AfxMessageBox(CString("Could not open stream!"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        TRACE("Opening stream succeeded\n");
    }
    else
    {
        // Set the Stream Channel Destination IP address to be the Multicast IP address
        uint32_t iData = ntohl(m_dwMulticastIP);
        uint32_t iSize = sizeof(iData);
        retval = J_Camera_WriteData(m_hCam, 0x0D18, &iData, &iSize);
        if (retval != J_ST_SUCCESS) {
            AfxMessageBox(CString("Could not set Stream Channel Destination IP Address manually!"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        TRACE("Setting the Stream Channel Destination IP Address manually succeeded\n");

        // Set the Stream Channel Destination Port Address to something!
        iData = ntohl(0x1234);
        retval = J_Camera_WriteData(m_hCam, 0x0D00, &iData, &iSize);
        if (retval != J_ST_SUCCESS) {
            AfxMessageBox(CString("Could not set Stream Channel Destination Port manually!"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        TRACE("Setting the Stream Channel Destination Port manually succeeded\n");
    }
    // Start Acquision
    retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);

    GetDlgItem(IDC_CLOSECAMERA)->EnableWindow(FALSE);
    GetDlgItem(IDC_START)->EnableWindow(FALSE);
    GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
    if(m_hView)
    {
        // Shows image
        J_Image_ShowImage(m_hView, pAqImageInfo);
    }
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

    // Stop Acquistion
    if (NULL == m_hCam) 
	{
		return;
    }

	retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);

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

    CButton* pCheckBox = (CButton*)GetDlgItem(IDC_VIDEO_CHECK);
    bool bDisplayVideo = (pCheckBox->GetCheck()!=0);
    pCheckBox->EnableWindow(TRUE);

    if (!bDisplayVideo)
    {
        uint32_t iData = 0x0000;
        uint32_t iSize = sizeof(iData);
        // Set the Stream Channel Destination Port Address to 0 to stop the streaming!
        retval = J_Camera_WriteData(m_hCam, 0x0D00, &iData, &iSize);
        if (retval != J_ST_SUCCESS) {
            AfxMessageBox(CString("Could not set Stream Channel Destination Port manually to 0!"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        TRACE("Setting the Stream Channel Destination Port manually to 0 succeeded\n");

        // Set the Stream Channel Destination IP address to 0 as well
        retval = J_Camera_WriteData(m_hCam, 0x0D18, &iData, &iSize);

        if (retval != J_ST_SUCCESS) {
            AfxMessageBox(CString("Could not set Stream Channel Destination IP Address manually to 0!"), MB_OK | MB_ICONEXCLAMATION);
            return;
        }
        TRACE("Setting the Stream Channel Destination IP Address manually to 0 succeeded\n");
    }

    GetDlgItem(IDC_CLOSECAMERA)->EnableWindow(TRUE);
    GetDlgItem(IDC_START)->EnableWindow(TRUE);
    GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
}

//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::InitializeControls()
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
        pSCtrl->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_LBL_WIDTH)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_WIDTH)->ShowWindow(SW_SHOW);

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
        pSCtrl->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_LBL_HEIGHT)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_HEIGHT)->ShowWindow(SW_SHOW);

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

    //- Gain -----------------------------------------------

    // Get SliderCtrl for Gain
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);

    // Get Gain Node
    retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_GAIN, &hNode);
    if (retval == J_ST_SUCCESS)
    {
        pSCtrl->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_LBL_GAIN)->ShowWindow(SW_SHOW);
        GetDlgItem(IDC_GAIN)->ShowWindow(SW_SHOW);

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
}
//--------------------------------------------------------------------------------------------------
// OnHScroll
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

//--------------------------------------------------------------------------------------------------
// OnBnClickedCancel
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastMasterDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CVCSampleMulticastMasterDlg::OnBnClickedOk()
{
	OnOK();
}

