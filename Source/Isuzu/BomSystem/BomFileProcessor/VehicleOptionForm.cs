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
    public partial class VehicleOptionForm : Form
    {
        public VehicleOptionForm(VehicleOptionCollection optionList, String optionName, List<String> validOptions)
        {
            InitializeComponent();
            Text = optionName + " Setup";
            ValidOptions = validOptions;
            m_optionList = optionList;
            UseValidTypeList = true;
            PopulateDataGrid();
        }

        public VehicleOptionForm(VehicleOptionCollection optionList, String optionName)
        {
            InitializeComponent();
            m_optionList = optionList;
            Text = optionName + " Setup";
            UseValidTypeList = false;
            PopulateDataGrid();
        }


        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Populate the data grid with the configured data.
        /// </summary>
        private void PopulateDataGrid()
        {   // Clear the grid before adding
            m_optionDataGridView.Rows.Clear();
            // Add all configured options to the grid
            foreach (VehicleOption option in m_optionList.Options)
            {   // Create a new row for the current option
                String[] row = { option.ModelCode, option.OptionValue };
                m_optionDataGridView.Rows.Add(row);
            }
        }


        //-----------------------------------------------------------------------------------------
        // Callbacks
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Display a form to allow the user to add a new association.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addButton_Click(object sender, EventArgs e)
        {
            VehicleOption option = null;
            if (UseValidTypeList)
            {
                AddOptionListForm addList = new AddOptionListForm(m_validOptions, "Add Axle Type");
                option = addList.AddNewOption();
            }
            else
            {
                AddOptionForm addForm = new AddOptionForm("Add Wheelbase Position");
                option = addForm.AddNewOption();
            }
            // Add the option to the list of options and redisplay the grid
            if (option != null)
            {
                m_optionList.Add(option.ModelCode, option.OptionValue);
                PopulateDataGrid();
            }
        }

        /// <summary>
        /// Remove a vevhicle build association.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_removeButton_Click(object sender, EventArgs e)
        {   // Populate the list of model codes
            List<String> modelCodes = new List<String>();
            foreach (VehicleOption option in m_optionList.Options)
            {
                modelCodes.Add(option.ModelCode);
            }
            // Display the form with the list of model codes
            RemoveOptionForm removeOption = new RemoveOptionForm(modelCodes, "Remove " + m_optionList.OptionName + " Entry");
            m_optionList.RemoveOption(removeOption.RemoveModelCode());
            // Update the display with the new item
            PopulateDataGrid();
        }

        /// <summary>
        /// Save the items in the grid.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in m_optionDataGridView.Rows)
            {
                m_optionList.UpdateOption(row.Cells["ModelCode"].Value.ToString(), row.Cells["Value"].Value.ToString());
            }
        }


        //-----------------------------------------------------------------------------------------
        // Member Variables and Properties
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Flag to indicate if the user should select the type from the list.
        /// </summary>
        private Boolean m_useValidTypeList;
        /// <summary>
        /// Get/Set the flag indicating if the user should select the option value from a valid type list.
        /// </summary>
        private Boolean UseValidTypeList
        {
            get { return m_useValidTypeList; }
            set { m_useValidTypeList = value; }
        }

        /// <summary>
        /// Option association list.
        /// </summary>
        private VehicleOptionCollection m_optionList;

        /// <summary>
        /// List of valid options.
        /// </summary>
        private List<String> m_validOptions;
        /// <summary>
        /// Get/Set the list of valid values for this option.
        /// </summary>
        private List<String> ValidOptions
        {
            get { return m_validOptions; }
            set { m_validOptions = value; }
        }
    }
}
