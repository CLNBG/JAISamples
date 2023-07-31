// DynamicLoadSampleDlg.h : header file
//

#pragma once

// It is necessary to define JAI_SDK_DYNAMIC_LOAD before including Jai_Factory.h if the Jai_Factory_Dynamic.h is going to
// be used for dynamically loading the Jai_Factory.dll!!!
#define JAI_SDK_DYNAMIC_LOAD 1
#include "Jai_Factory.h"

#define NODE_NAME_WIDTH			(int8_t*)"Width"
#define NODE_NAME_HEIGHT		(int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN			(int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART		(int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP		(int8_t*)"AcquisitionStop"

// CDynamicLoadSampleDlg dialog
class CDynamicLoadSampleDlg : public CDialog
{
    // Construction
public:
    CDynamicLoadSampleDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_DYNAMICLOADSAMPLE_DIALOG };

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