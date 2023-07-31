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

namespace RedCompensationSample
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

        // Handle to the Display Window to output the Red-compensated output image
        IntPtr myViewWindow;

        // Conversion buffer to convert the image before image processing
        Jai_FactoryWrapper.ImageInfo myConversionBuffer;

        // Gain values used by the color interpolation during image conversion and display
        uint myRGain;
        uint myGGain;
        uint myBGain;

        // Red-Compensation parameters
        uint myRedThreshold = 250;
        uint myGreenThreshold = 240;
        uint myBlueThreshold = 220;

        uint myRedGain = 100;
        uint myGreenGain = 50;
        uint myBlueGain = 50;

        public Form1()
        {
            InitializeComponent();

            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Open the factory with the default Registry database
            error = myFactory.Open("");

            // Search for cameras and update all controls
            SearchButton_Click(null, null);
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
                CNode nodeAcquisitionMode = myCamera.GetNode("AcquisitionMode");
                if (null != nodeAcquisitionMode)
                {
                    nodeAcquisitionMode.Value = "Continuous";

                    myCamera.AcquisitionCount = UInt32.MaxValue;
                }

                // Get gain values from the camera object
                myCamera.GetGain(ref myRGain, ref myGGain, ref myBGain);

                Int32 Width = 0;
                Int32 Height = 0;
                CNode WidthNode = myCamera.GetNode("Width");
                CNode HeightNode = myCamera.GetNode("Height");

                // Pleora hack! They set Width.Max and Height.Max to 16K in their custom systems
                if ((Convert.ToInt32(WidthNode.Max) >= 16383) && (Convert.ToInt32(WidthNode.Max) >= 16383))
                {
                    Width = Convert.ToInt32(WidthNode.Value);
                    Height = Convert.ToInt32(HeightNode.Value);
                }
                else
                {
                    Width = Convert.ToInt32(WidthNode.Max);
                    Height = Convert.ToInt32(HeightNode.Max);
                }

                Jai_FactoryWrapper.RECT frameRect = new Jai_FactoryWrapper.RECT(100, 100, Width, Height);
                Jai_FactoryWrapper.SIZE maxSize = new Jai_FactoryWrapper.SIZE(Width, Height);

                // Create View Window to display the processed output (instead of the original)
                Jai_FactoryWrapper.J_Image_OpenViewWindowEx(Jai_FactoryWrapper.EIVWWindowType.OverlappedStretch, "Red-Compensation Output", ref frameRect, ref maxSize, IntPtr.Zero, out myViewWindow);

                myCamera.NewImageDelegate += new Jai_FactoryWrapper.ImageCallBack(HandleImage);
                myCamera.StartImageAcquisition(true, 5);
            }
        }

        private void StopButton_Click(object sender, EventArgs e)
        {
            Jai_FactoryWrapper.EFactoryError retsta;

            if (myCamera != null)
            {
                myCamera.StopImageAcquisition();
                myCamera.NewImageDelegate -= new Jai_FactoryWrapper.ImageCallBack(HandleImage);
            }

            if (myViewWindow != IntPtr.Zero)
            {
                Jai_FactoryWrapper.J_Image_CloseViewWindow(myViewWindow);
                myViewWindow = IntPtr.Zero;
            }

            // Free the conversion buffer again if it has been allocated
            if (myConversionBuffer.ImageBuffer != IntPtr.Zero)
            {
                retsta = Jai_FactoryWrapper.J_Image_Free(ref myConversionBuffer);
                myConversionBuffer.ImageBuffer = IntPtr.Zero;
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

                if (myCamera.NumOfDataStreams > 0)
                {
                    StartButton.Enabled = true;
                    StopButton.Enabled = true;
                }
                else
                {
                    StartButton.Enabled = false;
                    StopButton.Enabled = false;
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

                CNode myPixelNode = myCamera.GetNode("PixelFormat");
                if (myPixelNode != null)
                {
                    string strVal = myPixelNode.Value.ToString().ToLower();
                    if (strVal.Contains("mono"))
                    {
                        enableCheckBox.Checked = false;
                        enableCheckBox.Enabled = false;
                    }
                }
                else
                {
                    enableCheckBox.Checked = false;
                    enableCheckBox.Enabled = false;
                }
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
            Jai_FactoryWrapper.EFactoryError retsta;

            // Allocate tha conversion buffer only once
            if (myConversionBuffer.ImageBuffer == IntPtr.Zero)
                retsta = Jai_FactoryWrapper.J_Image_Malloc(ref ImageInfo, ref myConversionBuffer);

            // Convert to internal Image format in order to be able to call the J_Image_Processing() function later
            retsta = Jai_FactoryWrapper.J_Image_FromRawToImage(ref ImageInfo, ref myConversionBuffer, myRGain, myGGain, myBGain);

            // Set up the Red-compensation parameter structure
            Jai_FactoryWrapper.RedCompensationStruct myRedCompensationParameters = new Jai_FactoryWrapper.RedCompensationStruct();
            // Remember to allocate the 10 Red-compensation ROI structures
            myRedCompensationParameters.ColorCompensationROI = new Jai_FactoryWrapper.ColorCompensationROIStruct[10];

            if (fullImageCheckBox.Checked)
            {
                myRedCompensationParameters.NumOfROI = 1;
                myRedCompensationParameters.ColorCompensationROI[0].RectOfROI.Left = 0;
                myRedCompensationParameters.ColorCompensationROI[0].RectOfROI.Right = (int)ImageInfo.SizeX - 1;
                myRedCompensationParameters.ColorCompensationROI[0].RectOfROI.Top = 0;
                myRedCompensationParameters.ColorCompensationROI[0].RectOfROI.Bottom = (int)ImageInfo.SizeY - 1;
                myRedCompensationParameters.ColorCompensationROI[0].RThreshold = myRedThreshold;
                myRedCompensationParameters.ColorCompensationROI[0].GThreshold = myGreenThreshold;
                myRedCompensationParameters.ColorCompensationROI[0].BThreshold = myBlueThreshold;
                myRedCompensationParameters.ColorCompensationROI[0].RGain = 4096 * myRedGain / 100;
                myRedCompensationParameters.ColorCompensationROI[0].GGain = 4096 * myGreenGain / 100;
                myRedCompensationParameters.ColorCompensationROI[0].BGain = 4096 * myBlueGain / 100;
            }
            else
            {
                myRedCompensationParameters.NumOfROI = 3;

                for (int i = 0; i < 3; i++)
                {
                    myRedCompensationParameters.ColorCompensationROI[i].RectOfROI.Left = (int)ImageInfo.SizeX * i * 2 / 5;
                    myRedCompensationParameters.ColorCompensationROI[i].RectOfROI.Right = (int)ImageInfo.SizeX * (i * 2 + 1) / 5 - 1;
                    myRedCompensationParameters.ColorCompensationROI[i].RectOfROI.Top = (int)ImageInfo.SizeY * 2 / 5;
                    myRedCompensationParameters.ColorCompensationROI[i].RectOfROI.Bottom = (int)ImageInfo.SizeY * 3 / 5 - 1;
                    myRedCompensationParameters.ColorCompensationROI[i].RThreshold = myRedThreshold;
                    myRedCompensationParameters.ColorCompensationROI[i].GThreshold = myGreenThreshold;
                    myRedCompensationParameters.ColorCompensationROI[i].BThreshold = myBlueThreshold;
                    myRedCompensationParameters.ColorCompensationROI[i].RGain = 4096 * myRedGain / 100;
                    myRedCompensationParameters.ColorCompensationROI[i].GGain = 4096 * myGreenGain / 100;
                    myRedCompensationParameters.ColorCompensationROI[i].BGain = 4096 * myBlueGain / 100;
                }
            }

            if (enableCheckBox.Checked)
            {
                retsta = Jai_FactoryWrapper.J_Image_Processing(ref myConversionBuffer, myRedCompensationParameters, true);
            }

            if (myViewWindow != IntPtr.Zero)
                Jai_FactoryWrapper.J_Image_ShowImage(myViewWindow, ref myConversionBuffer, myRGain, myGGain, myBGain);

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

        private void redThresholdNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            myRedThreshold = Convert.ToUInt32(redThresholdNumericUpDown.Value);
        }

        private void greenThresholdNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            myGreenThreshold = Convert.ToUInt32(greenThresholdNumericUpDown.Value);
        }

        private void blueThresholdNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            myBlueThreshold = Convert.ToUInt32(blueThresholdNumericUpDown.Value);
        }

        private void redGainNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            myRedGain = Convert.ToUInt32(redGainNumericUpDown.Value);
        }

        private void greenGainNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            myGreenGain = Convert.ToUInt32(greenGainNumericUpDown.Value);
        }

        private void blueGainNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            myBlueGain = Convert.ToUInt32(blueGainNumericUpDown.Value);
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