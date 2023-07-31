// ImageWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ImageProcessingThreadSample.h"
#include "ImageProcessingThreadSampleDlg.h"
#include "ImageWnd.h"


// CImageWnd

IMPLEMENT_DYNAMIC(CImageWnd, CWnd)

CImageWnd::CImageWnd(CWnd *pMain)
: m_pMainWnd(pMain)
{

}

CImageWnd::~CImageWnd()
{
}


BEGIN_MESSAGE_MAP(CImageWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CImageWnd message handlers

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
// Image display
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
void CImageWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Pointer of parent window
	CImageProcessingThreadSampleDlg *pParent = reinterpret_cast<CImageProcessingThreadSampleDlg *>(m_pMainWnd);

	// It displays it if there is Bitmap(m_pDiBmp).
	if(pParent->m_hBitmap)
	{
		HDC hDibDC;
		HGDIOBJ	hOldObj;

		// Makes the device context in the transfer origin.
		hDibDC = ::CreateCompatibleDC(dc.m_hDC);
		// M_pDiBmp is selected as Bitmap.
		hOldObj = ::SelectObject(hDibDC, pParent->m_hBitmap);

		// The StretchBlt mode is set in the half tone.
		::SetStretchBltMode(dc.m_hDC, COLORONCOLOR);
		// Transfer (draw)
		::StretchBlt(dc.m_hDC, 0, 0, WIN_WIDTH, WIN_HEIGHT, hDibDC, 0, 0, pParent->m_tDibBuffer.iSizeX, pParent->m_tDibBuffer.iSizeY, SRCCOPY);

		// Bitmap return of device context
		::SelectObject(hDibDC, hOldObj);
		// Device context deletion
		::DeleteDC(hDibDC);
	}
}

