using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;
using rj2_cs.Forms;
using System.Net;
using System.Runtime.InteropServices;

namespace CSharpSampleMulticastMaster
{
   public partial class Form1 : Form
   {
      // Main factory object
      CFactory myFactory = new CFactory();

      // Opened camera object
      CCamera myCamera;

      // Default multicast IP address
      //  Must be from 224.0.1.0 to 239.255.255.255 for muticasting
      UInt32 m_McIpAddress = 0xef000001;

      //--------------------------------------------------------------------------
      // Form1
      //--------------------------------------------------------------------------
      public Form1()
      {
         InitializeComponent();

         // Indicate default IP address
         textIpAddress.Notation = IPAddressTextBox.IPNotation.IPv4Decimal;
         IPAddress ipaddr = new IPAddress((Int32)IPAddress.HostToNetworkOrder((Int32)m_McIpAddress));
         textIpAddress.Text = ipaddr.ToString();

         // Open the factory with the default Registry database
         Jai_FactoryWrapper.EFactoryError error = myFactory.Open("");

         // Search cameras
         buttonSearchCameras_Click(null, null);
      }

      //--------------------------------------------------------------------------
      // Form1_FormClosing
      //--------------------------------------------------------------------------
      private void Form1_FormClosing(object sender, FormClosingEventArgs e)
      {
         // Close the camera
         buttonCloseCamera_Click(null, null);

         // Close the factory
         myFactory.Close();
      }

      //--------------------------------------------------------------------------
      // buttonSearchCameras_Click
      //--------------------------------------------------------------------------
      private void buttonSearchCameras_Click(object sender, EventArgs e)
      {
         Cursor = Cursors.WaitCursor;

         // Search for any new GigE cameras using Filter Driver
         myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);

         if (myFactory.CameraList.Count <= 0)
         {
            label1.Text = "No camera was found!";
            buttonOpenCamera.Enabled = false;
            textIpAddress.Enabled = false;
         }
         else
         {
             //This sample is appropriate only for GigE cameras.
             string strName = myFactory.CameraList[0].CameraID;

             if (false == strName.Contains("INT=>FD") && false == strName.Contains("INT=>SD"))
             {
                 MessageBox.Show(this, "This sample only works with GigE cameras.", "Camera Transport Name Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                 label1.Text = "No camera was found!";
                 return;
             }

             // Indicate the camera ID
             label1.Text = strName;
             buttonOpenCamera.Enabled = true;
             textIpAddress.Enabled = true;
         }

         buttonCloseCamera.Enabled = false;
         buttonStart.Enabled = false;
         buttonStop.Enabled = false;

         Cursor = Cursors.Default;
      }

      //--------------------------------------------------------------------------
      // buttonOpenCamera_Click
      //--------------------------------------------------------------------------
      private void buttonOpenCamera_Click(object sender, EventArgs e)
      {
         Cursor = Cursors.WaitCursor;

         if (myFactory.CameraList.Count > 0)
         {
            // Convert it to the format required in camera
            IPAddress ipaddr = IPAddress.Parse(textIpAddress.GetPureIPAddress());
            byte[] addressbytes = ipaddr.GetAddressBytes();
            m_McIpAddress = (UInt32)IPAddress.HostToNetworkOrder(Marshal.ReadInt32(addressbytes, 0));

            // Get the camera found first
            myCamera = myFactory.CameraList[0];
            // Open the camera
            Jai_FactoryWrapper.EFactoryError error = myCamera.Open(Jai_FactoryWrapper.EDeviceAccessFlags.Control, m_McIpAddress);
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

            buttonOpenCamera.Enabled = false;
            buttonCloseCamera.Enabled = true;
            buttonStart.Enabled = true;
            buttonStop.Enabled = false;
            textIpAddress.Enabled = false;
         }

         Cursor = Cursors.Default;
      }

      //--------------------------------------------------------------------------
      // buttonCloseCamera_Click
      //--------------------------------------------------------------------------
      private void buttonCloseCamera_Click(object sender, EventArgs e)
      {
         Cursor = Cursors.WaitCursor;

         if (myCamera != null)
         {
            buttonStop_Click(null, null);

            // Close the camera
            myCamera.Close();
            myCamera = null;
            buttonOpenCamera.Enabled = true;
            buttonCloseCamera.Enabled = false;
            buttonStart.Enabled = false;
            buttonStop.Enabled = false;
            textIpAddress.Enabled = true;
         }
         Cursor = Cursors.Default;
      }

      //--------------------------------------------------------------------------
      // buttonStart_Click
      //--------------------------------------------------------------------------
      private void buttonStart_Click(object sender, EventArgs e)
      {
         if (myCamera != null)
         {
            // Start acquisition
            myCamera.StartImageAcquisition(true, 5, m_McIpAddress);

            buttonStart.Enabled = false;
            buttonStop.Enabled = true;
            buttonCloseCamera.Enabled = false;
         }
      }

      //--------------------------------------------------------------------------
      // buttonStop_Click
      //--------------------------------------------------------------------------
      private void buttonStop_Click(object sender, EventArgs e)
      {
         if (myCamera != null && myCamera.IsAcquisitionRunning)
         {
            // Stop acquisition
            myCamera.StopImageAcquisition();

            buttonStart.Enabled = true;
            buttonStop.Enabled = false;
            buttonCloseCamera.Enabled = true;
         }
      }
   }
}