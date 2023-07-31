// MultipleConditionSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "afxwin.h"
#include <VLib.h>

// CMultipleConditionSampleDlg dialog
class CMultipleConditionSampleDlg : public CDialog
{
    // Construction
public:
    CMultipleConditionSampleDlg(CWnd* pParent = NULL);	// Standard constructor
    BOOL OpenFactoryAndCamera();                        // Open factory and open first camera
    void CloseFactoryAndCamera();                       // Close open camera and close factory

    FACTORY_HANDLE  m_hFactory;                         // Factory Handle
    CAM_HANDLE      m_hCamera;                          // Camera Handle
	VIEW_HANDLE		m_hView;	                        // View handle
	THRD_HANDLE		m_hThread;	                        // Thread handle
    STREAM_HANDLE   m_hDS;                              // Handle to the data stream
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];      // Camera ID
    HANDLE          m_hSupervisionThread;               // Handle to the GigE Vision event supervision thread
    HANDLE	        m_hGigEVisionEventCondition;        // Condition used for GigE Vision Event supervision
    HANDLE	        m_hCameraConnectionCondition;       // Condition used for Camera Connection supervision
    HANDLE	        m_hNewImageCondition;               // Condition used for getting the New Image Events
    bool            m_bEnableThread;                    // Flag indicating if the connection event thread should run
    bool            m_bThreadRunning;                   // Flag indicating if the connection event thread is running
    uint32_t        m_iValidBuffers;                    // Number of buffers allocated to image acquisition
    uint8_t*        m_pAquBuffer[NUM_OF_BUFFER];        // Buffers allocated to hold image data
    BUF_HANDLE      m_pAquBufferID[NUM_OF_BUFFER];      // Handles for all the image buffers
	HANDLE	        m_hKillThread;						// Signal to kill thread
	HANDLE	        m_hThreadKilled;					// Signal that thread has exited

    BOOL CreateDataStream(void);                        // Create Data Stream and prepare buffers
    BOOL CloseDataStream(void);                         // Close Data Stream and unprepare buffers
    BOOL CreateSupervisionThread(void);                 // Create supervision thread
    BOOL TerminateSupervisionThread(void);              // Terminate the supervision thread
    void SupervisionProcess(void);                      // The actual supervision thread
    uint32_t PrepareBuffer(int iBufferSize);            // Prepare the frame acquisition buffers
    BOOL UnPrepareBuffer(void);                         // Remove frame acquisition buffers
    void PrepareSwTrigSetting();                        // Set up the camera for Software Trigger
    void ShowErrorMsg(CString message, J_STATUS_TYPE error); // Error message display helper function
    BOOL PrepareImageViewWindow();                      // Create a new View Window for image display

    // Dialog Data
    enum { IDD = IDD_MULTIPLECONDITIONSAMPLE_DIALOG };

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
    afx_msg void OnBnClickedTrigButton();
    CStatic m_ConnectionStatusLabel;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
