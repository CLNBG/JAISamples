using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;

namespace HDRSequenceSample
{
    public partial class Form1 : Form
    {
        CFactory myFactory = new CFactory();                // Main CFactory object that gives us access to the cameras
        CCamera myCamera = null;                            // Main CCamera object to be used for this sample. The first camera detected will be opened

        IntPtr myImage1ViewWindowHandle = IntPtr.Zero;      // View Window handle used for displaying the first image in the sequence
        IntPtr myImage2ViewWindowHandle = IntPtr.Zero;      // View Window handle used for displaying the second image in the sequence
        IntPtr myImageHDRViewWindowHandle = IntPtr.Zero;    // View Window handle used for displaying the reulting HDR image

        int myAcquisitionState = 0;                         // State for the sequence acquisition. This help us assure that we always
                                                            // get two images before we use the HDR algorithms

        bool myAnalyzeImageRequired = true;                 // Flag to activate analysis of the next two images
        bool myAnalysisIsOk = true;                         // Result of the analysis
        int myAnalysisRetries = 0;                          // Number of retries used when trying to analyze the two images

        Jai_FactoryWrapper.ImageInfo myImageInfo1 = new Jai_FactoryWrapper.ImageInfo();     // Copy of the buffer information for the first image
        Jai_FactoryWrapper.ImageInfo myImageInfo2 = new Jai_FactoryWrapper.ImageInfo();     // Copy of the buffer information for the second image
        Jai_FactoryWrapper.ImageInfo myHDROutputImage = new Jai_FactoryWrapper.ImageInfo(); // Buffer information allocated for the HDR output image
        
        byte myImage1IsBrighter = 0;                        // This flag tells us if the first image is brighter than the second image
        int myBlackLevelImage1 = 0;                         // Black level for the first image. This value is used for the HDR merging
        int myBlackLevelImage2 = 0;                         // Black level for the second image. This value is used for the HDR merging
        float myDarkGain = 1.0f;                            // Multiplication factor between the dark image and the bright image
        float myDualSlopeGain = 1.0f;                       // Dual-slope gain. This value will typically be below 1.0 (1.0 = Dual-Slope off)
        object myLockObject = new object();                 // Object used for locking access to the flags and values above

        int myPixelDepth = 8;                               // Number of bits per pixel for the selected Pixel Format

        public Form1()
        {
            InitializeComponent();

            // Open the factory and search for cameras
            myFactory.Open();

            // Simulate press of the Search button
            SearchButton_Click(null, null);

            timer1.Enabled = true;
        }

        /// <summary>
        /// This function performs a GigE Vision Device Discovery using CFactory.UpdateCameraList() (searches for cameras on all NIC adapters).
        /// If at least one camera is found then it will be opened using CCamera.Open()
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void SearchButton_Click(object sender, EventArgs e)
        {
            // Check if a camera has already been opened! 
            if (myCamera != null)
            {
                // Close the camera again in order to release any allocated resources
                myCamera.Close();
                myCamera = null;
            }

            // Search for all GigE cameras on all NIC adapters and pick the Filter Driver (if it is available)
            myFactory.UpdateCameraList(CFactory.EDriverType.FilterDriver);

            // Did we finc any camera?
            if (myFactory.CameraList.Count > 0)
            {
                // Pick the first camera from the list of cameras
                myCamera = myFactory.CameraList[0];

                // Write the camera ID to the text box
                CameraIDTextBox.Text = myCamera.CameraID;

                // ... and finally open up the camera connection
                myCamera.Open();

                // Check if the camera supports the Sequence and Software Trigger
                if (CheckCameraFeatures())
                {
                    // Enable the Start and Stop buttons
                    StartButton.Enabled = true;
                    StopButton.Enabled = true;

                    // Update the camera settings GUI with values from the camera
                    UpdateCameraSettings();
                }
                else
                {
                    // Disable the Start and Stop buttons
                    StartButton.Enabled = false;
                    StopButton.Enabled = false;

                    // Write error information in the text box
                    CameraIDTextBox.Text = "The camera '" + myCamera.ModelName + "' does not support Sequences and/or Software Trigger mode required for this Sample!";
                }
            }
            else
            {
                // Disable the Start and Stop buttons
                StartButton.Enabled = false;
                StopButton.Enabled = false;

                // Write error information in the text box
                CameraIDTextBox.Text = "Error! No cameras found.";
            }
        }

        /// <summary>
        /// This function will set up the camera with sequences and software trigger and start the image acquisition
        /// It also creates View Windows that uses the picture boxes as Child Windows
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void StartButton_Click(object sender, EventArgs e)
        {
            // Verify that the camera connection is actually opened
            if ((myCamera != null) && myCamera.IsOpen)
            {
                // Create the View Windows
                // Try to read get the maximum image width and height by reading the Max values for the mandatory "Width" and "Height" features.
                CNode WidthNode = myCamera.GetNode("Width");
                CNode HeightNode = myCamera.GetNode("Height");

                Int32 Width = Convert.ToInt32(WidthNode.Max);
                Int32 Height = Convert.ToInt32(HeightNode.Max);

                Jai_FactoryWrapper.SIZE maxSize = new Jai_FactoryWrapper.SIZE(Width, Height);
                Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

                // Create the View Windows for the 3 tabs
                // Calculate the size of the window rect to display the images
                Jai_FactoryWrapper.RECT frameRect = new Jai_FactoryWrapper.RECT(0, 0, 100, 100); ;

                System.Windows.Forms.Control ctrl = System.Windows.Forms.Control.FromChildHandle(Image1PictureBox.Handle);

                frameRect = new Jai_FactoryWrapper.RECT(0, 0, ctrl.Width, ctrl.Height);

                error = Jai_FactoryWrapper.J_Image_OpenViewWindowEx(Jai_FactoryWrapper.EIVWWindowType.ChildStretch, "Image1", ref frameRect, ref maxSize, Image1PictureBox.Handle, out myImage1ViewWindowHandle);

                ctrl = System.Windows.Forms.Control.FromChildHandle(Image2PictureBox.Handle);

                frameRect = new Jai_FactoryWrapper.RECT(0, 0, ctrl.Width, ctrl.Height);

                error = Jai_FactoryWrapper.J_Image_OpenViewWindowEx(Jai_FactoryWrapper.EIVWWindowType.ChildStretch, "Image2", ref frameRect, ref maxSize, Image2PictureBox.Handle, out myImage2ViewWindowHandle);

                ctrl = System.Windows.Forms.Control.FromChildHandle(HDRImagePictureBox.Handle);

                frameRect = new Jai_FactoryWrapper.RECT(0, 0, ctrl.Width, ctrl.Height);

                error = Jai_FactoryWrapper.J_Image_OpenViewWindowEx(Jai_FactoryWrapper.EIVWWindowType.ChildStretch, "HDRImage", ref frameRect, ref maxSize, HDRImagePictureBox.Handle, out myImageHDRViewWindowHandle);

                // Setup the camera for software trigger
                SetupTriggerMode();

                // Setup the sequences
                SetupSequences();

                // Get the current camera values and update the GUI
                UpdateCameraSettings();

                // Assign the "new image" delagate and startup the image acquisition
                myCamera.NewImageDelegate += new Jai_FactoryWrapper.ImageCallBack(HandleImage);
                myCamera.StartImageAcquisition(false, 5);   // We dont want the CCamera object to create a View Window so the first parameter is "false"

                // Start the Auto triggering using Software Trigger
                TriggerCamera();
            }
        }

        /// <summary>
        /// This function will stop the image acquisition and free up the View Windows
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void StopButton_Click(object sender, EventArgs e)
        {
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Stop the image acquisition
            myCamera.StopImageAcquisition();
            myCamera.NewImageDelegate -= new Jai_FactoryWrapper.ImageCallBack(HandleImage);

            // Close the View Windows again
            if (myImage1ViewWindowHandle != IntPtr.Zero)
            {
                error = Jai_FactoryWrapper.J_Image_CloseViewWindow(myImage1ViewWindowHandle);
                myImage1ViewWindowHandle = IntPtr.Zero;
            }

            if (myImage2ViewWindowHandle != IntPtr.Zero)
            {
                error = Jai_FactoryWrapper.J_Image_CloseViewWindow(myImage2ViewWindowHandle);
                myImage2ViewWindowHandle = IntPtr.Zero;
            }

            if (myImageHDRViewWindowHandle != IntPtr.Zero)
            {
                error = Jai_FactoryWrapper.J_Image_CloseViewWindow(myImageHDRViewWindowHandle);
                myImageHDRViewWindowHandle = IntPtr.Zero;
            }

            // Have we allocated the output image buffer?
            if (myHDROutputImage.ImageBuffer != IntPtr.Zero)
            {
                // Free up the output image
                error = Jai_FactoryWrapper.J_Image_Free(ref myHDROutputImage);
                myHDROutputImage.ImageBuffer = IntPtr.Zero;
            }
        }

        /// <summary>
        /// This is the callback function (delegate) that will be called for each image received from the camera
        /// </summary>
        /// <param name="ImageInfo"></param>
        void HandleImage(ref Jai_FactoryWrapper.ImageInfo ImageInfo)
        {
            // Automatic re-trigger the camera
            TriggerCamera();

            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Lock the access to the shared flags and the acquisition state variable
            lock (myLockObject)
            {
                switch (myAcquisitionState)
                {
                    case 0: // Get the first image from sequence
                        // Show the image
                        if (myImage1ViewWindowHandle != IntPtr.Zero)
                        {
                            error = Jai_FactoryWrapper.J_Image_ShowImage(myImage1ViewWindowHandle, ref ImageInfo, 4096, 4096, 4096);
                        }
                        // Here we simply save the Image Info structure contents instead of copying any of the image data
                        // We can do this if we assume that we will finish processing the image before the internal buffer will be
                        // reused by the acquisition engine! Since we have allocated 5 buffers then this will always be possible because 
                        // the HDR algorithm processing speed is fast enough.
                        myImageInfo1 = ImageInfo;
                        break;

                    case 1: // Get the second image from sequence
                        // Show the image
                        if (myImage2ViewWindowHandle != IntPtr.Zero)
                        {
                            error = Jai_FactoryWrapper.J_Image_ShowImage(myImage2ViewWindowHandle, ref ImageInfo, 4096, 4096, 4096);
                        }
                        // Here we simply save the Image Info structure contents instead of copying any of the image data
                        // We can do this if we assume that we will finish processing the image before the internal buffer will be
                        // reused by the acquisition engine! Since we have allocated 5 buffers then this will always be possible because 
                        // the HDR algorithm processing speed is fast enough.
                        myImageInfo2 = ImageInfo;

                        // Do we need to (re)analyze the images?
                        if (myAnalyzeImageRequired)
                        {
                            error = Jai_FactoryWrapper.J_Image_AnalyzeHDR(ref myImageInfo1, ref myImageInfo2, ref myImage1IsBrighter, ref myBlackLevelImage1, ref myBlackLevelImage2, ref myDarkGain);
                            if (error == Jai_FactoryWrapper.EFactoryError.Success)
                            {
                                // We got a new analysis result
                                myAnalyzeImageRequired = false;

                                // ..  so we can now proceed with the HDR processing
                                myAnalysisIsOk = true;

                                // Update the Info and Settings tab value with the result from the analysis.
                                UpdateInfo();

                                // Reset retry counter to 0
                                myAnalysisRetries = 0;
                            }
                            else
                            {
                                // We cannot continue analyzing if the images are too far off
                                myAnalysisRetries++;

                                // .. so we only try 10 times in a row
                                if (myAnalysisRetries >= 10)
                                {
                                    MessageBox.Show(this, "The automatic HDR analysis failed!", "HDR Analysis Error", MessageBoxButtons.OK, MessageBoxIcon.Error, MessageBoxDefaultButton.Button1);
                                    myAnalyzeImageRequired = false;
                                }
                            }
                        }

                        // Is the analysis result OK?
                        if (myAnalysisIsOk)
                        {
                            // Have we already allocated the output image buffer?
                            // If not then we need to explicitly use the J_Image_MallocHDR() function to do so.
                            if (myHDROutputImage.ImageBuffer == IntPtr.Zero)
                                error = Jai_FactoryWrapper.J_Image_MallocHDR(ref myImageInfo1, ref myImageInfo2, ref myHDROutputImage);

                            // Here we convert between a boolean and a byte in order to use the J_Image_CombineImagesHDR()
                            byte bUseLog = 0;
                            if (LogCheckBox.Checked)
                                bUseLog = 1;

                            // The J_Image_CombineImagesHDR() requires that we know in advance which image is brighter and which one is darker
                            // Do we need to swap the two images because Image2 is the brightest image?
                            if (myImage1IsBrighter != 0)
                                error = Jai_FactoryWrapper.J_Image_FuseHDR(ref myImageInfo1, ref myImageInfo2, ref myHDROutputImage, myBlackLevelImage1, myBlackLevelImage2, myDarkGain, myDualSlopeGain, bUseLog);
                            else
                                error = Jai_FactoryWrapper.J_Image_FuseHDR(ref myImageInfo2, ref myImageInfo1, ref myHDROutputImage, myBlackLevelImage2, myBlackLevelImage1, myDarkGain, myDualSlopeGain, bUseLog);

                            // Show the HDR output image
                            if (myImageHDRViewWindowHandle != IntPtr.Zero)
                            {
                                // Get WhiteBalance Values
                                uint rGain = Convert.ToUInt32(RedGainNumericUpDown.Value * 4096);
                                uint gGain = Convert.ToUInt32(GreenGainNumericUpDown.Value * 4096);
                                uint bGain = Convert.ToUInt32(BlueGainNumericUpDown.Value * 4096);
                                error = Jai_FactoryWrapper.J_Image_ShowImage(myImageHDRViewWindowHandle, ref myHDROutputImage, rGain, gGain, bGain);
                            }
                        }
                        break;
                }

                // Change state automatically
                myAcquisitionState++;
                if (myAcquisitionState > 1)
                    myAcquisitionState = 0;

            }
            return;
        }

        /// <summary>
        /// This function will read the current sequence settings from the camera and update the slider controls and text boxes with the values
        /// </summary>
        private void UpdateCameraSettings()
        {
            if ((myCamera != null) && myCamera.IsOpen)
            {
                CNode node = null;

                // 1st sequence settings
                myCamera.GetNode("SequenceSelector").Value = 0;

                // Set up Expoure Time Raw slider
                node = myCamera.GetNode("SequenceExposureTimeRaw");
                if (node != null)
                {
                    ExposureTime1TextBox.Text = node.Value.ToString();
                    ExposureTime1TrackBar.Minimum = Convert.ToInt32(node.Min);
                    ExposureTime1TrackBar.Maximum = Convert.ToInt32(node.Max);
                    ExposureTime1TrackBar.Value = Convert.ToInt32(node.Value);

                    ExposureTime1TextBox.Enabled = true;
                    ExposureTime1TrackBar.Enabled = true;
                }
                else
                {
                    ExposureTime1TextBox.Text = "-";
                    ExposureTime1TrackBar.Minimum = 0;
                    ExposureTime1TrackBar.Maximum = 1;
                    ExposureTime1TrackBar.Value = 0;

                    ExposureTime1TextBox.Enabled = false;
                    ExposureTime1TrackBar.Enabled = false;
                }

                // Set up Gain Raw slider
                node = myCamera.GetNode("SequenceMasterGain");
                if (node != null)
                {
                    Gain1TextBox.Text = node.Value.ToString();
                    Gain1TrackBar.Minimum = Convert.ToInt32(node.Min);
                    Gain1TrackBar.Maximum = Convert.ToInt32(node.Max);
                    Gain1TrackBar.Value = Convert.ToInt32(node.Value);

                    Gain1TextBox.Enabled = true;
                    Gain1TrackBar.Enabled = true;
                }
                else
                {
                    Gain1TextBox.Text = "-";
                    Gain1TrackBar.Minimum = 0;
                    Gain1TrackBar.Maximum = 1;
                    Gain1TrackBar.Value = 0;

                    Gain1TextBox.Enabled = false;
                    Gain1TrackBar.Enabled = false;
                }

                // 2nd sequence settings
                myCamera.GetNode("SequenceSelector").Value = 1;

                // Set up Expoure Time Raw slider
                node = myCamera.GetNode("SequenceExposureTimeRaw");
                if (node != null)
                {
                    ExposureTime2TextBox.Text = node.Value.ToString();
                    ExposureTime2TrackBar.Minimum = Convert.ToInt32(node.Min);
                    ExposureTime2TrackBar.Maximum = Convert.ToInt32(node.Max);
                    ExposureTime2TrackBar.Value = Convert.ToInt32(node.Value);

                    ExposureTime2TextBox.Enabled = true;
                    ExposureTime2TrackBar.Enabled = true;
                }
                else
                {
                    ExposureTime2TextBox.Text = "-";
                    ExposureTime2TrackBar.Minimum = 0;
                    ExposureTime2TrackBar.Maximum = 1;
                    ExposureTime2TrackBar.Value = 0;

                    ExposureTime2TextBox.Enabled = false;
                    ExposureTime2TrackBar.Enabled = false;
                }

                // Set up Gain Raw slider
                node = myCamera.GetNode("SequenceMasterGain");
                if (node != null)
                {
                    Gain2TextBox.Text = node.Value.ToString();
                    Gain2TrackBar.Minimum = Convert.ToInt32(node.Min);
                    Gain2TrackBar.Maximum = Convert.ToInt32(node.Max);
                    Gain2TrackBar.Value = Convert.ToInt32(node.Value);

                    Gain2TextBox.Enabled = true;
                    Gain2TrackBar.Enabled = true;
                }
                else
                {
                    Gain2TextBox.Text = "-";
                    Gain2TrackBar.Minimum = 0;
                    Gain2TrackBar.Maximum = 1;
                    Gain2TrackBar.Value = 0;

                    Gain2TextBox.Enabled = false;
                    Gain2TrackBar.Enabled = false;
                }

                // Update the Pixel Format Conbo Box
                PixelFormatComboBox.Items.Clear();
                node = myCamera.GetNode("PixelFormat");

                foreach (CNode.IEnumValue enumval in node.EnumValues)
                {
                    PixelFormatComboBox.Items.Add(enumval.DisplayName);
                }

                PixelFormatComboBox.SelectedIndex = PixelFormatComboBox.FindStringExact(node.Value.ToString());
            }
        }

        /// <summary>
        /// This function sets up the trigger mode and sets up the sequence to contain two steps with full
        /// frame size
        /// </summary>
        private void SetupTriggerMode()
        {
            if ((myCamera != null) && myCamera.IsOpen)
            {
                // Prepare for software trigger:

                // But we have 2 ways of setting up triggers: JAI and GenICam SNC
                // The GenICam SFNC trigger setup is available if a node called
                // TriggerSelector is available
                if (myCamera.GetNode("TriggerSelector") != null)
                {
                    // Here we assume that this is the GenICam SFNC way of setting up the trigger
                    // To switch to Continuous the following is required:
                    // TriggerSelector=FrameStart
                    // TriggerMode[TriggerSelector]=On
                    // TriggerSource[TriggerSelector]=Software
                    myCamera.GetNode("TriggerSelector").Value = "FrameStart";
                    myCamera.GetNode("TriggerMode").Value = "On";
                    myCamera.GetNode("TriggerSource").Value = "Software";
                }
                else
                {
                    // Select triggered mode (not continuous mode)

                    // Here we assume that this is the JAI of setting up the trigger
                    // To switch to Continuous the following is required:
                    // ExposureMode=SequentialEPSTrigger
                    // LineSelector=CameraTrigger0
                    // LineSource=SoftwareTrigger0
                    // LineInverter=ActiveHigh
                    myCamera.GetNode("ExposureMode").Value = "SequentialEPSTrigger";

                    // Set Line Selector to "Camera Trigger 0"
                    myCamera.GetNode("LineSelector").Value = "CameraTrigger0";

                    // Set Line Source to "Software Trigger 0"
                    myCamera.GetNode("LineSource").Value = "SoftwareTrigger0";

                    // .. and finally set the Line Polarity (LineInverter) to "Active High"
                    myCamera.GetNode("LineInverter").Value = "ActiveHigh";
                }
            }
        }

        /// <summary>
        /// This function sets up the sequences to contain two steps with full
        /// frame size
        /// </summary>
        private void SetupSequences()
        {
            if ((myCamera != null) && myCamera.IsOpen)
            {
                // Setup the Sequences

                // But we have 2 ways of setting up sequences with triggers: JAI and GenICam SNC
                // The GenICam SFNC trigger setup is available if a node called
                // SequenceMode is available
                if (myCamera.GetNode("SequenceMode") != null)
                {
                    myCamera.GetNode("SequenceMode").Value = "On";
                }

                // Continuous sequence
                myCamera.GetNode("SequenceRepetitions").Value = 0;
                // .. with two steps
                myCamera.GetNode("SequenceEndingPosition").Value = 2;

                // 1st sequence step is full frame size
                myCamera.GetNode("SequenceSelector").Value = 0;
                myCamera.GetNode("SequenceROIOffsetX").Value = 0;
                myCamera.GetNode("SequenceROIOffsetY").Value = 0;
                myCamera.GetNode("SequenceROISizeX").Value = myCamera.GetNode("SequenceROISizeX").Max;
                myCamera.GetNode("SequenceROISizeY").Value = myCamera.GetNode("SequenceROISizeY").Max;

                // 2nd sequence step is full frame size
                myCamera.GetNode("SequenceSelector").Value = 1;
                myCamera.GetNode("SequenceROIOffsetX").Value = 0;
                myCamera.GetNode("SequenceROIOffsetY").Value = 0;
                myCamera.GetNode("SequenceROISizeX").Value = myCamera.GetNode("SequenceROISizeX").Max;
                myCamera.GetNode("SequenceROISizeY").Value = myCamera.GetNode("SequenceROISizeY").Max;

                // Save the sequence settings into flash memory
                myCamera.GetNode("SequenceSaveCommand").ExecuteCommand();
            }
        }

        /// <summary>
        /// Delegate to update the GUI from the acquisition callback function. We need to use this because the
        /// acquisition thread is not allowed to update the GUI directly but have to use the Invoke method
        /// </summary>
        delegate void UpdateInfoDelegate();

        /// <summary>
        /// Local GUI update function that can be used directly from the UI thread
        /// </summary>
        void localUpdateInfo()
        {
            DarkGainNumericUpDown.Value = Convert.ToDecimal(myDarkGain);
            BlackLevel1NumericUpDown.Value = Convert.ToDecimal(myBlackLevelImage1);
            BlackLevel2NumericUpDown.Value = Convert.ToDecimal(myBlackLevelImage2);

            UpdateResolution();
        }

        /// <summary>
        /// This function determines if the Invoke function is needed in order to update the GUI elements
        /// </summary>
        private void UpdateInfo()
        {
            // Is this function called from a different thread than the UI thread?
            if (DarkGainNumericUpDown.InvokeRequired)
            {
                // .. then use Invoke
                Invoke(new UpdateInfoDelegate(localUpdateInfo));
            }
            else
            {
                // .. or else we just update directly
                localUpdateInfo();
            }
        }

        /// <summary>
        /// Send a software trigger to the camera
        /// </summary>
        private void TriggerCamera()
        {
            // We trigger the camera by "pulsing" the SoftwareTrigger0 value between 0 and 1
            if ((myCamera != null) && myCamera.IsOpen)
            {
                // But we have 2 ways of sending a software trigger: JAI and GenICam SNC
                // The GenICam SFNC software trigger is available if a node called
                // TriggerSoftware is available
                if (myCamera.GetNode("TriggerSoftware") != null)
                {
                    // Here we assume that this is the GenICam SFNC way of setting up the trigger
                    // To switch to Continuous the following is required:
                    // TriggerSelector=FrameStart
                    // Execute TriggerSoftware[TriggerSelector] command
                    myCamera.GetNode("TriggerSelector").Value = "FrameStart";
                    myCamera.GetNode("TriggerSoftware").ExecuteCommand();
                }
                else
                {
                    // We need to "pulse" the Software Trigger feature in order to trigger the camera!
                    myCamera.GetNode("SoftwareTrigger0").Value = 0;
                    myCamera.GetNode("SoftwareTrigger0").Value = 1;
                    myCamera.GetNode("SoftwareTrigger0").Value = 0;
                }
            }
        }

        /// <summary>
        /// In this function we try to verify that all the features we need are indeed available in the connected camera.
        /// This sample will not work wit older CV-A10GE as well as TM-series and RM-series GigE Vision cameras.
        /// </summary>
        /// <returns></returns>
        private bool CheckCameraFeatures()
        {
            if ((myCamera != null) && myCamera.IsOpen)
            {
                bool found = false;
                CNode node = null;

                // We have 2 ways of setting up triggers: JAI and GenICam SNC
                // The GenICam SFNC trigger setup is available if a node called
                // TriggerSelector is available
                node = myCamera.GetNode("TriggerSelector");
                if (node != null)
                {
                    found = false;
                    foreach (CNode.IEnumValue enumValue in node.EnumValues)
                    {
                        if (enumValue.Name.Equals("FrameStart"))
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        return false;

                    // Set the TriggerSelector=FrameStart so we can check for the rest of the features needed
                    node.Value = "FrameStart";

                    // Check if the camera support Trigger
                    node = myCamera.GetNode("TriggerMode");
                    if (node == null)
                        return false;

                    // Check if the camera support Software Trigger
                    node = myCamera.GetNode("TriggerSource");
                    if (node == null)
                        return false;

                    found = false;
                    foreach (CNode.IEnumValue enumValue in node.EnumValues)
                    {
                        if (enumValue.Name.Equals("Software"))
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        return false;

                    // Check if the camera support Software Trigger
                    node = myCamera.GetNode("TriggerSoftware");
                    if (node == null)
                        return false;
                }
                else
                {
                    // Check if the camera support Sequential Edge-pre select mode
                    node = myCamera.GetNode("ExposureMode");
                    if (node == null)
                        return false;

                    found = false;
                    foreach (CNode.IEnumValue enumValue in node.EnumValues)
                    {
                        if (enumValue.Name.Equals("SequentialEPSTrigger"))
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        return false;

                    // Check if the camera support Software Trigger
                    node = myCamera.GetNode("LineSelector");
                    if (node == null)
                        return false;

                    found = false;
                    foreach (CNode.IEnumValue enumValue in node.EnumValues)
                    {
                        if (enumValue.Name.Equals("CameraTrigger0"))
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        return false;

                    node = myCamera.GetNode("LineSource");
                    if (node == null)
                        return false;

                    found = false;
                    foreach (CNode.IEnumValue enumValue in node.EnumValues)
                    {
                        if (enumValue.Name.Equals("SoftwareTrigger0"))
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        return false;
                }

                // Check for Sequence
                node = myCamera.GetNode("SequenceRepetitions");
                if (node == null)
                    return false;

                node = myCamera.GetNode("SequenceEndingPosition");
                if (node == null)
                    return false;

                node = myCamera.GetNode("SequenceSelector");
                if (node == null)
                    return false;

                node = myCamera.GetNode("SequenceROIOffsetX");
                if (node == null)
                    return false;

                node = myCamera.GetNode("SequenceROIOffsetY");
                if (node == null)
                    return false;

                node = myCamera.GetNode("SequenceROISizeX");
                if (node == null)
                    return false;

                node = myCamera.GetNode("SequenceROISizeY");
                if (node == null)
                    return false;

                node = myCamera.GetNode("SequenceSaveCommand");
                if (node == null)
                    return false;
            }

            return true;
        }

        /// <summary>
        /// If the used closes the main form then we make sure to stop the acquisition and close the camera connection correctly
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (myCamera != null)
            {
                // Stop the acquisition if it is already started
                StopButton_Click(null, null);

                // Close the camera
                myCamera.Close();
                return;
            }
        }

        /// <summary>
        /// The user changed the Dark Gain value by pressing the up/down buttons. This function will update the values and the GUI
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DarkGainNumericUpDown_ValueChanged_1(object sender, EventArgs e)
        {
            // User manually changed the Dark Gain
            myDarkGain = Convert.ToSingle(DarkGainNumericUpDown.Value);

            UpdateResolution();
        }

        /// <summary>
        /// This function will try to estimate the equivalent "resolution" for the HDR algorithm. The result depends on the DarkGain as well as
        /// the pixel format
        /// </summary>
        private void UpdateResolution()
        {
            int DarkBlackLevel = 0;
            if (myImage1IsBrighter != 0)
                DarkBlackLevel = myBlackLevelImage2;
            else
                DarkBlackLevel = myBlackLevelImage1;

            Double res = 0.0;

            if (myPixelDepth == 8)
            {
                res = Math.Log(myDarkGain * (255f - DarkBlackLevel), 2.0);
            }
            else if (myPixelDepth == 10)
            {
                res = Math.Log(myDarkGain * (1023f - DarkBlackLevel), 2.0);
            }

            // Update the "resolution" label
            ResolutionLabel.Text = "-> ~ " + res.ToString("0.0") + " bit resolution";
        }

        /// <summary>
        /// The user manually changed the Black Level for the first image
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void BlackLevel1NumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            // User manually changed the Black Level
            myBlackLevelImage1 = Convert.ToInt32(BlackLevel1NumericUpDown.Value);
        }

        /// <summary>
        /// The user manually changed the Black Level for the second image
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void BlackLevel2NumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            // User manually changed the Black Level
            myBlackLevelImage2 = Convert.ToInt32(BlackLevel2NumericUpDown.Value);
        }

        /// <summary>
        /// The user manually changed the Dual-Slope value by dragging the slider bar
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void DualSlopeTrackBar_Scroll(object sender, EventArgs e)
        {
            myDualSlopeGain = Convert.ToSingle(DualSlopeTrackBar.Value)/1000.0f;
            DualSlopeTextBox.Text = myDualSlopeGain.ToString("0.000");
        }

        /// <summary>
        /// The user request a re-analysis of the images in order to determine new values for Dark Gain and Black Levels
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ForceAnalysisButton_Click(object sender, EventArgs e)
        {
            lock (myLockObject)
            {
                myAnalysisIsOk = false;
                myAnalyzeImageRequired = true;
                myAnalysisRetries = 0;
                myAcquisitionState = 0;
            }
        }

        /// <summary>
        /// The dialog is resized so we need to inform the View Window about the changes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Image1PictureBox_Resize(object sender, EventArgs e)
        {
            if ((myCamera != null) && myCamera.IsOpen && myCamera.IsAcquisitionRunning && (myImage1ViewWindowHandle != IntPtr.Zero))
            {
                Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

                // Calculate the size of the window rect to display the images
                System.Windows.Forms.Control ctrl = System.Windows.Forms.Control.FromChildHandle(Image1PictureBox.Handle);

                Jai_FactoryWrapper.RECT frameRect = new Jai_FactoryWrapper.RECT(0, 0, ctrl.Width, ctrl.Height);

                // Resize the Child Window
                error = Jai_FactoryWrapper.J_Image_ResizeChildWindow(myImage1ViewWindowHandle, ref frameRect);
            }
        }

        /// <summary>
        /// The dialog is resized so we need to inform the View Window about the changes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Image2PictureBox_Resize(object sender, EventArgs e)
        {
            if ((myCamera != null) && myCamera.IsOpen && myCamera.IsAcquisitionRunning && (myImage2ViewWindowHandle != IntPtr.Zero))
            {
                Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

                // Calculate the size of the window rect to display the images
                System.Windows.Forms.Control ctrl = System.Windows.Forms.Control.FromChildHandle(Image2PictureBox.Handle);

                Jai_FactoryWrapper.RECT frameRect = new Jai_FactoryWrapper.RECT(0, 0, ctrl.Width, ctrl.Height);

                // Resize the Child Window
                error = Jai_FactoryWrapper.J_Image_ResizeChildWindow(myImage2ViewWindowHandle, ref frameRect);
            }
        }

        /// <summary>
        /// The dialog is resized so we need to inform the View Window about the changes
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void HDRImagePictureBox_Resize(object sender, EventArgs e)
        {
            if ((myCamera != null) && myCamera.IsOpen && myCamera.IsAcquisitionRunning && (myImageHDRViewWindowHandle != IntPtr.Zero))
            {
                Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

                // Calculate the size of the window rect to display the images
                System.Windows.Forms.Control ctrl = System.Windows.Forms.Control.FromChildHandle(HDRImagePictureBox.Handle);

                Jai_FactoryWrapper.RECT frameRect = new Jai_FactoryWrapper.RECT(0, 0, ctrl.Width, ctrl.Height);

                // Resize the Child Window
                error = Jai_FactoryWrapper.J_Image_ResizeChildWindow(myImageHDRViewWindowHandle, ref frameRect);
            }
        }

        /// <summary>
        /// The user adjusted the Exposure time for Sequence 1
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ExposureTime1TrackBar_Scroll(object sender, EventArgs e)
        {
            // Sequence 1 settings
            myCamera.GetNode("SequenceSelector").Value = 0;

            // Set up Expoure Time Raw slider
            myCamera.GetNode("SequenceExposureTimeRaw").Value = ExposureTime1TrackBar.Value;

            ExposureTime1TextBox.Text = ExposureTime1TrackBar.Value.ToString();
        }

        /// <summary>
        /// The user adjusted the Master Gain for Sequence 1
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Gain1TrackBar_Scroll(object sender, EventArgs e)
        {
            // Sequence 1 settings
            myCamera.GetNode("SequenceSelector").Value = 0;

            // Set up Expoure Time Raw slider
            myCamera.GetNode("SequenceMasterGain").Value = Gain1TrackBar.Value;

            Gain1TextBox.Text = Gain1TrackBar.Value.ToString();
        }

        /// <summary>
        /// The user let go of the Exposure time slider so we need to force a re-analysis
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ExposureTime1TrackBar_MouseUp(object sender, MouseEventArgs e)
        {
            ForceAnalysisButton_Click(null, null);
        }

        /// <summary>
        /// The user let go of the Gain slider so we need to force a re-analysis
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Gain1TrackBar_MouseUp(object sender, MouseEventArgs e)
        {
            ForceAnalysisButton_Click(null, null);
        }

        /// <summary>
        /// The user adjusted the Exposure time for Sequence 2
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ExposureTime2TrackBar_Scroll(object sender, EventArgs e)
        {
            // Sequence 2 settings
            myCamera.GetNode("SequenceSelector").Value = 1;

            // Set up Expoure Time Raw slider
            myCamera.GetNode("SequenceExposureTimeRaw").Value = ExposureTime2TrackBar.Value;

            ExposureTime2TextBox.Text = ExposureTime2TrackBar.Value.ToString();
        }

        /// <summary>
        /// The user adjusted the Master Gain for Sequence 2
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Gain2TrackBar_Scroll(object sender, EventArgs e)
        {
            // Sequence 2 settings
            myCamera.GetNode("SequenceSelector").Value = 1;

            // Set up Expoure Time Raw slider
            myCamera.GetNode("SequenceMasterGain").Value = Gain2TrackBar.Value;

            Gain1TextBox.Text = Gain2TrackBar.Value.ToString();
        }

        /// <summary>
        /// The user let go of the Exposure time slider so we need to force a re-analysis
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ExposureTime2TrackBar_MouseUp(object sender, MouseEventArgs e)
        {
            ForceAnalysisButton_Click(null, null);
        }

        /// <summary>
        /// The user let go of the Gain slider so we need to force a re-analysis
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Gain2TrackBar_MouseUp(object sender, MouseEventArgs e)
        {
            ForceAnalysisButton_Click(null, null);
        }

        /// <summary>
        /// The user selected another PixelFormat for the camera
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void PixelFormatComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            if ((myCamera != null) && myCamera.IsOpen)
            {
                if (PixelFormatComboBox.SelectedItem != null)
                {
                    CNode node = myCamera.GetNode("PixelFormat");

                    // We need to find the correct enum for the possible ones
                    foreach (CNode.IEnumValue enumval in node.EnumValues)
                    {
                        if (enumval.DisplayName.Equals(PixelFormatComboBox.SelectedItem.ToString()))
                        {
                            node.Value = enumval;
                            break;
                        }
                    }

                    // We need this information in order to determine the "resolution"
                    if (PixelFormatComboBox.SelectedItem.ToString().Contains("8"))
                        myPixelDepth = 8;
                    else if (PixelFormatComboBox.SelectedItem.ToString().Contains("10"))
                        myPixelDepth = 10;

                    UpdateResolution();

                    ForceAnalysisButton_Click(null, null);
                }
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            if ((myCamera != null) && myCamera.IsOpen && myCamera.IsAcquisitionRunning)
            {
                FrameRateTextBox.Text = (myCamera.FramesPerSecond/2.0).ToString("0.0") + " fps";
            }
            else
            {
                FrameRateTextBox.Text = "0.0 fps";
            }
        }
    }
}