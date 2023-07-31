using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using Jai_FactoryDotNET;

namespace UserDrawSample
{
    public partial class Form1 : Form
    {
        // Main factory object
        CFactory myFactory = new CFactory();

        // Opened camera obejct
        CCamera myCamera;

        // GenICam nodes
        CNode myWidthNode;
        CNode myHeightNode;
        CNode myGainNode;

        String imageOverlayString = "Image Overlay Text Example!";
        String viewWindowOverlayString = "View Window Overlay Text Example!";

        UInt64 iBlockID;
        UInt64 iLastBlockID;
        uint iAwaitDelivery;
        uint iQueuedBuffers;
        uint iMissingPackets;
        uint iTotalMissingPackets;
        ulong iTimeStamp;
        ulong iLastTimeStamp;
        double fTimeDifference;
        const ulong TicksPerMillisecond = 62500000 / 1000;
        ulong iNumLostFrames;
        uint iNumBlockIDSequenceErrors;
        bool bBlockIDSequenceErrorFlag;

        public Form1()
        {
            InitializeComponent();

            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            iBlockID = 0;
            iLastBlockID = 0;
            iAwaitDelivery = 0;
            iQueuedBuffers = 0;
            iMissingPackets = 0;
            iTotalMissingPackets = 0;
            iTimeStamp = 0;
            iLastTimeStamp = 0;
            fTimeDifference = 0.0;
            iNumLostFrames = 0;
            iNumBlockIDSequenceErrors = 0;
            bBlockIDSequenceErrorFlag = false;

            // Open the factory with the default Registry database
            error = myFactory.Open("");

            // Search for cameras and update all controls
            SearchButton_Click(null, null);
        }

        private void SearchButton_Click(object sender, EventArgs e)
        {
            // Search for any new cameras using Filter Driver
            myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);
            //myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.SocketDriver);

            if (myFactory.CameraList.Count > 0)
            {
                // Open the camera
                myCamera = myFactory.CameraList[0];

                CameraIDTextBox.Text = myCamera.CameraID;

                myCamera.Open();
                
                myCamera.EnablePacketResend = true;
                myCamera.PassCorruptFrames = false;
                //myCamera.EnableColorInterpolation = false;

                // Assign the View Window delegate so we get called each time the image is painted in the View Window
                myCamera.ViewWindowEventDelegate += new CCamera.ImageViewWindowDelegate(myCamera_ViewWindowEventDelegate);
                
                // Set the callback mask to be the two User Draw types
                Jai_FactoryWrapper.IVWOptionValue optionValue = new Jai_FactoryWrapper.IVWOptionValue();
                optionValue.UInt32Value = (UInt32) (Jai_FactoryWrapper.EIVWCallbackType.J_IVW_CB_USER_DRAW_IMAGE | Jai_FactoryWrapper.EIVWCallbackType.J_IVW_CB_USER_DRAW_VIEW_WINDOW);

                Jai_FactoryWrapper.J_Image_SetViewWindowOption(myCamera.WindowHandle, Jai_FactoryWrapper.EIVWOptionType.CallbackMask, ref optionValue);

                StartButton.Enabled = true;
                StopButton.Enabled = false;
                stretchCheckBox.Enabled = true;
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

                        //// Get Graphics object from the Device Context handle passed
                        //Graphics g = Graphics.FromHdcInternal(CallbackValue.UserDrawInfoValue.hDeviceContext);

                        //// Create font and brush.
                        //Font drawFont = new Font("Arial", 16);
                        //SolidBrush drawBrush = new SolidBrush(Color.Red);

                        //// Create point for upper-left corner of drawing.
                        //PointF drawPoint = new PointF(100.0F, 100.0F);

                        //g.DrawString(imageOverlayString, drawFont, drawBrush, drawPoint);

                        //if (enableUserDrawImageGridCheckBox.Checked)
                        //{
                        //    // Create a Solid red pen
                        //    Pen  redPen = new Pen(Color.Red, 2.0F);

                        //    for (int x=100; x<CallbackValue.UserDrawInfoValue.Width; x+=100)
                        //    {
                        //        g.DrawLine(redPen, x, 0, x, (int) (CallbackValue.UserDrawInfoValue.Height-1));
                        //    }

                        //    for (int y=100; y<CallbackValue.UserDrawInfoValue.Height; y+=100)
                        //    {
                        //        g.DrawLine(redPen, 0, y, (int) (CallbackValue.UserDrawInfoValue.Width-1), y);
                        //    }
                        //}
                    }
                    break;

                case Jai_FactoryWrapper.EIVWCallbackType.J_IVW_CB_USER_DRAW_VIEW_WINDOW:
                    {
                        // Get Graphics object from the Device Context handle passed
                        Graphics g = Graphics.FromHdcInternal(CallbackValue.UserDrawInfoValue.hDeviceContext);

                        // Create font and brush.
                        Font drawFont = new Font("Arial", 16);
                        SolidBrush drawBrushWhite = new SolidBrush(Color.White);
                        SolidBrush drawBrushRed = new SolidBrush(Color.Red);

                        // Create point for upper-left corner of drawing.
                        PointF drawPoint = new PointF(100.0F, 100.0F);

                        string sInfoString = "BlockID\t\t\t" + iBlockID;
                        g.DrawString(sInfoString, drawFont, drawBrushWhite, drawPoint);

                        sInfoString = "Timestamp\t\t" + iTimeStamp;
                        drawPoint.Y += 25;
                        g.DrawString(sInfoString, drawFont, drawBrushWhite, drawPoint);

                        sInfoString = "Timestamp Diff\t\t" + fTimeDifference + "ms";
                        drawPoint.Y += 25;
                        g.DrawString(sInfoString, drawFont, drawBrushWhite, drawPoint);

                        sInfoString = "Missing Packets\t\t" + iMissingPackets;
                        drawPoint.Y += 25;
                        if (iMissingPackets > 0)
                        {
                            g.DrawString(sInfoString, drawFont, drawBrushRed, drawPoint);
                        }
                        else
                        {
                            g.DrawString(sInfoString, drawFont, drawBrushWhite, drawPoint);
                        }

                        sInfoString = "Total Missed Packets\t" + iTotalMissingPackets;
                        drawPoint.Y += 25;
                        g.DrawString(sInfoString, drawFont, drawBrushWhite, drawPoint);

                        sInfoString = "Missed Frames\t\t" + iNumLostFrames;
                        drawPoint.Y += 25;
                        g.DrawString(sInfoString, drawFont, drawBrushWhite, drawPoint);

                        sInfoString = "BlockID Breaks\t\t" + iNumBlockIDSequenceErrors;
                        drawPoint.Y += 25;
                        if (bBlockIDSequenceErrorFlag)
                        {
                            g.DrawString(sInfoString, drawFont, drawBrushRed, drawPoint);
                        }
                        else
                        {
                            g.DrawString(sInfoString, drawFont, drawBrushWhite, drawPoint);
                        }


                        //g.DrawString(viewWindowOverlayString, drawFont, drawBrushWhite, drawPoint);

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

                // Reset display statistics
                iBlockID = 0;
                iLastBlockID = 0;
                iAwaitDelivery = 0;
                iQueuedBuffers = 0;
                iMissingPackets = 0;
                iTotalMissingPackets = 0;
                iTimeStamp = 0;
                iLastTimeStamp = 0;
                fTimeDifference = 0.0;
                iNumLostFrames = 0;
                iNumBlockIDSequenceErrors = 0;
                bBlockIDSequenceErrorFlag = false;
                myCamera.TotalMissingPackets = 0;
                
                // Attach image delegate function
                myCamera.NewImageDelegate += new Jai_FactoryWrapper.ImageCallBack(HandleImage);

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

                // Detach delegate
                myCamera.NewImageDelegate -= new Jai_FactoryWrapper.ImageCallBack(HandleImage);

                StartButton.Enabled = true;
                StopButton.Enabled = false;
                stretchCheckBox.Enabled = true;
            }
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

        // Local callback function used for handle new images
        void HandleImage(ref Jai_FactoryWrapper.ImageInfo ImageInfo)
        {
            // Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // This is in fact a callback, so we would need to handle the data as fast as possible and the frame buffer 
            // we get as a parameter will be recycled when we terminate.
            // This leaves us with two choices:
            // 1) Get the work we need to do done ASAP and return
            // 2) Make a copy of the image data and process this afterwards
            //
            // We have the access to the buffer directly via the ImageInfo.ImageBuffer variable
            // 
            // We can access the raw frame buffer bytes if we use "unsafe" code and pointer
            // To do this we need to set the "Allow unsafe code" in the project properties and then access the data like:
            //
            // unsafe
            // {
            //     // Cast IntPtr to pointer to byte
            //     byte* pArray = (byte*)ImageInfo.ImageBuffer;
            //     // Do something with the data
            //     // Read values
            //     byte value = pArray[10];
            //     // Write values
            //     for (int i = 0; i < 1000; i++)
            //         pArray[i] = (byte)(i % 255);
            // }
            //
            // // If we want to copy the data instead we can do like this without Unsafe code:
            // byte[] array = null;
            //
            // if (ImageInfo.ImageBuffer != IntPtr.Zero)
            // {
            //     // Allocate byte array that can contain the copy of data
            //     array = new byte[ImageInfo.ImageSize];
            //     // Do the copying
            //     Marshal.Copy(ImageInfo.ImageBuffer, array, 0, (int)ImageInfo.ImageSize);
            //
            //     // Do something with the raw data
            //     byte val = array[10];
            //}

            iTimeStamp = ImageInfo.TimeStamp;

            //Although these parameters are supported by GigE Vision cameras, other camera interfaces may not.
            try
            {
                iBlockID = ImageInfo.BlockID;
                iAwaitDelivery = ImageInfo.AwaitDelivery;
                iQueuedBuffers = ImageInfo.QueuedBuffers;
                iMissingPackets = ImageInfo.MissingPackets;
                iTotalMissingPackets = myCamera.TotalMissingPackets;
                iNumLostFrames = myCamera.NumFramesLost;
            }
            catch (Jai_FactoryDotNET.Jai_FactoryWrapper.FactoryErrorException e)
            {
                System.Diagnostics.Debug.WriteLine(e.Message);
            }

            // Compute frame timestamp delta
            if (iLastTimeStamp == 0) // Handle very first frame
            {
                iLastTimeStamp = iTimeStamp;
            }
            fTimeDifference = (iTimeStamp - iLastTimeStamp) / TicksPerMillisecond;
            iLastTimeStamp = iTimeStamp;

            // Output debug data
            Debug.WriteLine("Got BlockID " + iBlockID + " @ " + iTimeStamp + " (after " + fTimeDifference + " ms)" + "\tMissing Packets: " + iMissingPackets + " (Total: " + iTotalMissingPackets + ")" + "\tBlockID Seq Errors: " + iNumBlockIDSequenceErrors + "\tMissed Frames: " + iNumLostFrames);

            // Check for BlockID sequence breaks or repeats
            bBlockIDSequenceErrorFlag = false;
            if (iBlockID == iLastBlockID)
            {
                iNumBlockIDSequenceErrors++;
                bBlockIDSequenceErrorFlag = true;
                Debug.WriteLine("\tGot Dupe BlockID " + iBlockID + "!");
            } else if (iBlockID != iLastBlockID + 1)
            {
                iNumBlockIDSequenceErrors++;
                bBlockIDSequenceErrorFlag = true;
                Debug.WriteLine("\tGot BlockID " + iBlockID + " after BlockID " + iLastBlockID + "!");
            }
            iLastBlockID = iBlockID;

            // Draw horizontal and vertical lines
            // Note this assumes 8bit pixels else you'll have to modify
            const uint iNumDivisions = 100;
            const uint iLineWidth = 4;
            const uint iLineShade = 255;
            uint iWidth = ImageInfo.SizeX;
            uint iHeight = ImageInfo.SizeY;
            uint iLineJumpVertical = iHeight / iNumDivisions;
            uint iLineJumpHorizontal = iWidth / iNumDivisions;
            uint iVLinePos = (uint)(ImageInfo.BlockID % iNumDivisions) * iLineJumpVertical;
            uint iHLinePos = (uint)(ImageInfo.BlockID % iNumDivisions) * iLineJumpHorizontal;
            unsafe
            {
                // Cast IntPtr to pointer to byte
                byte* pArray = (byte*)ImageInfo.ImageBuffer;
                
                // Write vertical lines
                for (int line = 0; line < iLineWidth; line++)
                {
                    for (int x = 0; x < iWidth; x++)
                    {
                        pArray[(iVLinePos + line) * iWidth + x] = (byte)(iLineShade);
                    }
                }
                // Write horizontal lines
                for (int line = 0; line < iLineWidth; line++)
                {
                    for (int y = 0; y < iHeight; y++)
                    {
                        pArray[y * iWidth + iHLinePos + line] = (byte)(iLineShade);
                    }
                }

                // Now check first and last pixel for special value and then blockid in second and third pixel
                if ((pArray[0] == 0x42) && (pArray[ImageInfo.ImageSize - 1] == 0x42))
                {
                    // Okay are there a LOT of missing packets?
                    if (iMissingPackets < 200)
                    {
                        // No so it's a bogus buffer!
                        int iOldBlockID = pArray[1] * 256 + pArray[2];
                        Debug.WriteLine("\tBuffer not filled in BlockID " + iBlockID + " (has data from BlockID " + iOldBlockID + ")!");
                    }
                }
                // Then fill in new marker data
                pArray[0] = 0x42;
                pArray[1] = (byte)(iBlockID / 256);
                pArray[2] = (byte)(iBlockID % 256);
                pArray[ImageInfo.ImageSize - 1] = 0x42;
            }

            return;
        }
    }
}