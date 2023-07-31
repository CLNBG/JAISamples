// ZoomImageDisplaySampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

// CZoomImageDisplaySampleDlg dialog
class CZoomImageDisplaySampleDlg : public CDialog
{
    // Construction
public:
    CZoomImageDisplaySampleDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_ZOOMIMAGEDISPLAYSAMPLE_DIALOG };

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

    void ZoomIn();
    void ZoomIn(int zoomSpeed);
    void ZoomOut();
    void ZoomOut(int zoomSpeed);
    void ZoomDirect(int zoomPosition);
    void ZoomReset();

    void NavigateLeftUp();
    void NavigateLeftUp(int stepSize);
    void NavigateUp();
    void NavigateUp(int stepSize);
    void NavigateRightUp();
    void NavigateRightUp(int stepSize);
    void NavigateLeft();
    void NavigateLeft(int stepSize);
    void NavigateCenter();
    void NavigateRight();
    void NavigateRight(int stepSize);
    void NavigateLeftDown();
    void NavigateLeftDown(int stepSize);
    void NavigateDown();
    void NavigateDown(int stepSize);
    void NavigateRightDown();
    void NavigateRightDown(int stepSize);

    void AdjustImageOffset(POINT centerPosition);
    POINT CalculateCenter();

protected:
    HICON m_hIcon;
    CRITICAL_SECTION m_DiplayCriticalSection;
    J_tIMAGE_INFO m_LastImageInfo;
    POINT m_ImageOffset;
    SIZE m_AcquisitionSize;
    int m_ZoomRatio;

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
    afx_msg void OnBnClickedZoomInButton();
    afx_msg void OnBnClickedZoomOutButton();
    afx_msg void OnBnClickedRightButton();
    afx_msg void OnBnClickedLeftUpButton();
    afx_msg void OnBnClickedUpButton();
    afx_msg void OnBnClickedRightUpButton();
    afx_msg void OnBnClickedLeftButton();
    afx_msg void OnBnClickedCenterButton();
    afx_msg void OnBnClickedLeftDownButton();
    afx_msg void OnBnClickedDownButton();
    afx_msg void OnBnClickedRightDownButton();
    afx_msg void OnBnClickedZoomResetButton();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
