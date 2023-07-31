using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;
using System.Threading;
using System.Runtime.InteropServices;

namespace LensDistortionCorrectionSample
{
    public partial class Form1 : Form
    {
        // Main factory object
        CFactory myFactory = new CFactory();

        // Opened camera object
        CCamera myCamera;

        // GenICam nodes
        CNode myWidthNode;
        CNode myHeightNode;
        CNode myGainNode;

        uint myRedGain = 4096;
        uint myGreenGain = 4096;
        uint myBlueGain = 4096;

        IntPtr WindowHandle;

        Jai_FactoryWrapper.ImageInfo myConversionBuffer = new Jai_FactoryWrapper.ImageInfo();
        Jai_FactoryWrapper.ImageInfo myImageProcessingBuffer = new Jai_FactoryWrapper.ImageInfo();
        Jai_FactoryWrapper.UndistortStruct myUndistortParameters = new Jai_FactoryWrapper.UndistortStruct();
        object myUndistortParametersLock = new object();

        bool mybUndistort = true;
        bool mybInitialized = false;
        bool mybMonochrome = false;

        public Form1()
        {
            InitializeComponent();

            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Open the factory with the default Registry database
            error = myFactory.Open("");

            // Search for cameras and update all controls
            SearchButton_Click(null, null);
            mybInitialized = true;
        }

        private void WidthNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (myWidthNode != null)
            {
                myWidthNode.Value = int.Parse(WidthNumericUpDown.Value.ToString());
                SetFramegrabberValue("Width", (Int64)myWidthNode.Value);
            }
        }

        private void HeightNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (myHeightNode != null)
            {
                myHeightNode.Value = int.Parse(HeightNumericUpDown.Value.ToString());
                SetFramegrabberValue("Height", (Int64)myHeightNode.Value);
            }
        }

        private void GainTrackBar_Scroll(object sender, EventArgs e)
        {
            if (myGainNode != null)
                myGainNode.Value = int.Parse(GainTrackBar.Value.ToString());

            GainLabel.Text = myGainNode.Value.ToString();
        }

        private void StartButton_Click(object sender, EventArgs e)
        {
            if (myCamera != null)
            {
                Int32 Width = Convert.ToInt32(myCamera.GetNode("Width").Value);
                Int32 Height = Convert.ToInt32(myCamera.GetNode("Height").Value);

                myUndistortParameters.OpticalCenterX = Width / 2;
                myUndistortParameters.OpticalCenterY = Height / 2;
                myUndistortParameters.FocalLengthX = Width;
                myUndistortParameters.FocalLengthY = Width;
                myUndistortParameters.CoeffK1 = -0.194;
                myUndistortParameters.CoeffK2 = 0.115;
                myUndistortParameters.CoeffK3 = 0.0; // Only used if it is a fish-eye lens
                myUndistortParameters.CoeffP1 = -0.000619;
                myUndistortParameters.CoeffP2 = 0.00115;
                myUndistortParameters.InterpolationType = Jai_FactoryWrapper.EInterpolationType.InterpolationBiLinear;

                CNode.IEnumValue PixelFormatEnumerationValue = myCamera.GetNode("PixelFormat").Value as CNode.IEnumValue;
                Jai_FactoryWrapper.EPixelFormatType PixelFormatType = (Jai_FactoryWrapper.EPixelFormatType)PixelFormatEnumerationValue.Value;

                // In order to optimize for speed then we know that the MONO8, MONO10, MONO12 and MONO16 pixel formats
                // will be able to be processed without further conversions. The rest (Bayer, RGB and "packed" Mono)
                // will have to be converted using J_Image_FromRawToImage() or J_Image_FromRawToDIB()!
                string strType = PixelFormatEnumerationValue.Name.ToLower();
                if (strType.Contains("mono"))
                {
                    mybMonochrome = true;
                }
                else
                {
                    mybMonochrome = false;
                }

                Jai_FactoryWrapper.POINT pnt = new Jai_FactoryWrapper.POINT(10, 10);

                Jai_FactoryWrapper.RECT rect = new Jai_FactoryWrapper.RECT(0, 0, Width, Height);
                Jai_FactoryWrapper.SIZE maxSize = new Jai_FactoryWrapper.SIZE(Width, Height);

                // Open the live view
                Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

                error = Jai_FactoryWrapper.J_Image_OpenViewWindowEx(Jai_FactoryWrapper.EIVWWindowType.OverlappedStretch, "Output", ref rect, ref maxSize, IntPtr.Zero, out WindowHandle);

                myCamera.NewImageDelegate += new Jai_FactoryWrapper.ImageCallBack(HandleImage);
                myCamera.StartImageAcquisition(true, 5);
                StartButton.Enabled = false;
                StopButton.Enabled = true;
            }
        }

        private void StopButton_Click(object sender, EventArgs e)
        {
            if (myCamera != null)
            {
                myCamera.StopImageAcquisition();
                myCamera.NewImageDelegate -= new Jai_FactoryWrapper.ImageCallBack(HandleImage);

                lock (myUndistortParametersLock)
                {
                    // Free the internal undistortion map if it has been initialized
                    Jai_FactoryWrapper.J_Image_ProcessingFree(ref myConversionBuffer, ref myUndistortParameters);
                }

                if (myImageProcessingBuffer.ImageBuffer != IntPtr.Zero)
                    Jai_FactoryWrapper.J_Image_Free(ref myImageProcessingBuffer);

                myImageProcessingBuffer.ImageBuffer = IntPtr.Zero;

                if (myConversionBuffer.ImageBuffer != IntPtr.Zero)
                    Jai_FactoryWrapper.J_Image_Free(ref myConversionBuffer);

                myConversionBuffer.ImageBuffer = IntPtr.Zero;

                if (WindowHandle != IntPtr.Zero)
                {
                    // Close view window
                    Jai_FactoryWrapper.J_Image_CloseViewWindow(WindowHandle);
                    WindowHandle = IntPtr.Zero;
                }
                StartButton.Enabled = true;
                StopButton.Enabled = false;
            }
        }

        private void SearchButton_Click(object sender, EventArgs e)
        {
            if (null != myCamera)
            {
                if (myCamera.IsOpen)
                {
                    myCamera.Close();
                }

                myCamera = null;
            }

            // Discover GigE and/or generic GenTL devices using myFactory.UpdateCameraList(in this case specifying Filter Driver for GigE cameras).
            myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);

            // Open the camera - first check for GigE devices
            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                myCamera = myFactory.CameraList[i];
                if (Jai_FactoryWrapper.EFactoryError.Success == myCamera.Open())
                {
                    break;
                }
            }

            if (null != myCamera && myCamera.IsOpen)
            {
                CameraIDTextBox.Text = myCamera.CameraID;

                myCamera.GetGain(ref myRedGain, ref myGreenGain, ref myBlueGain);

                if (myCamera.NumOfDataStreams > 0)
                {
                    StartButton.Enabled = true;
                    StopButton.Enabled = true;
                    undistortParamGroupBox.Enabled = true;
                }
                else
                {
                    StartButton.Enabled = false;
                    StopButton.Enabled = false;
                    undistortParamGroupBox.Enabled = false;
                }

                int currentValue = 0;

                // Get the Width GenICam Node
                myWidthNode = myCamera.GetNode("Width");
                if (myWidthNode != null)
                {
                    currentValue = int.Parse(myWidthNode.Value.ToString());

                    // Update range for the Numeric Up/Down control
                    // Convert from integer to Decimal type
                    WidthNumericUpDown.Maximum = decimal.Parse(myWidthNode.Max.ToString());
                    WidthNumericUpDown.Minimum = decimal.Parse(myWidthNode.Min.ToString());
                    WidthNumericUpDown.Value = decimal.Parse(currentValue.ToString());

                    WidthNumericUpDown.Enabled = true;
                }
                else
                    WidthNumericUpDown.Enabled = false;

                SetFramegrabberValue("Width", (Int64)myWidthNode.Value);

                // Get the Height GenICam Node
                myHeightNode = myCamera.GetNode("Height");
                if (myHeightNode != null)
                {
                    currentValue = int.Parse(myHeightNode.Value.ToString());

                    // Update range for the Numeric Up/Down control
                    // Convert from integer to Decimal type
                    HeightNumericUpDown.Maximum = decimal.Parse(myHeightNode.Max.ToString());
                    HeightNumericUpDown.Minimum = decimal.Parse(myHeightNode.Min.ToString());
                    HeightNumericUpDown.Value = decimal.Parse(currentValue.ToString());

                    HeightNumericUpDown.Enabled = true;
                }
                else
                    HeightNumericUpDown.Enabled = false;

                SetFramegrabberValue("Height", (Int64)myHeightNode.Value);

                SetFramegrabberPixelFormat();

                // Get the GainRaw GenICam Node
                myGainNode = myCamera.GetNode("GainRaw");
                if (myGainNode != null)
                {
                    currentValue = int.Parse(myGainNode.Value.ToString());

                    // Update range for the TrackBar Controls
                    GainTrackBar.Maximum = int.Parse(myGainNode.Max.ToString());
                    GainTrackBar.Minimum = int.Parse(myGainNode.Min.ToString());
                    GainTrackBar.Value = currentValue;
                    GainLabel.Text = myGainNode.Value.ToString();

                    GainLabel.Enabled = true;
                    GainTrackBar.Enabled = true;
                }
                else
                {
                    GainLabel.Enabled = false;
                    GainTrackBar.Enabled = false;
                }

                if (null == myGainNode || null == myWidthNode || null == myHeightNode)
                {
                    return;
                }

                myUndistortParameters.OpticalCenterX = Convert.ToDouble(myWidthNode.Value) / 2.0;
                myUndistortParameters.OpticalCenterY = Convert.ToDouble(myHeightNode.Value) / 2.0;
                myUndistortParameters.FocalLengthX = Convert.ToDouble(myWidthNode.Value);
                myUndistortParameters.FocalLengthY = myUndistortParameters.FocalLengthX;
                myUndistortParameters.CoeffK1 = -0.194;
                myUndistortParameters.CoeffK2 = 0.115;
                myUndistortParameters.CoeffK3 = 0.0; // Only used if it is a fish-eye lens
                myUndistortParameters.CoeffP1 = -0.000619;
                myUndistortParameters.CoeffP2 = 0.00115;
                myUndistortParameters.InterpolationType = Jai_FactoryWrapper.EInterpolationType.InterpolationBiLinear;

                opticalCenterXNumericUpDown.Value = Convert.ToDecimal(myUndistortParameters.OpticalCenterX);
                opticalCenterYNumericUpDown.Value = Convert.ToDecimal(myUndistortParameters.OpticalCenterY);
                focalLengthXNumericUpDown.Value = Convert.ToDecimal(myUndistortParameters.FocalLengthX);
                focalLengthYNumericUpDown.Value = Convert.ToDecimal(myUndistortParameters.FocalLengthY);
                k1NumericUpDown.Value = Convert.ToDecimal(myUndistortParameters.CoeffK1);
                k2NumericUpDown.Value = Convert.ToDecimal(myUndistortParameters.CoeffK2);
                k3NumericUpDown.Value = Convert.ToDecimal(myUndistortParameters.CoeffK3);
                p1NumericUpDown.Value = Convert.ToDecimal(myUndistortParameters.CoeffP1);
                p2NumericUpDown.Value = Convert.ToDecimal(myUndistortParameters.CoeffP2);

                if (myUndistortParameters.InterpolationType == Jai_FactoryWrapper.EInterpolationType.InterpolationBiLinear)
                    biLinearCheckBox.Checked = true;
            }
            else
            {
                StartButton.Enabled = false;
                StopButton.Enabled = false;
                WidthNumericUpDown.Enabled = false;
                HeightNumericUpDown.Enabled = true;
                GainLabel.Enabled = false;
                GainTrackBar.Enabled = false;

                MessageBox.Show("No Cameras Found!");
            }
        }

        // Local callback function used for handle new images
        void HandleImage(ref Jai_FactoryWrapper.ImageInfo ImageInfo)
        {
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            if (myImageProcessingBuffer.ImageBuffer == IntPtr.Zero)
            {
                if (!mybMonochrome)
                {
                    // Allocate buffer to convert to "Image" format or "DIB" format
                    // To allocate for Image format we use J_Image_Malloc() and later on call
                    // J_Image_FromRawToImage()
                    // error = Jai_FactoryWrapper.J_Image_Malloc(ref ImageInfo, ref myImageProcessingBuffer);
                    // To allocate for DIB format we use J_Image_MallocDIB() and later on call
                    // J_Image_FromRawToDIB()
                    error = Jai_FactoryWrapper.J_Image_MallocDIB(ref ImageInfo, ref myConversionBuffer);

                    // Allocate the Lens Distortion Correction buffer
                    error = Jai_FactoryWrapper.J_Image_Malloc(ref myConversionBuffer, ref myImageProcessingBuffer, Jai_FactoryWrapper.EColorConversionPixelFormat.PixelFormatNone);
                }
                else
                {
                    // Allocate the Lens Distortion Correction buffer
                    error = Jai_FactoryWrapper.J_Image_Malloc(ref ImageInfo, ref myImageProcessingBuffer, Jai_FactoryWrapper.EColorConversionPixelFormat.PixelFormatNone);
                }

                // Initialize the internal undistortion map to improve processing speed
                lock (myUndistortParametersLock)
                {
                    error = Jai_FactoryWrapper.J_Image_ProcessingInit(ref ImageInfo, ref myUndistortParameters);
                }
            }

            if ((ImageInfo.AwaitDelivery < 2) && (WindowHandle != IntPtr.Zero))
            {
                if (mybUndistort)
                {
                    if (!mybMonochrome)
                    {
                        // First we need to convert the image into either 32-bit DIB or 48-bit "Image" format
                        error = Jai_FactoryWrapper.J_Image_FromRawToDIB(ref ImageInfo, ref myConversionBuffer, Jai_FactoryWrapper.EColorInterpolationAlgorithm.BayerStandardMultiprocessor, myRedGain, myBlueGain, myGreenGain);
                        //error = Jai_FactoryWrapper.J_Image_FromRawToImage(ref ImageInfo, ref myConversionBuffer, Jai_FactoryWrapper.EColorInterpolationAlgorithm.BayerStandardMultiprocessor, myRedGain, myBlueGain, myGreenGain);

                        // Do the Lens Distortion Correction
                        lock (myUndistortParametersLock)
                        {
                            error = Jai_FactoryWrapper.J_Image_Processing(ref myConversionBuffer, ref myImageProcessingBuffer, myUndistortParameters, true);
                        }
                    }
                    else
                    {
                        // Do the Lens Distortion Correction
                        lock (myUndistortParametersLock)
                        {
                            error = Jai_FactoryWrapper.J_Image_Processing(ref ImageInfo, ref myImageProcessingBuffer, myUndistortParameters, true);
                        }
                    }

                    // Shows undistorted image
                    error = Jai_FactoryWrapper.J_Image_ShowImage(WindowHandle, ref myImageProcessingBuffer, myRedGain, myGreenGain, myBlueGain);
                }
                else
                    error = Jai_FactoryWrapper.J_Image_ShowImage(WindowHandle, ref ImageInfo, myRedGain, myGreenGain, myBlueGain);

            }
            return;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (myCamera != null)
            {
                StopButton_Click(null, null);
                myCamera.Close();
                return;
            }
        }

        private void opticalCenterXNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (mybInitialized)
                applyButton.Enabled = true;
        }

        private void opticalCenterYNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (mybInitialized)
                applyButton.Enabled = true;
        }

        private void focalLengthXNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (mybInitialized)
                applyButton.Enabled = true;
        }

        private void focalLengthYNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (mybInitialized)
                applyButton.Enabled = true;
        }

        private void k1NumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (mybInitialized)
                applyButton.Enabled = true;
        }

        private void k2NumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (mybInitialized)
                applyButton.Enabled = true;
        }

        private void k3NumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (mybInitialized)
                applyButton.Enabled = true;
        }

        private void p1NumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (mybInitialized)
                applyButton.Enabled = true;
        }

        private void p2NumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (mybInitialized)
                applyButton.Enabled = true;
        }

        private void applyButton_Click(object sender, EventArgs e)
        {
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;
            lock (myUndistortParametersLock)
            {
                myUndistortParameters.OpticalCenterX = Convert.ToDouble(opticalCenterXNumericUpDown.Value);
                myUndistortParameters.OpticalCenterY = Convert.ToDouble(opticalCenterYNumericUpDown.Value);
                myUndistortParameters.FocalLengthX = Convert.ToDouble(focalLengthXNumericUpDown.Value);
                myUndistortParameters.FocalLengthY = Convert.ToDouble(focalLengthYNumericUpDown.Value);
                myUndistortParameters.CoeffK1 = Convert.ToDouble(k1NumericUpDown.Value);
                myUndistortParameters.CoeffK2 = Convert.ToDouble(k2NumericUpDown.Value);
                myUndistortParameters.CoeffK3 = Convert.ToDouble(k3NumericUpDown.Value);
                myUndistortParameters.CoeffP1 = Convert.ToDouble(p1NumericUpDown.Value);
                myUndistortParameters.CoeffP2 = Convert.ToDouble(p2NumericUpDown.Value);

                error = Jai_FactoryWrapper.J_Image_ProcessingInit(ref myImageProcessingBuffer, ref myUndistortParameters);
            }
            applyButton.Enabled = false;
        }

        private void undistortCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            mybUndistort = undistortCheckBox.Checked;
        }

        private void biLinearCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            lock (myUndistortParametersLock)
            {
                if (biLinearCheckBox.Checked)
                    myUndistortParameters.InterpolationType = Jai_FactoryWrapper.EInterpolationType.InterpolationBiLinear;
                else
                    myUndistortParameters.InterpolationType = Jai_FactoryWrapper.EInterpolationType.InterpolationNone;
            }
        }

        private void SetFramegrabberValue(String nodeName, Int64 int64Val)
        {
            if (null == myCamera)
            {
                return;
            }

            IntPtr hDevice = IntPtr.Zero;
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.J_Camera_GetLocalDeviceHandle(myCamera.CameraHandle, ref hDevice);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }

            if (IntPtr.Zero == hDevice)
            {
                return;
            }

            IntPtr hNode;
            error = Jai_FactoryWrapper.J_Camera_GetNodeByName(hDevice, nodeName, out hNode);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }

            if (IntPtr.Zero == hNode)
            {
                return;
            }

            error = Jai_FactoryWrapper.J_Node_SetValueInt64(hNode, false, int64Val);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }

            //Special handling for Active Silicon CXP boards, which also has nodes prefixed
            //with "Incoming":
            if ("Width" == nodeName || "Height" == nodeName)
            {
                string strIncoming = "Incoming" + nodeName;
                IntPtr hNodeIncoming;
                error = Jai_FactoryWrapper.J_Camera_GetNodeByName(hDevice, strIncoming, out hNodeIncoming);
                if (Jai_FactoryWrapper.EFactoryError.Success != error)
                {
                    return;
                }

                if (IntPtr.Zero == hNodeIncoming)
                {
                    return;
                }

                error = Jai_FactoryWrapper.J_Node_SetValueInt64(hNodeIncoming, false, int64Val);
            }
        }

        private void SetFramegrabberPixelFormat()
        {
            String nodeName = "PixelFormat";

            if (null == myCamera)
            {
                return;
            }

            IntPtr hDevice = IntPtr.Zero;
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.J_Camera_GetLocalDeviceHandle(myCamera.CameraHandle, ref hDevice);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }

            if (IntPtr.Zero == hDevice)
            {
                return;
            }

            long pf = 0;
            error = Jai_FactoryWrapper.J_Camera_GetValueInt64(myCamera.CameraHandle, nodeName, ref pf);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }
            UInt64 pixelFormat = (UInt64)pf;

            UInt64 jaiPixelFormat = 0;
            error = Jai_FactoryWrapper.J_Image_Get_PixelFormat(myCamera.CameraHandle, pixelFormat, ref jaiPixelFormat);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }

            StringBuilder sbJaiPixelFormatName = new StringBuilder(512);
            uint iSize = (uint)sbJaiPixelFormatName.Capacity;
            error = Jai_FactoryWrapper.J_Image_Get_PixelFormatName(myCamera.CameraHandle, jaiPixelFormat, sbJaiPixelFormatName, iSize);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }

            IntPtr hNode;
            error = Jai_FactoryWrapper.J_Camera_GetNodeByName(hDevice, nodeName, out hNode);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }

            if (IntPtr.Zero == hNode)
            {
                return;
            }

            error = Jai_FactoryWrapper.J_Node_SetValueString(hNode, false, sbJaiPixelFormatName.ToString());
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }

            //Special handling for Active Silicon CXP boards, which also has nodes prefixed
            //with "Incoming":
            string strIncoming = "Incoming" + nodeName;
            IntPtr hNodeIncoming;
            error = Jai_FactoryWrapper.J_Camera_GetNodeByName(hDevice, strIncoming, out hNodeIncoming);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                return;
            }

            if (IntPtr.Zero == hNodeIncoming)
            {
                return;
            }

            error = Jai_FactoryWrapper.J_Node_SetValueString(hNodeIncoming, false, sbJaiPixelFormatName.ToString());
        }
    }
}