namespace CSharpSwTrigger
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
            this.camListComboBox = new System.Windows.Forms.ComboBox();
            this.cameraIdTextBox = new System.Windows.Forms.TextBox();
            this.freeRunRadio = new System.Windows.Forms.RadioButton();
            this.swTrigRadio = new System.Windows.Forms.RadioButton();
            this.widthTrackBar = new System.Windows.Forms.TrackBar();
            this.widthLabel = new System.Windows.Forms.Label();
            this.widthTextBox = new System.Windows.Forms.TextBox();
            this.heightTextBox = new System.Windows.Forms.TextBox();
            this.heightLabel = new System.Windows.Forms.Label();
            this.heightTrackBar = new System.Windows.Forms.TrackBar();
            this.gainTextBox = new System.Windows.Forms.TextBox();
            this.gainLabel = new System.Windows.Forms.Label();
            this.gainTrackBar = new System.Windows.Forms.TrackBar();
            this.startButton = new System.Windows.Forms.Button();
            this.stopButton = new System.Windows.Forms.Button();
            this.wBalanceButton = new System.Windows.Forms.Button();
            this.swTriggerButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.statusBarLabel = new System.Windows.Forms.ToolStripStatusLabel();
            ((System.ComponentModel.ISupportInitialize)(this.widthTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.heightTrackBar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.gainTrackBar)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // camListComboBox
            // 
            this.camListComboBox.FormattingEnabled = true;
            this.camListComboBox.Location = new System.Drawing.Point(12, 20);
            this.camListComboBox.Name = "camListComboBox";
            this.camListComboBox.Size = new System.Drawing.Size(170, 21);
            this.camListComboBox.TabIndex = 0;
            this.camListComboBox.SelectedIndexChanged += new System.EventHandler(this.camListComboBox_SelectedIndexChanged);
            // 
            // cameraIdTextBox
            // 
            this.cameraIdTextBox.BackColor = System.Drawing.SystemColors.Control;
            this.cameraIdTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.cameraIdTextBox.Location = new System.Drawing.Point(12, 47);
            this.cameraIdTextBox.Multiline = true;
            this.cameraIdTextBox.Name = "cameraIdTextBox";
            this.cameraIdTextBox.Size = new System.Drawing.Size(433, 55);
            this.cameraIdTextBox.TabIndex = 1;
            // 
            // freeRunRadio
            // 
            this.freeRunRadio.AutoSize = true;
            this.freeRunRadio.Enabled = false;
            this.freeRunRadio.Location = new System.Drawing.Point(23, 22);
            this.freeRunRadio.Name = "freeRunRadio";
            this.freeRunRadio.Size = new System.Drawing.Size(89, 17);
            this.freeRunRadio.TabIndex = 2;
            this.freeRunRadio.TabStop = true;
            this.freeRunRadio.Text = "Free Running";
            this.freeRunRadio.UseVisualStyleBackColor = true;
            this.freeRunRadio.Click += new System.EventHandler(this.freeRunRadio_Click);
            // 
            // swTrigRadio
            // 
            this.swTrigRadio.AutoSize = true;
            this.swTrigRadio.Enabled = false;
            this.swTrigRadio.Location = new System.Drawing.Point(147, 22);
            this.swTrigRadio.Name = "swTrigRadio";
            this.swTrigRadio.Size = new System.Drawing.Size(103, 17);
            this.swTrigRadio.TabIndex = 3;
            this.swTrigRadio.TabStop = true;
            this.swTrigRadio.Text = "Software Trigger";
            this.swTrigRadio.UseVisualStyleBackColor = true;
            this.swTrigRadio.Click += new System.EventHandler(this.swTrigRadio_Click);
            // 
            // widthTrackBar
            // 
            this.widthTrackBar.Enabled = false;
            this.widthTrackBar.Location = new System.Drawing.Point(39, 74);
            this.widthTrackBar.Name = "widthTrackBar";
            this.widthTrackBar.Size = new System.Drawing.Size(180, 45);
            this.widthTrackBar.TabIndex = 4;
            this.widthTrackBar.Scroll += new System.EventHandler(this.widthTrackBar_Scroll);
            // 
            // widthLabel
            // 
            this.widthLabel.AutoSize = true;
            this.widthLabel.Location = new System.Drawing.Point(9, 81);
            this.widthLabel.Name = "widthLabel";
            this.widthLabel.Size = new System.Drawing.Size(35, 13);
            this.widthLabel.TabIndex = 5;
            this.widthLabel.Text = "Width";
            // 
            // widthTextBox
            // 
            this.widthTextBox.Enabled = false;
            this.widthTextBox.Location = new System.Drawing.Point(216, 77);
            this.widthTextBox.Name = "widthTextBox";
            this.widthTextBox.Size = new System.Drawing.Size(60, 20);
            this.widthTextBox.TabIndex = 6;
            this.widthTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // heightTextBox
            // 
            this.heightTextBox.Enabled = false;
            this.heightTextBox.Location = new System.Drawing.Point(216, 119);
            this.heightTextBox.Name = "heightTextBox";
            this.heightTextBox.Size = new System.Drawing.Size(60, 20);
            this.heightTextBox.TabIndex = 9;
            this.heightTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // heightLabel
            // 
            this.heightLabel.AutoSize = true;
            this.heightLabel.Location = new System.Drawing.Point(9, 123);
            this.heightLabel.Name = "heightLabel";
            this.heightLabel.Size = new System.Drawing.Size(38, 13);
            this.heightLabel.TabIndex = 8;
            this.heightLabel.Text = "Height";
            // 
            // heightTrackBar
            // 
            this.heightTrackBar.Enabled = false;
            this.heightTrackBar.Location = new System.Drawing.Point(39, 116);
            this.heightTrackBar.Name = "heightTrackBar";
            this.heightTrackBar.Size = new System.Drawing.Size(180, 45);
            this.heightTrackBar.TabIndex = 7;
            this.heightTrackBar.Scroll += new System.EventHandler(this.heightTrackBar_Scroll);
            // 
            // gainTextBox
            // 
            this.gainTextBox.Enabled = false;
            this.gainTextBox.Location = new System.Drawing.Point(216, 156);
            this.gainTextBox.Name = "gainTextBox";
            this.gainTextBox.Size = new System.Drawing.Size(60, 20);
            this.gainTextBox.TabIndex = 12;
            this.gainTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // gainLabel
            // 
            this.gainLabel.AutoSize = true;
            this.gainLabel.Location = new System.Drawing.Point(9, 160);
            this.gainLabel.Name = "gainLabel";
            this.gainLabel.Size = new System.Drawing.Size(29, 13);
            this.gainLabel.TabIndex = 11;
            this.gainLabel.Text = "Gain";
            // 
            // gainTrackBar
            // 
            this.gainTrackBar.Enabled = false;
            this.gainTrackBar.Location = new System.Drawing.Point(39, 153);
            this.gainTrackBar.Name = "gainTrackBar";
            this.gainTrackBar.Size = new System.Drawing.Size(180, 45);
            this.gainTrackBar.TabIndex = 10;
            this.gainTrackBar.Scroll += new System.EventHandler(this.gainTrackBar_Scroll);
            // 
            // startButton
            // 
            this.startButton.Enabled = false;
            this.startButton.Location = new System.Drawing.Point(348, 160);
            this.startButton.Name = "startButton";
            this.startButton.Size = new System.Drawing.Size(75, 23);
            this.startButton.TabIndex = 13;
            this.startButton.Text = "Start";
            this.startButton.UseVisualStyleBackColor = true;
            this.startButton.Click += new System.EventHandler(this.startButton_Click);
            // 
            // stopButton
            // 
            this.stopButton.Enabled = false;
            this.stopButton.Location = new System.Drawing.Point(348, 198);
            this.stopButton.Name = "stopButton";
            this.stopButton.Size = new System.Drawing.Size(75, 23);
            this.stopButton.TabIndex = 14;
            this.stopButton.Text = "Stop";
            this.stopButton.UseVisualStyleBackColor = true;
            this.stopButton.Click += new System.EventHandler(this.stopButton_Click);
            // 
            // wBalanceButton
            // 
            this.wBalanceButton.Enabled = false;
            this.wBalanceButton.Location = new System.Drawing.Point(348, 242);
            this.wBalanceButton.Name = "wBalanceButton";
            this.wBalanceButton.Size = new System.Drawing.Size(75, 34);
            this.wBalanceButton.TabIndex = 15;
            this.wBalanceButton.Text = "White Balance";
            this.wBalanceButton.UseVisualStyleBackColor = true;
            this.wBalanceButton.Click += new System.EventHandler(this.wBalanceButton_Click);
            // 
            // swTriggerButton
            // 
            this.swTriggerButton.Enabled = false;
            this.swTriggerButton.Location = new System.Drawing.Point(348, 297);
            this.swTriggerButton.Name = "swTriggerButton";
            this.swTriggerButton.Size = new System.Drawing.Size(75, 34);
            this.swTriggerButton.TabIndex = 16;
            this.swTriggerButton.Text = "Software Trigger";
            this.swTriggerButton.UseVisualStyleBackColor = true;
            this.swTriggerButton.Click += new System.EventHandler(this.swTriggerButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.cameraIdTextBox);
            this.groupBox1.Controls.Add(this.camListComboBox);
            this.groupBox1.Location = new System.Drawing.Point(10, 9);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(457, 120);
            this.groupBox1.TabIndex = 17;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Camera List and Camera ID";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.swTrigRadio);
            this.groupBox2.Controls.Add(this.freeRunRadio);
            this.groupBox2.Location = new System.Drawing.Point(12, 14);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(278, 50);
            this.groupBox2.TabIndex = 18;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Trigger Mode";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.groupBox2);
            this.groupBox3.Controls.Add(this.gainTextBox);
            this.groupBox3.Controls.Add(this.gainLabel);
            this.groupBox3.Controls.Add(this.gainTrackBar);
            this.groupBox3.Controls.Add(this.heightTextBox);
            this.groupBox3.Controls.Add(this.heightLabel);
            this.groupBox3.Controls.Add(this.heightTrackBar);
            this.groupBox3.Controls.Add(this.widthTextBox);
            this.groupBox3.Controls.Add(this.widthLabel);
            this.groupBox3.Controls.Add(this.widthTrackBar);
            this.groupBox3.Location = new System.Drawing.Point(9, 137);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(317, 204);
            this.groupBox3.TabIndex = 19;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Camera Settings";
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusBarLabel});
            this.statusStrip1.Location = new System.Drawing.Point(0, 354);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(477, 22);
            this.statusStrip1.TabIndex = 20;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // statusBarLabel
            // 
            this.statusBarLabel.Name = "statusBarLabel";
            this.statusBarLabel.Size = new System.Drawing.Size(0, 17);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(477, 376);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.swTriggerButton);
            this.Controls.Add(this.wBalanceButton);
            this.Controls.Add(this.stopButton);
            this.Controls.Add(this.startButton);
            this.Name = "Form1";
            this.Text = "CSharpSwTrigger";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.widthTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.heightTrackBar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.gainTrackBar)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox camListComboBox;
        private System.Windows.Forms.TextBox cameraIdTextBox;
        private System.Windows.Forms.RadioButton freeRunRadio;
        private System.Windows.Forms.RadioButton swTrigRadio;
        private System.Windows.Forms.TrackBar widthTrackBar;
        private System.Windows.Forms.Label widthLabel;
        private System.Windows.Forms.TextBox widthTextBox;
        private System.Windows.Forms.TextBox heightTextBox;
        private System.Windows.Forms.Label heightLabel;
        private System.Windows.Forms.TrackBar heightTrackBar;
        private System.Windows.Forms.TextBox gainTextBox;
        private System.Windows.Forms.Label gainLabel;
        private System.Windows.Forms.TrackBar gainTrackBar;
        private System.Windows.Forms.Button startButton;
        private System.Windows.Forms.Button stopButton;
        private System.Windows.Forms.Button wBalanceButton;
        private System.Windows.Forms.Button swTriggerButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel statusBarLabel;
    }
}

