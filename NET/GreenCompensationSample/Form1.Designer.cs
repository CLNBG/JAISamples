namespace GreenCompensationSample
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
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.compGainGroupBox = new System.Windows.Forms.GroupBox();
            this.blueGainNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.greenGainNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.redGainNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.thresholdGroupBox = new System.Windows.Forms.GroupBox();
            this.blueThresholdNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.greenThresholdNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.redThresholdNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.fullImageCheckBox = new System.Windows.Forms.CheckBox();
            this.enableCheckBox = new System.Windows.Forms.CheckBox();
            this.GainGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GainTrackBar)).BeginInit();
            this.ImageSizeGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.HeightNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.WidthNumericUpDown)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.compGainGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.blueGainNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.greenGainNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.redGainNumericUpDown)).BeginInit();
            this.thresholdGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.blueThresholdNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.greenThresholdNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.redThresholdNumericUpDown)).BeginInit();
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
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.compGainGroupBox);
            this.groupBox2.Controls.Add(this.thresholdGroupBox);
            this.groupBox2.Controls.Add(this.fullImageCheckBox);
            this.groupBox2.Controls.Add(this.enableCheckBox);
            this.groupBox2.Location = new System.Drawing.Point(6, 182);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(523, 114);
            this.groupBox2.TabIndex = 11;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Color-Compensation parameters";
            // 
            // compGainGroupBox
            // 
            this.compGainGroupBox.Controls.Add(this.blueGainNumericUpDown);
            this.compGainGroupBox.Controls.Add(this.greenGainNumericUpDown);
            this.compGainGroupBox.Controls.Add(this.redGainNumericUpDown);
            this.compGainGroupBox.Controls.Add(this.label8);
            this.compGainGroupBox.Controls.Add(this.label7);
            this.compGainGroupBox.Controls.Add(this.label6);
            this.compGainGroupBox.Location = new System.Drawing.Point(365, 8);
            this.compGainGroupBox.Name = "compGainGroupBox";
            this.compGainGroupBox.Size = new System.Drawing.Size(152, 100);
            this.compGainGroupBox.TabIndex = 3;
            this.compGainGroupBox.TabStop = false;
            this.compGainGroupBox.Text = "Compensation Gain (%)";
            // 
            // blueGainNumericUpDown
            // 
            this.blueGainNumericUpDown.Location = new System.Drawing.Point(57, 70);
            this.blueGainNumericUpDown.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.blueGainNumericUpDown.Name = "blueGainNumericUpDown";
            this.blueGainNumericUpDown.Size = new System.Drawing.Size(71, 20);
            this.blueGainNumericUpDown.TabIndex = 7;
            this.blueGainNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.blueGainNumericUpDown.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            this.blueGainNumericUpDown.ValueChanged += new System.EventHandler(this.blueGainNumericUpDown_ValueChanged);
            // 
            // greenGainNumericUpDown
            // 
            this.greenGainNumericUpDown.Location = new System.Drawing.Point(57, 44);
            this.greenGainNumericUpDown.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.greenGainNumericUpDown.Name = "greenGainNumericUpDown";
            this.greenGainNumericUpDown.Size = new System.Drawing.Size(71, 20);
            this.greenGainNumericUpDown.TabIndex = 6;
            this.greenGainNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.greenGainNumericUpDown.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            this.greenGainNumericUpDown.ValueChanged += new System.EventHandler(this.greenGainNumericUpDown_ValueChanged);
            // 
            // redGainNumericUpDown
            // 
            this.redGainNumericUpDown.Location = new System.Drawing.Point(57, 18);
            this.redGainNumericUpDown.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.redGainNumericUpDown.Name = "redGainNumericUpDown";
            this.redGainNumericUpDown.Size = new System.Drawing.Size(71, 20);
            this.redGainNumericUpDown.TabIndex = 5;
            this.redGainNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.redGainNumericUpDown.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            this.redGainNumericUpDown.ValueChanged += new System.EventHandler(this.redGainNumericUpDown_ValueChanged);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(12, 72);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(21, 13);
            this.label8.TabIndex = 4;
            this.label8.Text = "B *";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(10, 46);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(22, 13);
            this.label7.TabIndex = 3;
            this.label7.Text = "G *";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(10, 20);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(22, 13);
            this.label6.TabIndex = 2;
            this.label6.Text = "R *";
            // 
            // thresholdGroupBox
            // 
            this.thresholdGroupBox.Controls.Add(this.blueThresholdNumericUpDown);
            this.thresholdGroupBox.Controls.Add(this.greenThresholdNumericUpDown);
            this.thresholdGroupBox.Controls.Add(this.label5);
            this.thresholdGroupBox.Controls.Add(this.label4);
            this.thresholdGroupBox.Controls.Add(this.label3);
            this.thresholdGroupBox.Controls.Add(this.redThresholdNumericUpDown);
            this.thresholdGroupBox.Location = new System.Drawing.Point(206, 8);
            this.thresholdGroupBox.Name = "thresholdGroupBox";
            this.thresholdGroupBox.Size = new System.Drawing.Size(152, 100);
            this.thresholdGroupBox.TabIndex = 2;
            this.thresholdGroupBox.TabStop = false;
            this.thresholdGroupBox.Text = "Threshold";
            // 
            // blueThresholdNumericUpDown
            // 
            this.blueThresholdNumericUpDown.Location = new System.Drawing.Point(54, 70);
            this.blueThresholdNumericUpDown.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.blueThresholdNumericUpDown.Name = "blueThresholdNumericUpDown";
            this.blueThresholdNumericUpDown.Size = new System.Drawing.Size(71, 20);
            this.blueThresholdNumericUpDown.TabIndex = 5;
            this.blueThresholdNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.blueThresholdNumericUpDown.Value = new decimal(new int[] {
            220,
            0,
            0,
            0});
            this.blueThresholdNumericUpDown.ValueChanged += new System.EventHandler(this.blueThresholdNumericUpDown_ValueChanged);
            // 
            // greenThresholdNumericUpDown
            // 
            this.greenThresholdNumericUpDown.Location = new System.Drawing.Point(54, 44);
            this.greenThresholdNumericUpDown.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.greenThresholdNumericUpDown.Name = "greenThresholdNumericUpDown";
            this.greenThresholdNumericUpDown.Size = new System.Drawing.Size(71, 20);
            this.greenThresholdNumericUpDown.TabIndex = 4;
            this.greenThresholdNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.greenThresholdNumericUpDown.Value = new decimal(new int[] {
            250,
            0,
            0,
            0});
            this.greenThresholdNumericUpDown.ValueChanged += new System.EventHandler(this.greenThresholdNumericUpDown_ValueChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(7, 72);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(23, 13);
            this.label5.TabIndex = 3;
            this.label5.Text = "B <";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 46);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(30, 13);
            this.label4.TabIndex = 2;
            this.label4.Text = "G >=";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 20);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(24, 13);
            this.label3.TabIndex = 1;
            this.label3.Text = "R <";
            // 
            // redThresholdNumericUpDown
            // 
            this.redThresholdNumericUpDown.Location = new System.Drawing.Point(54, 18);
            this.redThresholdNumericUpDown.Maximum = new decimal(new int[] {
            255,
            0,
            0,
            0});
            this.redThresholdNumericUpDown.Name = "redThresholdNumericUpDown";
            this.redThresholdNumericUpDown.Size = new System.Drawing.Size(71, 20);
            this.redThresholdNumericUpDown.TabIndex = 0;
            this.redThresholdNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.redThresholdNumericUpDown.Value = new decimal(new int[] {
            240,
            0,
            0,
            0});
            this.redThresholdNumericUpDown.ValueChanged += new System.EventHandler(this.redThresholdNumericUpDown_ValueChanged);
            // 
            // fullImageCheckBox
            // 
            this.fullImageCheckBox.AutoSize = true;
            this.fullImageCheckBox.Location = new System.Drawing.Point(6, 43);
            this.fullImageCheckBox.Name = "fullImageCheckBox";
            this.fullImageCheckBox.Size = new System.Drawing.Size(79, 17);
            this.fullImageCheckBox.TabIndex = 1;
            this.fullImageCheckBox.Text = "Full Screen";
            this.fullImageCheckBox.UseVisualStyleBackColor = true;
            // 
            // enableCheckBox
            // 
            this.enableCheckBox.AutoSize = true;
            this.enableCheckBox.Checked = true;
            this.enableCheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.enableCheckBox.Location = new System.Drawing.Point(6, 20);
            this.enableCheckBox.Name = "enableCheckBox";
            this.enableCheckBox.Size = new System.Drawing.Size(59, 17);
            this.enableCheckBox.TabIndex = 0;
            this.enableCheckBox.Text = "Enable";
            this.enableCheckBox.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(537, 303);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.GainGroupBox);
            this.Controls.Add(this.ImageSizeGroupBox);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.StartButton);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "Green Compensation Sample";
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
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.compGainGroupBox.ResumeLayout(false);
            this.compGainGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.blueGainNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.greenGainNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.redGainNumericUpDown)).EndInit();
            this.thresholdGroupBox.ResumeLayout(false);
            this.thresholdGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.blueThresholdNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.greenThresholdNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.redThresholdNumericUpDown)).EndInit();
            this.ResumeLayout(false);

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
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.CheckBox fullImageCheckBox;
        private System.Windows.Forms.CheckBox enableCheckBox;
        private System.Windows.Forms.GroupBox compGainGroupBox;
        private System.Windows.Forms.NumericUpDown blueGainNumericUpDown;
        private System.Windows.Forms.NumericUpDown greenGainNumericUpDown;
        private System.Windows.Forms.NumericUpDown redGainNumericUpDown;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.GroupBox thresholdGroupBox;
        private System.Windows.Forms.NumericUpDown blueThresholdNumericUpDown;
        private System.Windows.Forms.NumericUpDown greenThresholdNumericUpDown;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown redThresholdNumericUpDown;
    }
}

