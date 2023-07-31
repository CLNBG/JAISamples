// LensDistortionSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "StopWatch.h"

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

// CLensDistortionSampleDlg dialog
class CLensDistortionSampleDlg : public CDialog
{
    // Construction
public:
    CLensDistortionSampleDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_LENSDISTORTIONSAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
public:
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
    VIEW_HANDLE     m_hView;
    THRD_HANDLE     m_hThread;
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
    bool            m_bCameraOpen;
    bool            m_bAcquisitionRunning;
    J_tIMAGE_INFO	m_ConversionBuffer;
    J_tIMAGE_INFO	m_ImageProcessingBuffer;
    J_tUNDISTORT_PARAMS m_UndistortParameters;
    bool            m_bUndistort;
    CStopWatch      m_StopWatch;
    double          m_dTimeSpan;
    CRITICAL_SECTION m_CriticalSection;
    CRITICAL_SECTION m_UndistortionParameterCriticalSection;
    bool            m_bMono;
	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;

    BOOL OpenFactoryAndCamera();
    void CloseFactoryAndCamera();
    void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
    void InitializeControls();
    void EnableControls();
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
public:
    afx_msg void OnBnClickedStart();
public:
    afx_msg void OnBnClickedStop();
public:
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnBnClickedUndistortCheck();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedInterpolationCheck();
    afx_msg void OnEnChangeP1Edit();
    afx_msg void OnEnChangeOpticalCenterXEdit();
    afx_msg void OnEnChangeOpticalCenterYEdit();
    afx_msg void OnEnChangeFocalLengthXEdit();
    afx_msg void OnEnChangeFocalLengthYEdit();
    afx_msg void OnEnChangeK1Edit();
    afx_msg void OnEnChangeK2Edit();
    afx_msg void OnEnChangeK3Edit();
    afx_msg void OnEnChangeP2Edit();
    afx_msg void OnBnClickedApplyButton();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
