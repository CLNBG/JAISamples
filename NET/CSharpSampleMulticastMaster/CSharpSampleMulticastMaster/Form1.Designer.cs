namespace CSharpSampleMulticastMaster
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
      this.buttonStop = new System.Windows.Forms.Button();
      this.buttonStart = new System.Windows.Forms.Button();
      this.label1 = new System.Windows.Forms.Label();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.textIpAddress = new rj2_cs.Forms.IPAddressTextBox();
      this.label2 = new System.Windows.Forms.Label();
      this.buttonOpenCamera = new System.Windows.Forms.Button();
      this.buttonCloseCamera = new System.Windows.Forms.Button();
      this.buttonSearchCameras = new System.Windows.Forms.Button();
      this.groupBox1.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.SuspendLayout();
      // 
      // buttonStop
      // 
      this.buttonStop.Enabled = false;
      this.buttonStop.Location = new System.Drawing.Point(375, 190);
      this.buttonStop.Name = "buttonStop";
      this.buttonStop.Size = new System.Drawing.Size(86, 25);
      this.buttonStop.TabIndex = 4;
      this.buttonStop.Text = "Stop Acq.";
      this.buttonStop.UseVisualStyleBackColor = true;
      this.buttonStop.Click += new System.EventHandler(this.buttonStop_Click);
      // 
      // buttonStart
      // 
      this.buttonStart.Enabled = false;
      this.buttonStart.Location = new System.Drawing.Point(375, 163);
      this.buttonStart.Name = "buttonStart";
      this.buttonStart.Size = new System.Drawing.Size(86, 25);
      this.buttonStart.TabIndex = 3;
      this.buttonStart.Text = "Start Acq.";
      this.buttonStart.UseVisualStyleBackColor = true;
      this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
      // 
      // label1
      // 
      this.label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.label1.Location = new System.Drawing.Point(9, 18);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(402, 65);
      this.label1.TabIndex = 5;
      this.label1.Text = "label1";
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.label1);
      this.groupBox1.Location = new System.Drawing.Point(10, 10);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(451, 93);
      this.groupBox1.TabIndex = 6;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "ID of the first camera found";
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.textIpAddress);
      this.groupBox2.Controls.Add(this.label2);
      this.groupBox2.Location = new System.Drawing.Point(10, 109);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(347, 106);
      this.groupBox2.TabIndex = 7;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Camara Settings";
      // 
      // textIpAddress
      // 
      this.textIpAddress.Location = new System.Drawing.Point(42, 57);
      this.textIpAddress.Name = "textIpAddress";
      this.textIpAddress.Size = new System.Drawing.Size(110, 19);
      this.textIpAddress.TabIndex = 1;
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(11, 20);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(312, 24);
      this.label2.TabIndex = 0;
      this.label2.Text = "Destination IP address of streaming\r\n  (Must be from 224.0.1.0 to 239.255.255.255" +
          " for muticasting)\r\n";
      // 
      // buttonOpenCamera
      // 
      this.buttonOpenCamera.Enabled = false;
      this.buttonOpenCamera.Location = new System.Drawing.Point(375, 109);
      this.buttonOpenCamera.Name = "buttonOpenCamera";
      this.buttonOpenCamera.Size = new System.Drawing.Size(86, 25);
      this.buttonOpenCamera.TabIndex = 8;
      this.buttonOpenCamera.Text = "Open Camera";
      this.buttonOpenCamera.UseVisualStyleBackColor = true;
      this.buttonOpenCamera.Click += new System.EventHandler(this.buttonOpenCamera_Click);
      // 
      // buttonCloseCamera
      // 
      this.buttonCloseCamera.Enabled = false;
      this.buttonCloseCamera.Location = new System.Drawing.Point(375, 136);
      this.buttonCloseCamera.Name = "buttonCloseCamera";
      this.buttonCloseCamera.Size = new System.Drawing.Size(86, 25);
      this.buttonCloseCamera.TabIndex = 9;
      this.buttonCloseCamera.Text = "Close Camera";
      this.buttonCloseCamera.UseVisualStyleBackColor = true;
      this.buttonCloseCamera.Click += new System.EventHandler(this.buttonCloseCamera_Click);
      // 
      // buttonSearchCameras
      // 
      this.buttonSearchCameras.Location = new System.Drawing.Point(427, 28);
      this.buttonSearchCameras.Name = "buttonSearchCameras";
      this.buttonSearchCameras.Size = new System.Drawing.Size(25, 23);
      this.buttonSearchCameras.TabIndex = 10;
      this.buttonSearchCameras.Text = "...";
      this.buttonSearchCameras.UseVisualStyleBackColor = true;
      this.buttonSearchCameras.Click += new System.EventHandler(this.buttonSearchCameras_Click);
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(474, 225);
      this.Controls.Add(this.buttonSearchCameras);
      this.Controls.Add(this.buttonCloseCamera);
      this.Controls.Add(this.buttonOpenCamera);
      this.Controls.Add(this.groupBox2);
      this.Controls.Add(this.groupBox1);
      this.Controls.Add(this.buttonStop);
      this.Controls.Add(this.buttonStart);
      this.Name = "Form1";
      this.Text = "C# Sample Multicast Master";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
      this.groupBox1.ResumeLayout(false);
      this.groupBox2.ResumeLayout(false);
      this.groupBox2.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Button buttonStop;
    private System.Windows.Forms.Button buttonStart;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.Label label2;
    private rj2_cs.Forms.IPAddressTextBox textIpAddress;
    private System.Windows.Forms.Button buttonOpenCamera;
    private System.Windows.Forms.Button buttonCloseCamera;
    private System.Windows.Forms.Button buttonSearchCameras;
  }
}

