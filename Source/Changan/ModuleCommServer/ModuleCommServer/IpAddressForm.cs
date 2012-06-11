using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Text;
using System.Windows.Forms;

namespace ModuleCommServer
{
    public partial class IpAddressForm : Form
    {
        // ----------------------------------------------------------------------------------------
        // Class Functions and Methods
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// </summary>
        public IpAddressForm()
        {
            InitializeComponent();
            // Set the current IP Address in the form
            m_ipAddressTextBox.Text = ModuleCommServer.Properties.Settings.Default.QnxIpAddress;
            m_winCcrtIpAddressTextBox.Text = ModuleCommServer.Properties.Settings.Default.WindowsIpAddress;
            m_errorLabel.Visible = false;
        }

        /// <summary>
        /// Verify if the specified IP Address is in a valid format.
        /// If the IP Address is not in a valid format, an error message will be displayed in the form.
        /// </summary>
        /// <param name="ipAddress">IP Address to validate.</param>
        /// <returns>Flag indicating if the IP Address is ina valid format.</returns>
        private Boolean ValidateIpAddress(String ipAddress)
        {
            Boolean ipValid = false;
            IPAddress address;
            if (!IPAddress.TryParse(ipAddress, out address))
            {   // Display an error message
                m_errorLabel.Text = "Invalid IP Address Format";
                m_errorLabel.Visible = true;
            }
            else
            {
                m_errorLabel.Visible = false;
                ipValid = true;
            }
            return ipValid;
        }


        // ----------------------------------------------------------------------------------------
        // Member Variables and Properties
        // ----------------------------------------------------------------------------------------

        
        // ----------------------------------------------------------------------------------------
        // Class Callbacks
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Save the newly entered IP Address.
        /// The IP Address will be validated to ensure it is in the correct format.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {   // Make sure the IP Address is valid
            if (ValidateIpAddress(m_ipAddressTextBox.Text) && ValidateIpAddress(m_winCcrtIpAddressTextBox.Text))
            {   // Save the IP Address
                ModuleCommServer.Properties.Settings.Default.QnxIpAddress = m_ipAddressTextBox.Text;
                ModuleCommServer.Properties.Settings.Default.WindowsIpAddress = m_winCcrtIpAddressTextBox.Text;
                ModuleCommServer.Properties.Settings.Default.Save();
            }
        }
    }
}
