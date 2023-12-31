namespace InterPacketDelayCalculator
{
    partial class InterPacketDelayForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(InterPacketDelayForm));
            this.OKButton = new System.Windows.Forms.Button();
            this.CclButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.MaxErrorUpDown = new System.Windows.Forms.NumericUpDown();
            this.label27 = new System.Windows.Forms.Label();
            this.PacketTransmissionTimeTextBox = new System.Windows.Forms.TextBox();
            this.label28 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label29 = new System.Windows.Forms.Label();
            this.InterPacketDelayTextBoxus = new System.Windows.Forms.TextBox();
            this.label30 = new System.Windows.Forms.Label();
            this.label26 = new System.Windows.Forms.Label();
            this.label24 = new System.Windows.Forms.Label();
            this.MaxTextBox = new System.Windows.Forms.TextBox();
            this.label25 = new System.Windows.Forms.Label();
            this.label22 = new System.Windows.Forms.Label();
            this.MinTextBox = new System.Windows.Forms.TextBox();
            this.label23 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.CurrentInterPacketDelayTextBox = new System.Windows.Forms.TextBox();
            this.label21 = new System.Windows.Forms.Label();
            this.InterPacketDelayTextBox = new System.Windows.Forms.TextBox();
            this.label17 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.TransmissionTimeTextBox = new System.Windows.Forms.TextBox();
            this.label20 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.MaxFpsTextBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.TotalSizeTextBox = new System.Windows.Forms.TextBox();
            this.CalculateButton = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.OverheadTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label13 = new System.Windows.Forms.Label();
            this.TickFrequenceTextBox = new System.Windows.Forms.TextBox();
            this.label14 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.PacketCountTextBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.PacketSizeTextBox = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.PayloadSizeTextBox = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.label31 = new System.Windows.Forms.Label();
            this.MbitPerSecTextBox = new System.Windows.Forms.TextBox();
            this.label32 = new System.Windows.Forms.Label();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.MaxErrorUpDown)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // OKButton
            // 
            this.OKButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.OKButton.Location = new System.Drawing.Point(258, 12);
            this.OKButton.Name = "OKButton";
            this.OKButton.Size = new System.Drawing.Size(75, 23);
            this.OKButton.TabIndex = 41;
            this.OKButton.Text = "OK";
            this.OKButton.UseVisualStyleBackColor = true;
            this.OKButton.Click += new System.EventHandler(this.OKButton_Click);
            // 
            // CclButton
            // 
            this.CclButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.CclButton.Location = new System.Drawing.Point(258, 41);
            this.CclButton.Name = "CclButton";
            this.CclButton.Size = new System.Drawing.Size(75, 23);
            this.CclButton.TabIndex = 42;
            this.CclButton.Text = "Cancel";
            this.CclButton.UseVisualStyleBackColor = true;
            this.CclButton.Click += new System.EventHandler(this.CancelButton_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label32);
            this.groupBox2.Controls.Add(this.MbitPerSecTextBox);
            this.groupBox2.Controls.Add(this.label31);
            this.groupBox2.Controls.Add(this.MaxErrorUpDown);
            this.groupBox2.Controls.Add(this.label27);
            this.groupBox2.Controls.Add(this.PacketTransmissionTimeTextBox);
            this.groupBox2.Controls.Add(this.label28);
            this.groupBox2.Controls.Add(this.groupBox1);
            this.groupBox2.Controls.Add(this.label19);
            this.groupBox2.Controls.Add(this.TransmissionTimeTextBox);
            this.groupBox2.Controls.Add(this.label20);
            this.groupBox2.Controls.Add(this.label15);
            this.groupBox2.Controls.Add(this.label16);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.MaxFpsTextBox);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.TotalSizeTextBox);
            this.groupBox2.Controls.Add(this.CalculateButton);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.OverheadTextBox);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.label13);
            this.groupBox2.Controls.Add(this.TickFrequenceTextBox);
            this.groupBox2.Controls.Add(this.label14);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.PacketCountTextBox);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.PacketSizeTextBox);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.label11);
            this.groupBox2.Controls.Add(this.PayloadSizeTextBox);
            this.groupBox2.Controls.Add(this.label12);
            this.groupBox2.Location = new System.Drawing.Point(12, 7);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(240, 492);
            this.groupBox2.TabIndex = 0;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Info";
            this.toolTip1.SetToolTip(this.groupBox2, "Info");
            // 
            // MaxErrorUpDown
            // 
            this.MaxErrorUpDown.DecimalPlaces = 2;
            this.MaxErrorUpDown.Location = new System.Drawing.Point(82, 254);
            this.MaxErrorUpDown.Name = "MaxErrorUpDown";
            this.MaxErrorUpDown.Size = new System.Drawing.Size(75, 20);
            this.MaxErrorUpDown.TabIndex = 43;
            this.MaxErrorUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.MaxErrorUpDown, "Expected bandwidth usage in percent of a 1 Gbit/s connection");
            this.MaxErrorUpDown.UpDownAlign = System.Windows.Forms.LeftRightAlignment.Left;
            this.MaxErrorUpDown.ValueChanged += new System.EventHandler(this.MaxErrorUpDown_ValueChanged);
            // 
            // label27
            // 
            this.label27.AutoSize = true;
            this.label27.Location = new System.Drawing.Point(158, 205);
            this.label27.Name = "label27";
            this.label27.Size = new System.Drawing.Size(57, 13);
            this.label27.TabIndex = 31;
            this.label27.Text = "μs/Packet";
            this.toolTip1.SetToolTip(this.label27, "Total transmission time per packet");
            // 
            // PacketTransmissionTimeTextBox
            // 
            this.PacketTransmissionTimeTextBox.Location = new System.Drawing.Point(82, 202);
            this.PacketTransmissionTimeTextBox.Name = "PacketTransmissionTimeTextBox";
            this.PacketTransmissionTimeTextBox.ReadOnly = true;
            this.PacketTransmissionTimeTextBox.Size = new System.Drawing.Size(75, 20);
            this.PacketTransmissionTimeTextBox.TabIndex = 30;
            this.PacketTransmissionTimeTextBox.TabStop = false;
            this.PacketTransmissionTimeTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.PacketTransmissionTimeTextBox, "Total transmission time per packet");
            // 
            // label28
            // 
            this.label28.AutoSize = true;
            this.label28.Location = new System.Drawing.Point(8, 205);
            this.label28.Name = "label28";
            this.label28.Size = new System.Drawing.Size(59, 13);
            this.label28.TabIndex = 29;
            this.label28.Text = "Trans. time";
            this.toolTip1.SetToolTip(this.label28, "Total transmission time per packet");
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label29);
            this.groupBox1.Controls.Add(this.InterPacketDelayTextBoxus);
            this.groupBox1.Controls.Add(this.label30);
            this.groupBox1.Controls.Add(this.label26);
            this.groupBox1.Controls.Add(this.label24);
            this.groupBox1.Controls.Add(this.MaxTextBox);
            this.groupBox1.Controls.Add(this.label25);
            this.groupBox1.Controls.Add(this.label22);
            this.groupBox1.Controls.Add(this.MinTextBox);
            this.groupBox1.Controls.Add(this.label23);
            this.groupBox1.Controls.Add(this.label18);
            this.groupBox1.Controls.Add(this.CurrentInterPacketDelayTextBox);
            this.groupBox1.Controls.Add(this.label21);
            this.groupBox1.Controls.Add(this.InterPacketDelayTextBox);
            this.groupBox1.Controls.Add(this.label17);
            this.groupBox1.Location = new System.Drawing.Point(7, 334);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(226, 152);
            this.groupBox1.TabIndex = 28;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Inter Packet Delay";
            this.toolTip1.SetToolTip(this.groupBox1, "Inter Packet Delay values and limits");
            // 
            // label29
            // 
            this.label29.AutoSize = true;
            this.label29.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label29.Location = new System.Drawing.Point(6, 126);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(34, 13);
            this.label29.TabIndex = 41;
            this.label29.Text = "Delay";
            this.toolTip1.SetToolTip(this.label29, "Packet delay in microseconds");
            // 
            // InterPacketDelayTextBoxus
            // 
            this.InterPacketDelayTextBoxus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.InterPacketDelayTextBoxus.Location = new System.Drawing.Point(75, 123);
            this.InterPacketDelayTextBoxus.Name = "InterPacketDelayTextBoxus";
            this.InterPacketDelayTextBoxus.ReadOnly = true;
            this.InterPacketDelayTextBoxus.Size = new System.Drawing.Size(75, 20);
            this.InterPacketDelayTextBoxus.TabIndex = 42;
            this.InterPacketDelayTextBoxus.TabStop = false;
            this.InterPacketDelayTextBoxus.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.InterPacketDelayTextBoxus, "Packet delay in microseconds");
            // 
            // label30
            // 
            this.label30.AutoSize = true;
            this.label30.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label30.Location = new System.Drawing.Point(151, 126);
            this.label30.Name = "label30";
            this.label30.Size = new System.Drawing.Size(18, 13);
            this.label30.TabIndex = 43;
            this.label30.Text = "μs";
            this.toolTip1.SetToolTip(this.label30, "Packet delay in microseconds");
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label26.Location = new System.Drawing.Point(6, 100);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(67, 13);
            this.label26.TabIndex = 38;
            this.label26.Text = "New value";
            this.toolTip1.SetToolTip(this.label26, "New value");
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Location = new System.Drawing.Point(151, 74);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(33, 13);
            this.label24.TabIndex = 37;
            this.label24.Text = "Ticks";
            this.toolTip1.SetToolTip(this.label24, "Maximum value");
            // 
            // MaxTextBox
            // 
            this.MaxTextBox.Location = new System.Drawing.Point(75, 71);
            this.MaxTextBox.Name = "MaxTextBox";
            this.MaxTextBox.ReadOnly = true;
            this.MaxTextBox.Size = new System.Drawing.Size(75, 20);
            this.MaxTextBox.TabIndex = 36;
            this.MaxTextBox.TabStop = false;
            this.MaxTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.MaxTextBox, "Maximum value");
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Location = new System.Drawing.Point(6, 74);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(27, 13);
            this.label25.TabIndex = 35;
            this.label25.Text = "Max";
            this.toolTip1.SetToolTip(this.label25, "Maximum value");
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(151, 48);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(33, 13);
            this.label22.TabIndex = 34;
            this.label22.Text = "Ticks";
            this.toolTip1.SetToolTip(this.label22, "Minimum value");
            // 
            // MinTextBox
            // 
            this.MinTextBox.Location = new System.Drawing.Point(75, 45);
            this.MinTextBox.Name = "MinTextBox";
            this.MinTextBox.ReadOnly = true;
            this.MinTextBox.Size = new System.Drawing.Size(75, 20);
            this.MinTextBox.TabIndex = 33;
            this.MinTextBox.TabStop = false;
            this.MinTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.MinTextBox, "Minimum value");
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Location = new System.Drawing.Point(6, 48);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(24, 13);
            this.label23.TabIndex = 32;
            this.label23.Text = "Min";
            this.toolTip1.SetToolTip(this.label23, "Minimum value");
            this.label23.Click += new System.EventHandler(this.label23_Click);
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(151, 22);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(33, 13);
            this.label18.TabIndex = 31;
            this.label18.Text = "Ticks";
            this.toolTip1.SetToolTip(this.label18, "Current value");
            // 
            // CurrentInterPacketDelayTextBox
            // 
            this.CurrentInterPacketDelayTextBox.Location = new System.Drawing.Point(75, 19);
            this.CurrentInterPacketDelayTextBox.Name = "CurrentInterPacketDelayTextBox";
            this.CurrentInterPacketDelayTextBox.ReadOnly = true;
            this.CurrentInterPacketDelayTextBox.Size = new System.Drawing.Size(75, 20);
            this.CurrentInterPacketDelayTextBox.TabIndex = 30;
            this.CurrentInterPacketDelayTextBox.TabStop = false;
            this.CurrentInterPacketDelayTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.CurrentInterPacketDelayTextBox, "Current value");
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(6, 22);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(41, 13);
            this.label21.TabIndex = 29;
            this.label21.Text = "Current";
            this.toolTip1.SetToolTip(this.label21, "Current value");
            // 
            // InterPacketDelayTextBox
            // 
            this.InterPacketDelayTextBox.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.InterPacketDelayTextBox.Location = new System.Drawing.Point(75, 97);
            this.InterPacketDelayTextBox.Name = "InterPacketDelayTextBox";
            this.InterPacketDelayTextBox.ReadOnly = true;
            this.InterPacketDelayTextBox.Size = new System.Drawing.Size(75, 20);
            this.InterPacketDelayTextBox.TabIndex = 39;
            this.InterPacketDelayTextBox.TabStop = false;
            this.InterPacketDelayTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.InterPacketDelayTextBox, "New value");
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label17.Location = new System.Drawing.Point(151, 100);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(38, 13);
            this.label17.TabIndex = 40;
            this.label17.Text = "Ticks";
            this.toolTip1.SetToolTip(this.label17, "New value");
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Location = new System.Drawing.Point(158, 179);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(54, 13);
            this.label19.TabIndex = 21;
            this.label19.Text = "ms/Image";
            this.toolTip1.SetToolTip(this.label19, "Total transmission time per image");
            // 
            // TransmissionTimeTextBox
            // 
            this.TransmissionTimeTextBox.Location = new System.Drawing.Point(82, 176);
            this.TransmissionTimeTextBox.Name = "TransmissionTimeTextBox";
            this.TransmissionTimeTextBox.ReadOnly = true;
            this.TransmissionTimeTextBox.Size = new System.Drawing.Size(75, 20);
            this.TransmissionTimeTextBox.TabIndex = 20;
            this.TransmissionTimeTextBox.TabStop = false;
            this.TransmissionTimeTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.TransmissionTimeTextBox, "Total transmission time per image");
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(8, 179);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(59, 13);
            this.label20.TabIndex = 19;
            this.label20.Text = "Trans. time";
            this.toolTip1.SetToolTip(this.label20, "Total transmission time per image");
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(158, 257);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(15, 13);
            this.label15.TabIndex = 27;
            this.label15.Text = "%";
            this.toolTip1.SetToolTip(this.label15, "Expected bandwidth usage in percent of a 1 Gbit/s connection");
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(8, 257);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(68, 13);
            this.label16.TabIndex = 25;
            this.label16.Text = "Bandwidth %";
            this.toolTip1.SetToolTip(this.label16, "Expected bandwidth usage in percent of a 1 Gbit/s connection");
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(158, 231);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(49, 13);
            this.label5.TabIndex = 24;
            this.label5.Text = "/Second";
            this.toolTip1.SetToolTip(this.label5, "Expected maximum number of images sent per second. This will typically be the fra" +
                    "me rate when the camera is running continuous");
            // 
            // MaxFpsTextBox
            // 
            this.MaxFpsTextBox.Location = new System.Drawing.Point(82, 228);
            this.MaxFpsTextBox.Name = "MaxFpsTextBox";
            this.MaxFpsTextBox.Size = new System.Drawing.Size(75, 20);
            this.MaxFpsTextBox.TabIndex = 23;
            this.MaxFpsTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.MaxFpsTextBox, "Expected maximum number of images sent per second. This will typically be the fra" +
                    "me rate when the camera is running continuous");
            this.MaxFpsTextBox.TextChanged += new System.EventHandler(this.MaxFpsTextBox_TextChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(8, 231);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(44, 13);
            this.label6.TabIndex = 22;
            this.label6.Text = "Max fps";
            this.toolTip1.SetToolTip(this.label6, "Expected maximum number of images sent per second. This will typically be the fra" +
                    "me rate when the camera is running continuous");
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(158, 153);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(67, 13);
            this.label3.TabIndex = 18;
            this.label3.Text = "Bytes/Image";
            this.toolTip1.SetToolTip(this.label3, "Total number of bytes sent over the ethernet per image");
            // 
            // TotalSizeTextBox
            // 
            this.TotalSizeTextBox.Location = new System.Drawing.Point(82, 150);
            this.TotalSizeTextBox.Name = "TotalSizeTextBox";
            this.TotalSizeTextBox.ReadOnly = true;
            this.TotalSizeTextBox.Size = new System.Drawing.Size(75, 20);
            this.TotalSizeTextBox.TabIndex = 17;
            this.TotalSizeTextBox.TabStop = false;
            this.TotalSizeTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.TotalSizeTextBox, "Total number of bytes sent over the ethernet per image");
            // 
            // CalculateButton
            // 
            this.CalculateButton.Enabled = false;
            this.CalculateButton.Location = new System.Drawing.Point(82, 306);
            this.CalculateButton.Name = "CalculateButton";
            this.CalculateButton.Size = new System.Drawing.Size(75, 23);
            this.CalculateButton.TabIndex = 18;
            this.CalculateButton.Text = "Calculate";
            this.toolTip1.SetToolTip(this.CalculateButton, "Calculate the new value for the Inter Packet Delay");
            this.CalculateButton.UseVisualStyleBackColor = true;
            this.CalculateButton.Click += new System.EventHandler(this.CalculateButton_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(8, 153);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(54, 13);
            this.label4.TabIndex = 16;
            this.label4.Text = "Total Size";
            this.toolTip1.SetToolTip(this.label4, "Total number of bytes sent over the ethernet per image");
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(158, 127);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(67, 13);
            this.label1.TabIndex = 15;
            this.label1.Text = "Bytes/Image";
            this.toolTip1.SetToolTip(this.label1, "Number of additional bytes sent over the ethernet on top of the Payload Size");
            // 
            // OverheadTextBox
            // 
            this.OverheadTextBox.Location = new System.Drawing.Point(82, 124);
            this.OverheadTextBox.Name = "OverheadTextBox";
            this.OverheadTextBox.ReadOnly = true;
            this.OverheadTextBox.Size = new System.Drawing.Size(75, 20);
            this.OverheadTextBox.TabIndex = 14;
            this.OverheadTextBox.TabStop = false;
            this.OverheadTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.OverheadTextBox, "Number of additional bytes sent over the ethernet on top of the Payload Size");
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 127);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(54, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "Overhead";
            this.toolTip1.SetToolTip(this.label2, "Number of additional bytes sent over the ethernet on top of the Payload Size");
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(158, 101);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(75, 13);
            this.label13.TabIndex = 12;
            this.label13.Text = "Ticks/Second";
            this.toolTip1.SetToolTip(this.label13, "Number of timestamp-ticks during 1 second. This is used as the base frequency for" +
                    " the Inter Packet Delay");
            // 
            // TickFrequenceTextBox
            // 
            this.TickFrequenceTextBox.Location = new System.Drawing.Point(82, 98);
            this.TickFrequenceTextBox.Name = "TickFrequenceTextBox";
            this.TickFrequenceTextBox.ReadOnly = true;
            this.TickFrequenceTextBox.Size = new System.Drawing.Size(75, 20);
            this.TickFrequenceTextBox.TabIndex = 11;
            this.TickFrequenceTextBox.TabStop = false;
            this.TickFrequenceTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.TickFrequenceTextBox, "Number of timestamp-ticks during 1 second. This is used as the base frequency for" +
                    " the Inter Packet Delay");
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(8, 101);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(52, 13);
            this.label14.TabIndex = 10;
            this.label14.Text = "Tick freq.";
            this.toolTip1.SetToolTip(this.label14, "Number of timestamp-ticks during 1 second. This is used as the base frequency for" +
                    " the Inter Packet Delay");
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(158, 75);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(41, 13);
            this.label7.TabIndex = 9;
            this.label7.Text = "/Image";
            this.toolTip1.SetToolTip(this.label7, "Number of packets used for transferring a single image");
            // 
            // PacketCountTextBox
            // 
            this.PacketCountTextBox.Location = new System.Drawing.Point(82, 72);
            this.PacketCountTextBox.Name = "PacketCountTextBox";
            this.PacketCountTextBox.ReadOnly = true;
            this.PacketCountTextBox.Size = new System.Drawing.Size(75, 20);
            this.PacketCountTextBox.TabIndex = 8;
            this.PacketCountTextBox.TabStop = false;
            this.PacketCountTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.PacketCountTextBox, "Number of packets used for transferring a single image");
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(8, 75);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(68, 13);
            this.label8.TabIndex = 7;
            this.label8.Text = "# of Packets";
            this.toolTip1.SetToolTip(this.label8, "Number of packets used for transferring a single image");
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(158, 49);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(33, 13);
            this.label9.TabIndex = 6;
            this.label9.Text = "Bytes";
            this.toolTip1.SetToolTip(this.label9, "Packet size (IP Header + UDP Header + GVSP Header + Payload)");
            // 
            // PacketSizeTextBox
            // 
            this.PacketSizeTextBox.Location = new System.Drawing.Point(82, 46);
            this.PacketSizeTextBox.Name = "PacketSizeTextBox";
            this.PacketSizeTextBox.ReadOnly = true;
            this.PacketSizeTextBox.Size = new System.Drawing.Size(75, 20);
            this.PacketSizeTextBox.TabIndex = 5;
            this.PacketSizeTextBox.TabStop = false;
            this.PacketSizeTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.PacketSizeTextBox, "Packet size (IP Header + UDP Header + GVSP Header + Payload)");
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(8, 49);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(64, 13);
            this.label10.TabIndex = 4;
            this.label10.Text = "Packet Size";
            this.toolTip1.SetToolTip(this.label10, "Packet size (IP Header + UDP Header + GVSP Header + Payload)");
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(158, 23);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(33, 13);
            this.label11.TabIndex = 3;
            this.label11.Text = "Bytes";
            this.toolTip1.SetToolTip(this.label11, "Raw image size in bytes");
            // 
            // PayloadSizeTextBox
            // 
            this.PayloadSizeTextBox.Location = new System.Drawing.Point(82, 20);
            this.PayloadSizeTextBox.Name = "PayloadSizeTextBox";
            this.PayloadSizeTextBox.ReadOnly = true;
            this.PayloadSizeTextBox.Size = new System.Drawing.Size(75, 20);
            this.PayloadSizeTextBox.TabIndex = 2;
            this.PayloadSizeTextBox.TabStop = false;
            this.PayloadSizeTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.PayloadSizeTextBox, "Raw image size in bytes");
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(8, 23);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(68, 13);
            this.label12.TabIndex = 1;
            this.label12.Text = "Payload Size";
            this.toolTip1.SetToolTip(this.label12, "Raw image size in bytes");
            // 
            // label31
            // 
            this.label31.AutoSize = true;
            this.label31.Location = new System.Drawing.Point(8, 283);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(57, 13);
            this.label31.TabIndex = 44;
            this.label31.Text = "Bandwidth";
            this.toolTip1.SetToolTip(this.label31, "Total bandwidth utilization (in Mbit/s)");
            // 
            // MbitPerSecTextBox
            // 
            this.MbitPerSecTextBox.Location = new System.Drawing.Point(82, 280);
            this.MbitPerSecTextBox.Name = "MbitPerSecTextBox";
            this.MbitPerSecTextBox.ReadOnly = true;
            this.MbitPerSecTextBox.Size = new System.Drawing.Size(75, 20);
            this.MbitPerSecTextBox.TabIndex = 45;
            this.MbitPerSecTextBox.TabStop = false;
            this.MbitPerSecTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.toolTip1.SetToolTip(this.MbitPerSecTextBox, "Total bandwidth utilization (in Mbit/s)");
            // 
            // label32
            // 
            this.label32.AutoSize = true;
            this.label32.Location = new System.Drawing.Point(158, 283);
            this.label32.Name = "label32";
            this.label32.Size = new System.Drawing.Size(37, 13);
            this.label32.TabIndex = 43;
            this.label32.Text = "Mbit/s";
            this.toolTip1.SetToolTip(this.label32, "Total bandwidth utilization (in Mbit/s)");
            // 
            // InterPacketDelayForm
            // 
            this.AcceptButton = this.CalculateButton;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(340, 509);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.CclButton);
            this.Controls.Add(this.OKButton);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "InterPacketDelayForm";
            this.Text = "Inter Packet Delay Calculation";
            this.Load += new System.EventHandler(this.InterPacketDelayForm_Load);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.MaxErrorUpDown)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button OKButton;
        private System.Windows.Forms.Button CclButton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TextBox TickFrequenceTextBox;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox PacketCountTextBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox PacketSizeTextBox;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox PayloadSizeTextBox;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Button CalculateButton;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox TotalSizeTextBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox OverheadTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.TextBox InterPacketDelayTextBox;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.TextBox TransmissionTimeTextBox;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.TextBox MaxTextBox;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.TextBox MinTextBox;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.TextBox CurrentInterPacketDelayTextBox;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.Label label27;
        private System.Windows.Forms.TextBox PacketTransmissionTimeTextBox;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.Label label29;
        private System.Windows.Forms.TextBox InterPacketDelayTextBoxus;
        private System.Windows.Forms.Label label30;
        private System.Windows.Forms.NumericUpDown MaxErrorUpDown;
        private System.Windows.Forms.TextBox MaxFpsTextBox;
        private System.Windows.Forms.Label label31;
        private System.Windows.Forms.Label label32;
        private System.Windows.Forms.TextBox MbitPerSecTextBox;
    }
}