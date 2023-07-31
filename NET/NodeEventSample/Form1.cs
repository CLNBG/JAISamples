using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;

namespace NodeEventSample
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
        CNode myPayloadSizeNode;

        Jai_FactoryWrapper.NodeChangeDelegate myPayloadSizeNodeChangeDelegate;

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
                myWidthNode.Value = int.Parse(WidthNumericUpDown.Value.ToString());
        }

        private void HeightNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (myHeightNode != null)
                myHeightNode.Value = int.Parse(HeightNumericUpDown.Value.ToString());
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

                // Get the GainRaw GenICam Node
                myPayloadSizeNode = myCamera.GetNode("PayloadSize");
                if (myPayloadSizeNode != null)
                {
                    currentValue = int.Parse(myPayloadSizeNode.Value.ToString());

                    PayloadSizeLabel.Text = myPayloadSizeNode.Value.ToString() + " bytes";

                    PayloadSizeLabel.Enabled = true;
                    PayloadSizeLabel2.Enabled = true;

                    // Create new delegate
                    myPayloadSizeNodeChangeDelegate = new Jai_FactoryWrapper.NodeChangeDelegate(myPayloadSizeNodeChangeDelegateFunction);

                    // Register the delegate
                    Jai_FactoryWrapper.J_Node_RegisterCallback(myPayloadSizeNode.NodeHandle, myPayloadSizeNodeChangeDelegate);
                }
                else
                {
                    PayloadSizeLabel.Enabled = false;
                }
            }
            else
            {
                WidthNumericUpDown.Enabled = false;
                HeightNumericUpDown.Enabled = true;
                PayloadSizeLabel.Enabled = false;
                PayloadSizeLabel2.Enabled = false;

                MessageBox.Show("No Cameras Found!");
            }
        }

        private bool myPayloadSizeNodeChangeDelegateFunction(IntPtr nodeHandle)
        {
            if (!PayloadSizeLabel.InvokeRequired)
            {
                PayloadSizeLabel.Text = myPayloadSizeNode.Value.ToString() + " bytes";
            }
            else
            {
                BeginInvoke(new UpdatePayloadSizeDelegate(UpdatePayloadSizeLabel));
            }
            return true;
        }

        public delegate void UpdatePayloadSizeDelegate();

        void UpdatePayloadSizeLabel()
        {
            PayloadSizeLabel.Text = myPayloadSizeNode.Value.ToString() + " bytes";
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (myFactory != null)
                myFactory.Close();
        }
    }
}