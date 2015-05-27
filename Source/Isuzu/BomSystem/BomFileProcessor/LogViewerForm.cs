using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows.Forms;

namespace BomFileProcessor
{
    public partial class LogViewerForm : Form
    {
        delegate void ClearLogDisplayCallback();
        delegate void DisplayLineCallback(string s);

        public LogViewerForm(Logger log, string filename)
        {
            InitializeComponent();
            m_logger = log;
            m_fileName = filename;
            // Place the file name in the title bar
            this.Text = this.Text + " - " + m_fileName;
            // Set the icon
            this.Icon = Icon.FromHandle(BomFileProcessor.Properties.Resources.bep_best.GetHicon());
            DisplayLogFile();
        }

        /// <summary>
        /// Display the contents of the log file.
        /// </summary>
        private void DisplayLogFile()
        {   // Clear the text box
            this.ClearLogDisplay();
            // Load the log file into memory, use a lock to prevent IOExceptions
            lock (m_logger)
            {
                m_logger.Log("INFO: Rendering Log " + m_fileName + ". This process can take several minutes");
                using (StreamReader reader = new StreamReader(LogFileName))
                {
                    while (reader.Peek() >= 0)
                    {
                        this.DisplayLine(reader.ReadLine() + Environment.NewLine);
                    }
                }
            }

            // Reset search index
            CurrentSearchIndex = 0;
        }

        private void ClearLogDisplay()
        {
            try
            {   //Thread-safe clearing
                if (this.m_logFileTextBox.InvokeRequired)
                {
                    ClearLogDisplayCallback d = new ClearLogDisplayCallback(ClearLogDisplay);
                    this.Invoke(d);
                }
                else
                {
                    this.m_logFileTextBox.Clear();
                }
            }
            catch (ObjectDisposedException)
            {   //Happens when the application closes
            }

            
        }
        private void DisplayLine(string s)
        {
            try
            {   //Thread-safe writing
                if (this.m_logFileTextBox.InvokeRequired)
                {
                    DisplayLineCallback d = new DisplayLineCallback(DisplayLine);
                    this.Invoke(d, new object[] { s });
                }
                else
                {
                    this.m_logFileTextBox.AppendText(s);
                }
            }
            catch (ObjectDisposedException)
            {   //Happens when the application closes
            }
            
        }

        /// <summary>
        /// Search for the specified string
        /// </summary>
        private void SearchForString()
        {
            int index = m_logFileTextBox.Text.ToLower().IndexOf(m_searchTextBox.Text.ToLower(), CurrentSearchIndex);
            if (index != -1)
            {   // Found the string, highlight the selected text
                m_logFileTextBox.Focus();
                m_logFileTextBox.Select(index, m_searchTextBox.Text.Length);
                // Move the focus so we can see the highlighted text
                m_logFileTextBox.ScrollToCaret();
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

        /// <summary>
        /// Close the window.
        /// </summary>
        /// <param name="sender">Object that invoked this callback.</param>
        /// <param name="e">Event arguments.</param>
        private void m_closeButton_Click(object sender, EventArgs e)
        {   // Clear the text and close the form
            m_searchTextBox.Clear();
            m_logFileTextBox.Clear();
            Close();
        }

        /// <summary>
        /// Reload the log file.
        /// </summary>
        /// <param name="sender">Object that invoked this callback.</param>
        /// <param name="e">Event arguments.</param>
        private void m_reloadButton_Click(object sender, EventArgs e)
        {
            DisplayLogFile();
        }

        /// <summary>
        /// Search the log text for the string the user typed in the search box.
        /// </summary>
        /// <param name="sender">Object that invoked this callback.</param>
        /// <param name="e">Event arguments.</param>
        private void m_findButton_Click(object sender, EventArgs e)
        {   // Search for the selected text
            SearchForString();
        }

        /// <summary>
        /// Get/Set the name of the log file currently being viewed.
        /// </summary>
        private String LogFileName
        {
            get { return m_fileName; }
            set { m_fileName = value; }
        }

        /// <summary>
        /// Get/Set the current search index.
        /// </summary>
        private int CurrentSearchIndex
        {
            get { return m_currentSearchIndex; }
            set { m_currentSearchIndex = value; }
        }

        private String m_fileName;
        private int m_currentSearchIndex;
        private Logger m_logger;

        /// <summary>
        /// Check the keypress.  If the key pressed is the Enter key, the log text will be searched
        /// for the text in the search box.
        /// </summary>
        /// <param name="sender">Object that invoked this callback.</param>
        /// <param name="e">Key event arguments.</param>
        private void m_searchTextBox_KeyDown(object sender, KeyEventArgs e)
        {   // Only act on the key if it is the Enter key.  Treat the Enter keypress the same as the Find button on the display.
            if ((e.KeyCode == Keys.LineFeed) || (e.KeyCode == Keys.Enter))
            {
                SearchForString();
            }
        }

        /// <summary>
        /// Update the current search location to the clicked location.
        /// </summary>
        /// <param name="sender">Object that invoked this argument.</param>
        /// <param name="e">Mouse event arguments.</param>
        private void m_logFileTextBox_MouseClick(object sender, MouseEventArgs e)
        {   // Reset the search index to the clicked location.
            CurrentSearchIndex = m_logFileTextBox.GetCharFromPosition(e.Location);
        }
    }
}