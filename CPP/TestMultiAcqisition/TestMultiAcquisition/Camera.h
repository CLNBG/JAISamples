
#pragma once

#include <Jai_Factory.h>

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"
#define	NUM_OF_BUFFERS	        5
#define	NUM_OF_COMPEN_PARAM	    3

#define	WAIT_FOR_THREAD_START	5000

class CCamera
{
public:
	CCamera(void);
	virtual ~CCamera(void);

	static LONG		nWindowStartPosition;

	// Implementation
public:
	FACTORY_HANDLE  m_hFactory;     // Factory Handle
	CAM_HANDLE      m_hCam;         // Camera Handle
	VIEW_HANDLE     m_hView;        // View window handle
	int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID string
	int32_t         m_iStreamChannel;   // Stream channel number (default 0)
	STREAM_HANDLE   m_hDS;              // Handle to the data stream
	HANDLE          m_hStreamThread;    // Handle to the image acquisition thread
	HANDLE          m_hStreamEventStarted; // Thread used to signal when image thread has started
	HANDLE          m_hStreamEventKill; // Thread used to signal when image thread stops
	uint32_t        m_iValidBuffers;    // Number of buffers allocated to image acquisition
	uint8_t*        m_pAquBuffer[NUM_OF_BUFFERS];    // Buffers allocated to hold image data
	BUF_HANDLE      m_pAquBufferID[NUM_OF_BUFFERS];  // Handles for all the image buffers
	HANDLE	        m_hCondition;       // Condition used for getting the New Image Events
	bool            m_bEnableThread;    // Flag indicating if the image thread should run
	bool            m_bStreamStarted;   // Flag indicating that the J_Stream_StartAcquisition() has been called
	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;
	CString			CameraModelStr;
	long			m_FrameCount;
	long			m_BadFrameCount;
	BOOL			m_bDisableDisplay;
	BOOL			m_bHighPriority;
	HWND			m_hWnd;
	bool			m_bIsBayerCamera;
	BOOL			m_bUseTestPattern;

	SIZE			ViewSize;
	POINT			TopLeft;
	RECT			ViewWindowRect;
	POINT			ViewWindowPos;

	unsigned short m_iRThresh;
	unsigned short m_iGThresh;
	unsigned short m_iBThresh;
	unsigned short m_iRGain;
	unsigned short m_iGGain;
	unsigned short m_iBGain;
	BOOL m_bFullScreen;
	BOOL m_bProcessImages;
	bool m_bSave;
	J_tCOLOR_COMPEN m_ColorCompen;
	J_tIMAGE_INFO m_ImageBufferInfo;
	int m_nThreadType;

	static void ResetWindowStartPosition()
	{
		nWindowStartPosition = 0;
	}

	BOOL Start();
	void Stop(void);   // Terminate the image acquisition thread

	BOOL CreateStreamThread(CAM_HANDLE hCam, uint32_t iChannel, uint32_t iBufferSize);
	BOOL TerminateStreamThread(void);   // Terminate the image acquisition thread
	void StreamProcess(void);           // The actual image acquisition thread
	void TerminateThread(void);         
	void WaitForThreadToTerminate(void);
	void CloseThreadHandle(void);
	uint32_t PrepareBuffer(int iBufferSize);
	BOOL UnPrepareBuffer(void);
	void ShowErrorMsg(CString message, J_STATUS_TYPE error);

	BOOL OpenCamera(FACTORY_HANDLE hFactory, int iIndex, HWND hWnd=0, int nThreadType=0);

	void CloseCamera();

	void CompensateImage(J_tIMAGE_INFO * pAqImageInfo);

	void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
};
