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

namespace ActionCommandSample
{
    public partial class Form1 : Form
    {
        // Main factory object
        CFactory myFactory = new CFactory();

        // Action Command Dialog
        ActionCommandForm myActionCommandForm;

        // Action command constants
        const UInt32 DEVICE_KEY = 0x00000001;
        const UInt32 GROUP_KEY_ACTION_1 = 0x00000001;
        const UInt32 GROUP_KEY_ACTION_2 = 0x00000002;
        const UInt32 GROUP_MASK_ACTION_1 = 0x00000001;
        const UInt32 GROUP_MASK_ACTION_2 = 0x00000002;

        const UInt32 GROUP_MASK_ALL_ACTIONS = 0xFFFFFFFF;

        // Timestamp from last image
        UInt64 myTimestampTickFrequency = 0;
        double myTimestampMs1 = 0.0;
        double myTimestampMs2 = 0.0;

        public Form1()
        {
            InitializeComponent();

            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Open the factory with the default Registry database
            error = myFactory.Open("");

            // Search for cameras and update all controls
            SearchButton_Click(null, null);
        }

        private void StartButton_Click(object sender, EventArgs e)
        {
            if ((myFactory.CameraList.Count > 0) && (myFactory.CameraList[0] != null))
            {
                myFactory.CameraList[0].NewImageDelegate += new Jai_FactoryWrapper.ImageCallBack(HandleImage1);
                myFactory.CameraList[0].StartImageAcquisition(true, 5);

                StopButton.Enabled = true;
                StartButton.Enabled = false;
                action1Button.Enabled = true;
            }
            if ((myFactory.CameraList.Count > 1) && (myFactory.CameraList[1] != null))
            {
                myFactory.CameraList[1].NewImageDelegate += new Jai_FactoryWrapper.ImageCallBack(HandleImage2);
                myFactory.CameraList[1].StartImageAcquisition(true, 5);

                StopButton.Enabled = true;
                StartButton.Enabled = false;
                action1Button.Enabled = true;
            }
        }

        private void StopButton_Click(object sender, EventArgs e)
        {
            if ((myFactory.CameraList.Count > 0) && (myFactory.CameraList[0] != null))
            {
                myFactory.CameraList[0].StopImageAcquisition();
                myFactory.CameraList[0].NewImageDelegate -= new Jai_FactoryWrapper.ImageCallBack(HandleImage1);

                StopButton.Enabled = false;
                StartButton.Enabled = true;
                action1Button.Enabled = false;
            }
            if ((myFactory.CameraList.Count > 1) && (myFactory.CameraList[1] != null))
            {
                myFactory.CameraList[1].StopImageAcquisition();
                myFactory.CameraList[1].NewImageDelegate -= new Jai_FactoryWrapper.ImageCallBack(HandleImage2);

                StopButton.Enabled = false;
                StartButton.Enabled = true;
                action1Button.Enabled = false;
            }
        }

        // Local callback function used for handle new images
        void HandleImage1(ref Jai_FactoryWrapper.ImageInfo ImageInfo)
        {
            // Convert timestamp from 'ticks' to milliseconds
            myTimestampMs1 = (double)ImageInfo.TimeStamp / ((double)(myTimestampTickFrequency / 1000));
            return;
        }

        // Local callback function used for handle new images
        void HandleImage2(ref Jai_FactoryWrapper.ImageInfo ImageInfo)
        {
            // Convert timestamp from 'ticks' to milliseconds
            myTimestampMs2 = (double)ImageInfo.TimeStamp / ((double)(myTimestampTickFrequency / 1000));
            return;
        }

        private void SearchButton_Click(object sender, EventArgs e)
        {
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Search for any new GigE cameras using Filter Driver
            myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);

            // Did we find any cameras?
            if (myFactory.CameraList.Count > 0)
            {
                CameraIDTextBox.Text = "";

                // Open the cameras
                foreach (CCamera cam in myFactory.CameraList)
                {
                    CameraIDTextBox.Text += cam.CameraID + Environment.NewLine;

                    error = cam.Open();

                    // Is the camera already opened by another application?
                    if (error == Jai_FactoryWrapper.EFactoryError.AccessDenied)
                    {
                        MessageBox.Show(this, "Access denied for camera '" + cam.CameraID + "'!\nThe camera is most likely opened by another application!?\n\nThe Action Commands can still be sent!", "Camera Open Error");
                    }
                    else if (error == Jai_FactoryWrapper.EFactoryError.Success)
                    {
                        //This sample is appropriate only for GigE cameras.
                        uint iSize = 512;
                        StringBuilder sbTransportLayerName = new StringBuilder(512);
                        error = Jai_FactoryWrapper.J_Camera_GetTransportLayerName(cam.ItemHandle, sbTransportLayerName, ref iSize);
                        if(Jai_FactoryWrapper.EFactoryError.Success != error)
                        {
                            MessageBox.Show(this, "Error getting transport layer name for " + cam.CameraID, "Camera Transport Name Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                            return;
                        }

                        string strName = sbTransportLayerName.ToString().ToUpper();
                        if (false == strName.Contains("GEV") && false == strName.Contains("GIGEVISION"))
                        {
                            MessageBox.Show(this, "This sample only works with GigE cameras.", "Camera Transport Name Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                            return;
                        }

                        StartButton.Enabled = true;
                        StopButton.Enabled = true;

                        // Set up actions inside the camera
                        if (!SetupActions())
                            MessageBox.Show(this, "Unable to set up Actions for camera '" + cam.CameraID + "'!\nThe camera is most likely not a GigE Vision 1.1 device!?\n\nThe Action Commands can still be sent!", "Camera Open Error");

                        // Get the Timestamp Tick Frequency in order to be able to convert the timestamps into milliseconds
                        if (null != cam.GetNode("GevTimestampTickFrequency"))
                        {
                            myTimestampTickFrequency = Convert.ToUInt64(cam.GetNode("GevTimestampTickFrequency").Value);
                        }
                    }

                    // Enable the controls
                    actionCommandGroupBox.Enabled = true;
                    StopButton.Enabled = false;
                    action1Button.Enabled = false;
                    if (cam != null && cam.IsOpen && !cam.IsReadOnly)
                    {
                        StartButton.Enabled = true;
                    }
                    else
                    {
                        openManualActionCommandButton_Click(null, null);
                    }
                }
            }
            else
            {
                StartButton.Enabled = true;
                StopButton.Enabled = true;
                actionCommandGroupBox.Enabled = false;

                MessageBox.Show("No Cameras Found!");
            }
        }

        private bool SetupActions()
        {
            CNode node;

            foreach (CCamera cam in myFactory.CameraList)
            {
                if (cam != null && cam.IsOpen && !cam.IsReadOnly)
                {
                    ///////////////////////////////////////////////////////////////////////////////
                    // Set up Action 1:
                    // ActionDeviceKey=DEVICE_KEY
                    // ActionSelector=1
                    // ActionGroupKey[ActionSelector]=GROUP_KEY_ACTION_1
                    // ActionGroupMask[ActionSelector]=GROUP_MASK_ACTION_1

                    // ActionDeviceKey=DEVICE_KEY
                    node = cam.GetNode("ActionDeviceKey");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get ActionDeviceKey node!");
                        return false;
                    }
                    node.Value = DEVICE_KEY;

                    // ActionSelector=1
                    node = cam.GetNode("ActionSelector");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get ActionSelector node!");
                        return false;
                    }
                    node.Value = 1;

                    // ActionGroupKey[ActionSelector]=GROUP_KEY_ACTION_1
                    node = cam.GetNode("ActionGroupKey");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get ActionGroupKey node!");
                        return false;
                    }
                    node.Value = GROUP_KEY_ACTION_1;

                    // ActionGroupMask[ActionSelector]=GROUP_MASK_ACTION_1
                    node = cam.GetNode("ActionGroupMask");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get ActionGroupMask node!");
                        return false;
                    }
                    node.Value = GROUP_MASK_ACTION_1;

                    ///////////////////////////////////////////////////////////////////////////////
                    // Set up the camera for trigger on Action 1
                    // TriggerSelector=FrameStart
                    // TriggerMode=On
                    // TriggerSource=Action1

                    // TriggerSelector=FrameStart
                    node = cam.GetNode("TriggerSelector");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get TriggerSelector node!");
                        return false;
                    }
                    node.Value = "FrameStart";

                    // TriggerMode=On
                    node = cam.GetNode("TriggerMode");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get TriggerMode node!");
                        return false;
                    }
                    node.Value = "On";

                    // TriggerSource=Action1
                    node = cam.GetNode("TriggerSource");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get TriggerSource node!");
                        return false;
                    }
                    node.Value = "Action1";

                    ///////////////////////////////////////////////////////////////////////////////
                    // Set up Action 2:
                    // ActionDeviceKey=DEVICE_KEY (already set)
                    // ActionSelector=2
                    // ActionGroupKey[ActionSelector]=GROUP_KEY_ACTION_2
                    // ActionGroupMask[ActionSelector]=GROUP_MASK_ACTION_2

                    // ActionSelector=2
                    node = cam.GetNode("ActionSelector");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get ActionSelector node!");
                        return false;
                    }
                    node.Value = 2;

                    // ActionGroupKey[ActionSelector]=GROUP_KEY_ACTION_2
                    node = cam.GetNode("ActionGroupKey");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get ActionGroupKey node!");
                        return false;
                    }
                    node.Value = GROUP_KEY_ACTION_2;

                    // ActionGroupMask[ActionSelector]=GROUP_MASK_ACTION_2
                    node = cam.GetNode("ActionGroupMask");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get ActionGroupMask node!");
                        return false;
                    }
                    node.Value = GROUP_MASK_ACTION_2;

                    ///////////////////////////////////////////////////////////////////////////////
                    // Set up the timestamp reset on Action 2
                    // LineSelector=TimeStampReset
                    // LineSource=Action1
                    // LineInverter=ActiveHigh

                    // LineSelector=TimeStampReset
                    node = cam.GetNode("LineSelector");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get LineSelector node!");
                        return false;
                    }
                    node.Value = "TimeStampReset";

                    // LineSource=Action2
                    node = cam.GetNode("LineSource");
                    if (node == null)
                    {
                        MessageBox.Show("Unable to get LineSource node!");
                        return false;
                    }
                    node.Value = "Action2";
                }
            }
            return true;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            StopButton_Click(null, null);

            // Revert the camera back to Continuous mode
            SetupContinuousMode();

            if (myActionCommandForm != null)
                myActionCommandForm.Close();

            foreach (CCamera cam in myFactory.CameraList)
            {
                if (cam != null)
                {
                    cam.Close();
                }
            }
        }

        private void SetupContinuousMode()
        {
            foreach (CCamera cam in myFactory.CameraList)
            {
                if (cam != null && cam.IsOpen && !cam.IsReadOnly)
                {
                    // ExposureMode=EdgePreSelect
                    cam.GetNode("ExposureMode").Value = "Continuous";
                    CNode node = cam.GetNode("TriggerMode");
                    if (null != node)
                    {
                        node.Value = "Off";
                    }
                }
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            // Update the timestamp on the dialog
            if (myTimestampMs1 != 0.0)
                timestampTextBox1.Text = myTimestampMs1.ToString("0.000") + "ms";
            else
                timestampTextBox1.Text = "";

            if (myTimestampMs2 != 0.0)
                timestampTextBox2.Text = myTimestampMs2.ToString("0.000") + "ms";
            else
                timestampTextBox2.Text = "";
        }

        private void setupActionsButton_Click(object sender, EventArgs e)
        {
            SetupActions();
        }

        private void action1Button_Click(object sender, EventArgs e)
        {
            // Send Action 1
            myFactory.SendActionCommand(DEVICE_KEY, GROUP_KEY_ACTION_1, GROUP_MASK_ALL_ACTIONS);
        }

        private void action2Button_Click(object sender, EventArgs e)
        {
            // Send Action 2
            myFactory.SendActionCommand(DEVICE_KEY, GROUP_KEY_ACTION_2, GROUP_MASK_ALL_ACTIONS);
        }

        private void openManualActionCommandButton_Click(object sender, EventArgs e)
        {
            if (myActionCommandForm == null)
            {
                myActionCommandForm = new ActionCommandForm(myFactory);
            }
            else
            {
                // It might already have been closed??
                if (myActionCommandForm.IsDisposed)
                    myActionCommandForm = new ActionCommandForm(myFactory);
            }

            if (myActionCommandForm != null)
                myActionCommandForm.Show(this);
        }
    }
}