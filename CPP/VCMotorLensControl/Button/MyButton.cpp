// MyButton.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "Button.h"
#include "ButtonDlg.h"
#include "MyButton.h"


// CMyButton
// CButton �N���X����h���������Ǝ��̃{�^���N���X

IMPLEMENT_DYNAMIC(CMyButton, CButton)

CMyButton::CMyButton()
: m_pParent(NULL)
{

}

CMyButton::~CMyButton()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// ���{�^�����������A���A���b�Z�[�W���`����
//////////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMyButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



void CMyButton::SetParent(CWnd * pParent)
{
	m_pParent = pParent;
}

// CMyButton ���b�Z�[�W �n���h��

//////////////////////////////////////////////////////////////////////////////////////////////////////
// �{�^�������������b�Z�[�W�@�n���h��
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
// �{�^�����A���b�Z�[�W�@�n���h��
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
