#pragma once


// CMyButton
// CButton クラスから派生させた独自のボタンクラス

class CMyButton : public CButton
{
	DECLARE_DYNAMIC(CMyButton)

public:
	CMyButton();
	virtual ~CMyButton();

protected:
	DECLARE_MESSAGE_MAP()

public:
	// ボタン押し下げ、復帰時のハンドラ プロトタイプ
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	void SetParent(CWnd * pParent);
	CWnd *m_pParent;
};


