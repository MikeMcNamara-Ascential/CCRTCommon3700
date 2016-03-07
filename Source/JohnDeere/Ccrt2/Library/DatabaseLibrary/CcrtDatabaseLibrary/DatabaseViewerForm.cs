using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CcrtDataBaseLibrary
{
    public partial class DatabaseViewerForm : Form
    {
        // ----------------------------------------------------------------------------------------
        // Class Functions
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// </summary>
        public DatabaseViewerForm()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Clear the form and prepare for new data to be displayed
        /// </summary>
        public void ClearForm()
        {   // Clear all the items from the port settings table
            m_portSettingsDataGridView.Rows.Clear();
            // Clear all the items from the data items table
            m_diDataGridView.Rows.Clear();
        }

        /// <summary>
        /// Display the contents of the port settings table.
        /// </summary>
        /// <param name="portSettings">POrt settings to be displayed in the form.</param>
        public void DisplayDatabaseContents(List<CcrtDb.PortSettings> portSettings)
        {
            foreach (CcrtDb.PortSettings setting in portSettings)
            {   // Create a new data row
                DataGridViewRow row = new DataGridViewRow();
                row.CreateCells(m_portSettingsDataGridView);
                row.Cells[m_portSettingsDataGridView.Columns["ServerNameCol"].Index].Value = setting.ServerName;
                row.Cells[m_portSettingsDataGridView.Columns["DeviceNameCol"].Index].Value = setting.DeviceName;
                row.Cells[m_portSettingsDataGridView.Columns["DeviceType"].Index].Value = setting.DeviceType;
                row.Cells[m_portSettingsDataGridView.Columns["BaudRateCol"].Index].Value = setting.BaudRate.ToString();
                row.Cells[m_portSettingsDataGridView.Columns["ParityCol"].Index].Value = setting.Parity;
                row.Cells[m_portSettingsDataGridView.Columns["DataBitsCol"].Index].Value = setting.DataBits.ToString();
                row.Cells[m_portSettingsDataGridView.Columns["StopBitsCol"].Index].Value = setting.StopBits.ToString();
                row.Cells[m_portSettingsDataGridView.Columns["SerialPortCol"].Index].Value = setting.PortName;
                row.Cells[m_portSettingsDataGridView.Columns["RxTimeoutCol"].Index].Value = setting.RxTimeout.ToString();
                row.Cells[m_portSettingsDataGridView.Columns["TxTimeoutCol"].Index].Value = setting.TxTimeout.ToString();
                row.Cells[m_portSettingsDataGridView.Columns["MonitorDataCol"].Index].Value = setting.MonitorData.ToString();
                row.Cells[m_portSettingsDataGridView.Columns["HeaderData"].Index].Value = setting.HeaderData;
                row.Cells[m_portSettingsDataGridView.Columns["HeaderLength"].Index].Value = setting.HeaderLength.ToString();
                row.Cells[m_portSettingsDataGridView.Columns["TrailerData"].Index].Value = setting.TrailerData;
                row.Cells[m_portSettingsDataGridView.Columns["TrailerLength"].Index].Value = setting.TrailerLength.ToString();
                // Add the row to the grid
                m_portSettingsDataGridView.Rows.Add(row);
            }
        }

        /// <summary>
        /// Display the contents of the data items table.
        /// </summary>
        /// <param name="dataItems">InputdataItems to be displayed in the form.</param>
        public void DisplayDatabaseContents(List<CcrtDb.InputDataItems> dataItems)
        {
            foreach (CcrtDb.InputDataItems item in dataItems)
            {   // Create a new row
                DataGridViewRow row = new DataGridViewRow();
                row.CreateCells(m_diDataGridView);
                // Populate the row with the data
                row.Cells[m_diDataGridView.Columns["DataItem"].Index].Value = item.DataName;
                row.Cells[m_diDataGridView.Columns["MinLength"].Index].Value = item.MinLength.ToString();
                row.Cells[m_diDataGridView.Columns["MaxLength"].Index].Value = item.MaxLength.ToString();
                // Add the row to the grid
                m_diDataGridView.Rows.Add(row);
            }
        }
 
        /// <summary>
        /// Close the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_closeButton_Click(object sender, EventArgs e)
        {
            Visible = false;
        }
    }
}
