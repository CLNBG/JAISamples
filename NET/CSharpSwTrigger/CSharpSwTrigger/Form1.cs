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


namespace CSharpSwTrigger
{
    public partial class Form1 : Form
    {
        CFactory myFactory = new CFactory();
        CCamera myCamera = null;
        bool bSoftwareTriggerSupported = false;

        public int iOldSelectedIndex; // Selected camera entry in the ListBox

        public Form1()
        {
            InitializeComponent();

            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;
            
            // Open the Factory
            error = myFactory.Open("");

            statusBarLabel.Text = "Searching for cameras...";

            if (error == Jai_FactoryWrapper.EFactoryError.Success)
            {
                camListComboBox.Items.Clear(); // delete camera list from combo box

                // Search for any new GigE cameras
                myFactory.UpdateCameraList(CFactory.EDriverType.Undefined);

                if (myFactory.CameraList.Count > 0)
                {
                    for (int i = 0; i < myFactory.CameraList.Count; i++)
                    {
                        string sList = myFactory.CameraList[i].ModelName;

                        if (myFactory.CameraList[i].CameraID.Contains("INT=>FD"))
                            sList += " (Filter Driver)";
                        else if (myFactory.CameraList[i].CameraID.Contains("INT=>SD"))
                            sList += " (Socket Driver)";

                        camListComboBox.Items.Add(sList);
                    }
                }
                else
                {
                    MessageBox.Show("No camera found");
                }

                statusBarLabel.Text = "Found " + myFactory.CameraList.Count.ToString() + " camera(s). Select camera from the ListBox to open it.";
            }
            else
            {
                showErrorMsg(error);
                //error handling?
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // Bind the value member of the trackbar to the textbox.
            widthTextBox.DataBindings.Add("Text", widthTrackBar, "Value");
            heightTextBox.DataBindings.Add("Text", heightTrackBar, "Value");
            gainTextBox.DataBindings.Add("Text", gainTrackBar, "Value");
        }

        private void showErrorMsg(Jai_FactoryWrapper.EFactoryError error)
        {
            String sErrorMsg = "Error = " + error.ToString();

            MessageBox.Show(sErrorMsg);
        }

        private void camListComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // do nothing if the selected camera is the same as the previous one
            if (myCamera != null && iOldSelectedIndex != camListComboBox.SelectedIndex)
            {
                // Close any other opened camera so we can open a new one...
                myCamera.Close();
                myCamera = null;
            }

            if (myCamera == null)
            {
                // open the selected camera from the list
                myCamera = myFactory.CameraList[camListComboBox.SelectedIndex];

                cameraIdTextBox.Text = myCamera.CameraID;
                error = myCamera.Open();
                if (error != Jai_FactoryWrapper.EFactoryError.Success)
                {
                    showErrorMsg(error);
                    return;
                }
                else
                {
                    statusBarLabel.Text = camListComboBox.SelectedItem.ToString();

                    // save the selected camera index
                    iOldSelectedIndex = camListComboBox.SelectedIndex;

                    // initialze controls
                    CNode myNode;
                    // width
                    myNode = myCamera.GetNode("Width");
                    widthTrackBar.Maximum = int.Parse(myNode.Max.ToString());
                    widthTrackBar.Minimum = int.Parse(myNode.Min.ToString());
                    widthTrackBar.Value = int.Parse(myNode.Value.ToString());
                    widthTrackBar.TickFrequency = (widthTrackBar.Maximum - widthTrackBar.Minimum)/10;

                    widthTextBox.Text = myNode.Value.ToString();
                    widthTrackBar.Enabled = true;

                    SetFramegrabberValue("Width", (Int64)myNode.Value);

                    // height
                    myNode = myCamera.GetNode("Height");
                    heightTrackBar.Maximum = int.Parse(myNode.Max.ToString());
                    heightTrackBar.Minimum = int.Parse(myNode.Min.ToString());
                    heightTrackBar.Value = int.Parse(myNode.Value.ToString());
                    heightTrackBar.TickFrequency = (heightTrackBar.Maximum - heightTrackBar.Minimum) / 10;

                    heightTextBox.Text = myNode.Value.ToString();
                    heightTrackBar.Enabled = true;

                    SetFramegrabberValue("Height", (Int64)myNode.Value);

                    SetFramegrabberPixelFormat();

                    // gain
                    myNode = myCamera.GetNode("GainRaw");
                    if (null != myNode)
                    {
                        gainTrackBar.Maximum = int.Parse(myNode.Max.ToString());
                        gainTrackBar.Minimum = int.Parse(myNode.Min.ToString());
                        gainTrackBar.Value = int.Parse(myNode.Value.ToString());
                        gainTrackBar.TickFrequency = (gainTrackBar.Maximum - gainTrackBar.Minimum) / 10;

                        gainTextBox.Text = myNode.Value.ToString();
                        gainTrackBar.Enabled = true;
                    }

                    // .. and remember to set the trigger accordingly

                    // But we have 2 ways of setting up triggers: JAI and GenICam SNC
                    // The GenICam SFNC trigger setup is available if a node called
                    // TriggerSelector is available
                    if (myCamera.GetNode("TriggerSelector") != null)
                    {
                        // Here we assume that this is the GenICam SFNC way of setting up the trigger
                        // To switch to Continuous the following is required:
                        // TriggerSelector=FrameStart
                        // TriggerMode[TriggerSelector]=Off
                        myCamera.GetNode("TriggerSelector").Value = "FrameStart";
                        myCamera.GetNode("TriggerMode").Value = "Off";

                        // Does this camera have a "Software Trigger" feature available?
                        myNode = myCamera.GetNode("TriggerSoftware");
                        if (myNode == null)
                        {
                            swTrigRadio.Enabled = false;
                            swTriggerButton.Enabled = false;
                            MessageBox.Show("No GenICam SFNC Software Trigger found!");
                            return;
                        }
                        else
                        {
                            bSoftwareTriggerSupported = true;
                            swTrigRadio.Enabled = true;
                            freeRunRadio.Enabled = true;
                        }
                    }
                    else
                    {
                        // Here we assume that this is the JAI of setting up the trigger
                        // To switch to Continuous the following is required:
                        // ExposureMode=Continuous
                        myCamera.GetNode("ExposureMode").Value = "Continuous";

                        // Does this camera have a "Software Trigger" feature available?
                        myNode = myCamera.GetNode("SoftwareTrigger0");
                        if (myNode == null)
                        {
                            swTrigRadio.Enabled = false;
                            swTriggerButton.Enabled = false;
                            MessageBox.Show("No Software Trigger found!");
                            return;
                        }
                        else
                        {
                            bSoftwareTriggerSupported = true;
                            swTrigRadio.Enabled = true;
                        }
                    }

                    // check free running as default
                    freeRunRadio.Checked = true;
                    freeRunRadio.Enabled = true;

                    startButton.Enabled = true;
                    stopButton.Enabled = false;

                    // Enable the WhiteBalance button if it is a Bayer color camera
                    wBalanceButton.Enabled = myCamera.IsBayerCamera;
                }
            }
        }

        private void freeRunRadio_Click(object sender, EventArgs e)
        {
            // But we have 2 ways of setting up triggers: JAI and GenICam SNC
            // The GenICam SFNC trigger setup is available if a node called
            // TriggerSelector is available
            if (myCamera.GetNode("TriggerSelector") != null)
            {
                // Here we assume that this is the GenICam SFNC way of setting up the trigger
                // To switch to Continuous the following is required:
                // TriggerSelector=FrameStart
                // TriggerMode[TriggerSelector]=Off
                myCamera.GetNode("TriggerSelector").Value = "FrameStart";
                myCamera.GetNode("TriggerMode").Value = "Off";
            }
            else
            {
                // Here we assume that this is the JAI of setting up the trigger
                // To switch to Continuous the following is required:
                // ExposureMode=Continuous
                myCamera.GetNode("ExposureMode").Value = "Continuous";
            }
        }

        private void swTrigRadio_Click(object sender, EventArgs e)
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
                // ExposureMode=EdgePreSelect
                // LineSelector=CameraTrigger0
                // LineSource=SoftwareTrigger0
                // LineInverter=ActiveHigh
                myCamera.GetNode("ExposureMode").Value = "EdgePreSelect";

                // Set Line Selector to "Camera Trigger 0"
                myCamera.GetNode("LineSelector").Value = "CameraTrigger0";

                // Set Line Source to "Software Trigger 0"
                myCamera.GetNode("LineSource").Value = "SoftwareTrigger0";

                // .. and finally set the Line Polarity (LineInverter) to "Active High"
                myCamera.GetNode("LineInverter").Value = "ActiveHigh";
            }
        }

        private void widthTrackBar_Scroll(object sender, EventArgs e)
        {
            myCamera.GetNode("Width").Value = int.Parse(widthTrackBar.Value.ToString());
            SetFramegrabberValue("Width", (Int64)myCamera.GetNode("Width").Value);
        }

        private void heightTrackBar_Scroll(object sender, EventArgs e)
        {
            myCamera.GetNode("Height").Value = int.Parse(heightTrackBar.Value.ToString());
            SetFramegrabberValue("Height", (Int64)myCamera.GetNode("Height").Value);
        }

        private void gainTrackBar_Scroll(object sender, EventArgs e)
        {
            myCamera.GetNode("GainRaw").Value = int.Parse(gainTrackBar.Value.ToString());
        }

        private void startButton_Click(object sender, EventArgs e)
        {
            if (myCamera != null)
            {
                // Start the Image Acquisition with 5 buffers
                myCamera.StartImageAcquisition(true, 5);

                stopButton.Enabled = true;
                if (swTrigRadio.Checked)
                    swTriggerButton.Enabled = true;

                startButton.Enabled = false;
                widthTrackBar.Enabled = false;
                heightTrackBar.Enabled = false;
                freeRunRadio.Enabled = false;
                swTrigRadio.Enabled = false;
                camListComboBox.Enabled = false;
                wBalanceButton.Enabled = myCamera.IsBayerCamera;
            }
        }

        private void stopButton_Click(object sender, EventArgs e)
        {
            if (myCamera != null)
            {
                myCamera.StopImageAcquisition();

                startButton.Enabled = true;
                widthTrackBar.Enabled = true;
                heightTrackBar.Enabled = true;
                freeRunRadio.Enabled = true;
                if (bSoftwareTriggerSupported)
                    swTrigRadio.Enabled = true;

                stopButton.Enabled = false;
                swTriggerButton.Enabled = false;
                wBalanceButton.Enabled = false;
                camListComboBox.Enabled = true;
            }
        }

        private void wBalanceButton_Click(object sender, EventArgs e)
        {
            int iWidth, iHeight;

            iWidth = int.Parse(myCamera.GetNode("Width").Value.ToString());
            iHeight = int.Parse(myCamera.GetNode("Height").Value.ToString());

            myCamera.ExecuteWhiteBalance(0, 0, iWidth, iHeight);
        }

        private void swTriggerButton_Click(object sender, EventArgs e)
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

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            // We have to manually clean up the Factory object.
            if (myFactory != null)
            {
                stopButton_Click(null, null);

                if (null != myCamera)
                {
                    CNode node = myCamera.GetNode("TriggerMode");
                    if (null != node)
                    {
                        node.Value = "Off";
                    }
                }
                myFactory.Close();
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