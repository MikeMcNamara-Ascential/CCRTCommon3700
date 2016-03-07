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
    public partial class ViewNotesForm : Form
    {
        //=========================================================================================
        // Member Functions
        //=========================================================================================

        /// <summary>
        /// Create a new note viewer form and display the notes from the provided reminder.
        /// </summary>
        /// <param name="reminder">Maintenance reminder item to show the notes of.</param>
        /// <param name="logger">Logging object to use for tracking activities.</param>
        /// <param name="displayLocalizer">Localizer to use for displaying data in the selected language.</param>
        public ViewNotesForm(MaintenanceReminderItem reminder, CcrtLogger logger, Localizer displayLocalizer)
        {
            InitializeComponent();
            Reminder = reminder;
            Logger = logger;
            DisplayLocalizer = displayLocalizer;
            UpdateDisplayLanguage();
            DisplayNotes();
        }

        /// <summary>
        /// Display the notes that have been entered for this reminder item.
        /// </summary>
        private void DisplayNotes()
        {
            m_notesTextBox.Text = Reminder.ItemSettings.MaintenanceNotes;
        }

        /// <summary>
        /// Update the labels on the form to the selected display language.
        /// </summary>
        private void UpdateDisplayLanguage()
        {
            Text = DisplayLocalizer.GetLocalizedString("NotesTitle");
            m_addNoteButton.Text = DisplayLocalizer.GetLocalizedString("AddNoteLabel");
            m_closeButton.Text = DisplayLocalizer.GetLocalizedString("CloseLabel");
        }




        //=========================================================================================
        // Member Variables and Properties
        //=========================================================================================

        /// <summary>
        /// Get/Set the localizer to use for displaying the forms with the local language.
        /// </summary>
        private Localizer DisplayLocalizer { get; set; }

        /// <summary>
        /// Logging object for tracking activities.
        /// </summary>
        private CcrtLogger Logger { get; set; }

        /// <summary>
        /// Get/Set the reminder being handled by this window.
        /// </summary>
        private MaintenanceReminderItem Reminder { get; set; }


        //=========================================================================================
        // Callbacks, Events and Delegates
        //=========================================================================================

        /// <summary>
        /// Allow the user to add a note from this form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_addNoteButton_Click(object sender, EventArgs e)
        {
            AddMaintenanceNoteForm frm = new AddMaintenanceNoteForm(Reminder, Logger, DisplayLocalizer);
            frm.ShowDialog();
            DisplayNotes();
        }

        /// <summary>
        /// Close the form.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void m_closeButton_Click(object sender, EventArgs e)
        {
            Close();
        }

    }
}
