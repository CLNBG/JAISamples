namespace ActionCommandSample
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.StopButton = new System.Windows.Forms.Button();
            this.StartButton = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.SearchButton = new System.Windows.Forms.Button();
            this.CameraIDTextBox = new System.Windows.Forms.TextBox();
            this.actionCommandGroupBox = new System.Windows.Forms.GroupBox();
            this.openManualActionCommandButton = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.timestampTextBox1 = new System.Windows.Forms.TextBox();
            this.action2Button = new System.Windows.Forms.Button();
            this.action1Button = new System.Windows.Forms.Button();
            this.infoTextBox = new System.Windows.Forms.TextBox();
            this.setupActionsButton = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.timestampTextBox2 = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.actionCommandGroupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // StopButton
            // 
            this.StopButton.Enabled = false;
            this.StopButton.Location = new System.Drawing.Point(129, 150);
            this.StopButton.Name = "StopButton";
            this.StopButton.Size = new System.Drawing.Size(100, 23);
            this.StopButton.TabIndex = 8;
            this.StopButton.Text = "Stop Acquisition";
            this.StopButton.UseVisualStyleBackColor = true;
            this.StopButton.Click += new System.EventHandler(this.StopButton_Click);
            // 
            // StartButton
            // 
            this.StartButton.Enabled = false;
            this.StartButton.Location = new System.Drawing.Point(16, 150);
            this.StartButton.Name = "StartButton";
            this.StartButton.Size = new System.Drawing.Size(100, 23);
            this.StartButton.TabIndex = 7;
            this.StartButton.Text = "Start Acquisition";
            this.StartButton.UseVisualStyleBackColor = true;
            this.StartButton.Click += new System.EventHandler(this.StartButton_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.SearchButton);
            this.groupBox1.Controls.Add(this.CameraIDTextBox);
            this.groupBox1.Location = new System.Drawing.Point(6, 7);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(523, 128);
            this.groupBox1.TabIndex = 6;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "ID of the first 2 cameras found";
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
            this.CameraIDTextBox.Size = new System.Drawing.Size(476, 102);
            this.CameraIDTextBox.TabIndex = 0;
            // 
            // actionCommandGroupBox
            // 
            this.actionCommandGroupBox.Controls.Add(this.timestampTextBox2);
            this.actionCommandGroupBox.Controls.Add(this.openManualActionCommandButton);
            this.actionCommandGroupBox.Controls.Add(this.label3);
            this.actionCommandGroupBox.Controls.Add(this.timestampTextBox1);
            this.actionCommandGroupBox.Controls.Add(this.action2Button);
            this.actionCommandGroupBox.Controls.Add(this.action1Button);
            this.actionCommandGroupBox.Controls.Add(this.infoTextBox);
            this.actionCommandGroupBox.Controls.Add(this.setupActionsButton);
            this.actionCommandGroupBox.Location = new System.Drawing.Point(6, 182);
            this.actionCommandGroupBox.Name = "actionCommandGroupBox";
            this.actionCommandGroupBox.Size = new System.Drawing.Size(525, 194);
            this.actionCommandGroupBox.TabIndex = 11;
            this.actionCommandGroupBox.TabStop = false;
            this.actionCommandGroupBox.Text = "Action Command Test";
            // 
            // openManualActionCommandButton
            // 
            this.openManualActionCommandButton.Location = new System.Drawing.Point(309, 111);
            this.openManualActionCommandButton.Name = "openManualActionCommandButton";
            this.openManualActionCommandButton.Size = new System.Drawing.Size(208, 23);
            this.openManualActionCommandButton.TabIndex = 12;
            this.openManualActionCommandButton.Text = "Open \'Send Action Command\' Dialog";
            this.openManualActionCommandButton.UseVisualStyleBackColor = true;
            this.openManualActionCommandButton.Click += new System.EventHandler(this.openManualActionCommandButton_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(309, 139);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(61, 13);
            this.label3.TabIndex = 5;
            this.label3.Text = "Timestamp:";
            // 
            // timestampTextBox1
            // 
            this.timestampTextBox1.Location = new System.Drawing.Point(374, 138);
            this.timestampTextBox1.Name = "timestampTextBox1";
            this.timestampTextBox1.ReadOnly = true;
            this.timestampTextBox1.Size = new System.Drawing.Size(141, 20);
            this.timestampTextBox1.TabIndex = 4;
            this.timestampTextBox1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // action2Button
            // 
            this.action2Button.Location = new System.Drawing.Point(10, 138);
            this.action2Button.Name = "action2Button";
            this.action2Button.Size = new System.Drawing.Size(170, 50);
            this.action2Button.TabIndex = 3;
            this.action2Button.Text = "Send Action 2 (Timestamp reset)";
            this.action2Button.UseVisualStyleBackColor = true;
            this.action2Button.Click += new System.EventHandler(this.action2Button_Click);
            // 
            // action1Button
            // 
            this.action1Button.Location = new System.Drawing.Point(10, 82);
            this.action1Button.Name = "action1Button";
            this.action1Button.Size = new System.Drawing.Size(170, 50);
            this.action1Button.TabIndex = 2;
            this.action1Button.Text = "Send Action 1 (Trigger)";
            this.action1Button.UseVisualStyleBackColor = true;
            this.action1Button.Click += new System.EventHandler(this.action1Button_Click);
            // 
            // infoTextBox
            // 
            this.infoTextBox.Location = new System.Drawing.Point(10, 17);
            this.infoTextBox.Multiline = true;
            this.infoTextBox.Name = "infoTextBox";
            this.infoTextBox.ReadOnly = true;
            this.infoTextBox.Size = new System.Drawing.Size(505, 59);
            this.infoTextBox.TabIndex = 1;
            this.infoTextBox.Text = resources.GetString("infoTextBox.Text");
            // 
            // setupActionsButton
            // 
            this.setupActionsButton.Location = new System.Drawing.Point(309, 82);
            this.setupActionsButton.Name = "setupActionsButton";
            this.setupActionsButton.Size = new System.Drawing.Size(208, 23);
            this.setupActionsButton.TabIndex = 0;
            this.setupActionsButton.Text = "Set-up Camera";
            this.setupActionsButton.UseVisualStyleBackColor = true;
            this.setupActionsButton.Click += new System.EventHandler(this.setupActionsButton_Click);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // timestampTextBox2
            // 
            this.timestampTextBox2.Location = new System.Drawing.Point(374, 165);
            this.timestampTextBox2.Name = "timestampTextBox2";
            this.timestampTextBox2.ReadOnly = true;
            this.timestampTextBox2.Size = new System.Drawing.Size(141, 20);
            this.timestampTextBox2.TabIndex = 13;
            this.timestampTextBox2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(543, 379);
            this.Controls.Add(this.actionCommandGroupBox);
            this.Controls.Add(this.StopButton);
            this.Controls.Add(this.StartButton);
            this.Controls.Add(this.groupBox1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.Text = "Action Command Sample";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.actionCommandGroupBox.ResumeLayout(false);
            this.actionCommandGroupBox.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button StopButton;
        private System.Windows.Forms.Button StartButton;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button SearchButton;
        private System.Windows.Forms.TextBox CameraIDTextBox;
        private System.Windows.Forms.GroupBox actionCommandGroupBox;
        private System.Windows.Forms.TextBox infoTextBox;
        private System.Windows.Forms.Button setupActionsButton;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox timestampTextBox1;
        private System.Windows.Forms.Button action2Button;
        private System.Windows.Forms.Button action1Button;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button openManualActionCommandButton;
        private System.Windows.Forms.TextBox timestampTextBox2;
    }
}

