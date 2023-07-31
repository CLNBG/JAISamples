#pragma once


// CImageWnd

class CImageWnd : public CWnd
{
    DECLARE_DYNAMIC(CImageWnd)

public:
    CImageWnd(CWnd *pMain);
    virtual ~CImageWnd();

protected:
    DECLARE_MESSAGE_MAP()
public:
    CWnd *m_pMainWnd;
    afx_msg void OnPaint();
};


