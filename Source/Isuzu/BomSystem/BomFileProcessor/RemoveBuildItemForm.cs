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
    public partial class RemoveBuildItemForm : Form
    {
        public RemoveBuildItemForm(List<String> items)
        {
            InitializeComponent();
            foreach (String item in items)
            {
                m_itemComboBox.Items.Add(item);
            }
            if (m_itemComboBox.Items.Count > 0)
            {
                m_itemComboBox.SelectedIndex = 0;
            }
            m_item = "";
        }


        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Remove the selected item.
        /// </summary>
        /// <returns></returns>
        public String RemoveItem()
        {
            ShowDialog();
            return m_item;
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
            if (m_itemComboBox.SelectedItem != null)
            {
                m_item = m_itemComboBox.SelectedItem.ToString();
            }
            Close();
        }


        //-----------------------------------------------------------------------------------------
        // Member Variables
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Item to be removed.
        /// </summary>
        private String m_item;

    }
}
