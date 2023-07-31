// HdrSequenceSampleDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"
#define	NUM_OF_BUFFER	        10
#define	VIEWCOUNT               3

// CHdrSequenceSampleDlg dialog
class CHdrSequenceSampleDlg : public CDialog
{
// Construction
public:
	CHdrSequenceSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_HDRSEQUENCESAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
    FACTORY_HANDLE  m_hFactory;
    CAM_HANDLE      m_hCamera;
    VIEW_HANDLE     m_hView[VIEWCOUNT];             // View window handles
    STREAM_HANDLE   m_hDS;                          // Handle to the data stream
    int32_t         m_iStreamChannel;               // Stream channel number (default 0)
    HANDLE          m_hStreamThread;                // Handle to the image acquisition thread
    HANDLE          m_hStreamEvent;                 // Thread used to signal when image thread stops
    uint32_t        m_iValidBuffers;                // Number of buffers allocated to image acquisition
    uint8_t*        m_pAquBuffer[NUM_OF_BUFFER];    // Buffers allocated to hold image data
    BUF_HANDLE      m_pAquBufferID[NUM_OF_BUFFER];  // Handles for all the image buffers
    HANDLE	        m_hNewImageCondition;           // Condition used for signaling that a new image has been acquired
    bool            m_bEnableThread;                // Flag indicating if the image thread should run
    int8_t          m_CameraIDString[J_CAMERA_ID_SIZE];
    int8_t          m_CameraModelString[J_CAMERA_INFO_SIZE];
    long            m_PixelCount;
    float           m_DarkGain;
    float           m_NewDarkGain;
    float           m_DualSlope;
    int             m_Image1BlackLevel;
    int             m_Image2BlackLevel;
    int             m_NewImage1BlackLevel;
    int             m_NewImage2BlackLevel;
    byte            m_SaturationLevel;
    uint8_t         m_IsImage1Brighter;
    bool            m_AnalysisRequested;
    bool            m_AnalysisOK;
    int             m_ProcessingTimeValue;
    double          m_RGain;
    double          m_BGain;

    // HDR Processing Thread control
    HANDLE          m_hHDRProcessingThread;             // Handle to the HDR processing thread
    HANDLE          m_hHDRProcessingEvent;              // Thread used to signal when HDR processing thread stops
    HANDLE	        m_hHDRProcessingCondition;          // Condition used for signaling that images are ready for processing
    bool            m_bEnableHDRProcessingThread;       // Flag indicating if the image thread should run
    int             m_ImageMask;                        // This is a bit-mask used for making sure we have a synchronously acquired image pair

    CRITICAL_SECTION m_HDRProcessingCriticalSection;    // Critical section to protect access to the image to be processed from the 3 threads

    J_tIMAGE_INFO	m_Image1Info;
    J_tIMAGE_INFO	m_Image2Info;
    J_tIMAGE_INFO   m_OutputImage;

    BOOL OpenFactoryAndCamera();        // Open factory and search for cameras. Open first camera
    void CloseFactoryAndCamera();       // Close open camera and close factory to clean up
    void InitializeControls();          // Initialize control on the dialog
    void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);   // Update the control states
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);
	void PrepareSwTrigSetting();
	void PrepareContinuousSetting();
    void ReAnalyze();


    BOOL CreateStreamThread(CAM_HANDLE hCam, uint32_t iChannel, uint32_t iBufferSize);
    BOOL TerminateStreamThread(void);   // Terminate the image acquisition thread
    void StreamProcess(void);           // The actual image acquisition thread
    void TerminateThread(void);         
    void WaitForThreadToTerminate(void);
    void CloseThreadHandle(void);
    uint32_t PrepareBuffer(int iBufferSize);
    BOOL UnPrepareBuffer(void);
    BOOL TerminateHDRProcessingThread(void);
    BOOL CreateHDRProcessingThread(void);
    void HDRProcessingThread(void);
    void WaitForHDRProcessingThreadToTerminate();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void ResizeChildWindow(CStatic& PictureControl);

public:
    afx_msg void OnBnClickedSearchButton();
    virtual BOOL DestroyWindow();
    afx_msg void OnBnClickedStartAcqButton();
    CStatic m_PictureControl1;
    CStatic m_PictureControl2;
    CStatic m_PictureControl3;
    afx_msg void OnBnClickedStopAcqButton();
    afx_msg void OnBnClickedTrigButton();
    afx_msg void OnBnClickedOpenButton();
    afx_msg void OnBnClickedCloseButton();
    afx_msg void OnBnClickedAnalyzeButton();
    CRichEditCtrl m_CameraIdEdit;
    CString m_GainValue;
    CString m_ProcessingTime;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    int m_PixelFormatInt;
    int64_t m_PixelFormatValues[10];
    afx_msg void OnCbnSelchangePixelformatCombo();
    BOOL m_AutoTrig;
    afx_msg void OnBnClickedAutotrigCheck();
    BOOL m_LogOutput;
    afx_msg void OnBnClickedLogCheck();
    afx_msg void OnBnClickedOk();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedChangeSequenceButton();
    CSliderCtrl m_DualSlopeSliderCtrl;
    afx_msg void OnEnChangeBlackLevelEdit1();
    afx_msg void OnEnChangeBlackLevelEdit2();
    afx_msg void OnDeltaposGainSpin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedSaveButton();
    CComboBox m_PixelFormatCtrl;
    afx_msg void OnBnClickedWbButton();
    afx_msg void OnEnChangeRgainEdit();
    CEdit m_RGainCtrl;
    afx_msg void OnDeltaposRgainSpin(NMHDR *pNMHDR, LRESULT *pResult);
    CSpinButtonCtrl m_RGainSpinCtrl;
    CEdit m_BGainCtrl;
    CSpinButtonCtrl m_BGainSpinCtrl;
    afx_msg void OnEnChangeBgainEdit();
    afx_msg void OnDeltaposBgainSpin(NMHDR *pNMHDR, LRESULT *pResult);
};
