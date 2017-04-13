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
    public partial class BomFileSetupForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Initialize the class and populate the form
        /// </summary>
        public BomFileSetupForm()
        {
            InitializeComponent();
            // Populate the data grid with the current settings
            PopulateDataGrid();
        }

        /// <summary>
        /// Populate the data grid with the 
        /// </summary>
        private void PopulateDataGrid()
        {   // Create the rows to be added to the table
            String[] upc = {BomFileProcessor.Properties.Resources.BomUpcColumnText, 
                            BomFileProcessor.Properties.Settings.Default.BomUpcIndex.ToString(),
                            BomFileProcessor.Properties.Settings.Default.BomUpcLength.ToString()};
            String[] fna = {BomFileProcessor.Properties.Resources.BomFnaColumnText,
                            BomFileProcessor.Properties.Settings.Default.U.ToString(),
                            BomFileProcessor.Properties.Settings.Default.BomFnaLength.ToString()};
            String[] partNumber = {BomFileProcessor.Properties.Resources.BomPartNumberColumnText,
                                   BomFileProcessor.Properties.Settings.Default.BomPartNumberIndex.ToString(),
                                   BomFileProcessor.Properties.Settings.Default.BomPartNumberLength.ToString()};
            String[] partName = {BomFileProcessor.Properties.Resources.BomPartNameColumnText,
                                 BomFileProcessor.Properties.Settings.Default.BomPartNameIndex.ToString(),
                                 BomFileProcessor.Properties.Settings.Default.BomPartNameLength.ToString()};
            String[] bomCount = {BomFileProcessor.Properties.Resources.BomTotalBomCountColumnText,
                                 BomFileProcessor.Properties.Settings.Default.BomCountIndex.ToString(),
                                 BomFileProcessor.Properties.Settings.Default.BomCountLength.ToString()};
            String[] bomRecordCount = {BomFileProcessor.Properties.Resources.BomRecordCountColumnText,
                                 BomFileProcessor.Properties.Settings.Default.BomRecordCountIndex.ToString(),
                                 BomFileProcessor.Properties.Settings.Default.BomRecordCountLength.ToString()};
            String[] modelCode = {BomFileProcessor.Properties.Resources.BomModelCodeColumnName,
                                  BomFileProcessor.Properties.Settings.Default.BomModelCodeIndex.ToString(),
                                  BomFileProcessor.Properties.Settings.Default.BomModelCodeLength.ToString()};
            String[] modelYear = {BomFileProcessor.Properties.Resources.BomModelYearColumnName,
                                  BomFileProcessor.Properties.Settings.Default.BomModelYearIndex.ToString(),
                                  BomFileProcessor.Properties.Settings.Default.BomModelYearLength.ToString()};
            String[] lotNumber = {BomFileProcessor.Properties.Resources.BomLotNumberColumnName,
                                  BomFileProcessor.Properties.Settings.Default.BomLotNumberIndex.ToString(),
                                  BomFileProcessor.Properties.Settings.Default.BomLotNumberLength.ToString()};
            String[] bookCode = {BomFileProcessor.Properties.Resources.BomBookCodeColumnName,
                                 BomFileProcessor.Properties.Settings.Default.BomBookCodeIndex.ToString(),
                                 BomFileProcessor.Properties.Settings.Default.BomBookCodeLength.ToString()};
            // Add the items to the table
            m_bomSettingsDataGridView.Rows.Clear();
            m_bomSettingsDataGridView.Rows.Add(bomCount);
            m_bomSettingsDataGridView.Rows.Add(bomRecordCount);
            m_bomSettingsDataGridView.Rows.Add(modelCode);
            m_bomSettingsDataGridView.Rows.Add(modelYear);
            m_bomSettingsDataGridView.Rows.Add(lotNumber);
            m_bomSettingsDataGridView.Rows.Add(partNumber);
            m_bomSettingsDataGridView.Rows.Add(upc);
            m_bomSettingsDataGridView.Rows.Add(fna);
            m_bomSettingsDataGridView.Rows.Add(bookCode);
        }


        //-----------------------------------------------------------------------------------------
        // Callbacks
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// Save the changes to the BOM file setup.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {
            foreach (DataGridViewRow row in m_bomSettingsDataGridView.Rows)
            {   // Check the row we are operating on
                if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomUpcColumnText)
                {
                    BomFileProcessor.Properties.Settings.Default.BomUpcIndex = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomUpcLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
                else if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomFnaColumnText)
                {
                    BomFileProcessor.Properties.Settings.Default.U = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomFnaLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
                else if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomPartNumberColumnText)
                {
                    BomFileProcessor.Properties.Settings.Default.BomPartNumberIndex = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomPartNumberLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
                else if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomPartNameColumnText)
                {
                    BomFileProcessor.Properties.Settings.Default.BomPartNameIndex = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomPartNameLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
                else if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomTotalBomCountColumnText)
                {
                    BomFileProcessor.Properties.Settings.Default.BomCountIndex = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomCountLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
                else if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomRecordCountColumnText)
                {
                    BomFileProcessor.Properties.Settings.Default.BomRecordCountIndex = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomRecordCountLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
                else if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomModelCodeColumnName)
                {
                    BomFileProcessor.Properties.Settings.Default.BomModelCodeIndex = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomModelCodeLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
                else if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomModelYearColumnName)
                {
                    BomFileProcessor.Properties.Settings.Default.BomModelYearIndex = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomModelYearLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
                else if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomLotNumberColumnName)
                {
                    BomFileProcessor.Properties.Settings.Default.BomLotNumberIndex = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomLotNumberLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
                else if (row.Cells["FieldName"].Value.ToString() == BomFileProcessor.Properties.Resources.BomBookCodeColumnName)
                {
                    BomFileProcessor.Properties.Settings.Default.BomBookCodeIndex = Convert.ToInt32(row.Cells["StartIndex"].Value);
                    BomFileProcessor.Properties.Settings.Default.BomBookCodeLength = Convert.ToInt32(row.Cells["Length"].Value);
                }
            }
            // save the settings
            BomFileProcessor.Properties.Settings.Default.Save();
            // Close the form
            Close();
        }

    }
}
