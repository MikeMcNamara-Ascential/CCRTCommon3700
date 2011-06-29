using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace BomFileProcessor
{
    public partial class AddOptionForm : Form
    {
        public AddOptionForm(String title)
        {
            InitializeComponent();
            Text = title;
        }


        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Add a new option.
        /// </summary>
        /// <returns>Option that was added by the user.</returns>
        public VehicleOption AddNewOption()
        {
            ShowDialog();
            return m_option;
        }

        
        //-----------------------------------------------------------------------------------------
        // Callbacks
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Store the data from the user and close the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_okButton_Click(object sender, EventArgs e)
        {   // Make sure all the data was stored
            if ((m_modelCodeTextBox.Text.Length > 0) && (m_valueTextBox.Text.Length > 0))
            {
                m_option = new VehicleOption(m_modelCodeTextBox.Text, m_valueTextBox.Text);
            }
            Close();
        }


        //-----------------------------------------------------------------------------------------
        // Member Variable and Properties
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Option association setup by the user.
        /// </summary>
        private VehicleOption m_option;

    }
}
