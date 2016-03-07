using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Logger
{
    public partial class LogFileChooserDialog : Form
    {
        /* ============================================================================================================
         * Member Functions
         * ============================================================================================================
         */

        /// <summary>
        /// Display a form for the user to select previous log files to view.
        /// </summary>
        /// <param name="logFileListing">List of log files to display.</param>
        public LogFileChooserDialog(List<FileInfo> logFileListing)
        {
            InitializeComponent();
            // Save the list of log files
            LogList = logFileListing;
            m_selectedFiles = new List<String>();
            PopulateLogFileListing();
        }

        /// <summary>
        /// Add all the log files to the box so the operator can select them.
        /// </summary>
        private void PopulateLogFileListing()
        {   // Clear the text box
            m_logFilesCheckedListBox.Items.Clear();
            // Add each log file to the box
            foreach (FileInfo file in LogList)
            {   // Add the file to the list
                m_logFilesCheckedListBox.Items.Add(file.Name);
            }
        }



        /* ============================================================================================================
         * Member Variables / Properties
         * ============================================================================================================
         */

        /// <summary>
        /// Get/Set the list of valid log files.
        /// </summary>
        private List<FileInfo> LogList { get; set; }

        /// <summary>
        /// List of log files selected by the user.
        /// </summary>
        private List<String> m_selectedFiles;

        /// <summary>
        /// Get the list of log files selected by the user.
        /// </summary>
        public List<String> SelectedLogFiles
        {
            get { return m_selectedFiles; }
        }

        /// <summary>
        /// Open the selected log files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_openButton_Click(object sender, EventArgs e)
        {   // Clear the list
            m_selectedFiles.Clear();
            for (Int16 index = 0; index < m_logFilesCheckedListBox.CheckedItems.Count; index++)
            {
                m_selectedFiles.Add(m_logFilesCheckedListBox.CheckedItems[index].ToString());
            }
            // Make sure to "close" the display
            Visible = false;
            DialogResult = DialogResult.OK;
        }
    }
}
