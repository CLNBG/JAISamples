// HdrSampleDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define	NUM_OF_BUFFER	10
#define	VIEWCOUNT 3

// CHdrSampleDlg dialog
class CHdrSampleDlg : public CDialog
{
// Construction
public:
	CHdrSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HDRSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
    // JAI SDK object handles
    FACTORY_HANDLE  m_hFactory;                         // Factory object handle
    VIEW_HANDLE     m_hView[VIEWCOUNT];                 // View window handles
    CAM_HANDLE      m_hCamera[2];                       // Camera handles (one for each sensor)
    STREAM_HANDLE   m_hDS[2];                           // Handle to the data stream (one for each sensor)
    int8_t       m_CameraIDString[2][J_CAMERA_ID_SIZE]; // Camera ID string (one for each sensor)
    int8_t       m_CameraModelString[2][J_CAMERA_INFO_SIZE]; // Camera Model Name (one for each sensor)
    // HDR Processing variables
    float           m_DarkGain;                         // This is the multiplication factor between the darker and brighter image. This is determined analyzing a set of images
    float           m_NewDarkGain;                      // New value for the m_DarkGain entered by user
    int             m_ImageBlackLevel[2];               // This is the Black Level value for each image.
    int             m_NewImageBlackLevel[2];            // New value for the m_ImageBlackLevel entered by user
    byte            m_SaturationLevel;                  // Saturation level used by HDR algorithm.
    uint8_t         m_IsImage1Brighter;                 // Flag that indicates if image #1 is brighter than image #2
    float           m_DualSlope;                        // Dual-slope value entered by user
    bool            m_AnalysisRequested;                // Flag to request the next pair of image to be (re)analyzed
    bool            m_AnalysisOK;                       // Flag to indicate if the last analysis was successful
    int             m_ProcessingTimeValue;              // The time it took for the last call to the HDR Fuse function
    int             m_ImageMask;                        // This is a bit-mask used for making sure we have a synchronously acquired image pair
    // Image Acquisition Thread control
    HANDLE          m_hStreamThread[2];                 // Handle to the image acquisition thread (one for each sensor)
    HANDLE          m_hStreamEvent[2];                  // Thread used to signal when image thread stops (one for each sensor)
    HANDLE	        m_hNewImageCondition[2];            // Condition used for signaling that a new image has been acquired (one for each sensor)
    bool            m_bEnableThread[2];                 // Flag indicating if the image thread should run (one for each sensor)
    uint32_t        m_iValidBuffers[2];                 // Number of buffers allocated to image acquisition (one for each sensor)
    uint8_t*        m_pAquBuffer[2][NUM_OF_BUFFER];     // Buffers allocated to hold image data (one for each sensor)
    BUF_HANDLE      m_pAquBufferID[2][NUM_OF_BUFFER];   // Handles for all the image buffers (one for each sensor)
    // HDR Processing Thread control
    HANDLE          m_hHDRProcessingThread;             // Handle to the HDR processing thread
    HANDLE          m_hHDRProcessingEvent;              // Thread used to signal when HDR processing thread stops
    HANDLE	        m_hHDRProcessingCondition;          // Condition used for signaling that images are ready for processing
    bool            m_bEnableHDRProcessingThread;       // Flag indicating if the image thread should run

    J_tIMAGE_INFO	m_ImageInfo[2];                     // Image information structure to hold the two images that are being HDR processed
    J_tIMAGE_INFO   m_OutputImage;                      // Output result from the HDR processing

    CRITICAL_SECTION m_HDRProcessingCriticalSection;    // Critical section to protect access to the image to be processed from the 3 threads

    void InitializeControls();          // Initialize control on the dialog
    void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);   // Update the control states
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);
	void PrepareCamera(int iCameraIndex);
    void ReAnalyze();


    BOOL CreateStreamThread(int iCameraIndex, uint32_t iChannel, uint32_t iBufferSize);
    BOOL TerminateStreamThread(int iCameraIndex);   // Terminate the image acquisition thread
    void StreamThread(int iCameraIndex);            // The actual image acquisition thread
    void HDRProcessingThread(void);                 // The actual HDR processing thread
    void WaitForThreadToTerminate(int iCameraIndex);
    void CloseThreadHandle(int iCameraIndex);
    uint32_t PrepareBuffer(int iCameraIndex, int iBufferSize);
    BOOL UnPrepareBuffer(int iCameraIndex);
    BOOL CreateHDRProcessingThread(void);
    BOOL TerminateHDRProcessingThread(void);         
    void WaitForHDRProcessingThreadToTerminate(void);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedSearchButton();
    virtual BOOL DestroyWindow();
    afx_msg void OnBnClickedStartAcqButton();
    CStatic m_PictureControl1;
    CStatic m_PictureControl2;
    CStatic m_PictureControl3;
    afx_msg void OnBnClickedStopAcqButton();
    afx_msg void OnBnClickedOpenButton();
    afx_msg void OnBnClickedCloseButton();
    afx_msg void OnBnClickedAnalyzeButton();
    CRichEditCtrl m_CameraIdEdit;
    CString m_GainValue;
    CString m_ProcessingTime;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    int m_PixelFormatInt;
    afx_msg void OnCbnSelchangePixelformatCombo();
    BOOL m_LogOutput;
    afx_msg void OnBnClickedLogCheck();
    afx_msg void OnBnClickedOk();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    CSliderCtrl m_DualSlopeSliderCtrl;
    afx_msg void OnEnChangeBlackLevelEdit1();
    afx_msg void OnEnChangeBlackLevelEdit2();
    afx_msg void OnDeltaposGainSpin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedSaveButton();
};
