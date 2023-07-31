// ActionCommandSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ActionCommandSample.h"
#include "ActionCommandSampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CActionCommandSampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CActionCommandSampleDlg::CActionCommandSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CActionCommandSampleDlg::IDD, pParent)
, m_AckCount(0)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;

    for (int i=0; i< MAX_CAMERAS; i++)
    {
        m_hCam[i] = NULL;
        m_hView[i] = NULL;
        m_hThread[i] = NULL;
    }

    m_bReqAck = true;
    m_CameraCount = 0;
}

void CActionCommandSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATUS_STATIC, m_statusbarStatic);
    DDX_Control(pDX, IDC_REQ_ACK_CHECK, m_reqAckCheck);
    DDX_Control(pDX, IDC_TIMESTAMP1_STATIC, m_TimestampStatic1);
    DDX_Control(pDX, IDC_TIMESTAMP2_STATIC, m_TimestampStatic2);
    DDX_Control(pDX, IDC_ACK_COUNT_SPIN, m_AckCountSpin);
    DDX_Control(pDX, IDC_ACK_COUNT_EDIT, m_AckCountEdit);
}

BEGIN_MESSAGE_MAP(CActionCommandSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CActionCommandSampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CActionCommandSampleDlg::OnBnClickedStop)
    ON_BN_CLICKED(IDC_ACTION1_BUTTON, &CActionCommandSampleDlg::OnBnClickedAction1Button)
    ON_BN_CLICKED(IDC_ACTION2_BUTTON, &CActionCommandSampleDlg::OnBnClickedAction2Button)
    ON_BN_CLICKED(IDC_REQ_ACK_CHECK, &CActionCommandSampleDlg::OnBnClickedReqAckCheck)
	ON_BN_CLICKED(IDCANCEL, &CActionCommandSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CActionCommandSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CActionCommandSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CActionCommandSampleDlg::OnInitDialog()
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
        if (m_hCam[0])
            GetDlgItem(IDC_CAMERAID1)->SetWindowText(CString((char*)m_sCameraId[0]));    // Display camera ID
        else
            GetDlgItem(IDC_CAMERAID2)->SetWindowText(_T("No camera 1 found"));

        if (m_hCam[1])
            GetDlgItem(IDC_CAMERAID2)->SetWindowText(CString((char*)m_sCameraId[1]));    // Display camera ID
        else
            GetDlgItem(IDC_CAMERAID2)->SetWindowText(_T("No camera 2 found"));
        EnableControls(TRUE, FALSE);   // Enable Controls

		if (m_bReqAck)
			m_reqAckCheck.SetCheck(1);
		else
			m_reqAckCheck.SetCheck(0);

		// Try to initialize the Actions for the connected cameras
		if (!SetupActions())
		{
			AfxMessageBox(CString("Unable to set-up the actions for one or more of the connected cameras!\nPlease check that the connected cameras are GigE Vision 1.1 devices and that they implement Actions!"), MB_OK | MB_ICONEXCLAMATION);
			EnableControls(FALSE, FALSE);  // Disable Controls
		}

		m_AckCountEdit.SetWindowText(_T("2"));
		m_AckCountSpin.SetPos(m_CameraCount);
		m_AckCountSpin.SetRange32(0,100);
		m_AckCount = m_CameraCount;

		UpdateData(FALSE);
    }
    else
    {
        GetDlgItem(IDC_CAMERAID1)->SetWindowText(CString("Error"));
        GetDlgItem(IDC_CAMERAID2)->SetWindowText(CString("Error"));
        EnableControls(FALSE, FALSE);  // Disable Controls
		return FALSE;
    }

    return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CActionCommandSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Stop acquisition
    OnBnClickedStop();

    // Set the camera back to continuous mode
    SetupContinuous();

    // Close factory & camera
    CloseFactoryAndCamera();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CActionCommandSampleDlg::OnPaint()
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
HCURSOR CActionCommandSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}
//--------------------------------------------------------------------------------------------------
// OpenFactoryAndCamera
//--------------------------------------------------------------------------------------------------
BOOL CActionCommandSampleDlg::OpenFactoryAndCamera()
{
    J_STATUS_TYPE   retval;
    uint32_t        iSize;
    uint32_t        iNumDev;
    bool8_t         bHasChange;

    // Open factory
    retval = J_Factory_Open((int8_t*)"" , &m_hFactory);
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

    // We only want to get MAX_CAMERAS cameras connected at a time
    // and we assume that iNumDev is the actual camera count*2 because we assume
    // that we have 2 drivers active (SockerDriver+FilerDriver)

	bool bFdUse = false;
	int	iValidCamera = 0;

    for (int i=0; i< (int)iNumDev; i++)
    {
        // Get camera IDs
        iSize = J_CAMERA_ID_SIZE;
		m_sCameraId[iValidCamera][0] = 0;
        retval = J_Factory_GetCameraIDByIndex(m_hFactory, i, m_sCameraId[iValidCamera], &iSize);
        if (retval != J_ST_SUCCESS)
        {
            AfxMessageBox(CString("Could not get the camera ID!"), MB_OK | MB_ICONEXCLAMATION);
            return FALSE;
        }
        TRACE("Camera ID[%d]: %s\n", i, m_sCameraId[iValidCamera]);

		if(0 == strncmp("TL=>GevTL , INT=>FD", (char*)m_sCameraId[iValidCamera], 19))
		{ // FD
			bFdUse = true;
			// Open camera
			retval = J_Camera_Open(m_hFactory, m_sCameraId[iValidCamera], &m_hCam[iValidCamera]);
			if (retval != J_ST_SUCCESS)
			{
				AfxMessageBox(CString("Could not open the camera!"), MB_OK | MB_ICONEXCLAMATION);
				return FALSE;
			}
			iValidCamera++;
			TRACE("Opening camera %d succeeded\n", iValidCamera);
		}
		else
		{ // SD
			if(bFdUse == false)
			{
				// Open camera
				retval = J_Camera_Open(m_hFactory, m_sCameraId[iValidCamera], &m_hCam[iValidCamera]);
				if (retval != J_ST_SUCCESS)
				{
					AfxMessageBox(CString("Could not open the camera!"), MB_OK | MB_ICONEXCLAMATION);
					return FALSE;
				}
				iValidCamera++;
				TRACE("Opening camera %d succeeded\n", iValidCamera);
			}
		}
		if(iValidCamera >= MAX_CAMERAS)
			break;
    }
    m_CameraCount = min(iValidCamera, MAX_CAMERAS);

	//This sample is appropriate only for GigE cameras.
	for(int i=0; i<m_CameraCount; i++)
	{
		int8_t buffer[J_CAMERA_ID_SIZE] = {0};
		uint32_t mySize = J_CAMERA_ID_SIZE;
		retval = J_Camera_GetTransportLayerName(m_hCam[i], buffer, &mySize);
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

	}//for(int i=0; i<m_CameraCount; i++)

	return TRUE;
}
//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CActionCommandSampleDlg::CloseFactoryAndCamera()
{
    for (int i=0; i<MAX_CAMERAS;i++)
    {
        if (m_hCam[i])
        {
			      J_Camera_SetValueString(m_hCam[i], (int8_t*)"TriggerMode", (int8_t*)"Off");

            // Close camera
            J_Camera_Close(m_hCam[i]);
            m_hCam[i] = NULL;
            TRACE("Closed camera %d\n", i);
        }
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
void CActionCommandSampleDlg::OnBnClickedStart()
{
    J_STATUS_TYPE   retval;
    int64_t int64Val;
    int64_t pixelFormat;
    int     bpp = 0;
    SIZE	ViewSize;
    POINT	TopLeft;

    for (int i=0; i<MAX_CAMERAS;i++)
    {
        if (m_hCam[i])
        {
            // Get Width from the camera
            retval = J_Camera_GetValueInt64(m_hCam[i], NODE_NAME_WIDTH, &int64Val);
            ViewSize.cx = (LONG)int64Val;     // Set window size cx

            // Get Height from the camera
            retval = J_Camera_GetValueInt64(m_hCam[i], NODE_NAME_HEIGHT, &int64Val);
            ViewSize.cy = (LONG)int64Val;     // Set window size cy

            // Set window position
            TopLeft.x = 10;
            TopLeft.y = 10+(i*(ViewSize.cy+25));

            // Get pixelformat from the camera
            retval = J_Camera_GetValueInt64(m_hCam[i], NODE_NAME_PIXELFORMAT, &int64Val);
            pixelFormat = int64Val;

            // Calculate number of bits (not bytes) per pixel using macro
            bpp = J_BitsPerPixel(pixelFormat);

            // Open view window
            retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView[i]);
            if (retval != J_ST_SUCCESS) {
                AfxMessageBox(CString("Could not open view window!"), MB_OK | MB_ICONEXCLAMATION);
                break;
            }
            TRACE("Opening view window succeeded\n");

            // Open stream
            if (i==0)
            {
                retval = J_Image_OpenStream(m_hCam[i], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CActionCommandSampleDlg::StreamCBFunc1), &m_hThread[i], (ViewSize.cx*ViewSize.cy*bpp)/8);
                if (retval != J_ST_SUCCESS) {
                    AfxMessageBox(CString("Could not open stream!"), MB_OK | MB_ICONEXCLAMATION);
                    break;
                }
                TRACE("Opening stream succeeded\n");
            }
            else if (i==1)
            {
                retval = J_Image_OpenStream(m_hCam[i], 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CActionCommandSampleDlg::StreamCBFunc2), &m_hThread[i], (ViewSize.cx*ViewSize.cy*bpp)/8);
                if (retval != J_ST_SUCCESS) {
                    AfxMessageBox(CString("Could not open stream!"), MB_OK | MB_ICONEXCLAMATION);
                    break;
                }
                TRACE("Opening stream succeeded\n");
            }
            // Start Acquision
            retval = J_Camera_ExecuteCommand(m_hCam[i], NODE_NAME_ACQSTART);
        }
    }

    EnableControls(TRUE, TRUE);
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CActionCommandSampleDlg::StreamCBFunc1(J_tIMAGE_INFO * pAqImageInfo)
{
    // Update timestamp
    CString timestamp;
    timestamp.Format(_T("%016llx"), pAqImageInfo->iTimeStamp);
    m_TimestampStatic1.SetWindowText(timestamp);

    if(m_hView[0])
    {
        // Shows image
        J_Image_ShowImage(m_hView[0], pAqImageInfo);
    }
}

//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CActionCommandSampleDlg::StreamCBFunc2(J_tIMAGE_INFO * pAqImageInfo)
{
    // Update timestamp
    CString timestamp;
    timestamp.Format(_T("%016llx"), pAqImageInfo->iTimeStamp);
    m_TimestampStatic2.SetWindowText(timestamp);

    if(m_hView[1])
    {
        // Shows image
        J_Image_ShowImage(m_hView[1], pAqImageInfo);
    }
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CActionCommandSampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

    for (int i=0; i<MAX_CAMERAS;i++)
    {
        if (m_hCam[i])
        {
            // Stop Acquision
            if (m_hCam[i]) {
                retval = J_Camera_ExecuteCommand(m_hCam[i], NODE_NAME_ACQSTOP);
            }

            if(m_hThread[i])
            {
                // Close stream
                retval = J_Image_CloseStream(m_hThread[i]);
                m_hThread[i] = NULL;
                TRACE("Closed stream\n");
            }

            if(m_hView[i])
            {
                // Close view window
                retval = J_Image_CloseViewWindow(m_hView[i]);
                m_hView[i] = NULL;
                TRACE("Closed view window\n");
            }
        }
    }
    EnableControls(TRUE, FALSE);
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CActionCommandSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
    GetDlgItem(IDC_START)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_STOP)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
}

bool CActionCommandSampleDlg::SetupActions()
{
    J_STATUS_TYPE	status = J_ST_SUCCESS;

    for (int i=0; i<MAX_CAMERAS;i++)
    {
        if (m_hCam[i])
        {
            // ActionDeviceKey = DEVICE_KEY
            if(J_ST_SUCCESS	!= J_Camera_SetValueInt64(m_hCam[i], (int8_t*)"ActionDeviceKey", DEVICE_KEY))
            {
                AfxMessageBox(_T("Failed to set the value to ActionDeviceKey!"), MB_OK | MB_ICONINFORMATION);
                return false;
            }

            ///////////////////////////////////////////////////////////////////////////////
            // ActionSelector = 1
            if(J_ST_SUCCESS != J_Camera_SetValueInt64(m_hCam[i], (int8_t*)"ActionSelector", 1))
            {
                AfxMessageBox(_T("Failed to set the value to ActionSelector!"), MB_OK | MB_ICONINFORMATION);
                return false;
            }

            // ActionGroupKey = GROUP_KEY1
            if(J_ST_SUCCESS != J_Camera_SetValueInt64(m_hCam[i], (int8_t*)"ActionGroupKey", GROUP_KEY1))
            {
                AfxMessageBox(_T("Failed to set the value to ActionGroupKey!"), MB_OK | MB_ICONINFORMATION);
                return false;
            }

            // ActionGroupMask = GROUP_MASK
            if(J_ST_SUCCESS != J_Camera_SetValueInt64(m_hCam[i], (int8_t*)"ActionGroupMask", GROUP_MASK1))
            {
                AfxMessageBox(_T("Failed to set the value to ActionGroupMask!"), MB_OK | MB_ICONINFORMATION);
                return false;
            }

            ///////////////////////////////////////////////////////////////////////////////
            // Settings of Trigger Action

            // We have two possible ways of setting up triggers: JAI or GenICam SFNC
            // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
            // called "TriggerSelector". Therefor we have to determine which way to use here.
            // First we see if a node called "TriggerSelector" exists.
            NODE_HANDLE hNode = NULL;
            status = J_Camera_GetNodeByName(m_hCam[i], (int8_t*)"TriggerSelector", &hNode);

            // Does the "TriggerSelector" node exist?
            if ((status == J_ST_SUCCESS) && (hNode != NULL))
            {
                // Here we assume that this is GenICam SFNC trigger so we do the following:
                // TriggerSelector=FrameStart
                // TriggerMode=On
                // TriggerSource=Action1
                if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"TriggerSelector", (int8_t*)"FrameStart"))
                {
                    AfxMessageBox(_T("Failed to set the value to TriggerSelector!"), MB_OK | MB_ICONINFORMATION);
                    return false;
                }

                if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"TriggerMode", (int8_t*)"On"))
                {
                    AfxMessageBox(_T("Failed to set the value to TriggerMode!"), MB_OK | MB_ICONINFORMATION);
                    return false;
                }

                if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"TriggerSource", (int8_t*)"Action1"))
                {
                    AfxMessageBox(_T("Failed to set the value to TriggerSource!"), MB_OK | MB_ICONINFORMATION);
                    return false;
                }
            }
            else
            {
                // Here we assume that this is JAI trigger so we do the following:
                // ExposureMode=EdgePreSelect
                // LineSelector=CameraTrigger0
                // LineSource=Action1
                // LineInverter=ActiveHigh
                if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"ExposureMode", (int8_t*)"EdgePreSelect"))
                {
                    AfxMessageBox(_T("Failed to set the value to ExposureMode!"), MB_OK | MB_ICONINFORMATION);
                    return false;
                }

                if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"LineSelector", (int8_t*)"CameraTrigger0"))
                {
                    AfxMessageBox(_T("Failed to set the value to LineSelector!"), MB_OK | MB_ICONINFORMATION);
                    return false;
                }

                if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"LineSource", (int8_t*)"Action1"))
                {
                    AfxMessageBox(_T("Failed to set the value to LineSource!"), MB_OK | MB_ICONINFORMATION);
                    return false;
                }
            }

            ///////////////////////////////////////////////////////////////////////////////

            ///////////////////////////////////////////////////////////////////////////////
            // ActionSelector = 2
            if(J_ST_SUCCESS != J_Camera_SetValueInt64(m_hCam[i], (int8_t*)"ActionSelector", 2))
            {
                AfxMessageBox(_T("Failed to set the value to ActionSelector!"), MB_OK | MB_ICONINFORMATION);
                return false;
            }

            // ActionGroupKey = GROUP_KEY2
            if(J_ST_SUCCESS != J_Camera_SetValueInt64(m_hCam[i], (int8_t*)"ActionGroupKey", GROUP_KEY2))
            {
                AfxMessageBox(_T("Failed to set the value to ActionGroupKey!"), MB_OK | MB_ICONINFORMATION);
                return false;
            }

            // ActionGroupMask = GROUP_MASK
            if(J_ST_SUCCESS != J_Camera_SetValueInt64(m_hCam[i], (int8_t*)"ActionGroupMask", GROUP_MASK2))
            {
                AfxMessageBox(_T("Failed to set the value to ActionGroupMask!"), MB_OK | MB_ICONINFORMATION);
                return false;
            }

            ///////////////////////////////////////////////////////////////////////////////
            if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"LineSelector", (int8_t*)"TimeStampReset"))
            {
                AfxMessageBox(_T("Failed to set the value to LineSelector!"), MB_OK | MB_ICONINFORMATION);
                return false;
            }

            if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"LineSource", (int8_t*)"Action2"))
            {
                AfxMessageBox(_T("Failed to set the value to LineSource!"), MB_OK | MB_ICONINFORMATION);
                return false;
            }
        }
    }

    return true;
}

bool CActionCommandSampleDlg::SetupContinuous()
{
    J_STATUS_TYPE	status = J_ST_SUCCESS;

    for (int i=0; i<MAX_CAMERAS;i++)
    {
        if (m_hCam[i])
        {
            // We have two possible ways of setting up triggers: JAI or GenICam SFNC
            // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
            // called "TriggerSelector". Therefor we have to determine which way to use here.
            // First we see if a node called "TriggerSelector" exists.
            NODE_HANDLE hNode = NULL;
            status = J_Camera_GetNodeByName(m_hCam[i], (int8_t*)"TriggerSelector", &hNode);

            // Does the "TriggerSelector" node exist?
            if ((status == J_ST_SUCCESS) && (hNode != NULL))
            {
                // Here we assume that this is GenICam SFNC trigger so we do the following:
                // TriggerSelector=FrameStart
                // TriggerMode=Off
                if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"TriggerSelector", (int8_t*)"FrameStart"))
                {
                    AfxMessageBox(_T("Failed to set the value to ExposureMode!"), MB_OK | MB_ICONINFORMATION);
                    return false;
                }

                if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"TriggerMode", (int8_t*)"Off"))
                {
                    AfxMessageBox(_T("Failed to set the value to ExposureMode!"), MB_OK | MB_ICONINFORMATION);
                    return false;
                }
            }
            else
            {
                // Here we assume that this is JAI trigger so we do the following:
                // ExposureMode=Continuous
                if(J_ST_SUCCESS != J_Camera_SetValueString(m_hCam[i], (int8_t*)"ExposureMode", (int8_t*)"Continuous"))
                {
                    AfxMessageBox(_T("Failed to set the value to ExposureMode!"), MB_OK | MB_ICONINFORMATION);
                    return false;
                }
            }
        }
    }
    return true;
}

void CActionCommandSampleDlg::OnBnClickedAction1Button()
{
    uint32_t	iNumOfAck;
    J_tACTION_CMD_ACK		*pAckBuffer = NULL;

    m_AckCount = m_AckCountSpin.GetPos();

    m_statusbarStatic.SetWindowText(_T(" "));

    if(m_bReqAck && (m_AckCount!=0))
    {
        iNumOfAck = (uint32_t)m_AckCount;
        pAckBuffer = new J_tACTION_CMD_ACK[iNumOfAck];
    }

    if(J_ST_SUCCESS != J_Factory_SendActionCommand(m_hFactory, DEVICE_KEY, GROUP_KEY1, GROUP_MASK, pAckBuffer, &iNumOfAck))
        m_statusbarStatic.SetWindowText(_T("Failed to send Action Command 1!"));
    else
    {
        if(m_bReqAck && (m_AckCount!=0))
        {
            if(iNumOfAck == 0)
                m_statusbarStatic.SetWindowText(_T("No Ack!"));
            else
            {
                CString sMessage;
                sMessage.Format(_T("%d Ack was received."), iNumOfAck);
                m_statusbarStatic.SetWindowText(sMessage);
            }
        }
        else
            m_statusbarStatic.SetWindowText(_T("Action Command 1 sent sucessfully!"));
    }

    if(pAckBuffer != NULL)
        delete [] pAckBuffer;
}

void CActionCommandSampleDlg::OnBnClickedAction2Button()
{
    uint32_t	iNumOfAck;
    J_tACTION_CMD_ACK		*pAckBuffer = NULL;

    m_AckCount = m_AckCountSpin.GetPos();

    m_statusbarStatic.SetWindowText(_T(" "));

    if(m_bReqAck && (m_AckCount!=0))
    {
        iNumOfAck = (uint32_t)m_AckCount;
        pAckBuffer = new J_tACTION_CMD_ACK[iNumOfAck];
    }

    if(J_ST_SUCCESS != J_Factory_SendActionCommand(m_hFactory, DEVICE_KEY, GROUP_KEY2, GROUP_MASK, pAckBuffer, &iNumOfAck))
        m_statusbarStatic.SetWindowText(_T("Failed to send Action Command 2!"));
    else
    {
        if(m_bReqAck && (m_AckCount!=0))
        {
            if(iNumOfAck == 0)
                m_statusbarStatic.SetWindowText(_T("No Ack!"));
            else
            {
                CString sMessage;
                sMessage.Format(_T("%d Ack was received."), iNumOfAck);
                m_statusbarStatic.SetWindowText(sMessage);
            }
        }
        else
            m_statusbarStatic.SetWindowText(_T("Action Command 2 sent sucessfully!"));
    }

    if(pAckBuffer != NULL)
        delete [] pAckBuffer;
}

void CActionCommandSampleDlg::OnBnClickedReqAckCheck()
{
    if(BST_CHECKED == m_reqAckCheck.GetCheck())
        m_bReqAck = true;
    else
        m_bReqAck = false;
}

void CActionCommandSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CActionCommandSampleDlg::OnBnClickedOk()
{
	OnOK();
}
