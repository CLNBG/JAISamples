// MyButton.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Button.h"
#include "ButtonDlg.h"
#include "MyButton.h"


// CMyButton
// CButton クラスから派生させた独自のボタンクラス

IMPLEMENT_DYNAMIC(CMyButton, CButton)

CMyButton::CMyButton()
: m_pParent(NULL)
{

}

CMyButton::~CMyButton()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// 左ボタン押し下げ、復帰メッセージを定義する
//////////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMyButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



void CMyButton::SetParent(CWnd * pParent)
{
	m_pParent = pParent;
}

// CMyButton メッセージ ハンドラ

//////////////////////////////////////////////////////////////////////////////////////////////////////
// ボタン押し下げメッセージ　ハンドラ
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CMyButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	BOOL flg;
	POINT p;
	GetCursorPos(&p);
	((CButtonDlg *)m_pParent)->ScreenToClient(&p);

	((CButtonDlg *)m_pParent)->GetDlgItem(40001)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->IrisCtrl( 1 );	// Open
		((CButtonDlg *)m_pParent)->DispBtStatus(1);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40002)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->IrisCtrl( 2 );	// Close
		((CButtonDlg *)m_pParent)->DispBtStatus(2);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40003)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->ZoomCtrl( 1 );	// Wide
		((CButtonDlg *)m_pParent)->DispBtStatus(3);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40004)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->ZoomCtrl( 2 );	// Tele
		((CButtonDlg *)m_pParent)->DispBtStatus(4);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40005)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->FocusCtrl( 1 );	// Near
		((CButtonDlg *)m_pParent)->DispBtStatus(5);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40006)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->FocusCtrl( 2 );	// Far
		((CButtonDlg *)m_pParent)->DispBtStatus(6);
	}

	CButton::OnLButtonDown(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// ボタン復帰メッセージ　ハンドラ
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CMyButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	BOOL flg;
	POINT p;
	GetCursorPos(&p);
	((CButtonDlg *)m_pParent)->ScreenToClient(&p);

	((CButtonDlg *)m_pParent)->GetDlgItem(40001)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->IrisCtrl( 0 );	// Stop
		((CButtonDlg *)m_pParent)->DispBtStatus(0);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40002)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->IrisCtrl( 0 );	// Stop
		((CButtonDlg *)m_pParent)->DispBtStatus(0);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40003)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->ZoomCtrl( 0 );	// Stop
		((CButtonDlg *)m_pParent)->DispBtStatus(0);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40004)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->ZoomCtrl( 0 );	// Stop
		((CButtonDlg *)m_pParent)->DispBtStatus(0);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40005)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->FocusCtrl( 0 );	// Stop
		((CButtonDlg *)m_pParent)->DispBtStatus(0);
	}

	((CButtonDlg *)m_pParent)->GetDlgItem(40006)->GetWindowRect(&rect);
	((CButtonDlg *)m_pParent)->ScreenToClient(&rect);
	flg = rect.PtInRect(p);
	if( flg == TRUE )
	{
		((CButtonDlg *)m_pParent)->FocusCtrl( 0 );	// Stop
		((CButtonDlg *)m_pParent)->DispBtStatus(0);
	}

	CButton::OnLButtonUp(nFlags, point);
}
