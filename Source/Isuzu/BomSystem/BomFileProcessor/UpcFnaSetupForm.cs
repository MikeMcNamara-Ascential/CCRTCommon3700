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
    public partial class UpcFnaSetupForm : Form
    {
        public UpcFnaSetupForm(BuildItemCollection buildItems)
        {
            InitializeComponent();
            BuildItems = buildItems;
            // Populate the grid with the UPC and FNA data for each module
            PopulateDataGrid();
        }


        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Add the UPC and FNA data for each module to the data grid.
        /// </summary>
        private void PopulateDataGrid()
        {   // Clear all rows from the grid
            m_upcFnaDataGridView.Rows.Clear();
            // Add each row to the grid
            foreach (BuildItem item in BuildItems.BuildItems)
            {   // Add the item to the grid
                String[] row = { item.ItemName, item.UPC, item.FNA };
                m_upcFnaDataGridView.Rows.Add(row);
            }
        }


        //-----------------------------------------------------------------------------------------
        // Callbacks
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Add a new item to extract from the BOM.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addItemButton_Click(object sender, EventArgs e)
        {
            AddBuildItemForm addItem = new AddBuildItemForm();
            BuildItem newItem = addItem.AddNewItem();
            if (newItem != null)
            {   // Add the item to the list
                BuildItems.Add(newItem.ItemName, newItem.UPC, newItem.FNA);
                // Update the display with the new item
                PopulateDataGrid();
            }
        }

        /// <summary>
        /// Remove an item that was being extracted from the BOM.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_removeButton_Click(object sender, EventArgs e)
        {   // Create a list of configured items that can be removed.
            List<String> itemList = new List<String>();
            foreach(BuildItem item in BuildItems.BuildItems)
            {
                itemList.Add(item.ItemName);
            }
            // Display the form with the list of removable items
            RemoveBuildItemForm removeItem = new RemoveBuildItemForm(itemList);
            BuildItems.Remove(removeItem.RemoveItem());
            // Update the display with the new item
            PopulateDataGrid();
        }

        /// <summary>
        /// Save the values from the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in m_upcFnaDataGridView.Rows)
            {
                BuildItems.UpdateItem(row.Cells["Module"].Value.ToString(), row.Cells["UPC"].Value.ToString(), row.Cells["FNA"].Value.ToString());
            }
        }


        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// List of BuildItems to be extracted from the BOM.
        /// </summary>
        private BuildItemCollection m_buildItems;
        /// <summary>
        /// Get/Set the list of build items to be extracted from the BOM.
        /// </summary>
        private BuildItemCollection BuildItems
        {
            get { return m_buildItems; }
            set { m_buildItems = value; }
        }

    }
}
