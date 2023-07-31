// LUTSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LUTSample.h"
#include "LUTSampleDlg.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Utility function to set the frame grabber's width/height (if one is present in the system).
void SetFramegrabberValue(CAM_HANDLE hCam, int8_t* szName, int64_t int64Val, int8_t* sCameraId)
{
	//Set frame grabber value, if applicable
	DEV_HANDLE hDev = NULL; //If a frame grabber exists, it is at the GenTL "local device layer".
	J_STATUS_TYPE retval = J_Camera_GetLocalDeviceHandle(hCam, &hDev);
	if(J_ST_SUCCESS != retval)
		return;

	if(NULL == hDev)
		return;

	NODE_HANDLE hNode;
	retval = J_Camera_GetNodeByName(hDev, szName, &hNode);
	if(J_ST_SUCCESS != retval)
		return;

	retval = J_Node_SetValueInt64(hNode, false, int64Val);
	if(J_ST_SUCCESS != retval)
		return;

	//Special handling for Active Silicon CXP boards, which also has nodes prefixed
	//with "Incoming":
	std::string strTransportName(sCameraId);
	if(std::string::npos != strTransportName.find("TLActiveSilicon"))
	{
		std::string strName(szName);
		if(std::string::npos != strName.find("Width") 
			|| std::string::npos != strName.find("Height"))
		{
			std::string strIncoming = "Incoming" + strName;
			NODE_HANDLE hNodeIncoming;
			J_STATUS_TYPE retval = J_Camera_GetNodeByName(hDev, (int8_t*)strIncoming.c_str(), &hNodeIncoming);
			if (retval == J_ST_SUCCESS)
			{
				retval = J_Node_SetValueInt64(hNodeIncoming, false, int64Val);
			}
		}

	}//if(std::string::npos != strTransportName.find("TLActiveSilicon"))
}

//Utility function to set the frame grabber's pixel format (if one is present in the system).
void SetFramegrabberPixelFormat(CAM_HANDLE hCam, int8_t* szName, int64_t jaiPixelFormat, int8_t* sCameraId)
{
	DEV_HANDLE hDev = NULL; //If a frame grabber exists, it is at the GenTL "local device layer".
	J_STATUS_TYPE retval = J_Camera_GetLocalDeviceHandle(hCam, &hDev);
	if(J_ST_SUCCESS != retval)
		return;

	if(NULL == hDev)
		return;

	int8_t szJaiPixelFormatName[512];
	uint32_t iSize = 512;
	retval = J_Image_Get_PixelFormatName(hCam, jaiPixelFormat, szJaiPixelFormatName, iSize);
	if(J_ST_SUCCESS != retval)
		return;

	NODE_HANDLE hLocalDeviceNode = 0;
	retval = J_Camera_GetNodeByName(hDev, (int8_t *)"PixelFormat", &hLocalDeviceNode);
	if(J_ST_SUCCESS != retval)
		return;

	if(0 == hLocalDeviceNode)
		return;

	//NOTE: this may fail if the camera and/or frame grabber does not use the SFNC naming convention for pixel formats!
	//Check the camera and frame grabber for details.
	retval = J_Node_SetValueString(hLocalDeviceNode, false, szJaiPixelFormatName);
	if(J_ST_SUCCESS != retval)
		return;

	//Special handling for Active Silicon CXP boards, which also has nodes prefixed
	//with "Incoming":
	std::string strTransportName(sCameraId);
	if(std::string::npos != strTransportName.find("TLActiveSilicon"))
	{
		std::string strIncoming = std::string("Incoming") + std::string(szName);
		NODE_HANDLE hNodeIncoming;
		J_STATUS_TYPE retval = J_Camera_GetNodeByName(hDev, (int8_t*)strIncoming.c_str(), &hNodeIncoming);
		if (retval == J_ST_SUCCESS)
		{
			//NOTE: this may fail if the camera and/or frame grabber does not use the SFNC naming convention for pixel formats!
			//Check the camera and frame grabber for details.
			retval = J_Node_SetValueString(hNodeIncoming, false, szJaiPixelFormatName);
		}
	}
}

// CLUTSampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CLUTSampleDlg::CLUTSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLUTSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;
    m_LUT.pLutR = NULL;
    m_LUT.pLutG = NULL;
    m_LUT.pLutB = NULL;
    m_LUT.iLUTEntries = 0;
    m_LUT.iPixelDepth = 0;
    m_LUT.iColors = 0;
    m_bProcess = false;
    m_bGamma = false;
    m_bOldGamma = false;
    m_GammaValue = 0.45;
    m_OldGammaValue = 0.45;
    m_OldPixelType = 0;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;

    InitializeCriticalSection(&m_CriticalSection);
}

void CLUTSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PIXELFORMAT_COMBO, m_PixelFormatCtrl);
}

BEGIN_MESSAGE_MAP(CLUTSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CLUTSampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CLUTSampleDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_PROCESS_CHECK, &CLUTSampleDlg::OnBnClickedProcessCheck)
    ON_BN_CLICKED(IDC_GAMMA_CHECK, &CLUTSampleDlg::OnBnClickedGammaCheck)
    ON_EN_CHANGE(IDC_GAMMA_EDIT, &CLUTSampleDlg::OnEnChangeGammaEdit)
    ON_BN_CLICKED(IDC_SAVE_LUT_BUTTON, &CLUTSampleDlg::OnBnClickedSaveLutButton)
    ON_BN_CLICKED(IDC_LOAD_LUT_BUTTON, &CLUTSampleDlg::OnBnClickedLoadLutButton)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_LINEAR_RADIO, &CLUTSampleDlg::OnBnClickedLinearRadio)
    ON_BN_CLICKED(IDC_INVERT_RADIO, &CLUTSampleDlg::OnBnClickedInvertRadio)
    ON_BN_CLICKED(IDC_COMPRESS_RADIO, &CLUTSampleDlg::OnBnClickedCompressRadio)
    ON_BN_CLICKED(IDC_EXPAND_RADIO, &CLUTSampleDlg::OnBnClickedExpandRadio)
    ON_BN_CLICKED(IDC_GAMMA_045_RADIO, &CLUTSampleDlg::OnBnClickedGamma045Radio)
    ON_BN_CLICKED(IDC_GAMMA_060_RADIO, &CLUTSampleDlg::OnBnClickedGamma060Radio)
    ON_CBN_SELCHANGE(IDC_PIXELFORMAT_COMBO, &CLUTSampleDlg::OnCbnSelchangePixelformatCombo)
	ON_BN_CLICKED(IDCANCEL, &CLUTSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CLUTSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLUTSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CLUTSampleDlg::OnInitDialog()
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
    InitializeControls();   // Initialize Controls

	if(m_bEnableStreaming)
	{
		((CButton*)GetDlgItem(IDC_LINEAR_RADIO))->SetCheck(1);

		CString gammaValueString;
		gammaValueString.Format(_T("%0.2f"), m_GammaValue);

		SetDlgItemText(IDC_GAMMA_EDIT, (LPCTSTR)gammaValueString);
		SetDlgItemText(IDC_LUT_FILENAME_EDIT, _T("test.lut"));
	}
	else
	{
		GetDlgItem(IDC_PROCESS_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_GAMMA_CHECK)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAVE_LUT_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_LOAD_LUT_BUTTON)->EnableWindow(FALSE);
		GetDlgItem(IDC_LINEAR_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_INVERT_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMPRESS_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_EXPAND_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_GAMMA_045_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_GAMMA_060_RADIO)->EnableWindow(FALSE);
		GetDlgItem(IDC_GAMMA_EDIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_PIXELFORMAT_COMBO)->EnableWindow(FALSE);
	}

    return TRUE;  // return TRUE  unless you set the focus to a control
}
//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CLUTSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Stop acquisition
    OnBnClickedStop();

    // Close factory & camera
    CloseFactoryAndCamera();

    DeleteCriticalSection(&m_CriticalSection);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLUTSampleDlg::OnPaint()
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
HCURSOR CLUTSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CLUTSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CLUTSampleDlg::OpenFactoryAndCamera()
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
void CLUTSampleDlg::CloseFactoryAndCamera()
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

    // Free up any memory allocated for the LUT
    J_Image_FreeLUT(&m_LUT);
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CLUTSampleDlg::OnBnClickedStart()
{
    J_STATUS_TYPE   retval;
    int64_t int64Val;

    SIZE	ViewSize;
    POINT	TopLeft;

	if(!m_bEnableStreaming)
	{
		ShowErrorMsg(CString("Streaming not enabled on this device."), 0);
		return;
	}

    // Get Width from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Width!"), retval);
        return;
    }
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

    // Get Height from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Height!"), retval);
        return;
    }
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

    // Set window position
    TopLeft.x = 100;
    TopLeft.y = 50;

	// Get the pixelformat from the camera
	int64_t pixelFormat = 0;
	uint64_t jaiPixelFormat = 0;
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

	//Set frame grabber pixel format, if applicable
	SetFramegrabberPixelFormat(m_hCam, NODE_NAME_PIXELFORMAT, pixelFormat, &m_sCameraId[0]);

	// Calculate number of bits (not bytes) per pixel using macro
	int bpp = J_BitsPerPixel(jaiPixelFormat);

    // Open view window
    retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open view window!"), retval);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Open stream
    retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CLUTSampleDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open stream!"), retval);
        return;
    }
    TRACE("Opening stream succeeded\n");

    // Start Acquisition
    retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
        return;
    }

    // Start timer to display processing speed
    SetTimer(1, 100, NULL);
    EnableControls(TRUE, TRUE);
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CLUTSampleDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
    if(m_hView)
    {
        // First time we need to allocate a lut that fits the pixel format
        if ((m_LUT.pLutR == NULL) || (pAqImageInfo->iPixelType != m_OldPixelType))
        {
            if (m_LUT.pLutR == NULL)
			{
                J_Image_MallocLUT(pAqImageInfo, &m_LUT);
			}
            else
            {
                // This tells us that the user changed the PixelFormat of the camera so we will most likely have to 
                // create a new LUT that fits with the new format!
                J_tLUT newLut;
                newLut.iLUTEntries = 0;
                newLut.iPixelDepth = 0;
                newLut.iColors = 0;
                newLut.pLutR = NULL;
                newLut.pLutG = NULL;
                newLut.pLutB = NULL;

                // We then create a new LUT and then copy the contents from the old LUT into the new format
                J_Image_MallocLUT(pAqImageInfo, &newLut);
                J_Image_ConvertLUT(&m_LUT, &newLut);

                // Free the old LUT
                J_Image_FreeLUT(&m_LUT);

                // Assign the new LUT to the member variable m_LUT
                m_LUT = newLut;
            }
            m_OldPixelType = pAqImageInfo->iPixelType;
        }

        // Here we can check if we need to change the LUT contents
        if ((m_bGamma != m_bOldGamma) || (m_GammaValue != m_OldGammaValue))
        {
            // Create Gamma LUT if needed
            if (m_bGamma)
            {
                J_Image_CreateGammaLUT(&m_LUT, 0, m_GammaValue); // Monochrome/Red LUT

                if (m_LUT.iColors > 1)
                {
                    J_Image_CreateGammaLUT(&m_LUT, 1, m_GammaValue); // Green LUT
                    J_Image_CreateGammaLUT(&m_LUT, 2, m_GammaValue); // Blue LUT
                }
            }

            m_bOldGamma = m_bGamma;
            m_OldGammaValue = m_GammaValue;
        }

        // For performance measurements only!
        m_StopWatch.Start();

        if (m_bProcess)
            J_Image_LUT(pAqImageInfo, &m_LUT);

        m_StopWatch.Stop();

        EnterCriticalSection(&m_CriticalSection);
        m_dTimeSpan = m_StopWatch.GetAverageTime();
        LeaveCriticalSection(&m_CriticalSection);

        // Shows image
        J_Image_ShowImage(m_hView, pAqImageInfo);
    }
}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CLUTSampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

    KillTimer(1);

    // Stop Acquisition
    if (m_hCam) {
        retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not Stop Acquisition!"), retval);
        }
    }

    if(m_hThread)
    {
        // Close stream
        retval = J_Image_CloseStream(m_hThread);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close Stream!"), retval);
        }
        m_hThread = NULL;
        TRACE("Closed stream\n");
    }

    if(m_hView)
    {
        // Close view window
        retval = J_Image_CloseViewWindow(m_hView);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close View Window!"), retval);
        }
        m_hView = NULL;
        TRACE("Closed view window\n");
    }

    EnableControls(TRUE, FALSE);
    m_StopWatch.Reset();
}
//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CLUTSampleDlg::InitializeControls()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode = NULL;
    NODE_HANDLE hEnumNode;
    int64_t int64Val;
    int comboboxIndex = 0;
    uint32_t numEnums;
	uint64_t jaiPixelFormat = 0;

    // Here we want to populate the ComboBox with the pixelformats from the camera
    retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_PIXELFORMAT, &hNode);
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
                    retval = J_Node_GetName(hEnumNode, buffer, &size); //J_Node_GetDescription(hEnumNode, buffer, &size);
					if(J_ST_SUCCESS == retval)
					{
						std::string strName((char*)buffer);

						//The name may have a prefix of "EnumEntry_PixelFormat_", so remove it.
						size_t pos = strName.rfind("EnumEntry_PixelFormat_");
						if(std::string::npos != pos && pos+1 < strName.length())
						{
							std::string strPrefix = strName.substr(0, pos+22);

							if(std::string::npos != (size_t)strPrefix.compare("EnumEntry_PixelFormat_"))
							{
								strName = strName.substr(pos+22);

								CString *pValuestring = new CString((char*)strName.c_str());
								comboboxIndex = m_PixelFormatCtrl.AddString(pValuestring->GetString());
								delete pValuestring;

								retval = J_Node_GetEnumEntryValue(hEnumNode, &int64Val);

								// Get the PixelFormat Enum value and put it in an array with same index as for the ComboBox
								m_PixelFormatValues[comboboxIndex] = int64Val;
 							}
						}
                    }
                }
            }
       }
    }
    else
    {
        ShowErrorMsg(CString("Unable to get PixelFormat node handle!"), retval);
    }

    // Get the current selected PixelFormat value and select it in the ComboBox
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &int64Val);

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

    CSliderCtrl* pSCtrl;

    //- Width ------------------------------------------------

	// Get SliderCtrl for Width
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WIDTH);
	if(m_bEnableStreaming)
	{
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

			retval = J_Node_GetInc(hNode, &m_iWidthInc);
			m_iWidthInc = max(1, m_iWidthInc);

			//Set frame grabber dimension, if applicable
			SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);
		}
		else
		{
			ShowErrorMsg(CString("Could not get Width node!"), retval);

			pSCtrl->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_WIDTH)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_WIDTH)->ShowWindow(SW_HIDE);
		}
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

	if(m_bEnableStreaming)
	{
		// Get Height Node
		retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_HEIGHT, &hNode);
		if (retval == J_ST_SUCCESS && m_bEnableStreaming)
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

			retval = J_Node_GetInc(hNode, &m_iHeightInc);
			m_iHeightInc = max(1, m_iHeightInc);

			//Set frame grabber dimension, if applicable
			SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);
		}
		else
		{
			ShowErrorMsg(CString("Could not get Height node!"), retval);

			pSCtrl->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_LBL_HEIGHT)->ShowWindow(SW_HIDE);
			GetDlgItem(IDC_HEIGHT)->ShowWindow(SW_HIDE);
		}
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
        //ShowErrorMsg(CString("Could not get Gain node!"), retval);

        pSCtrl->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_LBL_GAIN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_GAIN)->ShowWindow(SW_HIDE);
    }
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CLUTSampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
	GetDlgItem(IDC_START)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_SLIDER_WIDTH)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_WIDTH)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_HEIGHT)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_HEIGHT)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_GAIN)->EnableWindow(bIsCameraReady ? TRUE : FALSE);
    GetDlgItem(IDC_GAIN)->EnableWindow(bIsCameraReady ? TRUE : FALSE);

    GetDlgItem(IDC_PIXELFORMAT_COMBO)->EnableWindow(bIsCameraReady ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnHScroll
//--------------------------------------------------------------------------------------------------
void CLUTSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

		//To avoid J_Camera_SetValueInt64 errors,
		//make the slider value be an even multiple of the increment + min value
		int64_t inc = m_iWidthInc;
		if(inc > 1)
		{
			int64_t iMin = pSCtrl->GetRangeMin();

			int64_t diff = int64Val - iMin;

			ldiv_t divResult = div((long)diff, (long)inc);
			int64Val = iMin + (divResult.quot * inc);

			if(iPos != (int)int64Val)
			{
				pSCtrl->SetPos((int)int64Val);
			}
		}

        // Set Width value
        retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_WIDTH, int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not set Width!"), retval);
        }

		//Set frame grabber dimension, if applicable
		SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

        SetDlgItemInt(IDC_WIDTH, iPos);
    }

    //- Height -----------------------------------------------

    // Get SliderCtrl for Height
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) {

        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

		//To avoid J_Camera_SetValueInt64 errors,
		//make the slider value be an even multiple of the increment + min value
		int64_t inc = m_iHeightInc;
		if(inc > 1)
		{
			int64_t iMin = pSCtrl->GetRangeMin();

			int64_t diff = int64Val - iMin;

			ldiv_t divResult = div((long)diff, (long)inc);
			int64Val = iMin + (divResult.quot * inc);

			if(iPos != (int)int64Val)
			{
				pSCtrl->SetPos((int)int64Val);
			}
		}

        // Set Height Value
        retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_HEIGHT, int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not set Height!"), retval);
        }

		//Set frame grabber dimension, if applicable
		SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

        SetDlgItemInt(IDC_HEIGHT, iPos);
    }

    //- Gain -----------------------------------------------

    // Get SliderCtrl for Gain
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
    if (pSCtrl == (CSliderCtrl*)pScrollBar) {

        iPos = pSCtrl->GetPos();
        int64Val = (int64_t)iPos;

        // Set Gain value
        retval = J_Camera_SetValueInt64(m_hCam, NODE_NAME_GAIN, int64Val);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not set Gain!"), retval);
        }

        SetDlgItemInt(IDC_GAIN, iPos);
    }

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLUTSampleDlg::OnBnClickedProcessCheck()
{
    CButton* pButton = (CButton*)GetDlgItem(IDC_PROCESS_CHECK);
    if (pButton->GetCheck())
        m_bProcess = true;
    else
        m_bProcess = false;

    m_StopWatch.Reset();
}

void CLUTSampleDlg::OnBnClickedGammaCheck()
{
    CButton* pButton = (CButton*)GetDlgItem(IDC_GAMMA_CHECK);
    if (pButton->GetCheck())
        m_bGamma = true;
    else
        m_bGamma = false;

    m_StopWatch.Reset();
}

void CLUTSampleDlg::OnEnChangeGammaEdit()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
    CString gammaString;
    GetDlgItemText(IDC_GAMMA_EDIT, gammaString);

    m_GammaValue = _tstof(gammaString);

    m_StopWatch.Reset();
}

void CLUTSampleDlg::OnBnClickedSaveLutButton()
{
    J_STATUS_TYPE   retval;

    if (m_LUT.pLutR == NULL)
    {
        AfxMessageBox(_T("LUT not initialized correctly!"));
    }
    else
    {
        CString filename = _T("");
        GetDlgItemText(IDC_LUT_FILENAME_EDIT, filename);
        retval = J_Image_SaveLUT(&m_LUT, (LPCTSTR)filename);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not Save LUT to disk!"), retval);
        }
    }
}

void CLUTSampleDlg::OnBnClickedLoadLutButton()
{
    J_STATUS_TYPE   retval;

    CString filename = _T("");
    GetDlgItemText(IDC_LUT_FILENAME_EDIT, filename);
    retval = J_Image_LoadLUT(&m_LUT, (LPCTSTR)filename);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not Load LUT from disk!"), retval);
    }
}

void CLUTSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
    // Update the GUI with latest processing time value
    if (nIDEvent == 1)
    {
        CString TimeSpan = _T("");
        EnterCriticalSection(&m_CriticalSection);
        TimeSpan.Format(_T("%0.1f"), m_dTimeSpan);
        LeaveCriticalSection(&m_CriticalSection);
        SetDlgItemText(IDC_EDIT1, TimeSpan);
    }
    CDialog::OnTimer(nIDEvent);
}

void CLUTSampleDlg::OnBnClickedLinearRadio()
{
    J_STATUS_TYPE   retval;
    retval = J_Image_CreateLinearLUT(&m_LUT, 0, 0, m_LUT.iLUTEntries-1, 0, m_LUT.iLUTEntries-1);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not create Linear LUT for Mono/Red Channel!"), retval);
    }

    if (m_LUT.iColors > 1)
    {
        retval = J_Image_CreateLinearLUT(&m_LUT, 1, 0, m_LUT.iLUTEntries-1, 0, m_LUT.iLUTEntries-1);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Linear LUT for Green Channel!"), retval);
        }

        retval = J_Image_CreateLinearLUT(&m_LUT, 2, 0, m_LUT.iLUTEntries-1, 0, m_LUT.iLUTEntries-1);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Linear LUT for Blue Channel!"), retval);
        }
    }
    m_StopWatch.Reset();
}

void CLUTSampleDlg::OnBnClickedInvertRadio()
{
    J_STATUS_TYPE   retval;

	// Invert can be done using both J_Image_CreateLinearLUT() and J_Image_CreateKneeLUT()
    // If J_Image_CreateLinearLUT() is used then it looks like this:
    // retval = J_Image_CreateLinearLUT(&m_LUT, 0, m_LUT.iLUTEntries-1, m_LUT.iLUTEntries-1, 0);

    // Here is the way to do it using the J_Image_CreateKneeLUT() function instead:
    POINT knee1, knee2;
    knee1.x = 0;
    knee1.y = m_LUT.iLUTEntries-1;
    knee2.x = m_LUT.iLUTEntries-1;
    knee2.y = 0;
    retval = J_Image_CreateKneeLUT(&m_LUT, 0, knee1, knee2);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not create Inverted LUT for Mono/Red channel!"), retval);
    }
    if (m_LUT.iColors > 1)
    {
        retval = J_Image_CreateKneeLUT(&m_LUT, 1, knee1, knee2);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Inverted LUT for Green channel!"), retval);
        }
        retval = J_Image_CreateKneeLUT(&m_LUT, 2, knee1, knee2);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Inverted LUT for Blue channel!"), retval);
        }
    }
    m_StopWatch.Reset();
}

void CLUTSampleDlg::OnBnClickedCompressRadio()
{
    J_STATUS_TYPE   retval;
    retval = J_Image_CreateLinearLUT(&m_LUT, 0, 0, m_LUT.iLUTEntries-1, m_LUT.iLUTEntries/3, (m_LUT.iLUTEntries*2)/3);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not create Compressing LUT for Mono/Red channel!"), retval);
    }
    if (m_LUT.iColors > 1)
    {
        retval = J_Image_CreateLinearLUT(&m_LUT, 1, 0, m_LUT.iLUTEntries-1, m_LUT.iLUTEntries/3, (m_LUT.iLUTEntries*2)/3);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Compressing LUT for Green channel!"), retval);
        }
        retval = J_Image_CreateLinearLUT(&m_LUT, 2, 0, m_LUT.iLUTEntries-1, m_LUT.iLUTEntries/3, (m_LUT.iLUTEntries*2)/3);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Compressing LUT for Blue channel!"), retval);
        }
    }
    m_StopWatch.Reset();
}

void CLUTSampleDlg::OnBnClickedExpandRadio()
{
    J_STATUS_TYPE   retval;
    retval = J_Image_CreateLinearLUT(&m_LUT, 0, m_LUT.iLUTEntries/3, (m_LUT.iLUTEntries*2)/3, 0, m_LUT.iLUTEntries-1);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not create Expanding LUT for Mono/Red channel!"), retval);
    }
    if (m_LUT.iColors > 1)
    {
        retval = J_Image_CreateLinearLUT(&m_LUT, 1, m_LUT.iLUTEntries/3, (m_LUT.iLUTEntries*2)/3, 0, m_LUT.iLUTEntries-1);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Expanding LUT for Green channel!"), retval);
        }
        retval = J_Image_CreateLinearLUT(&m_LUT, 2, m_LUT.iLUTEntries/3, (m_LUT.iLUTEntries*2)/3, 0, m_LUT.iLUTEntries-1);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Expanding LUT for Blue channel!"), retval);
        }
    }
    m_StopWatch.Reset();
}

void CLUTSampleDlg::OnBnClickedGamma045Radio()
{
    J_STATUS_TYPE   retval;
    retval = J_Image_CreateGammaLUT(&m_LUT, 0, 0.45);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not create Gamma 0.45 LUT for Mono/Red channel!"), retval);
    }
    if (m_LUT.iColors > 1)
    {
        retval = J_Image_CreateGammaLUT(&m_LUT, 1, 0.45);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Gamma 0.45 LUT for Green channel!"), retval);
        }
        retval = J_Image_CreateGammaLUT(&m_LUT, 2, 0.45);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Gamma 0.45 LUT for Blue channel!"), retval);
        }
    }
    m_StopWatch.Reset();
}

void CLUTSampleDlg::OnBnClickedGamma060Radio()
{
    J_STATUS_TYPE   retval;
    retval = J_Image_CreateGammaLUT(&m_LUT, 0, 0.60);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not create Gamma 0.60 LUT!"), retval);
    }
    if (m_LUT.iColors > 1)
    {
        retval = J_Image_CreateGammaLUT(&m_LUT, 1, 0.60);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Gamma 0.60 LUT for Green channel!"), retval);
        }
        retval = J_Image_CreateGammaLUT(&m_LUT, 2, 0.60);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not create Gamma 0.60 LUT for Blue channel!"), retval);
        }
    }
    m_StopWatch.Reset();
}

void CLUTSampleDlg::OnCbnSelchangePixelformatCombo()
{
    // Change the Pixel Format 
    int index = m_PixelFormatCtrl.GetCurSel();

    J_Camera_SetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT,  m_PixelFormatValues[index]);
}

void CLUTSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CLUTSampleDlg::OnBnClickedOk()
{
	OnOK();
}
