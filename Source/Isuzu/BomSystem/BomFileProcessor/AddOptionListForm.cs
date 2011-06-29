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
    public partial class AddOptionListForm : Form
    {
        public AddOptionListForm(List<String> items, String title)
        {
            InitializeComponent();
            m_option = null;
            // Populate the drop down list
            PopulateList(items);
            Text = title;
        }


        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Populate the combo box with the list of valid option values.
        /// </summary>
        /// <param name="items">List of valid option values.</param>
        private void PopulateList(List<String> items)
        {   // Clear the list first
            m_optionListComboBox.Items.Clear();
            // Add the new items
            m_optionListComboBox.Items.AddRange(items.ToArray());
            m_optionListComboBox.SelectedIndex = 0;
        }

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
        /// Save the new association data.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {   // Make sure the user added all the data
            if (m_modelCodeTextBox.Text.Length > 0)
            {
                m_option = new VehicleOption(m_modelCodeTextBox.Text, m_optionListComboBox.SelectedItem.ToString());
            }
            // Close the window
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
