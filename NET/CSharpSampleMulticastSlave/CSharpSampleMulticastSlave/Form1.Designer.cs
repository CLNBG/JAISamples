namespace CSharpSampleMulticastSlave
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
      this.label1 = new System.Windows.Forms.Label();
      this.buttonStart = new System.Windows.Forms.Button();
      this.buttonStop = new System.Windows.Forms.Button();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.label2 = new System.Windows.Forms.Label();
      this.textIpAddress = new rj2_cs.Forms.IPAddressTextBox();
      this.groupBox1.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.SuspendLayout();
      // 
      // label1
      // 
      this.label1.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
      this.label1.Location = new System.Drawing.Point(8, 17);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(415, 65);
      this.label1.TabIndex = 0;
      this.label1.Text = "label1";
      // 
      // buttonStart
      // 
      this.buttonStart.Enabled = false;
      this.buttonStart.Location = new System.Drawing.Point(356, 125);
      this.buttonStart.Name = "buttonStart";
      this.buttonStart.Size = new System.Drawing.Size(86, 25);
      this.buttonStart.TabIndex = 1;
      this.buttonStart.Text = "Start Acq.";
      this.buttonStart.UseVisualStyleBackColor = true;
      this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
      // 
      // buttonStop
      // 
      this.buttonStop.Enabled = false;
      this.buttonStop.Location = new System.Drawing.Point(356, 156);
      this.buttonStop.Name = "buttonStop";
      this.buttonStop.Size = new System.Drawing.Size(86, 25);
      this.buttonStop.TabIndex = 2;
      this.buttonStop.Text = "Stop Acq.";
      this.buttonStop.UseVisualStyleBackColor = true;
      this.buttonStop.Click += new System.EventHandler(this.buttonStop_Click);
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.label1);
      this.groupBox1.Location = new System.Drawing.Point(10, 10);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(430, 93);
      this.groupBox1.TabIndex = 7;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "ID of the first camera found";
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.textIpAddress);
      this.groupBox2.Controls.Add(this.label2);
      this.groupBox2.Location = new System.Drawing.Point(10, 111);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(338, 106);
      this.groupBox2.TabIndex = 8;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Camara Settings (Read Only)";
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
      // textIpAddress
      // 
      this.textIpAddress.Enabled = false;
      this.textIpAddress.Location = new System.Drawing.Point(40, 56);
      this.textIpAddress.Name = "textIpAddress";
      this.textIpAddress.Size = new System.Drawing.Size(110, 19);
      this.textIpAddress.TabIndex = 1;
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(454, 229);
      this.Controls.Add(this.groupBox2);
      this.Controls.Add(this.groupBox1);
      this.Controls.Add(this.buttonStop);
      this.Controls.Add(this.buttonStart);
      this.Name = "Form1";
      this.Text = "C# Sample Multicast Slave";
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
      this.groupBox1.ResumeLayout(false);
      this.groupBox2.ResumeLayout(false);
      this.groupBox2.PerformLayout();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button buttonStart;
    private System.Windows.Forms.Button buttonStop;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.Label label2;
    private rj2_cs.Forms.IPAddressTextBox textIpAddress;
  }
}

