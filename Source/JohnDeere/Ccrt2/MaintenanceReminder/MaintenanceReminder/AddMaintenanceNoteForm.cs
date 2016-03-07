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
    public partial class AddMaintenanceNoteForm : Form
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        /// <summary>
        /// Create a new form for entering a note for the provided maintenance reminder item.
        /// </summary>
        /// <param name="reminderItem">REminder item to add the note to.</param>
        /// <param name="logger">Logger object to record activities.</param>
        /// <param name="displayLocalizer">Localizer to use for displaying data in the local language.</param>
        public AddMaintenanceNoteForm(MaintenanceReminderItem reminderItem, 
                                      CcrtLogger logger,
                                      Localizer displayLocalizer)
        {
            InitializeComponent();
            Reminder = reminderItem;
            Logger = logger;
            DisplayLocalizer = displayLocalizer;
            UpdateDisplayLanguage();
        }

        /// <summary>
        /// Add the note to the reminder.
        /// </summary>
        private Boolean AddNote()
        {   // Make sure the user entered their name before adding the note
            Boolean noteAdded = false;
            if (m_nameTextBox.Text.Length > 0)
            {   // Create the entry
                String entry = DateTime.Now + "\t" + m_nameTextBox.Text + "\t" + m_noteTextBox.Text + Environment.NewLine;
                Reminder.ItemSettings.MaintenanceNotes += entry;
                noteAdded = true;
                Logger.Log(m_nameTextBox.Text + " added a note to " + Reminder.ItemSettings.Name);
            }
            else
            {   // Inform the user they must enter a name
                String nameReqMsg = DisplayLocalizer.GetLocalizedString("NameRequiredMsg");
                String nameReqTitle = DisplayLocalizer.GetLocalizedString("NameRequiredTitle");
                MessageBox.Show(nameReqMsg, nameReqTitle, MessageBoxButtons.OKCancel,
                                MessageBoxIcon.Stop);
            }
            return noteAdded;
        }

        /// <summary>
        /// Update form labels with the selected display language.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            Text = DisplayLocalizer.GetLocalizedString("AddMaintenanceNoteTitle");
            m_nameLabel.Text = DisplayLocalizer.GetLocalizedString("NameLabel");
            m_noteLabel.Text = DisplayLocalizer.GetLocalizedString("NoteLabel");
            m_saveButton.Text = DisplayLocalizer.GetLocalizedString("SaveLabel");
            m_cancelButton.Text = DisplayLocalizer.GetLocalizedString("CancelLabel");
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
        /// Maintenance Reminder item being operated on.
        /// </summary>
        MaintenanceReminderItem Reminder { get; set; }



        //=========================================================================================
        // Callbacks, Events and Delegates
        //=========================================================================================

        /// <summary>
        /// Save the note and close the form if the note saved successfully.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_saveButton_Click(object sender, EventArgs e)
        {
            if (AddNote())
            {
                Close();
            }
        }
    }
}
