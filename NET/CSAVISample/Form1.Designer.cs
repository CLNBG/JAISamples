namespace AVISample
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
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.playAVIButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.optimizeForMono8CheckBox = new System.Windows.Forms.CheckBox();
            this.selectCodecCheckBox = new System.Windows.Forms.CheckBox();
            this.frameRateNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.GainGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GainTrackBar)).BeginInit();
            this.ImageSizeGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.HeightNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.WidthNumericUpDown)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.frameRateNumericUpDown)).BeginInit();
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
            // saveFileDialog1
            // 
            this.saveFileDialog1.DefaultExt = "avi";
            this.saveFileDialog1.FileName = "*.avi";
            this.saveFileDialog1.Filter = "AVI Files (*.avi)|*.avi|All Files (*.*)|*.*||";
            // 
            // playAVIButton
            // 
            this.playAVIButton.Location = new System.Drawing.Point(448, 152);
            this.playAVIButton.Name = "playAVIButton";
            this.playAVIButton.Size = new System.Drawing.Size(75, 23);
            this.playAVIButton.TabIndex = 11;
            this.playAVIButton.Text = "Play AVI";
            this.playAVIButton.UseVisualStyleBackColor = true;
            this.playAVIButton.Click += new System.EventHandler(this.playAVIButton_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.optimizeForMono8CheckBox);
            this.groupBox2.Controls.Add(this.selectCodecCheckBox);
            this.groupBox2.Controls.Add(this.frameRateNumericUpDown);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Location = new System.Drawing.Point(6, 182);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(525, 48);
            this.groupBox2.TabIndex = 12;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "AVI creation settings";
            // 
            // optimizeForMono8CheckBox
            // 
            this.optimizeForMono8CheckBox.AutoSize = true;
            this.optimizeForMono8CheckBox.Checked = true;
            this.optimizeForMono8CheckBox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.optimizeForMono8CheckBox.Location = new System.Drawing.Point(310, 20);
            this.optimizeForMono8CheckBox.Name = "optimizeForMono8CheckBox";
            this.optimizeForMono8CheckBox.Size = new System.Drawing.Size(173, 17);
            this.optimizeForMono8CheckBox.TabIndex = 3;
            this.optimizeForMono8CheckBox.Text = "Optimize for Mono8 pixel format";
            this.optimizeForMono8CheckBox.UseVisualStyleBackColor = true;
            // 
            // selectCodecCheckBox
            // 
            this.selectCodecCheckBox.AutoSize = true;
            this.selectCodecCheckBox.Location = new System.Drawing.Point(147, 20);
            this.selectCodecCheckBox.Name = "selectCodecCheckBox";
            this.selectCodecCheckBox.Size = new System.Drawing.Size(141, 17);
            this.selectCodecCheckBox.TabIndex = 2;
            this.selectCodecCheckBox.Text = "Manually select encoder";
            this.selectCodecCheckBox.UseVisualStyleBackColor = true;
            // 
            // frameRateNumericUpDown
            // 
            this.frameRateNumericUpDown.Location = new System.Drawing.Point(72, 18);
            this.frameRateNumericUpDown.Name = "frameRateNumericUpDown";
            this.frameRateNumericUpDown.Size = new System.Drawing.Size(57, 20);
            this.frameRateNumericUpDown.TabIndex = 1;
            this.frameRateNumericUpDown.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(10, 20);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(57, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "Frame rate";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(543, 236);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.playAVIButton);
            this.Controls.Add(this.GainGroupBox);
            this.Controls.Add(this.ImageSizeGroupBox);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.StartButton);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "AVI Sample";
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
            ((System.ComponentModel.ISupportInitialize)(this.frameRateNumericUpDown)).EndInit();
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
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
       private System.Windows.Forms.Button playAVIButton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.CheckBox selectCodecCheckBox;
        private System.Windows.Forms.NumericUpDown frameRateNumericUpDown;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.CheckBox optimizeForMono8CheckBox;
    }
}

