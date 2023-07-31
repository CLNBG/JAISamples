// OpenCVSampleSequenceDlg.h : header file
//

#pragma once

#include <jai_factory.h>

#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>

#include "afxwin.h"

#define PULSE_GEN_LENGTH 1000		// sequence trigger speed 10fps 
#define PULSE_GEN_REPEAT_COUNT 4	// sequence 1 to 4
#define GAIN_SEQ_1 -89
#define GAIN_SEQ_2 0
#define GAIN_SEQ_3 100
#define GAIN_SEQ_4 200

#define SWTRIG_TIMER 5000   // Timer 5 sec to send Software Trigger

// COpenCVSampleSequenceDlg dialog
class COpenCVSampleSequenceDlg : public CDialog
{
// Construction
public:
	COpenCVSampleSequenceDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OPENCVSAMPLESEQUENCE_DIALOG };

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
	THRD_HANDLE		m_hThread;	// Thread handle
	bool			m_bBayerCamera;
	int				m_iCurrentCameraIndex; //selected camera
	int				m_iSequenceCount; // 1 - 4
	bool			m_bSeqTriggerMode;
	CString			m_sCameraMac;
	CString			m_sStatusBar;
	int8_t			m_CameraId[J_CAMERA_ID_SIZE]; // camera information

	BOOL OpenFactory();
	void CloseFactory();
	void CloseCamera();
	void ShowErrorMsg(CString message, J_STATUS_TYPE error);
	void PrepareSeqTrigSetting();
	void PrepareContinuousSetting();
	void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
	void SendSoftwareTrigger();
	void InitializePixelFormat();
	void DisplayCameraInfo(int index);

	// member variables for openCV	
	IplImage*		m_cvImgRaw[2];
	IplImage*		m_cvImgBufferWB[2];
	IplImage*		m_cvImgBufferMono;
	IplImage*		m_cvImgBufferColor;
	IplImage*		m_cvImgBuffer32Bit;
	CvSize			m_cvImgSize;
	CvSize			m_cvSeqImgSize;
	CvScalar		m_cvScalarWB;	// RGB + Alpha

	// member func for openCV
	void InitializeOpenCVImages();
	void ConvertAndDisplay();
	void WhiteBalance();

	CComboBox m_cameraList;
	CButton m_continuousRadio;
	CButton m_seqTrigRadio;
	CButton m_startButton;
	CButton m_stopButton;
	CStatic m_statusBarStatic;
	CStatic m_cameraInfoStatic;
	CButton m_wbButton;

	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedContinuousradio();
	afx_msg void OnBnClickedSeqtrigradio();
	afx_msg void OnCbnSelchangeCamlistcombo();
	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedStopbutton();
	afx_msg void OnBnClickedWbalancebutton();
};
