// OpenCVSample1Dlg.h : header file
//

#pragma once

#include <Jai_Factory.h>

// OpenCV includes required
#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

// CVCSample1Dlg dialog
class COpenCVSample1Dlg : public CDialog
{
    // Construction
public:
    COpenCVSample1Dlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_VCSAMPLE1_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
public:
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
    THRD_HANDLE     m_hThread;      // Acquisition Thread Handle
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
    IplImage        *m_pImg;        // OpenCV Images
    IplImage        *m_pOutputImg;
    IplImage        *m_pColorOutputImg;
    CvMat           *m_pMapMatrix;  // OpenCV Matrix
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
public:
    afx_msg void OnBnClickedStart();
public:
    afx_msg void OnBnClickedStop();
public:
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
