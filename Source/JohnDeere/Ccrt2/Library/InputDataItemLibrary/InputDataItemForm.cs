using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace InputDataItemLibrary
{
    public partial class InputDataItemForm : Form
    {
        public InputDataItemForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Load the provided settings into the form and then display the form.
        /// </summary>
        /// <param name="settings">Data item settings to display in the form.</param>
        public void DisplaySettings(DataItemSettings settings)
        {
            m_dataItemNameTextBox.Text = settings.DataName;
            m_minLenghtTextBox.Text = settings.MinDataLength.ToString();
            m_maxLengthTextBox.Text = settings.MaxDataLength.ToString();
            if (Visible)
            {   // Form already displayed, just bring it to the front
                BringToFront();
            }
            else
            {   // Need to display the form
                Show(); 
            }
        }
    }
}
