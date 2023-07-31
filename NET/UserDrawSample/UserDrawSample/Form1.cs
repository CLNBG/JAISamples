using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;

namespace UserDrawSample
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

        String imageOverlayString = "Image Overlay Text Example!";
        String viewWindowOverlayString = "View Window Overlay Text Example!";

        public Form1()
        {
            InitializeComponent();

            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Open the factory with the default Registry database
            error = myFactory.Open("");

            // Search for cameras and update all controls
            SearchButton_Click(null, null);
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

                // Assign the View Window delegate so we get called each time the image is painted in the View Window
                myCamera.ViewWindowEventDelegate += new CCamera.ImageViewWindowDelegate(myCamera_ViewWindowEventDelegate);
                
                // Set the callback mask to be the two User Draw types
                Jai_FactoryWrapper.IVWOptionValue optionValue = new Jai_FactoryWrapper.IVWOptionValue();
                optionValue.UInt32Value = (UInt32) (Jai_FactoryWrapper.EIVWCallbackType.J_IVW_CB_USER_DRAW_IMAGE | Jai_FactoryWrapper.EIVWCallbackType.J_IVW_CB_USER_DRAW_VIEW_WINDOW);

                Jai_FactoryWrapper.J_Image_SetViewWindowOption(myCamera.WindowHandle, Jai_FactoryWrapper.EIVWOptionType.CallbackMask, ref optionValue);

                if (myCamera.NumOfDataStreams > 0)
                {
                    StartButton.Enabled = true;
                    StopButton.Enabled = true;
                    stretchCheckBox.Enabled = true;
                }
                else
                {
                    StartButton.Enabled = false;
                    StopButton.Enabled = false;
                    stretchCheckBox.Enabled = Enabled;
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
                    GainTrackBar.TickFrequency = (GainTrackBar.Maximum - GainTrackBar.Minimum) / 10;
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
                StartButton.Enabled = false;
                StopButton.Enabled = false;
                stretchCheckBox.Enabled = false;
                WidthNumericUpDown.Enabled = false;
                HeightNumericUpDown.Enabled = false;
                GainLabel.Enabled = false;
                GainTrackBar.Enabled = false;

                MessageBox.Show("No Cameras Found!");
            }
        }

        void myCamera_ViewWindowEventDelegate(CCamera camera, Jai_FactoryWrapper.EIVWCallbackType Type, ref Jai_FactoryWrapper.IVWCallbackValue CallbackValue)
        {
            switch (Type)
            {
                case Jai_FactoryWrapper.EIVWCallbackType.J_IVW_CB_USER_DRAW_IMAGE:
                    {
                        // Get Graphics object from the Device Context handle passed
                        Graphics g = Graphics.FromHdcInternal(CallbackValue.UserDrawInfoValue.hDeviceContext);

                        // Create font and brush.
                        Font drawFont = new Font("Arial", 16);
                        SolidBrush drawBrush = new SolidBrush(Color.Red);

                        // Create point for upper-left corner of drawing.
                        PointF drawPoint = new PointF(100.0F, 100.0F);

                        g.DrawString(imageOverlayString, drawFont, drawBrush, drawPoint);

                        if (enableUserDrawImageGridCheckBox.Checked)
                        {
                            // Create a Solid red pen
                            Pen  redPen = new Pen(Color.Red, 2.0F);

                            for (int x=100; x<CallbackValue.UserDrawInfoValue.Width; x+=100)
                            {
                                g.DrawLine(redPen, x, 0, x, (int) (CallbackValue.UserDrawInfoValue.Height-1));
                            }

                            for (int y=100; y<CallbackValue.UserDrawInfoValue.Height; y+=100)
                            {
                                g.DrawLine(redPen, 0, y, (int) (CallbackValue.UserDrawInfoValue.Width-1), y);
                            }
                        }
                    }
                    break;

                case Jai_FactoryWrapper.EIVWCallbackType.J_IVW_CB_USER_DRAW_VIEW_WINDOW:
                    {
                        // Get Graphics object from the Device Context handle passed
                        Graphics g = Graphics.FromHdcInternal(CallbackValue.UserDrawInfoValue.hDeviceContext);

                        // Create font and brush.
                        Font drawFont = new Font("Arial", 16);
                        SolidBrush drawBrush = new SolidBrush(Color.Blue);

                        // Create point for upper-left corner of drawing.
                        PointF drawPoint = new PointF(150.0F, 150.0F);

                        g.DrawString(viewWindowOverlayString, drawFont, drawBrush, drawPoint);

                        if (enableUserDrawViewWindowGridCheckBox.Checked)
                        {
                            // Create a Solid red pen
                            Pen bluePen = new Pen(Color.Blue, 1.0F);

                            for (int x=100; x<CallbackValue.UserDrawInfoValue.Width; x+=100)
                            {
                                g.DrawLine(bluePen, x, 0, x, (int)(CallbackValue.UserDrawInfoValue.Height - 1));
                            }

                            for (int y=100; y<CallbackValue.UserDrawInfoValue.Height; y+=100)
                            {
                                g.DrawLine(bluePen, 0, y, (int)(CallbackValue.UserDrawInfoValue.Width - 1), y);
                            }
                        }
                    }
                    break;
            }
        }

        private void StartButton_Click(object sender, EventArgs e)
        {
            if (myCamera != null)
            {
                // Set the "Stretch flag"
                myCamera.StretchLiveVideo = stretchCheckBox.Checked;

                myCamera.EnableMouseZoom = true;

                // Start the image acquisition with the picturebox windows handle. If the handle is IntPtr.Zero then a new window will be created
                myCamera.StartImageAcquisition(true, 5);

                // Enable Double-Buffering 
                Jai_FactoryWrapper.IVWOptionValue optionValue = new Jai_FactoryWrapper.IVWOptionValue();

                // Enable the Double-Buffering based on the check-box value
                optionValue.BooleanValue = enableDoubleBufferingCheckBox.Checked;

                Jai_FactoryWrapper.J_Image_SetViewWindowOption(myCamera.WindowHandle, Jai_FactoryWrapper.EIVWOptionType.DoubleBuffering, ref optionValue);

                StartButton.Enabled = false;
                StopButton.Enabled = true;
                stretchCheckBox.Enabled = false;
            }
        }

        private void StopButton_Click(object sender, EventArgs e)
        {
            if (myCamera != null)
            {
                myCamera.StopImageAcquisition();

                StartButton.Enabled = true;
                StopButton.Enabled = false;
                stretchCheckBox.Enabled = true;
            }
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

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (myCamera != null)
            {
                StopButton_Click(null, null);
                myCamera.Close();
            }
        }

        private void enableDoubleBufferingCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            // Set the callback mask to be the two User Draw types
            Jai_FactoryWrapper.IVWOptionValue optionValue = new Jai_FactoryWrapper.IVWOptionValue();

            // Enable the Double-Buffering based on the check-box value
            optionValue.BooleanValue = enableDoubleBufferingCheckBox.Checked;

            Jai_FactoryWrapper.J_Image_SetViewWindowOption(myCamera.WindowHandle, Jai_FactoryWrapper.EIVWOptionType.DoubleBuffering, ref optionValue);
        }

        private void imageOverlayTextBox_TextChanged(object sender, EventArgs e)
        {
            imageOverlayString = imageOverlayTextBox.Text;
        }

        private void viewWindowOverlayTextBox_TextChanged(object sender, EventArgs e)
        {
            viewWindowOverlayString = viewWindowOverlayTextBox.Text;
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