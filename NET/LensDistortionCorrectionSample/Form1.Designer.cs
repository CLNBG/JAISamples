namespace LensDistortionCorrectionSample
{
    partial class Form1
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
            this.GainGroupBox = new System.Windows.Forms.GroupBox();
            this.GainLabel = new System.Windows.Forms.Label();
            this.GainTrackBar = new System.Windows.Forms.TrackBar();
            this.ImageSizeGroupBox = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.HeightNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.WidthNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.StopButton = new System.Windows.Forms.Button();
            this.StartButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.SearchButton = new System.Windows.Forms.Button();
            this.CameraIDTextBox = new System.Windows.Forms.TextBox();
            this.undistortParamGroupBox = new System.Windows.Forms.GroupBox();
            this.opticalCenterXNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.opticalCenterYNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.focalLengthXNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.focalLengthYNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.k1NumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.k2NumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.k3NumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.p1NumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.p2NumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.biLinearCheckBox = new System.Windows.Forms.CheckBox();
            this.undistortCheckBox = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.applyButton = new System.Windows.Forms.Button();
            this.GainGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GainTrackBar)).BeginInit();
            this.ImageSizeGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.HeightNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.WidthNumericUpDown)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.undistortParamGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.opticalCenterXNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.opticalCenterYNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.focalLengthXNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.focalLengthYNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.k1NumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.k2NumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.k3NumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.p1NumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.p2NumericUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // GainGroupBox
            // 
            this.GainGroupBox.Controls.Add(this.GainLabel);
            this.GainGroupBox.Controls.Add(this.GainTrackBar);
            this.GainGroupBox.Location = new System.Drawing.Point(212, 84);
            this.GainGroupBox.Name = "GainGroupBox";
            this.GainGroupBox.Size = new System.Drawing.Size(200, 92);
            this.GainGroupBox.TabIndex = 10;
            this.GainGroupBox.TabStop = false;
            this.GainGroupBox.Text = "Gain Control";
            // 
            // GainLabel
            // 
            this.GainLabel.AutoSize = true;
            this.GainLabel.Enabled = false;
            this.GainLabel.Location = new System.Drawing.Point(12, 21);
            this.GainLabel.Name = "GainLabel";
            this.GainLabel.Size = new System.Drawing.Size(13, 13);
            this.GainLabel.TabIndex = 2;
            this.GainLabel.Text = "0";
            // 
            // GainTrackBar
            // 
            this.GainTrackBar.Enabled = false;
            this.GainTrackBar.Location = new System.Drawing.Point(6, 46);
            this.GainTrackBar.Name = "GainTrackBar";
            this.GainTrackBar.Size = new System.Drawing.Size(187, 45);
            this.GainTrackBar.TabIndex = 1;
            this.GainTrackBar.TickFrequency = 10;
            this.GainTrackBar.Scroll += new System.EventHandler(this.GainTrackBar_Scroll);
            // 
            // ImageSizeGroupBox
            // 
            this.ImageSizeGroupBox.Controls.Add(this.label2);
            this.ImageSizeGroupBox.Controls.Add(this.label1);
            this.ImageSizeGroupBox.Controls.Add(this.HeightNumericUpDown);
            this.ImageSizeGroupBox.Controls.Add(this.WidthNumericUpDown);
            this.ImageSizeGroupBox.Location = new System.Drawing.Point(6, 84);
            this.ImageSizeGroupBox.Name = "ImageSizeGroupBox";
            this.ImageSizeGroupBox.Size = new System.Drawing.Size(200, 91);
            this.ImageSizeGroupBox.TabIndex = 9;
            this.ImageSizeGroupBox.TabStop = false;
            this.ImageSizeGroupBox.Text = "Image Size";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 50);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(38, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Height";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Width";
            // 
            // HeightNumericUpDown
            // 
            this.HeightNumericUpDown.Enabled = false;
            this.HeightNumericUpDown.Location = new System.Drawing.Point(74, 48);
            this.HeightNumericUpDown.Name = "HeightNumericUpDown";
            this.HeightNumericUpDown.Size = new System.Drawing.Size(120, 20);
            this.HeightNumericUpDown.TabIndex = 1;
            this.HeightNumericUpDown.ValueChanged += new System.EventHandler(this.HeightNumericUpDown_ValueChanged);
            // 
            // WidthNumericUpDown
            // 
            this.WidthNumericUpDown.Enabled = false;
            this.WidthNumericUpDown.Location = new System.Drawing.Point(74, 20);
            this.WidthNumericUpDown.Name = "WidthNumericUpDown";
            this.WidthNumericUpDown.Size = new System.Drawing.Size(120, 20);
            this.WidthNumericUpDown.TabIndex = 0;
            this.WidthNumericUpDown.ValueChanged += new System.EventHandler(this.WidthNumericUpDown_ValueChanged);
            // 
            // StopButton
            // 
            this.StopButton.Enabled = false;
            this.StopButton.Location = new System.Drawing.Point(448, 122);
            this.StopButton.Name = "StopButton";
            this.StopButton.Size = new System.Drawing.Size(75, 23);
            this.StopButton.TabIndex = 8;
            this.StopButton.Text = "Stop";
            this.StopButton.UseVisualStyleBackColor = true;
            this.StopButton.Click += new System.EventHandler(this.StopButton_Click);
            // 
            // StartButton
            // 
            this.StartButton.Enabled = false;
            this.StartButton.Location = new System.Drawing.Point(448, 93);
            this.StartButton.Name = "StartButton";
            this.StartButton.Size = new System.Drawing.Size(75, 23);
            this.StartButton.TabIndex = 7;
            this.StartButton.Text = "Start";
            this.StartButton.UseVisualStyleBackColor = true;
            this.StartButton.Click += new System.EventHandler(this.StartButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.SearchButton);
            this.groupBox1.Controls.Add(this.CameraIDTextBox);
            this.groupBox1.Location = new System.Drawing.Point(6, 7);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(523, 71);
            this.groupBox1.TabIndex = 6;
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
            // undistortParamGroupBox
            // 
            this.undistortParamGroupBox.Controls.Add(this.applyButton);
            this.undistortParamGroupBox.Controls.Add(this.label11);
            this.undistortParamGroupBox.Controls.Add(this.label10);
            this.undistortParamGroupBox.Controls.Add(this.label9);
            this.undistortParamGroupBox.Controls.Add(this.label8);
            this.undistortParamGroupBox.Controls.Add(this.label7);
            this.undistortParamGroupBox.Controls.Add(this.label6);
            this.undistortParamGroupBox.Controls.Add(this.label5);
            this.undistortParamGroupBox.Controls.Add(this.label4);
            this.undistortParamGroupBox.Controls.Add(this.label3);
            this.undistortParamGroupBox.Controls.Add(this.p2NumericUpDown);
            this.undistortParamGroupBox.Controls.Add(this.biLinearCheckBox);
            this.undistortParamGroupBox.Controls.Add(this.p1NumericUpDown);
            this.undistortParamGroupBox.Controls.Add(this.k3NumericUpDown);
            this.undistortParamGroupBox.Controls.Add(this.k2NumericUpDown);
            this.undistortParamGroupBox.Controls.Add(this.k1NumericUpDown);
            this.undistortParamGroupBox.Controls.Add(this.focalLengthYNumericUpDown);
            this.undistortParamGroupBox.Controls.Add(this.focalLengthXNumericUpDown);
            this.undistortParamGroupBox.Controls.Add(this.opticalCenterYNumericUpDown);
            this.undistortParamGroupBox.Controls.Add(this.opticalCenterXNumericUpDown);
            this.undistortParamGroupBox.Enabled = false;
            this.undistortParamGroupBox.Location = new System.Drawing.Point(6, 182);
            this.undistortParamGroupBox.Name = "undistortParamGroupBox";
            this.undistortParamGroupBox.Size = new System.Drawing.Size(523, 187);
            this.undistortParamGroupBox.TabIndex = 11;
            this.undistortParamGroupBox.TabStop = false;
            this.undistortParamGroupBox.Text = "Lens Distortion Parameters";
            // 
            // opticalCenterXNumericUpDown
            // 
            this.opticalCenterXNumericUpDown.DecimalPlaces = 1;
            this.opticalCenterXNumericUpDown.Location = new System.Drawing.Point(98, 19);
            this.opticalCenterXNumericUpDown.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.opticalCenterXNumericUpDown.Name = "opticalCenterXNumericUpDown";
            this.opticalCenterXNumericUpDown.Size = new System.Drawing.Size(100, 20);
            this.opticalCenterXNumericUpDown.TabIndex = 0;
            this.opticalCenterXNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.opticalCenterXNumericUpDown.ValueChanged += new System.EventHandler(this.opticalCenterXNumericUpDown_ValueChanged);
            // 
            // opticalCenterYNumericUpDown
            // 
            this.opticalCenterYNumericUpDown.DecimalPlaces = 1;
            this.opticalCenterYNumericUpDown.Location = new System.Drawing.Point(98, 45);
            this.opticalCenterYNumericUpDown.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.opticalCenterYNumericUpDown.Name = "opticalCenterYNumericUpDown";
            this.opticalCenterYNumericUpDown.Size = new System.Drawing.Size(100, 20);
            this.opticalCenterYNumericUpDown.TabIndex = 1;
            this.opticalCenterYNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.opticalCenterYNumericUpDown.ValueChanged += new System.EventHandler(this.opticalCenterYNumericUpDown_ValueChanged);
            // 
            // focalLengthXNumericUpDown
            // 
            this.focalLengthXNumericUpDown.DecimalPlaces = 1;
            this.focalLengthXNumericUpDown.Location = new System.Drawing.Point(98, 71);
            this.focalLengthXNumericUpDown.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.focalLengthXNumericUpDown.Name = "focalLengthXNumericUpDown";
            this.focalLengthXNumericUpDown.Size = new System.Drawing.Size(100, 20);
            this.focalLengthXNumericUpDown.TabIndex = 2;
            this.focalLengthXNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.focalLengthXNumericUpDown.ValueChanged += new System.EventHandler(this.focalLengthXNumericUpDown_ValueChanged);
            // 
            // focalLengthYNumericUpDown
            // 
            this.focalLengthYNumericUpDown.DecimalPlaces = 1;
            this.focalLengthYNumericUpDown.Location = new System.Drawing.Point(98, 97);
            this.focalLengthYNumericUpDown.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.focalLengthYNumericUpDown.Name = "focalLengthYNumericUpDown";
            this.focalLengthYNumericUpDown.Size = new System.Drawing.Size(100, 20);
            this.focalLengthYNumericUpDown.TabIndex = 3;
            this.focalLengthYNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.focalLengthYNumericUpDown.ValueChanged += new System.EventHandler(this.focalLengthYNumericUpDown_ValueChanged);
            // 
            // k1NumericUpDown
            // 
            this.k1NumericUpDown.DecimalPlaces = 6;
            this.k1NumericUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            196608});
            this.k1NumericUpDown.Location = new System.Drawing.Point(397, 19);
            this.k1NumericUpDown.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.k1NumericUpDown.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            -2147483648});
            this.k1NumericUpDown.Name = "k1NumericUpDown";
            this.k1NumericUpDown.Size = new System.Drawing.Size(120, 20);
            this.k1NumericUpDown.TabIndex = 4;
            this.k1NumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.k1NumericUpDown.ValueChanged += new System.EventHandler(this.k1NumericUpDown_ValueChanged);
            // 
            // k2NumericUpDown
            // 
            this.k2NumericUpDown.DecimalPlaces = 6;
            this.k2NumericUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            196608});
            this.k2NumericUpDown.Location = new System.Drawing.Point(397, 45);
            this.k2NumericUpDown.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.k2NumericUpDown.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            -2147483648});
            this.k2NumericUpDown.Name = "k2NumericUpDown";
            this.k2NumericUpDown.Size = new System.Drawing.Size(120, 20);
            this.k2NumericUpDown.TabIndex = 5;
            this.k2NumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.k2NumericUpDown.ValueChanged += new System.EventHandler(this.k2NumericUpDown_ValueChanged);
            // 
            // k3NumericUpDown
            // 
            this.k3NumericUpDown.DecimalPlaces = 6;
            this.k3NumericUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            196608});
            this.k3NumericUpDown.Location = new System.Drawing.Point(397, 71);
            this.k3NumericUpDown.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.k3NumericUpDown.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            -2147483648});
            this.k3NumericUpDown.Name = "k3NumericUpDown";
            this.k3NumericUpDown.Size = new System.Drawing.Size(120, 20);
            this.k3NumericUpDown.TabIndex = 6;
            this.k3NumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.k3NumericUpDown.ValueChanged += new System.EventHandler(this.k3NumericUpDown_ValueChanged);
            // 
            // p1NumericUpDown
            // 
            this.p1NumericUpDown.DecimalPlaces = 6;
            this.p1NumericUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            262144});
            this.p1NumericUpDown.Location = new System.Drawing.Point(397, 97);
            this.p1NumericUpDown.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.p1NumericUpDown.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            -2147483648});
            this.p1NumericUpDown.Name = "p1NumericUpDown";
            this.p1NumericUpDown.Size = new System.Drawing.Size(120, 20);
            this.p1NumericUpDown.TabIndex = 7;
            this.p1NumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.p1NumericUpDown.ValueChanged += new System.EventHandler(this.p1NumericUpDown_ValueChanged);
            // 
            // p2NumericUpDown
            // 
            this.p2NumericUpDown.DecimalPlaces = 6;
            this.p2NumericUpDown.Increment = new decimal(new int[] {
            1,
            0,
            0,
            262144});
            this.p2NumericUpDown.Location = new System.Drawing.Point(397, 125);
            this.p2NumericUpDown.Maximum = new decimal(new int[] {
            10,
            0,
            0,
            0});
            this.p2NumericUpDown.Minimum = new decimal(new int[] {
            10,
            0,
            0,
            -2147483648});
            this.p2NumericUpDown.Name = "p2NumericUpDown";
            this.p2NumericUpDown.Size = new System.Drawing.Size(120, 20);
            this.p2NumericUpDown.TabIndex = 8;
            this.p2NumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.p2NumericUpDown.ValueChanged += new System.EventHandler(this.p2NumericUpDown_ValueChanged);
            // 
            // biLinearCheckBox
            // 
            this.biLinearCheckBox.AutoSize = true;
            this.biLinearCheckBox.Location = new System.Drawing.Point(10, 128);
            this.biLinearCheckBox.Name = "biLinearCheckBox";
            this.biLinearCheckBox.Size = new System.Drawing.Size(144, 17);
            this.biLinearCheckBox.TabIndex = 8;
            this.biLinearCheckBox.Text = "Use bi-linear interpolation";
            this.biLinearCheckBox.UseVisualStyleBackColor = true;
            this.biLinearCheckBox.CheckedChanged += new System.EventHandler(this.biLinearCheckBox_CheckedChanged);
            // 
            // undistortCheckBox
            // 
            this.undistortCheckBox.AutoSize = true;
            this.undistortCheckBox.Checked = true;
            this.undistortCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.undistortCheckBox.Location = new System.Drawing.Point(448, 158);
            this.undistortCheckBox.Name = "undistortCheckBox";
            this.undistortCheckBox.Size = new System.Drawing.Size(68, 17);
            this.undistortCheckBox.TabIndex = 9;
            this.undistortCheckBox.Text = "Undistort";
            this.undistortCheckBox.UseVisualStyleBackColor = true;
            this.undistortCheckBox.CheckedChanged += new System.EventHandler(this.undistortCheckBox_CheckedChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 21);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(87, 13);
            this.label3.TabIndex = 10;
            this.label3.Text = "Optical Center X:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 47);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(87, 13);
            this.label4.TabIndex = 11;
            this.label4.Text = "Optical Center Y:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 73);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(78, 13);
            this.label5.TabIndex = 12;
            this.label5.Text = "Focal length X:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(7, 99);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(78, 13);
            this.label6.TabIndex = 13;
            this.label6.Text = "Focal length Y:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(209, 21);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(152, 13);
            this.label7.TabIndex = 14;
            this.label7.Text = "Radial distortion coefficient k1:";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(209, 47);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(152, 13);
            this.label8.TabIndex = 15;
            this.label8.Text = "Radial distortion coefficient k2:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(209, 73);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(152, 13);
            this.label9.TabIndex = 16;
            this.label9.Text = "Radial distortion coefficient k3:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(209, 99);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(172, 13);
            this.label10.TabIndex = 17;
            this.label10.Text = "Tangential distortion coefficient p1:";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(209, 128);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(172, 13);
            this.label11.TabIndex = 18;
            this.label11.Text = "Tangential distortion coefficient p2:";
            // 
            // applyButton
            // 
            this.applyButton.Enabled = false;
            this.applyButton.Location = new System.Drawing.Point(224, 154);
            this.applyButton.Name = "applyButton";
            this.applyButton.Size = new System.Drawing.Size(75, 23);
            this.applyButton.TabIndex = 19;
            this.applyButton.Text = "Apply";
            this.applyButton.UseVisualStyleBackColor = true;
            this.applyButton.Click += new System.EventHandler(this.applyButton_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(537, 376);
            this.Controls.Add(this.undistortParamGroupBox);
            this.Controls.Add(this.GainGroupBox);
            this.Controls.Add(this.ImageSizeGroupBox);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.StartButton);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.undistortCheckBox);
            this.Name = "Form1";
            this.Text = "Lens Distortion Correction Sample";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.GainGroupBox.ResumeLayout(false);
            this.GainGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GainTrackBar)).EndInit();
            this.ImageSizeGroupBox.ResumeLayout(false);
            this.ImageSizeGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.HeightNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.WidthNumericUpDown)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.undistortParamGroupBox.ResumeLayout(false);
            this.undistortParamGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.opticalCenterXNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.opticalCenterYNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.focalLengthXNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.focalLengthYNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.k1NumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.k2NumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.k3NumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.p1NumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.p2NumericUpDown)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox GainGroupBox;
        private System.Windows.Forms.Label GainLabel;
        private System.Windows.Forms.TrackBar GainTrackBar;
        private System.Windows.Forms.GroupBox ImageSizeGroupBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown HeightNumericUpDown;
        private System.Windows.Forms.NumericUpDown WidthNumericUpDown;
        private System.Windows.Forms.Button StopButton;
        private System.Windows.Forms.Button StartButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button SearchButton;
        private System.Windows.Forms.TextBox CameraIDTextBox;
        private System.Windows.Forms.GroupBox undistortParamGroupBox;
        private System.Windows.Forms.NumericUpDown p2NumericUpDown;
        private System.Windows.Forms.CheckBox biLinearCheckBox;
        private System.Windows.Forms.NumericUpDown p1NumericUpDown;
        private System.Windows.Forms.NumericUpDown k3NumericUpDown;
        private System.Windows.Forms.NumericUpDown k2NumericUpDown;
        private System.Windows.Forms.NumericUpDown k1NumericUpDown;
        private System.Windows.Forms.NumericUpDown focalLengthYNumericUpDown;
        private System.Windows.Forms.NumericUpDown focalLengthXNumericUpDown;
        private System.Windows.Forms.NumericUpDown opticalCenterYNumericUpDown;
        private System.Windows.Forms.NumericUpDown opticalCenterXNumericUpDown;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox undistortCheckBox;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button applyButton;
    }
}

