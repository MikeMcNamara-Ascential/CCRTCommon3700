using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace InputServer
{
    public partial class StartupMessageWindow : Form
    {
        public StartupMessageWindow(Localizer localizer)
        {
            InitializeComponent();
            m_startLabel.Text = localizer.GetLocalizedString("StartingInputServer");
        }

        /// <summary>
        /// Display a startup message for the user.
        /// </summary>
        /// <param name="message">Message to display for the user.</param>
        /// <param name="textColor">Text color for the message.</param>
        public void DisplayStartupMessage(String message, Color textColor)
        {
            m_startMessageLabel.Text = message;
            m_startMessageLabel.ForeColor = textColor;
        }
    }
}
