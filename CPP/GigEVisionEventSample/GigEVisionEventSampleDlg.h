// GigEVisionEventSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "afxwin.h"

// CGigEVisionEventSampleDlg dialog
class CGigEVisionEventSampleDlg : public CDialog
{
    // Construction
public:
    CGigEVisionEventSampleDlg(CWnd* pParent = NULL);	// standard constructor
    BOOL OpenFactoryAndCamera();                        // Open factory and open first camera
    void CloseFactoryAndCamera();                       // Close open camera and close factory

    FACTORY_HANDLE  m_hFactory;                         // Factory Handle
    CAM_HANDLE      m_hCamera;                          // Camera Handle
	VIEW_HANDLE		m_hView;	                        // View handle
	THRD_HANDLE		m_hThread;	                        // Thread handle
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];      // Camera ID
    HANDLE          m_hSupervisionThread;               // Handle to the GigE Vision event supervision thread
    HANDLE	        m_hCondition;                       // Condition used for GigE Vision Event supervision
    bool            m_bEnableThread;                    // Flag indicating if the connection event thread should run
    bool            m_bThreadRunning;                   // Flag indicating if the connection event thread is running

    BOOL CreateSupervisionThread();                     // Create supervision thread
    BOOL TerminateSupervisionThread(void);              // Terminate the supervision thread
    void SupervisionProcess(void);                      // The actual supervision thread
    void PrepareSwTrigSetting();                        // Set up the camera for Software Trigger
    void ShowErrorMsg(CString message, J_STATUS_TYPE error); // Error message display helper function

    // Dialog Data
    enum { IDD = IDD_GIGEVISIONEVENTSAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    CStatic m_StatusLabel;
    afx_msg void OnBnClickedStartButton();
    afx_msg void OnBnClickedStopButton();
    afx_msg void OnBnClickedTrigButton();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
