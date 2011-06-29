using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace InputServer
{
    public partial class InputServerForm : Form
    {
        public InputServerForm()
        {
            InitializeComponent();
            // Set the BEP logo on the form
            Icon = Icon.FromHandle(InputServer.Properties.Resources.BepLogo.GetHicon());
            // Connect to the database to load input devices

        }

        /// <summary>
        /// Exit the application when the user selects the exit menu item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Environment.Exit(0);
        }

        /// <summary>
        /// Display the input device configuration form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void addInputDeviceToolStripMenuItem_Click(object sender, EventArgs e)
        {
            InputDeviceForm inputDeviceForm = new InputDeviceForm();
            inputDeviceForm.ShowDialog();
        }

        
    }
}
