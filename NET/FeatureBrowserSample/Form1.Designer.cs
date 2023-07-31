namespace FeatureBrowserSample
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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.NodeTreeView = new System.Windows.Forms.TreeView();
            this.NodePropertyGrid = new System.Windows.Forms.PropertyGrid();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.NodeTreeView);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.NodePropertyGrid);
            this.splitContainer1.Size = new System.Drawing.Size(694, 375);
            this.splitContainer1.SplitterDistance = 230;
            this.splitContainer1.TabIndex = 0;
            // 
            // NodeTreeView
            // 
            this.NodeTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.NodeTreeView.Location = new System.Drawing.Point(0, 0);
            this.NodeTreeView.Name = "NodeTreeView";
            this.NodeTreeView.Size = new System.Drawing.Size(230, 375);
            this.NodeTreeView.TabIndex = 1;
            this.NodeTreeView.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.NodeTreeView_AfterSelect);
            // 
            // NodePropertyGrid
            // 
            this.NodePropertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.NodePropertyGrid.Location = new System.Drawing.Point(0, 0);
            this.NodePropertyGrid.Name = "NodePropertyGrid";
            this.NodePropertyGrid.Size = new System.Drawing.Size(460, 375);
            this.NodePropertyGrid.TabIndex = 0;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(694, 375);
            this.Controls.Add(this.splitContainer1);
            this.Name = "Form1";
            this.Text = "Feature Browser Sample";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TreeView NodeTreeView;
        private System.Windows.Forms.PropertyGrid NodePropertyGrid;
    }
}

