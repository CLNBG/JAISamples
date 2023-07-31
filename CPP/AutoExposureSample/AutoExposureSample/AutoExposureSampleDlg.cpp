// AutoExposureSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AutoExposureSample.h"
#include "AutoExposureSampleDlg.h"
#include <vector>

#define _USE_MATH_DEFINES
#include <cmath>


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
	std::string strTransportName((char*)sCameraId);
	if(std::string::npos != strTransportName.find("TLActiveSilicon"))
	{
		std::string strName((char*)szName);
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
	std::string strTransportName((char*)sCameraId);
	if(std::string::npos != strTransportName.find("TLActiveSilicon"))
	{
		std::string strIncoming = std::string("Incoming") + std::string((char*)szName);
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

// CAutoExposureSampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CAutoExposureSampleDlg::CAutoExposureSampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAutoExposureSampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    // Initialize all JAI SDK handles
    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;

    // We are keeping the node handles for Gain and Exposure in order to make the control simpler and faster
    m_hGainNode = NULL;
    m_hExposureNode = NULL;

    // Initialize GUI update helpers
    m_UpdateUI = false;
    m_OldRAverage = 0;
    m_OldGAverage = 0;
    m_OldBAverage = 0;
    m_bIsCameraReady = false;
    m_bIsImageAcquiring = false;

    // Initialize the ALC controller
    m_ALCController.SetALCType(MinGainPriority);
    m_ALCController.SetKGain(25.0);
    m_ALCController.SetKExposure(3.0);
    m_ALCController.SetThreshold(0.02);         // Threshold of +-0.01 relative error

    m_ConversionBuffer.pImageBuffer = NULL; // Mark the buffer as empty

    m_bAutoExposure = true;  // ALC is default enabled
    m_bUseAverage = true;    // Use averafe

    InitializeCriticalSection(&m_ControlCriticalSection);
}

void CAutoExposureSampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_ALC_MODE_COMBO, m_ALCModeCtrl);
}

BEGIN_MESSAGE_MAP(CAutoExposureSampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CAutoExposureSampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CAutoExposureSampleDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_AUTOEXPOSURE_CHECK, &CAutoExposureSampleDlg::OnBnClickedAutoexposureCheck)
    ON_EN_CHANGE(IDC_XPOS_EDIT, &CAutoExposureSampleDlg::OnEnChangeXposEdit)
    ON_EN_CHANGE(IDC_YPOS_EDIT, &CAutoExposureSampleDlg::OnEnChangeYposEdit)
    ON_EN_CHANGE(IDC_WIDTH_EDIT, &CAutoExposureSampleDlg::OnEnChangeWidthEdit)
    ON_EN_CHANGE(IDC_HEIGHT_EDIT, &CAutoExposureSampleDlg::OnEnChangeHeightEdit)
    ON_EN_CHANGE(IDC_MIN_GAIN_EDIT, &CAutoExposureSampleDlg::OnEnChangeMinGainEdit)
    ON_EN_CHANGE(IDC_MAX_GAIN_EDIT, &CAutoExposureSampleDlg::OnEnChangeMaxGainEdit)
    ON_EN_CHANGE(IDC_MIN_EXPOSURE_EDIT, &CAutoExposureSampleDlg::OnEnChangeMinExposureEdit)
    ON_EN_CHANGE(IDC_MAX_EXPOSURE_EDIT, &CAutoExposureSampleDlg::OnEnChangeMaxExposureEdit)
    ON_CBN_SELCHANGE(IDC_ALC_MODE_COMBO, &CAutoExposureSampleDlg::OnCbnSelchangeAlcModeCombo)
    ON_BN_CLICKED(IDC_PEAK_RADIO, &CAutoExposureSampleDlg::OnBnClickedPeakRadio)
    ON_BN_CLICKED(IDC_AVERAGE_RADIO, &CAutoExposureSampleDlg::OnBnClickedAverageRadio)
	ON_BN_CLICKED(IDCANCEL, &CAutoExposureSampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CAutoExposureSampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAutoExposureSampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CAutoExposureSampleDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // Open factory & camera
    if (OpenFactoryAndCamera())
    {
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString((char*)m_sCameraId));    // Display camera ID
        m_bIsCameraReady = true;
        EnableControls();   // Enable Controls
    }
    else
    {
        GetDlgItem(IDC_CAMERAID)->SetWindowText(CString("Error"));
        m_bIsCameraReady = false;
        EnableControls();  // Enable Controls
    }

    // Update the check-box with current ALC state
    ((CButton*)GetDlgItem(IDC_AUTOEXPOSURE_CHECK))->SetCheck(m_bAutoExposure);

    // Select the first ALC mode
    m_ALCModeCtrl.SetCurSel(0);

    // Initialize Controls: This will read all the current values and limits from the camera and set up the GUI
    InitializeControls();   

	SetWindowPos(&CWnd::wndTopMost, 0 , 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

//--------------------------------------------------------------------------------------------------
// OnDestroy
//--------------------------------------------------------------------------------------------------
void CAutoExposureSampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Stop acquisition
    OnBnClickedStop();

    // Close factory & camera
    CloseFactoryAndCamera();

    // Free up the critical section
    DeleteCriticalSection(&m_ControlCriticalSection);

    // Free up memory allocated for the image conversion 
    if (m_ConversionBuffer.pImageBuffer == NULL)
        J_Image_Free(&m_ConversionBuffer);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAutoExposureSampleDlg::OnPaint()
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
HCURSOR CAutoExposureSampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

//--------------------------------------------------------------------------------------------------
// ShowErrorMsg
//--------------------------------------------------------------------------------------------------
void CAutoExposureSampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CAutoExposureSampleDlg::OpenFactoryAndCamera()
{
    J_STATUS_TYPE   retval;
    uint32_t        iSize;
    uint32_t        iNumDev;
    bool8_t         bHasChange;

    // Open factory
    retval = J_Factory_Open((int8_t*)"" , &m_hFactory);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not open factory!"), retval);
        return FALSE;
    }
    TRACE("Opening factory succeeded\n");

    // Update camera list
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
        ShowErrorMsg(CString("There is no camera!"), retval);
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
    TRACE("Opening camera succeeded\n");

    // We can now adjust the ALC controller settings based on the camera type found
	int8_t modelName[J_CAMERA_INFO_SIZE];
	int8_t serialNumber[J_CAMERA_INFO_SIZE];
    uint32_t size = sizeof(modelName);
	retval = J_Factory_GetCameraInfo(m_hFactory, m_sCameraId, CAM_INFO_MODELNAME, modelName, &size);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get model name of the camera!"), retval);
		//return FALSE;
	}

	size = sizeof(serialNumber); //Don't forget to reset the size for each call to J_Factory_GetCameraInfo!
	retval = J_Factory_GetCameraInfo(m_hFactory, m_sCameraId, CAM_INFO_SERIALNUMBER, serialNumber, &size);

    CString modelNameString = CString((char*)modelName);
    if (modelNameString.Find(_T("030GE"))!=-1)
    {
        m_ALCController.SetKGain(25.0);
        m_ALCController.SetKExposure(3.0);
    }
    else if (modelNameString.Find(_T("040GE"))!=-1)
    {
        m_ALCController.SetKGain(25.0);
        m_ALCController.SetKExposure(3.0);
    }
    else if (modelNameString.Find(_T("080GE"))!=-1)
    {
        m_ALCController.SetKGain(25.0);
        m_ALCController.SetKExposure(3.0);
    }
    else if (modelNameString.Find(_T("140GE"))!=-1)
    {
        m_ALCController.SetKGain(25.0);
        m_ALCController.SetKExposure(3.0);
    }
    else if (modelNameString.Find(_T("200GE"))!=-1)
    {
        m_ALCController.SetKGain(35.0);
        m_ALCController.SetKExposure(4.0);
    }
    else if (modelNameString.Find(_T("500GE"))!=-1)
    {
        m_ALCController.SetKGain(15.0);
        m_ALCController.SetKExposure(3.0);
    }

    // Set up the camera for continuous exposure mode

    // We have two possible ways of setting up triggers: JAI or GenICam SFNC
    // The JAI trigger setup uses a node called "ExposureMode" and the SFNC uses a node
    // called "TriggerSelector". Therefor we have to determine which way to use here.
    // First we see if a node called "TriggerSelector" exists.
    NODE_HANDLE hNode = NULL;
    retval = J_Camera_GetNodeByName(m_hCam, (int8_t*)"TriggerSelector", &hNode);

    // Does the "TriggerSelector" node exist?
    if ((retval == J_ST_SUCCESS) && (hNode != NULL))
    {
        // Here we assume that this is GenICam SFNC trigger so we do the following:
        // TriggerSelector=FrameStart
        // TriggerMode=Off
        retval = J_Camera_SetValueString(m_hCam, (int8_t*)"TriggerSelector", (int8_t*)"FrameStart");
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Unable to switch camera into continuous mode!"), retval);
            return FALSE;
        }
        retval = J_Camera_SetValueString(m_hCam, (int8_t*)"TriggerMode", (int8_t*)"Off");
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Unable to switch camera into continuous mode!"), retval);
            return FALSE;
        }
    }
    else
    {
        // Here we assume that this is JAI trigger so we do the following:
        // ExposureMode=Continuous

        retval = J_Camera_SetValueString(m_hCam, (int8_t*)"ExposureMode", (int8_t*)"Continuous");
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Unable to switch camera into continuous mode!"), retval);
            return FALSE;
        }
    }

    return TRUE;
}

//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CAutoExposureSampleDlg::CloseFactoryAndCamera()
{
    // Has the Camera been successfully opened?
    if (m_hCam)
    {
		J_Camera_SetValueString(m_hCam, (int8_t*)"TriggerMode", (int8_t*)"Off");

		// Close camera
        J_Camera_Close(m_hCam);
        m_hCam = NULL;
        TRACE("Closed camera\n");
    }

    // Has the Factory been sucessfully opened?
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
void CAutoExposureSampleDlg::OnBnClickedStart()
{
    J_STATUS_TYPE   retval;
    int64_t int64Val;

    SIZE	ViewSize;
    RECT	Frame;

    if (m_hCam == NULL)
        return;

    // Get Width from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

    // Get Height from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
    ViewSize.cy = (LONG)int64Val;     // Set window size cy

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

    // Create frame with correct aspect ratio
    Frame.left = 10;
    Frame.top = 10;
    Frame.right = 10+500;
    Frame.bottom = 10+(LONG)(500.0*((double)ViewSize.cy/(double)ViewSize.cx));

    // Get PixelFormat from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &int64Val);
    int pixelformat = (int)int64Val;

	//Set frame grabber pixel format, if applicable
	SetFramegrabberPixelFormat(m_hCam, NODE_NAME_PIXELFORMAT, pixelformat, &m_sCameraId[0]);

    // Calculate number of bits (not bytes) per pixel using macro
    int bpp = J_BitsPerPixel(pixelformat);

    // Open view window
    retval = J_Image_OpenViewWindowEx(J_IVW_OVERLAPPED_STRETCH, _T("Image View Window"), &Frame, &ViewSize, NULL, &m_hView);
    if (retval != J_ST_SUCCESS) {
        AfxMessageBox(CString("Could not open view window!"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    TRACE("Opening view window succeeded\n");

    // Open stream
    retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CAutoExposureSampleDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
    if (retval != J_ST_SUCCESS) {
        AfxMessageBox(CString("Could not open stream!"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }
    TRACE("Opening stream succeeded\n");

    // Start Acquision inside the camera
    retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);

    m_bIsImageAcquiring = true;
    EnableControls();

    // Start the UI update Timer
    SetTimer(1, 10, NULL);
}

//--------------------------------------------------------------------------------------------------
// FindPeak
//--------------------------------------------------------------------------------------------------
J_STATUS_TYPE CAutoExposureSampleDlg::FindPeak(J_tIMAGE_INFO * pBufferInfo, RECT *pMeasureRect, J_tBGR48 *pRGBpeak)
{
    POINT pixelPoint;

    // Initialize the peak value to 0
    pRGBpeak->R16 = 0;
    pRGBpeak->G16 = 0;
    pRGBpeak->B16 = 0;

    // Run through all teh pixels and find the maximum value
    for (int x=pMeasureRect->left; x<pMeasureRect->right; x++)
    {
        for (int y=pMeasureRect->top; y<pMeasureRect->bottom; y++)
        {
            // Get the pixel value
            pixelPoint.x = x;
            pixelPoint.y = y;
            PIXELVALUE pixelValue;

            if (J_Image_GetPixel(pBufferInfo, &pixelPoint, &pixelValue) == J_ST_SUCCESS)
            {
                // Use the PixelType to select how to interpret the pixelValue structure info
                switch (pBufferInfo->iPixelType)
                {
                    case J_GVSP_PIX_MONO8:
                        if (pixelValue.PixelValueUnion.Mono8Type.Value > pRGBpeak->R16)
                            pRGBpeak->R16 = pixelValue.PixelValueUnion.Mono8Type.Value;
                        if (pixelValue.PixelValueUnion.Mono8Type.Value > pRGBpeak->G16)
                            pRGBpeak->G16 = pixelValue.PixelValueUnion.Mono8Type.Value;
                        if (pixelValue.PixelValueUnion.Mono8Type.Value > pRGBpeak->B16)
                            pRGBpeak->B16 = pixelValue.PixelValueUnion.Mono8Type.Value;
                        break;

                    case J_GVSP_PIX_MONO16:
                        // Be aware that the Mono 16 values are "normalized" into 16-bit values by
                        // shifting the 10- and 12-bit values from the cameras up to MSB!
                        // If you need to get 10-bit or 12-bit values instead of the normalized
                        // values you will ned to do like this:
                        // value = pixelValue.PixelValueUnion.Mono16Type.Value & OxFFC0) >> 6 // 10bit mono
                        // ... or
                        // value = pixelValue.PixelValueUnion.Mono16Type.Value & OxFFF0) >> 4 // 12bit mono
                        if (pixelValue.PixelValueUnion.Mono16Type.Value > pRGBpeak->R16)
                            pRGBpeak->R16 = pixelValue.PixelValueUnion.Mono16Type.Value;
                        if (pixelValue.PixelValueUnion.Mono16Type.Value > pRGBpeak->G16)
                            pRGBpeak->G16 = pixelValue.PixelValueUnion.Mono16Type.Value;
                        if (pixelValue.PixelValueUnion.Mono16Type.Value > pRGBpeak->B16)
                            pRGBpeak->B16 = pixelValue.PixelValueUnion.Mono16Type.Value;
                        break;

                    case J_GVSP_PIX_BGR8_PACKED:
                        if (pixelValue.PixelValueUnion.BGR24Type.RValue > pRGBpeak->R16)
                            pRGBpeak->R16 = pixelValue.PixelValueUnion.BGR24Type.RValue;
                        if (pixelValue.PixelValueUnion.BGR24Type.GValue > pRGBpeak->G16)
                            pRGBpeak->G16 = pixelValue.PixelValueUnion.BGR24Type.GValue;
                        if (pixelValue.PixelValueUnion.BGR24Type.BValue > pRGBpeak->B16)
                            pRGBpeak->B16 = pixelValue.PixelValueUnion.BGR24Type.BValue;
                    break;

                    case J_GVSP_PIX_BGR16_PACKED_INTERNAL:
                        // Be aware that the BGR 16 values are "normalized" into 16-bit values by
                        // shifting the 10- and 12-bit values from the cameras up to MSB!
                        // If you need to get 10-bit or 12-bit values instead of the normalized
                        // values you will ned to do like this:
                        // RValue = pixelValue.PixelValueUnion.BGR48Type.RValue & OxFFC0) >> 6 // 10bit Color
                        // GValue = pixelValue.PixelValueUnion.BGR48Type.GValue & OxFFC0) >> 6 // 10bit Color
                        // BValue = pixelValue.PixelValueUnion.BGR48Type.BValue & OxFFC0) >> 6 // 10bit Color
                        // ... or
                        // RValue = pixelValue.PixelValueUnion.BGR48Type.RValue & OxFFF0) >> 4 // 12bit Color
                        // GValue = pixelValue.PixelValueUnion.BGR48Type.GValue & OxFFF0) >> 4 // 12bit Color
                        // BValue = pixelValue.PixelValueUnion.BGR48Type.BValue & OxFFF0) >> 4 // 12bit Color
                        if (pixelValue.PixelValueUnion.BGR48Type.RValue > pRGBpeak->R16)
                            pRGBpeak->R16 = pixelValue.PixelValueUnion.BGR48Type.RValue;
                        if (pixelValue.PixelValueUnion.BGR48Type.GValue > pRGBpeak->G16)
                            pRGBpeak->G16 = pixelValue.PixelValueUnion.BGR48Type.GValue;
                        if (pixelValue.PixelValueUnion.BGR48Type.BValue > pRGBpeak->B16)
                            pRGBpeak->B16 = pixelValue.PixelValueUnion.BGR48Type.BValue;
                    break;
                }
            }
        }
    }

    return J_ST_SUCCESS;
}

//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CAutoExposureSampleDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
    J_STATUS_TYPE error = J_ST_SUCCESS;

    // Get average value and calculate new control parameters:
    J_tBGR48 Average;
    J_tBGR48 Peak;

    Average.R16 = 0;
    Average.G16 = 0;
    Average.B16 = 0;
    Peak.R16 = 0;
    Peak.G16 = 0;
    Peak.B16 = 0;

    // We need to allocate conversion buffer once
    if (m_ConversionBuffer.pImageBuffer == NULL)
        error = J_Image_Malloc(pAqImageInfo, &m_ConversionBuffer);

    // Then we convert into 48bpp image format
    error = J_Image_FromRawToImageEx(pAqImageInfo, &m_ConversionBuffer, BAYER_STANDARD_MULTI);

    if(m_hView && (pAqImageInfo->iAwaitDelivery < 3))
    {
        // We shows the image AFTER conversion or else the J_Image_ShowImageEx will call J_Image_FromRawToImage internally too!
        J_Image_ShowImageEx(m_hView, &m_ConversionBuffer);
    }

    // .. and then we can calculate the average value for the measurement ROI
    error = J_Image_GetAverage(&m_ConversionBuffer, &m_MeasureRect, &Average);

    error = FindPeak(&m_ConversionBuffer, &m_MeasureRect, &Peak);

    // Update the last measured average
    int average = (int)Average.G16;

    if ((int)Average.R16 != m_OldRAverage)
    {
        m_OldRAverage = (int)Average.R16;
        m_UpdateUI = true;
    }
    if ((int)Average.G16 != m_OldGAverage)
    {
        m_OldGAverage = (int)Average.G16;
        m_UpdateUI = true;
    }
    if ((int)Average.B16 != m_OldBAverage)
    {
        m_OldBAverage = (int)Average.B16;
        m_UpdateUI = true;
    }

    // Update the last measured peak
    int peak = (int)Peak.G16;

    if ((int)Peak.R16 != m_OldRPeak)
    {
        m_OldRPeak = (int)Peak.R16;
        m_UpdateUI = true;
    }

    if ((int)Peak.G16 != m_OldGPeak)
    {
        m_OldGPeak = (int)Peak.G16;
        m_UpdateUI = true;
    }

    if ((int)Peak.B16 != m_OldBPeak)
    {
        m_OldBPeak = (int)Peak.B16;
        m_UpdateUI = true;
    }

    // Is the ALC active?
    if (m_bAutoExposure)
    {
        // Calculate will return true is any changes has been made to either Gain or Exposure
        bool changes = false;

        if (m_bUseAverage)
            changes = m_ALCController.Calculate(average);
        else
            changes = m_ALCController.Calculate(peak);

        if (changes)
        {
            // Send the new values to the camera
            UpdateGainAndExposureValues();
            m_UpdateUI = true;
        }
    }
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CAutoExposureSampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

    // Stop the GUI update timer
    KillTimer(1);

    // Stop Acquision
    if (m_hCam)
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

    m_bIsImageAcquiring = false;
    EnableControls();
}

//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CAutoExposureSampleDlg::InitializeControls()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    int64_t int64Val;

    CSliderCtrl* pSCtrl;

    if (m_hCam == NULL)
        return;

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
        retval = J_Node_GetValueInt64(hNode, TRUE, &int64Val);

        m_Width = (int)int64Val;
        pSCtrl->SetPos(m_Width);
        SetDlgItemInt(IDC_IMAGE_WIDTH_EDIT, m_Width);

		//Set frame grabber dimension, if applicable
		SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);
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
        retval = J_Node_GetValueInt64(hNode, TRUE, &int64Val);

        m_Height = (int)int64Val;
        pSCtrl->SetPos(m_Height);
        SetDlgItemInt(IDC_IMAGE_HEIGHT_EDIT, m_Height);

		//Set frame grabber dimension, if applicable
		SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);
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
    retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_GAIN, &m_hGainNode);
    if (retval == J_ST_SUCCESS)
    {
        int minGain = 0;
        int maxGain = 0;
        int currentGain = 0;

        // Get/Set Min
        retval = J_Node_GetMinInt64(m_hGainNode, &int64Val);
        pSCtrl->SetRangeMin((int)int64Val, TRUE);

        minGain = (int)int64Val;

        // Get/Set Max
        retval = J_Node_GetMaxInt64(m_hGainNode, &int64Val);
        pSCtrl->SetRangeMax((int)int64Val, TRUE);

        maxGain = (int)int64Val;

        // Get/Set Value
        retval = J_Node_GetValueInt64(m_hGainNode, TRUE, &int64Val);
        pSCtrl->SetPos((int)int64Val);

        currentGain = (int)int64Val;

        // Update the values inside the ALC controller
        m_ALCController.SetMinGain(minGain);
        m_ALCController.SetMaxGain(maxGain);
        m_ALCController.SetCurrentGain(currentGain);

        SetDlgItemInt(IDC_IMAGE_GAIN_EDIT, currentGain);

        SetDlgItemInt(IDC_MIN_GAIN_EDIT, minGain);
        SetDlgItemInt(IDC_MAX_GAIN_EDIT, maxGain);
        ((CSpinButtonCtrl*)GetDlgItem(IDC_MIN_GAIN_SPIN))->SetRange32(minGain, maxGain);
        ((CSpinButtonCtrl*)GetDlgItem(IDC_MAX_GAIN_SPIN))->SetRange32(minGain, maxGain);
        pSCtrl->SetSelection(minGain, maxGain);
        pSCtrl->SetTicFreq((maxGain-minGain)/20);
    }
    else
    {
        pSCtrl->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_LBL_GAIN)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_IMAGE_GAIN_EDIT)->ShowWindow(SW_HIDE);
    }

    //- Exposure time -----------------------------------------------

    // We have two possible ways of setting up Exposure time: JAI or GenICam SFNC
    // The JAI Exposure time setup uses a node called "ShutterMode" and the SFNC 
    // does not need to set up anything in order to be able to control the exposure time.
    // Therefor we have to determine which way to use here.
    // First we see if a node called "ShutterMode" exists.
    retval = J_Camera_GetNodeByName(m_hCam, (int8_t*)"ShutterMode", &hNode);

    // Does the "ShutterMode" node exist?
    if ((retval == J_ST_SUCCESS) && (hNode != NULL))
    {
        // Here we assume that this is JAI way so we do the following:
        // ShutterMode=ProgrammableExposure
        // Make sure that the ShutterMode selector is set to ProgrammableExposure
        retval = J_Camera_SetValueString(m_hCam, (int8_t*)"ShutterMode", (int8_t*)"ProgrammableExposure");
    }

    // Get SliderCtrl for Exposure
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE);

    // Get ExposureTimeRaw Node
    retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_EXPOSURE, &m_hExposureNode);
    if (retval == J_ST_SUCCESS)
    {
        int minExposure = 0;
        int maxExposure = 0;
        int currentExposure = 0;

        // Get/Set Min
        retval = J_Node_GetMinInt64(m_hExposureNode, &int64Val);
        pSCtrl->SetRangeMin((int)int64Val, TRUE);

        minExposure = (int)int64Val;

        // Get/Set Max
        retval = J_Node_GetMaxInt64(m_hExposureNode, &int64Val);
        pSCtrl->SetRangeMax((int)int64Val, TRUE);

        maxExposure = (int)int64Val;

        // Get/Set Value
        retval = J_Node_GetValueInt64(m_hExposureNode, TRUE, &int64Val);
        pSCtrl->SetPos((int)int64Val);

        currentExposure = (int)int64Val;

        // Update the values inside the ALC controller
        m_ALCController.SetMinExposure(minExposure);
        m_ALCController.SetMaxExposure(maxExposure);
        m_ALCController.SetCurrentExposure(currentExposure);

        SetDlgItemInt(IDC_IMAGE_EXPOSURE_EDIT, currentExposure);

        SetDlgItemInt(IDC_MIN_EXPOSURE_EDIT, minExposure);
        SetDlgItemInt(IDC_MAX_EXPOSURE_EDIT, maxExposure);
        ((CSpinButtonCtrl*)GetDlgItem(IDC_MIN_EXPOSURE_SPIN))->SetRange32(minExposure, maxExposure);
        ((CSpinButtonCtrl*)GetDlgItem(IDC_MAX_EXPOSURE_SPIN))->SetRange32(minExposure, maxExposure);
        pSCtrl->SetSelection(minExposure, maxExposure);
        pSCtrl->SetTicFreq((maxExposure-minExposure)/20);
    }

    // Get SliderCtrl for SetPoint
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SETPOINT);

    // Get PixelFormat from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &int64Val);
    int pixelformat = (int)int64Val;

    // We need to adjust the setpoint value range and start depending on the current pixel format!
    // If the number of bytes per pixel is 1 then we know it is 8-bit image
    // If the number of bytes per pixel is more than 1 then we know the image will be "normalized" into 16-bit image when converted
    if (J_BPP(pixelformat)==1)
    {
        m_ALCController.SetSetpoint(128);
        pSCtrl->SetRange(0,255);
        pSCtrl->SetPos(m_ALCController.GetSetPoint());
        SetDlgItemInt(IDC_SETPOINT, m_ALCController.GetSetPoint());
        pSCtrl->SetTicFreq(255/20);
    }
    else
    {
        m_ALCController.SetSetpoint(32768);
        pSCtrl->SetRange(0,65535);
        pSCtrl->SetPos(m_ALCController.GetSetPoint());
        SetDlgItemInt(IDC_SETPOINT, m_ALCController.GetSetPoint());
        pSCtrl->SetTicFreq(65535/20);
    }

    // Set up average measurement ROI as centre 1/3 of the image area as default
    m_MeasureRect.left = m_Width/3;
    m_MeasureRect.top = m_Height/3;
    m_MeasureRect.right = m_Width*2/3;
    m_MeasureRect.bottom = m_Height*2/3;

    SetDlgItemInt(IDC_WIDTH_EDIT, (m_MeasureRect.right-m_MeasureRect.left));
    SetDlgItemInt(IDC_HEIGHT_EDIT, (m_MeasureRect.bottom-m_MeasureRect.top));
    SetDlgItemInt(IDC_XPOS_EDIT, m_MeasureRect.left);
    SetDlgItemInt(IDC_YPOS_EDIT, m_MeasureRect.top);

    ((CSpinButtonCtrl*)GetDlgItem(IDC_XPOS_SPIN))->SetRange32(0, m_Width);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_YPOS_SPIN))->SetRange32(0, m_Height);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_WIDTH_SPIN))->SetRange32(0, m_Width);
    ((CSpinButtonCtrl*)GetDlgItem(IDC_HEIGHT_SPIN))->SetRange32(0, m_Height);

    ((CButton*)GetDlgItem(IDC_PEAK_RADIO))->SetCheck(!m_bUseAverage);
    ((CButton*)GetDlgItem(IDC_AVERAGE_RADIO))->SetCheck(m_bUseAverage);
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CAutoExposureSampleDlg::EnableControls()
{
    GetDlgItem(IDC_START)->EnableWindow(m_bIsCameraReady ? (m_bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_STOP)->EnableWindow(m_bIsCameraReady ? (m_bIsImageAcquiring ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_SLIDER_WIDTH)->EnableWindow(m_bIsCameraReady ? (m_bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_HEIGHT)->EnableWindow(m_bIsCameraReady ? (m_bIsImageAcquiring ? FALSE : TRUE) : FALSE);

    GetDlgItem(IDC_SLIDER_SETPOINT)->EnableWindow(m_bAutoExposure ? TRUE : FALSE);
    GetDlgItem(IDC_SETPOINT)->EnableWindow(m_bAutoExposure ? TRUE : FALSE);

    // The Gain and Exposure sliders depend on the actual ALC mode!
    GetDlgItem(IDC_SLIDER_GAIN)->EnableWindow(m_bIsCameraReady ? (m_bAutoExposure ? ((m_ALCController.GetALCType()==ExposureOnly) ? TRUE : FALSE) : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_EXPOSURE)->EnableWindow(m_bIsCameraReady ? (m_bAutoExposure ? ((m_ALCController.GetALCType()==GainOnly) ? TRUE : FALSE) : TRUE) : FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnHScroll
// This function handles all the Slider Control "drag" events
//--------------------------------------------------------------------------------------------------
void CAutoExposureSampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CSliderCtrl* pSCtrl;
    int iPos;
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;

    //- Width ------------------------------------------------

    // Get SliderCtrl for Width
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_WIDTH);
    // Is this scroll event from the Width Slider?
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        // Get current slider position
        iPos = pSCtrl->GetPos();

        // Get Width Node
        retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_WIDTH, &hNode);
        if (retval == J_ST_SUCCESS) 
        {
            // Set Value
            retval = J_Node_SetValueInt64(hNode, FALSE, (int64_t)iPos);

			//Set frame grabber dimension, if applicable
			SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, (int64_t)iPos, &m_sCameraId[0]);
        }

        // Update the Text Control with the new value
        SetDlgItemInt(IDC_IMAGE_WIDTH_EDIT, iPos);
        m_Width = iPos;
    }

    //- Height -----------------------------------------------

    // Get SliderCtrl for Height
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_HEIGHT);
    // Is this scroll event from the Height Slider?
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        // Get current slider position
        iPos = pSCtrl->GetPos();

        // Get Height Node
        retval = J_Camera_GetNodeByName(m_hCam, NODE_NAME_HEIGHT, &hNode);
        if (retval == J_ST_SUCCESS) 
        {
            // Set Value
            retval = J_Node_SetValueInt64(hNode, FALSE, (int64_t)iPos);

			//Set frame grabber dimension, if applicable
			SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, (int64_t)iPos, &m_sCameraId[0]);
        }

        // Update the Text Control with the new value
        SetDlgItemInt(IDC_IMAGE_HEIGHT_EDIT, iPos);
        m_Height = iPos;
    }

    // If the auto exposure is inactive then the user can control the values directly
    if (!m_bAutoExposure || (m_ALCController.GetALCType()==ExposureOnly))
    {
        //- Gain -----------------------------------------------

        // Get SliderCtrl for Gain
        pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
        // Is this scroll event from the Gain Slider?
        if (pSCtrl == (CSliderCtrl*)pScrollBar) 
        {
            // Get current slider position
            iPos = pSCtrl->GetPos();

            // Set Value
            retval = J_Node_SetValueInt64(m_hGainNode, TRUE, (int64_t)iPos);

            // Update the Text Control with the new value
            SetDlgItemInt(IDC_IMAGE_GAIN_EDIT, iPos);
            m_ALCController.SetCurrentGain(iPos);
        }
    }

    if (!m_bAutoExposure || (m_ALCController.GetALCType()==GainOnly))
    {
        //- Exposure -----------------------------------------------

        // Get SliderCtrl for Exposure
        pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE);
        // Is this scroll event from the Exposure Slider?
        if (pSCtrl == (CSliderCtrl*)pScrollBar) 
        {
            // Get current slider position
            iPos = pSCtrl->GetPos();

            // Set Value
            retval = J_Node_SetValueInt64(m_hExposureNode, TRUE, (int64_t)iPos);

            // Update the Text Control with the new value
            SetDlgItemInt(IDC_IMAGE_EXPOSURE_EDIT, iPos);
            m_ALCController.SetCurrentExposure(iPos);
        }
    }

    // Get SliderCtrl for the Set-point
    pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_SETPOINT);
    // Is this scroll event from the Set-point Slider?
    if (pSCtrl == (CSliderCtrl*)pScrollBar) 
    {
        // Get current slider position
        m_ALCController.SetSetpoint(pSCtrl->GetPos());

        // Update the Text Control with the new value
        SetDlgItemInt(IDC_SETPOINT, pSCtrl->GetPos());
    }

    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAutoExposureSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        if (m_UpdateUI)
        {
            // Update UI
            SetDlgItemInt(IDC_R_AVERAGE_EDIT, m_OldRAverage);
            SetDlgItemInt(IDC_G_AVERAGE_EDIT, m_OldGAverage);
            SetDlgItemInt(IDC_B_AVERAGE_EDIT, m_OldBAverage);
            SetDlgItemInt(IDC_R_PEAK_EDIT, m_OldRPeak);
            SetDlgItemInt(IDC_G_PEAK_EDIT, m_OldGPeak);
            SetDlgItemInt(IDC_B_PEAK_EDIT, m_OldBPeak);

            int newGain = m_ALCController.GetCurrentGain();

            CSliderCtrl* pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
            pSCtrl->SetPos(newGain);
            SetDlgItemInt(IDC_IMAGE_GAIN_EDIT, newGain);

            int newExposureTime = m_ALCController.GetCurrentExposure();

            pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE);
            pSCtrl->SetPos(newExposureTime);
            SetDlgItemInt(IDC_IMAGE_EXPOSURE_EDIT, newExposureTime);

            m_UpdateUI = false;
        }
    }

    CDialog::OnTimer(nIDEvent);
}

void CAutoExposureSampleDlg::UpdateGainAndExposureValues()
{
    if (m_hGainNode)
        J_Node_SetValueInt64(m_hGainNode, false, (int64_t)m_ALCController.GetCurrentGain());
    if (m_hExposureNode)
        J_Node_SetValueInt64(m_hExposureNode, false, (int64_t)m_ALCController.GetCurrentExposure());
}


void CAutoExposureSampleDlg::OnBnClickedAutoexposureCheck()
{
    if (((CButton*)GetDlgItem(IDC_AUTOEXPOSURE_CHECK))->GetCheck())
        m_bAutoExposure = true;
    else
        m_bAutoExposure = false;

    EnableControls();
}

void CAutoExposureSampleDlg::OnEnChangeXposEdit()
{
    m_MeasureRect.left = GetDlgItemInt(IDC_XPOS_EDIT);
    m_MeasureRect.right = m_MeasureRect.left + GetDlgItemInt(IDC_WIDTH_EDIT);
}

void CAutoExposureSampleDlg::OnEnChangeYposEdit()
{
    m_MeasureRect.top = GetDlgItemInt(IDC_YPOS_EDIT);
    m_MeasureRect.bottom = m_MeasureRect.top + GetDlgItemInt(IDC_HEIGHT_EDIT);
}

void CAutoExposureSampleDlg::OnEnChangeWidthEdit()
{
    m_MeasureRect.right = m_MeasureRect.left + GetDlgItemInt(IDC_WIDTH_EDIT);
}

void CAutoExposureSampleDlg::OnEnChangeHeightEdit()
{
    m_MeasureRect.bottom = m_MeasureRect.top + GetDlgItemInt(IDC_HEIGHT_EDIT);
}

void CAutoExposureSampleDlg::OnEnChangeMinGainEdit()
{
    int minGain = GetDlgItemInt(IDC_MIN_GAIN_EDIT);

    ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN))->SetSelection(minGain, m_ALCController.GetMaxGain());
    GetDlgItem(IDC_SLIDER_GAIN)->Invalidate();

    // When we adjust the Min gain then we should adjust the current gain accordingly in case the
    // current gain is outside the new boundries
    if (m_ALCController.SetMinGain(minGain))
    {
        int currentGain = m_ALCController.GetCurrentGain();
        J_Node_SetValueInt64(m_hGainNode, false, (int64_t)currentGain);

        CSliderCtrl* pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
        pSCtrl->SetPos(currentGain);
        SetDlgItemInt(IDC_IMAGE_GAIN_EDIT, currentGain);
    }
}

void CAutoExposureSampleDlg::OnEnChangeMaxGainEdit()
{
    int maxGain = GetDlgItemInt(IDC_MAX_GAIN_EDIT);

    ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN))->SetSelection(m_ALCController.GetMinGain(), maxGain);
    GetDlgItem(IDC_SLIDER_GAIN)->Invalidate();

    // When we adjust the Max gain then we should adjust the current gain accordingly in case the
    // current gain is outside the new boundries
    if (m_ALCController.SetMaxGain(maxGain))
    {
        int currentGain = m_ALCController.GetCurrentGain();
        J_Node_SetValueInt64(m_hGainNode, false, (int64_t)currentGain);

        CSliderCtrl* pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_GAIN);
        pSCtrl->SetPos(currentGain);
        SetDlgItemInt(IDC_IMAGE_GAIN_EDIT, currentGain);
    }
}

void CAutoExposureSampleDlg::OnEnChangeMinExposureEdit()
{
    int minExposure = GetDlgItemInt(IDC_MIN_EXPOSURE_EDIT);

    ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE))->SetSelection(minExposure, m_ALCController.GetMaxExposure());
    GetDlgItem(IDC_SLIDER_EXPOSURE)->Invalidate();

    // When we adjust the Min exposure then we should adjust the current exposure accordingly in case the
    // current exposure is outside the new boundries
    if (m_ALCController.SetMinExposure(minExposure))
    {
        int currentExposure = m_ALCController.GetCurrentExposure();
        J_Node_SetValueInt64(m_hExposureNode, false, (int64_t)currentExposure);

        CSliderCtrl* pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE);
        pSCtrl->SetPos(currentExposure);
        SetDlgItemInt(IDC_IMAGE_EXPOSURE_EDIT, currentExposure);
    }
}

void CAutoExposureSampleDlg::OnEnChangeMaxExposureEdit()
{
    int maxExposure = GetDlgItemInt(IDC_MAX_EXPOSURE_EDIT);

    ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE))->SetSelection(m_ALCController.GetMinExposure(), maxExposure);
    GetDlgItem(IDC_SLIDER_EXPOSURE)->Invalidate();

    // When we adjust the Max exposure then we should adjust the current exposure accordingly in case the
    // current exposure is outside the new boundries
    if (m_ALCController.SetMaxExposure(maxExposure))
    {
        int currentExposure = m_ALCController.GetCurrentExposure();
        J_Node_SetValueInt64(m_hExposureNode, false, (int64_t)currentExposure);

        CSliderCtrl* pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_EXPOSURE);
        pSCtrl->SetPos(currentExposure);
        SetDlgItemInt(IDC_IMAGE_EXPOSURE_EDIT, currentExposure);
    }
}

void CAutoExposureSampleDlg::OnCbnSelchangeAlcModeCombo()
{
    m_ALCController.SetALCType((EALCType) m_ALCModeCtrl.GetCurSel());
    EnableControls();
}

void CAutoExposureSampleDlg::OnBnClickedPeakRadio()
{
    if (((CButton*)GetDlgItem(IDC_PEAK_RADIO))->GetCheck())
        m_bUseAverage = false;
    else
        m_bUseAverage = true;
}

void CAutoExposureSampleDlg::OnBnClickedAverageRadio()
{
    if (((CButton*)GetDlgItem(IDC_AVERAGE_RADIO))->GetCheck())
        m_bUseAverage = true;
    else
        m_bUseAverage = false;
}

void CAutoExposureSampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CAutoExposureSampleDlg::OnBnClickedOk()
{
	OnOK();
}
