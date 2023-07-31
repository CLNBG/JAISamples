// ConnectionEventSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "afxwin.h"

// CConnectionEventSampleDlg dialog
class CConnectionEventSampleDlg : public CDialog
{
    // Construction
public:
    CConnectionEventSampleDlg(CWnd* pParent = NULL);	// standard constructor
    BOOL OpenFactoryAndCamera();                        // Open factory and open first camera
    void CloseFactoryAndCamera();                       // Close open camera and close factory

    FACTORY_HANDLE  m_hFactory;                         // Factory Handle
    CAM_HANDLE      m_hCam;                             // Camera Handle
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];      // Camera ID
    HANDLE          m_hConnectionSupervisionThread;     // Handle to the connection event supervision thread
    HANDLE	        m_hEventKill;                       // Event used for speeding up the termination of the supervision thread
    bool            m_bEnableThread;                    // Flag indicating if the connection event thread should run
    bool            m_bThreadRunning;                   // Flag indicating if the connection event thread is running
    VIEW_HANDLE     m_hView;
    THRD_HANDLE     m_hThread;

    BOOL CreateConnectionSupervisionThread();           // Create supervision thread
    BOOL TerminateConnectionSupervisionThread(void);    // Terminate the supervision thread
    void ConnectionSupervisionProcess(void);            // The actual supervision thread

    // Dialog Data
    enum { IDD = IDD_CONNECTIONEVENTSAMPLE_DIALOG };

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
    CStatic m_ConnectionStatusLabel;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
