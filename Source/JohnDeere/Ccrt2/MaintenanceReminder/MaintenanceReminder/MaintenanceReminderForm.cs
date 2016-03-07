using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using Logger;
using MaintenanceReminderLibrary;


namespace MaintenanceReminder
{
    public partial class MaintenanceReminderForm : Form
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        /// <summary>
        /// Create a new maintenance reminder form for the user to interact with.
        /// </summary>
        public MaintenanceReminderForm()
        {   // Set our icon
            this.Icon = MaintenanceReminder.Properties.Resources.maintenance;
            InitializeComponent();
            // Create a new reminder list
            Reminders = new List<MaintenanceReminderItem>();
            Logger = new CcrtLogger(MaintenanceReminder.Properties.Settings.Default.LogFilePath,
                                    "MaintenanceReminder", m_logTextBox, 10);
            // Setup the localizer items
            DisplayLocalizer = new Localizer(MaintenanceReminder.Properties.Settings.Default.CurrentLanguage, Logger);
            UpdateDisplayLanguage();
            // Create the expired settings dialog
            MaintenanceDueDlg = new ExpiredReminderForm(Reminders, Logger, DisplayLocalizer);
            // Load the reminder settings
            LoadReminders();
        }

        /// <summary>
        /// Display all the configured reminders in the data grid.
        /// </summary>
        private void DisplayReminders()
        {
            m_reminderDataGridView.Rows.Clear();
            List<String> expiredItems = new List<String>();
            foreach (MaintenanceReminderItem item in Reminders)
            {
                Double timeRemaining = item.ItemSettings.Interval - item.ItemSettings.CurrentElapsedTime;
                DataGridViewRow row = new DataGridViewRow();
                row.CreateCells(m_reminderDataGridView);
                row.Cells[m_reminderDataGridView.Columns["ReminderName"].Index].Value = item.ItemSettings.Name.Replace("_", " ");
                row.Cells[m_reminderDataGridView.Columns["Interval"].Index].Value = item.ItemSettings.Interval.ToString() + "  " + DisplayLocalizer.GetLocalizedString(item.ItemSettings.IntervalUnits);
                row.Cells[m_reminderDataGridView.Columns["ElapsedTime"].Index].Value = item.ItemSettings.CurrentElapsedTime.ToString("F2") + "  " + DisplayLocalizer.GetLocalizedString(item.ItemSettings.IntervalUnits);
                if (timeRemaining < 0.0)
                {   // Time has expired for this reminder, set the elapsed time background red and display the warning message
                    row.Cells[m_reminderDataGridView.Columns["ElapsedTime"].Index].Style.BackColor = Color.Red;
                    expiredItems.Add(item.ItemSettings.Name);
                }
                row.Cells[m_reminderDataGridView.Columns["TimeRemaining"].Index].Value = timeRemaining.ToString("F2") + "  " + DisplayLocalizer.GetLocalizedString(item.ItemSettings.IntervalUnits);
                m_reminderDataGridView.Rows.Add(row);
            }
            // Display the expired items dialog
            if (expiredItems.Count > 0)
            {
                MaintenanceDueDlg.DisplayExpiredMaintenanceItems(expiredItems);
            }
        }

        /// <summary>
        /// Load the reminder items from file.
        /// </summary>
        private void LoadReminders()
        {   // Open the reminders file
            Boolean fileUpdateRequired = false;
            try
            {
                XmlDocument reminderDoc = new XmlDocument();
                reminderDoc.Load(MaintenanceReminder.Properties.Settings.Default.MaintenanceSettingsFile);
                foreach (XmlNode reminderItem in reminderDoc.DocumentElement.ChildNodes)
                {   // Create a new reminder
                    ReminderSettings settings = new ReminderSettings();
                    settings.CurrentElapsedTime = Convert.ToDouble(reminderItem.FirstChild.Value);
                    settings.FaultMessage = reminderItem.Attributes.GetNamedItem("FaultText").Value;
                    settings.Interval = Convert.ToInt32(reminderItem.Attributes.GetNamedItem("MaintenanceInterval").Value);
                    settings.IntervalUnits = reminderItem.Attributes.GetNamedItem("IntervalUnits").Value;
                    try
                    {
                        settings.IntervalStartDate = Convert.ToDateTime(reminderItem.Attributes.GetNamedItem("IntervalStartDate").Value);
                    }
                    catch (NullReferenceException)
                    {
                        settings.IntervalStartDate = DateTime.Now;
                        fileUpdateRequired = true;
                    }
                    settings.WorkInstructionDocument = reminderItem.Attributes.GetNamedItem("WorkInstructionDocument").Value;
                    XmlNode notes = reminderItem.SelectSingleNode("Notes");
                    if ((notes != null) && (notes.FirstChild != null))
                    {
                        settings.MaintenanceNotes = notes.FirstChild.Value;
                    }
                    settings.Name = reminderItem.Name;
                    Reminders.Add(new MaintenanceReminderItem(settings));
                }
                // Display the reminders.
                DisplayReminders();
            }
            catch (IOException excpt)
            {
                Logger.Log("IOException opening maintenance file during UpdateElapsedTime() - " + excpt.Message);
                Logger.Log("- Exception Details: " + Environment.NewLine + excpt.StackTrace);
            }
            catch (Exception excpt)
            {
                Logger.Log("Exception opening maintenance file during UpdateElapsedTime() - " + excpt.Message);
                Logger.Log("- Exception Details: " + Environment.NewLine + excpt.StackTrace);
            }
            if (fileUpdateRequired)
            {
                MaintenanceReminderFile.SaveReminders(Reminders, Logger);
            }
        }

        /// <summary>
        /// Update the strings on the form to the selected localized language.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            String title;
            if (!System.Deployment.Application.ApplicationDeployment.IsNetworkDeployed)
            {
                title = String.Format("Maintenance Reminder - Version {0}", ProductVersion);
            }
            else
            {
                title = String.Format("Maintenance Reminder - Version {0}",
                                      System.Deployment.Application.ApplicationDeployment.CurrentDeployment.CurrentVersion.ToString());
            }
            Text = title;
            fileToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("FileMenu");
            exitToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("ExitOption");
            editToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("ConfigureMenu");
            languageToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("LanguageLabel");
            logFileLocationToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("LogFileLocationLabel");
            maintenanceItemsToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("MaintenanceItemsLabel");
            viewToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("ViewMenu");
            currentLogFileToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("LogFileLabel");
            previousToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("PreviousLabel");
            currentToolStripMenuItem.Text = DisplayLocalizer.GetLocalizedString("CurrentLabel");
            m_reminderDataGridView.Columns["ReminderName"].HeaderText = DisplayLocalizer.GetLocalizedString("ReminderNameLabel");
            m_reminderDataGridView.Columns["Interval"].HeaderText = DisplayLocalizer.GetLocalizedString("IntervalLabel");
            m_reminderDataGridView.Columns["ElapsedTime"].HeaderText = DisplayLocalizer.GetLocalizedString("ElapsedTimeLabel");
            m_reminderDataGridView.Columns["TimeRemaining"].HeaderText = DisplayLocalizer.GetLocalizedString("TimeRemainingLabel");
            m_reminderDataGridView.Columns["ViewNotes"].HeaderText = DisplayLocalizer.GetLocalizedString("ViewNotesLabel");
        }

        /// <summary>
        /// Update the elapsed time for each maintenance item.
        /// The data will be read from the file and updated.
        /// </summary>
        private void UpdateElapsedTime()
        {   // Load the XML document
            try
            {
                XmlDocument reminderDoc = new XmlDocument();
                reminderDoc.Load(MaintenanceReminder.Properties.Settings.Default.MaintenanceSettingsFile);
                // Get the elapsed times for each item
                foreach (XmlNode reminderNode in reminderDoc.DocumentElement.ChildNodes)
                {   // Find the reminder item
                    MaintenanceReminderItem reminderItem = Reminders.Find(reminder => reminder.ItemSettings.Name.Equals(reminderNode.Name));
                    if (reminderItem != null)
                    {   // Update elapsed time for maintenance items that are based on machine run time
                        if (reminderItem.ItemSettings.IntervalUnits == "Hours")
                        {
                            reminderItem.ItemSettings.CurrentElapsedTime = Convert.ToDouble(reminderNode.FirstChild.Value);
                        }
                        // Update elapsed time for maintenance items that are based on calendar time
                        else
                        {   // Determine the expiration time
                            switch (reminderItem.ItemSettings.IntervalUnits)
                            {
                                case "Days":
                                    reminderItem.ItemSettings.CurrentElapsedTime = (DateTime.Now - reminderItem.ItemSettings.IntervalStartDate).TotalDays;
                                    break;

                                case "Weeks":
                                    reminderItem.ItemSettings.CurrentElapsedTime = (DateTime.Now - reminderItem.ItemSettings.IntervalStartDate).TotalDays / 7.0;
                                    break;

                                case "Months":
                                    // Note: there are 8760 hours in a year which averages out to 730 hours in a month
                                    reminderItem.ItemSettings.CurrentElapsedTime = (DateTime.Now - reminderItem.ItemSettings.IntervalStartDate).TotalHours / 730.0;
                                    break;

                                case "Years":
                                    reminderItem.ItemSettings.CurrentElapsedTime = (DateTime.Now - reminderItem.ItemSettings.IntervalStartDate).TotalDays / 365.0;
                                    break;

                                default:
                                    Logger.Log("Unexpected interval unit: " + reminderItem.ItemSettings.IntervalUnits + " - Not updating elapsed time");
                                    break;
                            }
                        }
                    }
                }
                DisplayReminders();
            }
            catch (IOException excpt)
            {
                Logger.Log("IOException opening maintenance file during UpdateElapsedTime() - " + excpt.Message);
                Logger.Log("- Exception Details: " + Environment.NewLine + excpt.StackTrace);
            }
            catch (Exception excpt)
            {
                Logger.Log("Exception opening maintenance file during UpdateElapsedTime() - " + excpt.Message);
                Logger.Log("- Exception Details: " + Environment.NewLine + excpt.StackTrace);
            }
        }




        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// Get/Set the localizer to use for displaying the forms with the local language.
        /// </summary>
        private Localizer DisplayLocalizer { get; set; }

        /// <summary>
        /// Expired Reminders dialog.
        /// </summary>
        private ExpiredReminderForm MaintenanceDueDlg { get; set; }

        /// <summary>
        /// Logging object so we can find out what is going on.
        /// </summary>
        private CcrtLogger Logger { get; set; }

        /// <summary>
        /// List of configured reminders.
        /// </summary>
        private List<MaintenanceReminderItem> Reminders { get; set; }




        //=========================================================================================
        // Callbacks, Events and Delegates
        //=========================================================================================

        private void changePasswordToolStripMenuItem_Click(object sender, EventArgs e)
        {
            UserLogin login = new UserLogin(DisplayLocalizer);
            login.ChangePasswordSetup();
            if (login.ShowDialog() == DialogResult.OK)
            {   // Save the new password to file
                login.SaveNewPassword();
            }
        }

        /// <summary>
        /// Display the current log file for the user.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void currentToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Logger.ViewCurrentLogFile();
        }
      
        /// <summary>
        /// Exit the application
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MaintenanceDueDlg.TopMost = false;
            String exitMsg1 = DisplayLocalizer.GetLocalizedString("ExitNotificationMsg1");
            String exitMsg2 = DisplayLocalizer.GetLocalizedString("ExitNotificationMsg2");
            String confirmExitMsg = DisplayLocalizer.GetLocalizedString("ConfirmAppExitMsg");
            String confirmExitTitle = DisplayLocalizer.GetLocalizedString("ConfirmAppExitTitle");
            DialogResult exitConfirm = MessageBox.Show(exitMsg1 + Environment.NewLine + exitMsg2 +
                                                       Environment.NewLine + confirmExitMsg,
                                                       confirmExitTitle, MessageBoxButtons.YesNo, 
                                                       MessageBoxIcon.Question);
            if (exitConfirm == DialogResult.Yes)
            {
                Environment.Exit(0);
            }
            MaintenanceDueDlg.TopMost = true;
        }

        /// <summary>
        /// Display the form to allow the user to set the localization form.
        /// After the form is closed, the text on the form will be updated to the new localized text.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void languageToolStripMenuItem_Click(object sender, EventArgs e)
        {   // Have the localizer handle updating the display language
            DisplayLocalizer.SwitchLocale();
            UpdateDisplayLanguage();
        }

        /// <summary>
        /// Display the form to allow the user to change the location of the log file.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void logFileLocationToolStripMenuItem_Click(object sender, EventArgs e)
        {
            String logPath = Logger.SelectLogFilePath();
            if ((logPath.Length > 0) && (logPath != MaintenanceReminder.Properties.Settings.Default.LogFilePath))
            {   // Log file path has changed, save the updated location
                MaintenanceReminder.Properties.Settings.Default.LogFilePath = logPath;
                MaintenanceReminder.Properties.Settings.Default.Save();
            }
        }

        /// <summary>
        /// Display the maintenace item tool window so the user can add, remove and edit maintenance
        /// items.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void maintenanceItemsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ReminderConfigurationForm frm = new ReminderConfigurationForm(Reminders, Logger, DisplayLocalizer);
            frm.ShowDialog();
            DisplayReminders();
            MaintenanceReminderFile.SaveReminders(Reminders, Logger);
        }

        /// <summary>
        /// Update the elapsed time in the display for each reminder item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_elapsedTimeTimer_Tick(object sender, EventArgs e)
        {
            UpdateElapsedTime();
        }

        /// <summary>
        /// Handle the cell clicks.
        /// The only cell click we really care about is for displaying the notes.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_reminderDataGridView_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            if ((e.RowIndex >= 0) && (e.ColumnIndex == m_reminderDataGridView.Columns["ViewNotes"].Index))
            {   // Get the reminder item for the selected maintenance item
                String reminderItem = m_reminderDataGridView.Rows[e.RowIndex].Cells["ReminderName"].Value.ToString().Replace(" ", "_");
                MaintenanceReminderItem item = Reminders.Find(reminder => reminder.ItemSettings.Name.Equals(reminderItem));
                if (item != null)
                {   // Display the form with the notes of the seleted maintenance item
                    ViewNotesForm frm = new ViewNotesForm(item, Logger, DisplayLocalizer);
                    frm.Text = item.ItemSettings.Name.Replace("_", " ") + " " + DisplayLocalizer.GetLocalizedString("NotesTitle");
                    frm.ShowDialog();
                    MaintenanceReminderFile.SaveReminders(Reminders, Logger);
                }
            }
            else if ((e.RowIndex >= 0) && (e.ColumnIndex == m_reminderDataGridView.Columns["ViewWorkInstruction"].Index))
            {
                String reminderItem = m_reminderDataGridView.Rows[e.RowIndex].Cells["ReminderName"].Value.ToString().Replace(" ", "_");
                MaintenanceReminderItem item = Reminders.Find(reminder => reminder.ItemSettings.Name.Equals(reminderItem));
                if (item != null)
                {   // Display the work instruction
                    item.ViewWorkInstruction();
                }
            }
        }

        /// <summary>
        /// Display a prvious log file.
        /// The user will be presented with a list of previous log files to choose from.  The 
        /// selected log file will then be displayed.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void previousToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Logger.ViewPreviousLogFile();
        }

        /// <summary>
        /// Display the IP Address form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void qNXIPAddressToolStripMenuItem_Click(object sender, EventArgs e)
        {
            IpAddressForm frm = new IpAddressForm(DisplayLocalizer);
            frm.ShowDialog();
        }

    }
}
