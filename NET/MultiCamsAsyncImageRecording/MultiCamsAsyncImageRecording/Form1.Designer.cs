namespace MultiCamsAsyncImageRecording
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
            this.components = new System.ComponentModel.Container();
            this.camListBox = new System.Windows.Forms.ListBox();
            this.captureCountNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.skipCountNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.recordingModeComboBox = new System.Windows.Forms.ComboBox();
            this.recordingCountLabel = new System.Windows.Forms.Label();
            this.skipCountLabel = new System.Windows.Forms.Label();
            this.Recordinglabel1 = new System.Windows.Forms.Label();
            this.recordingStatusLabel = new System.Windows.Forms.Label();
            this.startCaptureButton = new System.Windows.Forms.Button();
            this.stopCaptureButton = new System.Windows.Forms.Button();
            this.replayButton = new System.Windows.Forms.Button();
            this.saveButton = new System.Windows.Forms.Button();
            this.saveRawCheckBox = new System.Windows.Forms.CheckBox();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.asynchImageRecordingGroupBox = new System.Windows.Forms.GroupBox();
            this.camListGroupBox = new System.Windows.Forms.GroupBox();
            this.SearchButton = new System.Windows.Forms.Button();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            this.StopButton = new System.Windows.Forms.Button();
            this.StartButton = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.captureCountNumericUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.skipCountNumericUpDown)).BeginInit();
            this.asynchImageRecordingGroupBox.SuspendLayout();
            this.camListGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // camListBox
            // 
            this.camListBox.FormattingEnabled = true;
            this.camListBox.Location = new System.Drawing.Point(6, 17);
            this.camListBox.Name = "camListBox";
            this.camListBox.Size = new System.Drawing.Size(187, 121);
            this.camListBox.TabIndex = 0;
            this.toolTip1.SetToolTip(this.camListBox, "Cameras that are found during Device Discovery ");
            // 
            // captureCountNumericUpDown
            // 
            this.captureCountNumericUpDown.Location = new System.Drawing.Point(102, 30);
            this.captureCountNumericUpDown.Maximum = new decimal(new int[] {
            500,
            0,
            0,
            0});
            this.captureCountNumericUpDown.Name = "captureCountNumericUpDown";
            this.captureCountNumericUpDown.Size = new System.Drawing.Size(92, 20);
            this.captureCountNumericUpDown.TabIndex = 1;
            this.toolTip1.SetToolTip(this.captureCountNumericUpDown, "Number of frames to record");
            this.captureCountNumericUpDown.Value = new decimal(new int[] {
            100,
            0,
            0,
            0});
            // 
            // skipCountNumericUpDown
            // 
            this.skipCountNumericUpDown.Location = new System.Drawing.Point(102, 56);
            this.skipCountNumericUpDown.Maximum = new decimal(new int[] {
            500,
            0,
            0,
            0});
            this.skipCountNumericUpDown.Name = "skipCountNumericUpDown";
            this.skipCountNumericUpDown.Size = new System.Drawing.Size(92, 20);
            this.skipCountNumericUpDown.TabIndex = 2;
            this.toolTip1.SetToolTip(this.skipCountNumericUpDown, "Number of frames to skip during recording");
            // 
            // recordingModeComboBox
            // 
            this.recordingModeComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.recordingModeComboBox.FormattingEnabled = true;
            this.recordingModeComboBox.Items.AddRange(new object[] {
            "List",
            "Cyclical"});
            this.recordingModeComboBox.Location = new System.Drawing.Point(102, 82);
            this.recordingModeComboBox.Name = "recordingModeComboBox";
            this.recordingModeComboBox.Size = new System.Drawing.Size(92, 21);
            this.recordingModeComboBox.TabIndex = 3;
            // 
            // recordingCountLabel
            // 
            this.recordingCountLabel.AutoSize = true;
            this.recordingCountLabel.Location = new System.Drawing.Point(7, 32);
            this.recordingCountLabel.Name = "recordingCountLabel";
            this.recordingCountLabel.Size = new System.Drawing.Size(89, 13);
            this.recordingCountLabel.TabIndex = 4;
            this.recordingCountLabel.Text = "Recording count:";
            // 
            // skipCountLabel
            // 
            this.skipCountLabel.AutoSize = true;
            this.skipCountLabel.Location = new System.Drawing.Point(7, 58);
            this.skipCountLabel.Name = "skipCountLabel";
            this.skipCountLabel.Size = new System.Drawing.Size(61, 13);
            this.skipCountLabel.TabIndex = 5;
            this.skipCountLabel.Text = "Skip count:";
            // 
            // Recordinglabel1
            // 
            this.Recordinglabel1.AutoSize = true;
            this.Recordinglabel1.Location = new System.Drawing.Point(7, 85);
            this.Recordinglabel1.Name = "Recordinglabel1";
            this.Recordinglabel1.Size = new System.Drawing.Size(88, 13);
            this.Recordinglabel1.TabIndex = 6;
            this.Recordinglabel1.Text = "Recording mode:";
            // 
            // recordingStatusLabel
            // 
            this.recordingStatusLabel.AutoSize = true;
            this.recordingStatusLabel.Location = new System.Drawing.Point(15, 297);
            this.recordingStatusLabel.Name = "recordingStatusLabel";
            this.recordingStatusLabel.Size = new System.Drawing.Size(102, 13);
            this.recordingStatusLabel.TabIndex = 7;
            this.recordingStatusLabel.Text = "Recording Stopped.";
            this.toolTip1.SetToolTip(this.recordingStatusLabel, "Status for the recording");
            // 
            // startCaptureButton
            // 
            this.startCaptureButton.Location = new System.Drawing.Point(201, 29);
            this.startCaptureButton.Name = "startCaptureButton";
            this.startCaptureButton.Size = new System.Drawing.Size(100, 23);
            this.startCaptureButton.TabIndex = 8;
            this.startCaptureButton.Text = "Start recording";
            this.toolTip1.SetToolTip(this.startCaptureButton, "Start Image Recording");
            this.startCaptureButton.UseVisualStyleBackColor = true;
            this.startCaptureButton.Click += new System.EventHandler(this.startCaptureButton_Click);
            // 
            // stopCaptureButton
            // 
            this.stopCaptureButton.Location = new System.Drawing.Point(201, 55);
            this.stopCaptureButton.Name = "stopCaptureButton";
            this.stopCaptureButton.Size = new System.Drawing.Size(100, 23);
            this.stopCaptureButton.TabIndex = 9;
            this.stopCaptureButton.Text = "Stop recording";
            this.toolTip1.SetToolTip(this.stopCaptureButton, "Stop Image Recording.");
            this.stopCaptureButton.UseVisualStyleBackColor = true;
            this.stopCaptureButton.Click += new System.EventHandler(this.stopRecordingButton_Click);
            // 
            // replayButton
            // 
            this.replayButton.Location = new System.Drawing.Point(307, 29);
            this.replayButton.Name = "replayButton";
            this.replayButton.Size = new System.Drawing.Size(87, 23);
            this.replayButton.TabIndex = 10;
            this.replayButton.Text = "Replay";
            this.toolTip1.SetToolTip(this.replayButton, "Replay recorded images in a seperate window");
            this.replayButton.UseVisualStyleBackColor = true;
            this.replayButton.Click += new System.EventHandler(this.replayButton_Click);
            // 
            // saveButton
            // 
            this.saveButton.Location = new System.Drawing.Point(400, 29);
            this.saveButton.Name = "saveButton";
            this.saveButton.Size = new System.Drawing.Size(77, 23);
            this.saveButton.TabIndex = 11;
            this.saveButton.Text = "Save";
            this.toolTip1.SetToolTip(this.saveButton, "Save recorded images to disk");
            this.saveButton.UseVisualStyleBackColor = true;
            this.saveButton.Click += new System.EventHandler(this.saveButton_Click);
            // 
            // saveRawCheckBox
            // 
            this.saveRawCheckBox.AutoSize = true;
            this.saveRawCheckBox.Location = new System.Drawing.Point(401, 58);
            this.saveRawCheckBox.Name = "saveRawCheckBox";
            this.saveRawCheckBox.Size = new System.Drawing.Size(76, 17);
            this.saveRawCheckBox.TabIndex = 12;
            this.saveRawCheckBox.Text = "Save Raw";
            this.saveRawCheckBox.UseVisualStyleBackColor = true;
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(224, 297);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(276, 19);
            this.progressBar1.TabIndex = 13;
            // 
            // asynchImageRecordingGroupBox
            // 
            this.asynchImageRecordingGroupBox.Controls.Add(this.recordingCountLabel);
            this.asynchImageRecordingGroupBox.Controls.Add(this.captureCountNumericUpDown);
            this.asynchImageRecordingGroupBox.Controls.Add(this.saveRawCheckBox);
            this.asynchImageRecordingGroupBox.Controls.Add(this.skipCountNumericUpDown);
            this.asynchImageRecordingGroupBox.Controls.Add(this.saveButton);
            this.asynchImageRecordingGroupBox.Controls.Add(this.recordingModeComboBox);
            this.asynchImageRecordingGroupBox.Controls.Add(this.replayButton);
            this.asynchImageRecordingGroupBox.Controls.Add(this.skipCountLabel);
            this.asynchImageRecordingGroupBox.Controls.Add(this.stopCaptureButton);
            this.asynchImageRecordingGroupBox.Controls.Add(this.Recordinglabel1);
            this.asynchImageRecordingGroupBox.Controls.Add(this.startCaptureButton);
            this.asynchImageRecordingGroupBox.Location = new System.Drawing.Point(12, 154);
            this.asynchImageRecordingGroupBox.Name = "asynchImageRecordingGroupBox";
            this.asynchImageRecordingGroupBox.Size = new System.Drawing.Size(487, 127);
            this.asynchImageRecordingGroupBox.TabIndex = 1;
            this.asynchImageRecordingGroupBox.TabStop = false;
            this.asynchImageRecordingGroupBox.Text = "Asynchronous Image Recording Control";
            // 
            // camListGroupBox
            // 
            this.camListGroupBox.Controls.Add(this.SearchButton);
            this.camListGroupBox.Controls.Add(this.camListBox);
            this.camListGroupBox.Location = new System.Drawing.Point(12, 4);
            this.camListGroupBox.Name = "camListGroupBox";
            this.camListGroupBox.Size = new System.Drawing.Size(241, 144);
            this.camListGroupBox.TabIndex = 14;
            this.camListGroupBox.TabStop = false;
            this.camListGroupBox.Text = "Camera List";
            // 
            // SearchButton
            // 
            this.SearchButton.Location = new System.Drawing.Point(206, 19);
            this.SearchButton.Name = "SearchButton";
            this.SearchButton.Size = new System.Drawing.Size(29, 23);
            this.SearchButton.TabIndex = 17;
            this.SearchButton.Text = "...";
            this.toolTip1.SetToolTip(this.SearchButton, "Search for cameras");
            this.SearchButton.UseVisualStyleBackColor = true;
            this.SearchButton.Click += new System.EventHandler(this.SearchButton_Click);
            // 
            // StopButton
            // 
            this.StopButton.Enabled = false;
            this.StopButton.Location = new System.Drawing.Point(319, 62);
            this.StopButton.Name = "StopButton";
            this.StopButton.Size = new System.Drawing.Size(100, 23);
            this.StopButton.TabIndex = 16;
            this.StopButton.Text = "Stop acqusition";
            this.toolTip1.SetToolTip(this.StopButton, "Stop Image Acquisition");
            this.StopButton.UseVisualStyleBackColor = true;
            this.StopButton.Click += new System.EventHandler(this.StopButton_Click);
            // 
            // StartButton
            // 
            this.StartButton.Enabled = false;
            this.StartButton.Location = new System.Drawing.Point(319, 23);
            this.StartButton.Name = "StartButton";
            this.StartButton.Size = new System.Drawing.Size(100, 23);
            this.StartButton.TabIndex = 15;
            this.StartButton.Text = "Start acqusition";
            this.toolTip1.SetToolTip(this.StartButton, "Start Image Acquisition");
            this.StartButton.UseVisualStyleBackColor = true;
            this.StartButton.Click += new System.EventHandler(this.StartButton_Click);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(512, 328);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.StartButton);
            this.Controls.Add(this.camListGroupBox);
            this.Controls.Add(this.asynchImageRecordingGroupBox);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.recordingStatusLabel);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "Form1";
            this.Text = "Async Image Recording from Multiple Cameras";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.captureCountNumericUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.skipCountNumericUpDown)).EndInit();
            this.asynchImageRecordingGroupBox.ResumeLayout(false);
            this.asynchImageRecordingGroupBox.PerformLayout();
            this.camListGroupBox.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox camListBox;
        private System.Windows.Forms.NumericUpDown captureCountNumericUpDown;
        private System.Windows.Forms.NumericUpDown skipCountNumericUpDown;
        private System.Windows.Forms.ComboBox recordingModeComboBox;
        private System.Windows.Forms.Label recordingCountLabel;
        private System.Windows.Forms.Label skipCountLabel;
        private System.Windows.Forms.Label Recordinglabel1;
        private System.Windows.Forms.Label recordingStatusLabel;
        private System.Windows.Forms.Button startCaptureButton;
        private System.Windows.Forms.Button stopCaptureButton;
        private System.Windows.Forms.Button replayButton;
        private System.Windows.Forms.Button saveButton;
        private System.Windows.Forms.CheckBox saveRawCheckBox;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.GroupBox asynchImageRecordingGroupBox;
        private System.Windows.Forms.GroupBox camListGroupBox;
        private System.Windows.Forms.ToolTip toolTip1;
        private System.Windows.Forms.Button StopButton;
        private System.Windows.Forms.Button StartButton;
        private System.Windows.Forms.Button SearchButton;
        private System.Windows.Forms.Timer timer1;
    }
}

