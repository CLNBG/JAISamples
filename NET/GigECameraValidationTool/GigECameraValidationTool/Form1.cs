using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;
using System.IO;

namespace GigECameraValidationTool
{
    public partial class Form1 : Form
    {
        private CFactory myFactory = new CFactory();
        private CCamera myCamera;

        private List<IntegerTypeNode> IntList = new List<IntegerTypeNode>();
        private List<FloatTypeNode> FloatList = new List<FloatTypeNode>();
        private List<EnumTypeNode> EnumList = new List<EnumTypeNode>();
        private List<Node> NullNodeList = new List<Node>();
        
        public Nullable<int> iOldSelectedIndex = null;

        private int featureProgressCount = 0;       // Count for progress bar
        private Object thisLock = new Object(); 
        System.Windows.Forms.Timer t = new Timer(); // timer for progress bar

        public Form1()
        {
            InitializeComponent();

            // Find cameras and list in the combo box
            DiscoverCameras();
        }

        #region VALIDATION_CODE
        private void ValidateNodeValues()
        {
            List<CNode> features = myCamera.GetSubFeatures("Root");

            // configure progress bar
            featureProgressCount = 0;
            progressBar1.Minimum = 0;
            progressBar1.Maximum = features.Count;
            progressBar1.Value = 0;
            t.Interval = 100;
            t.Tick += new EventHandler(t_Tick);
            t.Start();

            foreach (CNode node in features)
            {
                List<CNode> child = myCamera.GetSubFeatures(node.Name);

                foreach (CNode childNode in child)
                {
                    if (childNode.IsAvailable && childNode.IsReadable)
                    {
                        ValidationReturnCode result;

                        if (childNode.NodeType == Jai_FactoryWrapper.EConfNodeType.IInteger)
                        {
                            result = ValidateIntegerNode(childNode);
                            switch (result)
                            {
                                case ValidationReturnCode.NotPassed:
                                    IntList.Add(new IntegerTypeNode(node.DisplayName, childNode.DisplayName, childNode.Name, Int64.Parse(childNode.Min.ToString()), Int64.Parse(childNode.Max.ToString()), Int64.Parse(childNode.Value.ToString())));
                                    break;
                                case ValidationReturnCode.NullValue:
                                    NullNodeList.Add(new Node(node.DisplayName, childNode.DisplayName, childNode.Name));
                                    break;
                            }
                        }
                        else if (childNode.NodeType == Jai_FactoryWrapper.EConfNodeType.IFloat)
                        {
                            result = ValidateFloatNode(childNode);
                            switch (result)
                            {
                                case ValidationReturnCode.NotPassed:
                                    FloatList.Add(new FloatTypeNode(node.DisplayName, childNode.DisplayName, childNode.Name, double.Parse(childNode.Min.ToString()), double.Parse(childNode.Max.ToString()), double.Parse(childNode.Value.ToString())));
                                    break;
                                case ValidationReturnCode.NullValue:
                                    NullNodeList.Add(new Node(node.DisplayName, childNode.DisplayName, childNode.Name));
                                    break;
                            }
                        }
                        else if (childNode.NodeType == Jai_FactoryWrapper.EConfNodeType.IEnumeration)
                        {
                            result = ValidateEnumNode(childNode);
                            switch (result)
                            {
                                case ValidationReturnCode.NotPassed:
                                    EnumList.Add(new EnumTypeNode(node.DisplayName, childNode.DisplayName, childNode.Name, childNode.Value.ToString()));
                                    break;
                                case ValidationReturnCode.NullValue:
                                    NullNodeList.Add(new Node(node.DisplayName, childNode.DisplayName, childNode.Name));
                                    break;
                            }
                        }
                    }
                }

                lock (thisLock)
                {
                    featureProgressCount++;
                }
            }

            lblIntCount.Text = IntList.Count.ToString();
            lblEnumCount.Text = EnumList.Count.ToString();
            lblFloatCount.Text = FloatList.Count.ToString();
            fileWriter();
        }

        private ValidationReturnCode ValidateIntegerNode(CNode node)
        {
            try
            {
                if (node.Value != null)
                {
                    Int64 nodeValue = Int64.Parse(node.Value.ToString());
                    Int64 max = Int64.Parse(node.Max.ToString());
                    Int64 min = Int64.Parse(node.Min.ToString());
                    Int64 inc = Int64.Parse(node.Inc.ToString());

                    if (nodeValue > max || nodeValue < min || ((nodeValue - min) % inc) != 0)
                        return ValidationReturnCode.NotPassed;
                    else 
                        return ValidationReturnCode.Passed;
                }
                else
                {
                    return ValidationReturnCode.NullValue;
                }
            }
            catch (Jai_FactoryWrapper.FactoryErrorException ex)
            {
                MessageBox.Show("Error reading node value!" + ex.Message);
                return ValidationReturnCode.Unknown;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return ValidationReturnCode.Unknown;
            }
        }

        private ValidationReturnCode ValidateFloatNode(CNode node)
        {
            try
            {

                if (node.Value != null)
                {
                    double nodeValue = double.Parse(node.Value.ToString());
                    double max = double.Parse(node.Max.ToString());
                    double min = double.Parse(node.Min.ToString());

                    if (nodeValue > max || nodeValue < min)
                        return ValidationReturnCode.NotPassed;
                    else
                        return ValidationReturnCode.Passed;
                }
                else
                {
                    return ValidationReturnCode.NullValue;
                }
            }
            catch (Jai_FactoryWrapper.FactoryErrorException ex)
            {
                MessageBox.Show("Error reading node value!" + ex.Message);
                return ValidationReturnCode.Unknown;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return ValidationReturnCode.Unknown;
            }
        }

        private ValidationReturnCode ValidateEnumNode(CNode node)
        {
            CNode.IEnumValue[] values = node.EnumValues;

            try
            {
                foreach (CNode.IEnumValue value in values)
                {
                    if (value.DisplayName == node.Value.ToString())
                        return ValidationReturnCode.Passed;
                }
                return ValidationReturnCode.NotPassed;
            }
            catch (Jai_FactoryWrapper.FactoryErrorException ex)
            {
                MessageBox.Show("Error reading node value!" + ex.Message);
                return ValidationReturnCode.Unknown;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                return ValidationReturnCode.Unknown;
            }
        }
        #endregion

        private void DiscoverCameras()
        {
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Open the Factory
            error = myFactory.Open("");

            statusBarLabel.Text = "Searching for cameras...";

            if (error == Jai_FactoryWrapper.EFactoryError.Success)
            {
                camListComboBox.Items.Clear(); // delete camera list from combo box

                // Search for any new GigE cameras
                myFactory.UpdateCameraList(CFactory.EDriverType.Undefined);

                if (myFactory.CameraList.Count > 0)
                {
                    for (int i = 0; i < myFactory.CameraList.Count; i++)
                    {
                        string sList = myFactory.CameraList[i].ModelName;

                        if (myFactory.CameraList[i].CameraID.Contains("INT=>FD"))
                            sList += " (Filter Driver)";
                        else if (myFactory.CameraList[i].CameraID.Contains("INT=>SD"))
                            sList += " (Socket Driver)";
                        else
                            continue;

                        camListComboBox.Items.Add(sList);
                    }

                    if (0 == camListComboBox.Items.Count)
                    {
                        MessageBox.Show(this, "This sample only works with GigE cameras.", "Camera Transport Name Error", MessageBoxButtons.OK, MessageBoxIcon.Stop);
                    }
                }
                else
                {
                    MessageBox.Show("No camera found");
                }

                statusBarLabel.Text = "Found " + camListComboBox.Items.Count.ToString() + " camera(s). Select camera from the ListBox to open it.";
            }
            else
                showErrorMsg(error);
        }

        private void showErrorMsg(Jai_FactoryWrapper.EFactoryError error)
        {
            String sErrorMsg = "Error = " + error.ToString();

            MessageBox.Show(sErrorMsg);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            myFactory.Close();
        }

        private void fileWriter()
        {
            using (StreamWriter sw = new StreamWriter("log.txt"))
            {
                // write the current date
                sw.WriteLine("Test Date:" + DateTime.Now.ToString("MMMM dd, yyyy H:mm:ss zzz"));
                sw.WriteLine();
                sw.WriteLine("========== Camera Information ==========");
                sw.WriteLine("Model Name: " + myCamera.ModelName);
                sw.WriteLine("Serial Number: " + myCamera.SerialNumber);
                sw.WriteLine("Device Version: " + myCamera.GetNodeValue("DeviceVersion"));
                sw.WriteLine("Device Firmware Version: " + myCamera.GetNodeValue("DeviceFirmwareVersion"));
                sw.WriteLine("Device FPGA Version: " + myCamera.GetNodeValue("DeviceFPGAVersion"));
                sw.WriteLine("XML File Information: " + myCamera.GetNodeValue("GevFirstURL"));
                sw.WriteLine();

                sw.WriteLine("========== Test Summary ==========");
                if (IntList.Count != 0)
                    sw.WriteLine("Number of errors in IInteger type: " + IntList.Count);
                
                if (FloatList.Count != 0)
                    sw.WriteLine("Number of error in IFolat type: " + FloatList.Count);

                if (EnumList.Count != 0)
                    sw.WriteLine("Number of error in IEnumeration type: " + EnumList.Count);
 
                if (IntList.Count == 0 && FloatList.Count == 0 && EnumList.Count == 0)
                    sw.WriteLine("No Error found");

                if (NullNodeList.Count > 0)
                    sw.WriteLine("Number of nodes that have null values: " + NullNodeList.Count);

                sw.WriteLine();

                if (IntList.Count > 0)
                {
                    sw.WriteLine("========== IInteger Nodes ==========");
                    sw.WriteLine("Number of errors = " + IntList.Count);
                    sw.WriteLine("");
                    foreach (IntegerTypeNode i in IntList)
                    {
                        sw.WriteLine("Parent DisplayName: " + i.ParentDisplayName);
                        sw.WriteLine("DisplayName: " + i.DisplayName);
                        sw.WriteLine("GenICam Name: " + i.GenIcamName);
                        sw.WriteLine("Minimum value: " + i.MinValue.ToString());
                        sw.WriteLine("Maximum value: " + i.MaxValue.ToString());
                        sw.WriteLine("Actual Node Value: " + i.ActualValue.ToString());
                        sw.WriteLine();
                        sw.WriteLine();
                    }
                }

                if (FloatList.Count > 0)
                {
                    sw.WriteLine("========== IFloat Nodes==========");
                    sw.WriteLine("Number of errors = " + FloatList.Count);
                    sw.WriteLine("");
                    foreach (FloatTypeNode f in FloatList)
                    {
                        sw.WriteLine("Parent DisplayName: " + f.ParentDisplayName);
                        sw.WriteLine("DisplayName: " + f.DisplayName);
                        sw.WriteLine("GenICam Name: " + f.GenIcamName);
                        sw.WriteLine("Minimum value: " + f.MinValue.ToString());
                        sw.WriteLine("Maximum value: " + f.MaxValue.ToString());
                        sw.WriteLine("Actual Node Value: " + f.ActualValue);
                        sw.WriteLine();
                        sw.WriteLine();
                    }
                }

                if (EnumList.Count > 0)
                {
                    sw.WriteLine("========== IEnumeration Nodes ==========");
                    sw.WriteLine("Number of errors = " + EnumList.Count);
                    sw.WriteLine("");
                    foreach (EnumTypeNode e in EnumList)
                    {
                        sw.WriteLine("Parent DisplayName: " + e.ParentDisplayName);
                        sw.WriteLine("DisplayName: " + e.DisplayName);
                        sw.WriteLine("GenICam Name: " + e.GenIcamName);
                        sw.WriteLine("Actual Node Value: " + e.ActualValue);
                        sw.WriteLine();
                        sw.WriteLine();
                    }
                }

                if (NullNodeList.Count > 0)
                {
                    sw.WriteLine("========== There are {0} nodes that have null values. ==========", NullNodeList.Count);
                    sw.WriteLine();
                    foreach (Node n in NullNodeList)
                    {
                        sw.WriteLine("Parent DisplayName: " + n.ParentDisplayName);
                        sw.WriteLine("DisplayName: " + n.DisplayName);
                        sw.WriteLine("GenICam Name: " + n.GenIcamName);
                        sw.WriteLine();
                        sw.WriteLine();
                    }
                }

                sw.Close();
            }
        }

        private void camListComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // do nothing if the selected camera is the same as the previous one
            if (myCamera != null && iOldSelectedIndex != camListComboBox.SelectedIndex)
            {
                // Close any other opened camera so we can open a new one...
                myCamera.Close();
                myCamera = null;
            }

            if (myCamera == null)
            {
                // open the selected camera from the list
                myCamera = myFactory.CameraList[camListComboBox.SelectedIndex];

                cameraIdTextBox.Text = myCamera.CameraID;
                error = myCamera.Open();

                if (error != Jai_FactoryWrapper.EFactoryError.Success)
                {
                    showErrorMsg(error);
                    return;
                }
                else
                {
                    try
                    {
                        myCamera.GetNode("UserSetSelector").Value = "Default";
                        Jai_FactoryWrapper.J_Camera_ExecuteCommand(myCamera.CameraHandle, "UserSetLoad");

                        statusBarLabel.Text = camListComboBox.SelectedItem.ToString() + " has been selected";

                        // save the selected camera index
                        iOldSelectedIndex = camListComboBox.SelectedIndex;
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("Error:" + ex.Message);
                    }

                }
            }
        }

        private void btnDisplayResult_Click(object sender, EventArgs e)
        {
            if (File.Exists("log.txt"))
                System.Diagnostics.Process.Start("notepad.exe", "log.txt");
        }

        private void t_Tick(object sender, EventArgs e)
        {
            lock (thisLock)
            {
                progressBar1.Value = featureProgressCount;

                if (progressBar1.Value >= progressBar1.Maximum)
                    t.Stop();
            }
        }

        private void btnGenICamXMLTest_Click(object sender, EventArgs e)
        {
            ValidateNodeValues();
        }
    }
}