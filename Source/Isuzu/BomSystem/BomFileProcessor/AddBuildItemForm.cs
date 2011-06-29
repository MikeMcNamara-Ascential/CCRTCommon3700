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
    public partial class AddBuildItemForm : Form
    {
        public AddBuildItemForm()
        {
            InitializeComponent();
            m_item = null;
        }

        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Display the form for adding a new build item.
        /// </summary>
        /// <returns></returns>
        public BuildItem AddNewItem()
        {
            ShowDialog();
            return m_item;
        }


        //-----------------------------------------------------------------------------------------
        // Callbacks
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Store the data in the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_okButton_Click(object sender, EventArgs e)
        {
            if ((m_itemNameTextBox.Text.Length > 0) && (m_upcTextBox.Text.Length > 0) && (m_fnaTextBox.Text.Length > 0))
            {
                m_item = new BuildItem(m_itemNameTextBox.Text, m_upcTextBox.Text, m_fnaTextBox.Text);
            }
            Close();
        }


        //-----------------------------------------------------------------------------------------
        // Member Variables
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Build item from data that was entered into the form.
        /// </summary>
        BuildItem m_item;

    }
}
