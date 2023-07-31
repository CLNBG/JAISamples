// AutoExposureSampleDlg.h : header file
//

#pragma once

#include <Jai_Factory.h>
#include "ALC.h"
#include "afxwin.h"

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_EXPOSURE      (int8_t*)"ExposureTimeRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"
#define NODE_NAME_EXPOSURE_MODE (int8_t*)"ExposureMode"

// CAutoExposureSampleDlg dialog
class CAutoExposureSampleDlg : public CDialog
{
    // Construction
public:
    CAutoExposureSampleDlg(CWnd* pParent = NULL);	    // standard constructor

    // Dialog Data
    enum { IDD = IDD_AUTOEXPOSURESAMPLE_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    // Implementation
public:
    FACTORY_HANDLE  m_hFactory;                         // Factory Handle
    CAM_HANDLE      m_hCam;                             // Camera Handle
    VIEW_HANDLE     m_hView;                            // View Window handle
    THRD_HANDLE     m_hThread;                          // Stream Channel handle
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];      // Camera ID
    NODE_HANDLE     m_hGainNode;                        // Handle to "GainRaw" node
    NODE_HANDLE     m_hExposureNode;                    // Handle to "ExposureTimeRaw" node

    BOOL OpenFactoryAndCamera();                    
    void CloseFactoryAndCamera();
    void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
    void InitializeControls();
    void EnableControls();
    void UpdateGainAndExposureValues();                 // Update the GUI with the current Gain and Exposure Time values

protected:
    HICON m_hIcon;

    CAutomaticLevelControl  m_ALCController;            // The Closed-loop controller

    bool m_bAutoExposure;                               // Auto Exposure ON/OFF
    bool m_bUseAverage;                                 // True if Average is used, false if Peak is used
    int m_Width;                                        // Width of image
    int m_Height;                                       // Height of image
    RECT m_MeasureRect;                                 // Area to measure Average in
    bool m_bIsCameraReady;                              // Flag used for enable/disable controls based on the current state
    bool m_bIsImageAcquiring;                           // Flag used for enable/disable controls based on the current state
    bool m_UpdateUI;                                    // Flag used for signalling that the GUI needs to be updated
    int m_OldRAverage;                                  // Cached value for the last measured average in the image
    int m_OldGAverage;                                  // Cached value for the last measured average in the image
    int m_OldBAverage;                                  // Cached value for the last measured average in the image
    int m_OldRPeak;                                     // Cached value for the last measured peak in the image
    int m_OldGPeak;                                     // Cached value for the last measured peak in the image
    int m_OldBPeak;                                     // Cached value for the last measured peak in the image

    CRITICAL_SECTION m_ControlCriticalSection;          // Critical section to ensure access to control parameters
    J_tIMAGE_INFO m_ConversionBuffer;                   // Buffer used for converting images before average is calculated

    void ShowErrorMsg(CString message, J_STATUS_TYPE error);
    J_STATUS_TYPE FindPeak(J_tIMAGE_INFO * pBufferInfo, RECT *pMeasureRect, J_tBGR48 *pRGBpeak);

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedStart();
    afx_msg void OnBnClickedStop();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnBnClickedAutoexposureCheck();
    afx_msg void OnEnChangeXposEdit();
    afx_msg void OnEnChangeYposEdit();
    afx_msg void OnEnChangeWidthEdit();
    afx_msg void OnEnChangeHeightEdit();
    afx_msg void OnEnChangeMinGainEdit();
    afx_msg void OnEnChangeMaxGainEdit();
    afx_msg void OnEnChangeMinExposureEdit();
    afx_msg void OnEnChangeMaxExposureEdit();
    afx_msg void OnCbnSelchangeAlcModeCombo();
    CComboBox m_ALCModeCtrl;
    afx_msg void OnBnClickedPeakRadio();
    afx_msg void OnBnClickedAverageRadio();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
