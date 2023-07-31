using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;
using System.Runtime.InteropServices;
using System.Net;

namespace CSharpSampleMulticastSlave
{
   public partial class Form1 : Form
   {
      // Main factory object
      CFactory myFactory = new CFactory();

      // Opened camera object
      CCamera myCamera;

      //--------------------------------------------------------------------------
      // Form1
      //--------------------------------------------------------------------------
      public Form1()
      {
         InitializeComponent();

         // Open the factory with the default Registry database
         Jai_FactoryWrapper.EFactoryError error = myFactory.Open("");

         // Search for any new GigE cameras using Filter Driver
         myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);

         if (myFactory.CameraList.Count > 0)
         {
            // Get the camera found first 
            myCamera = myFactory.CameraList[0];

            // Open the camera with slave mode
            error = myCamera.Open(Jai_FactoryWrapper.EDeviceAccessFlags.ReadOnly, 0);
            if (Jai_FactoryWrapper.EFactoryError.Success != error)
            {
                MessageBox.Show(this, "Error opening camera " + myCamera.CameraID + "\nNote: This sample only works with GigE cameras.", "Camera Open Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                return;
            }

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

            // Get and indicate the camera ID
            label1.Text = myCamera.CameraID;

            // Read and indicate IP address from the camera
            ReadIPaddress();

            buttonStart.Enabled = true;
            buttonStop.Enabled = false;
         }
         else
         {
            label1.Text = "No camera was found!";
            buttonStart.Enabled = false;
            buttonStop.Enabled = false;
         }
      }

      //--------------------------------------------------------------------------
      // Form1_FormClosing
      //--------------------------------------------------------------------------
      private void Form1_FormClosing(object sender, FormClosingEventArgs e)
      {
        // Close the camera
        buttonStop_Click(null, null);

        // Close the factory
        myFactory.Close();
      }

      //--------------------------------------------------------------------------
      // buttonStart_Click
      //--------------------------------------------------------------------------
      private void buttonStart_Click(object sender, EventArgs e)
      {
         if (myCamera != null)
         {
            // We need to be sure that the values in the camera are re-read because the
            // Master application might have changed the image size!
            // This is done by invalidating the node values
            Jai_FactoryWrapper.J_Camera_InvalidateNodes(myCamera.CameraHandle);

            // Start acquisition
            myCamera.StartImageAcquisition(true, 5);

            buttonStart.Enabled = false;
            buttonStop.Enabled = true;
         }
      }

      //--------------------------------------------------------------------------
      // buttonStop_Click
      //--------------------------------------------------------------------------
      private void buttonStop_Click(object sender, EventArgs e)
      {
         if (myCamera != null)
         {
            // Stop acquisition
            if (myCamera.IsAcquisitionRunning)
                myCamera.StopImageAcquisition();

            buttonStart.Enabled = true;
            buttonStop.Enabled = false;
         }
      }

      //--------------------------------------------------------------------------
      // ReadIPaddress
      //--------------------------------------------------------------------------
      private void ReadIPaddress()
      {
         // Allocate the managed buffer to hold the data:
         byte[] data = new byte[4];
         // Allocate a handle and pin the data in memory so we can get a pointer to data area:
         GCHandle gch = GCHandle.Alloc(data, GCHandleType.Pinned);
         // Get the pointer of the data inside managed array
         IntPtr bufferPtr = gch.AddrOfPinnedObject();

         // Read SCDA0 reg. (First Stream Channel Destination Address)
         uint size = 4;
         Jai_FactoryWrapper.EFactoryError retsta = Jai_FactoryWrapper.J_Camera_ReadData(myCamera.CameraHandle, 0x0D18, bufferPtr, ref size);
         IPAddress ipaddress = new IPAddress(data);

         // Indicate IP address
         textIpAddress.Text = ipaddress.ToString();
      }
   }
}