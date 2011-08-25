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
        }

        /// <summary>
        /// Display a file browser dialog so the user can choose the location for specified Pass Confirmation
        /// Files.
        /// </summary>
        /// <param name="initPath">Starting directory location.</param>
        /// <param name="systemType">Type of system to select the location for.</param>
        /// <returns>Pass Confirmation file location selected by the user.</returns>
        private String SelectPassFileLocation(String initPath, String systemType)
        {
            String location = initPath;
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select location for " + systemType + " Pass Confirmation Files";
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
            String directory = SelectPassFileLocation(BomFileProcessor.Properties.Settings.Default.CcrtFileLocation, "CCRT");
            if (directory.Length > 0)
            {
                BomFileProcessor.Properties.Settings.Default.CcrtFileLocation = directory;
                BomFileProcessor.Properties.Settings.Default.Save();
                m_ccrtPassConfirmFileLocationTextBox.Clear();
                m_ccrtPassConfirmFileLocationTextBox.Text = directory;
            }
        }

        /// <summary>
        /// A folder selection dialog will be displayed so the user can select the location 
        /// to place the Pass confirmation files for Spartan.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_SpartanSelectButton_Click(object sender, EventArgs e)
        {
            String directory = SelectPassFileLocation(BomFileProcessor.Properties.Settings.Default.SpartanFileLocation, "Spartan");
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
            String directory = SelectPassFileLocation(BomFileProcessor.Properties.Settings.Default.SpartanFlashFileLocation, "Spartan");
            if (directory.Length > 0)
            {
                BomFileProcessor.Properties.Settings.Default.SpartanFlashFileLocation = directory;
                BomFileProcessor.Properties.Settings.Default.Save();
                m_spartanFlashPassFileLocationTextBox.Clear();
                m_spartanFlashPassFileLocationTextBox.Text = directory;
            }

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
            BomFileProcessor.Properties.Settings.Default.Save();
        }



    }
}
