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
using System.Globalization;

namespace InterPacketDelayCalculator
{
    public partial class InterPacketDelayForm : Form
    {
        private int _InterPacketDelay = 0;
        private CCamera myCamera;

        int PayloadSize = 0;
        int PacketSize = 0;
        int PacketCount = 0;
        Int64 TickFrequency = 0;
        int OverHead = 0;
        int TotalSize = 0;
        int TransmissionRate = 1000000000 / 8;
        double TransmissionTime = 0.0;
        double MaxFps = 60.0;
        double ExpectedBandwidthUsage = 90.0;
        int CurrentInterPacketDelay = 0;
        int MinInterPacketDelay = 0;
        Int64 MaxInterPacketDelay = Int32.MaxValue;
        double PauseTime = 0;
        double TotalInterPacketDelayTime = 0.0;
        bool Initialized = false;

        public InterPacketDelayForm(CCamera camera)
        {
            InitializeComponent();

            myCamera = camera;
        }

        public int InterPacketDelay
        {
            get
            {
                return _InterPacketDelay;
            }
        }

        private void MaxFpsTextBox_TextChanged(object sender, EventArgs e)
        {
            if (Initialized)
            {
                CalculateButton.Enabled = true;
            }
        }

        private void CalculateButton_Click(object sender, EventArgs e)
        {
            try
            {
                MaxFps = double.Parse(MaxFpsTextBox.Text);
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message);
            }

            try
            {
                  ExpectedBandwidthUsage = double.Parse(MaxErrorUpDown.Value.ToString());
            }
            catch (Exception ex)
            {
                MessageBox.Show(this, ex.Message);
            }

            TransmissionTime = (MaxFps * (double)TotalSize) / (double)TransmissionRate;

            PauseTime = ExpectedBandwidthUsage / 100.0 - TransmissionTime;

            // Is the transmission time larger than the time calculated based on the expected bandwidth usage?
            if (PauseTime < 0.0)
            {
                string message = string.Format("The calculated transmission time is too long!\nEither the number of frames per second ({0} fps) has to be lowered or the expected bandwidth usage ({1} %) has to be increased!", MaxFpsTextBox.Text, MaxErrorUpDown.Value.ToString());
                _InterPacketDelay = MinInterPacketDelay;
                MessageBox.Show(this, message, "Inter Packet Delay Calculation Error");
            }
            else
            {
                TotalInterPacketDelayTime = PauseTime / ((double)PacketCount * MaxFps);
                _InterPacketDelay = (int)Math.Truncate(TotalInterPacketDelayTime * (double)TickFrequency + 0.5);
                InterPacketDelayTextBox.Text = _InterPacketDelay.ToString();

                if (_InterPacketDelay < MinInterPacketDelay)
                {
                    string message = string.Format("The calculated Inter Packet Delay {0} is smaller than allowed minimum value {1}!\nThe value is forced to the minimum value automatically!", _InterPacketDelay, MinInterPacketDelay);
                    _InterPacketDelay = MinInterPacketDelay;
                    MessageBox.Show(this, message, "Inter Packet Delay Calculation Error");
                }

                if (_InterPacketDelay > MaxInterPacketDelay)
                {
                    string message = string.Format("The calculated Inter Packet Delay {0} is greate than allowed maximum value {1}!\nThe value is forced to the maximum value automatically!", _InterPacketDelay, MaxInterPacketDelay);
                    _InterPacketDelay = int.Parse(MaxInterPacketDelay.ToString());
                    MessageBox.Show(this, message, "Inter Packet Delay Calculation Error");
                }

                InterPacketDelayTextBox.Text = _InterPacketDelay.ToString();

                InterPacketDelayTextBoxus.Text = ((double)((double)_InterPacketDelay * 1000000 / (double)TickFrequency)).ToString();

                MbitPerSecTextBox.Text = (MaxFps * (double)TotalSize * 8.0 / 1000000.0).ToString("0.0");
            }

           CalculateButton.Enabled = false;
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
        }

        private void CancelButton_Click(object sender, EventArgs e)
        {

        }

        private void InterPacketDelayForm_Load(object sender, EventArgs e)
        {
            // Gets a NumberFormatInfo associated with the en-US culture.
            NumberFormatInfo nfi = new CultureInfo( "en-US", false ).NumberFormat;
            nfi.NumberDecimalSeparator = ",";

            // We need to get all the values from the camera
            if (myCamera != null)
            {
                object obj = null;

                // PayloadSize (Mandatory GenICam)
                obj = myCamera.GetNodeValue("PayloadSize");
                if (obj != null)
                {
                    try
                    {
                        PayloadSize = int.Parse(obj.ToString());
                        PayloadSizeTextBox.Text = PayloadSize.ToString();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(this, ex.Message);
                    }
                }

                // Packet Size (Recommended GenICam)
                obj = myCamera.GetNodeValue("GevSCPSPacketSize");
                if (obj != null)
                {
                    try
                    {
                        PacketSize = int.Parse(obj.ToString());
                        PacketSizeTextBox.Text = PacketSize.ToString();
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(this, ex.Message);
                    }
                }
                else
                {
                    // If not defined then we need to read it manually from register
                    // Allocate the managed buffer to hold the data:
                    byte[] buffer = new byte[4];

                    // Allocate a handle and pin the data in memory so we can get a pointer to data area:
                    GCHandle gch = GCHandle.Alloc(buffer, GCHandleType.Pinned);

                    // Get the pointer of the data inside managed array 
                    IntPtr bufferPtr = gch.AddrOfPinnedObject();

                    uint size = 4;
                    Jai_FactoryWrapper.EFactoryError retsta = Jai_FactoryWrapper.J_Camera_ReadData(myCamera.CameraHandle, 0xD04, bufferPtr, ref size);

                    PacketSize = IPAddress.HostToNetworkOrder(Marshal.ReadInt32(bufferPtr)) & 0x0000FFFF;
                    PacketSizeTextBox.Text = PacketSize.ToString();

                    // Free the handle again after we have call the unmanaged dll 
                    gch.Free();
                }

                //Get the real Ethernet link speed
                obj = myCamera.GetNodeValue("GevLinkSpeed");
                if (obj != null)
                {
                    try
                    {
                        Int32 linkSpeed = Convert.ToInt32(obj);
                        TransmissionRate = linkSpeed * (Int32)1E6 / 8;
                    }
                    catch (Exception ex)
                    {
                        System.Diagnostics.Debug.WriteLine(ex.Message);
                    }
                }

                // Calculate number of packets
                int PacketPayload = PacketSize - 36;
                PacketCount = (PayloadSize / PacketPayload) + 2;

                if ((PayloadSize % PacketPayload) != 0)
                    PacketCount++;

                PacketCountTextBox.Text = PacketCount.ToString();

                // Get the Tick Frequency
                // If not defined then we need to read it manually from register
                // Allocate the managed buffer to hold the data:
                byte[] buffer2 = new byte[8];

                // Allocate a handle and pin the data in memory so we can get a pointer to data area:
                GCHandle gch2 = GCHandle.Alloc(buffer2, GCHandleType.Pinned);

                // Get the pointer of the data inside managed array 
                IntPtr bufferPtr2 = gch2.AddrOfPinnedObject();

                uint size2 = 8;
                Jai_FactoryWrapper.EFactoryError retsta2 = Jai_FactoryWrapper.J_Camera_ReadData(myCamera.CameraHandle, 0x93C, bufferPtr2, ref size2);

                TickFrequency = IPAddress.HostToNetworkOrder(Marshal.ReadInt64(bufferPtr2));

                // Free the handle again after we have call the unmanaged dll 
                gch2.Free();

                TickFrequenceTextBox.Text = TickFrequency.ToString();

                OverHead = (PacketCount * 54) + 36 + 12; // Total # of packets * 54 (Ethernet+IP+UDP+GVSP headers) + 36 (Image Leader payload) + 10 (Image Trailer payload + 2 extra padding??)
                OverheadTextBox.Text = OverHead.ToString();

                TotalSize = PayloadSize + OverHead;
                TotalSizeTextBox.Text = TotalSize.ToString();

                TransmissionTime = (double)TotalSize / (double)TransmissionRate;
                TransmissionTimeTextBox.Text = ((double)(TransmissionTime*1000.0)).ToString("0.000000");

                PacketTransmissionTimeTextBox.Text = ((double)((double)PacketSize * 1000000.0 / (double)TransmissionRate)).ToString("0.000000");

                MaxFpsTextBox.Text = MaxFps.ToString("0.00");

//                MaxErrorTextBox.Text = ExpectedBandwidthUsage.ToString("0.00");
                MaxErrorUpDown.Value = Convert.ToDecimal(ExpectedBandwidthUsage);

                // Update Current InterPacketDelay field
                CNode InterPacketDelayNode = myCamera.GetNode("GevSCPD");

                if (InterPacketDelayNode == null)
                {
                    InterPacketDelayNode = myCamera.GetNode("PacketDelay");

                    if (InterPacketDelayNode == null)
                    {
                        // We then need to read it directly from the register
                        // If not defined then we need to read it manually from register
                        // Allocate the managed buffer to hold the data:
                        byte[] buffer3 = new byte[4];

                        // Allocate a handle and pin the data in memory so we can get a pointer to data area:
                        GCHandle gch3 = GCHandle.Alloc(buffer3, GCHandleType.Pinned);

                        // Get the pointer of the data inside managed array 
                        IntPtr bufferPtr3 = gch3.AddrOfPinnedObject();

                        uint size3 = 4;
                        Jai_FactoryWrapper.EFactoryError retsta = Jai_FactoryWrapper.J_Camera_ReadData(myCamera.CameraHandle, 0xD04, bufferPtr3, ref size3);

                        CurrentInterPacketDelay = IPAddress.HostToNetworkOrder(Marshal.ReadInt32(bufferPtr3));
                        CurrentInterPacketDelayTextBox.Text = CurrentInterPacketDelay.ToString();

                        MinInterPacketDelay = 0;
                        MinTextBox.Text = MinInterPacketDelay.ToString();

                        MaxInterPacketDelay = Int32.MaxValue;
                        MaxTextBox.Text = MaxInterPacketDelay.ToString();

                        // Free the handle again after we have call the unmanaged dll 
                        gch3.Free();
                    }
                    else
                    {
                        CurrentInterPacketDelay = Convert.ToInt32(InterPacketDelayNode.Value);
                        CurrentInterPacketDelayTextBox.Text = CurrentInterPacketDelay.ToString();

                        MinInterPacketDelay = Convert.ToInt32(InterPacketDelayNode.Min);
                        MinTextBox.Text = MinInterPacketDelay.ToString();

                        MaxInterPacketDelay = Convert.ToInt64(InterPacketDelayNode.Max);
                        MaxTextBox.Text = MaxInterPacketDelay.ToString();
                    }
                }
                else
                {
                    CurrentInterPacketDelay = Convert.ToInt32(InterPacketDelayNode.Value);
                    CurrentInterPacketDelayTextBox.Text = CurrentInterPacketDelay.ToString();

                    MinInterPacketDelay = Convert.ToInt32(InterPacketDelayNode.Min);
                    MinTextBox.Text = MinInterPacketDelay.ToString();

                    MaxInterPacketDelay = Convert.ToInt64(InterPacketDelayNode.Max);
                    MaxTextBox.Text = MaxInterPacketDelay.ToString();
                }

                // Try to get the frame rate from AcquisitionFrameRateRaw
                obj = myCamera.GetNodeValue("AcquisitionFrameRateRaw");
                if (obj != null)
                {
                    try
                    {
                        string[] split = obj.ToString().Split(new char[] { ' ' });
                        if (split.Length > 1)
                        {
                            string fpsstring = split[0].Replace('.', ',');
                            MaxFps = double.Parse(fpsstring, nfi);
                            MaxFpsTextBox.Text = MaxFps.ToString("0.00");
                        }
                        else
                        {
                            // Ok - this might be because we have characters right after the number :-(
                            string lowercharstring = split[0].ToLower();
                            string numbers = lowercharstring.TrimEnd(new char[] { ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' });

                            string fpsstring = numbers.Replace('.', ',');
                            MaxFps = double.Parse(fpsstring, nfi);
                            MaxFpsTextBox.Text = MaxFps.ToString("0.00");
                        }
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show(this, ex.Message);
                    }
                }
                else
                {
                    // It might be an IEnumeration with the FrameRate value??
                    obj = myCamera.GetNodeValue("AcquisitionFrameRate");
                    if (obj != null)
                    {
                        try
                        {
                            string[] split = obj.ToString().Split(new char[] { ' ' });
                            if (split.Length > 1)
                            {
                                string fpsstring = split[0].Replace('.', ',');
                                MaxFps = double.Parse(fpsstring,nfi);
                                MaxFpsTextBox.Text = MaxFps.ToString("0.00");
                            }
                            else
                            {
                                // Ok - this might be because we have characters right after the number :-(
                                string lowercharstring = split[0].ToLower();
                                string numbers = lowercharstring.TrimEnd(new char[] { ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' });

                                string fpsstring = numbers.Replace('.', ',');
                                MaxFps = double.Parse(fpsstring,nfi);
                                MaxFpsTextBox.Text = MaxFps.ToString("0.00");
                            }
                        }
                        catch (Exception ex)
                        {
                            MessageBox.Show(this, ex.Message);
                        }
                    }
                }
                CalculateButton_Click(null, null);
            }
            Initialized = true;
        }

        private void label23_Click(object sender, EventArgs e)
        {

        }

        private void MaxErrorUpDown_ValueChanged(object sender, EventArgs e)
        {
            if (Initialized)
            {
                CalculateButton_Click(null, null);
            }
        }
    }
}