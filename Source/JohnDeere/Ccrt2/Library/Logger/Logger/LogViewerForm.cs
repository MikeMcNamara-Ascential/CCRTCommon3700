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
    public partial class LogViewerForm : Form
    {
        /* ============================================================================================================
         * Member Functions
         * ============================================================================================================
         */

        /// <summary>
        /// Class constructor.
        /// Displays the log viewer form with the contents of the specified log file.
        /// </summary>
        /// <param name="logFileName">Name of the log file to display.</param>
        public LogViewerForm(String logFileName)
        {
            InitializeComponent();
            // Add the file name to the title bar
            Text += " - " + logFileName;
            // Save the name of the log file and display the contents
            LogFileName = logFileName;
            DisplayLogFile();
        }

        /// <summary>
        /// Display the contents of the log file.
        /// </summary>
        private void DisplayLogFile()
        {   // Clear the text box
            m_logTextBox.Clear();
            // Load the log file into memory
            using (StreamReader reader = new StreamReader(LogFileName))
            {
                while (reader.Peek() >= 0)
                {
                    m_logTextBox.AppendText(reader.ReadLine() + Environment.NewLine);
                }
            }
            // Reset search index
            CurrentSearchIndex = 0;
        }

        /// <summary>
        /// Search for the specified string
        /// </summary>
        private void SearchForString()
        {
            Int32 index = m_logTextBox.Text.ToLower().IndexOf(m_searchTextBox.Text.ToLower(), CurrentSearchIndex);
            if (index != -1)
            {   // Found the string, highlight the selected text
                m_logTextBox.Focus();
                m_logTextBox.Select(index, m_searchTextBox.Text.Length);
                // Move the focus so we can see the highlighted text
                m_logTextBox.ScrollToCaret();
                // Update the search index so we do not find the same instance
                CurrentSearchIndex = index + m_searchTextBox.Text.Length;
            }
            else
            {   // String not found, notify user and reset index to the beginning of the file
                MessageBox.Show("Could not find: " + m_searchTextBox.Text + " - Restarting search from top", "String not found",
                                MessageBoxButtons.OK, MessageBoxIcon.Information);
                CurrentSearchIndex = 0;
            }
        }



        /* ============================================================================================================
         * Member Variables / Properties
         * ============================================================================================================
         */

        /// <summary>
        /// Get/Set the current search index.
        /// </summary>
        private Int32 CurrentSearchIndex { get; set; }

        /// <summary>
        /// Name of the log file being displayed.
        /// </summary>
        private String LogFileName { get; set; }



        /* ============================================================================================================
         * Class Callbacks
         * ============================================================================================================
         */

        /// <summary>
        /// Close the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_closeButton_Click(object sender, EventArgs e)
        {   // Clear the text box and close the form
            m_searchTextBox.Clear();
            m_logTextBox.Clear();
            Close();
        }

        /// <summary>
        /// Search the log text for the specified string.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_findButton_Click(object sender, EventArgs e)
        {   // Search for the selected text
            SearchForString();
        }

        /// <summary>
        /// Update the current search location to the clicked location.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_logTextBox_MouseClick(object sender, MouseEventArgs e)
        {
            CurrentSearchIndex = m_logTextBox.GetCharFromPosition(e.Location);
        }

        /// <summary>
        /// Reload the log data from file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_reloadButton_Click(object sender, EventArgs e)
        {
            DisplayLogFile();
        }

        /// <summary>
        /// Check the keypress.  If the key pressed is the Enter key, the log text will be searched
        /// for the text in the search box.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_searchTextBox_KeyDown(object sender, KeyEventArgs e)
        {   // Only act on the key if it is the Enter key.
            // Treat the Enter keypress the same as the Find button on the display.
            if ((e.KeyCode == Keys.LineFeed) || (e.KeyCode == Keys.Enter))
            {
                SearchForString();
            }
        }

    }
}
