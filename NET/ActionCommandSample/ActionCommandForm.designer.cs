namespace ActionCommandSample
{
    partial class ActionCommandForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ActionCommandForm));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupMaskTextBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.groupKeyTextBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.deviceKeyTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.requestAcknowledgeCheckBox = new System.Windows.Forms.CheckBox();
            this.acknowledgeCountNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.acknowledgeListBox = new System.Windows.Forms.ListBox();
            this.expectedAcknowledeCountLabel = new System.Windows.Forms.Label();
            this.sendActionCommandButton = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.acknowledgeCountNumericUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.groupMaskTextBox);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.groupKeyTextBox);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.deviceKeyTextBox);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(10, 5);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(154, 94);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Action Command Data";
            // 
            // groupMaskTextBox
            // 
            this.groupMaskTextBox.Location = new System.Drawing.Point(76, 67);
            this.groupMaskTextBox.Name = "groupMaskTextBox";
            this.groupMaskTextBox.Size = new System.Drawing.Size(70, 20);
            this.groupMaskTextBox.TabIndex = 4;
            this.groupMaskTextBox.Text = "0x00000000";
            this.groupMaskTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(5, 70);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(68, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "Group Mask:";
            // 
            // groupKeyTextBox
            // 
            this.groupKeyTextBox.Location = new System.Drawing.Point(76, 42);
            this.groupKeyTextBox.Name = "groupKeyTextBox";
            this.groupKeyTextBox.Size = new System.Drawing.Size(70, 20);
            this.groupKeyTextBox.TabIndex = 3;
            this.groupKeyTextBox.Text = "0x00000000";
            this.groupKeyTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(5, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Group Key:";
            // 
            // deviceKeyTextBox
            // 
            this.deviceKeyTextBox.Location = new System.Drawing.Point(76, 17);
            this.deviceKeyTextBox.Name = "deviceKeyTextBox";
            this.deviceKeyTextBox.Size = new System.Drawing.Size(70, 20);
            this.deviceKeyTextBox.TabIndex = 2;
            this.deviceKeyTextBox.Text = "0x00000000";
            this.deviceKeyTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(5, 20);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Device Key:";
            // 
            // requestAcknowledgeCheckBox
            // 
            this.requestAcknowledgeCheckBox.AutoSize = true;
            this.requestAcknowledgeCheckBox.Location = new System.Drawing.Point(174, 11);
            this.requestAcknowledgeCheckBox.Name = "requestAcknowledgeCheckBox";
            this.requestAcknowledgeCheckBox.Size = new System.Drawing.Size(134, 17);
            this.requestAcknowledgeCheckBox.TabIndex = 5;
            this.requestAcknowledgeCheckBox.Text = "Request Acknowledge";
            this.requestAcknowledgeCheckBox.UseVisualStyleBackColor = true;
            this.requestAcknowledgeCheckBox.CheckedChanged += new System.EventHandler(this.requestAcknowledgeCheckBox_CheckedChanged);
            // 
            // acknowledgeCountNumericUpDown
            // 
            this.acknowledgeCountNumericUpDown.Location = new System.Drawing.Point(260, 29);
            this.acknowledgeCountNumericUpDown.Name = "acknowledgeCountNumericUpDown";
            this.acknowledgeCountNumericUpDown.Size = new System.Drawing.Size(53, 20);
            this.acknowledgeCountNumericUpDown.TabIndex = 6;
            this.acknowledgeCountNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.acknowledgeCountNumericUpDown.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.acknowledgeCountNumericUpDown.ValueChanged += new System.EventHandler(this.acknowledgeCountNumericUpDown_ValueChanged);
            // 
            // acknowledgeListBox
            // 
            this.acknowledgeListBox.Location = new System.Drawing.Point(173, 55);
            this.acknowledgeListBox.Name = "acknowledgeListBox";
            this.acknowledgeListBox.SelectionMode = System.Windows.Forms.SelectionMode.None;
            this.acknowledgeListBox.Size = new System.Drawing.Size(140, 43);
            this.acknowledgeListBox.TabIndex = 9;
            // 
            // expectedAcknowledeCountLabel
            // 
            this.expectedAcknowledeCountLabel.AutoSize = true;
            this.expectedAcknowledeCountLabel.Location = new System.Drawing.Point(172, 32);
            this.expectedAcknowledeCountLabel.Name = "expectedAcknowledeCountLabel";
            this.expectedAcknowledeCountLabel.Size = new System.Drawing.Size(86, 13);
            this.expectedAcknowledeCountLabel.TabIndex = 10;
            this.expectedAcknowledeCountLabel.Text = "Expected Count:";
            // 
            // sendActionCommandButton
            // 
            this.sendActionCommandButton.Image = global::ActionCommandSample.Properties.Resources.thunderbolt;
            this.sendActionCommandButton.Location = new System.Drawing.Point(322, 9);
            this.sendActionCommandButton.Name = "sendActionCommandButton";
            this.sendActionCommandButton.Size = new System.Drawing.Size(90, 90);
            this.sendActionCommandButton.TabIndex = 1;
            this.sendActionCommandButton.Text = "Send Action Command";
            this.sendActionCommandButton.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageAboveText;
            this.sendActionCommandButton.UseVisualStyleBackColor = true;
            this.sendActionCommandButton.Click += new System.EventHandler(this.sendActionCommandButton_Click);
            // 
            // ActionCommandForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(420, 107);
            this.Controls.Add(this.expectedAcknowledeCountLabel);
            this.Controls.Add(this.acknowledgeListBox);
            this.Controls.Add(this.acknowledgeCountNumericUpDown);
            this.Controls.Add(this.requestAcknowledgeCheckBox);
            this.Controls.Add(this.sendActionCommandButton);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "ActionCommandForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Send Action Command";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.acknowledgeCountNumericUpDown)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox deviceKeyTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox groupKeyTextBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox groupMaskTextBox;
        private System.Windows.Forms.Button sendActionCommandButton;
        private System.Windows.Forms.CheckBox requestAcknowledgeCheckBox;
        private System.Windows.Forms.NumericUpDown acknowledgeCountNumericUpDown;
        private System.Windows.Forms.ListBox acknowledgeListBox;
        private System.Windows.Forms.Label expectedAcknowledeCountLabel;
    }
}