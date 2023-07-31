using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Jai_FactoryDotNET;

namespace FeatureBrowserSample
{
    public partial class Form1 : Form
    {
        CFactory myFactory = new CFactory();
        public Form1()
        {
            InitializeComponent();

            // Open the factory
            if (myFactory.Open("") == Jai_FactoryWrapper.EFactoryError.Success)
            {
                // Discover GigE and/or generic GenTL devices using myFactory.UpdateCameraList(in this case specifying Filter Driver for GigE cameras).
                myFactory.UpdateCameraList(Jai_FactoryDotNET.CFactory.EDriverType.FilterDriver);

                if (myFactory.CameraList.Count > 0)
                {
                    // OK - we managed to open the Factory. Now we need to display all the cameras we have found (if any)
                    PopulateTreeView();
                }
                else
                {
                    // Show a message telling about the problems
                    MessageBox.Show(this, "No cameras found!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                // Show a message telling about the problems
                MessageBox.Show("Unable to open factory!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void PopulateTreeView()
        {
            // Clear the tree
            NodeTreeView.Nodes.Clear();

            if (myFactory.CameraList.Count > 0)
            {
                foreach (CCamera c in myFactory.CameraList)
                {
                    // Open camera
                    if (c.Open() == Jai_FactoryWrapper.EFactoryError.Success)
                    {
                        // Add the camera to the Root of the tree
                        TreeNode cameraNode = new TreeNode(c.ModelName, CreateNodeTree(c, "Root"));
                        cameraNode.Tag = c;
                        NodeTreeView.Nodes.Add(cameraNode);
                    }
                }
            }
        }

        private TreeNode[] CreateNodeTree(CCamera c, string p)
        {
            List<CNode> features = c.GetSubFeatures(p);

            TreeNode[] childNodes = new TreeNode[features.Count];

            int index = 0;
            foreach (CNode n in features)
            {
                if (n.NodeType == Jai_FactoryWrapper.EConfNodeType.ICategory)
                    childNodes[index] = new TreeNode(n.DisplayName, CreateNodeTree(c, n.Name));
                else
                    childNodes[index] = new TreeNode(n.DisplayName);
                childNodes[index].Tag = n;
                index++;
            }

            return childNodes;
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void NodeTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (e.Node.Tag is CNode)
                NodePropertyGrid.SelectedObject = e.Node.Tag as CNode;
            else if (e.Node.Tag is CCamera)
                NodePropertyGrid.SelectedObject = e.Node.Tag as CCamera;
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (myFactory != null)
                myFactory.Close();
        }
    }
}