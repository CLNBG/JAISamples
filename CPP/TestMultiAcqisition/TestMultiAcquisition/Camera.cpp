#include "StdAfx.h"
#include "Camera.h"

#include <vector>
#include <VLib.h>
#include <TLI/Client.h>
#include <Jai_Factory.h>

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

//============================================================================
// Thread Process Caller
//============================================================================
DWORD ProcessCaller(CCamera* pThread)
{
	pThread->StreamProcess();

	return 0;
}

LONG CCamera::nWindowStartPosition = 0;

CCamera::CCamera(void)
{
	// Initialize all handles
	m_hFactory = NULL;
	m_hCam = NULL;
	m_hView = NULL;

	m_hStreamThread = NULL;
	m_hStreamEventStarted = NULL;
	m_hStreamEventKill = NULL;
	m_hDS = NULL;
	m_iStreamChannel = 0;
	m_hCondition = NULL;
	m_bStreamStarted = false;

	m_iWidthInc = 1;
	m_iHeightInc = 1;
	m_bEnableStreaming = false;
	CameraModelStr = "";
	m_FrameCount = 0;
	m_BadFrameCount = 0;
	m_bDisableDisplay = FALSE;
	m_bHighPriority = TRUE;
	m_hWnd = NULL;

	m_iRThresh = (250);
	m_iGThresh = (20);
	m_iBThresh = (220);
	m_iRGain = (100);
	m_iGGain = (50);
	m_iBGain = (50);
	m_bFullScreen = (FALSE);
	m_bProcessImages = (TRUE);
	m_bSave = (false);
	m_ImageBufferInfo.pImageBuffer = NULL;
	m_bIsBayerCamera = false;
}

CCamera::~CCamera(void)
{
	CloseCamera();
}

void CCamera::ShowErrorMsg(CString message, J_STATUS_TYPE error)
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
// Create Stream Thread
//--------------------------------------------------------------------------------------------------
BOOL CCamera::CreateStreamThread(CAM_HANDLE hCam, uint32_t iChannel, uint32_t iBufferSize)
{
	J_STATUS_TYPE   retval;

	// Is it already created?
	if(m_hStreamThread)
		return FALSE;

	// Open the stream channel
	if(m_hDS == NULL)
	{
		retval = J_Camera_CreateDataStream(hCam, iChannel, &m_hDS);

		if(   (retval != J_ST_SUCCESS)
			|| (m_hDS == NULL))
		{
			if (retval != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("Could not create DataStream!"), retval);
			}

			return FALSE;
		}
	}

	//Lets adjust the buffersize: Does the datastream define the size?
	size_t iStreamDefinesPayloadSize = 0;
	uint32_t iSize = sizeof(iStreamDefinesPayloadSize);
	retval = J_DataStream_GetStreamInfo(m_hDS, STREAM_INFO_DEFINES_PAYLOADSIZE, &iStreamDefinesPayloadSize, &iSize);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get STREAM_INFO_DEFINES_PAYLOADSIZE from datastream!"), retval);
		return FALSE;
	}

	//If the datastream defines the payload size, get the value
	if(1 == iStreamDefinesPayloadSize)
	{
		uint64_t iValue = 0;
		iSize = sizeof(iValue);
		retval = J_DataStream_GetStreamInfo(m_hDS, STREAM_INFO_PAYLOAD_SIZE, &iValue, &iSize);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get STREAM_INFO_DEFINES_PAYLOADSIZE from datastream!"), retval);
			return FALSE;
		}

		iBufferSize = (uint32_t)iValue;
	}

	// Prepare the frame buffers (this announces the buffers to the acquisition engine)
	if(0 == PrepareBuffer(iBufferSize))
	{
		ShowErrorMsg(CString("PrepareBuffer failed!"), retval);

		retval = J_DataStream_Close(m_hDS);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not close DataStream!"), retval);
		}
		return FALSE;
	}

	// Stream thread event created?
	if(m_hStreamEventKill == NULL)
		m_hStreamEventKill = CreateEvent(NULL, true, false, NULL);
	else
		ResetEvent(m_hStreamEventKill);

	// Set the thread execution flag
	m_bEnableThread = true;

	// Create a Stream Thread.
	if(NULL == (m_hStreamThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProcessCaller, this, NULL, NULL)))
	{
		ShowErrorMsg(CString("CreateThread failed!"), retval);
		retval = J_DataStream_Close(m_hDS);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not close DataStream!"), retval);
		}
		return FALSE;
	}

	return TRUE;
}

//==============================================================////
// Terminate Stream Thread
//==============================================================////
BOOL CCamera::TerminateStreamThread(void)
{
	J_STATUS_TYPE   retval;

	// Is the data stream opened?
	if(m_hDS == NULL)
		return FALSE;

	// Reset the thread execution flag.
	m_bEnableThread = false;

	// Signal the image thread to stop faster
	if (m_hCondition)
	{
		retval = J_Event_ExitCondition(m_hCondition);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not exit Condition!"), retval);
		}
	}

	// Stop the image acquisition engine
	retval = J_DataStream_StopAcquisition(m_hDS, ACQ_STOP_FLAG_KILL);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not stop DataStream acquisition!"), retval);
	}

	// Mark stream acquisition as stopped
	m_bStreamStarted = false;

	// Wait for the thread to end
	WaitForThreadToTerminate();

	// UnPrepare Buffers (this removed the buffers from the acquisition engine and frees buffers)
	UnPrepareBuffer();

	// Close Stream
	if(m_hDS)
	{
		retval = J_DataStream_Close(m_hDS);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not close DataStream!"), retval);
		}
		m_hDS = NULL;
	}

	return TRUE;
}

//==============================================================////
// Terminate image acquisition thread
//==============================================================////
void CCamera::TerminateThread(void)
{
	SetEvent(m_hStreamEventKill);
}

//==============================================================////
// Wait for thread to terminate
//==============================================================////
void CCamera::WaitForThreadToTerminate(void)
{
	WaitForSingleObject(m_hStreamEventKill, INFINITE);

	// Close the thread handle and stream event handle
	CloseThreadHandle();
}

//==============================================================////
// Close handles and stream
//==============================================================////
void CCamera::CloseThreadHandle(void)
{
	if(m_hStreamThread)
	{
		CloseHandle(m_hStreamThread);
		m_hStreamThread = NULL;
	}

	if(m_hStreamEventStarted)
	{
		CloseHandle(m_hStreamEventStarted);
		m_hStreamEventStarted = NULL;
	}

	if(m_hStreamEventKill)
	{
		CloseHandle(m_hStreamEventKill);
		m_hStreamEventKill = NULL;
	}
}

//==============================================================////
// Prepare frame buffers
//==============================================================////
uint32_t CCamera::PrepareBuffer(int iBufferSize)
{
	J_STATUS_TYPE	iResult = J_ST_SUCCESS;
	int			i;

	m_iValidBuffers = 0;

	for(i = 0 ; i < NUM_OF_BUFFERS ; i++)
	{
		// Make the buffer for one frame. 
		m_pAquBuffer[i] = new uint8_t[iBufferSize];

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
BOOL CCamera::UnPrepareBuffer(void)
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

		delete m_pAquBuffer[i];
		m_pAquBuffer[i] = NULL;
		m_pAquBufferID[i] = 0;
	}

	m_iValidBuffers = 0;

	return TRUE;
}

BOOL CCamera::OpenCamera(FACTORY_HANDLE hFactory, int iIndex, HWND hWnd, int nThreadType)
{
	J_STATUS_TYPE   retval;
	uint32_t        iSize;

	m_hFactory = hFactory;

	m_hWnd = hWnd;

	m_nThreadType = nThreadType;

	m_bUseTestPattern = FALSE;

	// Get camera ID
	iSize = (uint32_t)sizeof(m_sCameraId);
	retval = J_Factory_GetCameraIDByIndex(m_hFactory, iIndex, m_sCameraId, &iSize);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get the camera ID!"), retval);
		return FALSE;
	}
	TRACE("Camera ID: %s\n", m_sCameraId);

	if(0 == strncmp("TL=>GevTL , INT=>SD", (char*)m_sCameraId, 19))
	{ // SD
		return FALSE;
	}

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
		return FALSE;
	}

	m_bEnableStreaming = true;

	//To handle frame grabbers with "virtual hardware", let's check for the presence of recommended nodes:
	NODE_HANDLE hNode = 0;
	retval = J_Camera_GetNodeByName((CAM_HANDLE)m_hCam, (int8_t *)"DeviceControl", &hNode);
	if(J_ST_SUCCESS != retval || NULL == hNode)
	{
		return FALSE;
	}

	int8_t   sCameraModel[J_CAMERA_INFO_SIZE];
	uint32_t iCameraInfoSize = J_CAMERA_INFO_SIZE;

	//get model name
	J_STATUS_TYPE status = J_Factory_GetCameraInfo(m_hFactory, m_sCameraId, CAM_INFO_MODELNAME, sCameraModel, &iCameraInfoSize);
	if(J_ST_SUCCESS == status) 
	{
		CameraModelStr = CString(sCameraModel);
	}
	else
	{
		//ShowErrorMsg(CString("Could not get camera ModelName info!"), status);
		CString token (_T("DEV=>"));
		int tokenSize = token.GetLength();
		CString s ((char*)m_sCameraId);
		int pos = s.Find(token);
		if(-1 != pos)
		{
			CameraModelStr = s.Mid(pos+tokenSize);
		}
		else
		{
			CameraModelStr = CString(m_sCameraId);
		}
	}

	return TRUE;
}

void CCamera::CloseCamera()
{
	if (m_hCam)
	{
		// Close camera
		J_STATUS_TYPE retval = J_Camera_Close(m_hCam);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not close the camera!"), retval);
		}
		m_hCam = NULL;
		TRACE("Closed camera\n");

		// Free the Image conversion buffer if it has been allocated
		if(m_ImageBufferInfo.pImageBuffer != NULL)
		{
			J_Image_Free(&m_ImageBufferInfo);
			m_ImageBufferInfo.pImageBuffer = NULL;
		}
	}
}

BOOL CCamera::Start()
{
	J_STATUS_TYPE   retval;
	int64_t int64Val;

	SIZE	ViewSize;
	POINT	TopLeft;
	RECT	ViewWindowRect;
	POINT	ViewWindowPos;

	if(!m_bEnableStreaming)
	{
		ShowErrorMsg(CString("Streaming not enabled on this device."), 0);
		return FALSE;
	}

	// Get Width from the camera based on GenICam name
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_WIDTH, &int64Val);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get Width value!"), retval);
		return FALSE;
	}
	ViewSize.cx = (LONG)int64Val;     // Set window size cx

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_WIDTH, int64Val, &m_sCameraId[0]);

	// Get Height from the camera
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_HEIGHT, &int64Val);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get Height value!"), retval);
		return FALSE;
	}
	ViewSize.cy = (LONG)int64Val;     // Set window size cy

	//Set frame grabber dimension, if applicable
	SetFramegrabberValue(m_hCam, NODE_NAME_HEIGHT, int64Val, &m_sCameraId[0]);

	// Get the pixelformat from the camera
	int64_t pixelFormat = 0;
	uint64_t jaiPixelFormat = 0;
	retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Unable to get PixelFormat value!"), retval);
		return FALSE;
	}
	J_Image_Get_PixelFormat(m_hCam, pixelFormat, &jaiPixelFormat);

	//Set frame grabber pixel format, if applicable
	SetFramegrabberPixelFormat(m_hCam, NODE_NAME_PIXELFORMAT, pixelFormat, &m_sCameraId[0]);

	// Calculate number of bits (not bytes) per pixel using macro
	int bpp = J_BitsPerPixel(jaiPixelFormat);

	// Open view window
	if(FALSE == m_bDisableDisplay)
	{
		// Set window position
		TopLeft.x = 100 + nWindowStartPosition;
		TopLeft.y = 50;

		// Set window position
		ViewWindowPos.x = 100 + nWindowStartPosition;
		ViewWindowPos.y = 50;

		ViewWindowRect.left = 100;
		ViewWindowRect.top = 50;
		ViewWindowRect.right = 100 + 100; //ViewSize.cx;
		ViewWindowRect.bottom = 50 + 100; // ViewSize.cy;

		LONG nWidth = ViewWindowRect.right - ViewWindowRect.left;
		ViewWindowRect.left += nWindowStartPosition;

		nWindowStartPosition += (nWidth + 60);

		if(0 == m_hWnd)
		{
			retval = J_Image_OpenViewWindow(_T("Image View Window"), &TopLeft, &ViewSize, &m_hView);
		}
		else
		{
			retval = J_Image_OpenViewWindowEx(J_IVW_OVERLAPPED_STRETCH, _T("Image View Window"), &ViewWindowRect, &ViewSize, m_hWnd, &m_hView);
		}

		if (retval != J_ST_SUCCESS) {
			ShowErrorMsg(CString("Could not open view window!"), retval);
			return FALSE;
		}
		TRACE("Opening view window succeeded\n");
	}
	else
	{
		m_hView = NULL;
	}

	m_FrameCount = 0;
	m_BadFrameCount = 0;

	//Enable or disable test pattern
	do
	{
		NODE_HANDLE hNodes = 0;
		int status = J_ST_SUCCESS;

		status = J_Camera_GetNodeByName((CAM_HANDLE)m_hCam, (int8_t *)"TestImageSelector", &hNodes);
		if(J_ST_SUCCESS != status)
		{
			break;
		}

		if(false == m_bUseTestPattern)
		{
			status = J_Node_SetValueString(hNodes, false, (int8_t *)"Off");
		}
		else
		{
			uint32_t num = 0;
			status = J_Node_GetNumOfEnumEntries(hNodes, &num);

			if(0 != hNodes && num > 0)
			{
				for(int i=num-1; i>=0; i--)
				{
					NODE_HANDLE hEnumNode;
					status = J_Node_GetEnumEntryByIndex(hNodes, i, &hEnumNode);
					if(J_ST_SUCCESS != status)
					{
						break;
					}

					//Get the name in question
					uint32_t iSize = 80;
					std::vector<int8_t> vecName;
					vecName.resize(iSize);
					int8_t* pBuffer = &vecName[0];

					status = J_Node_GetName(hEnumNode, pBuffer, &iSize);
					//status = J_Node_GetDisplayName(hEnumNode, pBuffer, &iSize);
					if(J_ST_SUCCESS != status)
					{
						break;
					}

					std::string strName((char*)pBuffer);

					//The name may have a prefix of "EnumEntry_PixelFormat_", so remove it.
					size_t pos = strName.rfind("_");
					if(std::string::npos != pos && pos+1 < strName.length())
					{
						std::string strPrefix = strName.substr(0, pos+1);

						if(0 == strPrefix.compare("EnumEntry_TestImageSelector_"))
						{
							strName = strName.substr(pos+1);
						}
					}

					status = J_Node_SetValueString(hNodes, false, (int8_t *)strName.c_str());
					if(J_ST_SUCCESS == status)
					{
						break;
					}
				
				}//for(int i=num-1; i>=0; i--)
			
			}//if(0 != hNodes && num > 0)
		}

	}while(false);//Enable or disable test pattern

	//Check to see if this is a bayer pixel format
	{
		m_bIsBayerCamera = false;
		J_STATUS_TYPE   retval;
		NODE_HANDLE hNode;

		if (m_hCam)
		{
			// Here we want to populate the ComboBox with the pixelformats from the camera
			retval = J_Camera_GetNodeByName(m_hCam, (int8_t*)"PixelFormat", &hNode);
			if (retval == J_ST_SUCCESS)
			{
				int8_t buffer[100];
				uint32_t size = sizeof(buffer);
				retval = J_Node_GetValueString(hNode, false, buffer, &size);
				if (retval == J_ST_SUCCESS)
				{
					CString stValuestring((char*)buffer);
					stValuestring.MakeUpper();
					if (stValuestring.Find(_T("BAY"))!=-1)
						m_bIsBayerCamera = true;
				}
			}
			else
			{
				OutputDebugString("Unable to get PixelFormat node handle!");
			}
		}

	}//Check to see if this is a bayer pixel format

	// Start Stream thread event created?
	if(m_hStreamEventStarted == NULL)
		m_hStreamEventStarted = CreateEvent(NULL, true, false, NULL);
	else
		ResetEvent(m_hStreamEventStarted);

	if(0 == m_nThreadType)
	{
		// Create image acquisition thread (this allocates buffers)
		CreateStreamThread(m_hCam, 0, (ViewSize.cx * ViewSize.cy * bpp)/8);

		// Wait for the stream thread to start the acquisition
		uint32_t dwWait = WaitForSingleObject(m_hStreamEventStarted, WAIT_FOR_THREAD_START);
		if(WAIT_OBJECT_0 != dwWait)
		{
			ShowErrorMsg(CString("Could not Start Acquisition thread!"), dwWait);
			return FALSE;
		}
	}
	else
	{
		// Open stream
		retval = J_Image_OpenStream(m_hCam
									, 0
									, reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this)
									, reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&CCamera::StreamCBFunc)
									, &m_hStreamThread
									, (ViewSize.cx*ViewSize.cy*bpp)/8
									);
		if (retval != J_ST_SUCCESS) 
		{
			ShowErrorMsg(CString("Could not open stream!"), retval);
			return FALSE;
		}

		// Mark stream acquisition as started
		m_bStreamStarted = true;
	}

	TRACE("Opening stream succeeded\n");

	// Start Acquisition
	retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTART);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not Start Acquisition!"), retval);
		return FALSE;
	}

	return TRUE;
}

void CCamera::Stop()
{
	J_STATUS_TYPE retval;

	if(!m_bEnableStreaming)
	{
		return;
	}

	// Stop Acquisition
	if (m_hCam && m_bStreamStarted) 
	{
		retval = J_Camera_ExecuteCommand(m_hCam, NODE_NAME_ACQSTOP);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not Stop Acquisition!"), retval);
		}
	}

	if(0 == m_nThreadType)
	{
		// Close stream (this frees all allocated buffers)
		TerminateStreamThread();
		TRACE("Closed stream\n");
	}
	else
	{
		// Close Stream
		if(m_hStreamThread)
		{
			// Close stream
			retval = J_Image_CloseStream(m_hStreamThread);
			if (retval != J_ST_SUCCESS)
			{
				ShowErrorMsg(CString("Could not close Stream!"), retval);
			}
			m_hStreamThread = NULL;
			TRACE("Closed stream\n");
		}

		// Mark stream acquisition as stopped
		m_bStreamStarted = false;
	}

	// View window opened?
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
}

//==============================================================////
// Stream Processing Function
//==============================================================////
void CCamera::StreamProcess(void)
{
	J_STATUS_TYPE	iResult;
	uint32_t        iSize;
	BUF_HANDLE	    iBufferID;
	uint64_t	    iQueued = 0;
	uint64_t	    iAwait = 0;

	if(m_bHighPriority)
	{
		SetThreadPriority(m_hStreamThread, THREAD_PRIORITY_ABOVE_NORMAL);
	}

	// Create structure to be used for image display
	J_tIMAGE_INFO	tAqImageInfo = {0, 0, 0, 0, NULL, 0, 0, 0, 0, 0, 0};

	J_COND_WAIT_RESULT	WaitResult;

	// Create the condition used for signaling the new image event
	iResult = J_Event_CreateCondition(&m_hCondition);

	EVT_HANDLE	hEvent;					// Buffer event handle
	EVENT_NEW_BUFFER_DATA eventData;	// Strut for EventGetData

	// Register the event with the acquisition engine
	uint32_t iAcquisitionFlag = ACQ_START_NEXT_IMAGE;

	iResult = J_DataStream_RegisterEvent(m_hDS, EVENT_NEW_BUFFER, m_hCondition, &hEvent); 

	// Start image acquisition
	iResult = J_DataStream_StartAcquisition(m_hDS, iAcquisitionFlag, ULLONG_MAX);

	// Mark stream acquisition as started
	m_bStreamStarted = true;
	assert(m_hStreamEventStarted);
	SetEvent(m_hStreamEventStarted);

	// Loop of Stream Processing
	OutputDebugString(_T(">>> Start Stream Process Loop.\n"));

	const uint64_t iTimeout = 1000;
	uint64_t jaiPixelFormat = 0;
	uint64_t defaultPixelFormat = 0;

	// Try to setup a default pixel format.
	// Get the pixelformat from the camera
	int64_t pixelFormat = 0;
	J_STATUS_TYPE retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
	if (J_ST_SUCCESS == retval)
	{
		retval = J_Image_Get_PixelFormat(m_hCam, pixelFormat, &defaultPixelFormat);
	}

	DEV_HANDLE hDev = NULL; //If a frame grabber exists, it is at the GenTL "local device layer".
	retval = J_Camera_GetLocalDeviceHandle(m_hCam, &hDev);
	if(J_ST_SUCCESS == retval || NULL != hDev)
	{
		NODE_HANDLE hLocalDeviceNode = 0;
		retval = J_Camera_GetNodeByName(hDev, (int8_t *)"PixelFormat", &hLocalDeviceNode);
		if(J_ST_SUCCESS == retval || NULL != hLocalDeviceNode)
		{
			pixelFormat = 0;
			retval = J_Camera_GetValueInt64(m_hCam, NODE_NAME_PIXELFORMAT, &pixelFormat);
			if (J_ST_SUCCESS == retval)
			{
				uint64_t localPixelFormat = 0;
				retval = J_Image_Get_PixelFormat(m_hCam, pixelFormat, &localPixelFormat);
				if (J_ST_SUCCESS == retval)
				{
					defaultPixelFormat = localPixelFormat;
				}
			}
		}
	}

	while(m_bEnableThread)
	{
		// Wait for Buffer event (or kill event)
		iResult = J_Event_WaitForCondition(m_hCondition, iTimeout, &WaitResult);
		if(J_ST_SUCCESS != iResult)
		{
			OutputDebugString(_T("J_Event_WaitForCondition Error.\n"));
		}

		// Did we get a new buffer event?
		if(J_COND_WAIT_SIGNAL == WaitResult)
		{
			// Get the Buffer Handle from the event
			iSize = (uint32_t)sizeof(EVENT_NEW_BUFFER_DATA);

			iResult = J_Event_GetData(hEvent, &eventData,  &iSize);

			iBufferID = eventData.BufferHandle;

			// Did we receive the event data?
			if (iResult == J_ST_SUCCESS)
			{
				InterlockedIncrement(&m_FrameCount);

				// Fill in structure for image display

				// Get the pointer to the frame buffer.
				uint64_t infoValue = 0;
				iSize = (uint32_t)sizeof (void *);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_BASE, &(infoValue), &iSize);
				tAqImageInfo.pImageBuffer = (uint8_t*)infoValue;
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_BASE.\n"));
					goto Requeue;
				}

				// Get the effective data size.
				iSize = sizeof (size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_SIZE	, &(infoValue), &iSize);
				tAqImageInfo.iImageSize = (uint32_t)infoValue;
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_SIZE.\n"));
					goto Requeue;
				}

				// Get block ID.
				iSize = sizeof (uint64_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_FRAMEID, &(infoValue), &iSize);
				tAqImageInfo.iBlockId = infoValue;

				// Get Frame Width.
				iSize = sizeof (size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_WIDTH	, &(infoValue), &iSize);
				tAqImageInfo.iSizeX = (uint32_t)infoValue;
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_WIDTH.\n"));
					goto Requeue;
				}

				// Get Frame Height.
				iSize = sizeof (size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_HEIGHT	, &(infoValue), &iSize);
				tAqImageInfo.iSizeY = (uint32_t)infoValue;
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_HEIGHT.\n"));
					goto Requeue;
				}

				// Get Pixel Format Type.
				static bool bCheckPixelFormat = true;

				if(bCheckPixelFormat)
				{
					iSize = sizeof (uint64_t);
					iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_PIXELTYPE, &(infoValue), &iSize);
					if(GC_ERR_SUCCESS != iResult)
					{
						tAqImageInfo.iPixelType = jaiPixelFormat;
						bCheckPixelFormat = false;
					}

					//Convert the camera's pixel format value to one understood by the JAI SDK.
					iResult = J_Image_Get_PixelFormat(m_hCam, infoValue, &jaiPixelFormat);
					if(GC_ERR_SUCCESS == iResult)
					{
						tAqImageInfo.iPixelType = jaiPixelFormat;
					}
					else
					{
						OutputDebugString(_T("Error with J_Image_Get_PixelFormat in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE - assuming GVSP_PIX_MONO8.\n"));
						tAqImageInfo.iPixelType = defaultPixelFormat;
						bCheckPixelFormat = false;
					}
				}
				else
				{
					tAqImageInfo.iPixelType = defaultPixelFormat;
				}

				// Get Timestamp.
				iSize = sizeof (infoValue);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_TIMESTAMP, &(infoValue), &iSize);
				tAqImageInfo.iTimeStamp = infoValue;
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_TIMESTAMP.\n"));
				}

				// Get # of missing packets in frame.
				iSize = (uint32_t)sizeof (uint32_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_NUM_PACKETS_MISSING, &(tAqImageInfo.iMissingPackets), &iSize);                

				// Initialize number of valid buffers announced
				tAqImageInfo.iAnnouncedBuffers = m_iValidBuffers;

				// Get # of buffers queued
				iSize = sizeof (infoValue);
				iResult = J_DataStream_GetStreamInfo(m_hDS, STREAM_INFO_CMD_NUMBER_OF_FRAMES_QUEUED, &iQueued, &iSize);
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> STREAM_INFO_NUM_QUEUED.\n"));
				}
				tAqImageInfo.iQueuedBuffers = static_cast<uint32_t>(iQueued & 0x0ffffffffL);

				// Get X-offset.
				iSize = sizeof (size_t);
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_XOFFSET, &infoValue, &iSize);
				tAqImageInfo.iOffsetX = (uint32_t)(infoValue);
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_XOFFSET.\n"));
				}

				// Get Y-offset.
				iSize = sizeof (size_t);
				infoValue = 0;
				iResult = J_DataStream_GetBufferInfo(m_hDS, iBufferID, BUFFER_INFO_YOFFSET, &infoValue, &iSize);
				tAqImageInfo.iOffsetY = (uint32_t)(infoValue);
				if(GC_ERR_SUCCESS != iResult)
				{
					OutputDebugString(_T("Error with J_DataStream_GetBufferInfo in CStreamThread::StreamProcess ==> BUFFER_INFO_YOFFSET.\n"));
				}

				if(m_bEnableThread)
				{
					// Get # of frames awaiting delivery
					iSize = sizeof(size_t);
					iResult = J_DataStream_GetStreamInfo(m_hDS, STREAM_INFO_CMD_NUMBER_OF_FRAMES_AWAIT_DELIVERY, &iAwait, &iSize);

					// Display image
					if(m_hView && iAwait < 2L)
					{
						if(m_bProcessImages && m_bIsBayerCamera)
						{
							CompensateImage(&tAqImageInfo);
						}
						else
						{
							// Shows image
							J_Image_ShowImage(m_hView, &tAqImageInfo);
						}
					}
				}
Requeue:

				// Queue This Buffer Again for reuse in acquisition engine
				iResult = J_DataStream_QueueBuffer(m_hDS, iBufferID);

			}//if (iResult == J_ST_SUCCESS)
			else
			{
				OutputDebugString("EventGetData error.\n");
				InterlockedIncrement(&m_BadFrameCount);
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
				InterlockedIncrement(&m_BadFrameCount);
				OutputDebugString("J_COND_WAIT_TIMEOUT\n");
				break;
				// Error event
			case	J_COND_WAIT_ERROR:
				iResult = 3;
				InterlockedIncrement(&m_BadFrameCount);
				OutputDebugString("J_COND_WAIT_ERROR\n");
				break;
				// Unknown?
			default:
				iResult = 4;
				InterlockedIncrement(&m_BadFrameCount);
				OutputDebugString("UNKNOWN WAIT_ERROR\n");
				break;
			}
		}
	}
	OutputDebugString(_T(">>> Terminated Stream Process Loop.\n"));

	// Unregister new buffer event with acquisition engine
	iResult = J_DataStream_UnRegisterEvent(m_hDS, EVENT_NEW_BUFFER); 

	// Free the event object
	if (hEvent != NULL)
	{
		iResult = J_Event_Close(hEvent);
		hEvent = NULL;
	}

	// Terminate the thread.
	TerminateThread();

	// Free the Condition
	if (m_hCondition != NULL)
	{
		iResult = J_Event_CloseCondition(m_hCondition);
		m_hCondition = NULL;
	}
}

void CCamera::CompensateImage(J_tIMAGE_INFO * pAqImageInfo)
{
	if(m_hView)
	{
		//Convert the camera's pixel format value to one understood by the JAI SDK.
		uint64_t currentPixelFormat = pAqImageInfo->iPixelType;
		uint64_t jaiPixelFormat = 0;
		J_STATUS_TYPE iResult = J_Image_Get_PixelFormat(m_hCam, currentPixelFormat, &jaiPixelFormat);

		pAqImageInfo->iPixelType = jaiPixelFormat;

		if(GC_ERR_SUCCESS != iResult)
		{
			OutputDebugString(_T("Error with J_Image_Get_PixelFormat in CStreamThread::StreamProcess ==> BUFFER_INFO_PIXELTYPE.\n"));
			return;
		}

		// We need to allocate the conversion buffer once
		if(m_ImageBufferInfo.pImageBuffer == NULL)
		{
			iResult = J_Image_Malloc(pAqImageInfo, &m_ImageBufferInfo);
			if(GC_ERR_SUCCESS != iResult)
			{
				OutputDebugString(_T("Error with J_Image_Malloc in CStreamThread::StreamProcess.\n"));
				return;
			}
		}

		// Converts from RAW to internal image before applying the Red-compensation algorithm.
		iResult = J_Image_FromRawToImageEx(pAqImageInfo, &m_ImageBufferInfo, BAYER_STANDARD_MULTI);
		if(GC_ERR_SUCCESS != iResult)
		{
			OutputDebugString(_T("Error with J_Image_FromRawToImageEx in CStreamThread::StreamProcess.\n"));
			return;
		}

		if(m_bFullScreen)
		{	
			// Full Screen
			m_ColorCompen.NumOfROI = 1;
			m_ColorCompen.CompenParams[0].RectOfROI.left = 0;
			m_ColorCompen.CompenParams[0].RectOfROI.right = m_ImageBufferInfo.iSizeX - 1;
			m_ColorCompen.CompenParams[0].RectOfROI.top = 0;
			m_ColorCompen.CompenParams[0].RectOfROI.bottom = m_ImageBufferInfo.iSizeY - 1;

			// Threshold
			m_ColorCompen.CompenParams[0].RThreshold = m_iRThresh;
			m_ColorCompen.CompenParams[0].GThreshold = m_iGThresh;
			m_ColorCompen.CompenParams[0].BThreshold = m_iBThresh;

			// Compensation gain
			m_ColorCompen.CompenParams[0].RGain = 4096 * m_iRGain / 100;
			m_ColorCompen.CompenParams[0].GGain = 4096 * m_iGGain / 100;
			m_ColorCompen.CompenParams[0].BGain = 4096 * m_iBGain / 100;
		}
		else
		{	// Uses 3 ROI(windows)
			for(int i = 0 ; i < NUM_OF_COMPEN_PARAM ; i++)
			{
				// ROI
				m_ColorCompen.CompenParams[i].RectOfROI.left = m_ImageBufferInfo.iSizeX * i * 2 / 5;
				m_ColorCompen.CompenParams[i].RectOfROI.right = m_ImageBufferInfo.iSizeX * (i * 2 + 1) / 5 - 1;
				m_ColorCompen.CompenParams[i].RectOfROI.top = m_ImageBufferInfo.iSizeY * 2 / 5;
				m_ColorCompen.CompenParams[i].RectOfROI.bottom = m_ImageBufferInfo.iSizeY * 3 / 5 - 1;

				// Threshold
				m_ColorCompen.CompenParams[i].RThreshold = m_iRThresh;
				m_ColorCompen.CompenParams[i].GThreshold = m_iGThresh;
				m_ColorCompen.CompenParams[i].BThreshold = m_iBThresh;

				// Compensation gain
				m_ColorCompen.CompenParams[i].RGain = 4096 * m_iRGain / 100;	// 1.0 : 4096
				m_ColorCompen.CompenParams[i].GGain = 4096 * m_iGGain / 100;	// 1.0 : 4096
				m_ColorCompen.CompenParams[i].BGain = 4096 * m_iBGain / 100;	// 1.0 : 4096
			}
			m_ColorCompen.NumOfROI = NUM_OF_COMPEN_PARAM;
		}

		// Perform the Green-compensation
		J_Image_Processing(&m_ImageBufferInfo, GREEN_COMPENSATION, reinterpret_cast<void *>(&m_ColorCompen));

		// Shows image
		J_Image_ShowImage(m_hView, &m_ImageBufferInfo);
	}
}
//--------------------------------------------------------------------------------------------------
// StreamCBFunc
//--------------------------------------------------------------------------------------------------
void CCamera::StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo)
{
	if(pAqImageInfo)
	{
		if(m_hView)
		{
			if(m_bProcessImages && m_bIsBayerCamera)
			{
				CompensateImage(pAqImageInfo);
			}
			else
			{
				// Shows image
				J_Image_ShowImage(m_hView, pAqImageInfo);
			}
		}

		InterlockedIncrement(&m_FrameCount);
	}
	else
	{
		InterlockedIncrement(&m_BadFrameCount);
	}
}
