using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Text;
using System.Windows.Forms;

namespace MaintenanceReminder
{
    public partial class IpAddressForm : Form
    {
        // ----------------------------------------------------------------------------------------
        // Member Functions and Methods
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// </summary>
        public IpAddressForm(Localizer localizer)
        {
            InitializeComponent();
            // Set the current IP Address in the form
            m_ipAddressTextBox.Text = MaintenanceReminder.Properties.Settings.Default.QnxIpAddress;
            m_errorLabel.Visible = false;
            StringLocalizer = localizer;
            // Localize the text in the form
            Text = StringLocalizer.GetLocalizedString("QnxIpAddressMenuItem");
            m_ipAddressLabel.Text = StringLocalizer.GetLocalizedString("QnxIpAddressLabel");
            m_cancelButton.Text = StringLocalizer.GetLocalizedString("CloseLabel");
            m_saveButton.Text = StringLocalizer.GetLocalizedString("SaveLabel");
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
                m_errorLabel.Text = StringLocalizer.GetLocalizedString("InvalidIpFormat");
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

        /// <summary>
        /// Localizer to use for displaying localized text in the form.
        /// </summary>
        Localizer StringLocalizer { get; set; }
        
        
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
            if (ValidateIpAddress(m_ipAddressTextBox.Text))
            {   // Save the IP Address
                MaintenanceReminder.Properties.Settings.Default.QnxIpAddress = m_ipAddressTextBox.Text;
                MaintenanceReminder.Properties.Settings.Default.Save();
            }
        }
    }
}
