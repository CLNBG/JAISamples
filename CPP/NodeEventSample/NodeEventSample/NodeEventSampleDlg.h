// NodeEventSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "afxwin.h"

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PAYLOADSIZE   (int8_t*)"PayloadSize"

// CNodeEventSampleDlg dialog
class CNodeEventSampleDlg : public CDialog
{
    // Construction
public:
    CNodeEventSampleDlg(CWnd* pParent = NULL);	// standard constructor

    // Dialog Data
    enum { IDD = IDD_NODEEVENTSAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
public:
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
	bool			m_bEnableStreaming;

    BOOL OpenFactoryAndCamera();
    void CloseFactoryAndCamera();
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
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    CStatic m_PayloadSizeCtrl;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
