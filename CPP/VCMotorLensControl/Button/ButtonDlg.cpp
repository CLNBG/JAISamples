// ButtonDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "Button.h"
#include "ButtonDlg.h"
#include <process.h>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CButtonDlg ダイアログ




CButtonDlg::CButtonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CButtonDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_iWidthInc = 1;
	m_iHeightInc = 1;

	m_bEnableStreaming = false;
}

BOOL CButtonDlg::Create()
{
	return CDialog::Create( CButtonDlg::IDD );
}

void CButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATUS, m_oStatus);
}

BEGIN_MESSAGE_MAP(CButtonDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CButtonDlg メッセージ ハンドラ

BOOL CButtonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// "バージョン情報..." メニューをシステム メニューに追加します。

	// IDM_ABOUTBOX は、システム コマンドの範囲内になければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

    BOOL retval;

    // Open factory & camera
    retval = OpenFactoryAndCamera();  

	// このダイアログのアイコンを設定します。アプリケーションのメイン ウィンドウがダイアログでない場合、
	//  Framework は、この設定を自動的に行います。
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンの設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンの設定

	// TODO: 初期化をここに追加します。

	RECT BtRectIrisOpen  = {20, 10, 80, 40};	// left, top, right, bottom
	UINT uiIrisOpenBtnID = 40001;
	m_oIrisOpenBtn.SetParent(this);
	m_oIrisOpenBtn.Create(_T("IRIS Open"), WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY, BtRectIrisOpen, this, uiIrisOpenBtnID);
	m_oIrisOpenBtn.SetFont(this->GetFont(), TRUE);

	RECT BtRectIrisClose  = {90, 10, 150, 40};	// left, top, right, bottom
	UINT uiIrisCloseBtnID = 40002;
	m_oIrisCloseBtn.SetParent(this);
	m_oIrisCloseBtn.Create(_T("IRIS Close"), WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY, BtRectIrisClose, this, uiIrisCloseBtnID);
	m_oIrisCloseBtn.SetFont(this->GetFont(), TRUE);

	RECT BtRectZoomWide  = {20, 40, 80, 70};	// left, top, right, bottom
	UINT uiZoomWideBtnID = 40003;
	m_oZoomWideBtn.SetParent(this);
	m_oZoomWideBtn.Create(_T("Zoom Wide"), WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY, BtRectZoomWide, this, uiZoomWideBtnID);
	m_oZoomWideBtn.SetFont(this->GetFont(), TRUE);

	RECT BtRectZoomTele  = {90, 40, 150, 70};	// left, top, right, bottom
	UINT uiZoomTeleBtnID = 40004;
	m_oZoomTeleBtn.SetParent(this);
	m_oZoomTeleBtn.Create(_T("Zoom Tele"), WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY, BtRectZoomTele, this, uiZoomTeleBtnID);
	m_oZoomTeleBtn.SetFont(this->GetFont(), TRUE);

	RECT BtRectFocusNear  = {20, 70, 80, 100};	// left, top, right, bottom
	UINT uiFocusNearBtnID = 40005;
	m_oFocusNearBtn.SetParent(this);
	m_oFocusNearBtn.Create(_T("Focus Near"), WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY, BtRectFocusNear, this, uiFocusNearBtnID);
	m_oFocusNearBtn.SetFont(this->GetFont(), TRUE);

	RECT BtRectFocusFar  = {90, 70, 150, 100};	// left, top, right, bottom
	UINT uiFocusFarBtnID = 40006;
	m_oFocusFarBtn.SetParent(this);
	m_oFocusFarBtn.Create(_T("Focus Far"), WS_VISIBLE | WS_CHILD | BS_CENTER | BS_PUSHBUTTON | BS_TEXT | BS_NOTIFY, BtRectFocusFar, this, uiFocusFarBtnID);
	m_oFocusFarBtn.SetFont(this->GetFont(), TRUE);

	return TRUE;  // フォーカスをコントロールに設定した場合を除き、TRUE を返します。
}

void CButtonDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: ここにメッセージ ハンドラ コードを追加します。

    // Close factory & camera
    CloseFactoryAndCamera();
}

void CButtonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ダイアログに最小化ボタンを追加する場合、アイコンを描画するための
//  下のコードが必要です。ドキュメント/ビュー モデルを使う MFC アプリケーションの場合、
//  これは、Framework によって自動的に設定されます。

void CButtonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// クライアントの四角形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンの描画
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ユーザーが最小化したウィンドウをドラッグしているときに表示するカーソルを取得するために、
//  システムがこの関数を呼び出します。
HCURSOR CButtonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//--------------------------------------------------------------------------------------------------
// OpenFactoryAndCamera
//--------------------------------------------------------------------------------------------------
BOOL CButtonDlg::OpenFactoryAndCamera()
{
	J_STATUS_TYPE   retval;
	uint32_t        iSize;
	uint32_t        iNumDev;
	bool8_t         bHasChange;

	m_bEnableStreaming = false;

	// Open factory
	retval = J_Factory_Open((int8_t*)"", &m_hFactory);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not open factory!"), retval);
		return FALSE;
	}
	TRACE("Opening factory succeeded\n");

	//Update camera list
	retval = J_Factory_UpdateCameraList(m_hFactory, &bHasChange);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not update camera list!"), retval);
		return FALSE;
	}
	TRACE("Updating camera list succeeded\n");

	// Get the number of Cameras
	retval = J_Factory_GetNumOfCameras(m_hFactory, &iNumDev);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get the number of cameras!"), retval);
		return FALSE;
	}

	if (iNumDev == 0)
	{
		ShowErrorMsg(CString("Invalid number of cameras!"), retval);
		return FALSE;
	}
	TRACE("%d cameras were found\n", iNumDev);

	// Get camera ID
	iSize = (uint32_t)sizeof(m_sCameraId);
	retval = J_Factory_GetCameraIDByIndex(m_hFactory, 0, m_sCameraId, &iSize);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not get the camera ID!"), retval);
		return FALSE;
	}
	TRACE("Camera ID: %s\n", m_sCameraId);

	// Open camera
	retval = J_Camera_Open(m_hFactory, m_sCameraId, &m_hCam);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Could not open the camera!"), retval);
		return FALSE;
	}

	//Make sure streaming is supported!
	uint32_t numStreams = 0;
	retval = J_Camera_GetNumOfDataStreams(m_hCam, &numStreams);
	if (retval != J_ST_SUCCESS)
	{
		ShowErrorMsg(CString("Error with J_Camera_GetNumOfDataStreams."), retval);
		return FALSE;
	}

	if(0 == numStreams)
	{
		m_bEnableStreaming = false;
	}
	else
	{
		m_bEnableStreaming = true;
	}


	if(iNumDev > 0 && 0 != m_hCam)
	{
		TRACE("Opening camera succeeded\n");
	}
	else
	{
		ShowErrorMsg(CString("Invalid number of Devices!"), iNumDev);
		return FALSE;
	}

	return TRUE;
}
//--------------------------------------------------------------------------------------------------
// CloseFactoryAndCamera
//--------------------------------------------------------------------------------------------------
void CButtonDlg::CloseFactoryAndCamera()
{
    J_STATUS_TYPE   retval;

    if (m_hCam)
    {
        // Close camera
        retval = J_Camera_Close(m_hCam);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close the camera!"), retval);
        }
        m_hCam = NULL;
        TRACE("Closed camera\n");
    }

    if (m_hFactory)
    {
        // Close factory
        retval = J_Factory_Close(m_hFactory);
        if (retval != J_ST_SUCCESS)
        {
            ShowErrorMsg(CString("Could not close the factory!"), retval);
        }
        m_hFactory = NULL;
        TRACE("Closed factory\n");
    }
}

void CButtonDlg::ShowErrorMsg(CString message, J_STATUS_TYPE error)
{
    CString errorMsg;
    errorMsg.Format(_T("%s: Error = %d: "), message, error);

    switch(error)
    {
    case J_ST_INVALID_BUFFER_SIZE:	errorMsg += "Invalid buffer size ";	                break;
    case J_ST_INVALID_HANDLE:		errorMsg += "Invalid handle ";		                break;
    case J_ST_INVALID_ID:			errorMsg += "Invalid ID ";			                break;
    case J_ST_ACCESS_DENIED:		errorMsg += "Access denied ";		                break;
    case J_ST_NO_DATA:				errorMsg += "No data ";				                break;
    case J_ST_ERROR:				errorMsg += "Generic error ";		                break;
    case J_ST_INVALID_PARAMETER:	errorMsg += "Invalid parameter ";	                break;
    case J_ST_TIMEOUT:				errorMsg += "Timeout ";				                break;
    case J_ST_INVALID_FILENAME:		errorMsg += "Invalid file name ";	                break;
    case J_ST_INVALID_ADDRESS:		errorMsg += "Invalid address ";		                break;
    case J_ST_FILE_IO:				errorMsg += "File IO error ";		                break;
    case J_ST_GC_ERROR:				errorMsg += "GenICam error ";		                break;
    case J_ST_VALIDATION_ERROR:		errorMsg += "Settings File Validation Error ";		break;
    case J_ST_VALIDATION_WARNING:	errorMsg += "Settings File Validation Warning ";    break;
    }

    AfxMessageBox(errorMsg, MB_OKCANCEL | MB_ICONINFORMATION);

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// ボタンの状態を表示する関数。この関数は、独自ボタンオブジェクトから直接呼ばれる
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CButtonDlg::DispBtStatus(int iBtStatus)
{
	if(iBtStatus == 1)
		m_oStatus.SetWindowText(_T("Open"));
	else if(iBtStatus == 2)
		m_oStatus.SetWindowText(_T("Close"));
	else if(iBtStatus == 3)
		m_oStatus.SetWindowText(_T("Wide"));
	else if(iBtStatus == 4)
		m_oStatus.SetWindowText(_T("Tele"));
	else if(iBtStatus == 5)
		m_oStatus.SetWindowText(_T("Near"));
	else if(iBtStatus == 6)
		m_oStatus.SetWindowText(_T("Far"));
	else
		m_oStatus.SetWindowText(_T(""));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CButtonDlg::IrisCtrl(int flg)
{
    J_STATUS_TYPE   retval;

	switch( flg )
	{
		case 0:		// Stop
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensIris", (int8_t*)"Stop");
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensIris=Stop!"), retval);
			}
			break;

		case 1:		// Open
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensIris", (int8_t*)"Open");
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensIris=Open!"), retval);
			}
			break;

		case 2:		// Close
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensIris", (int8_t*)"Close");
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensIris=Close!"), retval);
			}
			break;

		default:
			break;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CButtonDlg::ZoomCtrl(int flg)
{
    J_STATUS_TYPE   retval;

	switch( flg )
	{
		case 0:		// Stop
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensZoom", (int8_t*)"Stop");
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensZoom=Stop!"), retval);
			}
			break;

		case 1:		// Wide
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensZoom", (int8_t*)"Wide");
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensZoom=Wide!"), retval);
			}
			break;

		case 2:		// Tele
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensZoom", (int8_t*)"Tele");
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensZoom=Tele!"), retval);
			}
			break;

		default:
			break;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
void CButtonDlg::FocusCtrl(int flg)
{
    J_STATUS_TYPE   retval;

	switch( flg )
	{
		case 0:		// Stop
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensFocus", (int8_t*)"Stop");
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensFocus=Stop!"), retval);
			}
			break;

		case 1:		// Near
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensFocus", (int8_t*)"Near");
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensFocus=Near!"), retval);
			}
			break;

		case 2:		// far
            retval = J_Camera_SetValueString(m_hCam, (int8_t*)"MotorLensFocus", (int8_t*)"Far");
			if(retval != J_ST_SUCCESS) 
			{
				ShowErrorMsg(CString("Could not set MotorLensFocus=Far!"), retval);
			}
			break;

		default:
			break;
	}

	return;
}
