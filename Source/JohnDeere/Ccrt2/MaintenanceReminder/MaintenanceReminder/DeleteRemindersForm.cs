using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using Logger;
using MaintenanceReminderLibrary;

namespace MaintenanceReminder
{
    public partial class DeleteRemindersForm : Form
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        public DeleteRemindersForm(List<MaintenanceReminderItem> reminders, 
                                   CcrtLogger logger, Localizer displayLocalizer)
        {
            InitializeComponent();
            Reminders = reminders;
            Logger = logger;
            DisplayLocalizer = displayLocalizer;
            UpdateDisplayLanguage();
            DisplayReminders();

        }

        /// <summary>
        /// Delete the selected reminders.
        /// </summary>
        private void DeleteReminders()
        {   // Verify the user really wants to delete the reminders before actually removing them
            String deleteList = "";   // Need to initialize string with something or get error
            foreach (Int32 index in m_reminderCheckedListBox.CheckedIndices)
            {
                deleteList += (Reminders[index].ItemSettings.Name + Environment.NewLine);
            }
            String confirmDeleteMsg = DisplayLocalizer.GetLocalizedString("ConfirmDeleteMsg");
            String confirmDeleteTitle = DisplayLocalizer.GetLocalizedString("ConfirmDeleteTitle");
            DialogResult result = MessageBox.Show(confirmDeleteMsg + Environment.NewLine + deleteList, 
                                                  confirmDeleteTitle,
                                                   MessageBoxButtons.OKCancel, MessageBoxIcon.Question);
            if (result == DialogResult.OK)
            {
                List<String> itemsToDelete = new List<String>();
                foreach (String item in m_reminderCheckedListBox.CheckedItems)
                {
                    itemsToDelete.Add(item);
                }
                MaintenanceReminderFile.DeleteReminder(itemsToDelete, Reminders, Logger);
            }
            // Redisplay the remaining reminders
            DisplayReminders();
        }

        /// <summary>
        /// Display the list of configured reminders.
        /// </summary>
        private void DisplayReminders()
        {   // Clear the reminder list first so we do not get duplicate listings
            m_reminderCheckedListBox.Items.Clear();
            foreach (MaintenanceReminderItem item in Reminders)
            {
                m_reminderCheckedListBox.Items.Add(item.ItemSettings.Name);
            }
        }

        /// <summary>
        /// Update the form data to the selected display language.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            Text = DisplayLocalizer.GetLocalizedString("DeleteReminderTitle");
            m_closeButton.Text = DisplayLocalizer.GetLocalizedString("CloseLabel");
            m_deleteButton.Text = DisplayLocalizer.GetLocalizedString("DeleteLabel");
            m_reminderLabel.Text = DisplayLocalizer.GetLocalizedString("ReminderListLabel");
        }



        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// Get/Set the localizer to use for displaying the forms with the local language.
        /// </summary>
        private Localizer DisplayLocalizer { get; set; }

        /// <summary>
        /// Logger object for logging activities.
        /// </summary>
        private CcrtLogger Logger { get; set; }

        /// <summary>
        /// List of currently configured maintenance reminders.
        /// </summary>
        private List<MaintenanceReminderItem> Reminders { get; set; }



        //=========================================================================================
        // Callbacks, Events and Delegates
        //=========================================================================================

        /// <summary>
        /// Delete the selected reminders after confirming the action
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_deleteButton_Click(object sender, EventArgs e)
        {
            if (m_reminderCheckedListBox.CheckedIndices.Count > 0)
            {
                DeleteReminders();
            }
            else
            {
                String noRemindersMsg = DisplayLocalizer.GetLocalizedString("NoRemindersSelectedMsg");
                String notDeletingMsg = DisplayLocalizer.GetLocalizedString("NotDeletingMsg");
                String noRemindersTitle = DisplayLocalizer.GetLocalizedString("NoRemindersSelectedTitle");
                MessageBox.Show(noRemindersMsg + Environment.NewLine + notDeletingMsg,
                                noRemindersTitle, MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }
    }
}
