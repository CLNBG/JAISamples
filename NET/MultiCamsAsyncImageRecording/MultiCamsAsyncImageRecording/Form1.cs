using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;
using System.Threading;
using System.IO;

namespace MultiCamsAsyncImageRecording
{
    public partial class Form1 : Form
    {
        // Main factory object
        CFactory myFactory = new CFactory();
        CCamera myCamera;
       
        private int _progressCharIndex = 0;

        public Form1()
        {
            InitializeComponent();
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Open the factory with the default Registry database
            error = myFactory.Open("");

            // Search for cameras, and open
            SearchButton_Click(null, null);

            // Select List recoding mode as default
            recordingModeComboBox.SelectedIndex = 0;
        }

        private void startCaptureButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                if (myFactory.CameraList[i].IsAsyncImageRecordingRunning || (myFactory.CameraList[i].TotalAsyncImagesRecordedCount > 0))
                {
                    DialogResult res = MessageBox.Show(this, "The Asychynchronuous Image Recording is already active or the internal buffer is not empty! Do you want to restart the image recording and discard recorded images?", "Asynchronous Image Capture", MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button2);

                    if (res == DialogResult.Yes)
                    {
                        myFactory.CameraList[i].StopAsyncImageRecording();
                        myFactory.CameraList[i].FreeAsyncRecordedImages();
                        myFactory.CameraList[i].StartAsyncImageRecording(Convert.ToInt32(captureCountNumericUpDown.Value), (CCamera.AsyncImageRecordingMode)recordingModeComboBox.SelectedIndex, Convert.ToInt32(skipCountNumericUpDown.Value));
                    }
                }
                else
                {
                    myFactory.CameraList[i].StartAsyncImageRecording(Convert.ToInt32(captureCountNumericUpDown.Value), (CCamera.AsyncImageRecordingMode)recordingModeComboBox.SelectedIndex, Convert.ToInt32(skipCountNumericUpDown.Value));
                }
            }
        }

        private void stopRecordingButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                myFactory.CameraList[i].StopAsyncImageRecording();

                progressBar1.Style = ProgressBarStyle.Blocks;
                progressBar1.Minimum = 0;
                progressBar1.Maximum = myFactory.CameraList[i].AsyncImageRecordingCount;
                progressBar1.Value = myFactory.CameraList[i].TotalAsyncImagesRecordedCount;
                progressBar1.Enabled = false;
            }
        }

        private void replayButton_Click(object sender, EventArgs e)
        {
            // Here we have access to the stored images! Lets show them in an image window!!
            //Create a replay window
            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                if (!myFactory.CameraList[i].IsAsyncImageRecordingRunning && (myFactory.CameraList[i].TotalAsyncImagesRecordedCount > 0))
                {
                    IntPtr WindowHandle = IntPtr.Zero;

                    // Try to read get the maximum width and height by looking for "SensorWidth" and "SensorHeight"
                    Int32 Width = 0;
                    Int32 Height = 0;
                    CNode WidthNode = myFactory.CameraList[i].GetNode("Width");
                    CNode HeightNode = myFactory.CameraList[i].GetNode("Height");

                    Width = Convert.ToInt32(WidthNode.Max);
                    Height = Convert.ToInt32(HeightNode.Max);

                    IntPtr nodeHandle;

                    uint BytesPerPixel = 4;
                    if (Jai_FactoryWrapper.J_Camera_GetNodeByName(myFactory.CameraList[i].CameraHandle, "PixelFormat", out nodeHandle) == Jai_FactoryWrapper.EFactoryError.Success)
                    {
                        Int64 value = 0;
                        if (Jai_FactoryWrapper.J_Node_GetValueInt64(nodeHandle, false, ref value) == Jai_FactoryWrapper.EFactoryError.Success)
                        {
                            Jai_FactoryWrapper.EPixelFormatType pixeltype = (Jai_FactoryWrapper.EPixelFormatType)value;
                            BytesPerPixel = Jai_FactoryWrapper.GetPixelTypeMemorySize(pixeltype);
                        }
                    }

                    Jai_FactoryWrapper.SIZE maxSize = new Jai_FactoryWrapper.SIZE(Width, Height);

                    Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

                    // Calculate the size of the window rect to display the images
                    int RectWidth = 0;
                    int RectHeight = 0;

                    Jai_FactoryWrapper.RECT frameRect = new Jai_FactoryWrapper.RECT(0, 0, 100, 100); ;

                    // Does the image fit in width?
                    if ((Width + 2 * System.Windows.Forms.SystemInformation.Border3DSize.Width) > System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Width)
                        RectWidth = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Width - 2 * System.Windows.Forms.SystemInformation.Border3DSize.Width;
                    else
                        RectWidth = Width;

                    // Does the image fit in Height?
                    if ((Height + System.Windows.Forms.SystemInformation.Border3DSize.Height + System.Windows.Forms.SystemInformation.CaptionHeight) > System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Height)
                        RectHeight = System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Height - System.Windows.Forms.SystemInformation.Border3DSize.Height - System.Windows.Forms.SystemInformation.CaptionHeight;
                    else
                        RectHeight = Height;

                    frameRect = new Jai_FactoryWrapper.RECT(0, 0, RectWidth, RectHeight);

                    string sDisplayName = myFactory.CameraList[i].ModelName;
                    // Open the replay view
                    error = Jai_FactoryWrapper.J_Image_OpenViewWindowEx(Jai_FactoryWrapper.EIVWWindowType.OverlappedStretch, "Replay " + sDisplayName , ref frameRect, ref maxSize, IntPtr.Zero, out WindowHandle);

                    if (WindowHandle != IntPtr.Zero)
                    {
                        string sMoedlName = myFactory.CameraList[i].ModelName;

                        List<Jai_FactoryWrapper.ImageInfo> imageList = myFactory.CameraList[i].GetAsyncRecordedImages();
                        if (imageList != null && (imageList.Count > 0))
                        {
                            for (int index = 0; index < myFactory.CameraList[i].TotalAsyncImagesRecordedCount; index++)
                            {
                                Jai_FactoryWrapper.ImageInfo ii = imageList[index];
                                Jai_FactoryWrapper.J_Image_SetViewWindowTitle(WindowHandle, "Replay " + sMoedlName + " " + ii.TimeStamp.ToString() + " (" + index.ToString() + "/" + myFactory.CameraList[i].TotalAsyncImagesRecordedCount.ToString() + ")");
                                Jai_FactoryWrapper.J_Image_ShowImage(WindowHandle, ref ii, 4096, 4096, 4096);
                                Application.DoEvents();
                                Thread.Sleep(10);
                            }
                        }

                        Jai_FactoryWrapper.J_Image_CloseViewWindow(WindowHandle);
                    }
                }
            }
        }

        private void saveButton_Click(object sender, EventArgs e)
        {
            Directory.CreateDirectory(".\\Images");

            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                string sModelName = myFactory.CameraList[i].ModelName;
                string sSubDirName = ".\\Images\\Device" + i.ToString();

                Directory.CreateDirectory(sSubDirName);
                // Have we got any images to save to disk?
                if (! myFactory.CameraList[i].IsAsyncImageRecordingRunning && ( myFactory.CameraList[i].TotalAsyncImagesRecordedCount > 0))
                {
                    // Prompt the user if he wants to continue or not with the image save
                    if (MessageBox.Show(this, "Image save might take long time!\nAre you sure you want to continue?", "Image Save", MessageBoxButtons.OKCancel, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1) == DialogResult.OK)
                    {
                        // Disable the Image Recording buttons as long as we are saving the images
                        asynchImageRecordingGroupBox.Enabled = false;

                        // Get the recorded images as a list
                        List<Jai_FactoryWrapper.ImageInfo> imageList =  myFactory.CameraList[i].GetAsyncRecordedImages();

                        // Any images recorded?
                        if (imageList != null && (imageList.Count > 0))
                        {
                            // Run through the list of recorded images
                            for (int index = 0; index <  myFactory.CameraList[i].TotalAsyncImagesRecordedCount; index++)
                            {
                                Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

                                // Get the recorded image at this index
                                Jai_FactoryWrapper.ImageInfo ii = imageList[index];

                                // Are we saving the images in "raw" format or in Tiff?
                                if (saveRawCheckBox.Checked)
                                {
                                    // Save the image to disk
                                    //error = Jai_FactoryWrapper.J_Image_SaveFileRaw(ref ii, ".\\RecordedImage" + index.ToString("000") + ".raw");
                                    error = Jai_FactoryWrapper.J_Image_SaveFileRaw(ref ii, sSubDirName + "\\" + sModelName + "_" + ii.TimeStamp.ToString() + ".raw");
                                }
                                else
                                {
                                    // Create local image that will contain the converted image
                                    Jai_FactoryWrapper.ImageInfo localImageInfo = new Jai_FactoryWrapper.ImageInfo();

                                    // Allocate buffer that will contain the converted image
                                    // In this sample we re-allocate the buffer over-and-over because we assume that the recorded images could be
                                    // of different size (If we have been using the Sequence functionality in the cameras)
                                    error = Jai_FactoryWrapper.J_Image_Malloc(ref ii, ref localImageInfo);

                                    // Convert the raw image to image format
                                    error = Jai_FactoryWrapper.J_Image_FromRawToImage(ref ii, ref localImageInfo, 4096, 4096, 4096);

                                    // Save the image to disk
                                    //error = Jai_FactoryWrapper.J_Image_SaveFile(ref localImageInfo, ".\\RecordedImage" + index.ToString("000") + ".tif");
                                    error = Jai_FactoryWrapper.J_Image_SaveFile(ref ii, sSubDirName + "\\" + sModelName + "_" + ii.TimeStamp.ToString() + ".tif");
 
                                    //Free the conversion buffer
                                    error = Jai_FactoryWrapper.J_Image_Free(ref localImageInfo);
                                }
                                Application.DoEvents();
                            }

                            MessageBox.Show(this, "The recorded images has been saved!", "Image save", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        }
                    }
                }
            }
            // Re-enable the Image Recording buttons
            asynchImageRecordingGroupBox.Enabled = true;
        }

        private void StartButton_Click(object sender, EventArgs e)
        {
            // Set Acquisition count to: stop the acquisition inside the camera after first image 
            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                myFactory.CameraList[i].AcquisitionCount = 1;
            }

            // Acquisition will stop after first image 
            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                myFactory.CameraList[i].StartImageAcquisition(true, 5);
            }

            // Set Acquisition count to 0: do not stop image acquisition
            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                myFactory.CameraList[i].AcquisitionCount = uint.MaxValue;
            }

            // Start Image Acqusition: This is to reduce image transmission delay between cameras
            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                myFactory.CameraList[i].StartImageAcquisition(true, 5);
            }
        }

        private void StopButton_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
                myFactory.CameraList[i].StopImageAcquisition();
            }
        }

        private void SearchButton_Click(object sender, EventArgs e)
        {
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // enable Force IP
            myFactory.EnableAutoForceIP = true;

            // Search for any new GigE cameras using Filter Driver
            myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);

            if (myFactory.CameraList.Count > 0)
            {
                camListBox.Items.Clear();
                for (int i = 0; i < myFactory.CameraList.Count; i++)
                {
                    string sList = myFactory.CameraList[i].ModelName;
                    camListBox.Items.Add(sList);

                    error = myFactory.CameraList[i].Open();

                    // Try to read get the maximum width and height by looking for "SensorWidth" and "SensorHeight"
                    Int32 Width = 0;
                    Int32 Height = 0;
                    CNode WidthNode = myFactory.CameraList[i].GetNode("Width");
                    CNode HeightNode = myFactory.CameraList[i].GetNode("Height");

                    Width = Convert.ToInt32(WidthNode.Max);
                    Height = Convert.ToInt32(HeightNode.Max);

                    myCamera = myFactory.CameraList[i];

                    SetFramegrabberValue("Width", (Int64)WidthNode.Value);

                    SetFramegrabberValue("Height", (Int64)HeightNode.Value);

                    SetFramegrabberPixelFormat();

                }

                StartButton.Enabled = true;
                StopButton.Enabled = true;

            }
            else
            {
                MessageBox.Show("No Camera Found!");
            }

        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            StopButton_Click(null, null);

            for (int i = 0; i < myFactory.CameraList.Count; i++)
            {
               myFactory.CameraList[i].Close();
            }
            myFactory.Close();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            int iLastCamera = myFactory.CameraList.Count - 1;
            if (iLastCamera < 0)
                return;

            if (myFactory.CameraList[iLastCamera] != null)
            {
                startCaptureButton.Enabled = !myFactory.CameraList[iLastCamera].IsAsyncImageRecordingRunning;
                stopCaptureButton.Enabled = myFactory.CameraList[iLastCamera].IsAsyncImageRecordingRunning;
                replayButton.Enabled = !myFactory.CameraList[iLastCamera].IsAsyncImageRecordingRunning
                    && (myFactory.CameraList[iLastCamera].TotalAsyncImagesRecordedCount > 0);
                saveButton.Enabled = !myFactory.CameraList[iLastCamera].IsAsyncImageRecordingRunning && (myFactory.CameraList[iLastCamera].TotalAsyncImagesRecordedCount > 0);

                if (!myFactory.CameraList[iLastCamera].IsAsyncImageRecordingRunning)
                {
                    recordingStatusLabel.Text = "Recording Stopped.";
                    progressBar1.Style = ProgressBarStyle.Blocks;
                    progressBar1.Minimum = 0;
                    progressBar1.Maximum = myFactory.CameraList[iLastCamera].AsyncImageRecordingCount;
                    progressBar1.Value = myFactory.CameraList[iLastCamera].TotalAsyncImagesRecordedCount;
                    progressBar1.Enabled = false;
                }
                else
                {
                    if (myFactory.CameraList[iLastCamera].GetAsyncImageRecordingMode == CCamera.AsyncImageRecordingMode.List)
                    {
                        recordingStatusLabel.Text = "Recorded " + myFactory.CameraList[iLastCamera].TotalAsyncImagesRecordedCount.ToString() + " images out of " + myFactory.CameraList[iLastCamera].AsyncImageRecordingCount.ToString();
                        progressBar1.Style = ProgressBarStyle.Blocks;
                        progressBar1.Minimum = 0;
                        progressBar1.Maximum = myFactory.CameraList[iLastCamera].AsyncImageRecordingCount;
                        progressBar1.Value = myFactory.CameraList[iLastCamera].TotalAsyncImagesRecordedCount;
                    }
                    else
                    {
                        if (myFactory.CameraList[iLastCamera].TotalAsyncImagesRecordedCount < myFactory.CameraList[iLastCamera].AsyncImageRecordingCount)
                        {
                            recordingStatusLabel.Text = "Recorded " + myFactory.CameraList[iLastCamera].TotalAsyncImagesRecordedCount.ToString() + " images out of " + myFactory.CameraList[iLastCamera].AsyncImageRecordingCount.ToString();
                            progressBar1.Style = ProgressBarStyle.Blocks;
                            progressBar1.Minimum = 0;
                            progressBar1.Maximum = myFactory.CameraList[iLastCamera].AsyncImageRecordingCount;
                            progressBar1.Value = myFactory.CameraList[iLastCamera].TotalAsyncImagesRecordedCount;
                        }
                        else
                        {
                            recordingStatusLabel.Text = "Recording frames Cyclically " + GetProgressChar();
                            progressBar1.Style = ProgressBarStyle.Marquee;
                        }
                    }
                }

                StartButton.Enabled = !myFactory.CameraList[iLastCamera].IsAcquisitionRunning;
                StopButton.Enabled = myFactory.CameraList[iLastCamera].IsAcquisitionRunning;
                SearchButton.Enabled = !myFactory.CameraList[iLastCamera].IsAcquisitionRunning;
            }
            else
            {
                startCaptureButton.Enabled = false;
                stopCaptureButton.Enabled = false;
                replayButton.Enabled = false;
                saveButton.Enabled = false;
                SearchButton.Enabled = true;
            }
        }

        private string GetProgressChar()
        {
            _progressCharIndex++;

            if (_progressCharIndex > 3)
                _progressCharIndex = 0;

            switch (_progressCharIndex)
            {
                case 0:
                    return "/";
                case 1:
                    return "--";
                case 2:
                    return "\\";
                case 3:
                    return "|";
            }

            return "";
        }

        private void showErrorMsg(Jai_FactoryWrapper.EFactoryError error)
        {
            String sErrorMsg = "Error = " + error.ToString();

            MessageBox.Show(sErrorMsg);
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