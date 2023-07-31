// WheelDlg.h : 
//

#pragma once

#include <Jai_Factory.h>
#include "afxwin.h"


class CWheelDlg : public CDialog
{
public:
	CWheelDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_WHEEL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    FACTORY_HANDLE  m_hFactory;     // Factory Handle
    CAM_HANDLE      m_hCam;         // Camera Handle
    THRD_HANDLE     m_hThread;
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID
	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    BOOL OpenFactoryAndCamera();
    void CloseFactoryAndCamera();
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);

	CButton m_oOpen;
	CButton m_oStop;
	CButton m_oClose;
	int SetCheckMark(int iSelect);
	int m_iUDSelect;
	int m_iCurrentState;
	UINT_PTR m_uipTimerID;
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
