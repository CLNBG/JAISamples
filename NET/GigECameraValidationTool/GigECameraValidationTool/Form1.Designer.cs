namespace GigECameraValidationTool
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
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.cameraIdTextBox = new System.Windows.Forms.TextBox();
            this.camListComboBox = new System.Windows.Forms.ComboBox();
            this.statusStrip1 = new System.Windows.Forms.StatusStrip();
            this.statusBarLabel = new System.Windows.Forms.ToolStripStatusLabel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.lblFloatCount = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.lblEnumCount = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.btnDisplayResult = new System.Windows.Forms.Button();
            this.lblIntCount = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.btnGenICamXMLTest = new System.Windows.Forms.Button();
            this.groupBox2.SuspendLayout();
            this.statusStrip1.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.cameraIdTextBox);
            this.groupBox2.Controls.Add(this.camListComboBox);
            this.groupBox2.Location = new System.Drawing.Point(8, 5);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(456, 110);
            this.groupBox2.TabIndex = 25;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Camera List and Camera ID";
            // 
            // cameraIdTextBox
            // 
            this.cameraIdTextBox.BackColor = System.Drawing.SystemColors.Control;
            this.cameraIdTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.cameraIdTextBox.Location = new System.Drawing.Point(12, 45);
            this.cameraIdTextBox.Multiline = true;
            this.cameraIdTextBox.Name = "cameraIdTextBox";
            this.cameraIdTextBox.Size = new System.Drawing.Size(433, 55);
            this.cameraIdTextBox.TabIndex = 9;
            // 
            // camListComboBox
            // 
            this.camListComboBox.FormattingEnabled = true;
            this.camListComboBox.Location = new System.Drawing.Point(12, 19);
            this.camListComboBox.Name = "camListComboBox";
            this.camListComboBox.Size = new System.Drawing.Size(196, 21);
            this.camListComboBox.TabIndex = 8;
            this.camListComboBox.SelectedIndexChanged += new System.EventHandler(this.camListComboBox_SelectedIndexChanged);
            // 
            // statusStrip1
            // 
            this.statusStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.statusBarLabel});
            this.statusStrip1.Location = new System.Drawing.Point(0, 244);
            this.statusStrip1.Name = "statusStrip1";
            this.statusStrip1.Size = new System.Drawing.Size(477, 22);
            this.statusStrip1.TabIndex = 24;
            this.statusStrip1.Text = "statusStrip1";
            // 
            // statusBarLabel
            // 
            this.statusBarLabel.Name = "statusBarLabel";
            this.statusBarLabel.Size = new System.Drawing.Size(0, 17);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.progressBar1);
            this.groupBox1.Controls.Add(this.lblFloatCount);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.lblEnumCount);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.btnDisplayResult);
            this.groupBox1.Controls.Add(this.lblIntCount);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.btnGenICamXMLTest);
            this.groupBox1.Location = new System.Drawing.Point(8, 121);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(456, 114);
            this.groupBox1.TabIndex = 23;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Gen<i>Cam XML Test";
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(233, 90);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(213, 20);
            this.progressBar1.TabIndex = 13;
            // 
            // lblFloatCount
            // 
            this.lblFloatCount.AutoSize = true;
            this.lblFloatCount.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.lblFloatCount.ForeColor = System.Drawing.Color.Red;
            this.lblFloatCount.Location = new System.Drawing.Point(162, 88);
            this.lblFloatCount.Name = "lblFloatCount";
            this.lblFloatCount.Size = new System.Drawing.Size(0, 13);
            this.lblFloatCount.TabIndex = 12;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(6, 88);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(117, 13);
            this.label1.TabIndex = 11;
            this.label1.Text = "Number of IFloat errors:";
            // 
            // lblEnumCount
            // 
            this.lblEnumCount.AutoSize = true;
            this.lblEnumCount.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblEnumCount.ForeColor = System.Drawing.Color.Red;
            this.lblEnumCount.Location = new System.Drawing.Point(162, 63);
            this.lblEnumCount.Name = "lblEnumCount";
            this.lblEnumCount.Size = new System.Drawing.Size(0, 13);
            this.lblEnumCount.TabIndex = 10;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 63);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(156, 13);
            this.label3.TabIndex = 9;
            this.label3.Text = "Number of IEnumeration errors: ";
            // 
            // btnDisplayResult
            // 
            this.btnDisplayResult.Location = new System.Drawing.Point(266, 58);
            this.btnDisplayResult.Name = "btnDisplayResult";
            this.btnDisplayResult.Size = new System.Drawing.Size(168, 23);
            this.btnDisplayResult.TabIndex = 8;
            this.btnDisplayResult.Text = "Display errors";
            this.btnDisplayResult.UseVisualStyleBackColor = true;
            this.btnDisplayResult.Click += new System.EventHandler(this.btnDisplayResult_Click);
            // 
            // lblIntCount
            // 
            this.lblIntCount.AutoSize = true;
            this.lblIntCount.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblIntCount.ForeColor = System.Drawing.Color.Red;
            this.lblIntCount.Location = new System.Drawing.Point(162, 35);
            this.lblIntCount.Name = "lblIntCount";
            this.lblIntCount.Size = new System.Drawing.Size(0, 13);
            this.lblIntCount.TabIndex = 7;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(6, 35);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(130, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Number of IInteger errors: ";
            // 
            // btnGenICamXMLTest
            // 
            this.btnGenICamXMLTest.Location = new System.Drawing.Point(266, 30);
            this.btnGenICamXMLTest.Name = "btnGenICamXMLTest";
            this.btnGenICamXMLTest.Size = new System.Drawing.Size(168, 23);
            this.btnGenICamXMLTest.TabIndex = 5;
            this.btnGenICamXMLTest.Text = "Run Gen<i>Cam XML Test";
            this.btnGenICamXMLTest.UseVisualStyleBackColor = true;
            this.btnGenICamXMLTest.Click += new System.EventHandler(this.btnGenICamXMLTest_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(477, 266);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.statusStrip1);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.statusStrip1.ResumeLayout(false);
            this.statusStrip1.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox cameraIdTextBox;
        private System.Windows.Forms.ComboBox camListComboBox;
        private System.Windows.Forms.StatusStrip statusStrip1;
        private System.Windows.Forms.ToolStripStatusLabel statusBarLabel;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Label lblFloatCount;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lblEnumCount;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btnDisplayResult;
        private System.Windows.Forms.Label lblIntCount;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnGenICamXMLTest;
    }
}

