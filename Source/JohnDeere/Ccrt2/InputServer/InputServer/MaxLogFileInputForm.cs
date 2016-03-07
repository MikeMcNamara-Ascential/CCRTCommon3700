using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Logger;

namespace InputServer
{
    public partial class MaxLogFileInputForm : Form
    {
        // ----------------------------------------------------------------------------------------
        // Class Functions and Methods
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Class constructor.
        /// The current max log file count will be entered into the form on creation.
        /// </summary>
        public MaxLogFileInputForm(CcrtLogger logger, Localizer localizer)
        {
            InitializeComponent();
            m_maxLogFileCountTextBox.Text = InputServer.Properties.Settings.Default.LogFileCount.ToString();
            LogFile = logger;
            // Set the displayed text to the current locale
            Text = localizer.GetLocalizedString("MaxLogFileCountTitle");
            m_maxCountLabel.Text = localizer.GetLocalizedString("MaxNumberOfFiles");
            m_cancelButton.Text = localizer.GetLocalizedString("CloseLabel");
            m_okButton.Text = localizer.GetLocalizedString("SaveLabel");
        }



        // ----------------------------------------------------------------------------------------
        // Class Variables and Properties
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Logger object to log files.
        /// </summary>
        private CcrtLogger m_logger;

        /// <summary>
        /// Get/Set the logger object for this process.
        /// </summary>
        private CcrtLogger LogFile
        {
            get { return m_logger; }
            set { m_logger = value; }
        }


        
        // ----------------------------------------------------------------------------------------
        // Class Callbacks
        // ----------------------------------------------------------------------------------------

        /// <summary>
        /// Update the maximum number of log files.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_okButton_Click(object sender, EventArgs e)
        {
            Int32 newCount = Convert.ToInt32(m_maxLogFileCountTextBox.Text);
            if (newCount != InputServer.Properties.Settings.Default.LogFileCount)
            {   // Update the max file count
                LogFile.Log("Updating maximum number of log files from " +
                            InputServer.Properties.Settings.Default.LogFileCount.ToString() + " to " +
                            newCount.ToString());
                InputServer.Properties.Settings.Default.LogFileCount = newCount;
                InputServer.Properties.Settings.Default.Save();
            }
        }
    }
}
