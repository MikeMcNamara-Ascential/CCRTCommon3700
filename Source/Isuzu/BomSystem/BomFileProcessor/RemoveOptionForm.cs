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
    public partial class RemoveOptionForm : Form
    {
        public RemoveOptionForm(List<String> modelCodes, String title)
        {
            InitializeComponent();
            // Populate the combo box with the list of model codes
            m_modelCodeComboBox.Items.Clear();
            m_modelCodeComboBox.Items.AddRange(modelCodes.ToArray());
            // Set to the first item in the list
            if (m_modelCodeComboBox.Items.Count > 0)
            {
                m_modelCodeComboBox.SelectedIndex = 0;
            }
            m_modelCode = "";
            Text = title;
        }


        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Remove the selected item.
        /// </summary>
        /// <returns></returns>
        public String RemoveModelCode()
        {
            ShowDialog();
            return m_modelCode;
        }


        //-----------------------------------------------------------------------------------------
        // Callbacks
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Store the selected item to be removed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_removeButton_Click(object sender, EventArgs e)
        {
            if (m_modelCodeComboBox.SelectedItem != null)
            {
                m_modelCode = m_modelCodeComboBox.SelectedItem.ToString();
            }
            Close();
        }

        
        //-----------------------------------------------------------------------------------------
        // Member Variables
        //-----------------------------------------------------------------------------------------
        /// <summary>
        /// Selected model code to remove.
        /// </summary>
        private String m_modelCode;
    }
}
