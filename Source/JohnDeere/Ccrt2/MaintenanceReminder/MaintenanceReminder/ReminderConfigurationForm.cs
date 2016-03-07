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
    public partial class ReminderConfigurationForm : Form
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        public ReminderConfigurationForm(List<MaintenanceReminderItem> reminders, 
                                         CcrtLogger logger,
                                         Localizer displayLocalizer)
        {
            InitializeComponent();
            Logger = logger;
            DisplayLocalizer = displayLocalizer;
            // Save the reminders
            Reminders = reminders;
            PopulateMaintenanceReminders();
            UpdateDisplayLanguage();
        }

        /// <summary>
        /// Clear the form of any residue
        /// </summary>
        private void ClearForm()
        {
            m_intervalText.Text = "";
            m_intervalUnitsText.Text = "";
            m_elapsedTimeText.Text = "";
            m_elapsedTimeUnitsText.Text = "";
            m_faultMessageTextBox.Clear();
            m_maintenanceNotesTextBox.Clear();
        }

        /// <summary>
        /// Display the selected maintenance reminder item in the form.
        /// </summary>
        /// <param name="item">Item to be displayed in the form.</param>
        private void DisplayReminderItem(MaintenanceReminderItem item)
        {
            m_intervalText.Text = item.ItemSettings.Interval.ToString();
            m_intervalUnitsText.Text = item.ItemSettings.IntervalUnits;
            m_elapsedTimeText.Text = item.ItemSettings.CurrentElapsedTime.ToString();
            m_elapsedTimeUnitsText.Text = item.ItemSettings.IntervalUnits;
            m_faultMessageTextBox.Text = item.ItemSettings.FaultMessage;
            m_maintenanceNotesTextBox.Text = item.ItemSettings.MaintenanceNotes;
        }

        /// <summary>
        /// Add the maintenance reminders to the form.
        /// </summary>
        private void PopulateMaintenanceReminders(Int32 displayIndex = 0)
        {
            ClearForm();
            m_maintenanceItemsComboBox.Items.Clear();
            foreach (MaintenanceReminderItem item in Reminders)
            {   // Add the name to the combo box
                m_maintenanceItemsComboBox.Items.Add(item.ItemSettings.Name.Replace("_", " "));
            }
            // Select the first reminder
            if ((Reminders.Count > 0) && (displayIndex < Reminders.Count))
            {
                m_maintenanceItemsComboBox.SelectedIndex = displayIndex;
            }
        }

        /// <summary>
        /// Set the items on the display to the correct display language.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            Text = DisplayLocalizer.GetLocalizedString("ReminderConfigurationTitle");
            m_maintenanceItemLabel.Text = DisplayLocalizer.GetLocalizedString("MaintenanceItemsLabel");
            m_maintenanceIntervalLabel.Text = DisplayLocalizer.GetLocalizedString("IntervalLabel");
            m_currentElapsedTimeLabel.Text = DisplayLocalizer.GetLocalizedString("ElapsedTimeLabel");
            m_faultMessageLabel.Text = DisplayLocalizer.GetLocalizedString("MaintenanceMessageLabel");
            m_maintenanceNotesLabel.Text = DisplayLocalizer.GetLocalizedString("MaintenanceNotesLabel");
            m_addNoteButton.Text = DisplayLocalizer.GetLocalizedString("AddNoteLabel");
            m_closeButton.Text = DisplayLocalizer.GetLocalizedString("CloseLabel");
            m_addButton.Text = DisplayLocalizer.GetLocalizedString("AddReminderLabel");
            m_editButton.Text = DisplayLocalizer.GetLocalizedString("EditReminderLabel");
            m_deleteButton.Text = DisplayLocalizer.GetLocalizedString("DeleteReminderLabel");
            if (m_maintenanceItemsComboBox.SelectedIndex >= 0)
            {
                m_elapsedTimeUnitsText.Text = DisplayLocalizer.GetLocalizedString(Reminders[m_maintenanceItemsComboBox.SelectedIndex].ItemSettings.IntervalUnits);
                m_intervalUnitsText.Text = DisplayLocalizer.GetLocalizedString(Reminders[m_maintenanceItemsComboBox.SelectedIndex].ItemSettings.IntervalUnits);
            }
            else
            {
                m_elapsedTimeUnitsText.Text = DisplayLocalizer.GetLocalizedString("Hours");
                m_intervalUnitsText.Text = DisplayLocalizer.GetLocalizedString("Hours");
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
        /// Logger object for tracking actions and events.
        /// </summary>
        private CcrtLogger Logger { get; set; }

        /// <summary>
        /// List of currently configured maintence reminders.
        /// </summary>
        private List<MaintenanceReminderItem> Reminders { get; set; }


        //=========================================================================================
        // Callbacks, Events and Delegates
        //=========================================================================================

        /// <summary>
        /// Display the add reminder form so the user can add a new reminder.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addButton_Click(object sender, EventArgs e)
        {
            AddMaintenanceItemForm frm = new AddMaintenanceItemForm("Add", Reminders, Logger, DisplayLocalizer);
            frm.ShowDialog();
            PopulateMaintenanceReminders();
            if (m_maintenanceItemsComboBox.SelectedIndex >= 0) m_addNoteButton.Visible = true;
        }

        /// <summary>
        /// Display the form to allow the user to add a note to the reminder item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addNoteButton_Click(object sender, EventArgs e)
        {
            Int32 currentIndex = m_maintenanceItemsComboBox.SelectedIndex;
            if (currentIndex >= 0)
            {
                AddMaintenanceNoteForm frm = new AddMaintenanceNoteForm(Reminders[currentIndex],
                                                                        Logger, DisplayLocalizer);
                frm.ShowDialog();
                PopulateMaintenanceReminders(currentIndex);
            }
        }

        /// <summary>
        /// Display the reminder delete form so the user can remove reminders
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_deleteButton_Click(object sender, EventArgs e)
        {
            DeleteRemindersForm frm = new DeleteRemindersForm(Reminders, Logger, DisplayLocalizer);
            frm.ShowDialog();
            PopulateMaintenanceReminders();
            if (m_maintenanceItemsComboBox.SelectedIndex < 0) m_addNoteButton.Visible = false;
        }

        /// <summary>
        /// Display the add reminder form so the user can edit the currently selected reminder.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_editButton_Click(object sender, EventArgs e)
        {
            AddMaintenanceItemForm frm = new AddMaintenanceItemForm("Edit", Reminders, Logger, DisplayLocalizer,
                                                                    m_maintenanceItemsComboBox.SelectedIndex);
            frm.ShowDialog();
            PopulateMaintenanceReminders();
            if (m_maintenanceItemsComboBox.SelectedIndex >= 0) m_addNoteButton.Visible = true;
        }

        /// <summary>
        /// Display the newly selected item.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_maintenanceItemsComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            DisplayReminderItem(Reminders[m_maintenanceItemsComboBox.SelectedIndex]);
        }

    }
}
