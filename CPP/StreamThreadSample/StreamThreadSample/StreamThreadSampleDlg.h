// StreamThreadSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_PAYLOADSIZE   (int8_t*)"PayloadSize"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"
#define	NUM_OF_BUFFERS	        5

// CStreamThreadSampleDlg dialog
class CStreamThreadSampleDlg : public CDialog
{
    // Construction
public:
    CStreamThreadSampleDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_STREAMTHREADSAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
public:
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
    VIEW_HANDLE     m_hView;        // View window handle
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID string
    int32_t         m_iStreamChannel;   // Stream channel number (default 0)
    STREAM_HANDLE   m_hDS;              // Handle to the data stream
    HANDLE          m_hStreamThread;    // Handle to the image acquisition thread
    HANDLE          m_hStreamEvent;     // Thread used to signal when image thread stops
    uint32_t        m_iValidBuffers;    // Number of buffers allocated to image acquisition
    uint8_t*        m_pAquBuffer[NUM_OF_BUFFERS];    // Buffers allocated to hold image data
    BUF_HANDLE      m_pAquBufferID[NUM_OF_BUFFERS];  // Handles for all the image buffers
    HANDLE	        m_hCondition;       // Condition used for getting the New Image Events
    bool            m_bEnableThread;    // Flag indicating if the image thread should run
    bool            m_bStreamStarted;   // Flag indicating that the J_Stream_StartAcquisition() has been called
	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;

    BOOL OpenFactoryAndCamera();        // Open factory and search for cameras. Open first camera
    void CloseFactoryAndCamera();       // Close open camera and close factory to clean up
    void InitializeControls();          // Initialize control on the dialog
    void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);   // Update the control states

    BOOL CreateStreamThread(CAM_HANDLE hCam, uint32_t iChannel, uint32_t iBufferSize);
    BOOL TerminateStreamThread(void);   // Terminate the image acquisition thread
    void StreamProcess(void);           // The actual image acquisition thread
    void TerminateThread(void);         
    void WaitForThreadToTerminate(void);
    void CloseThreadHandle(void);
    uint32_t PrepareBuffer(int iBufferSize);
    BOOL UnPrepareBuffer(void);
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);

protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedStart();
    afx_msg void OnBnClickedStop();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
