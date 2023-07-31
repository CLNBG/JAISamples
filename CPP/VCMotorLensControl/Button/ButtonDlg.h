// ButtonDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once
#include <Jai_Factory.h>
#include "afxwin.h"
#include "mybutton.h"


// CButtonDlg �_�C�A���O
class CButtonDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CButtonDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^
	BOOL Create();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_BUTTON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
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

	CStatic m_oStatus;					// Status�\���I�u�W�F�N�g
	void DispBtStatus(int iBtStatus);	// Status�\���֐�

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
