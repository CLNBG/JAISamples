// ImageProcessingThreadSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "ImageWnd.h"
#include "afxwin.h"

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

// CImageProcessingThreadSampleDlg dialog
class CImageProcessingThreadSampleDlg : public CDialog
{
    // Construction
public:
    CImageProcessingThreadSampleDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_IMAGEPROCESSINGTHREADSAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
public:
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
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
    void ImageThread(void * pParam);
    void CheckCreateBmp(J_tIMAGE_INFO * pImageInfo);
    J_tIMAGE_INFO m_tBuffer;
    J_tIMAGE_INFO m_tDibBuffer;
    J_tIMAGE_INFO *m_pOriginalImage;
    CImageWnd *m_pImageWnd;
    CBitmap *m_pDiBmp;
    HBITMAP m_hBitmap;
    BITMAPINFO	m_tBmp;
    void BitInvert(J_tIMAGE_INFO * pImageInfo);
    CButton m_oAfterProcess;
    afx_msg void OnBnClickedAfterProcess();
    int m_iAfterProcess;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
