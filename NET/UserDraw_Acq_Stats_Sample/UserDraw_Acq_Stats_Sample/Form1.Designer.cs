namespace UserDrawSample
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
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.SearchButton = new System.Windows.Forms.Button();
            this.CameraIDTextBox = new System.Windows.Forms.TextBox();
            this.stretchCheckBox = new System.Windows.Forms.CheckBox();
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
            this.enableDoubleBufferingCheckBox = new System.Windows.Forms.CheckBox();
            this.enableUserDrawImageGridCheckBox = new System.Windows.Forms.CheckBox();
            this.enableUserDrawViewWindowGridCheckBox = new System.Windows.Forms.CheckBox();
            this.imageOverlayTextBox = new System.Windows.Forms.TextBox();
            this.viewWindowOverlayTextBox = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.GainGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GainTrackBar)).BeginInit();
            this.ImageSizeGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.HeightNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.WidthNumericUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.SearchButton);
            this.groupBox1.Controls.Add(this.CameraIDTextBox);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(523, 71);
            this.groupBox1.TabIndex = 2;
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
            // stretchCheckBox
            // 
            this.stretchCheckBox.AutoSize = true;
            this.stretchCheckBox.Location = new System.Drawing.Point(440, 164);
            this.stretchCheckBox.Name = "stretchCheckBox";
            this.stretchCheckBox.Size = new System.Drawing.Size(89, 17);
            this.stretchCheckBox.TabIndex = 12;
            this.stretchCheckBox.Text = "Stretch video";
            this.stretchCheckBox.UseVisualStyleBackColor = true;
            // 
            // GainGroupBox
            // 
            this.GainGroupBox.Controls.Add(this.GainLabel);
            this.GainGroupBox.Controls.Add(this.GainTrackBar);
            this.GainGroupBox.Location = new System.Drawing.Point(218, 89);
            this.GainGroupBox.Name = "GainGroupBox";
            this.GainGroupBox.Size = new System.Drawing.Size(200, 91);
            this.GainGroupBox.TabIndex = 11;
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
            this.GainTrackBar.Location = new System.Drawing.Point(6, 44);
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
            this.ImageSizeGroupBox.Location = new System.Drawing.Point(12, 89);
            this.ImageSizeGroupBox.Name = "ImageSizeGroupBox";
            this.ImageSizeGroupBox.Size = new System.Drawing.Size(200, 91);
            this.ImageSizeGroupBox.TabIndex = 10;
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
            this.StopButton.Location = new System.Drawing.Point(454, 127);
            this.StopButton.Name = "StopButton";
            this.StopButton.Size = new System.Drawing.Size(75, 23);
            this.StopButton.TabIndex = 9;
            this.StopButton.Text = "Stop";
            this.StopButton.UseVisualStyleBackColor = true;
            this.StopButton.Click += new System.EventHandler(this.StopButton_Click);
            // 
            // StartButton
            // 
            this.StartButton.Enabled = false;
            this.StartButton.Location = new System.Drawing.Point(454, 98);
            this.StartButton.Name = "StartButton";
            this.StartButton.Size = new System.Drawing.Size(75, 23);
            this.StartButton.TabIndex = 8;
            this.StartButton.Text = "Start";
            this.StartButton.UseVisualStyleBackColor = true;
            this.StartButton.Click += new System.EventHandler(this.StartButton_Click);
            // 
            // enableDoubleBufferingCheckBox
            // 
            this.enableDoubleBufferingCheckBox.AutoSize = true;
            this.enableDoubleBufferingCheckBox.Location = new System.Drawing.Point(12, 186);
            this.enableDoubleBufferingCheckBox.Name = "enableDoubleBufferingCheckBox";
            this.enableDoubleBufferingCheckBox.Size = new System.Drawing.Size(368, 17);
            this.enableDoubleBufferingCheckBox.TabIndex = 13;
            this.enableDoubleBufferingCheckBox.Text = "Enable Double-Buffering (Removes flickering from View Window overlay)";
            this.enableDoubleBufferingCheckBox.UseVisualStyleBackColor = true;
            this.enableDoubleBufferingCheckBox.CheckedChanged += new System.EventHandler(this.enableDoubleBufferingCheckBox_CheckedChanged);
            // 
            // enableUserDrawImageGridCheckBox
            // 
            this.enableUserDrawImageGridCheckBox.AutoSize = true;
            this.enableUserDrawImageGridCheckBox.Location = new System.Drawing.Point(12, 210);
            this.enableUserDrawImageGridCheckBox.Name = "enableUserDrawImageGridCheckBox";
            this.enableUserDrawImageGridCheckBox.Size = new System.Drawing.Size(144, 17);
            this.enableUserDrawImageGridCheckBox.TabIndex = 14;
            this.enableUserDrawImageGridCheckBox.Text = "Draw Image Grid Overlay";
            this.enableUserDrawImageGridCheckBox.UseVisualStyleBackColor = true;
            // 
            // enableUserDrawViewWindowGridCheckBox
            // 
            this.enableUserDrawViewWindowGridCheckBox.AutoSize = true;
            this.enableUserDrawViewWindowGridCheckBox.Location = new System.Drawing.Point(12, 236);
            this.enableUserDrawViewWindowGridCheckBox.Name = "enableUserDrawViewWindowGridCheckBox";
            this.enableUserDrawViewWindowGridCheckBox.Size = new System.Drawing.Size(180, 17);
            this.enableUserDrawViewWindowGridCheckBox.TabIndex = 15;
            this.enableUserDrawViewWindowGridCheckBox.Text = "Draw View Window Grid Overlay";
            this.enableUserDrawViewWindowGridCheckBox.UseVisualStyleBackColor = true;
            // 
            // imageOverlayTextBox
            // 
            this.imageOverlayTextBox.Location = new System.Drawing.Point(218, 208);
            this.imageOverlayTextBox.Name = "imageOverlayTextBox";
            this.imageOverlayTextBox.Size = new System.Drawing.Size(317, 20);
            this.imageOverlayTextBox.TabIndex = 16;
            this.imageOverlayTextBox.Text = "Image Overlay text example";
            this.imageOverlayTextBox.TextChanged += new System.EventHandler(this.imageOverlayTextBox_TextChanged);
            // 
            // viewWindowOverlayTextBox
            // 
            this.viewWindowOverlayTextBox.Location = new System.Drawing.Point(218, 234);
            this.viewWindowOverlayTextBox.Name = "viewWindowOverlayTextBox";
            this.viewWindowOverlayTextBox.Size = new System.Drawing.Size(317, 20);
            this.viewWindowOverlayTextBox.TabIndex = 17;
            this.viewWindowOverlayTextBox.Text = "View Window Overlay text example";
            this.viewWindowOverlayTextBox.TextChanged += new System.EventHandler(this.viewWindowOverlayTextBox_TextChanged);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(543, 266);
            this.Controls.Add(this.viewWindowOverlayTextBox);
            this.Controls.Add(this.imageOverlayTextBox);
            this.Controls.Add(this.enableUserDrawViewWindowGridCheckBox);
            this.Controls.Add(this.enableUserDrawImageGridCheckBox);
            this.Controls.Add(this.enableDoubleBufferingCheckBox);
            this.Controls.Add(this.stretchCheckBox);
            this.Controls.Add(this.GainGroupBox);
            this.Controls.Add(this.ImageSizeGroupBox);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.StartButton);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "User-Draw Sample";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.GainGroupBox.ResumeLayout(false);
            this.GainGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GainTrackBar)).EndInit();
            this.ImageSizeGroupBox.ResumeLayout(false);
            this.ImageSizeGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.HeightNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.WidthNumericUpDown)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button SearchButton;
        private System.Windows.Forms.TextBox CameraIDTextBox;
        private System.Windows.Forms.CheckBox stretchCheckBox;
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
        private System.Windows.Forms.CheckBox enableDoubleBufferingCheckBox;
        private System.Windows.Forms.CheckBox enableUserDrawImageGridCheckBox;
        private System.Windows.Forms.CheckBox enableUserDrawViewWindowGridCheckBox;
        private System.Windows.Forms.TextBox imageOverlayTextBox;
        private System.Windows.Forms.TextBox viewWindowOverlayTextBox;
    }
}

