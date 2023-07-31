// VCSampleLIntensityDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <Jai_Factory.h>

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

// CVCSampleLIntensityDlg dialog
class CVCSampleLIntensityDlg : public CDialog
{
// Construction
public:
	CVCSampleLIntensityDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VCSAMPLELINTENSITY_DIALOG };

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
	CAM_HANDLE      m_hCam;	// Camera handle 
	VIEW_HANDLE		m_hView;	// View handle
	THRD_HANDLE		m_hThread;	// Thread handle
	int8_t			m_sCameraId[J_CAMERA_ID_SIZE]; // camera ID string array
	RECT m_RectToMeasure;
	unsigned int m_iCountMesuredImages;
	FILE *m_fp;
	int64_t m_pixelformat;
	J_tIMAGE_INFO m_CnvImageInfo;
    int m_RIntensity;
    int m_GIntensity;
    int m_BIntensity;
    CRITICAL_SECTION m_UICriticalSection;
	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;

	BOOL OpenFactoryAndCamera();
	void CloseFactoryAndCamera();
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);
	void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);


	afx_msg void OnBnClickedStartbutton();
	afx_msg void OnBnClickedStopbutton();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();

	CStatic m_cameraidStatic;	
	CSliderCtrl m_gainSlider;
	CStatic m_gainStatic;
	CStatic m_viewerPictureCtl;
	CButton m_testStartButton;
	CButton m_testStopButton;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CStatic m_IntensityString;
    CStatic m_SampleNumber;
};
