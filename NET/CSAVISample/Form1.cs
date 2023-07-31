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
using System.Drawing.Imaging;
using System.Diagnostics;
using AviFile;

namespace AVISample
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
        String myAVIFileName = "";

        AviManager myAviManager;
        VideoStream aviStream;

        Jai_FactoryWrapper.EPixelFormatType pixelFormatType = Jai_FactoryWrapper.EPixelFormatType.GVSP_PIX_MONO8;
        Jai_FactoryWrapper.ImageInfo myDIBImageInfo = new Jai_FactoryWrapper.ImageInfo();
        ColorPalette myMonoColorPalette = null;

        uint myRGain = 4096;
        uint myGGain = 4096;
        uint myBGain = 4096;

        bool myIsMono = false;

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
                // Get filename for the AVI file
                if (saveFileDialog1.ShowDialog(this) == DialogResult.OK)
                {
                    // Open the AVI file for writing
                    myAviManager = new AviManager(saveFileDialog1.FileName, false, this.Handle);

                    if (myAviManager != null)
                    {
                        int Width = Convert.ToInt32(WidthNumericUpDown.Value);
                        int Height = Convert.ToInt32(HeightNumericUpDown.Value);
                        int ImageSize = Width * Height * 4;

                        PixelFormat pixelFormat = PixelFormat.Format32bppArgb;

                        CNode pfNode = myCamera.GetNode("PixelFormat");
                        if (pfNode != null)
                            pixelFormatType = (Jai_FactoryWrapper.EPixelFormatType)((Jai_FactoryDotNET.CNode.IEnumValue)pfNode.Value).Value;

                        //// Add the video stream to the AVI file
                        if (optimizeForMono8CheckBox.Checked && null != pfNode)
                        {
                            //if (pixelFormatType == Jai_FactoryWrapper.EPixelFormatType.GVSP_PIX_MONO8)
                            CNode.IEnumValue PixelFormatEnumerationValue = pfNode.Value as CNode.IEnumValue;
                            string strType = PixelFormatEnumerationValue.Name.ToLower();
                            if (strType.Contains("mono") && strType.Contains("8"))
                            {
                                pixelFormat = PixelFormat.Format8bppIndexed;
                                ImageSize = Width * Height;
                                myIsMono = true;
                            }
                            else
                            {
                                myIsMono = false;
                            }
                        }

                        // Add the video stream to the AVI file
                        aviStream = myAviManager.AddVideoStream(selectCodecCheckBox.Checked, Convert.ToDouble(frameRateNumericUpDown.Value), ImageSize, Width, Height, pixelFormat); 

                        myAVIFileName = saveFileDialog1.FileName;
                    }
                    else
                    {
                        MessageBox.Show(this, "Unable to create AVI Writer!", "AVI Error");
                    }

                    myCamera.NewImageDelegate += new Jai_FactoryWrapper.ImageCallBack(HandleImage);
                    myCamera.SkipImageDisplayWhenBusy = false;
                    myCamera.StartImageAcquisition(true, 5);
                }
                else
                {
                    myAVIFileName = "";
                }
            }
        }

        private void StopButton_Click(object sender, EventArgs e)
        {
            if (myCamera != null)
            {
                // Stop the acquisition
                myCamera.StopImageAcquisition();
                myCamera.NewImageDelegate -= new Jai_FactoryWrapper.ImageCallBack(HandleImage);

                // Close the AVI writer
                if (myAviManager != null)
                {
                    myAviManager.Close();

                    myAviManager = null;
                }
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

                // Get the gain values to be used for image conversion later
                myCamera.GetGain(ref myRGain, ref myGGain, ref myBGain);

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
            }
            else
            {
                StartButton.Enabled = true;
                StopButton.Enabled = true;
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

            //if ((ImageInfo.PixelFormat == Jai_FactoryWrapper.EPixelFormatType.GVSP_PIX_MONO8) && optimizeForMono8CheckBox.Checked)
            if (myIsMono && optimizeForMono8CheckBox.Checked)
            {
                // Now we have to create a Bitmap object that we later on can add to the AVI file
                Bitmap newImageBitmap = new Bitmap((int)ImageInfo.SizeX, (int)ImageInfo.SizeY, (int)ImageInfo.SizeX, System.Drawing.Imaging.PixelFormat.Format8bppIndexed, ImageInfo.ImageBuffer);

                // Create a Monochrome palette (only once)
                if (myMonoColorPalette == null)
                {
                    Bitmap monoBitmap = new Bitmap(1, 1, PixelFormat.Format8bppIndexed);
                    myMonoColorPalette = monoBitmap.Palette;

                    for (int i = 0; i < 256; i++)
                        myMonoColorPalette.Entries[i] = Color.FromArgb(i, i, i);
                }

                // Set the Monochrome Color Palette
                newImageBitmap.Palette = myMonoColorPalette;

                // If the bitmap got created OK then simply add it to the AVI file
                if (newImageBitmap != null && aviStream != null)
                {
                    aviStream.AddFrame(newImageBitmap);
                }
            }
            else
            {
                // We have to allocate the conversion buffer - but we only want to do it once!
                if (myDIBImageInfo.ImageBuffer == IntPtr.Zero)
                {
                    error = Jai_FactoryWrapper.J_Image_MallocDIB(ref ImageInfo, ref myDIBImageInfo);
                }

                // OK - lets check if we have the DIB ImageInfo allocated
                if (myDIBImageInfo.ImageBuffer != IntPtr.Zero)
                {
                    // Now we convert from RAW to DIB in order to generate AGB color image
                    error = Jai_FactoryWrapper.J_Image_FromRawToDIB(ref ImageInfo, ref myDIBImageInfo, myRGain, myGGain, myBGain);

                    if (error == Jai_FactoryWrapper.EFactoryError.Success)
                    {
                        // Now we have to create a Bitmap object that we later on can add to the AVI file
                        Bitmap newImageBitmap = new Bitmap((int)myDIBImageInfo.SizeX, (int)myDIBImageInfo.SizeY, (int)myDIBImageInfo.SizeX * 4, System.Drawing.Imaging.PixelFormat.Format32bppArgb, myDIBImageInfo.ImageBuffer);

                        // If the bitmap got created OK then simply add it to the AVI file
                        if (newImageBitmap != null && aviStream != null)
                        {
                            aviStream.AddFrame(newImageBitmap);
                        }
                    }
                }
            }
            return;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Make sure to stop acquisition before closing
            StopButton_Click(null, null);

            // Free the BID image info buffer (if it is allocated)
            if (myDIBImageInfo.ImageBuffer != IntPtr.Zero)
            {
                Jai_FactoryWrapper.J_Image_Free(ref myDIBImageInfo);
                myDIBImageInfo.ImageBuffer = IntPtr.Zero;
            }

            if (myCamera != null)
            {
                myCamera.Close();
                return;
            }
        }

        private void playAVIButton_Click(object sender, EventArgs e)
        {
            if (!myAVIFileName.Equals(""))
            {
                Process p;
                ProcessStartInfo pInfo;

                pInfo = new ProcessStartInfo();
                pInfo.Verb = "open";
                pInfo.FileName = myAVIFileName;
                pInfo.UseShellExecute = true;

                p = Process.Start(pInfo);
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