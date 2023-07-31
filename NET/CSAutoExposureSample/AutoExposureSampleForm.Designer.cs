namespace CSAutoExposureSample
{
    partial class AutoExposureSampleForm
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.SearchButton = new System.Windows.Forms.Button();
            this.CameraIDTextBox = new System.Windows.Forms.TextBox();
            this.StopButton = new System.Windows.Forms.Button();
            this.StartButton = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.exposureTextBox = new System.Windows.Forms.TextBox();
            this.exposureTrackBar = new System.Windows.Forms.TrackBar();
            this.label4 = new System.Windows.Forms.Label();
            this.gainTextBox = new System.Windows.Forms.TextBox();
            this.gainTrackBar = new System.Windows.Forms.TrackBar();
            this.label3 = new System.Windows.Forms.Label();
            this.heightTextBox = new System.Windows.Forms.TextBox();
            this.heightTrackBar = new System.Windows.Forms.TrackBar();
            this.label2 = new System.Windows.Forms.Label();
            this.widthTextBox = new System.Windows.Forms.TextBox();
            this.widthTrackBar = new System.Windows.Forms.TrackBar();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.roiheightNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.roiwidthNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.yposNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.xposNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.label17 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.maxExposureNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.maxGainNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.minExposureNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.minGainNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.label13 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.averageBTextBox = new System.Windows.Forms.TextBox();
            this.averageGTextBox = new System.Windows.Forms.TextBox();
            this.averageRTextBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.setpointTextBox = new System.Windows.Forms.TextBox();
            this.setpointTrackBar = new System.Windows.Forms.TrackBar();
            this.label5 = new System.Windows.Forms.Label();
            this.ALCTypeComboBox = new System.Windows.Forms.ComboBox();
            this.enableALCcheckBox = new System.Windows.Forms.CheckBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.exposureTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gainTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.heightTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.widthTrackBar)).BeginInit();
            this.groupBox3.SuspendLayout();
            this.groupBox5.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.roiheightNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.roiwidthNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.yposNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.xposNumericUpDown)).BeginInit();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.maxExposureNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.maxGainNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.minExposureNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.minGainNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.setpointTrackBar)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.SearchButton);
            this.groupBox1.Controls.Add(this.CameraIDTextBox);
            this.groupBox1.Location = new System.Drawing.Point(7, 8);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(523, 71);
            this.groupBox1.TabIndex = 7;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "ID of the first camera found";
            // 
            // SearchButton
            // 
            this.SearchButton.Location = new System.Drawing.Point(488, 17);
            this.SearchButton.Name = "SearchButton";
            this.SearchButton.Size = new System.Drawing.Size(29, 23);
            this.SearchButton.TabIndex = 6;
            this.SearchButton.Text = "...";
            this.SearchButton.UseVisualStyleBackColor = true;
            this.SearchButton.Click += new System.EventHandler(this.SearchButton_Click);
            // 
            // CameraIDTextBox
            // 
            this.CameraIDTextBox.Enabled = false;
            this.CameraIDTextBox.Location = new System.Drawing.Point(6, 19);
            this.CameraIDTextBox.Multiline = true;
            this.CameraIDTextBox.Name = "CameraIDTextBox";
            this.CameraIDTextBox.Size = new System.Drawing.Size(476, 45);
            this.CameraIDTextBox.TabIndex = 0;
            // 
            // StopButton
            // 
            this.StopButton.Enabled = false;
            this.StopButton.Location = new System.Drawing.Point(449, 114);
            this.StopButton.Name = "StopButton";
            this.StopButton.Size = new System.Drawing.Size(75, 23);
            this.StopButton.TabIndex = 10;
            this.StopButton.Text = "Stop";
            this.StopButton.UseVisualStyleBackColor = true;
            this.StopButton.Click += new System.EventHandler(this.StopButton_Click);
            // 
            // StartButton
            // 
            this.StartButton.Enabled = false;
            this.StartButton.Location = new System.Drawing.Point(449, 85);
            this.StartButton.Name = "StartButton";
            this.StartButton.Size = new System.Drawing.Size(75, 23);
            this.StartButton.TabIndex = 9;
            this.StartButton.Text = "Start";
            this.StartButton.UseVisualStyleBackColor = true;
            this.StartButton.Click += new System.EventHandler(this.StartButton_Click);
            // 
            // timer1
            // 
            this.timer1.Interval = 10;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.exposureTextBox);
            this.groupBox2.Controls.Add(this.exposureTrackBar);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.gainTextBox);
            this.groupBox2.Controls.Add(this.gainTrackBar);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.heightTextBox);
            this.groupBox2.Controls.Add(this.heightTrackBar);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.widthTextBox);
            this.groupBox2.Controls.Add(this.widthTrackBar);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Location = new System.Drawing.Point(7, 86);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(436, 205);
            this.groupBox2.TabIndex = 11;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Camera Settings";
            // 
            // exposureTextBox
            // 
            this.exposureTextBox.Location = new System.Drawing.Point(380, 169);
            this.exposureTextBox.Name = "exposureTextBox";
            this.exposureTextBox.ReadOnly = true;
            this.exposureTextBox.Size = new System.Drawing.Size(50, 20);
            this.exposureTextBox.TabIndex = 11;
            this.exposureTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // exposureTrackBar
            // 
            this.exposureTrackBar.Location = new System.Drawing.Point(64, 157);
            this.exposureTrackBar.Name = "exposureTrackBar";
            this.exposureTrackBar.Size = new System.Drawing.Size(310, 45);
            this.exposureTrackBar.TabIndex = 10;
            this.exposureTrackBar.TickStyle = System.Windows.Forms.TickStyle.Both;
            this.exposureTrackBar.Scroll += new System.EventHandler(this.exposureTrackBar_Scroll);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 172);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(54, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "Exposure:";
            // 
            // gainTextBox
            // 
            this.gainTextBox.Location = new System.Drawing.Point(380, 123);
            this.gainTextBox.Name = "gainTextBox";
            this.gainTextBox.ReadOnly = true;
            this.gainTextBox.Size = new System.Drawing.Size(50, 20);
            this.gainTextBox.TabIndex = 8;
            this.gainTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // gainTrackBar
            // 
            this.gainTrackBar.Location = new System.Drawing.Point(64, 111);
            this.gainTrackBar.Name = "gainTrackBar";
            this.gainTrackBar.Size = new System.Drawing.Size(310, 45);
            this.gainTrackBar.TabIndex = 7;
            this.gainTrackBar.TickStyle = System.Windows.Forms.TickStyle.Both;
            this.gainTrackBar.Scroll += new System.EventHandler(this.gainTrackBar_Scroll);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 126);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(32, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Gain:";
            // 
            // heightTextBox
            // 
            this.heightTextBox.Location = new System.Drawing.Point(380, 76);
            this.heightTextBox.Name = "heightTextBox";
            this.heightTextBox.ReadOnly = true;
            this.heightTextBox.Size = new System.Drawing.Size(50, 20);
            this.heightTextBox.TabIndex = 5;
            this.heightTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // heightTrackBar
            // 
            this.heightTrackBar.Location = new System.Drawing.Point(64, 65);
            this.heightTrackBar.Name = "heightTrackBar";
            this.heightTrackBar.Size = new System.Drawing.Size(310, 45);
            this.heightTrackBar.TabIndex = 4;
            this.heightTrackBar.TickStyle = System.Windows.Forms.TickStyle.Both;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 79);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(41, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Height:";
            // 
            // widthTextBox
            // 
            this.widthTextBox.Location = new System.Drawing.Point(380, 30);
            this.widthTextBox.Name = "widthTextBox";
            this.widthTextBox.ReadOnly = true;
            this.widthTextBox.Size = new System.Drawing.Size(50, 20);
            this.widthTextBox.TabIndex = 2;
            this.widthTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // widthTrackBar
            // 
            this.widthTrackBar.Location = new System.Drawing.Point(64, 19);
            this.widthTrackBar.Name = "widthTrackBar";
            this.widthTrackBar.Size = new System.Drawing.Size(310, 45);
            this.widthTrackBar.TabIndex = 1;
            this.widthTrackBar.TickStyle = System.Windows.Forms.TickStyle.Both;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 33);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(38, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Width:";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.groupBox5);
            this.groupBox3.Controls.Add(this.groupBox4);
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.averageBTextBox);
            this.groupBox3.Controls.Add(this.averageGTextBox);
            this.groupBox3.Controls.Add(this.averageRTextBox);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.setpointTextBox);
            this.groupBox3.Controls.Add(this.setpointTrackBar);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Controls.Add(this.ALCTypeComboBox);
            this.groupBox3.Controls.Add(this.enableALCcheckBox);
            this.groupBox3.Location = new System.Drawing.Point(7, 293);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(523, 190);
            this.groupBox3.TabIndex = 12;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Automatic Level Control Settings";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.roiheightNumericUpDown);
            this.groupBox5.Controls.Add(this.roiwidthNumericUpDown);
            this.groupBox5.Controls.Add(this.yposNumericUpDown);
            this.groupBox5.Controls.Add(this.xposNumericUpDown);
            this.groupBox5.Controls.Add(this.label17);
            this.groupBox5.Controls.Add(this.label16);
            this.groupBox5.Controls.Add(this.label15);
            this.groupBox5.Controls.Add(this.label14);
            this.groupBox5.Location = new System.Drawing.Point(303, 103);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(213, 78);
            this.groupBox5.TabIndex = 22;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Measurement ROI";
            // 
            // roiheightNumericUpDown
            // 
            this.roiheightNumericUpDown.Location = new System.Drawing.Point(147, 49);
            this.roiheightNumericUpDown.Name = "roiheightNumericUpDown";
            this.roiheightNumericUpDown.ReadOnly = true;
            this.roiheightNumericUpDown.Size = new System.Drawing.Size(60, 20);
            this.roiheightNumericUpDown.TabIndex = 8;
            this.roiheightNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.roiheightNumericUpDown.ValueChanged += new System.EventHandler(this.roiheightNumericUpDown_ValueChanged);
            // 
            // roiwidthNumericUpDown
            // 
            this.roiwidthNumericUpDown.Location = new System.Drawing.Point(147, 19);
            this.roiwidthNumericUpDown.Name = "roiwidthNumericUpDown";
            this.roiwidthNumericUpDown.ReadOnly = true;
            this.roiwidthNumericUpDown.Size = new System.Drawing.Size(60, 20);
            this.roiwidthNumericUpDown.TabIndex = 7;
            this.roiwidthNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.roiwidthNumericUpDown.ValueChanged += new System.EventHandler(this.roiwidthNumericUpDown_ValueChanged);
            // 
            // yposNumericUpDown
            // 
            this.yposNumericUpDown.Location = new System.Drawing.Point(41, 49);
            this.yposNumericUpDown.Name = "yposNumericUpDown";
            this.yposNumericUpDown.ReadOnly = true;
            this.yposNumericUpDown.Size = new System.Drawing.Size(60, 20);
            this.yposNumericUpDown.TabIndex = 6;
            this.yposNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.yposNumericUpDown.ValueChanged += new System.EventHandler(this.yposNumericUpDown_ValueChanged);
            // 
            // xposNumericUpDown
            // 
            this.xposNumericUpDown.Location = new System.Drawing.Point(41, 19);
            this.xposNumericUpDown.Name = "xposNumericUpDown";
            this.xposNumericUpDown.ReadOnly = true;
            this.xposNumericUpDown.Size = new System.Drawing.Size(60, 20);
            this.xposNumericUpDown.TabIndex = 5;
            this.xposNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.xposNumericUpDown.ValueChanged += new System.EventHandler(this.xposNumericUpDown_ValueChanged);
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(105, 51);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(41, 13);
            this.label17.TabIndex = 3;
            this.label17.Text = "Height:";
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(105, 21);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(38, 13);
            this.label16.TabIndex = 2;
            this.label16.Text = "Width:";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(4, 51);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(37, 13);
            this.label15.TabIndex = 1;
            this.label15.Text = "Y-pos:";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(4, 21);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(37, 13);
            this.label14.TabIndex = 0;
            this.label14.Text = "X-pos:";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.maxExposureNumericUpDown);
            this.groupBox4.Controls.Add(this.maxGainNumericUpDown);
            this.groupBox4.Controls.Add(this.minExposureNumericUpDown);
            this.groupBox4.Controls.Add(this.minGainNumericUpDown);
            this.groupBox4.Controls.Add(this.label13);
            this.groupBox4.Controls.Add(this.label12);
            this.groupBox4.Controls.Add(this.label11);
            this.groupBox4.Controls.Add(this.label10);
            this.groupBox4.Location = new System.Drawing.Point(6, 103);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(290, 79);
            this.groupBox4.TabIndex = 21;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Gain- and Exposure Time limits";
            // 
            // maxExposureNumericUpDown
            // 
            this.maxExposureNumericUpDown.Location = new System.Drawing.Point(220, 49);
            this.maxExposureNumericUpDown.Name = "maxExposureNumericUpDown";
            this.maxExposureNumericUpDown.ReadOnly = true;
            this.maxExposureNumericUpDown.Size = new System.Drawing.Size(60, 20);
            this.maxExposureNumericUpDown.TabIndex = 7;
            this.maxExposureNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.maxExposureNumericUpDown.ValueChanged += new System.EventHandler(this.maxExposureNumericUpDown_ValueChanged);
            // 
            // maxGainNumericUpDown
            // 
            this.maxGainNumericUpDown.Location = new System.Drawing.Point(220, 19);
            this.maxGainNumericUpDown.Name = "maxGainNumericUpDown";
            this.maxGainNumericUpDown.ReadOnly = true;
            this.maxGainNumericUpDown.Size = new System.Drawing.Size(60, 20);
            this.maxGainNumericUpDown.TabIndex = 6;
            this.maxGainNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.maxGainNumericUpDown.ValueChanged += new System.EventHandler(this.maxGainNumericUpDown_ValueChanged);
            // 
            // minExposureNumericUpDown
            // 
            this.minExposureNumericUpDown.Location = new System.Drawing.Point(79, 49);
            this.minExposureNumericUpDown.Name = "minExposureNumericUpDown";
            this.minExposureNumericUpDown.ReadOnly = true;
            this.minExposureNumericUpDown.Size = new System.Drawing.Size(60, 20);
            this.minExposureNumericUpDown.TabIndex = 5;
            this.minExposureNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.minExposureNumericUpDown.ValueChanged += new System.EventHandler(this.minExposureNumericUpDown_ValueChanged);
            // 
            // minGainNumericUpDown
            // 
            this.minGainNumericUpDown.Location = new System.Drawing.Point(79, 19);
            this.minGainNumericUpDown.Name = "minGainNumericUpDown";
            this.minGainNumericUpDown.ReadOnly = true;
            this.minGainNumericUpDown.Size = new System.Drawing.Size(60, 20);
            this.minGainNumericUpDown.TabIndex = 4;
            this.minGainNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.minGainNumericUpDown.ValueChanged += new System.EventHandler(this.minGainNumericUpDown_ValueChanged);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(142, 51);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(77, 13);
            this.label13.TabIndex = 3;
            this.label13.Text = "Max Exposure:";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(4, 51);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(74, 13);
            this.label12.TabIndex = 2;
            this.label12.Text = "Min Exposure:";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(142, 21);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(55, 13);
            this.label11.TabIndex = 1;
            this.label11.Text = "Max Gain:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(4, 21);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(52, 13);
            this.label10.TabIndex = 0;
            this.label10.Text = "Min Gain:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(437, 37);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(17, 13);
            this.label9.TabIndex = 20;
            this.label9.Text = "B:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(362, 37);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(18, 13);
            this.label8.TabIndex = 19;
            this.label8.Text = "G:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(286, 37);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(18, 13);
            this.label7.TabIndex = 18;
            this.label7.Text = "R:";
            // 
            // averageBTextBox
            // 
            this.averageBTextBox.Location = new System.Drawing.Point(455, 34);
            this.averageBTextBox.Name = "averageBTextBox";
            this.averageBTextBox.ReadOnly = true;
            this.averageBTextBox.Size = new System.Drawing.Size(50, 20);
            this.averageBTextBox.TabIndex = 17;
            this.averageBTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // averageGTextBox
            // 
            this.averageGTextBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.averageGTextBox.Location = new System.Drawing.Point(380, 34);
            this.averageGTextBox.Name = "averageGTextBox";
            this.averageGTextBox.ReadOnly = true;
            this.averageGTextBox.Size = new System.Drawing.Size(50, 20);
            this.averageGTextBox.TabIndex = 16;
            this.averageGTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // averageRTextBox
            // 
            this.averageRTextBox.Location = new System.Drawing.Point(305, 34);
            this.averageRTextBox.Name = "averageRTextBox";
            this.averageRTextBox.ReadOnly = true;
            this.averageRTextBox.Size = new System.Drawing.Size(50, 20);
            this.averageRTextBox.TabIndex = 15;
            this.averageRTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(232, 37);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(50, 13);
            this.label6.TabIndex = 14;
            this.label6.Text = "Average:";
            // 
            // setpointTextBox
            // 
            this.setpointTextBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.setpointTextBox.Location = new System.Drawing.Point(380, 69);
            this.setpointTextBox.Name = "setpointTextBox";
            this.setpointTextBox.ReadOnly = true;
            this.setpointTextBox.Size = new System.Drawing.Size(50, 20);
            this.setpointTextBox.TabIndex = 13;
            this.setpointTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // setpointTrackBar
            // 
            this.setpointTrackBar.Location = new System.Drawing.Point(64, 57);
            this.setpointTrackBar.Name = "setpointTrackBar";
            this.setpointTrackBar.Size = new System.Drawing.Size(310, 45);
            this.setpointTrackBar.TabIndex = 11;
            this.setpointTrackBar.TickStyle = System.Windows.Forms.TickStyle.Both;
            this.setpointTrackBar.Scroll += new System.EventHandler(this.setpointTrackBar_Scroll);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(6, 72);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(49, 13);
            this.label5.TabIndex = 2;
            this.label5.Text = "Setpoint:";
            // 
            // ALCTypeComboBox
            // 
            this.ALCTypeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.ALCTypeComboBox.FormattingEnabled = true;
            this.ALCTypeComboBox.Items.AddRange(new object[] {
            "Minimum Gain Priority",
            "Minimum Exposure Priority",
            "Gain Priority",
            "Exposure Priority",
            "Gain Only",
            "Exposure Only"});
            this.ALCTypeComboBox.Location = new System.Drawing.Point(6, 33);
            this.ALCTypeComboBox.Name = "ALCTypeComboBox";
            this.ALCTypeComboBox.Size = new System.Drawing.Size(190, 21);
            this.ALCTypeComboBox.TabIndex = 1;
            this.ALCTypeComboBox.SelectedIndexChanged += new System.EventHandler(this.ALCTypeComboBox_SelectedIndexChanged);
            // 
            // enableALCcheckBox
            // 
            this.enableALCcheckBox.AutoSize = true;
            this.enableALCcheckBox.Location = new System.Drawing.Point(6, 15);
            this.enableALCcheckBox.Name = "enableALCcheckBox";
            this.enableALCcheckBox.Size = new System.Drawing.Size(82, 17);
            this.enableALCcheckBox.TabIndex = 0;
            this.enableALCcheckBox.Text = "Enable ALC";
            this.enableALCcheckBox.UseVisualStyleBackColor = true;
            this.enableALCcheckBox.CheckedChanged += new System.EventHandler(this.enableALCcheckBox_CheckedChanged);
            // 
            // AutoExposureSampleForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(538, 489);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.StartButton);
            this.Controls.Add(this.groupBox1);
            this.Name = "AutoExposureSampleForm";
            this.Text = "Auto Exposure Sample";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.AutoExposureSampleForm_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.exposureTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gainTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.heightTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.widthTrackBar)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.roiheightNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.roiwidthNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.yposNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.xposNumericUpDown)).EndInit();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.maxExposureNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.maxGainNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.minExposureNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.minGainNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.setpointTrackBar)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button SearchButton;
        private System.Windows.Forms.TextBox CameraIDTextBox;
        private System.Windows.Forms.Button StopButton;
        private System.Windows.Forms.Button StartButton;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox heightTextBox;
        private System.Windows.Forms.TrackBar heightTrackBar;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox widthTextBox;
        private System.Windows.Forms.TrackBar widthTrackBar;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox exposureTextBox;
        private System.Windows.Forms.TrackBar exposureTrackBar;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox gainTextBox;
        private System.Windows.Forms.TrackBar gainTrackBar;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.CheckBox enableALCcheckBox;
        private System.Windows.Forms.ComboBox ALCTypeComboBox;
        private System.Windows.Forms.TextBox setpointTextBox;
        private System.Windows.Forms.TrackBar setpointTrackBar;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox averageBTextBox;
        private System.Windows.Forms.TextBox averageGTextBox;
        private System.Windows.Forms.TextBox averageRTextBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.NumericUpDown maxExposureNumericUpDown;
        private System.Windows.Forms.NumericUpDown maxGainNumericUpDown;
        private System.Windows.Forms.NumericUpDown minExposureNumericUpDown;
        private System.Windows.Forms.NumericUpDown minGainNumericUpDown;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.NumericUpDown roiheightNumericUpDown;
        private System.Windows.Forms.NumericUpDown roiwidthNumericUpDown;
        private System.Windows.Forms.NumericUpDown yposNumericUpDown;
        private System.Windows.Forms.NumericUpDown xposNumericUpDown;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Label label14;
    }
}

