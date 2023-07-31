// ReadPixelSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "afxwin.h"
#include "afxcmn.h"
#include <GDIPLUS.H>
#include <STRSAFE.H>

using namespace Gdiplus;

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

// CReadPixelSampleDlg dialog
class CReadPixelSampleDlg : public CDialog
{
    // Construction
public:
    CReadPixelSampleDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_READPIXELSAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
public:
    FACTORY_HANDLE		m_hFactory;     // Factory Handle
    CAM_HANDLE			m_hCam;         // Camera Handle
    VIEW_HANDLE			m_hView;
    THRD_HANDLE			m_hThread;
    int8_t				m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
    PIXELVALUE			m_PixelValue;
    uint64_t			m_PixelType;
    CRITICAL_SECTION    m_CriticalSection;
	int64_t				m_iWidthInc;
	int64_t				m_iHeightInc;
	bool				m_bEnableStreaming;
    
    J_tIMAGE_INFO		m_CnvImageInfo;    // Image info structure
    int m_XPos;
    int m_YPos;
    bool m_bPositionChanged;
    bool m_bStretchViewWindow;

    // View Window Options
    bool m_bEnableMouseZoom;
    bool m_bEnableMouseCursor;
    bool m_bEnableDoubleBuffering;

    // View Window events
    bool m_bEnableViewWindowMoveEvents;
    bool m_bEnableViewWindowSizeEvents;
    bool m_bEnableMouseMoveEvents;
    bool m_bEnableMouseLButtonDownEvents;
    bool m_bEnableMouseLButtonUpEvents;
    bool m_bEnableMouseRButtonDownEvents;
    bool m_bEnableMouseRButtonUpEvents;
    bool m_bEnableZoomPosChangeEvents;
    bool m_bEnableAreaSelectionEvents;
    bool m_bEnableUserDrawImageEvents;
    bool m_bEnableUserDrawViewWindowEvents;

    bool m_bEnableUserDrawImageGrid;
    bool m_bEnableUserDrawViewWindowGrid;

    CString m_ViewWindowEventText;
    CString m_UserDrawImageEventText;
    CString m_UserDrawViewWindowEventText;

    BOOL OpenFactoryAndCamera();
    void CloseFactoryAndCamera();
    void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
    void InitializeControls();
    void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);
    void ViewWindowNotificationCallback(VIEW_HANDLE hWin, J_IVW_CALLBACK_TYPE Type, void *pValue);
    void SetupViewWindowCallbackMask();

protected:
    HICON m_hIcon;

    GdiplusStartupInput m_gdiplusStartupInput;
    ULONG_PTR m_gdiplusToken;

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
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CStatic m_ValueStaticCtrl;
    afx_msg void OnBnClickedStretchedCheck();
    afx_msg void OnBnClickedEnableMouseZoomCheck();
    afx_msg void OnBnClickedEnableMouseCursorCheck();
    afx_msg void OnBnClickedWindowMoveEventCheck();
    afx_msg void OnBnClickedWindowSizeEventCheck();
    afx_msg void OnBnClickedMouseMoveEventCheck();
    afx_msg void OnBnClickedLbuttonDownCheck();
    afx_msg void OnBnClickedLbuttonUpCheck();
    afx_msg void OnBnClickedRbuttonDownCheck();
    afx_msg void OnBnClickedRbuttonUpCheck();
    afx_msg void OnBnClickedZoomPosEventCheck();
    afx_msg void OnBnClickedAreaEventCheck();
    afx_msg void OnBnClickedUserDrawImageEventCheck();
    afx_msg void OnBnClickedUserDrawViewWindowEventCheck();
    afx_msg void OnEnChangeUserDrawImageTextEdit();
    afx_msg void OnEnChangeUserDrawViewWindowTextEdit();
    afx_msg void OnBnClickedEnableDoubleBufferingCheck();
    afx_msg void OnBnClickedUserDrawImageGridCheck();
    afx_msg void OnBnClickedUserDrawViewWindowGridCheck();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
