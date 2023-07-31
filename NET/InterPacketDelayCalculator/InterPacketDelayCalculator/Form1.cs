using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;

namespace InterPacketDelayCalculator
{
    public partial class Form1 : Form
    {
        // Main factory object
        CFactory myFactory = new CFactory();

        // Opened camera object
        CCamera myCamera;

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
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Search for any new GigE cameras using Filter Driver
            myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);

            if (myFactory.CameraList.Count > 0)
            {
                // Open the camera
                myCamera = myFactory.CameraList[0];

                CameraIDTextBox.Text = myCamera.CameraID;

                error = myCamera.Open();

                //This sample is appropriate only for GigE cameras.
                uint iSize = 512;
                StringBuilder sbTransportLayerName = new StringBuilder(512);
                error = Jai_FactoryWrapper.J_Camera_GetTransportLayerName(myCamera.ItemHandle, sbTransportLayerName, ref iSize);
                if (Jai_FactoryWrapper.EFactoryError.Success != error)
                {
                    MessageBox.Show(this, "Error getting transport layer name for " + myCamera.CameraID, "Camera Transport Name Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                    return;
                }

                string strName = sbTransportLayerName.ToString().ToUpper();
                if (false == strName.Contains("GEV") && false == strName.Contains("GIGEVISION"))
                {
                    MessageBox.Show(this, "This sample only works with GigE cameras.", "Camera Transport Name Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                    return;
                }

                if (error == Jai_FactoryWrapper.EFactoryError.Success)
                    openWizardButton.Enabled = true;
                else
                {
                    openWizardButton.Enabled = false;
                    if (error == Jai_FactoryWrapper.EFactoryError.AccessDenied)
                    {
                        // Someone already opened the camera
                        MessageBox.Show(this, "The camera is already open by another application!", "Camera open error", MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1);
                    }
                }
            }
            else
            {
                openWizardButton.Enabled = true;
                MessageBox.Show("No Cameras Found!");
            }
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (myCamera != null)
                myCamera.Close();
        }

        private void openWizardButton_Click(object sender, EventArgs e)
        {
            // Create the Inter-Packet Delay form
            InterPacketDelayForm myInterPacketDelayForm = new InterPacketDelayForm(myCamera);

            // .. and show it
            if (myInterPacketDelayForm.ShowDialog(this) == DialogResult.OK)
            {
                Int32 currentInterPacketDelayValue = Convert.ToInt32(myCamera.GetNodeValue("GevSCPD"));
                
                // We can now get the value from the form and use it to set the value inside the camera
                // Befor that we might want to check if it has changed!?
                if (currentInterPacketDelayValue != myInterPacketDelayForm.InterPacketDelay)
                {
                    if (MessageBox.Show(this, "The new Inter-Packet Delay (" + myInterPacketDelayForm.InterPacketDelay.ToString() + ") is different from the current value (" + currentInterPacketDelayValue.ToString() + ").\nDo you want to change to the new value?", "Inter-Packer Delay Changed", MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1) == DialogResult.Yes)
                    {
                        myCamera.GetNode("GevSCPD").Value = myInterPacketDelayForm.InterPacketDelay;
                    }
                }
            }
        }
    }
}