using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;
using System.Net;
using Microsoft.Win32;

namespace ActionCommandSample
{
    public partial class ActionCommandForm : Form
    {
        CFactory myFactory;
        UInt32 myDeviceKey = 1;
        UInt32 myGroupKey = 1;
        UInt32 myGroupMask = 1;
        bool   myRequestAcknowledge = true;
        UInt32 myAckCount = 1;

        public ActionCommandForm(CFactory factory)
        {
            InitializeComponent();

            myFactory = factory;

            // We want to restore the last address from the registry
            GetLastActionData();
        }

        public void GetLastActionData()
        {
            RegistryKey key = Registry.CurrentUser.OpenSubKey(Application.ProductName + "\\" + this.Name);

            if (key != null)
            {
                object deviceKeyValue = key.GetValue("ActionCommandDeviceKey");
                if (deviceKeyValue != null)
                {
                    myDeviceKey = Convert.ToUInt32(deviceKeyValue);
                    deviceKeyTextBox.Text = "0x" + myDeviceKey.ToString("X8");
                }

                object groupKeyValue = key.GetValue("ActionCommandGroupKey");
                if (groupKeyValue != null)
                {
                    myGroupKey = Convert.ToUInt32(groupKeyValue);
                    groupKeyTextBox.Text = "0x" + myGroupKey.ToString("X8");
                }

                object groupMaskValue = key.GetValue("ActionCommandGroupMask");
                if (groupMaskValue != null)
                {
                    myGroupMask = Convert.ToUInt32(groupMaskValue);
                    groupMaskTextBox.Text = "0x" + myGroupMask.ToString("X8");
                }

                object acknowledgeCountValue = key.GetValue("ActionCommandAckCount");
                if (acknowledgeCountValue != null)
                {
                    myAckCount = Convert.ToUInt32(acknowledgeCountValue);
                }

                object requestAckValue = key.GetValue("ActionCommandRequestAck");
                if (requestAckValue != null)
                {
                    if (Convert.ToUInt32(groupMaskValue) == 0)
                        myRequestAcknowledge = false;
                    else
                        myRequestAcknowledge = true;
                }

                acknowledgeCountNumericUpDown.Value = Convert.ToDecimal(myAckCount);
                requestAcknowledgeCheckBox.Checked = myRequestAcknowledge;

                acknowledgeCountNumericUpDown.Enabled = myRequestAcknowledge;
                acknowledgeListBox.Enabled = myRequestAcknowledge;
                expectedAcknowledeCountLabel.Enabled = myRequestAcknowledge;
            }
        }

        private void SaveCurrentActionCommandData()
        {
            try
            {
                RegistryKey key = Registry.CurrentUser.CreateSubKey(Application.ProductName + "\\" + this.Name);
                if (key != null)
                {
                    key.SetValue("ActionCommandDeviceKey", myDeviceKey);
                    key.SetValue("ActionCommandGroupKey", myGroupKey);
                    key.SetValue("ActionCommandGroupMask", myGroupMask);

                    if (myRequestAcknowledge)
                        key.SetValue("ActionCommandRequestAck", 1);
                    else
                        key.SetValue("ActionCommandRequestAck", 0);

                    key.SetValue("ActionCommandAckCount", myAckCount);
                }
            }
            catch (Exception e)
            {
                System.Diagnostics.Debug.WriteLine(e.Message);
            }
        }

        private void sendActionCommandButton_Click(object sender, EventArgs e)
        {
            // Parse the values from the Text Boxes
            string valueString = deviceKeyTextBox.Text.ToUpper();

            // Is this a hex number?
            if (valueString.Contains("0X"))
            {
                valueString = valueString.Substring(valueString.IndexOf("0X") + 2);
                myDeviceKey = UInt32.Parse(valueString, System.Globalization.NumberStyles.HexNumber);
            }
            else // Assume decimal number
            {
                try
                {
                    myDeviceKey = UInt32.Parse(valueString);
                }
                catch (Exception ex)
                {
                    // We could not parse the address as decimal.
                    System.Diagnostics.Debug.WriteLine(ex.Message);
                    myDeviceKey = 0;
                }
            }

            deviceKeyTextBox.Text = "0x" + myDeviceKey.ToString("X8");

            valueString = groupKeyTextBox.Text.ToUpper();

            // Is this a hex number?
            if (valueString.Contains("0X"))
            {
                valueString = valueString.Substring(valueString.IndexOf("0X") + 2);
                myGroupKey = UInt32.Parse(valueString, System.Globalization.NumberStyles.HexNumber);
            }
            else // Assume decimal number
            {
                try
                {
                    myGroupKey = UInt32.Parse(valueString);
                }
                catch (Exception ex)
                {
                    // We could not parse the address as decimal.
                    System.Diagnostics.Debug.WriteLine(ex.Message);
                    myGroupKey = 0;
                }
            }

            groupKeyTextBox.Text = "0x" + myGroupKey.ToString("X8");

            valueString = groupMaskTextBox.Text.ToUpper();

            // Is this a hex number?
            if (valueString.Contains("0X"))
            {
                valueString = valueString.Substring(valueString.IndexOf("0X") + 2);
                myGroupMask = UInt32.Parse(valueString, System.Globalization.NumberStyles.HexNumber);
            }
            else // Assume decimal number
            {
                try
                {
                    myGroupMask = UInt32.Parse(valueString);
                }
                catch (Exception ex)
                {
                    // We could not parse the address as decimal.
                    System.Diagnostics.Debug.WriteLine(ex.Message);
                    myGroupMask = 0;
                }
            }

            groupMaskTextBox.Text = "0x" + myGroupMask.ToString("X8");

            // Persist the last Data to the registry
            SaveCurrentActionCommandData();

            Jai_FactoryWrapper.EFactoryError error = Jai_FactoryWrapper.EFactoryError.Success;

            // Execute the Action Command
            if (myRequestAcknowledge)
            {
                acknowledgeListBox.Items.Clear();

                Jai_FactoryWrapper.ActionCommandAck[] AckBufferArray = null;

                error = myFactory.SendActionCommand(myDeviceKey, myGroupKey, myGroupMask, ref AckBufferArray, myAckCount);

                if ((AckBufferArray != null) && (AckBufferArray.Length > 0))
                {
                    for (int i = 0; i < AckBufferArray.Length; i++)
                    {
                        IPAddress myIPAddress = new IPAddress(AckBufferArray[i].IPAddress);
                        System.Diagnostics.Debug.WriteLine("Got acknowledge from Device " + myIPAddress.ToString());
                        acknowledgeListBox.Items.Add((i+1).ToString() + ") " + myIPAddress.ToString());
                    }
                }
                else
                {
                    acknowledgeListBox.Items.Add("No acknowledge");
                }
            }
            else
            {
                error = myFactory.SendActionCommand(myDeviceKey, myGroupKey, myGroupMask);
            }

            if (error != Jai_FactoryWrapper.EFactoryError.Success)
                MessageBox.Show(this, "Error executing Action Command!", Jai_FactoryWrapper.GetFactoryErrorString(error), MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void requestAcknowledgeCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            myRequestAcknowledge = requestAcknowledgeCheckBox.Checked;

            acknowledgeCountNumericUpDown.Enabled = myRequestAcknowledge;
            acknowledgeListBox.Enabled = myRequestAcknowledge;
            expectedAcknowledeCountLabel.Enabled = myRequestAcknowledge;

            // Persist the last Data to the registry
            SaveCurrentActionCommandData();
        }

        private void acknowledgeCountNumericUpDown_ValueChanged(object sender, EventArgs e)
        {
            myAckCount = Convert.ToUInt32(acknowledgeCountNumericUpDown.Value);

            // Persist the last Data to the registry
            SaveCurrentActionCommandData();
        }
    }
}