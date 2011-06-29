using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Windows.Forms;

namespace BomFileProcessor
{
    public partial class LogFileChooserDialog : Form
    {
        public LogFileChooserDialog(List<FileInfo> logListing)
        {
            InitializeComponent();
            // Save the list of log files
            LogList = logListing;
            m_selectedLogs = new List<String>();
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

        /// <summary>
        /// Store the selected items in a list so they can be opened.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_openButton_Click(object sender, EventArgs e)
        {   // Get the selected items
            m_selectedLogs.Clear();
            for(int index = 0; index < m_logFilesCheckedListBox.CheckedItems.Count; index++)
            {
                m_selectedLogs.Add(m_logFilesCheckedListBox.CheckedItems[index].ToString());
            }
            // Make sure to "close" the display
            Visible = false;
        }

        /// <summary>
        /// Get/Set the selected log files.
        /// </summary>
        public List<String> SelectedLogFiles
        {
            get { return m_selectedLogs; }
        }

        /// <summary>
        /// Get/Set the list of log files available on the system.
        /// </summary>
        private List<FileInfo> LogList
        {
            get { return m_logFiles; }
            set { m_logFiles = value; }
        }

        private List<FileInfo> m_logFiles;
        private List<String> m_selectedLogs;
    }
}