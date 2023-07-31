// ActionCommandSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "afxwin.h"
#include "afxcmn.h"

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

#define MAX_CAMERAS             2

//////////////////////////////////
#define	DEVICE_KEY  0x00000001
#define GROUP_KEY1  0x00000001
#define GROUP_KEY2  0x00000002
#define	GROUP_MASK  0xffffffff
#define GROUP_MASK1 0x00000001
#define GROUP_MASK2 0x00000002

// CActionCommandSampleDlg dialog
class CActionCommandSampleDlg : public CDialog
{
// Construction
public:
	CActionCommandSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ACTIONCOMMANDSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
public:
 	FACTORY_HANDLE  m_hFactory;             // Factory Handle
	CAM_HANDLE      m_hCam[MAX_CAMERAS];    // Camera Handles
    VIEW_HANDLE     m_hView[MAX_CAMERAS];   // View Window handles
	THRD_HANDLE     m_hThread[MAX_CAMERAS]; // Stream handles
    int8_t          m_sCameraId[MAX_CAMERAS][J_CAMERA_ID_SIZE]; // Camera IDs

  BOOL OpenFactoryAndCamera();
  void CloseFactoryAndCamera();
  void StreamCBFunc1(J_tIMAGE_INFO * pAqImageInfo);
  void StreamCBFunc2(J_tIMAGE_INFO * pAqImageInfo);
  void InitializeControls();
  void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);
  bool SetupActions();
  bool SetupContinuous();

protected:
	HICON m_hIcon;
   	bool  m_bReqAck;
    int   m_CameraCount;

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
  afx_msg void OnBnClickedAction1Button();
  afx_msg void OnBnClickedAction2Button();
  CStatic m_statusbarStatic;
  CButton m_reqAckCheck;
  afx_msg void OnBnClickedReqAckCheck();
  CStatic m_TimestampStatic1;
  CStatic m_TimestampStatic2;
  int m_AckCount;
  CSpinButtonCtrl m_AckCountSpin;
  CEdit m_AckCountEdit;
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedOk();
};
