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
    public partial class PassConfirmationFileSetupForm : Form
    {
        //-----------------------------------------------------------------------------------------
        // Member Functions
        //-----------------------------------------------------------------------------------------

        public PassConfirmationFileSetupForm()
        {
            InitializeComponent();
            // Initialize the form with the default data
            InitializeFormData();
        }

        /// <summary>
        /// Initialize the form data.
        /// </summary>
        private void InitializeFormData()
        {
            m_ccrtPassConfirmFileLocationTextBox.Text = BomFileProcessor.Properties.Settings.Default.CcrtFileLocation;
            m_spartanPassFileLocationTextBox.Text = BomFileProcessor.Properties.Settings.Default.SpartanFileLocation;
            m_spartanFlashPassFileLocationTextBox.Text = BomFileProcessor.Properties.Settings.Default.SpartanFlashFileLocation;
            m_fileCheckDelayNumericUpDown.Value = Convert.ToInt32(BomFileProcessor.Properties.Settings.Default.PassConfirmationCheckDelay);
            m_monitorPassConfirmFilesCheckBox.Checked = BomFileProcessor.Properties.Settings.Default.CheckForPassConfirmationFiles;
            m_widowsESNFileLocationTextBox.Text = BomFileProcessor.Properties.Settings.Default.WindowsPCESNFileLocation;
            m_realTimeESNFileLocationTextBox.Text = BomFileProcessor.Properties.Settings.Default.RealTimePCESNFileLocation;
        }

        /// <summary>
        /// Display a file browser dialog so the user can choose the location for specified Pass Confirmation
        /// Files.
        /// </summary>
        /// <param name="initPath">Starting directory location.</param>
        /// <param name="systemType">Type of system to select the location for.</param>
        /// <returns>Pass Confirmation file location selected by the user.</returns>
        private String SelectFileLocation(String initPath, String systemType, String fileType)
        {
            String location = initPath;
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select location for " + systemType + " " + fileType + " " +"Files";
            dlg.ShowNewFolderButton = true;
            dlg.SelectedPath = location;
            if (dlg.ShowDialog() == DialogResult.OK)
            {   // Store the new location
                location = dlg.SelectedPath;
            }
            return location;
        }

        //-----------------------------------------------------------------------------------------
        // Callback Functions
        //-----------------------------------------------------------------------------------------

        /// <summary>
        /// A folder selection dialog will be displayed so that the user can select the location
        /// of the CCRT generated Pass Confirmation files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_ccrtSelectButton_Click(object sender, EventArgs e)
        {
            //String directory = SelectFileLocation(BomFileProcessor.Properties.Settings.Default.CcrtFileLocation, "CCRT", "Pass Confirmation");
            //if (directory.Length > 0)
            //{
                BomFileProcessor.Properties.Settings.Default.CcrtFileLocation = m_ccrtPassConfirmFileLocationTextBox.Text;
                BomFileProcessor.Properties.Settings.Default.Save();
                m_ccrtPassConfirmFileLocationTextBox.Clear();
                m_ccrtPassConfirmFileLocationTextBox.Text = m_ccrtPassConfirmFileLocationTextBox.Text;
            //}
        }

        /// <summary>
        /// A folder selection dialog will be displayed so the user can select the location 
        /// to place the Pass confirmation files for Spartan.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_SpartanSelectButton_Click(object sender, EventArgs e)
        {
            String directory = SelectFileLocation(BomFileProcessor.Properties.Settings.Default.SpartanFileLocation, "Spartan", "Pass Confirmation");
            if (directory.Length > 0)
            {
                BomFileProcessor.Properties.Settings.Default.SpartanFileLocation = directory;
                BomFileProcessor.Properties.Settings.Default.Save();
                m_spartanPassFileLocationTextBox.Clear();
                m_spartanPassFileLocationTextBox.Text = directory;
            }
        }
        /// <summary>
        /// A folder selection dialog will be displayed so the user can select the location 
        /// to place the Pass confirmation files for Spartan.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_SpartanFlashSelectButton_Click(object sender, EventArgs e)
        {
            String directory = SelectFileLocation(BomFileProcessor.Properties.Settings.Default.SpartanFlashFileLocation, "Spartan", "Pass Confirmation");
            if (directory.Length > 0)
            {
                BomFileProcessor.Properties.Settings.Default.SpartanFlashFileLocation = directory;
                BomFileProcessor.Properties.Settings.Default.Save();
                m_spartanFlashPassFileLocationTextBox.Clear();
                m_spartanFlashPassFileLocationTextBox.Text = directory;
            }

        }
        /// <summary>
        /// A folder selection dialog will be displayed so the user can select the location 
        /// Spartan places ESN file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_widowsESNSelectButton_Click(object sender, EventArgs e)
        {
            String directory = SelectFileLocation(BomFileProcessor.Properties.Settings.Default.WindowsPCESNFileLocation, "Spartan", "ESN File");
            if (directory.Length > 0)
            {
                BomFileProcessor.Properties.Settings.Default.WindowsPCESNFileLocation = directory;
                BomFileProcessor.Properties.Settings.Default.Save();
                m_widowsESNFileLocationTextBox.Clear();
                m_widowsESNFileLocationTextBox.Text = directory;
            }

        }
        /// <summary>
        /// A folder selection dialog will be displayed so the user can select the location 
        /// to place ESN on real time PC.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_realTimeESNSelectButton_Click(object sender, EventArgs e)
        {
            //String directory = SelectFileLocation(BomFileProcessor.Properties.Settings.Default.WindowsPCESNFileLocation, "CCRT", "ESN File");
            //if (directory.Length > 0)
            //{
                BomFileProcessor.Properties.Settings.Default.RealTimePCESNFileLocation = m_realTimeESNFileLocationTextBox.Text;
                BomFileProcessor.Properties.Settings.Default.Save();
                m_realTimeESNFileLocationTextBox.Clear();
                m_realTimeESNFileLocationTextBox.Text = m_realTimeESNFileLocationTextBox.Text;
            //}
        }
        /// <summary>
        /// Store all the data the user has entered.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        { 
            BomFileProcessor.Properties.Settings.Default.CcrtFileLocation = m_ccrtPassConfirmFileLocationTextBox.Text;
            BomFileProcessor.Properties.Settings.Default.SpartanFileLocation = m_spartanPassFileLocationTextBox.Text;
            BomFileProcessor.Properties.Settings.Default.SpartanFlashFileLocation = m_spartanFlashPassFileLocationTextBox.Text;
            BomFileProcessor.Properties.Settings.Default.CheckForPassConfirmationFiles = m_monitorPassConfirmFilesCheckBox.Checked;
            BomFileProcessor.Properties.Settings.Default.PassConfirmationCheckDelay = Convert.ToInt32(m_fileCheckDelayNumericUpDown.Value);
            BomFileProcessor.Properties.Settings.Default.WindowsPCESNFileLocation = m_widowsESNFileLocationTextBox.Text;
            BomFileProcessor.Properties.Settings.Default.RealTimePCESNFileLocation = m_realTimeESNFileLocationTextBox.Text;
            BomFileProcessor.Properties.Settings.Default.Save();
        }
    }
}
