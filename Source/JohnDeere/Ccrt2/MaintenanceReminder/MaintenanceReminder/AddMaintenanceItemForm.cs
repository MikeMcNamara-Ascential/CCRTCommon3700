using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Logger;
using MaintenanceReminderLibrary;

namespace MaintenanceReminder
{
    public partial class AddMaintenanceItemForm : Form
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        /// <summary>
        /// Create a new maintenance reminder add/edit form.
        /// </summary>
        /// <param name="updateType">Type of update being performed: Add or Edit</param>
        /// <param name="reminders">List of configured reminders.</param>
        /// <param name="logger">Logger object to use for logging activites.</param>
        /// <param name="displayLocalizer">Localizer to use for displaying the current selected language.</param>
        public AddMaintenanceItemForm(String updateType, 
                                      List<MaintenanceReminderItem> reminders,
                                      CcrtLogger logger,
                                      Localizer displaylocalizer)
        {
            InitializeComponent();
            Logger = logger;
            DisplayLocalizer = displaylocalizer;
            // Set the update type
            UpdateType = updateType;
            m_addButton.Text = UpdateType;
            // Save the list of reminders locally
            Reminders = reminders;
            UpdateDisplayLanguage();
        }

        /// <summary>
        /// Create a new maintenance reminder add/edit form.
        /// </summary>
        /// <param name="updateType">Type of update being performed: Add or Edit</param>
        /// <param name="reminders">List of configured reminders.</param>
        /// <param name="logger">Logger object to use for logging activites.</param>
        /// <param name="displayLocalizer">Localizer to use for displaying the current selected language.</param>
        /// <param name="reminderIndex">Index of the selected reminder to edit.</param>
        public AddMaintenanceItemForm(String updateType, 
                                      List<MaintenanceReminderItem> reminders, 
                                      CcrtLogger logger,
                                      Localizer displayLocalizer,
                                      Int32 reminderIndex) : 
            this(updateType, reminders, logger, displayLocalizer)
        {
            // Display the selected item
            if (reminderIndex >= 0)
            {
                SelectedReminder = Reminders[reminderIndex];
                DisplayReminder(SelectedReminder);
            }
            else
            {
                updateType = "Add";
            }
            // Set the update type
            UpdateType = updateType;
            m_addButton.Text = DisplayLocalizer.GetLocalizedString(UpdateType);
        }

        /// <summary>
        /// Clear the form entries.
        /// </summary>
        private void ClearForm()
        {
            m_maintenanceItemTextBox.Clear();
            m_intervalTextBox.Clear();
            m_intervalUnitsComboBox.SelectedIndex = 0;
            m_faultMessageTextBox.Clear();
            m_workInstructionTextBox.Clear();
        }

        /// <summary>
        /// Create a new reminder item using the data entered into the form.
        /// </summary>
        private ReminderSettings CreateNewReminder()
        {   // Create the new reminder
            ReminderSettings settings = new ReminderSettings();
            ReminderSettings returnSettings = null;
            // Make sure a name was suplied
            if (m_maintenanceItemTextBox.Text.Length > 0)
            {   
                settings.Name = m_maintenanceItemTextBox.Text.Replace(" ", "_");
                // Make sure a valid interval was supplied
                try
                {
                    Int32 interval = Convert.ToInt32(m_intervalTextBox.Text);
                    if (interval < 0)
                    {
                        interval = -interval;
                    }
                    else if (interval == 0)
                    {
                        interval = 100;
                    }
                    settings.Interval = interval;
                    // Save the interval units
                    settings.IntervalUnits = IntervalUnits.GetUnits(m_intervalUnitsComboBox.SelectedIndex);
                    settings.IntervalStartDate = DateTime.Now;
                    // Save the display message
                    settings.FaultMessage = m_faultMessageTextBox.Text;
                    // Save the work instruction document
                    settings.WorkInstructionDocument = m_workInstructionTextBox.Text;
                    returnSettings = settings;
                    Logger.Log("Created new reminder: " + settings.Name);
                }
                catch (FormatException excpt)
                {
                    String invalidIntervalMsg = DisplayLocalizer.GetLocalizedString("InvlaidIntervalMessage");
                    String enterIntValMsg = DisplayLocalizer.GetLocalizedString("EnterIntegerMessage");
                    String invlaidFormatTitle = DisplayLocalizer.GetLocalizedString("InvalidDataFormatTitle");
                    MessageBox.Show(invalidIntervalMsg + Environment.NewLine + enterIntValMsg + Environment.NewLine + excpt.Message,
                                    invalidIntervalMsg, MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {   // Give the user a warning
                String itemNotBlankMsg = DisplayLocalizer.GetLocalizedString("ItemNotBlankMsg");
                String blankFieldTitle = DisplayLocalizer.GetLocalizedString("BlankFieldTitle");
                MessageBox.Show(itemNotBlankMsg, blankFieldTitle, MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
            return returnSettings;
        }

        /// <summary>
        /// Display the specified maintenance reminder item.
        /// </summary>
        /// <param name="item">Reiner item to display.</param>
        private void DisplayReminder(MaintenanceReminderItem item)
        {
            m_maintenanceItemTextBox.Text = item.ItemSettings.Name;
            m_intervalTextBox.Text = item.ItemSettings.Interval.ToString();
            m_intervalUnitsComboBox.SelectedIndex = m_intervalUnitsComboBox.FindString(DisplayLocalizer.GetLocalizedString(item.ItemSettings.IntervalUnits));
            m_faultMessageTextBox.Text = item.ItemSettings.FaultMessage;
            m_workInstructionTextBox.Text = item.ItemSettings.WorkInstructionDocument;
        }

        /// <summary>
        /// Display the current selected language in the form.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            Text = DisplayLocalizer.GetLocalizedString(UpdateType + "MaintenanceReminderTitle");
            m_maintenanceItemLabel.Text = DisplayLocalizer.GetLocalizedString("ReminderNameLabel");
            m_maintenanceIntervalLabel.Text = DisplayLocalizer.GetLocalizedString("IntervalLabel");
            m_faultMessageLabel.Text = DisplayLocalizer.GetLocalizedString("MaintenanceMessageLabel");
            m_addButton.Text = DisplayLocalizer.GetLocalizedString("AddLabel");
            m_closeButton.Text = DisplayLocalizer.GetLocalizedString("CloseLabel");
            m_workInstructionLabel.Text = DisplayLocalizer.GetLocalizedString("WorkInstructionDocument");
            m_wiBrowseButton.Text = DisplayLocalizer.GetLocalizedString("Browse");
            m_intervalUnitsComboBox.Items.Clear();
            m_intervalUnitsComboBox.Items.AddRange(IntervalUnits.GetDisplayUnits(DisplayLocalizer).ToArray());
            // Select the default maintenance interval units
            m_intervalUnitsComboBox.SelectedIndex = 0;
        }



        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// Get/Set the localizer to use for displaying the forms with the local language.
        /// </summary>
        private Localizer DisplayLocalizer { get; set; }

        /// <summary>
        /// Logger object for logging activites.
        /// </summary>
        private CcrtLogger Logger { get; set; }

        /// <summary>
        /// List of currently configured reminders.
        /// </summary>
        private List<MaintenanceReminderItem> Reminders { get; set; }

        /// <summary>
        /// Reminder item that has been selected for editing.
        /// </summary>
        private MaintenanceReminderItem SelectedReminder { get; set; }

        /// <summary>
        /// Get/Set the type of update being performed
        /// </summary>
        private String UpdateType { get; set; }




        //=========================================================================================
        // Callbacks, Events and Delegates
        //=========================================================================================

        /// <summary>
        /// Add/Update the maintenance reminder.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addButton_Click(object sender, EventArgs e)
        {   // Determine if this is add or edit
            if (UpdateType == "Add")
            {
                ReminderSettings settings = CreateNewReminder();
                MaintenanceReminderItem item = new MaintenanceReminderItem(settings);
                if (settings != null)
                {
                    if (null == Reminders.Find(delegate(MaintenanceReminderItem reminderItem)
                                              {
                                                  return reminderItem.ItemSettings.Name == settings.Name;
                                              }
                                              )
                        )
                    {
                        Reminders.Add(item);
                        String reminderAddedMsg = DisplayLocalizer.GetLocalizedString("ReminderAddedMsg");
                        String reminderAddedTitle = DisplayLocalizer.GetLocalizedString("ReminderAddedTitle");
                        MessageBox.Show(reminderAddedMsg + item.ItemSettings.Name.Replace("_", " "), reminderAddedTitle,
                                         MessageBoxButtons.OK, MessageBoxIcon.Information);
                    }
                    else
                    {
                        String reminderExistsMsg = DisplayLocalizer.GetLocalizedString("ReminderExistsMessage");
                        String reminderExistsTitle = DisplayLocalizer.GetLocalizedString("ReminderExistsTitle");
                        MessageBox.Show(settings.Name.Replace("_", " ") + Environment.NewLine + reminderExistsMsg,
                                        reminderExistsTitle, MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    ClearForm();
                }
            }
            else
            {
                SelectedReminder.ItemSettings.FaultMessage = m_faultMessageTextBox.Text;
                SelectedReminder.ItemSettings.Interval = Convert.ToInt32(m_intervalTextBox.Text);
                SelectedReminder.ItemSettings.IntervalUnits = IntervalUnits.GetUnits(m_intervalUnitsComboBox.SelectedIndex);
                SelectedReminder.ItemSettings.WorkInstructionDocument = m_workInstructionTextBox.Text;
                String updatedReminderMsg = DisplayLocalizer.GetLocalizedString("UpdatedReminderMsg");
                String updatedReminderTitle = DisplayLocalizer.GetLocalizedString("UpdatedReminderTitle");
                MessageBox.Show(updatedReminderMsg + SelectedReminder.ItemSettings.Name.Replace("_", " "),
                                updatedReminderTitle, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// Display the file chooser dialog box to allow the user to select the appropriate work instruction document.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_wiBrowseButton_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            dlg.Multiselect = false;
            dlg.RestoreDirectory = true;
            dlg.Title = DisplayLocalizer.GetLocalizedString("SelectWorkInstructionDocument");
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                m_workInstructionTextBox.Text = dlg.FileName;
            }
        }

    }
}
