// VCSampleMulticastMasterDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

#define REG_ADDR_SCDA0      (0x0d18)
#define DEFAULT_IPADDR      (0xef000001)

// CVCSampleMulticastMasterDlg dialog
class CVCSampleMulticastMasterDlg : public CDialog
{
    // Construction
public:
    CVCSampleMulticastMasterDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_VCSAMPLEMULTICASTMASTER_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


    // Implementation
public:
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
    VIEW_HANDLE     m_hView;
    THRD_HANDLE     m_hThread;
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
    DWORD           m_dwMulticastIP;
    uint32_t        m_nCameras;


    BOOL OpenFactory();
    void CloseFactoryAndCamera();
    void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
    void InitializeControls();

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
    afx_msg void OnBnClickedOpencamera();
    afx_msg void OnBnClickedClosecamera();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
