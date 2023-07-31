// TestMultiAcqisitionDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include <vector>
#include "afxwin.h"

class CCamera; //forward dec.

// CTestMultiAcqisitionDlg dialog
class CTestMultiAcqisitionDlg : public CDialog
{
// Construction
public:
	CTestMultiAcqisitionDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TESTMULTIACQISITION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	FACTORY_HANDLE  m_hFactory;     // Factory Handle
	std::vector<CCamera*> m_CameraVector;
	UINT_PTR m_nWindowTimer;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedOk();

	BOOL OpenFactoryAndCamera();        // Open factory and search for cameras. Open first camera
	void CloseFactoryAndCamera();       // Close open camera and close factory to clean up
	void ShowErrorMsg(CString message, J_STATUS_TYPE error);

	CListBox m_cameraList;
	CButton m_ctrlEnableDisplay;
	CButton m_ctrlHighPriority;
	CListBox m_ctrlFrameCount;
	CListBox m_ctrlBadFrames;
	afx_msg void OnBnClickedSearch();
	CButton m_ctrlSearch;
	CButton m_ctrlProcessImages;
	virtual BOOL DestroyWindow();
	int m_nRadioThreadType;
	CButton m_ctrlUseTestPattern;
	afx_msg void OnBnClickedCloseCamera();
	CButton m_ctrlCloseCameras;
};
