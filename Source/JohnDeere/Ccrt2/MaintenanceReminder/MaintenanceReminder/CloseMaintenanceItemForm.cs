using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using Logger;
using MaintenanceReminderLibrary;

namespace MaintenanceReminder
{
    public partial class CloseMaintenanceItemForm : Form
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        /// <summary>
        /// Create the Close Maintenance Item form.
        /// </summary>
        /// <param name="item">Maintenance Item to be closed.</param>
        /// <param name="localizer">Localizer to use for displaying the form in the selected language.</param>
        /// <param name="logger">Logger object to use for logging data.</param>
        public CloseMaintenanceItemForm(MaintenanceReminderItem item, CcrtLogger logger, Localizer localizer)
        {
            InitializeComponent();
            ReminderItem = item;
            Logger = logger;
            DisplayLocalizer = localizer;
            UpdateDisplayLanguage();
            m_itemName.Text = item.ItemSettings.Name.Replace("_", " ");
            m_itemText.Text = item.ItemSettings.FaultMessage;
        }

        /// <summary>
        /// Send a command to the QNX side to reset the elapsed time for this maintenance item.
        /// </summary>
        private void ResetElapsedTime()
        {
            String command = "rsh " + MaintenanceReminder.Properties.Settings.Default.QnxIpAddress +
                             " -l root -n /usr/local/bin/SendCmd -ncore/MachineDataBroker -tCommand -xResetMaintenanceItem," +
                             ReminderItem.ItemSettings.Name;
            ProcessStartInfo procStartInfo = new ProcessStartInfo("cmd", "/c " + command);
            procStartInfo.CreateNoWindow = true;
            Process proc = new Process();
            proc.StartInfo = procStartInfo;
            proc.Start();
            Logger.Log("Reset elapsed time for " + ReminderItem.ItemSettings.Name.Replace("_", " "));
        }

        /// <summary>
        /// Update the text on the form to the selected display language.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            Text = DisplayLocalizer.GetLocalizedString("CloseMaintenanceItemTitle");
            m_saveButton.Text = DisplayLocalizer.GetLocalizedString("CloseLabel");
            m_itemCompleteCheckBox.Text = DisplayLocalizer.GetLocalizedString("Complete");
            m_itemLabel.Text = DisplayLocalizer.GetLocalizedString("ReminderNameLabel");
            m_itemTextLabel.Text = DisplayLocalizer.GetLocalizedString("MaintenanceTextLabel");
            m_nameLabel.Text = DisplayLocalizer.GetLocalizedString("NameLabel");
            m_notesLabel.Text = DisplayLocalizer.GetLocalizedString("NoteLabel");
        }




        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// Get/Set the localizer to use for displaying the forms with the local language.
        /// </summary>
        private Localizer DisplayLocalizer { get; set; }

        /// <summary>
        /// Logging object so we can find out what is going on.
        /// </summary>
        private CcrtLogger Logger { get; set; }

        /// <summary>
        /// Maintenance Reminder item to be closed.
        /// </summary>
        private MaintenanceReminderItem ReminderItem { get; set; }




        //=========================================================================================
        // Callbacks, Events and Delegates
        //=========================================================================================

        /// <summary>
        /// Close the dialog.
        /// If the Complete box is checked, the elapsed time will be reset.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {   // If notes have been entered, add them to the notes for this reminder
            if (!String.IsNullOrEmpty(m_notesTextBox.Text))
            {
                if (!String.IsNullOrEmpty(m_nameTextBox.Text))
                {
                    ReminderItem.AddMaintenanceNote(m_nameTextBox.Text + "\t" + m_notesTextBox.Text);
                    // If the complete box is checked, reset the elapsed time
                    if (m_itemCompleteCheckBox.Checked)
                    {
                        String message = DisplayLocalizer.GetLocalizedString("MaintenanceCompleteLogMsg");
                        ReminderItem.AddMaintenanceNote(message);
                        Logger.Log(ReminderItem.ItemSettings.Name + ": " + message);
                        if (ReminderItem.ItemSettings.IntervalUnits == "Hours")
                        {
                            Thread resetTimeThread = new Thread(new ThreadStart(ResetElapsedTime));
                            resetTimeThread.Start();
                        }
                        else
                        {
                            ReminderItem.ItemSettings.IntervalStartDate = DateTime.Now;
                        }
                    }
                    else
                    {
                        String message = DisplayLocalizer.GetLocalizedString("MaintenanceNotCompleteLogMsg");
                        ReminderItem.AddMaintenanceNote(message);
                        Logger.Log(ReminderItem.ItemSettings.Name + ": " + message);
                    }
                }
                else
                {
                    MessageBox.Show(DisplayLocalizer.GetLocalizedString("EnterNameMsg"),
                                    DisplayLocalizer.GetLocalizedString("MissingNameTitle"),
                                    MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                }
            }
            else
            {
                MessageBox.Show(DisplayLocalizer.GetLocalizedString("EnterMaintenanceNoteMsg"),
                                DisplayLocalizer.GetLocalizedString("MissingTextTitle"), MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        /// <summary>
        /// Display the work instruction document to the user.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_viewWiButton_Click(object sender, EventArgs e)
        {
            ReminderItem.ViewWorkInstruction();
        }

    }
}
