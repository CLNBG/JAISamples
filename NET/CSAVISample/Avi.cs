using System;
using System.Drawing;
using System.Runtime.InteropServices;

namespace AviFile {

	public class Avi{

		public static int PALETTE_SIZE = 4*256; //RGBQUAD * 256 colours

		public static readonly int streamtypeVIDEO = mmioFOURCC('v', 'i', 'd', 's');
		public static readonly int streamtypeAUDIO = mmioFOURCC('a', 'u', 'd', 's');
		public static readonly int streamtypeMIDI = mmioFOURCC('m', 'i', 'd', 's');
		public static readonly int streamtypeTEXT = mmioFOURCC('t', 'x', 't', 's');
		
		public const int OF_SHARE_DENY_WRITE = 32;
		public const int OF_WRITE			 = 1;
		public const int OF_READWRITE		 = 2;
		public const int OF_CREATE			 = 4096;

		public const int BMP_MAGIC_COOKIE = 19778; //ascii string "BM"

		public const int AVICOMPRESSF_INTERLEAVE = 0x00000001;    // interleave
		public const int AVICOMPRESSF_DATARATE = 0x00000002;    // use a data rate
		public const int AVICOMPRESSF_KEYFRAMES = 0x00000004;    // use keyframes
		public const int AVICOMPRESSF_VALID = 0x00000008;    // has valid data
		public const int AVIIF_KEYFRAME = 0x00000010;

		public const UInt32 ICMF_CHOOSE_KEYFRAME = 0x0001;	// show KeyFrame Every box
		public const UInt32 ICMF_CHOOSE_DATARATE = 0x0002;	// show DataRate box
		public const UInt32 ICMF_CHOOSE_PREVIEW  = 0x0004;	// allow expanded preview dialog

        //macro mmioFOURCC
        public static Int32 mmioFOURCC(char ch0, char ch1, char ch2, char ch3) {
            return ((Int32)(byte)(ch0) | ((byte)(ch1) << 8) |
                ((byte)(ch2) << 16) | ((byte)(ch3) << 24));
        }

		#region structure declarations

		[StructLayout(LayoutKind.Sequential, Pack=1)]
		public struct RECT{ 
			public UInt32 left; 
			public UInt32 top; 
			public UInt32 right; 
			public UInt32 bottom; 
		} 		

		[StructLayout(LayoutKind.Sequential, Pack=1)]
		public struct BITMAPINFOHEADER {
			public Int32 biSize;
			public Int32 biWidth;
			public Int32 biHeight;
			public Int16 biPlanes;
			public Int16 biBitCount;
			public Int32 biCompression;
			public Int32 biSizeImage;
			public Int32 biXPelsPerMeter;
			public Int32 biYPelsPerMeter;
			public Int32 biClrUsed;
			public Int32 biClrImportant;
		}

        public struct BITMAPINFOHEADER8BPP
        {
            public Int32 biSize;
            public Int32 biWidth;
            public Int32 biHeight;
            public Int16 biPlanes;
            public Int16 biBitCount;
            public Int32 biCompression;
            public Int32 biSizeImage;
            public Int32 biXPelsPerMeter;
            public Int32 biYPelsPerMeter;
            public Int32 biClrUsed;
            public Int32 biClrImportant;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 255*3)]
            public byte[] Palette;
        }

        [StructLayout(LayoutKind.Sequential)] 
		public struct PCMWAVEFORMAT {
			public short wFormatTag;
			public short nChannels;
			public int nSamplesPerSec;
			public int nAvgBytesPerSec;
			public short nBlockAlign;
			public short wBitsPerSample;
			public short cbSize;
		}

		[StructLayout(LayoutKind.Sequential, Pack=1)]
		public struct AVISTREAMINFO {
			public Int32    fccType;
			public Int32    fccHandler;
			public Int32    dwFlags;
			public Int32    dwCaps;
			public Int16    wPriority;
			public Int16    wLanguage;
			public Int32    dwScale;
			public Int32    dwRate;
			public Int32    dwStart;
			public Int32    dwLength;
			public Int32    dwInitialFrames;
			public Int32    dwSuggestedBufferSize;
			public Int32    dwQuality;
			public Int32    dwSampleSize;
			public RECT		rcFrame;
			public Int32    dwEditCount;
			public Int32    dwFormatChangeCount;
			[MarshalAs(UnmanagedType.ByValArray, SizeConst=64)]
			public UInt16[]    szName;
		}
		[StructLayout(LayoutKind.Sequential, Pack=1)]
		public struct BITMAPFILEHEADER{
			public Int16 bfType; //"magic cookie" - must be "BM"
			public Int32 bfSize;
			public Int16 bfReserved1;
			public Int16 bfReserved2;
			public Int32 bfOffBits;
		}

				
		[StructLayout(LayoutKind.Sequential, Pack=1)]
			public struct AVIFILEINFO{
			public Int32 dwMaxBytesPerSecond;
			public Int32 dwFlags;
			public Int32 dwCaps;
			public Int32 dwStreams;
			public Int32 dwSuggestedBufferSize;
			public Int32 dwWidth;
			public Int32 dwHeight;
			public Int32 dwScale;
			public Int32 dwRate;
			public Int32 dwLength;
			public Int32 dwEditCount;
			[MarshalAs(UnmanagedType.ByValArray, SizeConst=64)]
			public char[] szFileType;
		}

		[StructLayout(LayoutKind.Sequential, Pack=1)]
			public struct AVICOMPRESSOPTIONS {
			public UInt32   fccType;
			public UInt32   fccHandler;
			public UInt32   dwKeyFrameEvery;  // only used with AVICOMRPESSF_KEYFRAMES
			public UInt32   dwQuality;
			public UInt32   dwBytesPerSecond; // only used with AVICOMPRESSF_DATARATE
			public UInt32   dwFlags;
			public IntPtr   lpFormat;
			public UInt32   cbFormat;
			public IntPtr   lpParms;
			public UInt32   cbParms;
			public UInt32   dwInterleaveEvery;
		}

		/// <summary>AviSaveV needs a pointer to a pointer to an AVICOMPRESSOPTIONS structure</summary>
		[StructLayout(LayoutKind.Sequential, Pack=1)]
			public class AVICOMPRESSOPTIONS_CLASS {
			public UInt32   fccType;
			public UInt32   fccHandler;
			public UInt32   dwKeyFrameEvery;  // only used with AVICOMRPESSF_KEYFRAMES
			public UInt32   dwQuality;
			public UInt32   dwBytesPerSecond; // only used with AVICOMPRESSF_DATARATE
			public UInt32   dwFlags;
			public IntPtr   lpFormat;
			public UInt32   cbFormat;
			public IntPtr   lpParms;
			public UInt32   cbParms;
			public UInt32   dwInterleaveEvery;

			public AVICOMPRESSOPTIONS ToStruct(){
				AVICOMPRESSOPTIONS returnVar = new AVICOMPRESSOPTIONS();
				returnVar.fccType = this.fccType;
				returnVar.fccHandler = this.fccHandler;
				returnVar.dwKeyFrameEvery = this.dwKeyFrameEvery;
				returnVar.dwQuality = this.dwQuality;
				returnVar.dwBytesPerSecond = this.dwBytesPerSecond;
				returnVar.dwFlags = this.dwFlags;
				returnVar.lpFormat = this.lpFormat;
				returnVar.cbFormat = this.cbFormat;
				returnVar.lpParms = this.lpParms;
				returnVar.cbParms = this.cbParms;
				returnVar.dwInterleaveEvery = this.dwInterleaveEvery;
				return returnVar;
			}
		}


		#endregion structure declarations
        #region compress

        /// <summary>
        /// Values for dwFlags of ICOpen() 
        /// </summary>
        public enum ICMODE
        {
            ICMODE_COMPRESS = 1,
            ICMODE_DECOMPRESS = 2,
            ICMODE_FASTDECOMPRESS = 3,
            ICMODE_QUERY = 4,
            ICMODE_FASTCOMPRESS = 5,
            ICMODE_DRAW = 8
        }
        public class FOURCC
        {
            public static readonly int DIVX = FOURCC.mmioFOURCC('d', 'i', 'v', 'x');
            public static readonly int MP42 = FOURCC.mmioFOURCC('M', 'P', '4', '2');
            public static readonly int streamtypeVIDEO = mmioFOURCC('v', 'i', 'd', 's');
            public static readonly int streamtypeAUDIO = mmioFOURCC('a', 'u', 'd', 's');
            public static readonly int streamtypeMIDI = mmioFOURCC('m', 'i', 'd', 's');
            public static readonly int streamtypeTEXT = mmioFOURCC('t', 'x', 't', 's');
            public static readonly int ICTYPE_VIDEO = mmioFOURCC('v', 'i', 'd', 'c');
            public static readonly int ICTYPE_AUDIO = mmioFOURCC('a', 'u', 'd', 'c');
            public static readonly int ICM_FRAMERATE = mmioFOURCC('F', 'r', 'm', 'R');
            public static readonly int ICM_KEYFRAMERATE = mmioFOURCC('K', 'e', 'y', 'R');
            public static Int32 mmioFOURCC(char ch0, char ch1, char ch2, char ch3)
            {
                return ((Int32)(byte)(ch0) | ((byte)(ch1) << 8) |
                    ((byte)(ch2) << 16) | ((byte)(ch3) << 24));
            }
        }
        /// <summary>
        /// constants for the biCompression field
        /// </summary>
        public enum BI
        {
            BI_RGB,
            BI_RLE8,
            BI_RLE4,
            BI_BITFIELDS,
            BI_JPEG,
            BI_PNG,
        }

        /// <summary>
        /// Flags for index
        /// </summary>
        public enum AVIIF
        {
            AVIIF_LIST = 0x00000001, // chunk is a 'LIST'
            AVIIF_KEYFRAME = 0x00000010, // this frame is a key frame.
            AVIIF_FIRSTPART = 0x00000020, // this frame is the start of a partial frame.
            AVIIF_LASTPART = 0x00000040, // this frame is the end of a partial frame.
            AVIIF_MIDPART = 0x00000040 | 0x00000020, //(AVIIF_LASTPART|AVIIF_FIRSTPART)

            AVIIF_NOTIME = 0x00000100, // this frame doesn't take any time
            AVIIF_COMPUSE = 0x0FFF0000 // these bits are for compressor use
        }
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public class ICINFO
        {
            public int dwSize;
            public int fccType;
            public int fccHandler;
            public int dwFlags;
            public int dwVersion;
            public int dwVersionICM;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
            public string szName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string szDescription;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
            public string szDriver;
        }

        [StructLayout(LayoutKind.Sequential)]
        public class COMPVARS
        {
            public int cbSize;
            public int dwFlags;
            public IntPtr hic;
            public int fccType;
            public int fccHandler;
            public IntPtr lpbiIn;
            public IntPtr lpbiOut;
            public IntPtr lpBitsOut;
            public IntPtr lpBitsPrev;
            public int lFrame;
            public int lKey;
            public int lDataRate;
            public int lQ;
            public int lKeyCount;
            public IntPtr lpState;
            public int cbState;
        }

        [System.Flags]
        public enum ICDRAWFlag
        {
            ICDRAW_QUERY = 0x00000001,   // test for support
            ICDRAW_FULLSCREEN = 0x00000002,   // draw to full screen
            ICDRAW_HDC = 0x00000004,   // draw to a HDC/HWND
            ICDRAW_ANIMATE = 0x00000008,	  // expect palette animation
            ICDRAW_CONTINUE = 0x00000010,	  // draw is a continuation of previous draw
            ICDRAW_MEMORYDC = 0x00000020,	  // DC is offscreen, by the way
            ICDRAW_UPDATING = 0x00000040,	  // We're updating, as opposed to playing
            ICDRAW_RENDER = 0x00000080,   // used to render data not draw it
            ICDRAW_BUFFER = 0x00000100   // please buffer this data offscreen, we will need to update it
        }
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential)]
        internal struct ICDRAWBEGIN
        {
            public ICDRAWFlag dwFlags;        // flags

            public IntPtr hpal;           // palette to draw with
            public IntPtr hwnd;           // window to draw to
            public IntPtr hdc;            // HDC to draw to

            public int xDst;           // destination rectangle
            public int yDst;
            public int dxDst;
            public int dyDst;

            public IntPtr lpbi;           // format of frame to draw

            public int xSrc;           // source rectangle
            public int ySrc;
            public int dxSrc;
            public int dySrc;

            public int dwRate;         // frames/second = (dwRate/dwScale)
            public int dwScale;

        }
        #endregion

        #region video

        public delegate int capErrorCallback(
        IntPtr hWnd,
        int nID,
        string lpsz
        );
        public delegate int capVideoStreamCallback(
        IntPtr hWnd,
        ref VIDEOHDR lpVHdr
        );

        [System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential)]
        public struct VIDEOHDR
        {
            public IntPtr lpData;                 /* pointer to locked data buffer */
            public int dwBufferLength;         /* Length of data buffer */
            public int dwBytesUsed;            /* Bytes actually used */
            public int dwTimeCaptured;         /* Milliseconds from start of stream */
            public int dwUser;                 /* for client's use */
            public int dwFlags;                /* assorted flags (see defines) */
            public int dwReserved;          /* reserved for driver */
        }
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential)]
        public struct CaptureParms
        {
            public int dwRequestMicroSecPerFrame;  // Requested capture rate
            public bool fMakeUserHitOKToCapture;    // Show "Hit OK to cap" dlg?
            public uint wPercentDropForError;       // Give error msg if > (10%)
            public bool fYield;                     // Capture via background task?
            public int dwIndexSize;                // Max index size in frames (32K)
            public uint wChunkGranularity;          // Junk chunk granularity (2K)
            public bool fUsingDOSMemory;            // Use DOS buffers?
            public uint wNumVideoRequested;         // # video buffers, If 0, autocalc
            public bool fCaptureAudio;              // Capture audio?
            public uint wNumAudioRequested;         // # audio buffers, If 0, autocalc
            public uint vKeyAbort;                  // Virtual key causing abort
            public bool fAbortLeftMouse;            // Abort on left mouse?
            public bool fAbortRightMouse;           // Abort on right mouse?
            public bool fLimitEnabled;              // Use wTimeLimit?
            public uint wTimeLimit;                 // Seconds to capture
            public bool fMCIControl;                // Use MCI video source?
            public bool fStepMCIDevice;             // Step MCI device?
            public int dwMCIStartTime;             // Time to start in MS
            public int dwMCIStopTime;              // Time to stop in MS
            public bool fStepCaptureAt2x;           // Perform spatial averaging 2x
            public int wStepCaptureAverageFrames;  // Temporal average n Frames
            public int dwAudioBufferSize;          // Size of audio bufs (0 = default)
            public int fDisableWriteCache;         // Attempt to disable write cache
            public int AVStreamMaster;             // Which stream controls length?
        }
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential)]
        public struct BITMAPINFO
        {
            public BITMAPINFOHEADER bmiHeader;
            public int bmiColors;
        }
        [System.Runtime.InteropServices.StructLayout(LayoutKind.Sequential)]
        public struct CAPSTATUS
        {
            public int uiImageWidth;               // Width of the image
            public int uiImageHeight;              // Height of the image
            public bool fLiveWindow;                // Now Previewing video?
            public bool fOverlayWindow;             // Now Overlaying video?
            public bool fScale;                     // Scale image to client?
            public Point ptScroll;                   // Scroll position
            public bool fUsingDefaultPalette;       // Using default driver palette?
            public bool fAudioHardware;             // Audio hardware present?
            public bool fCapFileExists;             // Does capture file exist?
            public int dwCurrentVideoFrame;        // # of video frames cap'td
            public int dwCurrentVideoFramesDropped;// # of video frames dropped
            public int dwCurrentWaveSamples;       // # of wave samples cap'td
            public int dwCurrentTimeElapsedMS;     // Elapsed capture duration
            public IntPtr hPalCurrent;                // Current palette in use
            public bool fCapturingNow;              // Capture in progress?
            public int dwReturn;                   // Error value after any operation
            public int wNumVideoAllocated;         // Actual number of video buffers
            public int wNumAudioAllocated;         // Actual number of audio buffers
        }
        #endregion


		#region method declarations

        public const uint FACILITY_ITF   = 0x00040000;
        public const uint SEVERITY_ERROR = 0x80000000;
        public const uint AVI_ERROR      = 0x00004000;

        public const uint AVIERR_UNSUPPORTED        = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 101;
        public const uint AVIERR_BADFORMAT          = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 102;
        public const uint AVIERR_MEMORY             = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 103;
        public const uint AVIERR_INTERNAL           = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 104;
        public const uint AVIERR_BADFLAGS           = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 105;
        public const uint AVIERR_BADPARAM           = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 106;
        public const uint AVIERR_BADSIZE            = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 107;
        public const uint AVIERR_BADHANDLE          = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 108;
        public const uint AVIERR_FILEREAD           = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 109;
        public const uint AVIERR_FILEWRITE          = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 110;
        public const uint AVIERR_FILEOPEN           = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 111;
        public const uint AVIERR_COMPRESSOR         = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 112;
        public const uint AVIERR_NOCOMPRESSOR       = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 113;
        public const uint AVIERR_READONLY           = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 114;
        public const uint AVIERR_NODATA             = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 115;
        public const uint AVIERR_BUFFERTOOSMALL     = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 116;
        public const uint AVIERR_CANTCOMPRESS       = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 117;
        public const uint AVIERR_USERABORT          = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 198;
        public const uint AVIERR_ERROR              = SEVERITY_ERROR | FACILITY_ITF | AVI_ERROR | 199;
        public const uint REGDB_E_CLASSNOTREG       = SEVERITY_ERROR | FACILITY_ITF | 0x154;

        public static string FormatAviErrorMessage(int errorcode)
        {
            string msg = "Unknown AVI error code";
            switch ((uint)errorcode)
            {
                case 0: msg = "Success"; break;
                case AVIERR_BADFORMAT: msg = "AVIERR_BADFORMAT: corrupt file or unrecognized format"; break;
                case AVIERR_MEMORY: msg = "AVIERR_MEMORY: insufficient memory"; break;
                case AVIERR_FILEREAD: msg = "AVIERR_FILEREAD: disk error while reading file"; break;
                case AVIERR_FILEOPEN: msg = "AVIERR_FILEOPEN: disk error while opening file"; break;
                case REGDB_E_CLASSNOTREG: msg = "REGDB_E_CLASSNOTREG: file type not recognised"; break;
                case AVIERR_READONLY: msg = "AVIERR_READONLY: file is read-only"; break;
                case AVIERR_NOCOMPRESSOR: msg = "AVIERR_NOCOMPRESSOR: a suitable compressor could not be found"; break;
                case AVIERR_UNSUPPORTED: msg = "AVIERR_UNSUPPORTED: compression is not supported for this type of data"; break;
                case AVIERR_INTERNAL: msg = "AVIERR_INTERNAL: internal error"; break;
                case AVIERR_BADFLAGS: msg = "AVIERR_BADFLAGS"; break;
                case AVIERR_BADPARAM: msg = "AVIERR_BADPARAM"; break;
                case AVIERR_BADSIZE: msg = "AVIERR_BADSIZE"; break;
                case AVIERR_BADHANDLE: msg = "AVIERR_BADHANDLE"; break;
                case AVIERR_FILEWRITE: msg = "AVIERR_FILEWRITE: disk error while writing file"; break;
                case AVIERR_COMPRESSOR: msg = "AVIERR_COMPRESSOR"; break;
                case AVIERR_NODATA: msg = "AVIERR_READONLY"; break;
                case AVIERR_BUFFERTOOSMALL: msg = "AVIERR_BUFFERTOOSMALL"; break;
                case AVIERR_CANTCOMPRESS: msg = "AVIERR_CANTCOMPRESS"; break;
                case AVIERR_USERABORT: msg = "AVIERR_USERABORT"; break;
                case AVIERR_ERROR: msg = "AVIERR_ERROR"; break;
                case 1: msg = "Unable to close the AVI file properly!"; break;
            }
            return msg;
        }

		//Initialize the AVI library
		[DllImport("avifil32.dll")]
		public static extern void AVIFileInit();

		//Open an AVI file
		[DllImport("avifil32.dll", PreserveSig=true)]
		public static extern int AVIFileOpen(
			ref int ppfile,
			String szFile,
			int uMode,
			int pclsidHandler);

		//Get a stream from an open AVI file
		[DllImport("avifil32.dll")]
		public static extern int AVIFileGetStream(
			int pfile,
			out IntPtr ppavi,  
			int fccType,       
			int lParam);

		//Get the start position of a stream
		[DllImport("avifil32.dll", PreserveSig=true)]
		public static extern int AVIStreamStart(int pavi);

		//Get the length of a stream in frames
		[DllImport("avifil32.dll", PreserveSig=true)]
		public static extern int AVIStreamLength(int pavi);

		//Get information about an open stream
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamInfo(
			IntPtr pAVIStream,
			ref AVISTREAMINFO psi,
			int lSize);

		//Get a pointer to a GETFRAME object (returns 0 on error)
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamGetFrameOpen(
			IntPtr pAVIStream,
			ref BITMAPINFOHEADER bih);

		//Get a pointer to a packed DIB (returns 0 on error)
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamGetFrame(
			int pGetFrameObj,
			int lPos);

		//Create a new stream in an open AVI file
		[DllImport("avifil32.dll")]
		public static extern int AVIFileCreateStream(
			int pfile,
			out IntPtr ppavi, 
			ref AVISTREAMINFO ptr_streaminfo);

        //Create an editable stream
        [DllImport("avifil32.dll")]
        public static extern int CreateEditableStream(
            ref IntPtr ppsEditable,
            IntPtr psSource
        );

        //Cut samples from an editable stream
        [DllImport("avifil32.dll")]
        public static extern int EditStreamCut(
            IntPtr pStream,
            ref Int32 plStart,
            ref Int32 plLength,
            ref IntPtr ppResult
        );

        //Copy a part of an editable stream
        [DllImport("avifil32.dll")]
        public static extern int EditStreamCopy(
            IntPtr pStream,
            ref Int32 plStart,
            ref Int32 plLength,
            ref IntPtr ppResult
        );

        //Paste an editable stream into another editable stream
        [DllImport("avifil32.dll")]
        public static extern int EditStreamPaste(
            IntPtr pStream,
            ref Int32 plPos,
            ref Int32 plLength,
            IntPtr pstream,
            Int32 lStart,
            Int32 lLength
        );

        //Change a stream's header values
        [DllImport("avifil32.dll")]
        public static extern int EditStreamSetInfo(
            IntPtr pStream,
            ref AVISTREAMINFO lpInfo,
            Int32 cbInfo
        );

        [DllImport("avifil32.dll")]
        public static extern int AVIMakeFileFromStreams(
            ref IntPtr ppfile,
            int nStreams,
            ref IntPtr papStreams
        );

        //Set the format for a new stream
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamSetFormat(
			IntPtr aviStream, Int32 lPos, 
			ref BITMAPINFOHEADER lpFormat, Int32 cbFormat);

        [DllImport("avifil32.dll")]
        public static extern int AVIStreamSetFormat(
            IntPtr aviStream, Int32 lPos,
            IntPtr lpFormat, Int32 cbFormat);

        //Set the format for a new stream
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamSetFormat(
			IntPtr aviStream, Int32 lPos, 
			ref PCMWAVEFORMAT lpFormat, Int32 cbFormat);
		
		//Read the format for a stream
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamReadFormat(
			IntPtr aviStream, Int32 lPos,
			ref BITMAPINFOHEADER lpFormat, ref Int32 cbFormat
			);

		//Read the size of the format for a stream
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamReadFormat(
			IntPtr aviStream, Int32 lPos,
			int empty, ref Int32 cbFormat
			);
		
		//Read the format for a stream
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamReadFormat(
			IntPtr aviStream, Int32 lPos,
			ref PCMWAVEFORMAT lpFormat, ref Int32 cbFormat
			);

		//Write a sample to a stream
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamWrite(
			IntPtr aviStream, Int32 lStart, Int32 lSamples, 
			IntPtr lpBuffer, Int32 cbBuffer, Int32 dwFlags, 
			Int32 dummy1, Int32 dummy2);

		//Release the GETFRAME object
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamGetFrameClose(
			int pGetFrameObj);

		//Release an open AVI stream
		[DllImport("avifil32.dll")]
		public static extern int AVIStreamRelease(IntPtr aviStream);

		//Release an open AVI file
		[DllImport("avifil32.dll")]
		public static extern int AVIFileRelease(int pfile);

		//Close the AVI library
		[DllImport("avifil32.dll")]
		public static extern void AVIFileExit();

		[DllImport("avifil32.dll")]
		public static extern int AVIMakeCompressedStream(
			out IntPtr ppsCompressed, IntPtr aviStream, 
			ref AVICOMPRESSOPTIONS ao, IntPtr dummy);

		[DllImport("avifil32.dll")]
		public static extern bool AVISaveOptions(
			IntPtr hwnd,
			UInt32 uiFlags,              
			Int32 nStreams,                      
			ref IntPtr ppavi,
			ref AVICOMPRESSOPTIONS_CLASS plpOptions  
			);

		[DllImport("avifil32.dll")]
		public static extern long AVISaveOptionsFree(
			int nStreams,
			ref AVICOMPRESSOPTIONS_CLASS plpOptions  
			);

		[DllImport("avifil32.dll")]
		public static extern int AVIFileInfo(
			int pfile, 
			ref AVIFILEINFO pfi,
			int lSize);

		[DllImport("winmm.dll", EntryPoint="mmioStringToFOURCCA")]
		public static extern int mmioStringToFOURCC(String sz, int uFlags);

		[DllImport("avifil32.dll")]
		public static extern int AVIStreamRead(
			IntPtr pavi, 
			Int32 lStart,     
			Int32 lSamples,   
			IntPtr lpBuffer, 
			Int32 cbBuffer,   
			Int32  plBytes,  
			Int32  plSamples 
			);

		[DllImport("avifil32.dll")]
		public static extern int AVISaveV(
			String szFile,
			Int16 empty,
			Int16 lpfnCallback,
			Int16 nStreams,
			ref IntPtr ppavi,
			ref AVICOMPRESSOPTIONS_CLASS plpOptions
			);


        [DllImport("MSVFW32.dll")]
        public static extern void ICSeqCompressFrameEnd(ref COMPVARS pc);
        /*	[DllImport("MSVFW32.dll")]
    public static extern IntPtr ICSeqCompressFrame(
                 COMPVARS pc,  
                 int uiFlags,  
                 IntPtr lpBits, 
                 ref bool pfKey,  
                 ref int plSize  
                 );*/
        [DllImport("MSVFW32.dll")]
        public static extern int ICCompressGetFormatSize(
            int hic,
            ref BITMAPINFO lpbiInput
            );

        [DllImport("MSVFW32.dll")]
        public static extern bool ICSeqCompressFrameStart(
            COMPVARS pc,
            ref BITMAPINFO lpbiIn
            );
        [DllImport("MSVFW32.dll")]
        public static extern int ICSeqCompressFrame(
            COMPVARS pc,
            int uiFlags,
            byte[] lpBits,
            ref bool pfKey,
            ref long plSize
            );
        [DllImport("MSVFW32.dll", CharSet = CharSet.Ansi)]
        public static extern int ICGetInfo(
            int hic,
            ICINFO lpicinfo,
            int cb
            );
        [DllImport("MSVFW32.dll")]
        public static extern bool ICCompressorChoose(
            IntPtr hwnd,
            int uiFlags,
            ref BITMAPINFOHEADER pvIn,   
            IntPtr lpData,
            COMPVARS pc,
            String lpszTitle
            );
        [DllImport("MSVFW32.dll")]
        public static extern bool ICCompressorChoose(
            IntPtr hwnd,
            int uiFlags,
            IntPtr pvIn,
            IntPtr lpData,
            COMPVARS pc,
            String lpszTitle
            );
        [DllImport("MSVFW32.dll")]
        public static extern void ICCompressorFree(
            COMPVARS pc
            );
        [DllImport("MSVFW32.dll")]
        public static extern int ICLocate(
            int fccType,
            int fccHandler,
            ref BITMAPINFOHEADER lpbiIn,
            ref BITMAPINFOHEADER lpbiOut,
            short wFlags
            );
        [DllImport("MSVFW32.dll"), PreserveSig]
        public static extern int ICOpen(int fccType, int fccHandler, ICMODE wMode);
        [DllImport("MSVFW32.dll")]
        public static extern int ICClose(int hic);
        [DllImport("MSVFW32.dll")]
        public static extern int ICCompress(
            int hic,
            int dwFlags,        // flags
            ref BITMAPINFOHEADER lpbiOutput,     // output format
            IntPtr lpData,         // output data
            ref BITMAPINFOHEADER lpbiInput,      // format of frame to compress
            IntPtr lpBits,         // frame data to compress
            int lpckid,         // ckid for data in AVI file
            int lpdwFlags,      // flags in the AVI index.
            int lFrameNum,      // frame number of seq.
            int dwFrameSize,    // reqested size in bytes. (if non zero)
            int dwQuality,      // quality within one frame
            //	BITMAPINFOHEADER  lpbiPrev,       // format of previous frame
            int lpbiPrev,       // format of previous frame
            int lpPrev          // previous frame
            );
        [DllImport("MSVFW32.dll")]
        public static extern int ICDecompress(
            IntPtr hic,
            int dwFlags,
            ref BITMAPINFOHEADER lpbiFormat,
            byte[] lpData,
            ref BITMAPINFOHEADER lpbi,
            byte[] lpBits
            );
        [DllImport("MSVFW32.dll")]
        public static extern int ICSendMessage(IntPtr hic, int msg, ref BITMAPINFO dw1, ref BITMAPINFO dw2);
        [DllImport("MSVFW32.dll")]
        public static extern int ICSendMessage(IntPtr hic, int msg, int dw1, int dw2);
        [DllImport("MSVFW32.dll")]
        public static extern int ICSendMessage(IntPtr hic, int msg, ICINFO dw1, int dw2);
        [DllImport("MSVFW32.dll")]
        public static extern int ICSendMessage(IntPtr hic, int msg, IntPtr dw1, int dw2);
        public static readonly int DRV_USER = 0x4000;
        public static readonly int ICM_USER = (DRV_USER + 0x0000);
        public static readonly int ICM_GETSTATE = (ICM_USER + 0);
        public static readonly int ICM_SETSTATE = (ICM_USER + 1);
        public static readonly int ICM_COMPRESS_BEGIN = (ICM_USER + 7);    // begin a series of compress calls.
        public static readonly int ICM_COMPRESS = (ICM_USER + 8);   // compress a frame
        public static readonly int ICM_COMPRESS_END = (ICM_USER + 9);   // end of a series of compress calls.
        public static readonly int ICM_COMPRESS_GET_FORMAT = (ICM_USER + 4);
        public static readonly int ICM_DECOMPRESS_BEGIN = (ICM_USER + 12);   // start a series of decompress calls
        public static readonly int ICM_DECOMPRESS = (ICM_USER + 13);   // decompress a frame
        public static readonly int ICM_DECOMPRESS_END = (ICM_USER + 14);

		#endregion method declarations

	}
}
