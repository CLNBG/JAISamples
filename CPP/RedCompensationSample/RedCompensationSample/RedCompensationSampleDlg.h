// RedCompensationSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

#define	NUM_OF_COMPEN_PARAM	    3

// CRedCompensationSampleDlg dialog
class CRedCompensationSampleDlg : public CDialog
{
    // Construction
public:
    CRedCompensationSampleDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_REDCOMPENSATIONSAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
public:
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
    VIEW_HANDLE     m_hView;
    THRD_HANDLE     m_hThread;
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;

    BOOL OpenFactoryAndCamera();
    void CloseFactoryAndCamera();
    void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
    void InitializeControls();
    void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);

	CEdit m_RThresh;
	CEdit m_RGain;
	CEdit m_GGain;
	CEdit m_BGain;
	CButton m_swFullScreen;
	CButton m_swCompen;
	CSpinButtonCtrl m_RThreshSpin;
	CSpinButtonCtrl m_GThreshSpin;
	CSpinButtonCtrl m_BThreshSpin;
	CSpinButtonCtrl m_RGainSpin;
	CSpinButtonCtrl m_GGainSpin;
	CSpinButtonCtrl m_BGainSpin;
	CSpinButtonCtrl m_RWBGainSpin;
	CSpinButtonCtrl m_GWBGainSpin;
	CSpinButtonCtrl m_BWBGainSpin;
	unsigned short m_iRThresh;
	unsigned short m_iGThresh;
	unsigned short m_iBThresh;
	unsigned short m_iRGain;
	unsigned short m_iGGain;
	unsigned short m_iBGain;
	BOOL m_bFullScreen;
	BOOL m_bCompen;
	bool m_bSave;
	J_tCOLOR_COMPEN m_ColorCompen;
	J_tIMAGE_INFO m_ImageBufferInfo;

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
	afx_msg void OnDeltaposRthreshspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposGthreshspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposBthreshspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposRgainspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposGgainspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposBgainspin(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedFull();
	afx_msg void OnBnClickedCompen();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
