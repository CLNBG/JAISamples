// VCSampleMulticastSlaveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VCSampleMulticastSlave.h"
#include "VCSampleMulticastSlaveDlg.h"
#include <Winsock2.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVCSampleMulticastSlaveDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CVCSampleMulticastSlaveDlg::CVCSampleMulticastSlaveDlg(CWnd* pParent /*=NULL*/)
: CDialog(CVCSampleMulticastSlaveDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;
}

void CVCSampleMulticastSlaveDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVCSampleMulticastSlaveDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CVCSampleMulticastSlaveDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CVCSampleMulticastSlaveDlg::OnBnClickedStop)
    ON_BN_CLICKED(IDCANCEL, &CVCSampleMulticastSlaveDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CVCSampleMulticastSlaveDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CVCSampleMulticastSlaveDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CVCSampleMulticastSlaveDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
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
    InitializeControls();   // Initialize Controls

    return TRUE;  // return TRUE  unless you set the focus to a control
}

//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastSlaveDlg::OnDestroy()
{
    CDialog::OnDestroy();

	OnBnClickedStop();

	CloseFactoryAndCamera();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVCSampleMulticastSlaveDlg::OnPaint()
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
HCURSOR CVCSampleMulticastSlaveDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

//--------------------------------------------------------------------------------------------------
// OpenFactoryAndCamera
//--------------------------------------------------------------------------------------------------
BOOL CVCSampleMulticastSlaveDlg::OpenFactoryAndCamera()
{
    J_STATUS_TYPE   retval;
    uint32_t        iSize;
    uint32_t        iNumDev;
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
    retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not get the number of cameras!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    if (iNumDev == 0)
    {
        AfxMessageBox(CString("There is no camera!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("%d cameras were found\n", iNumDev);

    // Get camera ID
    iSize = (uint32_t)sizeof(m_sCameraId);
    retval = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_sCameraId, &iSize);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not get the camera ID!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("Camera ID: %s\n", m_sCameraId);

    // Open camera with Multicast
    retval = J_Camera_OpenMc(m_hFactory, m_sCameraId, &m_hCam, DEVICE_ACCESS_READONLY, 0);
    if (retval != J_ST_SUCCESS)
    {
        AfxMessageBox(CString("Could not open the camera!"), MB_OK | MB_ICONEXCLAMATION);
        return FALSE;
    }
    TRACE("Opening camera succeeded\n");

	//This sample is appropriate only for GigE cameras.
	int8_t buffer[J_CAMERA_ID_SIZE] = {0};
	uint32_t mySize = J_CAMERA_ID_SIZE;
	retval = J_Camera_GetTransportLayerName(m_hCam, buffer, &mySize);
	if (retval != J_ST_SUCCESS)
	{
		AfxMessageBox(CString("Error calling J_Camera_GetTransportLayerName."), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	//Convert to upper case
	std::string strTransportName((char*)buffer);
	std::transform(strTransportName.begin(), strTransportName.end(), strTransportName.begin(), (int( *)(int)) toupper);

	if(std::string::npos == strTransportName.find("GEV") && std::string::npos == strTransportName.find("GIGEVISION"))
	{
		int32_t ret = AfxMessageBox(CString("This sample only works with GigE cameras.\n\nExit application?"), MB_YESNO | MB_ICONSTOP);
		if(IDYES == ret)
		{
			OnOK();
			return FALSE;
		}
	}

    return TRUE;
}
//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastSlaveDlg::CloseFactoryAndCamera()
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
void CVCSampleMulticastSlaveDlg::OnBnClickedStart()
{
    J_STATUS_TYPE   retval;
    int64_t int64Val;
    int64_t pixelFormat;

    SIZE	ViewSize;
    POINT	TopLeft;

    // Get updated values from camera in case the Width and Height values had been changed from the Master 
    // This is done by invalidating the node values
    retval = J_Camera_InvalidateNodes(m_hCam);

    // Update the UI
    InitializeControls();

    // Get Width from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

    // Get Height from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

    // Get pixelformat from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &int64Val);
    pixelFormat = int64Val;

    // Calculate number of bits (not bytes) per pixel using macro
    int bpp = J_BitsPerPixel(pixelFormat);

    // Set window position
    TopLeft.x = 100;
    TopLeft.y = 50;

    // Open view window
    retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView);
    if (retval != J_ST_SUCCESS) {
        AfxMessageBox(CString("Could not open view window!"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Open stream
    retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CVCSampleMulticastSlaveDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
    if (retval != J_ST_SUCCESS) {
        AfxMessageBox(CString("Could not open stream!"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    TRACE("Opening stream succeeded\n");

    EnableControls(TRUE, TRUE);
}

//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastSlaveDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
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
void CVCSampleMulticastSlaveDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

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

    EnableControls(TRUE, FALSE);
}

//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastSlaveDlg::InitializeControls()
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


    //- IP Address -----------------------------------------
    // SCDA0: 0x0d18 First stream channel destination address
    uint32_t size;
    DWORD data;

    size = 4;
    retval = J_Camera_ReadData(m_hCam, REG_ADDR_SCDA0, &data, &size);
    if (retval == J_ST_SUCCESS)
    {
        data = ntohl(data);
    }
    else
    {
        data = 0;
    }
    CIPAddressCtrl *ipc = (CIPAddressCtrl *)GetDlgItem(IDC_IP_ADDRESS);
    ipc->SetAddress(data);

}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastSlaveDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
    GetDlgItem(IDC_START)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_STOP)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedCancel
//--------------------------------------------------------------------------------------------------
void CVCSampleMulticastSlaveDlg::OnBnClickedCancel()
{
    OnCancel();
}

void CVCSampleMulticastSlaveDlg::OnBnClickedOk()
{
	OnOK();
}
