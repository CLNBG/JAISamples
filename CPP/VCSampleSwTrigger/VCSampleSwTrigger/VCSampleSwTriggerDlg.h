// VCSampleSwTriggerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include <Jai_Factory.h>


// CVCSampleSwTriggerDlg dialog
class CVCSampleSwTriggerDlg : public CDialog
{
// Construction
public:
	CVCSampleSwTriggerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VCSAMPLESWTRIGGER_DIALOG };

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
	int8_t			m_CameraId[J_CAMERA_ID_SIZE]; // camera information
	uint32_t		m_OldCameraIndex; //previously selected camera
	bool			m_bayerCamera;

	BOOL OpenFactory();
	void CloseFactoryAndCamera();
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);
	void InitializeControls();
	void PrepareSwTrigSetting();
	void PrepareContinuousSetting();

	CListBox m_cameraList;
	CStatic m_cameraIdStatic;
	CSliderCtrl m_widthSlider;
	CSliderCtrl m_heightSlider;
	CSliderCtrl m_gainSlider;
	CStatic m_widthStatic;
	CStatic m_heightStatic;
	CStatic m_gainStatic;
	CButton m_startButton;
	CButton m_stopButton;
	CStatic m_statusBarStatic;
	CButton m_wBalanceButton;
	CButton m_swTriggerButton;

	afx_msg void OnDestroy();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLbnDblclkCameralist();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedStopbutton();
	afx_msg void OnBnClickedSwtriggerbutton();
	afx_msg void OnBnClickedWbalancebutton();
	afx_msg void OnBnClickedContinuousradio();
	afx_msg void OnBnClickedSwtrigradio();
	CButton m_continuousRadio;
	CButton m_swtrigRadio;
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
