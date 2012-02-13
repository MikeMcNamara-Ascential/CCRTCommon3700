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
    public partial class ModelCodeSpecificSettings : Form
    {
        public ModelCodeSpecificSettings(ModelCodeOptionsCollection modelCodeOptionsCollection)
        {
            InitializeComponent();
            ModelCodeOptionsItems = modelCodeOptionsCollection;
            // Populate the grid with the model code options
            PopulateDataGrid();
            m_modificationsMade = false;
        }

        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Add the Model Code Options to the data grid.
        /// </summary>
        private void PopulateDataGrid()
        {   // Clear all rows from the grid
            m_modelOptionsDGV.Rows.Clear();
            // Add each row to the grid
            foreach (ModelCodeOptions item in ModelCodeOptionsItems.ModelCodeOptionsList)
            {   // Add the item to the grid
                String[] row = { item.ModelCode, item.WriteESN, item.ESNLeadingCharacters };
                m_modelOptionsDGV.Rows.Add(row);
            }
        }

                //-----------------------------------------------------------------------------------------
        // Callbacks
        //-----------------------------------------------------------------------------------------
        
        /// <summary>
        /// Add a new item to the model code options list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addButton_Click(object sender, EventArgs e)
        {
            AddModelSpecificOptionsForm addItem = new AddModelSpecificOptionsForm();
            ModelCodeOptions newItem = addItem.AddNewItem();
            if (newItem != null)
            {   // Add the item to the list
                ModelCodeOptionsItems.Add(newItem.ModelCode, newItem.WriteESN, newItem.ESNLeadingCharacters);
                // Update the display with the new item
                PopulateDataGrid();
            }
        }

        /// <summary>
        /// Remove a model code from the options list.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_removeButton_Click(object sender, EventArgs e)
        {            
            List<String> itemList = new List<String>();
            foreach(ModelCodeOptions item in ModelCodeOptionsItems.ModelCodeOptionsList)
            {
                itemList.Add(item.ModelCode);
            }
            // Display the form with the list of removable items
            RemoveModelCodeSpecificOptionsForm removeItem = new RemoveModelCodeSpecificOptionsForm(itemList);
            ModelCodeOptionsItems.Remove(removeItem.RemoveItem());
            // Update the display with the new item
            PopulateDataGrid();
        }

        private void m_closeButton_Click(object sender, EventArgs e)
        {

        }
        
        /// <summary>
        /// Save the values from the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in m_modelOptionsDGV.Rows)
            {
                ModelCodeOptionsItems.UpdateItem(row.Cells["ModelCode"].Value.ToString(), row.Cells["WriteESN"].Value.ToString(), row.Cells["EsnLeadingChars"].Value.ToString());
            }
            m_modificationsMade = false;
        }

        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// List of BuildItems to be extracted from the BOM.
        /// </summary>
        private ModelCodeOptionsCollection m_modelCodeOptionsItems;
        /// <summary>
        /// Get/Set the list of build items to be extracted from the BOM.
        /// </summary>
        private ModelCodeOptionsCollection ModelCodeOptionsItems
        {
            get { return m_modelCodeOptionsItems; }
            set { m_modelCodeOptionsItems = value; }
        }

        private void ModelCodeSpecificSettings_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (m_modificationsMade)
            {
                DialogResult result = MessageBox.Show("Save Changes?", "Confirmation Required",
    MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
                if (result == DialogResult.Yes)
                {
                    foreach (DataGridViewRow row in m_modelOptionsDGV.Rows)
                        ModelCodeOptionsItems.UpdateItem(row.Cells["ModelCode"].Value.ToString(), row.Cells["WriteESN"].Value.ToString(), row.Cells["EsnLeadingChars"].Value.ToString());

                }
                else if (result == DialogResult.Cancel)
                {
                    e.Cancel = true;
                }
            }
        }
        private bool m_modificationsMade;

        private void m_modelOptionsDGV_CellBeginEdit(object sender, DataGridViewCellCancelEventArgs e)
        {
            m_modificationsMade = true;
        }

    }
}
