// VCAVISampleDlg.h : header file
//

#pragma once
#include <Jai_Factory.h>

#include <vfw.h>

#define NODE_NAME_WIDTH         (int8_t*)"Width"
#define NODE_NAME_HEIGHT        (int8_t*)"Height"
#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

// First, we'll define the WAV file format.
#include <pshpack1.h>
typedef struct
{ 
    char id[4];                     //="fmt "
    unsigned long size;             //=16
    short wFormatTag;               //=WAVE_FORMAT_PCM=1
    unsigned short wChannels;       //=1 or 2 for mono or stereo
    unsigned long dwSamplesPerSec;  //=11025 or 22050 or 44100
    unsigned long dwAvgBytesPerSec; //=wBlockAlign * dwSamplesPerSec
    unsigned short wBlockAlign;     //=wChannels * (wBitsPerSample==8?1:2)
    unsigned short wBitsPerSample;  //=8 or 16, for bits per sample
} FmtChunk;

typedef struct
{ 
    char id[4];            //="data"
    unsigned long size;    //=datsize, size of the following array
    unsigned char data[1]; //=the raw data goes here
} DataChunk;

typedef struct
{ 
    char id[4];         //="RIFF"
    unsigned long size; //=datsize+8+16+4
    char type[4];       //="WAVE"
    FmtChunk fmt;
    DataChunk dat;
} WavChunk;
#include <poppack.h>

// This is the internal structure represented by the HAVI handle:
typedef struct
{ 
    IAVIFile *pfile;                // Created by CreateAvi
    WAVEFORMATEX wfx;               // As given to CreateAvi (.nChanels=0 if none was given). Used when audio stream is first created.
    int period;                     // Specified in CreateAvi, used when the video stream is first created
    IAVIStream *as;                 // Audio stream, initialised when audio stream is first created
    IAVIStream *ps, *psCompressed;  // Video stream, when first created
    unsigned long nframe, nsamp;    // Which frame will be added next, which sample will be added next
    bool iserr;                     // If true, then no function will do anything
} TAviUtil;

typedef HANDLE HAVI;

// CVCAVISampleDlg dialog
class CVCAVISampleDlg : public CDialog
{
// Construction
public:
	CVCAVISampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VCAVISAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
public:
 	FACTORY_HANDLE  m_hFactory;     // Factory Handle
	CAM_HANDLE      m_hCam;         // Camera Handle
    VIEW_HANDLE     m_hView;
	THRD_HANDLE     m_hThread;
    int8_t          m_sCameraId[J_CAMERA_ID_SIZE];      // Camera ID
	int64_t			m_iWidthInc;
	int64_t			m_iHeightInc;
	bool			m_bEnableStreaming;

    HAVI            m_hAVI;                             // Handle to the AVI
    J_tIMAGE_INFO  *m_pDIBBufferInfo;
	uint8_t        *m_pOldBuffer;                       // Copy of the buffer allocated using J_MallocDIB()
    LPBYTE          m_pBits;                            // Pointer to the image data in DIBSection
    HBITMAP         m_hBmp;                             // Handle to the DIBSection
    CString         m_AviFileName;
    bool            m_bIsAviFileReady;
    bool            m_Use8BitMonoForOptimization;
    byte            m_Mono8BitmapInfo[1064/*sizeof(BITMAPINFOHEADER)+(256*sizeof(RGBQUAD))*/];

    // Image Scaling helpers
    HDC             m_hMemoryDC;
    HBITMAP         m_hMemoryBitmap;
    BITMAPINFO     *m_pBmi;

    bool            m_bResizeImage;
    int             m_iResizeImageWidth;
    int             m_iResizeImageHeight;

    uint32_t m_RGain;   // Red channel gain. The gain is calculates as (m_xGain/4096) during White Balance calculation
    uint32_t m_GGain;   // Green channel gain (This should always be kept at 4096 - a gain of 1.0)
    uint32_t m_BGain;   // Blue channel gain

    bool m_Toggle;

    BOOL OpenFactoryAndCamera();
    void CloseFactoryAndCamera();
	void StreamCBFunc(J_tIMAGE_INFO * pAqImageInfo);
    void InitializeControls();
    void EnableControls(BOOL bIsCameraReady, BOOL bIsImageAcquiring);
    BOOL CreateDIBSectionBitmap(HBITMAP *pBmpHandle, void **ppBuffer, int Width, int Height, BITMAPINFO *pBmi, bool Mono8Format);
    void ShowErrorMsg(CString message, J_STATUS_TYPE error);

    // AVI functions:
    // CreateAvi - call this to start the creation of the avi file.
    // The period is the number of ms between each bitmap frame.
    // The waveformat can be null if you're not going to add any audio,
    // or if you're going to add audio from a file.
    HAVI CreateAvi(LPCTSTR fn, int frameperiod, const WAVEFORMATEX *wfx);

    // AddAviFrame - adds this bitmap to the avi file. hbm must point be a DIBSection.
    // It is the callers responsibility to free the hbm.
    HRESULT AddAviFrame(HAVI avi, HBITMAP hbm);

    // AddAviAudio - adds this junk of audio. The format of audio was as specified in the
    // wfx parameter to CreateAVI. This fails if NULL was given.
    // Both return S_OK if okay, otherwise one of the AVI errors.
    HRESULT AddAviAudio(HAVI avi, void *dat, unsigned long numbytes);

    // AddAviWav - a convenient way to add an entire wave file to the avi.
    // The wav file may be in in memory (in which case flags=SND_MEMORY)
    // or a file on disk (in which case flags=SND_FILENAME).
    // This function requires that either a null WAVEFORMATEX was passed to CreateAvi,
    // or that the wave file now being added has the same format as was
    // added earlier.
    HRESULT AddAviWav(HAVI avi, LPCTSTR wav, DWORD flags);

    // SetAviVideoCompression - allows compression of the video. If compression is desired,
    // then this function must have been called before any bitmap frames had been added.
    // The bitmap hbm must be a DIBSection (so that avi knows what format/size you're giving it),
    // but won't actually be added to the movie.
    // This function can display a dialog box to let the user choose compression. In this case,
    // set ShowDialog to true and specify the parent window. If opts is non-NULL and its
    // dwFlags property includes AVICOMPRESSF_VALID, then opts will be used to give initial
    // values to the dialog. If opts is non-NULL then the chosen options will be placed in it.
    // This function can also be used to choose a compression without a dialog box. In this
    // case, set ShowDialog to false, and hparent is ignored, and the compression specified
    // in 'opts' is used, and there's no need to call GotAviVideoCompression afterwards.
    HRESULT SetAviVideoCompression(HAVI avi, HBITMAP hbm, AVICOMPRESSOPTIONS *opts, bool ShowDialog, HWND hparent);

    // CloseAvi - the avi must be closed with this message.
    HRESULT CloseAvi(HAVI avi);

    // FormatAviMessage - given an error code, formats it as a string.
    // It returns the length of the error message. If buf/len points
    // to a real buffer, then it also writes as much as possible into there.
    unsigned int FormatAviMessage(HRESULT code, TCHAR *buf,unsigned int len);

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnDestroy();
public:
  afx_msg void OnBnClickedStart();
public:
  afx_msg void OnBnClickedStop();
public:
  afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
  afx_msg void OnEnChangeFrameRateEdit();
  afx_msg void OnBnClickedPlayAvi();
  afx_msg void OnBnClickedImageResizeCheck();
  afx_msg void OnEnChangeWidthEdit();
  afx_msg void OnEnChangeHeightEdit();
  afx_msg void OnBnClickedCancel();
  afx_msg void OnBnClickedOk();
};
