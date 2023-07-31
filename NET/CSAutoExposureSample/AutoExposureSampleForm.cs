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

namespace CSAutoExposureSample
{
    public partial class AutoExposureSampleForm : Form
    {
        // Main factory object
        CFactory myFactory = new CFactory();

        // Opened camera obejct
        CCamera myCamera;

        // GenICam nodes
        CNode myWidthNode;
        CNode myHeightNode;
        CNode myGainNode;
        CNode myExposureNode;

        bool m_bAutoExposure = true;                        // Auto Exposure ON/OFF
        bool m_bUseAverage = true;                          // True if Average is used, false if Peak is used
        int m_Width = 0;                                    // Width of image
        int m_Height = 0;                                   // Height of image
        Jai_FactoryWrapper.RECT m_MeasureRect;              // Area to measure Average in
        bool m_UpdateUI = false;                            // Flag used for signalling that the GUI needs to be updated
        int m_OldRAverage;                                  // Cached value for the last measured average in the image
        int m_OldGAverage;                                  // Cached value for the last measured average in the image
        int m_OldBAverage;                                  // Cached value for the last measured average in the image

        ALC m_ALCController = new ALC();                                // The Closed-loop controller
        Jai_FactoryWrapper.ImageInfo m_ConversionBuffer;    // Buffer used for converting images before average is calculated

        public AutoExposureSampleForm()
        {
            // Initialize the ALC controller
            m_ALCController.SetALCType(ALC.EALCType.MinGainPriority);
            m_ALCController.SetKGain(25.0);
            m_ALCController.SetKExposure(3.0);
            m_ALCController.SetThreshold(0.02);         // Threshold of +-0.01 relative error

            InitializeComponent();

            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            ALCTypeComboBox.SelectedIndex = 0;

            // Open the factory with the default Registry database
            error = myFactory.Open("");

            // Search for cameras and update all controls
            SearchButton_Click(null, null);

            enableALCcheckBox.Checked = m_bAutoExposure;

            EnableControls();
        }

        private void SearchButton_Click(object sender, EventArgs e)
        {
            // Search for any new GigE cameras using Filter Driver
            myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);

            if (myFactory.CameraList.Count > 0)
            {
                // Open the camera
                myCamera = myFactory.CameraList[0];

                CameraIDTextBox.Text = myCamera.CameraID;

                myCamera.Open();

                if (myCamera.ModelName.Contains("030GE"))
                {
                    m_ALCController.SetKGain(25.0);
                    m_ALCController.SetKExposure(3.0);
                }
                else if (myCamera.ModelName.Contains("040GE"))
                {
                    m_ALCController.SetKGain(25.0);
                    m_ALCController.SetKExposure(3.0);
                }
                else if (myCamera.ModelName.Contains("080GE"))
                {
                    m_ALCController.SetKGain(25.0);
                    m_ALCController.SetKExposure(3.0);
                }
                else if (myCamera.ModelName.Contains("140GE"))
                {
                    m_ALCController.SetKGain(25.0);
                    m_ALCController.SetKExposure(3.0);
                }
                else if (myCamera.ModelName.Contains("200GE"))
                {
                    m_ALCController.SetKGain(35.0);
                    m_ALCController.SetKExposure(4.0);
                }
                else if (myCamera.ModelName.Contains("500GE"))
                {
                    m_ALCController.SetKGain(15.0);
                    m_ALCController.SetKExposure(3.0);
                }

                // Get the Width GenICam Node
                myWidthNode = myCamera.GetNode("Width");
                if (myWidthNode != null)
                {
                    m_Width = int.Parse(myWidthNode.Value.ToString());

                    widthTrackBar.Minimum = Convert.ToInt32(myWidthNode.Min);
                    widthTrackBar.Maximum = Convert.ToInt32(myWidthNode.Max);
                    widthTrackBar.Value = Convert.ToInt32(myWidthNode.Value);
                    widthTrackBar.TickFrequency = (widthTrackBar.Maximum - widthTrackBar.Minimum) / 20;
                    widthTextBox.Text = myWidthNode.Value.ToString();
                }
                else
                    m_Width = 0;

                SetFramegrabberValue("Width", (Int64)myWidthNode.Value);

                // Get the Height GenICam Node
                myHeightNode = myCamera.GetNode("Height");
                if (myHeightNode != null)
                {
                    m_Height = int.Parse(myHeightNode.Value.ToString());

                    heightTrackBar.Minimum = Convert.ToInt32(myHeightNode.Min);
                    heightTrackBar.Maximum = Convert.ToInt32(myHeightNode.Max);
                    heightTrackBar.Value = Convert.ToInt32(myHeightNode.Value);
                    heightTrackBar.TickFrequency = (heightTrackBar.Maximum - heightTrackBar.Minimum) / 20;
                    heightTextBox.Text = myHeightNode.Value.ToString();
                }
                else
                    m_Height = 0;

                SetFramegrabberValue("Height", (Int64)myHeightNode.Value);

                SetFramegrabberPixelFormat();

                // Get the GainRaw GenICam Node
                myGainNode = myCamera.GetNode("GainRaw");

                if (myGainNode != null)
                {
                    // Update the values inside the ALC controller
                    m_ALCController.SetMinGain(Convert.ToInt32(myGainNode.Min));
                    m_ALCController.SetMaxGain(Convert.ToInt32(myGainNode.Max));
                    m_ALCController.SetCurrentGain(Convert.ToInt32(myGainNode.Value));

                    gainTrackBar.Minimum = Convert.ToInt32(myGainNode.Min);
                    gainTrackBar.Maximum = Convert.ToInt32(myGainNode.Max);
                    gainTrackBar.Value = Convert.ToInt32(myGainNode.Value);
                    gainTrackBar.TickFrequency = (gainTrackBar.Maximum - gainTrackBar.Minimum) / 20;
                    gainTextBox.Text = myGainNode.Value.ToString();
                    minGainNumericUpDown.Minimum = Convert.ToInt32(myGainNode.Min);
                    minGainNumericUpDown.Maximum = Convert.ToInt32(myGainNode.Max);
                    minGainNumericUpDown.Value = Convert.ToInt32(myGainNode.Min);
                    maxGainNumericUpDown.Minimum = Convert.ToInt32(myGainNode.Min);
                    maxGainNumericUpDown.Maximum = Convert.ToInt32(myGainNode.Max);
                    maxGainNumericUpDown.Value = Convert.ToInt32(myGainNode.Max);
                }

                // Get the GainRaw GenICam Node
                myExposureNode = myCamera.GetNode("ExposureTimeRaw");

                if (myExposureNode != null)
                {
                    // Update the values inside the ALC controller
                    m_ALCController.SetMinExposure(Convert.ToInt32(myExposureNode.Min));
                    m_ALCController.SetMaxExposure(Convert.ToInt32(myExposureNode.Max));
                    m_ALCController.SetCurrentExposure(Convert.ToInt32(myExposureNode.Value));

                    exposureTrackBar.Minimum = Convert.ToInt32(myExposureNode.Min);
                    exposureTrackBar.Maximum = Convert.ToInt32(myExposureNode.Max);
                    exposureTrackBar.Value = Convert.ToInt32(myExposureNode.Value);
                    exposureTrackBar.TickFrequency = (exposureTrackBar.Maximum - exposureTrackBar.Minimum) / 20;
                    exposureTextBox.Text = myExposureNode.Value.ToString();
                    minExposureNumericUpDown.Minimum = Convert.ToInt32(myExposureNode.Min);
                    minExposureNumericUpDown.Maximum = Convert.ToInt32(myExposureNode.Max);
                    minExposureNumericUpDown.Value = Convert.ToInt32(myExposureNode.Min);
                    maxExposureNumericUpDown.Minimum = Convert.ToInt32(myExposureNode.Min);
                    maxExposureNumericUpDown.Maximum = Convert.ToInt32(myExposureNode.Max);
                    maxExposureNumericUpDown.Value = Convert.ToInt32(myExposureNode.Max);
                }

                object pixelformatValue = myCamera.GetNodeValue("PixelFormat");

                CNode.IEnumValue enumValue = pixelformatValue as CNode.IEnumValue;

                uint BytesPerPixel = Jai_FactoryWrapper.GetPixelTypeMemorySize((Jai_FactoryWrapper.EPixelFormatType)enumValue.Value);

                if (BytesPerPixel == 1)
                {
                    m_ALCController.SetSetPoint(128);
                    setpointTrackBar.Minimum = 0;
                    setpointTrackBar.Maximum = 255;
                    setpointTrackBar.Value = m_ALCController.GetSetPoint();
                    setpointTextBox.Text = setpointTrackBar.Value.ToString();
                    setpointTrackBar.TickFrequency = 255 / 20;
                }
                else
                {
                    m_ALCController.SetSetPoint(32768);
                    setpointTrackBar.Minimum = 0;
                    setpointTrackBar.Maximum = 65535;
                    setpointTrackBar.Value = m_ALCController.GetSetPoint();
                    setpointTextBox.Text = setpointTrackBar.Value.ToString();
                    setpointTrackBar.TickFrequency = 65535 / 20;
                }

                // Set up average measurement ROI as centre 1/3 of the image area as default
                m_MeasureRect.Left = m_Width / 3;
                m_MeasureRect.Top = m_Width / 3;
                m_MeasureRect.Right = m_Width * 2 / 3;
                m_MeasureRect.Bottom = m_Height * 2 / 3;

                roiwidthNumericUpDown.Minimum = 0;
                roiwidthNumericUpDown.Maximum = m_Width;
                roiwidthNumericUpDown.Value = m_MeasureRect.Right - m_MeasureRect.Left;

                roiheightNumericUpDown.Minimum = 0;
                roiheightNumericUpDown.Maximum = m_Height;
                roiheightNumericUpDown.Value = m_MeasureRect.Bottom - m_MeasureRect.Top;

                xposNumericUpDown.Minimum = 0;
                xposNumericUpDown.Maximum = m_Width;
                xposNumericUpDown.Value = m_MeasureRect.Left;

                yposNumericUpDown.Minimum = 0;
                yposNumericUpDown.Maximum = m_Width;
                yposNumericUpDown.Value = m_MeasureRect.Top;

                StartButton.Enabled = true;
                StopButton.Enabled = true;
            }
            else
            {
                StartButton.Enabled = true;
                StopButton.Enabled = true;

                MessageBox.Show("No Cameras Found!");
            }
        }

        private void EnableControls()
        {
            if (myCamera != null)
            {
                StartButton.Enabled = myCamera.IsOpen ? myCamera.IsAcquisitionRunning ? false : true : false;
                StopButton.Enabled = myCamera.IsOpen ? myCamera.IsAcquisitionRunning ? true : false : false;
                heightTrackBar.Enabled = myCamera.IsOpen ? myCamera.IsAcquisitionRunning ? false : true : false;
                widthTrackBar.Enabled = myCamera.IsOpen ? myCamera.IsAcquisitionRunning ? false : true : false;

                setpointTrackBar.Enabled = m_bAutoExposure ? true : false;

                // The Gain and Exposure sliders depend on the actual ALC mode!
                gainTrackBar.Enabled = myCamera.IsOpen ? m_bAutoExposure ? (m_ALCController.GetALCType() == ALC.EALCType.ExposureOnly) ? true : false : true : false;
                exposureTrackBar.Enabled = myCamera.IsOpen ? m_bAutoExposure ? (m_ALCController.GetALCType() == ALC.EALCType.GainOnly) ? true : false : true : false;
            }
        }

        private void StartButton_Click(object sender, EventArgs e)
        {
            if (myCamera != null)
            {
                myCamera.NewImageDelegate += new Jai_FactoryWrapper.ImageCallBack(HandleImage);
                myCamera.StartImageAcquisition(true, 5);
            }

            timer1.Enabled = true;
            EnableControls();
        }

        private void StopButton_Click(object sender, EventArgs e)
        {
            timer1.Enabled = false;

            if (myCamera != null)
            {
                myCamera.StopImageAcquisition();
                myCamera.NewImageDelegate -= new Jai_FactoryWrapper.ImageCallBack(HandleImage);
            }
            EnableControls();
        }

        // Local callback function used for handle new images
        void HandleImage(ref Jai_FactoryWrapper.ImageInfo ImageInfo)
        {
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Get average value and calculate new control parameters:
            Jai_FactoryWrapper.PixelValue Average = new Jai_FactoryWrapper.PixelValue();

            Average.BGR48_R = 0;
            Average.BGR48_G = 0;
            Average.BGR48_B = 0;

            // We need to allocate conversion buffer once
            if (m_ConversionBuffer.ImageBuffer == IntPtr.Zero)
                error = Jai_FactoryWrapper.J_Image_Malloc(ref ImageInfo, ref m_ConversionBuffer);

            // Then we convert into 48bpp image format
            error = Jai_FactoryWrapper.J_Image_FromRawToImage(ref ImageInfo, ref m_ConversionBuffer, Jai_FactoryWrapper.EColorInterpolationAlgorithm.BayerStandardMultiprocessor, 4096, 4096, 4096);

            // .. and then we can calculate the average value for the measurement ROI
            error = Jai_FactoryWrapper.J_Image_GetAverage(ref m_ConversionBuffer, ref m_MeasureRect, ref Average);

//            error = FindPeak(&m_ConversionBuffer, &m_MeasureRect, &Peak);

            // Update the last measured average
            int average = (int)Average.BGR48_G;

            if ((int)Average.BGR48_R != m_OldRAverage)
            {
                m_OldRAverage = (int)Average.BGR48_R;
                m_UpdateUI = true;
            }
            if ((int)Average.BGR48_G != m_OldGAverage)
            {
                m_OldGAverage = (int)Average.BGR48_G;
                m_UpdateUI = true;
            }
            if ((int)Average.BGR48_B != m_OldBAverage)
            {
                m_OldBAverage = (int)Average.BGR48_B;
                m_UpdateUI = true;
            }

            // Update the last measured peak
            int peak = average;
//            int peak = (int)Peak.G16;

            //if ((int)Peak.R16 != m_OldRPeak)
            //{
            //    m_OldRPeak = (int)Peak.R16;
            //    m_UpdateUI = true;
            //}

            //if ((int)Peak.G16 != m_OldGPeak)
            //{
            //    m_OldGPeak = (int)Peak.G16;
            //    m_UpdateUI = true;
            //}

            //if ((int)Peak.B16 != m_OldBPeak)
            //{
            //    m_OldBPeak = (int)Peak.B16;
            //    m_UpdateUI = true;
            //}

            // Is the ALC active?
            if (m_bAutoExposure)
            {
                // Calculate will return true is any changes has been made to either Gain or Exposure
                bool changes = false;

                if (m_bUseAverage)
                    changes = m_ALCController.Calculate(average);
                else
                    changes = m_ALCController.Calculate(peak);

                if (changes)
                {
                    // Send the new values to the camera
                    UpdateGainAndExposureValues();
                    m_UpdateUI = true;
                }
            }
            return;
        }

        private void UpdateGainAndExposureValues()
        {
            if (myGainNode != null)
                myGainNode.Value = m_ALCController.GetCurrentGain();
            if (myExposureNode != null)
                myExposureNode.Value = m_ALCController.GetCurrentExposure();
        }

        private void AutoExposureSampleForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (myCamera != null)
            {
                myCamera.Close();
                return;
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if (m_UpdateUI)
            {
                // Update UI
                averageRTextBox.Text = m_OldRAverage.ToString();
                averageGTextBox.Text = m_OldGAverage.ToString();
                averageBTextBox.Text = m_OldBAverage.ToString();

                int newGain = m_ALCController.GetCurrentGain();

                gainTrackBar.Value = newGain;
                gainTextBox.Text = newGain.ToString();

                int newExposureTime = m_ALCController.GetCurrentExposure();

                exposureTrackBar.Value = newExposureTime;
                exposureTextBox.Text = newExposureTime.ToString();

                m_UpdateUI = false;
            }
        }

        private void enableALCcheckBox_CheckedChanged(object sender, EventArgs e)
        {
            m_bAutoExposure = enableALCcheckBox.Checked;
            EnableControls();
        }

        private void ALCTypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            m_ALCController.SetALCType((ALC.EALCType) ALCTypeComboBox.SelectedIndex);
            EnableControls();
        }

        private void setpointTrackBar_Scroll(object sender, EventArgs e)
        {
            m_ALCController.SetSetPoint(setpointTrackBar.Value);
            setpointTextBox.Text = setpointTrackBar.Value.ToString();
        }

        private void minGainNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            int minGain = Convert.ToInt32(minGainNumericUpDown.Value);

            // When we adjust the Min gain then we should adjust the current gain accordingly in case the
            // current gain is outside the new boundries
            if (m_ALCController.SetMinGain(minGain))
            {
                int currentGain = m_ALCController.GetCurrentGain();
                myGainNode.Value = currentGain;
                gainTrackBar.Value = currentGain;
                gainTextBox.Text = currentGain.ToString();
            }
        }

        private void maxGainNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            int maxGain = Convert.ToInt32(maxGainNumericUpDown.Value);

            // When we adjust the Min gain then we should adjust the current gain accordingly in case the
            // current gain is outside the new boundries
            if (m_ALCController.SetMaxGain(maxGain))
            {
                int currentGain = m_ALCController.GetCurrentGain();
                myGainNode.Value = currentGain;
                gainTrackBar.Value = currentGain;
                gainTextBox.Text = currentGain.ToString();
            }
        }

        private void minExposureNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            int minExposure = Convert.ToInt32(minExposureNumericUpDown.Value);

            // When we adjust the Min gain then we should adjust the current gain accordingly in case the
            // current gain is outside the new boundries
            if (m_ALCController.SetMinExposure(minExposure))
            {
                int currentExposure = m_ALCController.GetCurrentExposure();
                myExposureNode.Value = currentExposure;
                exposureTrackBar.Value = currentExposure;
                exposureTextBox.Text = currentExposure.ToString();
            }
        }

        private void maxExposureNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            int maxExposure = Convert.ToInt32(maxExposureNumericUpDown.Value);

            // When we adjust the Min gain then we should adjust the current gain accordingly in case the
            // current gain is outside the new boundries
            if (m_ALCController.SetMaxExposure(maxExposure))
            {
                int currentExposure = m_ALCController.GetCurrentExposure();
                myExposureNode.Value = currentExposure;
                exposureTrackBar.Value = currentExposure;
                exposureTextBox.Text = currentExposure.ToString();
            }
        }

        private void xposNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            m_MeasureRect.Left = Convert.ToInt32(xposNumericUpDown.Value);
            m_MeasureRect.Right = m_MeasureRect.Left + Convert.ToInt32(roiwidthNumericUpDown.Value);
        }

        private void yposNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            m_MeasureRect.Top = Convert.ToInt32(yposNumericUpDown.Value);
            m_MeasureRect.Bottom = m_MeasureRect.Top + Convert.ToInt32(roiheightNumericUpDown.Value);
        }

        private void roiwidthNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            m_MeasureRect.Right = m_MeasureRect.Left + Convert.ToInt32(roiwidthNumericUpDown.Value);
        }

        private void roiheightNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            m_MeasureRect.Bottom = m_MeasureRect.Top + Convert.ToInt32(roiheightNumericUpDown.Value);
        }

        private void gainTrackBar_Scroll(object sender, EventArgs e)
        {
            // If the auto exposure is inactive then the user can control the values directly
            if (!m_bAutoExposure || (m_ALCController.GetALCType() == ALC.EALCType.ExposureOnly))
            {
                // Set Value
                myGainNode.Value = gainTrackBar.Value;

                // Update the Text Control with the new value
                gainTextBox.Text = myGainNode.Value.ToString();
                m_ALCController.SetCurrentGain(Convert.ToInt32(myGainNode.Value));
            }
        }

        private void exposureTrackBar_Scroll(object sender, EventArgs e)
        {
            // If the auto exposure is inactive then the user can control the values directly
            if (!m_bAutoExposure || (m_ALCController.GetALCType() == ALC.EALCType.GainOnly))
            {
                // Set Value
                myExposureNode.Value = exposureTrackBar.Value;

                // Update the Text Control with the new value
                exposureTextBox.Text = myExposureNode.Value.ToString();
                m_ALCController.SetCurrentExposure(Convert.ToInt32(myExposureNode.Value));
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