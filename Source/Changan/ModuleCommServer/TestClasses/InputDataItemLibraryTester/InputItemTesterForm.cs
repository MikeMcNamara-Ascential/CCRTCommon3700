using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using InputDataItemLibrary;


namespace InputDataItemLibraryTester
{
    public partial class InputItemTesterForm : Form
    {
        public InputItemTesterForm()
        {
            InitializeComponent();
            DataItems = new List<InputDataItem>();
        }

        /// <summary>
        /// Create a DataItemSettings object from the data entered on the form.
        /// </summary>
        /// <returns>DataItemSettings object created from the form data.</returns>
        private DataItemSettings GetItemSettings()
        {
            DataItemSettings settings = new DataItemSettings();
            settings.DataName = m_dataItemTextBox.Text;
            settings.MinDataLength = Convert.ToInt32(m_minLengthTextBox.Text);
            settings.MaxDataLength = Convert.ToInt32(m_maxLengthTextBox.Text);
            settings.HeaderData = m_headerTextBox.Text;
            settings.HeaderDataLength = Convert.ToInt32(m_headerLengthTextBox.Text);
            settings.TrailerData = m_trailerTextBox.Text;
            settings.TrailerDataLength = Convert.ToInt32(m_trailerLengthTextBox.Text);
            return settings;
        }

        /// <summary>
        /// Display the list of configured data items.
        /// </summary>
        private void DisplayDataItems()
        {   // Clear the data grid view first
            m_diDataGridView.Rows.Clear();
            // Add each item to the grid
            foreach (InputDataItem item in DataItems)
            {   // Create a new row
                DataGridViewRow row = new DataGridViewRow();
                row.CreateCells(m_diDataGridView);
                // Populate the data in each cell
                row.Cells[m_diDataGridView.Columns["ItemName"].Index].Value = item.ItemSettings.DataName;
                row.Cells[m_diDataGridView.Columns["MinLength"].Index].Value = item.ItemSettings.MinDataLength.ToString();
                row.Cells[m_diDataGridView.Columns["MaxLength"].Index].Value = item.ItemSettings.MaxDataLength.ToString();
                row.Cells[m_diDataGridView.Columns["HeaderData"].Index].Value = item.ItemSettings.HeaderData;
                row.Cells[m_diDataGridView.Columns["HeaderLength"].Index].Value = item.ItemSettings.HeaderDataLength.ToString();
                row.Cells[m_diDataGridView.Columns["TrailerData"].Index].Value = item.ItemSettings.TrailerData;
                row.Cells[m_diDataGridView.Columns["TrailerLength"].Index].Value = item.ItemSettings.TrailerDataLength.ToString();
                // Add the row to the table
                m_diDataGridView.Rows.Add(row);
            }
        }

        /// <summary>
        /// Determine if the provided item matches the requested item.
        /// </summary>
        /// <param name="item">Data item to check.</param>
        /// <returns>Flag indicating if the provided item matches the desired item.</returns>
        private Boolean FindDataItem(InputDataItem item)
        {
            return item.ItemSettings.DataName == m_dataItemTextBox.Text;
        }

        /// <summary>
        /// List of data items.
        /// </summary>
        private List<InputDataItem> m_dataItems;
        private List<InputDataItem> DataItems
        {
            get { return m_dataItems; }
            set { m_dataItems = value; }
        }

        /// <summary>
        /// Add a new data item to the list of data items.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addItemButton_Click(object sender, EventArgs e)
        {   // Create a new InputdataItem from the form entries
            DataItems.Add(new InputDataItem(GetItemSettings()));
            DisplayDataItems();
        }

        /// <summary>
        /// Handle clicks inside the cells.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_diDataGridView_CellClick(object sender, DataGridViewCellEventArgs e)
        {   // Ignore clicks that are not on button cells
            if ((e.RowIndex >= 0) && (e.ColumnIndex == m_diDataGridView.Columns["ViewDataButtonCol"].Index))
            {   // User clicked a button, get the data item selected
                m_dataItems[e.RowIndex].ShowDataItemSettings();
            }
        }

        /// <summary>
        /// Update the settings for the speficied data item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_updateItemButton_Click(object sender, EventArgs e)
        {   // Find the matching item
            DataItems[DataItems.FindIndex(FindDataItem)].UpdatedataItemSettings(m_dataItemTextBox.Text,
                                                                                Convert.ToInt32(m_minLengthTextBox.Text),
                                                                                Convert.ToInt32(m_maxLengthTextBox.Text),
                                                                                m_headerTextBox.Text,
                                                                                Convert.ToInt32(m_headerLengthTextBox.Text),
                                                                                m_trailerTextBox.Text,
                                                                                Convert.ToInt32(m_trailerLengthTextBox.Text));
            DisplayDataItems();
        }

        /// <summary>
        /// Remove the specified data item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_removeItemButton_Click(object sender, EventArgs e)
        {
            DataItems.RemoveAt(DataItems.FindIndex(FindDataItem));
            DisplayDataItems();
        }
    }
}
