using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;

namespace ConsoleApplication
{
   class Program
   {
       static Jai_FactoryDotNET.CFactory factory;
       static CCamera myCamera;

       static void Main(string[] args)
       {
           // GenICam nodes
           CNode myWidthNode;
           CNode myHeightNode;

           Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

           factory = new Jai_FactoryDotNET.CFactory();

           error = factory.Open("");
           CFactory.EDriverType myPreferredDriverType = CFactory.EDriverType.FilterDriver;

           Console.Out.WriteLine("Factory version = " + factory.Version);
           Console.Out.WriteLine("Build Date =      " + factory.BuildDate);
           Console.Out.WriteLine("Build Time =      " + factory.BuildTime);
           Console.Out.WriteLine("Manufacturer =    " + factory.Manufacturer);

           bool AnyChanges = factory.UpdateCameraList(myPreferredDriverType);

           if (factory.NumOfCameras > 0)
           {
               int index = 1;
               foreach (CCamera c in factory.CameraList)
               {
                   Console.Out.WriteLine(index.ToString() + ") " + c.ModelName);
                   index++;
               }

               Console.Out.WriteLine("Starting Live Video from a camera!\nSelect camera number and press Return to continue.");
               int CameraChar = Console.In.Read();
               int CameraIndex = 0;

               // Did the user press 1-9
               if ((CameraChar >= 0x31) && (CameraChar <= 0x39))
                   CameraIndex = CameraChar - 0x31;

               factory.CameraList[CameraIndex].Open();

               myCamera = factory.CameraList[CameraIndex];

               // Get the Width GenICam Node
               myWidthNode = myCamera.GetNode("Width");
               if (myWidthNode != null)
               {
                   SetFramegrabberValue("Width", (Int64)myWidthNode.Value);
               }

               myHeightNode = myCamera.GetNode("Height");
               if (myHeightNode != null)
               {
                   SetFramegrabberValue("Height", (Int64)myHeightNode.Value);
               }

               SetFramegrabberPixelFormat();

               factory.CameraList[CameraIndex].StartImageAcquisition(true, 5);

               Console.Out.WriteLine("Press any key to exit");
               while (!Console.KeyAvailable)
               {
                   Application.DoEvents();
               }

               factory.CameraList[CameraIndex].StopImageAcquisition();
           }
           else
           {
               Console.Out.WriteLine("No Cameras found!\nPress Return to continue.");
               Console.In.ReadLine();
           }

           factory.Close();
       }

       private static void SetFramegrabberValue(String nodeName, Int64 int64Val)
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

       static private void SetFramegrabberPixelFormat()
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
