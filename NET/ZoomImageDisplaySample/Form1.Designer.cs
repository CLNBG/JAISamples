namespace ZoomImageDisplaySample
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
            this.CameraIDTextBox = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.SearchButton = new System.Windows.Forms.Button();
            this.StartButton = new System.Windows.Forms.Button();
            this.StopButton = new System.Windows.Forms.Button();
            this.ImageSizeGroupBox = new System.Windows.Forms.GroupBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.HeightNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.WidthNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.GainGroupBox = new System.Windows.Forms.GroupBox();
            this.GainLabel = new System.Windows.Forms.Label();
            this.GainTrackBar = new System.Windows.Forms.TrackBar();
            this.ZoomOutbutton = new System.Windows.Forms.Button();
            this.ZoomResetbutton = new System.Windows.Forms.Button();
            this.ZoomInbutton = new System.Windows.Forms.Button();
            this.RightDownButton = new System.Windows.Forms.Button();
            this.DownButton = new System.Windows.Forms.Button();
            this.LeftDownButton = new System.Windows.Forms.Button();
            this.RightButton = new System.Windows.Forms.Button();
            this.CenterButton = new System.Windows.Forms.Button();
            this.LeftButton = new System.Windows.Forms.Button();
            this.RightUpButton = new System.Windows.Forms.Button();
            this.UpButton = new System.Windows.Forms.Button();
            this.LeftUpButton = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.groupBox1.SuspendLayout();
            this.ImageSizeGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.HeightNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.WidthNumericUpDown)).BeginInit();
            this.GainGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GainTrackBar)).BeginInit();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
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
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.SearchButton);
            this.groupBox1.Controls.Add(this.CameraIDTextBox);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(523, 71);
            this.groupBox1.TabIndex = 1;
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
            // StartButton
            // 
            this.StartButton.Enabled = false;
            this.StartButton.Location = new System.Drawing.Point(425, 98);
            this.StartButton.Name = "StartButton";
            this.StartButton.Size = new System.Drawing.Size(111, 23);
            this.StartButton.TabIndex = 2;
            this.StartButton.Text = "Start Acquisition";
            this.StartButton.UseVisualStyleBackColor = true;
            this.StartButton.Click += new System.EventHandler(this.StartButton_Click);
            // 
            // StopButton
            // 
            this.StopButton.Enabled = false;
            this.StopButton.Location = new System.Drawing.Point(425, 127);
            this.StopButton.Name = "StopButton";
            this.StopButton.Size = new System.Drawing.Size(111, 23);
            this.StopButton.TabIndex = 3;
            this.StopButton.Text = "Stop Acquisition";
            this.StopButton.UseVisualStyleBackColor = true;
            this.StopButton.Click += new System.EventHandler(this.StopButton_Click);
            // 
            // ImageSizeGroupBox
            // 
            this.ImageSizeGroupBox.Controls.Add(this.label2);
            this.ImageSizeGroupBox.Controls.Add(this.label1);
            this.ImageSizeGroupBox.Controls.Add(this.HeightNumericUpDown);
            this.ImageSizeGroupBox.Controls.Add(this.WidthNumericUpDown);
            this.ImageSizeGroupBox.Location = new System.Drawing.Point(12, 89);
            this.ImageSizeGroupBox.Name = "ImageSizeGroupBox";
            this.ImageSizeGroupBox.Size = new System.Drawing.Size(200, 80);
            this.ImageSizeGroupBox.TabIndex = 4;
            this.ImageSizeGroupBox.TabStop = false;
            this.ImageSizeGroupBox.Text = "Image Size";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(38, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Height";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(35, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Width";
            // 
            // HeightNumericUpDown
            // 
            this.HeightNumericUpDown.Enabled = false;
            this.HeightNumericUpDown.Location = new System.Drawing.Point(74, 49);
            this.HeightNumericUpDown.Name = "HeightNumericUpDown";
            this.HeightNumericUpDown.Size = new System.Drawing.Size(120, 20);
            this.HeightNumericUpDown.TabIndex = 1;
            this.HeightNumericUpDown.ValueChanged += new System.EventHandler(this.HeightNumericUpDown_ValueChanged);
            // 
            // WidthNumericUpDown
            // 
            this.WidthNumericUpDown.Enabled = false;
            this.WidthNumericUpDown.Location = new System.Drawing.Point(74, 18);
            this.WidthNumericUpDown.Name = "WidthNumericUpDown";
            this.WidthNumericUpDown.Size = new System.Drawing.Size(120, 20);
            this.WidthNumericUpDown.TabIndex = 0;
            this.WidthNumericUpDown.ValueChanged += new System.EventHandler(this.WidthNumericUpDown_ValueChanged);
            // 
            // GainGroupBox
            // 
            this.GainGroupBox.Controls.Add(this.GainLabel);
            this.GainGroupBox.Controls.Add(this.GainTrackBar);
            this.GainGroupBox.Location = new System.Drawing.Point(218, 89);
            this.GainGroupBox.Name = "GainGroupBox";
            this.GainGroupBox.Size = new System.Drawing.Size(200, 80);
            this.GainGroupBox.TabIndex = 5;
            this.GainGroupBox.TabStop = false;
            this.GainGroupBox.Text = "Gain Control";
            // 
            // GainLabel
            // 
            this.GainLabel.AutoSize = true;
            this.GainLabel.Enabled = false;
            this.GainLabel.Location = new System.Drawing.Point(12, 17);
            this.GainLabel.Name = "GainLabel";
            this.GainLabel.Size = new System.Drawing.Size(13, 13);
            this.GainLabel.TabIndex = 2;
            this.GainLabel.Text = "0";
            // 
            // GainTrackBar
            // 
            this.GainTrackBar.Enabled = false;
            this.GainTrackBar.Location = new System.Drawing.Point(6, 32);
            this.GainTrackBar.Name = "GainTrackBar";
            this.GainTrackBar.Size = new System.Drawing.Size(187, 45);
            this.GainTrackBar.TabIndex = 1;
            this.GainTrackBar.TickFrequency = 20;
            this.GainTrackBar.TickStyle = System.Windows.Forms.TickStyle.Both;
            this.GainTrackBar.Scroll += new System.EventHandler(this.GainTrackBar_Scroll);
            // 
            // ZoomOutbutton
            // 
            this.ZoomOutbutton.Image = global::ZoomImageDisplaySample.Properties.Resources.zoom_out;
            this.ZoomOutbutton.Location = new System.Drawing.Point(101, 62);
            this.ZoomOutbutton.Name = "ZoomOutbutton";
            this.ZoomOutbutton.Size = new System.Drawing.Size(47, 47);
            this.ZoomOutbutton.TabIndex = 17;
            this.ZoomOutbutton.UseVisualStyleBackColor = true;
            this.ZoomOutbutton.Click += new System.EventHandler(this.ZoomOutbutton_Click);
            // 
            // ZoomResetbutton
            // 
            this.ZoomResetbutton.Image = global::ZoomImageDisplaySample.Properties.Resources.search;
            this.ZoomResetbutton.Location = new System.Drawing.Point(55, 62);
            this.ZoomResetbutton.Name = "ZoomResetbutton";
            this.ZoomResetbutton.Size = new System.Drawing.Size(47, 47);
            this.ZoomResetbutton.TabIndex = 16;
            this.ZoomResetbutton.UseVisualStyleBackColor = true;
            this.ZoomResetbutton.Click += new System.EventHandler(this.ZoomResetbutton_Click);
            // 
            // ZoomInbutton
            // 
            this.ZoomInbutton.Image = global::ZoomImageDisplaySample.Properties.Resources.zoom_in;
            this.ZoomInbutton.Location = new System.Drawing.Point(9, 62);
            this.ZoomInbutton.Name = "ZoomInbutton";
            this.ZoomInbutton.Size = new System.Drawing.Size(47, 47);
            this.ZoomInbutton.TabIndex = 15;
            this.ZoomInbutton.UseVisualStyleBackColor = true;
            this.ZoomInbutton.Click += new System.EventHandler(this.ZoomInbutton_Click);
            // 
            // RightDownButton
            // 
            this.RightDownButton.Image = global::ZoomImageDisplaySample.Properties.Resources.right_down;
            this.RightDownButton.Location = new System.Drawing.Point(100, 108);
            this.RightDownButton.Name = "RightDownButton";
            this.RightDownButton.Size = new System.Drawing.Size(47, 47);
            this.RightDownButton.TabIndex = 14;
            this.RightDownButton.UseVisualStyleBackColor = true;
            this.RightDownButton.Click += new System.EventHandler(this.RightDownButton_Click);
            // 
            // DownButton
            // 
            this.DownButton.Image = global::ZoomImageDisplaySample.Properties.Resources.down;
            this.DownButton.Location = new System.Drawing.Point(54, 108);
            this.DownButton.Name = "DownButton";
            this.DownButton.Size = new System.Drawing.Size(47, 47);
            this.DownButton.TabIndex = 13;
            this.DownButton.UseVisualStyleBackColor = true;
            this.DownButton.Click += new System.EventHandler(this.DownButton_Click);
            // 
            // LeftDownButton
            // 
            this.LeftDownButton.Image = global::ZoomImageDisplaySample.Properties.Resources.left_down;
            this.LeftDownButton.Location = new System.Drawing.Point(8, 108);
            this.LeftDownButton.Name = "LeftDownButton";
            this.LeftDownButton.Size = new System.Drawing.Size(47, 47);
            this.LeftDownButton.TabIndex = 12;
            this.LeftDownButton.UseVisualStyleBackColor = true;
            this.LeftDownButton.Click += new System.EventHandler(this.LeftDownButton_Click);
            // 
            // RightButton
            // 
            this.RightButton.Image = global::ZoomImageDisplaySample.Properties.Resources.right;
            this.RightButton.Location = new System.Drawing.Point(100, 62);
            this.RightButton.Name = "RightButton";
            this.RightButton.Size = new System.Drawing.Size(47, 47);
            this.RightButton.TabIndex = 11;
            this.RightButton.UseVisualStyleBackColor = true;
            this.RightButton.Click += new System.EventHandler(this.RightButton_Click);
            // 
            // CenterButton
            // 
            this.CenterButton.Image = global::ZoomImageDisplaySample.Properties.Resources.home;
            this.CenterButton.Location = new System.Drawing.Point(54, 62);
            this.CenterButton.Name = "CenterButton";
            this.CenterButton.Size = new System.Drawing.Size(47, 47);
            this.CenterButton.TabIndex = 10;
            this.CenterButton.UseVisualStyleBackColor = true;
            this.CenterButton.Click += new System.EventHandler(this.CenterButton_Click);
            // 
            // LeftButton
            // 
            this.LeftButton.Image = global::ZoomImageDisplaySample.Properties.Resources.left;
            this.LeftButton.Location = new System.Drawing.Point(8, 62);
            this.LeftButton.Name = "LeftButton";
            this.LeftButton.Size = new System.Drawing.Size(47, 47);
            this.LeftButton.TabIndex = 9;
            this.LeftButton.UseVisualStyleBackColor = true;
            this.LeftButton.Click += new System.EventHandler(this.LeftButton_Click);
            // 
            // RightUpButton
            // 
            this.RightUpButton.Image = global::ZoomImageDisplaySample.Properties.Resources.right_up;
            this.RightUpButton.Location = new System.Drawing.Point(101, 16);
            this.RightUpButton.Name = "RightUpButton";
            this.RightUpButton.Size = new System.Drawing.Size(47, 47);
            this.RightUpButton.TabIndex = 8;
            this.RightUpButton.UseVisualStyleBackColor = true;
            this.RightUpButton.Click += new System.EventHandler(this.RightUpButton_Click);
            // 
            // UpButton
            // 
            this.UpButton.Image = global::ZoomImageDisplaySample.Properties.Resources.up;
            this.UpButton.Location = new System.Drawing.Point(54, 16);
            this.UpButton.Name = "UpButton";
            this.UpButton.Size = new System.Drawing.Size(47, 47);
            this.UpButton.TabIndex = 7;
            this.UpButton.UseVisualStyleBackColor = true;
            this.UpButton.Click += new System.EventHandler(this.UpButton_Click);
            // 
            // LeftUpButton
            // 
            this.LeftUpButton.Image = global::ZoomImageDisplaySample.Properties.Resources.left_up;
            this.LeftUpButton.Location = new System.Drawing.Point(8, 16);
            this.LeftUpButton.Name = "LeftUpButton";
            this.LeftUpButton.Size = new System.Drawing.Size(47, 47);
            this.LeftUpButton.TabIndex = 6;
            this.LeftUpButton.UseVisualStyleBackColor = true;
            this.LeftUpButton.Click += new System.EventHandler(this.LeftUpButton_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.LeftDownButton);
            this.groupBox2.Controls.Add(this.LeftUpButton);
            this.groupBox2.Controls.Add(this.UpButton);
            this.groupBox2.Controls.Add(this.RightUpButton);
            this.groupBox2.Controls.Add(this.RightDownButton);
            this.groupBox2.Controls.Add(this.LeftButton);
            this.groupBox2.Controls.Add(this.DownButton);
            this.groupBox2.Controls.Add(this.CenterButton);
            this.groupBox2.Controls.Add(this.RightButton);
            this.groupBox2.Location = new System.Drawing.Point(13, 172);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(156, 163);
            this.groupBox2.TabIndex = 18;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Pan Control";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.ZoomOutbutton);
            this.groupBox3.Controls.Add(this.ZoomInbutton);
            this.groupBox3.Controls.Add(this.ZoomResetbutton);
            this.groupBox3.Location = new System.Drawing.Point(176, 172);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(156, 163);
            this.groupBox3.TabIndex = 19;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Zoom Control";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(546, 347);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.GainGroupBox);
            this.Controls.Add(this.ImageSizeGroupBox);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.StartButton);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "Zoom Image Display Sample";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ImageSizeGroupBox.ResumeLayout(false);
            this.ImageSizeGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.HeightNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.WidthNumericUpDown)).EndInit();
            this.GainGroupBox.ResumeLayout(false);
            this.GainGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.GainTrackBar)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox CameraIDTextBox;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button StartButton;
        private System.Windows.Forms.Button StopButton;
        private System.Windows.Forms.GroupBox ImageSizeGroupBox;
        private System.Windows.Forms.NumericUpDown HeightNumericUpDown;
        private System.Windows.Forms.NumericUpDown WidthNumericUpDown;
        private System.Windows.Forms.GroupBox GainGroupBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TrackBar GainTrackBar;
        private System.Windows.Forms.Button SearchButton;
        private System.Windows.Forms.Label GainLabel;
        private System.Windows.Forms.Button LeftUpButton;
        private System.Windows.Forms.Button UpButton;
        private System.Windows.Forms.Button RightUpButton;
        private System.Windows.Forms.Button LeftButton;
        private System.Windows.Forms.Button CenterButton;
        private System.Windows.Forms.Button RightButton;
        private System.Windows.Forms.Button LeftDownButton;
        private System.Windows.Forms.Button DownButton;
        private System.Windows.Forms.Button RightDownButton;
        private System.Windows.Forms.Button ZoomInbutton;
        private System.Windows.Forms.Button ZoomResetbutton;
        private System.Windows.Forms.Button ZoomOutbutton;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
    }
}

