#pragma once


// CMyButton
// CButton �N���X����h���������Ǝ��̃{�^���N���X

class CMyButton : public CButton
{
	DECLARE_DYNAMIC(CMyButton)

public:
	CMyButton();
	virtual ~CMyButton();

protected:
	DECLARE_MESSAGE_MAP()

public:
	// �{�^�����������A���A���̃n���h�� �v���g�^�C�v
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	void SetParent(CWnd * pParent);
	CWnd *m_pParent;
};


