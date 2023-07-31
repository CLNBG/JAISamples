// VCAVISampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VCAVISample.h"
#include "VCAVISampleDlg.h"
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

// CVCAVISampleDlg dialog

//--------------------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------------------
CVCAVISampleDlg::CVCAVISampleDlg(CWnd* pParent /*=NULL*/)
: CDialog(CVCAVISampleDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_hFactory = NULL;
    m_hCam = NULL;
    m_hView = NULL;
    m_hThread = NULL;
    m_pDIBBufferInfo = NULL;
    m_hAVI = NULL;
    m_hBmp = NULL;
    m_pBmi = NULL;
    m_pBits = NULL;
	m_pOldBuffer = NULL;
    m_Toggle = true;
    m_Use8BitMonoForOptimization = false;
    m_bIsAviFileReady = false;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;

    m_hMemoryDC = NULL;
    m_hMemoryBitmap = NULL;
    m_bResizeImage = false;
    m_iResizeImageWidth = 320;
    m_iResizeImageHeight = 240;

    m_RGain = 4096;   // Red channel gain. The gain is calculates as (m_xGain/4096) during White Balance calculation
    m_GGain = 4096;   // Green channel gain (This should always be kept at 4096 - a gain of 1.0)
    m_BGain = 4096;   // Blue channel gain
}

void CVCAVISampleDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVCAVISampleDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_START, &CVCAVISampleDlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CVCAVISampleDlg::OnBnClickedStop)
    ON_WM_HSCROLL()
    ON_EN_CHANGE(IDC_FRAME_RATE_EDIT, &CVCAVISampleDlg::OnEnChangeFrameRateEdit)
    ON_BN_CLICKED(IDC_PLAY_AVI, &CVCAVISampleDlg::OnBnClickedPlayAvi)
    ON_BN_CLICKED(IDC_IMAGE_RESIZE_CHECK, &CVCAVISampleDlg::OnBnClickedImageResizeCheck)
    ON_EN_CHANGE(IDC_WIDTH_EDIT, &CVCAVISampleDlg::OnEnChangeWidthEdit)
    ON_EN_CHANGE(IDC_HEIGHT_EDIT, &CVCAVISampleDlg::OnEnChangeHeightEdit)
	ON_BN_CLICKED(IDCANCEL, &CVCAVISampleDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CVCAVISampleDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CVCAVISampleDlg message handlers

//--------------------------------------------------------------------------------------------------
// OnInitDialog
//--------------------------------------------------------------------------------------------------
BOOL CVCAVISampleDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    m_hMemoryDC = CreateCompatibleDC(NULL);

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
void CVCAVISampleDlg::OnDestroy()
{
    CDialog::OnDestroy();

    // Stop acquisition
    OnBnClickedStop();

    // Close factory & camera
    CloseFactoryAndCamera();

    // Cleanup the Memory DC and Bitmap
    if (m_hMemoryBitmap != NULL)
        DeleteObject(m_hMemoryBitmap);

    if (m_hMemoryDC != NULL)
        DeleteDC(m_hMemoryDC);

    if (m_pBmi != NULL)
        free(m_pBmi);

}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVCAVISampleDlg::OnPaint()
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
HCURSOR CVCAVISampleDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CVCAVISampleDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
BOOL CVCAVISampleDlg::OpenFactoryAndCamera()
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
void CVCAVISampleDlg::CloseFactoryAndCamera()
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

    // Free the DIB conversion buffer if it has been allocated
    if (m_pDIBBufferInfo != NULL)
    {
        retval = J_Image_Free(m_pDIBBufferInfo);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not free BufferInfo structure!"), retval);
        }
        m_pDIBBufferInfo = NULL;
    }

}
//--------------------------------------------------------------------------------------------------
// OnBnClickedStart
//--------------------------------------------------------------------------------------------------
void CVCAVISampleDlg::OnBnClickedStart()
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
        ShowErrorMsg(CString("Could not get Width value!"), retval);
    }
    ViewSize.cx = (LONG)int64Val;     // Set window size cx

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

    // Get Height from the camera
    retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
    if (retval != J_ST_SUCCESS)
    {
        ShowErrorMsg(CString("Could not get Height value!"), retval);
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
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Unable to get PixelFormat value!"), retval);
		return;
	}
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
    retval = J_Image_OpenStream(m_hCam, 0, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this), reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CVCAVISampleDlg::StreamCBFunc), &m_hThread, (ViewSize.cx*ViewSize.cy*bpp)/8);
    if (retval != J_ST_SUCCESS) {
        ShowErrorMsg(CString("Could not open stream!"), retval);
        return;
    }
    TRACE("Opening stream succeeded\n");

    // Prompt the user for a filename for the AVI-file
    // szFilters is a text string that includes two file name filters:
    // "*.my" for "MyType Files" and "*.*' for "All Files."
    TCHAR szFilters[]= _T("AVI Files (*.avi)|*.avi|All Files (*.*)|*.*||");

    // Create an Open dialog; the default file name extension is ".avi".
    CFileDialog myFileDialog(FALSE, _T("avi"), _T("*.avi"), OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilters, this);
   
    m_bIsAviFileReady = false;

    // Display the file dialog. When user clicks OK, fileDlg.DoModal() 
    // returns IDOK.
    if( myFileDialog.DoModal ()==IDOK)
    {
        // Remember to specify the actual frame-rate the camera is producing! This has to be specified up-front when we create
        // the AVI stream
        int framerate = GetDlgItemInt(IDC_FRAME_RATE_EDIT);
        int frameperiodeMs = (int)((1.0/(double)framerate)*1000.0);

        m_AviFileName = myFileDialog.GetFileName();
        m_hAVI = CreateAvi(m_AviFileName, frameperiodeMs, NULL);

        // Do we want to select Compresser?
        bool SelectCompressionEncoder = (((CButton*)GetDlgItem(IDC_COMPRESSION_CHECK))->GetCheck() > 0);

        // Is this a Mono8 pixel format and does the user want to try to optimize for size?
        m_Use8BitMonoForOptimization = (pixelFormat == J_GVSP_PIX_MONO8) && (((CButton*)GetDlgItem(IDC_OPTIMIZATION_CHECK))->GetCheck() > 0);

        // First we need to create an appropriate DIBSection bitmap in order for us to create the AVI stream compression
        HBITMAP hBmp = NULL;

        // If we want to resize the image then the DIBSection has to have the correct size
        if (m_bResizeImage)
        {
            // We might already have created a Memory Bitmap with a different size so we deletes the old
            // one and create a new one instead
            if (m_hMemoryBitmap != NULL)
            {
                DeleteObject(m_hMemoryBitmap);
                m_hMemoryBitmap = NULL;
            }

            // Create the DIBSection for the resize size
            if (CreateDIBSectionBitmap(&m_hMemoryBitmap, NULL, m_iResizeImageWidth, m_iResizeImageHeight, (BITMAPINFO*)&m_Mono8BitmapInfo, m_Use8BitMonoForOptimization))
            {
                // Select the new bitmap into the Memory DC
                SelectObject(m_hMemoryDC, m_hMemoryBitmap);
                // Set the Stretch mode to Color on Color
                SetStretchBltMode(m_hMemoryDC,COLORONCOLOR);
                hBmp = m_hMemoryBitmap;
            }
        }
        else
        {
            // Create the DIBSection for the full image size
            CreateDIBSectionBitmap(&hBmp, NULL, ViewSize.cx, ViewSize.cy, (BITMAPINFO*)&m_Mono8BitmapInfo, m_Use8BitMonoForOptimization); 
        }

        // Set the compressor. If SelectCompressionEncoder=true then the user will have to select form a list of installed
        // encoders. If SelectCompressionEncoder=false then the default (uncompressed) encoder will automatically be selected
        HRESULT hr = SetAviVideoCompression(m_hAVI, hBmp, NULL, SelectCompressionEncoder, m_hWnd);
        if (hr != AVIERR_OK)
        {
            // Create an error message
            TCHAR buffer[100];
            if (FormatAviMessage(hr, buffer, 100))
            {
                MessageBox(buffer, _T("Set AVI Video Compression error!"));
            }
            EnableControls(TRUE, FALSE);
        }
        else
        {
            // Start Acquisition
            retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);
            if (retval != J_ST_SUCCESS)
            {
                ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
                return;
            }

            EnableControls(TRUE, TRUE);
        }
    }
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CVCAVISampleDlg::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
    J_STATUS_TYPE retsta = J_ST_SUCCESS;

    // We can choose to display the image too, but this will only slow down the AVI file creation
    if(m_hView && ((pAqImageInfo->iAwaitDelivery < 2) || m_Toggle))
    {
        // Shows image
        J_Image_ShowImage(m_hView, pAqImageInfo);
    }

    // Only display every second image if we are very busy!
    m_Toggle = !m_Toggle;

    if (m_hAVI != NULL)
    {
        // Flip the image upside-down since the input to the compressor is a bitmap
        retsta = J_Image_Flip(pAqImageInfo, J_FLIP_VERTICAL);

        if (m_Use8BitMonoForOptimization && (pAqImageInfo->iPixelType == J_GVSP_PIX_MONO8))
        {
            if (m_hBmp == NULL)
            {
                // The size might have changed so we need to free up and re-allocate
                if (m_pBmi)
                    free(m_pBmi);

                m_pBmi = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER)+(256*sizeof(RGBQUAD)));

                // Create the DIBSection once
                ZeroMemory(m_pBmi, sizeof(BITMAPINFOHEADER)+(256*sizeof(RGBQUAD)));

                // Fill-in necessary information to the Bitmap Header
                m_pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                m_pBmi->bmiHeader.biWidth = pAqImageInfo->iSizeX;
                m_pBmi->bmiHeader.biHeight = pAqImageInfo->iSizeY;
                m_pBmi->bmiHeader.biPlanes = 1;
                m_pBmi->bmiHeader.biBitCount = 8; // Mono8 format
                m_pBmi->bmiHeader.biCompression = BI_RGB;
                m_pBmi->bmiHeader.biSizeImage = pAqImageInfo->iImageSize;
                m_pBmi->bmiHeader.biXPelsPerMeter = 75;
                m_pBmi->bmiHeader.biYPelsPerMeter = 75;
                m_pBmi->bmiHeader.biClrUsed = 0;
                m_pBmi->bmiHeader.biClrImportant = 0;

                // Create a color palette with 256 entries for the monochrome bitmap
                for (int i=0; i<256; i++)
                {
                    m_pBmi->bmiColors[i].rgbRed = i;
                    m_pBmi->bmiColors[i].rgbGreen = i;
                    m_pBmi->bmiColors[i].rgbBlue = i;
                    m_pBmi->bmiColors[i].rgbReserved = 0;
                }

                // Create DIB section based on the original Bitmap Info Header structure
                m_hBmp = CreateDIBSection(NULL, m_pBmi, DIB_RGB_COLORS, (void **)&m_pBits, NULL,0);
            }

            // Copy the image contents to the DIBSection
            if (m_pBits != NULL)
            {
                memcpy(m_pBits, pAqImageInfo->pImageBuffer, pAqImageInfo->iImageSize);

                if (m_bResizeImage)
                {
                    // Here we need to resize the image using
                    int iLines = StretchDIBits(m_hMemoryDC, 0,0, m_iResizeImageWidth, m_iResizeImageHeight, 0,0, pAqImageInfo->iSizeX, pAqImageInfo->iSizeY, m_pBits, m_pBmi, DIB_RGB_COLORS, SRCCOPY);
                    if (iLines != GDI_ERROR)
                    {
                        AddAviFrame(m_hAVI, m_hMemoryBitmap);  
                    }
                }
                else
                {
                    if (m_hBmp != NULL)
                    {
                        AddAviFrame(m_hAVI, m_hBmp);  
                    }
                }
            }
        }
        else
        {
            // OK - here we have to do the conversion between the RAW image format from the camera to a DIB
            // We want to reuse the same conversion buffer to speed up things so we allocate it only once
            // We allocate it right here because it needs the information from the pAqImageInfo to determine
            // memory allocation size.
            if (m_pDIBBufferInfo == NULL)
            {
                m_pDIBBufferInfo = new J_tIMAGE_INFO;

                // Allocate the buffer used for conversion
                retsta = J_Image_MallocDIB(pAqImageInfo, m_pDIBBufferInfo);

                if (retsta == J_ST_SUCCESS)
                {
                    if (m_hBmp == NULL)
                    {
                        // The size might have changed so we need to free up and re-allocate
                        if (m_pBmi)
                            free(m_pBmi);

                        m_pBmi = (BITMAPINFO*) malloc(sizeof(BITMAPINFOHEADER));
        
                        // Create the DIBSection once
                        ZeroMemory(m_pBmi, sizeof(BITMAPINFOHEADER));

                        // Fill-in necessary information to the Bitmap Header
                        m_pBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                        m_pBmi->bmiHeader.biWidth = m_pDIBBufferInfo->iSizeX;
                        m_pBmi->bmiHeader.biHeight = m_pDIBBufferInfo->iSizeY;
                        m_pBmi->bmiHeader.biPlanes = 1;
                        m_pBmi->bmiHeader.biBitCount = 32; // ARGB format
                        m_pBmi->bmiHeader.biCompression = BI_RGB;
                        m_pBmi->bmiHeader.biSizeImage = m_pDIBBufferInfo->iImageSize;
                        m_pBmi->bmiHeader.biXPelsPerMeter = 75;
                        m_pBmi->bmiHeader.biYPelsPerMeter = 75;
                        m_pBmi->bmiHeader.biClrUsed = 0;
                        m_pBmi->bmiHeader.biClrImportant = 0;

                        // Create DIB section
                        m_hBmp = CreateDIBSection(NULL, m_pBmi, DIB_RGB_COLORS, (void **)&m_pBits, NULL,0);

                        // Here we cheat a litte just to avoid copying all the data after color interpolation:
                        // We simply overwrite the pointer to the allocated buffer by the pointer to the DIB bits
                        if (m_pBits != NULL)
				        {
					        m_pOldBuffer = m_pDIBBufferInfo->pImageBuffer;
                            m_pDIBBufferInfo->pImageBuffer = m_pBits;
				        }
                    }
                }
            }

            // Now we convert from RAW to DIB in order to generate AGB color image
            retsta = J_Image_FromRawToDIB(pAqImageInfo, m_pDIBBufferInfo, m_RGain, m_GGain, m_BGain);

            if (retsta == J_ST_SUCCESS)
            {
                if (m_bResizeImage)
                {
                    // Here we need to resize the image using
                    int iLines = StretchDIBits(m_hMemoryDC, 0,0, m_iResizeImageWidth, m_iResizeImageHeight, 0,0, m_pDIBBufferInfo->iSizeX, m_pDIBBufferInfo->iSizeY, m_pBits, m_pBmi, DIB_RGB_COLORS, SRCCOPY);
                    if (iLines != GDI_ERROR)
                    {
                        AddAviFrame(m_hAVI, m_hMemoryBitmap);  
                    }
                }
                else
                {
                    // Copy the image contents to the DIBSection
                    if (m_pBits != NULL)
                    {
                        // If we didnt cheat above then we have to copy the result from the color conversion to the DIB here
                        // memcpy(m_pBits, m_pDIBBufferInfo->pImageBuffer, m_pDIBBufferInfo->iImageSize);
                        if (m_hBmp != NULL)
                        {
                            AddAviFrame(m_hAVI, m_hBmp);  
                        }
                    }
                }
            }
        }
    }
}

BOOL CVCAVISampleDlg::CreateDIBSectionBitmap(HBITMAP *phBmp, void **ppBuffer, int Width, int Height, BITMAPINFO *pBmi, bool Mono8Format)
{
    HBITMAP hBmp = NULL;
    LPBYTE pBits = NULL;
    BITMAPINFO *pLocalBmi = NULL;

    if (!Mono8Format)
    {
        pLocalBmi = (BITMAPINFO *)malloc(sizeof(BITMAPINFO));

        if (pLocalBmi)
        {
            // Create the DIBSection once from a BITMAPINFO structure
            ZeroMemory(pLocalBmi, sizeof(BITMAPINFO));

            // Fill-in necessary information to the Bitmap Header
            pLocalBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            pLocalBmi->bmiHeader.biWidth = Width;
            pLocalBmi->bmiHeader.biHeight = Height;
            pLocalBmi->bmiHeader.biPlanes = 1;
            pLocalBmi->bmiHeader.biBitCount = 32; // ARGB format
            pLocalBmi->bmiHeader.biClrUsed = 0;
            pLocalBmi->bmiHeader.biCompression = BI_RGB;
            pLocalBmi->bmiHeader.biSizeImage = Width*Height*4;
            pLocalBmi->bmiHeader.biXPelsPerMeter = 75;
            pLocalBmi->bmiHeader.biYPelsPerMeter = 75;
            pLocalBmi->bmiHeader.biClrImportant = 0;

            // Create DIB section
            hBmp = CreateDIBSection(NULL, pLocalBmi, DIB_RGB_COLORS, (void **)&pBits, NULL,0);

            if (pBmi != NULL)
                memcpy(pBmi, pLocalBmi, sizeof(BITMAPINFO));

            free(pLocalBmi);
        }
    }
    else
    {
        // Create the DIBSection once from a BITMAPINFO structure
        pLocalBmi = (BITMAPINFO *)malloc(sizeof(BITMAPINFOHEADER)+(256*sizeof(RGBQUAD)));
        ZeroMemory(pLocalBmi, sizeof(BITMAPINFOHEADER)+(256*sizeof(RGBQUAD)));

        // Fill-in necessary information to the Bitmap Header
        pLocalBmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        pLocalBmi->bmiHeader.biWidth = Width;
        pLocalBmi->bmiHeader.biHeight = Height;
        pLocalBmi->bmiHeader.biPlanes = 1;
        pLocalBmi->bmiHeader.biBitCount = 8; // Mono 8
        pLocalBmi->bmiHeader.biCompression = BI_RGB;
        pLocalBmi->bmiHeader.biSizeImage = Width*Height;
        pLocalBmi->bmiHeader.biXPelsPerMeter = 75;
        pLocalBmi->bmiHeader.biYPelsPerMeter = 75;
        pLocalBmi->bmiHeader.biClrUsed = 256;
        pLocalBmi->bmiHeader.biClrImportant = 0;
        
        // Create a color palette with 256 entries for the monochrome bitmap
        for (int i=0; i<256; i++)
        {
            pLocalBmi->bmiColors[i].rgbRed = i;
            pLocalBmi->bmiColors[i].rgbGreen = i;
            pLocalBmi->bmiColors[i].rgbBlue = i;
            pLocalBmi->bmiColors[i].rgbReserved = 0;
        }

        // Create DIB section
        hBmp = CreateDIBSection(NULL, pLocalBmi, DIB_RGB_COLORS, (void **)&pBits, NULL,0);

        if (pBmi != NULL)
            memcpy(pBmi, pLocalBmi, sizeof(BITMAPINFOHEADER)+(256*sizeof(RGBQUAD)));

        free(pLocalBmi);
    }

    if (hBmp == NULL)
        return FALSE;

    *phBmp = hBmp;

    if (ppBuffer != NULL)
        *ppBuffer = pBits;

    return TRUE;
}

//--------------------------------------------------------------------------------------------------
// OnBnClickedStop
//--------------------------------------------------------------------------------------------------
void CVCAVISampleDlg::OnBnClickedStop()
{
    J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

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

    if (m_pDIBBufferInfo != NULL)
    {
		// Remember to restore old buffer pointer before caling free
		m_pDIBBufferInfo->pImageBuffer = m_pOldBuffer;

		// Free the memory
        retval = J_Image_Free(m_pDIBBufferInfo);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not free memory!"), retval);
        }

        delete m_pDIBBufferInfo;
        m_pDIBBufferInfo = NULL;
    }

    // Close the AVI file
    if (m_hAVI != NULL)
    {
        CloseAvi(m_hAVI);
        m_hAVI = NULL;
        m_bIsAviFileReady = true;
    }

    // Delete the bitmap we used for adding AVI frames
    if (m_hBmp != NULL)
    {
        DeleteObject(m_hBmp);
        m_hBmp = NULL;
    }

    // Free the bitmap info header
    if (m_pBmi != NULL)
    {
        free(m_pBmi);
        m_pBmi = NULL;
    }

    EnableControls(TRUE, FALSE);
}

//--------------------------------------------------------------------------------------------------
// InitializeControls
//--------------------------------------------------------------------------------------------------
void CVCAVISampleDlg::InitializeControls()
{
    J_STATUS_TYPE   retval;
    NODE_HANDLE hNode;
    int64_t int64Val;

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

    // Set the frame-rate edit-box
    ((CSpinButtonCtrl*) GetDlgItem(IDC_FRAME_RATE_SPIN))->SetPos32(30);
    ((CButton*)GetDlgItem(IDC_COMPRESSION_CHECK))->SetCheck(1);
    ((CButton*)GetDlgItem(IDC_OPTIMIZATION_CHECK))->SetCheck(1);

    ((CButton*)GetDlgItem(IDC_IMAGE_RESIZE_CHECK))->SetCheck((int)m_bResizeImage);
    CString str;
    str.Format(_T("%d"), m_iResizeImageWidth);
    SetDlgItemText(IDC_WIDTH_EDIT, str);
    str.Format(_T("%d"), m_iResizeImageHeight);
    SetDlgItemText(IDC_HEIGHT_EDIT, str);
}

//--------------------------------------------------------------------------------------------------
// EnableControls
//--------------------------------------------------------------------------------------------------
void CVCAVISampleDlg::EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring)
{
	GetDlgItem(IDC_START)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? TRUE : FALSE) : FALSE);
    GetDlgItem(IDC_SLIDER_WIDTH)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_WIDTH)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_HEIGHT)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_HEIGHT)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_SLIDER_GAIN)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? TRUE : FALSE);
    GetDlgItem(IDC_GAIN)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? TRUE : FALSE);
    GetDlgItem(IDC_PLAY_AVI)->EnableWindow(m_bIsAviFileReady);

    GetDlgItem(IDC_IMAGE_RESIZE_CHECK)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_WIDTH_EDIT)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_HEIGHT_EDIT)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);

    GetDlgItem(IDC_COMPRESSION_CHECK)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_OPTIMIZATION_CHECK)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_FRAME_RATE_EDIT)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
    GetDlgItem(IDC_FRAME_RATE_SPIN)->EnableWindow((bIsCameraReady && m_bEnableStreaming) ? (bIsImageAcquiring ? FALSE : TRUE) : FALSE);
}

//--------------------------------------------------------------------------------------------------
// OnHScroll
//--------------------------------------------------------------------------------------------------
void CVCAVISampleDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
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

			ldiv_t divResult = div((long)diff, ( long)inc);
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

			ldiv_t divResult = div((long)diff, ( long)inc);
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

//--------------------------------------------------------------------------------------------------
// CreateAvi:
// Call this to start the creation of the avi file.
// The period is the number of ms between each bitmap frame.
// The waveformat can be null if you're not going to add any audio,
// or if you're going to add audio from a file.
//--------------------------------------------------------------------------------------------------
HAVI CVCAVISampleDlg::CreateAvi(LPCTSTR fileName, int framePeriodeMs, const WAVEFORMATEX *wfx)
{ 
    IAVIFile *pfile;

    // Initialize the AVIFile library
    AVIFileInit();

    // Open the AVI file
    HRESULT hr = AVIFileOpen(&pfile, fileName, OF_WRITE|OF_CREATE, NULL);

    // Everything OK?
    if (hr!=AVIERR_OK) 
    {
        // Cleanup and release AVIFile library
        AVIFileExit(); 
        return NULL;
    }

    // Create and initialize structure to hold all necessary date for the AVI file
    TAviUtil *au = new TAviUtil;
    au->pfile = pfile;
    if (wfx==NULL) 
        ZeroMemory(&au->wfx,sizeof(WAVEFORMATEX)); 
    else 
        CopyMemory(&au->wfx,wfx,sizeof(WAVEFORMATEX));

    au->period = framePeriodeMs;
    au->as=0; 
    au->ps=0; 
    au->psCompressed=0;
    au->nframe=0; 
    au->nsamp=0;
    au->iserr=false;

    return (HAVI)au;
}


//--------------------------------------------------------------------------------------------------
// CloseAvi - the avi must be closed with this function.
//--------------------------------------------------------------------------------------------------
HRESULT CVCAVISampleDlg::CloseAvi(HAVI avi)
{ 
    // AVI file library open?
    if (avi==NULL) 
        return AVIERR_BADHANDLE;

    TAviUtil *au = (TAviUtil*)avi;
    if (au->as!=0) 
        AVIStreamRelease(au->as); 
    au->as=0;
    if (au->psCompressed!=0) 
        AVIStreamRelease(au->psCompressed); 
    au->psCompressed=0;
    if (au->ps!=0) 
        AVIStreamRelease(au->ps); 
    au->ps=0;
    if (au->pfile!=0) 
        AVIFileRelease(au->pfile); 
    au->pfile=0;
    AVIFileExit();
    delete au;
    return S_OK;
}

//--------------------------------------------------------------------------------------------------
// SetAviVideoCompression - allows compression of the video. If compression is desired,
// then this function must have been called before any bitmap frames had been added.
// The bitmap hbm must be a DIBSection (so that avi knows what format/size you're giving it),
// but won't actually be added to the movie.
// This function can display a dialog box to let the user choose compression. In this case,
// set ShowDialog to true and specify the parent window. If opts is non-NULL and its
// dwFlags property includes AVICOMPRESSF_VALID, then opts will be used to give initial
// values to the dialog. If opts is non-NULL then the chosen options will be placed in it.
// This function can also be used to choose a compression without a dialog box. In this
// case, set ShowDialog to false, and hparent is ignored, and the compression specified
// in 'opts' is used, and there's no need to call GotAviVideoCompression afterwards.
//--------------------------------------------------------------------------------------------------
HRESULT CVCAVISampleDlg::SetAviVideoCompression(HAVI avi, HBITMAP hbm, AVICOMPRESSOPTIONS *opts, bool ShowDialog, HWND hparent)
{ 
    // Validate parameters
    if (avi==NULL) 
        return AVIERR_BADHANDLE;
    if (hbm==NULL) 
        return AVIERR_BADPARAM;

    DIBSECTION dibs; 
    int sbm = GetObject(hbm,sizeof(dibs),&dibs);
    if (sbm!=sizeof(DIBSECTION)) 
        return AVIERR_BADPARAM;

    TAviUtil *au = (TAviUtil*)avi;
    if (au->iserr) 
        return AVIERR_ERROR;

    // Compression already defined?
    if (au->psCompressed!=0) 
        return AVIERR_COMPRESSOR;
    
    // Is the stream already created?
    if (au->ps==0) // create the stream, if it wasn't there before
    { 
        // Create and initialize the AVI Stream Info structure
        AVISTREAMINFO strhdr; 
        ZeroMemory(&strhdr,sizeof(strhdr));
        strhdr.fccType = streamtypeVIDEO;// stream type
        strhdr.fccHandler = 0; 
        strhdr.dwScale = au->period;
        strhdr.dwRate = 1000;
        strhdr.dwSuggestedBufferSize  = dibs.dsBmih.biSizeImage;
        strhdr.dwQuality = -1;
        _stprintf_s(strhdr.szName, 48, _T("JAI SDK AVI sample video stream"));
        strhdr.rcFrame.bottom = dibs.dsBmih.biHeight;
        strhdr.rcFrame.right = dibs.dsBmih.biWidth;

        // Create the AVI stream
        HRESULT hr=AVIFileCreateStream(au->pfile, &au->ps, &strhdr);
        if (hr!=AVIERR_OK) 
        {
            au->iserr=true; 
            return hr;
        }
    }

    // Is the compression not defined yet?
    if (au->psCompressed==0) // set the compression, prompting dialog if necessary
    { 
        // Create and initialize the Compression options structure
        AVICOMPRESSOPTIONS myopts; 
        ZeroMemory(&myopts,sizeof(myopts));
        AVICOMPRESSOPTIONS *aopts[1];
        if (opts!=NULL) 
            aopts[0]=opts; 
        else 
            aopts[0]=&myopts;

        // Do we want to prompt the user for the Compression?
        if (ShowDialog)
        { 
            // Retrieve any saved options from file
            BOOL res = (BOOL)AVISaveOptions(hparent, ICMF_CHOOSE_KEYFRAME | ICMF_CHOOSE_DATARATE, 1, &au->ps, aopts);
            if (!res) 
            {
                // Free resources allocated by AVISaveOptions
                AVISaveOptionsFree(1,aopts); 
                au->iserr=true; 
                return AVIERR_USERABORT;
            }
        }

        // Create Compressed Stream from an uncompressed stream and a Compression Filter
        HRESULT hr = AVIMakeCompressedStream(&au->psCompressed, au->ps, aopts[0], NULL);

        // Free resources allocated by AVISaveOptions
        AVISaveOptionsFree(1,aopts);
        if (hr != AVIERR_OK) 
        {
            au->iserr=true; 
            return hr;
        }

        // Set the Stream Format
        if (m_Use8BitMonoForOptimization)
            hr = AVIStreamSetFormat(au->psCompressed, 0, m_Mono8BitmapInfo, sizeof(m_Mono8BitmapInfo));
        else
            hr = AVIStreamSetFormat(au->psCompressed, 0, &dibs.dsBmih, dibs.dsBmih.biSize+dibs.dsBmih.biClrUsed*sizeof(RGBQUAD));

        if (hr!=AVIERR_OK) 
        {
            au->iserr=true; 
            return hr;
        }
    }
    return AVIERR_OK;
}

//--------------------------------------------------------------------------------------------------
// AddAviFrame - adds this bitmap to the avi file. hbm must point be a DIBSection.
// It is the callers responsibility to free the hbm.
//--------------------------------------------------------------------------------------------------
HRESULT CVCAVISampleDlg::AddAviFrame(HAVI avi, HBITMAP hbm)
{ 
    // Validate parameters
    if (avi==NULL) 
        return AVIERR_BADHANDLE;
    if (hbm==NULL) 
        return AVIERR_BADPARAM;

    // Get DIBSection from the handle to the Bitmap
    DIBSECTION dibs; 
    int sbm = GetObject(hbm,sizeof(dibs),&dibs);
    if (sbm!=sizeof(DIBSECTION)) 
        return AVIERR_BADPARAM;

    // Get the AVI information structure
    TAviUtil *au = (TAviUtil*)avi;
    if (au->iserr) 
        return AVIERR_ERROR;

    // Is the Stream already created?
    if (au->ps==0) // create the stream, if it wasn't there before
    { 
        // Create and initialize the AVI Stream Info structure
        AVISTREAMINFO strhdr; 
        ZeroMemory(&strhdr,sizeof(strhdr));
        strhdr.fccType = streamtypeVIDEO;// stream type
        strhdr.fccHandler = 0; 
        strhdr.dwScale = au->period;
        strhdr.dwRate = 1000;
        strhdr.dwSuggestedBufferSize  = dibs.dsBmih.biSizeImage;
        strhdr.dwQuality = -1;
        _stprintf_s(strhdr.szName, 48, _T("JAI SDK AVI sample video stream"));
        strhdr.rcFrame.bottom = dibs.dsBmih.biHeight;
        strhdr.rcFrame.right = dibs.dsBmih.biWidth;

        // Create it!
        HRESULT hr=AVIFileCreateStream(au->pfile, &au->ps, &strhdr);
        if (hr!=AVIERR_OK) 
        {
            au->iserr=true; 
            return hr;
        }
    }

    // Is the Compression Option already created?
    if (au->psCompressed==0)
    { 
        // Create an empty compression, if the user hasn't set any
        AVICOMPRESSOPTIONS opts; 
        ZeroMemory(&opts,sizeof(opts));
        HRESULT hr = AVIMakeCompressedStream(&au->psCompressed, au->ps, &opts, NULL);
        if (hr != AVIERR_OK) 
        {
            au->iserr=true; 
            return hr;
        }

        if (m_Use8BitMonoForOptimization)
        {
            // Set the Stream Format based on the Bitmap Info Header and color palette previously defined
            hr = AVIStreamSetFormat(au->psCompressed, 0, m_Mono8BitmapInfo, sizeof(m_Mono8BitmapInfo));
        }
        else
        {
            // Set the Stream Format based on the DIBSection
            hr = AVIStreamSetFormat(au->psCompressed, 0, &dibs.dsBmih, dibs.dsBmih.biSize+dibs.dsBmih.biClrUsed*sizeof(RGBQUAD));
        }

        if (hr!=AVIERR_OK) 
        {
            au->iserr=true; 
            return hr;
        }
    }

    //Now we can add the frame
    HRESULT hr = AVIStreamWrite(au->psCompressed, au->nframe, 1, dibs.dsBm.bmBits, dibs.dsBmih.biSizeImage, AVIIF_KEYFRAME, NULL, NULL);
    if (hr!=AVIERR_OK) 
    {
        au->iserr=true; 
        return hr;
    }

    // Count up the frame number
    au->nframe++; 
    return S_OK;
}


//--------------------------------------------------------------------------------------------------
// AddAviAudio - adds this chunk of audio. The format of audio was as specified in the
// wfx parameter to CreateAVI. This fails if NULL was given.
// Both return S_OK if okay, otherwise one of the AVI errors.
//--------------------------------------------------------------------------------------------------
HRESULT CVCAVISampleDlg::AddAviAudio(HAVI avi, void *dat, unsigned long numbytes)
{ 
    // Validate parameters
    if (avi==NULL) 
        return AVIERR_BADHANDLE;
    if (dat==NULL || numbytes==0) 
        return AVIERR_BADPARAM;

    TAviUtil *au = (TAviUtil*)avi;
    if (au->iserr) 
        return AVIERR_ERROR;
    if (au->wfx.nChannels==0) 
        return AVIERR_BADFORMAT;

    unsigned long numsamps = numbytes*8 / au->wfx.wBitsPerSample;
    if ((numsamps*au->wfx.wBitsPerSample/8)!=numbytes) 
        return AVIERR_BADPARAM;

    // Is the stream already created?
    if (au->as==0) // create the stream if necessary
    { 
        // Create and Initialize the AVI Stream Info structure
        AVISTREAMINFO ahdr; 
        ZeroMemory(&ahdr,sizeof(ahdr));
        ahdr.fccType=streamtypeAUDIO;
        ahdr.dwScale=au->wfx.nBlockAlign;
        ahdr.dwRate=au->wfx.nSamplesPerSec*au->wfx.nBlockAlign; 
        ahdr.dwSampleSize=au->wfx.nBlockAlign;
        ahdr.dwQuality=(DWORD)-1;

        // Create it!
        HRESULT hr = AVIFileCreateStream(au->pfile, &au->as, &ahdr);
        if (hr!=AVIERR_OK) 
        {
            au->iserr=true; 
            return hr;
        }

        // Set the WAV format
        hr = AVIStreamSetFormat(au->as,0,&au->wfx,sizeof(WAVEFORMATEX));
        if (hr!=AVIERR_OK) 
        {
            au->iserr=true; 
            return hr;
        }
    }

    // Now we can write the data
    HRESULT hr = AVIStreamWrite(au->as,au->nsamp,numsamps,dat,numbytes,0,NULL,NULL);
    if (hr!=AVIERR_OK) 
    {
        au->iserr=true; 
        return hr;
    }

    // Update the number of samples saved
    au->nsamp+=numsamps; 
    
    return S_OK;
}

//--------------------------------------------------------------------------------------------------
// AddAviWav - a convenient way to add an entire wave file to the avi.
// The wav file may be in in memory (in which case flags=SND_MEMORY)
// or a file on disk (in which case flags=SND_FILENAME).
// This function requires that either a null WAVEFORMATEX was passed to CreateAvi,
// or that the wave file now being added has the same format as was
// added earlier.
//--------------------------------------------------------------------------------------------------
HRESULT CVCAVISampleDlg::AddAviWav(HAVI avi, LPCTSTR src, DWORD flags)
{ 
    // Validate parameters
    if (avi==NULL) 
        return AVIERR_BADHANDLE;
    if (flags!=SND_MEMORY && flags!=SND_FILENAME) 
        return AVIERR_BADFLAGS;
    if (src==0) 
        return AVIERR_BADPARAM;
    TAviUtil *au = (TAviUtil*)avi;
    if (au->iserr) 
        return AVIERR_ERROR;

    char *buf=0; 
    WavChunk *wav = (WavChunk*)src;

    // Did the user specify a sound-file?
    if (flags==SND_FILENAME)
    { 
        // Open the file
        HANDLE hf=CreateFile(src,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
        if (hf==INVALID_HANDLE_VALUE) 
        {
            au->iserr=true; 
            return AVIERR_FILEOPEN;
        }
        DWORD size = GetFileSize(hf,NULL);
        buf = new char[size];
        DWORD red; 
        ReadFile(hf,buf,size,&red,NULL);
        CloseHandle(hf);
        wav = (WavChunk*)buf;
    }

    // Check that format doesn't clash
    bool badformat=false;
    if (au->wfx.nChannels==0)
    { 
        au->wfx.wFormatTag=wav->fmt.wFormatTag;
        au->wfx.cbSize=0;
        au->wfx.nAvgBytesPerSec=wav->fmt.dwAvgBytesPerSec;
        au->wfx.nBlockAlign=wav->fmt.wBlockAlign;
        au->wfx.nChannels=wav->fmt.wChannels;
        au->wfx.nSamplesPerSec=wav->fmt.dwSamplesPerSec;
        au->wfx.wBitsPerSample=wav->fmt.wBitsPerSample;
    }
    else
    { 
        if (au->wfx.wFormatTag!=wav->fmt.wFormatTag) 
            badformat=true;
        if (au->wfx.nAvgBytesPerSec!=wav->fmt.dwAvgBytesPerSec) 
            badformat=true;
        if (au->wfx.nBlockAlign!=wav->fmt.wBlockAlign) 
            badformat=true;
        if (au->wfx.nChannels!=wav->fmt.wChannels) 
            badformat=true;
        if (au->wfx.nSamplesPerSec!=wav->fmt.dwSamplesPerSec) 
            badformat=true;
        if (au->wfx.wBitsPerSample!=wav->fmt.wBitsPerSample) 
            badformat=true;
    }
    if (badformat) 
    {
        if (buf!=0) 
            delete[] buf; 
        return AVIERR_BADFORMAT;
    }

    // Is the stream already created?
    if (au->as==0) // create the stream if necessary
    { 
        // Create and Initialize the AVI Stream Info structure
        AVISTREAMINFO ahdr; 
        ZeroMemory(&ahdr,sizeof(ahdr));
        ahdr.fccType=streamtypeAUDIO;
        ahdr.dwScale=au->wfx.nBlockAlign;
        ahdr.dwRate=au->wfx.nSamplesPerSec*au->wfx.nBlockAlign; 
        ahdr.dwSampleSize=au->wfx.nBlockAlign;
        ahdr.dwQuality=(DWORD)-1;

        // Create it!
        HRESULT hr = AVIFileCreateStream(au->pfile, &au->as, &ahdr);
        if (hr!=AVIERR_OK) 
        {
            if (buf!=0) 
                delete[] buf; 
            au->iserr=true; 
            return hr;
        }

        // Set the Stream Format
        hr = AVIStreamSetFormat(au->as,0,&au->wfx,sizeof(WAVEFORMATEX));
        if (hr!=AVIERR_OK) 
        {
            if (buf!=0) delete[] buf; 
            au->iserr=true; 
            return hr;
        }
    }

    // Now we can write the data
    unsigned long numbytes = wav->dat.size;
    unsigned long numsamps = numbytes*8 / au->wfx.wBitsPerSample;
    HRESULT hr = AVIStreamWrite(au->as,au->nsamp,numsamps,wav->dat.data,numbytes,0,NULL,NULL);
    if (buf!=0) 
        delete[] buf;
    if (hr!=AVIERR_OK) 
    {
        au->iserr=true; 
        return hr;
    }

    // Update the number of samples saved
    au->nsamp+=numsamps; 
    return S_OK;
}


//--------------------------------------------------------------------------------------------------
// FormatAviMessage - given an error code, formats it as a string.
// It returns the length of the error message. If buf/len points
// to a real buffer, then it also writes as much as possible into there.
//--------------------------------------------------------------------------------------------------
unsigned int CVCAVISampleDlg::FormatAviMessage(HRESULT code, TCHAR *buf,unsigned int len)
{ 
    const TCHAR *msg=_T("unknown avi result code");
    switch (code)
    { 
    case S_OK: 
        msg=_T("Success"); 
        break;
    case AVIERR_BADFORMAT: 
        msg=_T("AVIERR_BADFORMAT: corrupt file or unrecognized format"); 
        break;
    case AVIERR_MEMORY: 
        msg=_T("AVIERR_MEMORY: insufficient memory"); 
        break;
    case AVIERR_FILEREAD: 
        msg=_T("AVIERR_FILEREAD: disk error while reading file"); 
        break;
    case AVIERR_FILEOPEN: 
        msg=_T("AVIERR_FILEOPEN: disk error while opening file"); 
        break;
    case REGDB_E_CLASSNOTREG: 
        msg=_T("REGDB_E_CLASSNOTREG: file type not recognised"); 
        break;
    case AVIERR_READONLY: 
        msg=_T("AVIERR_READONLY: file is read-only"); 
        break;
    case AVIERR_NOCOMPRESSOR: 
        msg=_T("AVIERR_NOCOMPRESSOR: a suitable compressor could not be found"); 
        break;
    case AVIERR_UNSUPPORTED: 
        msg=_T("AVIERR_UNSUPPORTED: compression is not supported for this type of data"); 
        break;
    case AVIERR_INTERNAL: 
        msg=_T("AVIERR_INTERNAL: internal error"); 
        break;
    case AVIERR_BADFLAGS: 
        msg=_T("AVIERR_BADFLAGS"); 
        break;
    case AVIERR_BADPARAM: 
        msg=_T("AVIERR_BADPARAM"); 
        break;
    case AVIERR_BADSIZE: 
        msg=_T("AVIERR_BADSIZE"); 
        break;
    case AVIERR_BADHANDLE: 
        msg=_T("AVIERR_BADHANDLE"); 
        break;
    case AVIERR_FILEWRITE: 
        msg=_T("AVIERR_FILEWRITE: disk error while writing file"); 
        break;
    case AVIERR_COMPRESSOR: 
        msg=_T("AVIERR_COMPRESSOR"); 
        break;
    case AVIERR_NODATA: 
        msg=_T("AVIERR_READONLY"); 
        break;
    case AVIERR_BUFFERTOOSMALL: 
        msg=_T("AVIERR_BUFFERTOOSMALL"); 
        break;
    case AVIERR_CANTCOMPRESS: 
        msg=_T("AVIERR_CANTCOMPRESS"); 
        break;
    case AVIERR_USERABORT: 
        msg=_T("AVIERR_USERABORT"); 
        break;
    case AVIERR_ERROR: 
        msg=_T("AVIERR_ERROR"); 
        break;
    }

    unsigned int mlen=(unsigned int)_tcslen(msg);
    if (buf==0 || len==0) 
        return mlen;
    unsigned int n=mlen; 
    if (n+1>len) 
        n=len-1;
    _tcsncpy_s(buf,len,msg,n); 
    buf[n]=0;
    return mlen;
}

void CVCAVISampleDlg::OnEnChangeFrameRateEdit()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
}

void CVCAVISampleDlg::OnBnClickedPlayAvi()
{
    ShellExecute(NULL, _T("open"), (LPCTSTR)m_AviFileName, NULL, NULL, SW_SHOWNORMAL);
}

void CVCAVISampleDlg::OnBnClickedImageResizeCheck()
{
    m_bResizeImage = (((CButton*)GetDlgItem(IDC_IMAGE_RESIZE_CHECK))->GetCheck() > 0);
}

void CVCAVISampleDlg::OnEnChangeWidthEdit()
{
    m_iResizeImageWidth = GetDlgItemInt(IDC_WIDTH_EDIT);
}

void CVCAVISampleDlg::OnEnChangeHeightEdit()
{
    m_iResizeImageHeight = GetDlgItemInt(IDC_HEIGHT_EDIT);
}

void CVCAVISampleDlg::OnBnClickedCancel()
{
	OnCancel();
}

void CVCAVISampleDlg::OnBnClickedOk()
{
	OnOK();
}
