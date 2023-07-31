// VCSampleMissingFramesDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <Jai_Factory.h>


// CVCSampleMissingFramesDlg dialog
class CVCSampleMissingFramesDlg : public CDialog
{
// Construction
public:
	CVCSampleMissingFramesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VCSAMPLEMISSINGFRAMES_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	FACTORY_HANDLE  m_hFactory;	// Factory handle
	CAM_HANDLE      m_hCamera;	// Camera handle 
	VIEW_HANDLE		m_hView;	// View handle
	THRD_HANDLE		m_hThread;	// Thread handle
	int8_t			m_CameraId[J_CAMERA_ID_SIZE]; // camera ID string array
	uint32_t		m_OldCameraIndex; //previously selected camera
	uint64_t		m_iCountFramesWithMissingPackets;
	uint64_t		m_iCountMissingFrames;
	uint64_t		m_iCountReceivedFrames;
	uint64_t		m_iCountTotalMissingPackets;
	uint64_t		m_iCountCurruptedFrames;
	uint64_t		m_iCountLostFrames;
	bool			m_bDropFramesWithMissingPacket;
	CRITICAL_SECTION m_UICriticalSection;

	BOOL OpenFactory();
	void CloseFactoryAndCamera();
	void ShowErrorMsg(J_STATUS_TYPE error);
	void InitializeControls();
	void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);

	CComboBox m_cameraList;
	CStatic m_cameraIdStatic;
	CSliderCtrl m_widthSlider;
	CSliderCtrl m_heightSlider;
	CSliderCtrl m_gainSlider;
	CStatic m_widthStatic;
	CStatic m_heightStatic;
	CStatic m_gainStatic;
	CButton m_startButton;
	CButton m_stopButton;
	CStatic m_missingFramesStatic;
	CStatic m_fMissingPacketsStatic;

	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CStatic m_statusBarStatic;
	afx_msg void OnCbnSelchangeCamlistcombo();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedStopbutton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_viewerPictureCtrl;
	CButton m_dropFramesCheck;
	CStatic m_totalMissingPacketsStatic;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
