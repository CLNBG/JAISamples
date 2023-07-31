// ButtonDlg.h : ヘッダー ファイル
//

#pragma once
#include <Jai_Factory.h>
#include "afxwin.h"
#include "mybutton.h"


// CButtonDlg ダイアログ
class CButtonDlg : public CDialog
{
// コンストラクション
public:
	CButtonDlg(CWnd* pParent = NULL);	// 標準コンストラクタ
	BOOL Create();

// ダイアログ データ
	enum { IDD = IDD_BUTTON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート


// 実装
protected:
	HICON m_hIcon;

	// 生成された、メッセージ割り当て関数
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	FACTORY_HANDLE  m_hFactory;			// Factory Handle
	CAM_HANDLE      m_hCam;			// Camera Handle
	int8_t          m_sCameraId[J_CAMERA_ID_SIZE];    // Camera ID

	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;

	bool			m_bEnableStreaming;

	CStatic m_oStatus;					// Status表示オブジェクト
	void DispBtStatus(int iBtStatus);	// Status表示関数

    BOOL OpenFactoryAndCamera();
    void CloseFactoryAndCamera();
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);

	void IrisCtrl(BOOL flg);
	void ZoomCtrl(BOOL flg);
	void FocusCtrl(BOOL flg);

	CMyButton m_oIrisOpenBtn;
	CMyButton m_oIrisCloseBtn;
	CMyButton m_oZoomWideBtn;
	CMyButton m_oZoomTeleBtn;
	CMyButton m_oFocusNearBtn;
	CMyButton m_oFocusFarBtn;
};
