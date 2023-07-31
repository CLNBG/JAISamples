// LUTSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "StopWatch.h"
#include "afxwin.h"

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

// CLUTSampleDlg dialog
class CLUTSampleDlg : public CDialog
{
    // Construction
public:
    CLUTSampleDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_LUTSAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
public:
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
    VIEW_HANDLE     m_hView;
    THRD_HANDLE     m_hThread;
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];  // Camera ID
  	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;
    J_tLUT          m_LUT;                          // Lookup table
    bool            m_bProcess;
    bool            m_bGamma;
    bool            m_bOldGamma;
    double          m_GammaValue;
    double          m_OldGammaValue;
    CStopWatch      m_StopWatch;
    double          m_dTimeSpan;
    uint64_t        m_OldPixelType;
    CRITICAL_SECTION m_CriticalSection;
    int m_PixelFormatInt;
    int64_t m_PixelFormatValues[10];

    BOOL OpenFactoryAndCamera();
    void CloseFactoryAndCamera();
    void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
    void InitializeControls();
    void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);
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
    afx_msg void OnBnClickedProcessCheck();
    afx_msg void OnBnClickedGammaCheck();
    afx_msg void OnEnChangeGammaEdit();
    afx_msg void OnBnClickedSaveLutButton();
    afx_msg void OnBnClickedLoadLutButton();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedLinearRadio();
    afx_msg void OnBnClickedInvertRadio();
    afx_msg void OnBnClickedCompressRadio();
    afx_msg void OnBnClickedExpandRadio();
    afx_msg void OnBnClickedGamma045Radio();
    afx_msg void OnBnClickedGamma060Radio();
    CComboBox m_PixelFormatCtrl;
    afx_msg void OnCbnSelchangePixelformatCombo();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
