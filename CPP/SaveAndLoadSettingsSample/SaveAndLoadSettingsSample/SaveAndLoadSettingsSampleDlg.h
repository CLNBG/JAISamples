// SaveAndLoadSettingsSampleDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <Jai_Factory.h>

// CSaveAndLoadSettingsSampleDlg dialog
class CSaveAndLoadSettingsSampleDlg : public CDialog
{
// Construction
public:
	CSaveAndLoadSettingsSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SAVEANDLOADSETTINGSSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
public:
  FACTORY_HANDLE  m_hFactory;     // Factory Handle
  CAM_HANDLE      m_hCam;         // Camera Handle
  int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
  bool		      m_bEnableStreaming;

  BOOL OpenFactoryAndCamera();
  void CloseFactoryAndCamera();

  void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);
  void ShowErrorMsg(CString message, J_STATUS_TYPE error);

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedSaveSettingsButton();
    afx_msg void OnBnClickedLoadSettingsButton();
    afx_msg void OnDestroy();
    CComboBox m_SaveSettingsComboBox;
    CComboBox m_LoadSettingsComboBox;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
