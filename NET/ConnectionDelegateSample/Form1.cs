using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;

namespace ConnectionDelegateSample
{
   public partial class Form1 : Form
   {
      // Main factory object
      CFactory myFactory = new CFactory();

      // Opened camera obejct
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

      /// <summary>
      /// Search for GigE Vision compliant cameras and open the first one found
      /// </summary>
      /// <param name="sender"></param>
      /// <param name="e"></param>
      private void SearchButton_Click(object sender, EventArgs e)
      {
         // Search for any new GigE cameras using Filter Driver
         myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);

         if (myFactory.CameraList.Count > 0)
         {
            // Open the camera
            myCamera = myFactory.CameraList[0];

            CameraIDTextBox.Text = myCamera.CameraID;

            myCamera.Open();

            //This sample is appropriate only for GigE cameras.
            uint iSize = 512;
            StringBuilder sbTransportLayerName = new StringBuilder(512);
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.J_Camera_GetTransportLayerName(myCamera.ItemHandle, sbTransportLayerName, ref iSize);
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

            // Update connection status label with current connection status
            ConnectionStatusLabel.Text = myCamera.ConnectionStatus.ToString();

            // Attach to NewConnectionStatusDelegate in order to get  notified every time the connection status changes
            myCamera.NewConnectionStatusDelegate += new Jai_FactoryWrapper.ConnectionStatusCallBack(myCamera_NewConnectionStatusDelegate);
         }
         else
         {
            MessageBox.Show("No Cameras Found!");
         }
      }

      /// <summary>
      /// Delegate to be called by the camera object every time the camera connection status changes
      /// </summary>
      /// <param name="camera"></param>
      /// <param name="connectionStatus"></param>
      void myCamera_NewConnectionStatusDelegate(CCamera camera, Jai_FactoryWrapper.EDeviceConnectionType connectionStatus)
      {
         // Is this called from the UI thread?
         if (!ConnectionStatusLabel.InvokeRequired)
         {
            // Simply update the label with the new connection status
            ConnectionStatusLabel.Text = connectionStatus.ToString();
         }
         else
         {
            // Since this delegate is not called from the UI thread then we need to Invoke another delegate that will update the label
            object[] myArray = new object[1];
            myArray[0] = connectionStatus;

            BeginInvoke(new UpdateConnectionStatusDelegate(UpdateConnectionStatusLabel), myArray);
         }
      }

      public delegate void UpdateConnectionStatusDelegate(Jai_FactoryWrapper.EDeviceConnectionType newConnectionStatus);

      void UpdateConnectionStatusLabel(Jai_FactoryWrapper.EDeviceConnectionType newConnectionStatus)
      {
         ConnectionStatusLabel.Text = myCamera.ConnectionStatus.ToString();
      }

      private void Form1_FormClosing(object sender, FormClosingEventArgs e)
      {
         if (myFactory != null)
            myFactory.Close();
      }
   }
}